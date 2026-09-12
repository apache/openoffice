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

#include <exception>
#include <typeinfo>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rtl/alloc.h"
#include "rtl/ustrbuf.hxx"

#include <com/sun/star/uno/genfunc.hxx>
#include "com/sun/star/uno/RuntimeException.hpp"
#include <uno/data.h>

#include <bridges/cpp_uno/shared/bridge.hxx>
#include <bridges/cpp_uno/shared/types.hxx>
#include "bridges/cpp_uno/shared/unointerfaceproxy.hxx"
#include "bridges/cpp_uno/shared/vtables.hxx"

#include "abi.hxx"
#include "share.hxx"

using namespace ::rtl;
using namespace ::com::sun::star::uno;

//==================================================================================================

// The AArch64 outgoing-call trampoline, implemented in call.s.  It loads the
// argument registers from the caller-prepared arrays, copies overflow args to
// the outgoing stack, performs the indirect call, and returns x0/x1 and d0..d3.
extern "C" void callVirtualFunction(
	sal_uInt64 pFunction, sal_uInt64 pIndirectRet,
	sal_uInt64 *pGPR, double *pFPR,
	unsigned char *pStack, sal_uInt32 nStackBytes,
	sal_uInt64 *pGPRReturn, double *pFPRReturn );

static void callVirtualMethod(void * pThis, sal_uInt32 nVtableIndex,
							  void * pRegisterReturn, typelib_TypeDescriptionReference * pReturnTypeRef, bool bSimpleReturn,
							  void * pIndirectReturn,
							  unsigned char *pStack, sal_uInt32 nStack,
							  sal_uInt64 *pGPR, sal_uInt32 nGPR,
							  double *pFPR, sal_uInt32 nFPR) __attribute__((noinline));

