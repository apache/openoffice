/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */


// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_bridges.hxx"

// This is an implementation of the parameter-classification rules of the
// AArch64 procedure call standard ("Procedure Call Standard for the Arm 64-bit
// Architecture", ARM IHI 0055).
//
// Unlike the System V AMD64 ABI (used by the x86-64 bridge), AAPCS64 does not
// split aggregates into per-eightbyte INTEGER/SSE classes.  Instead:
//   * scalars go in one GPR (x) or one FP/SIMD (v) register;
//   * a Homogeneous Floating-point Aggregate (HFA: <= 4 members, all the same
//     FP type, recursively) goes in consecutive v registers;
//   * any other aggregate <= 16 bytes goes in 1-2 GPRs;
//   * a non-HFA aggregate > 16 bytes is passed indirectly (a pointer to a
//     caller-allocated copy).
// Register fill is "all or nothing": if an aggregate does not fit entirely in
// the remaining registers of its bank, it is passed wholly on the stack.
//
// This is a clean-room implementation from the public specifications; see
// ../../../../AAPCS64_BRIDGE_SPEC.md.

#include "abi.hxx"

#include "bridges/cpp_uno/shared/types.hxx"

#include <rtl/ustring.hxx>
#include <string.h>

using namespace aarch64;

namespace {

// The element type of a Homogeneous Floating-point Aggregate.
enum HfaKind
{
	HFA_NONE,       // not (yet) an HFA
	HFA_FLOAT,      // all members are FLOAT (4-byte)
	HFA_DOUBLE      // all members are DOUBLE (8-byte)
};

// Combine the running HFA kind with a newly-seen member kind.  Two members
// of different FP types, or any non-FP member, break the homogeneity.
HfaKind mergeHfa( HfaKind running, HfaKind seen )
{
	if ( seen == HFA_NONE )
		return HFA_NONE;
	if ( running == HFA_NONE )
		return seen;
	return ( running == seen ) ? running : HFA_NONE;
}

bool isComplexAggregate( typelib_TypeDescriptionReference *pTypeRef )
{
	typelib_TypeDescription * pTypeDescr = 0;
	TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );
	const typelib_CompoundTypeDescription *pComp =
		reinterpret_cast<const typelib_CompoundTypeDescription *>( pTypeDescr );
	bool complex = pComp->pBaseTypeDescription != 0 &&
		isComplexAggregate( pComp->pBaseTypeDescription->aBase.pWeakRef );
	for ( sal_Int32 i = 0; !complex && i < pComp->nMembers; ++i )
	{
		typelib_TypeClass typeClass = pComp->ppTypeRefs[i]->eTypeClass;
		if ( typeClass == typelib_TypeClass_STRUCT ||
			 typeClass == typelib_TypeClass_EXCEPTION )
			complex = isComplexAggregate( pComp->ppTypeRefs[i] );
		else
			complex = !bridges::cpp_uno::shared::isSimpleType( typeClass );
	}
	TYPELIB_DANGER_RELEASE( pTypeDescr );
	return complex;
}

// Recursively determine whether pTypeRef is (part of) a homogeneous
// floating-point aggregate, accumulating the element kind and member count.
//
// Returns false the moment homogeneity is violated (a non-FP scalar, or a
// second distinct FP type, or > 4 elements).  A FLOAT/DOUBLE scalar counts as
// a 1-element HFA of itself; a struct flattens its members (and base classes).
bool collectHfa( typelib_TypeDescriptionReference *pTypeRef, HfaKind &rKind, int &rCount )
{
	switch ( pTypeRef->eTypeClass )
	{
		case typelib_TypeClass_FLOAT:
			rKind = mergeHfa( rKind, HFA_FLOAT );
			if ( rKind == HFA_NONE ) return false;
			return ( ++rCount <= 4 );

		case typelib_TypeClass_DOUBLE:
			rKind = mergeHfa( rKind, HFA_DOUBLE );
			if ( rKind == HFA_NONE ) return false;
			return ( ++rCount <= 4 );

		case typelib_TypeClass_STRUCT:
		case typelib_TypeClass_EXCEPTION:
		{
			typelib_TypeDescription * pTypeDescr = 0;
			TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );

			const typelib_CompoundTypeDescription *pComp =
				reinterpret_cast<const typelib_CompoundTypeDescription*>( pTypeDescr );

			// rCount is cumulative over the whole recursion, so remember where
			// this aggregate started in order to size-check it below.
			const int nCountAtEntry = rCount;
			bool bOk = true;

			// Flatten base class first (its members precede ours in layout).
			if ( pComp->pBaseTypeDescription )
			{
				bOk = collectHfa(
					pComp->pBaseTypeDescription->aBase.pWeakRef, rKind, rCount );
			}

			for ( sal_Int32 i = 0; bOk && i < pComp->nMembers; ++i )
				bOk = collectHfa( pComp->ppTypeRefs[i], rKind, rCount );

			if ( bOk )
			{
				// Reject anything the elements do not tile exactly: only the
				// elements contributed by THIS aggregate count towards its size.
				sal_Int32 elementSize = rKind == HFA_FLOAT ? 4 : 8;
				bOk = pTypeDescr->nSize ==
					( rCount - nCountAtEntry ) * elementSize;
				for ( sal_Int32 i = 0; bOk && i < pComp->nMembers; ++i )
					bOk = pComp->pMemberOffsets[i] % elementSize == 0;
			}

			TYPELIB_DANGER_RELEASE( pTypeDescr );
			return bOk;
		}

		default:
			// Any non-FP, non-aggregate member breaks homogeneity.
			rKind = HFA_NONE;
			return false;
	}
}

