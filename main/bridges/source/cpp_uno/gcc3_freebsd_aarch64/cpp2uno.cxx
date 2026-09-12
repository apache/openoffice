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

#include <stdio.h>
#include <stdlib.h>
#include <hash_map>

#include <rtl/alloc.h>
#include <osl/mutex.hxx>

#include <com/sun/star/uno/genfunc.hxx>
#include "com/sun/star/uno/RuntimeException.hpp"
#include <uno/data.h>
#include <typelib/typedescription.hxx>

#include "bridges/cpp_uno/shared/bridge.hxx"
#include "bridges/cpp_uno/shared/cppinterfaceproxy.hxx"
#include "bridges/cpp_uno/shared/types.hxx"
#include "bridges/cpp_uno/shared/vtablefactory.hxx"

#include "abi.hxx"
#include "share.hxx"

using namespace ::osl;
using namespace ::rtl;
using namespace ::com::sun::star::uno;

//==================================================================================================

// Perform the UNO call
//
// We must convert the parameters stored in gpreg, fpreg and ovrflw to UNO
// arguments and call pThis->getUnoI()->pDispatcher.
//
// gpreg:  this, [gpr params x0..x7]   (the indirect-result ptr is x8, separate)
// fpreg:  [fpr params d0..d7]
// ovrflw: [gpr or fpr params (properly aligned)]
//
// On AArch64 a structure bigger than 16 bytes is returned via the buffer
// addressed by x8 (pIndirectReturn); 'this' is always x0 = gpreg[0].
// Simple types are returned in x0,x1 (int) or d0,d1 (fp); HFAs in d0..d3;
// non-HFA structures <= 16 bytes in x0,x1.
static typelib_TypeClass cpp2uno_call(
	bridges::cpp_uno::shared::CppInterfaceProxy * pThis,
	const typelib_TypeDescription * pMemberTypeDescr,
	typelib_TypeDescriptionReference * pReturnTypeRef, // 0 indicates void return
	sal_Int32 nParams, typelib_MethodParameter * pParams,
	void ** gpreg, void ** fpreg, unsigned char * ovrflw,
	void * pIndirectReturn, // AArch64 x8 indirect-result pointer (0 if none)
	sal_uInt64 * pRegisterReturn /* space for register return */ )
{
	unsigned int nr_gpr = 0; //number of gpr registers used
	unsigned int nr_fpr = 0; //number of fpr registers used
	sal_uInt32 stackOffset = 0;

	// return
	typelib_TypeDescription * pReturnTypeDescr = 0;
	if (pReturnTypeRef)
		TYPELIB_DANGER_GET( &pReturnTypeDescr, pReturnTypeRef );

	void * pUnoReturn = 0;
	void * pCppReturn = 0; // complex return ptr: if != 0 && != pUnoReturn, reconversion need

	if ( pReturnTypeDescr )
	{
		if ( aarch64::return_in_hidden_param( pReturnTypeRef ) )
		{
			// AArch64: the indirect-result pointer arrives in x8, NOT in the
			// first general-purpose argument register (unlike x86-64 SysV).
			// So we take it from pIndirectReturn and do NOT consume a gpreg
			// slot here; 'this' still occupies gpreg[0] below.
			pCppReturn = pIndirectReturn;

			pUnoReturn = ( bridges::cpp_uno::shared::relatesToInterfaceType( pReturnTypeDescr )
						   ? alloca( pReturnTypeDescr->nSize )
						   : pCppReturn ); // direct way
		}
		else
			pUnoReturn = pRegisterReturn; // direct way for simple types
	}

	// pop this (x0)
	gpreg++;
	nr_gpr++;

	// stack space
	// parameters
	void ** pUnoArgs = reinterpret_cast<void **>(alloca( 4 * sizeof(void *) * nParams ));
	void ** pCppArgs = pUnoArgs + nParams;
	// indices of values this have to be converted (interface conversion cpp<=>uno)
	sal_Int32 * pTempIndizes = reinterpret_cast<sal_Int32 *>(pUnoArgs + (2 * nParams));
	// type descriptions for reconversions
	typelib_TypeDescription ** ppTempParamTypeDescr = reinterpret_cast<typelib_TypeDescription **>(pUnoArgs + (3 * nParams));

	sal_Int32 nTempIndizes = 0;

	for ( sal_Int32 nPos = 0; nPos < nParams; ++nPos )
	{
		const typelib_MethodParameter & rParam = pParams[nPos];

		int nUsedGPR = 0;
		int nUsedFPR = 0;
		if ( !rParam.bOut && bridges::cpp_uno::shared::isSimpleType( rParam.pTypeRef ) ) // value
		{
			// A simple UNO type occupies exactly one register, GPR or FPR.
			if ( rParam.pTypeRef->eTypeClass == typelib_TypeClass_FLOAT ||
				 rParam.pTypeRef->eTypeClass == typelib_TypeClass_DOUBLE )
			{
				nUsedFPR = 1; nUsedGPR = 0;
			}
			else
			{
				nUsedFPR = 0; nUsedGPR = 1;
			}

			OSL_ASSERT( ( nUsedFPR == 1 && nUsedGPR == 0 ) || ( nUsedFPR == 0 && nUsedGPR == 1 ) );

			if ( nUsedFPR == 1 )
			{
				if ( nr_fpr < aarch64::MAX_FPR_REGS )
				{
					pCppArgs[nPos] = pUnoArgs[nPos] = fpreg++;
					nr_fpr++;
				}
				else
				{
					stackOffset = aarch64::align_stack_offset(
						stackOffset, rParam.pTypeRef );
					pCppArgs[nPos] = pUnoArgs[nPos] = ovrflw + stackOffset;
					stackOffset += aarch64::stack_size( rParam.pTypeRef );
				}
			}
			else if ( nUsedGPR == 1 )
			{
				if ( nr_gpr < aarch64::MAX_GPR_REGS )
				{
					pCppArgs[nPos] = pUnoArgs[nPos] = gpreg++;
					nr_gpr++;
				}
				else
				{
					stackOffset = aarch64::align_stack_offset(
						stackOffset, rParam.pTypeRef );
					pCppArgs[nPos] = pUnoArgs[nPos] = ovrflw + stackOffset;
					stackOffset += aarch64::stack_size( rParam.pTypeRef );
				}
			}
		}
		else // struct <= 16 bytes || ptr to complex value || ref
		{
			typelib_TypeDescription * pParamTypeDescr = 0;
			TYPELIB_DANGER_GET( &pParamTypeDescr, rParam.pTypeRef );

			void *pCppStack = 0;
			if ( nr_gpr < aarch64::MAX_GPR_REGS )
			{
				pCppArgs[nPos] = pCppStack = *gpreg++;
				nr_gpr++;
			}
			else
			{
				stackOffset = (stackOffset + sizeof(void *) - 1) &
					~(sizeof(void *) - 1);
				pCppArgs[nPos] = pCppStack =
					*reinterpret_cast<void **>( ovrflw + stackOffset );
				stackOffset += sizeof(void *);
			}

			if (! rParam.bIn) // is pure out
			{
				// uno out is unconstructed mem!
				pUnoArgs[nPos] = alloca( pParamTypeDescr->nSize );
				pTempIndizes[nTempIndizes] = nPos;
				// will be released at reconversion
				ppTempParamTypeDescr[nTempIndizes++] = pParamTypeDescr;
			}
			else if ( bridges::cpp_uno::shared::relatesToInterfaceType( pParamTypeDescr ) ) // is in/inout
			{
				uno_copyAndConvertData( pUnoArgs[nPos] = alloca( pParamTypeDescr->nSize ),
										pCppStack, pParamTypeDescr,
										pThis->getBridge()->getCpp2Uno() );
				pTempIndizes[nTempIndizes] = nPos; // has to be reconverted
				// will be released at reconversion
				ppTempParamTypeDescr[nTempIndizes++] = pParamTypeDescr;
			}

			if ( pCppStack )
			{
				if ( pParamTypeDescr->eTypeClass == typelib_TypeClass_STRUCT ||
					 pParamTypeDescr->eTypeClass == typelib_TypeClass_EXCEPTION )
				{
					// struct in registers/stack: copy it to a temporary if
					// it needs conversion
					if ( bridges::cpp_uno::shared::relatesToInterfaceType( pParamTypeDescr ) )
					{
						// already handled above
					}
					else
						pUnoArgs[nPos] = pCppStack;
				}
			}
			if ( pParamTypeDescr )
				TYPELIB_DANGER_RELEASE( pParamTypeDescr );
		}
	}

	// ExceptionHolder
	uno_Any aUnoExc; // Any will be constructed by callee
	uno_Any * pUnoExc = &aUnoExc;

	// invoke uno dispatch call
	(*pThis->getUnoI()->pDispatcher)( pThis->getUnoI(), pMemberTypeDescr, pUnoReturn, pUnoArgs, &pUnoExc );

	// in case an exception occurred...
	if ( pUnoExc )
	{
		// destruct temporary in/inout params
		for ( ; nTempIndizes--; )
		{
			sal_Int32 nIndex = pTempIndizes[nTempIndizes];

			if (pParams[nIndex].bIn) // is in/inout => was constructed
				uno_destructData( pUnoArgs[nIndex], ppTempParamTypeDescr[nTempIndizes], 0 );
			TYPELIB_DANGER_RELEASE( ppTempParamTypeDescr[nTempIndizes] );
		}
		if (pReturnTypeDescr)
			TYPELIB_DANGER_RELEASE( pReturnTypeDescr );

		CPPU_CURRENT_NAMESPACE::raiseException( &aUnoExc, pThis->getBridge()->getUno2Cpp() ); // has to destruct the any
		// is here for dummy
		return typelib_TypeClass_VOID;
	}
	else // else no exception occurred...
	{
		// temporary params
		for ( ; nTempIndizes--; )
		{
			sal_Int32 nIndex = pTempIndizes[nTempIndizes];
			typelib_TypeDescription * pParamTypeDescr = ppTempParamTypeDescr[nTempIndizes];

			if ( pParams[nIndex].bOut ) // inout/out
			{
				// convert and assign
				uno_destructData( pCppArgs[nIndex], pParamTypeDescr, cpp_release );
				uno_copyAndConvertData( pCppArgs[nIndex], pUnoArgs[nIndex], pParamTypeDescr,
										pThis->getBridge()->getUno2Cpp() );
			}
			// destroy temp uno param
			uno_destructData( pUnoArgs[nIndex], pParamTypeDescr, 0 );

			TYPELIB_DANGER_RELEASE( pParamTypeDescr );
		}
		// return
		if ( pCppReturn ) // has complex return
		{
			if ( pUnoReturn != pCppReturn ) // needs reconversion
			{
				uno_copyAndConvertData( pCppReturn, pUnoReturn, pReturnTypeDescr,
										pThis->getBridge()->getUno2Cpp() );
				// destroy temp uno return
				uno_destructData( pUnoReturn, pReturnTypeDescr, 0 );
			}
			// complex return ptr is set to return reg
			*reinterpret_cast<void **>(pRegisterReturn) = pCppReturn;
		}
		if ( pReturnTypeDescr )
		{
			typelib_TypeClass eRet = (typelib_TypeClass)pReturnTypeDescr->eTypeClass;
			TYPELIB_DANGER_RELEASE( pReturnTypeDescr );
			return eRet;
		}
		else
			return typelib_TypeClass_VOID;
	}
}


