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



#include "manager.hxx"
#include "player.hxx"

#include <tools/urlobj.hxx>

#define AVMEDIA_WIN_MANAGER_IMPLEMENTATIONNAME "com.sun.star.comp.avmedia.Manager_DirectX"
#define AVMEDIA_WIN_MANAGER_SERVICENAME "com.sun.star.media.Manager_DirectX"

using namespace ::com::sun::star;

namespace avmedia { namespace win {
// ----------------
// - Manager -
// ----------------

Manager::Manager( const uno::Reference< uno::XComponentContext >& rxContext ) :
    mxContext( rxContext )
{
}

// ------------------------------------------------------------------------------

Manager::~Manager()
{
}

// ------------------------------------------------------------------------------

uno::Reference< media::XPlayer > SAL_CALL Manager::createPlayer( const ::rtl::OUString& rURL )
{
    Player*                             pPlayer( new Player( mxContext ) );
    uno::Reference< media::XPlayer >    xRet( pPlayer );
    const INetURLObject                 aURL( rURL );

    if( !pPlayer->create( aURL.GetMainURL( INetURLObject::DECODE_UNAMBIGUOUS ) )  )
        xRet = uno::Reference< media::XPlayer >();

    return xRet;
}

// ------------------------------------------------------------------------------

::rtl::OUString SAL_CALL Manager::getImplementationName(  )
{
    return getImplementationName_Static();
}

// ------------------------------------------------------------------------------

sal_Bool SAL_CALL Manager::supportsService( const ::rtl::OUString& ServiceName )
{
    return ServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( AVMEDIA_WIN_MANAGER_SERVICENAME ) );
}

// ------------------------------------------------------------------------------

uno::Sequence< ::rtl::OUString > SAL_CALL Manager::getSupportedServiceNames(  )
{
    return getSupportedServiceNames_Static();
}

// ------------------------------------------------------------------------------

::rtl::OUString Manager::getImplementationName_Static()
{
    return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( AVMEDIA_WIN_MANAGER_IMPLEMENTATIONNAME ) );
}

// ------------------------------------------------------------------------------

uno::Sequence< ::rtl::OUString > Manager::getSupportedServiceNames_Static()
{
    uno::Sequence< ::rtl::OUString > aRet(1);
    aRet[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( AVMEDIA_WIN_MANAGER_SERVICENAME ) );
    return aRet;
}

} // namespace win
} // namespace avmedia