// Classify an aggregate (STRUCT/EXCEPTION).  Sets the GPR/FPR counts and
// returns true if it is passed in registers, false if it must be passed
// indirectly (in memory).
bool classifyAggregate( typelib_TypeDescriptionReference *pTypeRef, int &nUsedGPR, int &nUsedFPR )
{
	// First, the HFA test.
	HfaKind kind = HFA_NONE;
	int count = 0;
	if ( collectHfa( pTypeRef, kind, count ) && kind != HFA_NONE )
	{
		nUsedFPR = count;
		nUsedGPR = 0;
		return true; // HFA passed in consecutive FP regs
	}

	// Not HFA: if bigger than 16 bytes, pass indirectly.
	typelib_TypeDescription * pTypeDescr = 0;
	TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );
	if ( pTypeDescr->nSize > 16 )
	{
		TYPELIB_DANGER_RELEASE( pTypeDescr );
		nUsedGPR = nUsedFPR = 0;
		return false; // indirect
	}

	// small aggregate: it occupies 1 or 2 GPRs depending on size
	nUsedFPR = 0;
	nUsedGPR = ( pTypeDescr->nSize + 7 ) / 8;
	if ( nUsedGPR < 1 ) nUsedGPR = 1;
	TYPELIB_DANGER_RELEASE( pTypeDescr );
	return true;
}

} // anonymous namespace