//==================================================================================================
extern "C" sal_uInt32 cpp_vtable_call(
	sal_Int32 nFunctionIndex, sal_Int32 nVtableOffset,
	void ** gpreg, void ** fpreg, unsigned char * ovrflw,
	void * pIndirectReturn, // AArch64 x8 indirect-result pointer (0 if none)
	sal_uInt64 * pRegisterReturn /* space for register return */ )
{
	// gpreg:  this, [other gpr params x0..x7]
	// fpreg:  [fpr params d0..d7]
	// ovrflw: [gpr or fpr params (properly aligned)]
	// pIndirectReturn: x8 (the hidden return buffer), when bit 0x80000000 set.
	//
	// On AArch64 'this' is ALWAYS x0 = gpreg[0]; the hidden return pointer is
	// the separate x8 register, not a displaced first GPR (unlike x86-64 SysV
	// where it occupied gpreg[0] and 'this' moved to gpreg[1]).
	if ( nFunctionIndex & 0x80000000 )
		nFunctionIndex &= 0x7fffffff;

	void * pThis = gpreg[0];
	pThis = static_cast<char *>( pThis ) - nVtableOffset;

	bridges::cpp_uno::shared::CppInterfaceProxy * pCppI =
		bridges::cpp_uno::shared::CppInterfaceProxy::castInterfaceToProxy( pThis );

	typelib_InterfaceTypeDescription * pTypeDescr = pCppI->getTypeDescr();

	OSL_ENSURE( nFunctionIndex < pTypeDescr->nMapFunctionIndexToMemberIndex, "### illegal vtable index!\n" );
	if ( nFunctionIndex >= pTypeDescr->nMapFunctionIndexToMemberIndex )
	{
		throw RuntimeException( OUString::createFromAscii("illegal vtable index!"),
								reinterpret_cast<XInterface *>( pCppI ) );
	}

	// determine called method
	sal_Int32 nMemberPos = pTypeDescr->pMapFunctionIndexToMemberIndex[nFunctionIndex];
	OSL_ENSURE( nMemberPos < pTypeDescr->nAllMembers, "### illegal member index!\n" );

	TypeDescription aMemberDescr( pTypeDescr->ppAllMembers[nMemberPos] );

	sal_uInt32 eRet;
	switch ( aMemberDescr.get()->eTypeClass )
	{
		case typelib_TypeClass_INTERFACE_ATTRIBUTE:
		{
			typelib_TypeDescriptionReference *pAttrTypeRef =
				reinterpret_cast<typelib_InterfaceAttributeTypeDescription *>( aMemberDescr.get() )->pAttributeTypeRef;

			if ( pTypeDescr->pMapMemberIndexToFunctionIndex[nMemberPos] == nFunctionIndex )
			{
				// is GET method
					eRet = cpp2uno_call( pCppI, aMemberDescr.get(), pAttrTypeRef,
						0, 0, // no params
						gpreg, fpreg, ovrflw, pIndirectReturn, pRegisterReturn );
					eRet = aarch64::get_return_kind( pAttrTypeRef );
			}
			else
			{
				// is SET method
				typelib_MethodParameter aParam;
				aParam.pTypeRef = pAttrTypeRef;
				aParam.bIn		= sal_True;
				aParam.bOut		= sal_False;

				eRet = cpp2uno_call( pCppI, aMemberDescr.get(),
						0, // indicates void return
						1, &aParam,
						gpreg, fpreg, ovrflw, pIndirectReturn, pRegisterReturn );
			}
			break;
		}
		case typelib_TypeClass_INTERFACE_METHOD:
		{
			// is METHOD
			switch ( nFunctionIndex )
			{
				case 1: // acquire()
					pCppI->acquireProxy(); // non virtual call!
					eRet = typelib_TypeClass_VOID;
					break;
				case 2: // release()
					pCppI->releaseProxy(); // non virtual call!
					eRet = typelib_TypeClass_VOID;
					break;
				case 0: // queryInterface() opt
				{
					// queryInterface([in] type) returns an Any (> 16 bytes),
					// so on AArch64 the result buffer is x8 (pIndirectReturn),
					// 'this' is gpreg[0], and the type argument is the first
					// real parameter, gpreg[1].
					typelib_TypeDescription * pTD = 0;
					TYPELIB_DANGER_GET( &pTD, reinterpret_cast<Type *>( gpreg[1] )->getTypeLibType() );
					if ( pTD )
					{
						XInterface * pInterface = 0;
						(*pCppI->getBridge()->getCppEnv()->getRegisteredInterface)
							( pCppI->getBridge()->getCppEnv(),
							  reinterpret_cast<void **>(&pInterface),
							  pCppI->getOid().pData,
							  reinterpret_cast<typelib_InterfaceTypeDescription *>( pTD ) );

						if ( pInterface )
						{
							::uno_any_construct( reinterpret_cast<uno_Any *>( pIndirectReturn ),
												 &pInterface, pTD, cpp_acquire );

							pInterface->release();
							TYPELIB_DANGER_RELEASE( pTD );

							reinterpret_cast<void **>( pRegisterReturn )[0] = pIndirectReturn;
							eRet = typelib_TypeClass_ANY;
							break;
						}
						TYPELIB_DANGER_RELEASE( pTD );
					}
				} // else perform queryInterface()
				default:
				{
					typelib_InterfaceMethodTypeDescription *pMethodTD =
						reinterpret_cast<typelib_InterfaceMethodTypeDescription *>( aMemberDescr.get() );

					eRet = cpp2uno_call( pCppI, aMemberDescr.get(),
										 pMethodTD->pReturnTypeRef,
										 pMethodTD->nParams,
										 pMethodTD->pParams,
										 gpreg, fpreg, ovrflw, pIndirectReturn, pRegisterReturn );
					eRet = aarch64::get_return_kind( pMethodTD->pReturnTypeRef );
				}
			}
			break;
		}
		default:
		{
			throw RuntimeException( OUString::createFromAscii("no member description found!"),
									reinterpret_cast<XInterface *>( pCppI ) );
			// is here for dummy
			eRet = typelib_TypeClass_VOID;
		}
	}

	return eRet;
}