static void callVirtualMethod(void * pThis, sal_uInt32 nVtableIndex,
							  void * pRegisterReturn, typelib_TypeDescriptionReference * pReturnTypeRef, bool bSimpleReturn,
							  void * pIndirectReturn,
							   unsigned char *pStack, sal_uInt32 nStack,
							  sal_uInt64 *pGPR, sal_uInt32 nGPR,
							  double *pFPR, sal_uInt32 nFPR)
{
#if OSL_DEBUG_LEVEL > 1
	// Let's figure out what is really going on here
	{
		fprintf( stderr, "= callVirtualMethod() =\nGPR's (%d): ", nGPR );
		for ( unsigned int i = 0; i < nGPR; ++i )
			fprintf( stderr, "0x%lx, ", pGPR[i] );
		fprintf( stderr, "\nFPR's (%d): ", nFPR );
		for ( unsigned int i = 0; i < nFPR; ++i )
			fprintf( stderr, "%f, ", pFPR[i] );
		// The overflow area is a packed byte image, not an array of words.
		fprintf( stderr, "\nStack (%d bytes): ", nStack );
		for ( unsigned int i = 0; i < nStack; ++i )
			fprintf( stderr, "%02x ", pStack[i] );
		fprintf( stderr, "\n" );
	}
#endif

	// The call instruction within callVirtualFunction may throw exceptions.  So
	// that the compiler handles this correctly, it is important that (a)
	// callVirtualMethod might call dummy_can_throw_anything (although this never
	// happens at runtime), which in turn can throw exceptions, and (b)
	// callVirtualMethod is not inlined at its call site (so that any exceptions
	// thrown across the call are caught):
	if ( !pThis )
		CPPU_CURRENT_NAMESPACE::dummy_can_throw_anything( "xxx" ); // address something

	// Should not happen, but...
	if ( nFPR > aarch64::MAX_FPR_REGS )
		nFPR = aarch64::MAX_FPR_REGS;
	if ( nGPR > aarch64::MAX_GPR_REGS )
		nGPR = aarch64::MAX_GPR_REGS;

	// Get pointer to the C++ virtual method from the vtable.
	sal_uInt64 pMethod = *((sal_uInt64 *)pThis);
	pMethod += 8 * nVtableIndex;
	pMethod = *((sal_uInt64 *)pMethod);

	// Return register save areas: x0,x1 and d0..d3 (HFA up to 4 elements).
	sal_uInt64 gpReturn[2] = { 0, 0 };
	double fpReturn[4] = { 0, 0, 0, 0 };

	// Ensure the GPR/FPR arrays are the full register width even if fewer were
	// filled (the trampoline always loads all 8 of each).
	sal_uInt64 gpr[aarch64::MAX_GPR_REGS];
	double fpr[aarch64::MAX_FPR_REGS];
	for ( sal_uInt32 i = 0; i < aarch64::MAX_GPR_REGS; ++i )
		gpr[i] = ( i < nGPR ) ? pGPR[i] : 0;
	for ( sal_uInt32 i = 0; i < aarch64::MAX_FPR_REGS; ++i )
		fpr[i] = ( i < nFPR ) ? pFPR[i] : 0;

	callVirtualFunction(
		pMethod,
		reinterpret_cast<sal_uInt64>( pIndirectReturn ), // x8, 0 if none
		gpr, fpr,
		pStack, nStack,
		gpReturn, fpReturn );

	switch (pReturnTypeRef->eTypeClass)
	{
	case typelib_TypeClass_HYPER:
	case typelib_TypeClass_UNSIGNED_HYPER:
		*reinterpret_cast<sal_uInt64 *>( pRegisterReturn ) = gpReturn[0];
		break;
	case typelib_TypeClass_LONG:
		*reinterpret_cast<sal_Int32 *>( pRegisterReturn ) =
			*reinterpret_cast<sal_Int32 *>( &gpReturn[0] );
		break;
	case typelib_TypeClass_UNSIGNED_LONG:
	case typelib_TypeClass_ENUM:
		*reinterpret_cast<sal_uInt32 *>( pRegisterReturn ) =
			*reinterpret_cast<sal_uInt32 *>( &gpReturn[0] );
		break;
	case typelib_TypeClass_CHAR:
	case typelib_TypeClass_UNSIGNED_SHORT:
		*reinterpret_cast<sal_uInt16 *>( pRegisterReturn ) = *reinterpret_cast<sal_uInt16*>( &gpReturn[0] );
		break;
	case typelib_TypeClass_SHORT:
		*reinterpret_cast<sal_Int16 *>( pRegisterReturn ) =
			*reinterpret_cast<sal_Int16 *>( &gpReturn[0] );
		break;
	case typelib_TypeClass_BOOLEAN:
		*reinterpret_cast<sal_uInt8 *>( pRegisterReturn ) = *reinterpret_cast<sal_uInt8*>( &gpReturn[0] );
		break;
	case typelib_TypeClass_BYTE:
		*reinterpret_cast<sal_Int8 *>( pRegisterReturn ) =
			*reinterpret_cast<sal_Int8 *>( &gpReturn[0] );
		break;
	case typelib_TypeClass_FLOAT:
		*reinterpret_cast<float *>( pRegisterReturn ) =
			*reinterpret_cast<float *>( &fpReturn[0] );
		break;
	case typelib_TypeClass_DOUBLE:
		*reinterpret_cast<double *>( pRegisterReturn ) =
			*reinterpret_cast<double *>( &fpReturn[0] );
		break;
	case typelib_TypeClass_STRUCT:
	case typelib_TypeClass_EXCEPTION:
		aarch64::fill_struct( pReturnTypeRef, gpReturn, fpReturn, pRegisterReturn );
		break;
	default:
		break;
	}
}

//==================================================================================================
// The AArch64 outgoing-call trampoline, implemented in call.s, is declared in ABI
// and used by callVirtualMethod above. The rest of the file implements the
// public callVirtualMethod entrypoints used by the bridge; these are identical
// to the macOS AArch64 implementation and therefore are compatible on FreeBSD.

extern "C" void callVirtualFunction(
	sal_uInt64 pFunction, sal_uInt64 pIndirectRet,
	sal_uInt64 *pGPR, double *pFPR,
	unsigned char *pStack, sal_uInt32 nStackBytes,
	sal_uInt64 *pGPRReturn, double *pFPRReturn );

// ... per-bridge wrappers are generated at build time; no further code required here.