// Public API implementations.
namespace aarch64
{

bool examine_argument( typelib_TypeDescriptionReference *pTypeRef, bool bInReturn, int &nUsedGPR, int &nUsedFPR )
{
	nUsedGPR = 0;
	nUsedFPR = 0;

	switch ( pTypeRef->eTypeClass )
	{
		case typelib_TypeClass_VOID:
			return true;

		case typelib_TypeClass_CHAR:
		case typelib_TypeClass_BOOLEAN:
		case typelib_TypeClass_BYTE:
		case typelib_TypeClass_SHORT:
		case typelib_TypeClass_UNSIGNED_SHORT:
		case typelib_TypeClass_LONG:
		case typelib_TypeClass_UNSIGNED_LONG:
		case typelib_TypeClass_HYPER:
		case typelib_TypeClass_UNSIGNED_HYPER:
		case typelib_TypeClass_ENUM:
			nUsedGPR = 1;
			return true;

		case typelib_TypeClass_FLOAT:
		case typelib_TypeClass_DOUBLE:
			nUsedFPR = 1;
			return true;

		// These UNO types are always handled by the bridge as a pointer/
		// reference (one GPR), never passed by value through this classifier.
		case typelib_TypeClass_STRING:
		case typelib_TypeClass_TYPE:
		case typelib_TypeClass_ANY:
		case typelib_TypeClass_TYPEDEF:
		case typelib_TypeClass_SEQUENCE:
		case typelib_TypeClass_INTERFACE:
			nUsedGPR = 1;
			return true;

		case typelib_TypeClass_STRUCT:
		case typelib_TypeClass_EXCEPTION:
			if ( bInReturn )
				return classifyAggregate( pTypeRef, nUsedGPR, nUsedFPR );
			nUsedGPR = 1; // generated UNO C++ bindings pass aggregates by const reference
			return true;

		default:
			return false;
	}
}

bool return_in_hidden_param( typelib_TypeDescriptionReference *pTypeRef )
{
	switch ( pTypeRef->eTypeClass )
	{
		case typelib_TypeClass_STRING:
		case typelib_TypeClass_TYPE:
		case typelib_TypeClass_ANY:
		case typelib_TypeClass_TYPEDEF:
		case typelib_TypeClass_UNION:
		case typelib_TypeClass_ARRAY:
		case typelib_TypeClass_SEQUENCE:
		case typelib_TypeClass_INTERFACE:
			// These are C++ wrapper objects, not pointer-sized scalar values.
			// On FreeBSD aarch64 (System V), C++ class return values (non-POD
			// with non-trivial copy constructor) are returned via x8 buffer.
			return true;
		default:
			break;
	}

	if ( pTypeRef->eTypeClass == typelib_TypeClass_STRUCT ||
		 pTypeRef->eTypeClass == typelib_TypeClass_EXCEPTION )
	{
		if ( isComplexAggregate( pTypeRef ) )
			return true;
	}

	int g, s;
	// Returned in registers iff examine_argument() says it fits;
	// otherwise the caller must pass an indirect-result buffer in x8.
	return !examine_argument( pTypeRef, true, g, s );
}

sal_uInt32 get_return_kind( typelib_TypeDescriptionReference *pTypeRef )
{
	if ( pTypeRef->eTypeClass == typelib_TypeClass_STRUCT ||
		 pTypeRef->eTypeClass == typelib_TypeClass_EXCEPTION )
	{
		HfaKind kind = HFA_NONE;
		int count = 0;
		if ( collectHfa( pTypeRef, kind, count ) && count >= 1 && count <= 4 )
			return kind == HFA_FLOAT ? RETURN_KIND_HFA_FLOAT : RETURN_KIND_HFA_DOUBLE;
	}
	return pTypeRef->eTypeClass;
}

void fill_struct( typelib_TypeDescriptionReference *pTypeRef, const sal_uInt64* pGPR, const double* pFPR, void *pStruct )
{
	int nUsedGPR = 0;
	int nUsedFPR = 0;
	if ( !examine_argument( pTypeRef, true, nUsedGPR, nUsedFPR ) )
	{
		return;
	}

	if ( nUsedFPR > 0 )
	{
		// HFA: each member occupies one v register; the members are contiguous
		// in the struct.  Copy element-by-element to honour FLOAT (4-byte) vs
		// DOUBLE (8-byte) element width.
		HfaKind kind = HFA_NONE;
		int count = 0;
		collectHfa( pTypeRef, kind, count );
		if ( kind == HFA_FLOAT )
		{
			float *pDest = reinterpret_cast< float *>( pStruct );
			for ( int i = 0; i < nUsedFPR; ++i )
				pDest[i] = *reinterpret_cast< const float *>( pFPR + i );
		}
		else // HFA_DOUBLE
		{
			double *pDest = reinterpret_cast< double *>( pStruct );
			for ( int i = 0; i < nUsedFPR; ++i )
				pDest[i] = pFPR[i];
		}
	}
	else
	{
		typelib_TypeDescription * pTypeDescr = 0;
		TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );
		memcpy( pStruct, pGPR, pTypeDescr->nSize );
		TYPELIB_DANGER_RELEASE( pTypeDescr );
	}
}

sal_uInt32 align_stack_offset( sal_uInt32 offset, typelib_TypeDescriptionReference *pTypeRef )
{
	typelib_TypeDescription * pTypeDescr = 0;
	TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );
	sal_uInt32 alignment = pTypeDescr->nAlignment;
	TYPELIB_DANGER_RELEASE( pTypeDescr );
	if ( alignment == 0 )
		alignment = 1;
	return ( offset + alignment - 1 ) & ~( alignment - 1 );
}

sal_uInt32 stack_size( typelib_TypeDescriptionReference *pTypeRef )
{
	typelib_TypeDescription * pTypeDescr = 0;
	TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );
	sal_uInt32 size = pTypeDescr->nSize;
	// AAPCS64 requires each stack argument to occupy a full 8-byte
	// slot; small types are sign- or zero-extended into the slot.
	if ( size < 8 )
		size = 8;
	TYPELIB_DANGER_RELEASE( pTypeDescr );
	return size;
}

} // namespace aarch64
