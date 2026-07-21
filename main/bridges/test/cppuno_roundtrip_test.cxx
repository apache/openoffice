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

// Focused C++/UNO bridge round-trip test.
//
// Maps a plain C++ UNO object from the C++ language-binding environment to the
// binary UNO environment and back.  Constructing those Mappings loads the
// C++/UNO bridge DLL — msci_uno.dll on x86, mscx_uno.dll on x64 (the name comes
// from CPPU_CURRENT_LANGUAGE_BINDING_NAME).  A call on the doubly-mapped proxy
// then traverses the full bridge path in both directions:
//   C++ proxy vtable trampoline (call.asm) -> uno2cpp marshalling -> uno
//   dispatch -> cpp2uno marshalling -> the original C++ object -> and the return
//   value marshalled back.
// So a green run proves the x64 bridge's asm + argument/return marshalling
// actually work at run time, not merely that they link.

#include "gtest/gtest.h"

#include <rtl/ustring.hxx>
#include <rtl/ustring.h>
#include <uno/environment.hxx>
#include <uno/mapping.hxx>
#include <uno/dispatcher.h>
#include <uno/any2.h>
#include <uno/lbnames.h>
#include <typelib/typedescription.h>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>

using ::rtl::OUString;
using namespace ::com::sun::star::uno;
using ::com::sun::star::lang::XServiceInfo;

#define OUSTR(x) OUString( RTL_CONSTASCII_USTRINGPARAM(x) )

namespace {

// Minimal C++ UNO object.  XServiceInfo gives us a string return
// (getImplementationName), a bool arg + bool return (supportsService) and a
// sequence return (getSupportedServiceNames) — enough to exercise marshalling.
class TestObj : public ::cppu::WeakImplHelper1< XServiceInfo >
{
public:
    virtual OUString SAL_CALL getImplementationName()
        throw (RuntimeException)
        { return OUSTR("mscx.roundtrip.TestObj"); }

    virtual sal_Bool SAL_CALL supportsService( const OUString & name )
        throw (RuntimeException)
        { return name.equalsAscii("Yes"); }

    virtual Sequence< OUString > SAL_CALL getSupportedServiceNames()
        throw (RuntimeException)
        { Sequence< OUString > s(1); s[0] = OUSTR("svc.one"); return s; }
};

} // namespace

TEST(CppUnoBridge, roundtrip)
{
    // These load the C++/UNO bridge DLL (<cpp-env>_uno.dll).
    Mapping cpp2uno( Environment::getCurrent(), Environment( OUSTR(UNO_LB_UNO) ) );
    Mapping uno2cpp( Environment( OUSTR(UNO_LB_UNO) ), Environment::getCurrent() );
    ASSERT_TRUE( cpp2uno.is() );   // null => bridge DLL failed to load
    ASSERT_TRUE( uno2cpp.is() );

    Type const & rXSI = ::getCppuType( (Reference< XServiceInfo > *)0 );

    Reference< XServiceInfo > xOrig( static_cast< XServiceInfo * >( new TestObj ) );

    // Map C++ -> UNO(binary): the bridge (mscx_uno) wraps xOrig in a uno_Interface
    // whose dispatcher marshals uno-form calls into C++ calls on xOrig.
    void * pUno = 0;
    cpp2uno.mapInterface( &pUno, xOrig.get(), rXSI );
    ASSERT_TRUE( pUno != 0 );
    uno_Interface * pUnoI = static_cast< uno_Interface * >( pUno );

    // Map that uno_Interface back UNO -> C++.  UNO guarantees object identity across
    // a round-trip back to the SAME environment, so the bridge collapses this to the
    // original C++ object (xBack == xOrig).  This is correct behaviour, and proves the
    // bridge's uno->cpp mapInterface recognises its own proxies.
    void * pCppBack = 0;
    uno2cpp.mapInterface( &pCppBack, pUno, rXSI );
    ASSERT_TRUE( pCppBack != 0 );
    Reference< XServiceInfo > xBack(
        static_cast< XServiceInfo * >( pCppBack ), SAL_NO_ACQUIRE );
    EXPECT_EQ( xOrig.get(), xBack.get() );   // identity preserved (collapse)

    // Genuine marshalled call THROUGH the bridge: invoke getImplementationName on the
    // uno_Interface proxy directly (no collapse).  This runs the bridge's marshalling
    // in mscx_uno.dll: unmarshal the (empty) uno arg block, call xOrig's C++ vtable,
    // and marshal the OUString return back into uno form (an rtl_uString*).
    typelib_TypeDescription * pXsiTd = 0;
    rXSI.getDescription( &pXsiTd );
    if ( ! pXsiTd->bComplete )
        typelib_typedescription_complete( &pXsiTd );
    typelib_InterfaceTypeDescription * pItd =
        reinterpret_cast< typelib_InterfaceTypeDescription * >( pXsiTd );
    // ppAllMembers[0..2] = XInterface (queryInterface/acquire/release);
    // [3] = XServiceInfo::getImplementationName (first local method).
    ASSERT_TRUE( pItd->nAllMembers > 3 );
    typelib_TypeDescription * pMethodTd = 0;
    typelib_typedescriptionreference_getDescription( &pMethodTd, pItd->ppAllMembers[3] );
    ASSERT_TRUE( pMethodTd != 0 );

    rtl_uString * pRet = 0;         // uno-form return slot for a string method
    uno_Any exc;
    uno_Any * pExc = &exc;
    (*pUnoI->pDispatcher)( pUnoI, pMethodTd, &pRet, 0, &pExc );

    EXPECT_TRUE( pExc == 0 );        // no exception marshalled out
    ASSERT_TRUE( pRet != 0 );
    OUString ret( pRet );           // acquires
    rtl_uString_release( pRet );    // drop the dispatcher's reference
    EXPECT_TRUE( ret.equalsAscii("mscx.roundtrip.TestObj") );

    typelib_typedescription_release( pMethodTd );
    typelib_typedescription_release( pXsiTd );
    (*pUnoI->release)( pUnoI );
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
