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



#include "player.hxx"
#include "window.hxx"

using namespace ::com::sun::star;

namespace avmedia { namespace xine {

// ----------------
// - Player -
// ----------------

Player::Player()
{
}

// ------------------------------------------------------------------------------

Player::~Player()
{
}

// ------------------------------------------------------------------------------

bool Player::create( const ::rtl::OUString& /* rURL */ )
{
    bool bRet = false;


    return bRet;
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::start(  )
{
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::stop(  )
{
}

// ------------------------------------------------------------------------------

sal_Bool SAL_CALL Player::isPlaying()
{
    bool bRet = false;

    return bRet;
}

// ------------------------------------------------------------------------------

double SAL_CALL Player::getDuration(  )
{
	double fRet = 0.0;

    return fRet;
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::setMediaTime( double /* fTime */ )
{
}

// ------------------------------------------------------------------------------

double SAL_CALL Player::getMediaTime(  )
{
	double fRet = 0.0;

    return fRet;
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::setStopTime( double /* fTime */ )
{
}

// ------------------------------------------------------------------------------

double SAL_CALL Player::getStopTime(  )
{
	double fRet = 0.0;

    return fRet;
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::setRate( double /* fRate */ )
{
}

// ------------------------------------------------------------------------------

double SAL_CALL Player::getRate(  )
{
	double fRet = 0.0;

    return fRet;
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::setPlaybackLoop( sal_Bool /* bSet */ )
{
}

// ------------------------------------------------------------------------------

sal_Bool SAL_CALL Player::isPlaybackLoop(  )
{
	bool bRet = false;

    return bRet;
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::setMute( sal_Bool /* bSet */ )
{
}

// ------------------------------------------------------------------------------

sal_Bool SAL_CALL Player::isMute(  )
{
	bool bRet = false;

    return bRet;
}

// ------------------------------------------------------------------------------

void SAL_CALL Player::setVolumeDB( sal_Int16 /* nVolumeDB */ )
{
}

// ------------------------------------------------------------------------------

sal_Int16 SAL_CALL Player::getVolumeDB(  )
{
	sal_Int16 nRet = 0;

    return nRet;
}

// ------------------------------------------------------------------------------

awt::Size SAL_CALL Player::getPreferredPlayerWindowSize(  )
{
    awt::Size aSize( 0, 0 );

    return aSize;
}

// ------------------------------------------------------------------------------

uno::Reference< ::media::XPlayerWindow > SAL_CALL Player::createPlayerWindow( const uno::Sequence< uno::Any >& aArguments )
{
    uno::Reference< ::media::XPlayerWindow >    xRet;
    awt::Size                                   aSize( getPreferredPlayerWindowSize() );

    if( aSize.Width > 0 && aSize.Height > 0 )
    {
        ::avmedia::xine::Window* pWindow = new ::avmedia::xine::Window( *this );

        xRet = pWindow;

        if( !pWindow->create( aArguments ) )
            xRet = uno::Reference< ::media::XPlayerWindow >();
    }

    return xRet;
}

// ------------------------------------------------------------------------------

uno::Reference< media::XFrameGrabber > SAL_CALL Player::createFrameGrabber(  )
{
    return NULL;
}

// ------------------------------------------------------------------------------

::rtl::OUString SAL_CALL Player::getImplementationName(  )
{
    return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( AVMEDIA_XINE_PLAYER_IMPLEMENTATIONNAME ) );
}

// ------------------------------------------------------------------------------

sal_Bool SAL_CALL Player::supportsService( const ::rtl::OUString& ServiceName )
{
    return ServiceName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM ( AVMEDIA_XINE_PLAYER_SERVICENAME ) );
}

// ------------------------------------------------------------------------------

uno::Sequence< ::rtl::OUString > SAL_CALL Player::getSupportedServiceNames(  )
{
    uno::Sequence< ::rtl::OUString > aRet(1);
    aRet[0] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( AVMEDIA_XINE_PLAYER_SERVICENAME ) );

    return aRet;
}

} // namespace xine
} // namespace avmedia
