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



#include <stdio.h>

#include <avmedia/mediawindow.hxx>
#include "mediawindow_impl.hxx"
#include "mediamisc.hxx"
#include "mediawindow.hrc"
#include <tools/urlobj.hxx>
#include <vcl/msgbox.hxx>
#include <unotools/pathoptions.hxx>
#include <sfx2/filedlghelper.hxx>
#include <comphelper/processfactory.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/media/XManager.hpp>
#include "com/sun/star/ui/dialogs/TemplateDescription.hpp"

#define AVMEDIA_FRAMEGRABBER_DEFAULTFRAME_MEDIATIME 3.0

using namespace ::com::sun::star;

namespace avmedia {

// ---------------
// - MediaWindow -
// ---------------

MediaWindow::MediaWindow( Window* parent, bool bInternalMediaControl ) :
    mpImpl( new priv::MediaWindowImpl( parent, this, bInternalMediaControl ) )
{
    mpImpl->Show();
}

// -------------------------------------------------------------------------

MediaWindow::~MediaWindow()
{
    mpImpl->cleanUp();
    delete mpImpl;
    mpImpl = NULL;
}

// -------------------------------------------------------------------------

void MediaWindow::setURL( const ::rtl::OUString& rURL )
{
    if( mpImpl )
        mpImpl->setURL( rURL );
}

// -------------------------------------------------------------------------

const ::rtl::OUString& MediaWindow::getURL() const
{
    return mpImpl->getURL();
}

// -------------------------------------------------------------------------

bool MediaWindow::isValid() const
{
    return( mpImpl != NULL && mpImpl->isValid() );
}

// -------------------------------------------------------------------------

void MediaWindow::MouseMove( const MouseEvent& /* rMEvt */ )
{
}

// ---------------------------------------------------------------------

void MediaWindow::MouseButtonDown( const MouseEvent& /* rMEvt */ )
{
}

// ---------------------------------------------------------------------

void MediaWindow::MouseButtonUp( const MouseEvent& /* rMEvt */ )
{
}

// -------------------------------------------------------------------------

void MediaWindow::KeyInput( const KeyEvent& /* rKEvt */ )
{
}

// -------------------------------------------------------------------------

void MediaWindow::KeyUp( const KeyEvent& /* rKEvt */ )
{
}

// -------------------------------------------------------------------------

void MediaWindow::Command( const CommandEvent& /* rCEvt */ )
{
}

// -------------------------------------------------------------------------

sal_Int8 MediaWindow::AcceptDrop( const AcceptDropEvent& /* rEvt */ )
{
    return 0;
}

// -------------------------------------------------------------------------

sal_Int8 MediaWindow::ExecuteDrop( const ExecuteDropEvent& /* rEvt */ )
{
    return 0;
}

// -------------------------------------------------------------------------

void MediaWindow::StartDrag( sal_Int8 /* nAction */, const Point& /* rPosPixel */ )
{
}

// -------------------------------------------------------------------------

bool MediaWindow::hasPreferredSize() const
{
    return( mpImpl != NULL && mpImpl->hasPreferredSize() );
}

// -------------------------------------------------------------------------

Size MediaWindow::getPreferredSize() const
{
    return mpImpl->getPreferredSize();
}

// -------------------------------------------------------------------------

void MediaWindow::setPosSize( const Rectangle& rNewRect )
{
    if( mpImpl )
    {
        mpImpl->setPosSize( rNewRect );
    }
}

// -------------------------------------------------------------------------

Rectangle MediaWindow::getPosSize() const
{
    return Rectangle( mpImpl->GetPosPixel(), mpImpl->GetSizePixel() );
}

// -------------------------------------------------------------------------

void MediaWindow::setPointer( const Pointer& rPointer )
{
    if( mpImpl )
        mpImpl->setPointer( rPointer );
}

// -------------------------------------------------------------------------

const Pointer& MediaWindow::getPointer() const
{
    return mpImpl->getPointer();
}

// -------------------------------------------------------------------------

bool MediaWindow::setZoom( ::com::sun::star::media::ZoomLevel eLevel )
{
    return( mpImpl != NULL && mpImpl->setZoom( eLevel ) );
}

// -------------------------------------------------------------------------

::com::sun::star::media::ZoomLevel MediaWindow::getZoom() const
{
    return mpImpl->getZoom();
}

// -------------------------------------------------------------------------

bool MediaWindow::start()
{
    return( mpImpl != NULL && mpImpl->start() );
}

// -------------------------------------------------------------------------

void MediaWindow::stop()
{
    if( mpImpl )
        mpImpl->stop();
}

// -------------------------------------------------------------------------

bool MediaWindow::isPlaying() const
{
    return( mpImpl != NULL && mpImpl->isPlaying() );
}

// -------------------------------------------------------------------------

double MediaWindow::getDuration() const
{
    return mpImpl->getDuration();
}

// -------------------------------------------------------------------------

void MediaWindow::setMediaTime( double fTime )
{
    if( mpImpl )
        mpImpl->setMediaTime( fTime );
}

// -------------------------------------------------------------------------

double MediaWindow::getMediaTime() const
{
    return mpImpl->getMediaTime();
}

// -------------------------------------------------------------------------

void MediaWindow::setStopTime( double fTime )
{
    if( mpImpl )
        mpImpl->setStopTime( fTime );
}

// -------------------------------------------------------------------------

double MediaWindow::getStopTime() const
{
    return mpImpl->getStopTime();
}

// -------------------------------------------------------------------------

void MediaWindow::setRate( double fRate )
{
    if( mpImpl )
        mpImpl->setRate( fRate );
}

// -------------------------------------------------------------------------

double MediaWindow::getRate() const
{
    return mpImpl->getRate();
}

// -------------------------------------------------------------------------

void MediaWindow::setPlaybackLoop( bool bSet )
{
    if( mpImpl )
        mpImpl->setPlaybackLoop( bSet );
}

// -------------------------------------------------------------------------

bool MediaWindow::isPlaybackLoop() const
{
    return mpImpl->isPlaybackLoop();
}

// -------------------------------------------------------------------------

void MediaWindow::setMute( bool bSet )
{
    if( mpImpl )
        mpImpl->setMute( bSet );
}

// -------------------------------------------------------------------------

bool MediaWindow::isMute() const
{
    return mpImpl->isMute();
}

// -------------------------------------------------------------------------

void MediaWindow::updateMediaItem( MediaItem& rItem ) const
{
    if( mpImpl )
        mpImpl->updateMediaItem( rItem );
}

// -------------------------------------------------------------------------

void MediaWindow::executeMediaItem( const MediaItem& rItem )
{
    if( mpImpl )
        mpImpl->executeMediaItem( rItem );
}

// -------------------------------------------------------------------------

void MediaWindow::show()
{
    if( mpImpl )
        mpImpl->Show();
}

// -------------------------------------------------------------------------

void MediaWindow::hide()
{
    if( mpImpl )
        mpImpl->Hide();
}

// -------------------------------------------------------------------------

void MediaWindow::enable()
{
    if( mpImpl )
        mpImpl->Enable();
}

// -------------------------------------------------------------------------

void MediaWindow::disable()
{
    if( mpImpl )
        mpImpl->Disable();
}

// -------------------------------------------------------------------------

Window* MediaWindow::getWindow() const
{
    return mpImpl;
}

// -------------------------------------------------------------------------

void MediaWindow::getMediaFilters( FilterNameVector& rFilterNameVector )
{
    static const char* pFilters[] = {   "AIF Audio", "aif;aiff",
                                        "AU Audio", "au",
                                        "AVI", "avi",
                                        "CD Audio", "cda",
                                        "FLAC Audio", "flac",
                                        "Flash Video", "flv",
                                        "Matroska Media", "mkv",
                                        "MIDI Audio", "mid;midi",
                                        "MPEG Audio", "mp2;mp3;mpa",
                                        "MPEG Video", "mpg;mpeg;mpv;mp4",
                                        "Ogg bitstream", "ogg;oga;ogv",
                                        "Quicktime Video", "mov",
                                        "Vivo Video", "viv",
                                        "WAVE Audio", "wav",
                                        "Windows Media Video", "wmv" };

    unsigned int i;
	for( i = 0; i < ( sizeof( pFilters ) / sizeof( char* ) ); i += 2 )
    {
        rFilterNameVector.push_back( ::std::make_pair< ::rtl::OUString, ::rtl::OUString >(
                                        ::rtl::OUString::createFromAscii( pFilters[ i ] ),
                                        ::rtl::OUString::createFromAscii( pFilters[ i + 1 ] ) ) );
    }
}

// -------------------------------------------------------------------------

bool MediaWindow::executeMediaURLDialog( Window* /* pParent */, ::rtl::OUString& rURL, bool bInsertDialog )
{
    ::sfx2::FileDialogHelper        aDlg( com::sun::star::ui::dialogs::TemplateDescription::FILEOPEN_SIMPLE, 0 );
    static const ::rtl::OUString    aWildcard( RTL_CONSTASCII_USTRINGPARAM( "*." ) );
    FilterNameVector                aFilters;
    const ::rtl::OUString           aSeparator( RTL_CONSTASCII_USTRINGPARAM( ";" ) );
    ::rtl::OUString                 aAllTypes;

    aDlg.SetTitle( AVMEDIA_RESID( bInsertDialog ? AVMEDIA_STR_INSERTMEDIA_DLG : AVMEDIA_STR_OPENMEDIA_DLG ) );

    getMediaFilters( aFilters );

	unsigned int i;
    for( i = 0; i < aFilters.size(); ++i )
    {
        for( sal_Int32 nIndex = 0; nIndex >= 0; )
        {
            if( !aAllTypes.isEmpty() )
                aAllTypes += aSeparator;

            ( aAllTypes += aWildcard ) += aFilters[ i ].second.getToken( 0, ';', nIndex );
        }
    }

    // add filter for all media types
    aDlg.AddFilter( AVMEDIA_RESID( AVMEDIA_STR_ALL_MEDIAFILES ), aAllTypes );

    for( i = 0; i < aFilters.size(); ++i )
    {
        ::rtl::OUString aTypes;

        for( sal_Int32 nIndex = 0; nIndex >= 0; )
        {
            if( !aTypes.isEmpty() )
                aTypes += aSeparator;

            ( aTypes += aWildcard ) += aFilters[ i ].second.getToken( 0, ';', nIndex );
        }

        // add single filters
        aDlg.AddFilter( aFilters[ i ].first, aTypes );
    }

    // add filter for all types
    aDlg.AddFilter( AVMEDIA_RESID( AVMEDIA_STR_ALL_FILES ), String( RTL_CONSTASCII_USTRINGPARAM( "*.*" ) ) );

    if( aDlg.Execute() == ERRCODE_NONE )
    {
        const INetURLObject aURL( aDlg.GetPath() );
        rURL = aURL.GetMainURL( INetURLObject::DECODE_UNAMBIGUOUS );
    }
    else if( !rURL.isEmpty() )
        rURL = ::rtl::OUString();

    return !rURL.isEmpty();
}

// -------------------------------------------------------------------------

void MediaWindow::executeFormatErrorBox( Window* pParent )
{
    ErrorBox aErrBox( pParent, AVMEDIA_RESID( AVMEDIA_ERR_URL ) );

    aErrBox.Execute();
}

// -------------------------------------------------------------------------

bool MediaWindow::isMediaURL( const ::rtl::OUString& rURL, bool bDeep, Size* pPreferredSizePixel )
{
    const INetURLObject aURL( rURL );
    bool                bRet = false;

    if( aURL.GetProtocol() != INET_PROT_NOT_VALID )
    {
        if( bDeep || pPreferredSizePixel )
        {
            try
            {
                sal_Bool bIsJavaBasedMediaWindow;
    	        uno::Reference< media::XPlayer > xPlayer( priv::MediaWindowImpl::createPlayer(
        	                                                aURL.GetMainURL( INetURLObject::DECODE_UNAMBIGUOUS ),
                                                            bIsJavaBasedMediaWindow ) );

            	if( xPlayer.is() )
                {
                    bRet = true;

                    if( pPreferredSizePixel )
                    {
                        const awt::Size aAwtSize( xPlayer->getPreferredPlayerWindowSize() );

                        pPreferredSizePixel->Width() = aAwtSize.Width;
                        pPreferredSizePixel->Height() = aAwtSize.Height;
                    }
                }
            }
            catch( ... )
            {
            }
        }
        else
        {
            FilterNameVector        aFilters;
            const ::rtl::OUString   aExt( aURL.getExtension() );

            getMediaFilters( aFilters );

			unsigned int i;
            for( i = 0; ( i < aFilters.size() ) && !bRet; ++i )
            {
                for( sal_Int32 nIndex = 0; nIndex >= 0 && !bRet; )
                {
                    if( aExt.equalsIgnoreAsciiCase( aFilters[ i ].second.getToken( 0, ';', nIndex ) ) )
                        bRet = true;
                }
            }
        }
    }

    return bRet;
}

// -------------------------------------------------------------------------

uno::Reference< media::XPlayer > MediaWindow::createPlayer( const ::rtl::OUString& rURL )
{
    sal_Bool bJavaBased = sal_False;
    return priv::MediaWindowImpl::createPlayer( rURL, bJavaBased );
}

// -------------------------------------------------------------------------

uno::Reference< graphic::XGraphic > MediaWindow::grabFrame( const ::rtl::OUString& rURL,
                                                            bool bAllowToCreateReplacementGraphic,
                                                            double fMediaTime )
{
    uno::Reference< media::XPlayer >    xPlayer( createPlayer( rURL ) );
    uno::Reference< graphic::XGraphic > xRet;
    ::std::auto_ptr< Graphic >          apGraphic;

    if( xPlayer.is() )
    {
        uno::Reference< media::XFrameGrabber > xGrabber( xPlayer->createFrameGrabber() );

        if( xGrabber.is() )
        {
            if( AVMEDIA_FRAMEGRABBER_DEFAULTFRAME == fMediaTime )
                fMediaTime = AVMEDIA_FRAMEGRABBER_DEFAULTFRAME_MEDIATIME;

            if( fMediaTime >= xPlayer->getDuration() )
                fMediaTime = ( xPlayer->getDuration() * 0.5 );

            xRet = xGrabber->grabFrame( fMediaTime );
        }

        if( !xRet.is() && bAllowToCreateReplacementGraphic )
        {
            awt::Size aPrefSize( xPlayer->getPreferredPlayerWindowSize() );

            if( !aPrefSize.Width && !aPrefSize.Height )
            {
                const BitmapEx aBmpEx( AVMEDIA_RESID( AVMEDIA_BMP_AUDIOLOGO ) );
                apGraphic.reset( new Graphic( aBmpEx ) );
            }
        }
    }

    if( !xRet.is() && !apGraphic.get() && bAllowToCreateReplacementGraphic )
    {
        const BitmapEx aBmpEx( AVMEDIA_RESID( AVMEDIA_BMP_EMPTYLOGO ) );
        apGraphic.reset( new Graphic( aBmpEx ) );
    }

    if( apGraphic.get() )
        xRet = apGraphic->GetXGraphic();

    return xRet;
}

} // namespace avmdia

