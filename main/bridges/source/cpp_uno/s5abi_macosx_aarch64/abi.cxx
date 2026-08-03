/**************************************************************
 *
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
 *
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_bridges.hxx"

// This is an implementation of the parameter-classification rules of the
// AArch64 procedure call standard ("Procedure Call Standard for the Arm 64-bit
// Architecture", ARM IHI 0055), with the deviations documented in Apple's
// "Writing ARM64 Code for Apple Platforms".
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
// ../../../../AAPCS64_BRIDGE_SPEC.md.  libffi's aarch64 backend was consulted
// only as a behavioural reference; no code is copied.

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
                sal_Int32 elementSize = rKind == HFA_FLOAT ? 4 : 8;
                bOk = pTypeDescr->nSize == rCount * elementSize;
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
    if ( collectHfa( pTypeRef, kind, count ) && kind != HFA_NONE && count >= 1 && count <= 4 )
    {
        nUsedGPR = 0;
        nUsedFPR = count;       // one v register per member
        return true;
    }

    // Otherwise classify by size.
    typelib_TypeDescription * pTypeDescr = 0;
    TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );
    sal_Int32 nSize = pTypeDescr->nSize;
    TYPELIB_DANGER_RELEASE( pTypeDescr );

    if ( nSize > 16 )
    {
        // Non-HFA aggregate > 16 bytes => passed indirectly.
        return false;
    }

    // Non-HFA aggregate <= 16 bytes => 1 or 2 GPRs (8 bytes each).
    nUsedGPR = ( nSize > 8 ) ? 2 : 1;
    nUsedFPR = 0;
    return true;
}

} // anonymous namespace

bool aarch64::examine_argument( typelib_TypeDescriptionReference *pTypeRef, bool bInReturn, int &nUsedGPR, int &nUsedFPR )
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
#if OSL_DEBUG_LEVEL > 1
            OSL_TRACE( "Unhandled case: pTypeRef->eTypeClass == %d\n", pTypeRef->eTypeClass );
#endif
            OSL_ASSERT( 0 );
    }
    return false;
}

bool aarch64::return_in_hidden_param( typelib_TypeDescriptionReference *pTypeRef )
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
            // Apple's arm64 C++ ABI returns them through the buffer in x8.
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
    // Returned in registers iff examine_argument() says it fits; otherwise the
    // caller must pass an indirect-result buffer in x8.
    return !examine_argument( pTypeRef, true, g, s );
}

sal_uInt32 aarch64::get_return_kind( typelib_TypeDescriptionReference *pTypeRef )
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

void aarch64::fill_struct( typelib_TypeDescriptionReference *pTypeRef, const sal_uInt64 *pGPR, const double *pFPR, void *pStruct )
{
    int nUsedGPR = 0;
    int nUsedFPR = 0;
    if ( !examine_argument( pTypeRef, true, nUsedGPR, nUsedFPR ) )
    {
        // Should not happen: indirect returns are written through x8 directly,
        // not scattered here.
        OSL_ASSERT( 0 );
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
            float *pDest = reinterpret_cast<float *>( pStruct );
            for ( int i = 0; i < nUsedFPR; ++i )
                pDest[i] = *reinterpret_cast<const float *>( pFPR + i );
        }
        else // HFA_DOUBLE
        {
            double *pDest = reinterpret_cast<double *>( pStruct );
            for ( int i = 0; i < nUsedFPR; ++i )
                pDest[i] = pFPR[i];
        }
    }
    else
    {
        typelib_TypeDescription * pTypeDescr = 0;
        TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );
        // The registers contain up to 16 bytes, but the destination has the
        // aggregate's exact size and need not be 64-bit aligned.
        memcpy( pStruct, pGPR, pTypeDescr->nSize );
        TYPELIB_DANGER_RELEASE( pTypeDescr );
    }
}

sal_uInt32 aarch64::align_stack_offset(
    sal_uInt32 offset, typelib_TypeDescriptionReference *pTypeRef )
{
    typelib_TypeDescription * pTypeDescr = 0;
    TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );
    sal_uInt32 alignment = pTypeDescr->nAlignment;
    TYPELIB_DANGER_RELEASE( pTypeDescr );
    if ( alignment == 0 )
        alignment = 1;
    return (offset + alignment - 1) & ~(alignment - 1);
}

sal_uInt32 aarch64::stack_size( typelib_TypeDescriptionReference *pTypeRef )
{
    typelib_TypeDescription * pTypeDescr = 0;
    TYPELIB_DANGER_GET( &pTypeDescr, pTypeRef );
    sal_uInt32 size = pTypeDescr->nSize;
    TYPELIB_DANGER_RELEASE( pTypeDescr );
    return size;
}
