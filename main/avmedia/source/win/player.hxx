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



#ifndef _PLAYER_HXX
#define _PLAYER_HXX

#include "wincommon.hxx"

#include "com/sun/star/media/XPlayer.hdl"

#include <cppuhelper/compbase2.hxx>
#include <cppuhelper/basemutex.hxx>

struct IGraphBuilder;
struct IBaseFilter;
struct IMediaControl;
struct IMediaEventEx;
struct IMediaSeeking;
struct IMediaPosition;
struct IBasicAudio;
struct IBasicVideo;
struct IVideoWindow;
struct IDDrawExclModeVideo;
struct IDirectDraw;
struct IDirectDrawSurface;

namespace avmedia { namespace win {

// ----------
// - Player -
// ----------
typedef ::cppu::WeakComponentImplHelper2< ::com::sun::star::media::XPlayer,
                                          ::com::sun::star::lang::XServiceInfo > Player_BASE;

class Player :  public cppu::BaseMutex,
                public Player_BASE
{
public:

                        Player( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext );
                        ~Player();

    bool                create( const ::rtl::OUString& rURL );

    void                setNotifyWnd( int nNotifyWnd );
    void                setDDrawParams( IDirectDraw* pDDraw, IDirectDrawSurface* pDDrawSurface );
    long                processEvent();

    const IVideoWindow* getVideoWindow() const;

    // XPlayer
    virtual void SAL_CALL start(  );
    virtual void SAL_CALL stop(  );
    virtual sal_Bool SAL_CALL isPlaying(  );
    virtual double SAL_CALL getDuration(  );
    virtual void SAL_CALL setMediaTime( double fTime );
    virtual double SAL_CALL getMediaTime(  );
    virtual void SAL_CALL setStopTime( double fTime );
    virtual double SAL_CALL getStopTime(  );
    virtual void SAL_CALL setRate( double fRate );
    virtual double SAL_CALL getRate(  );
    virtual void SAL_CALL setPlaybackLoop( sal_Bool bSet );
    virtual sal_Bool SAL_CALL isPlaybackLoop(  );
    virtual void SAL_CALL setMute( sal_Bool bSet );
    virtual sal_Bool SAL_CALL isMute(  );
    virtual void SAL_CALL setVolumeDB( sal_Int16 nVolumeDB );
    virtual sal_Int16 SAL_CALL getVolumeDB(  );
    virtual ::com::sun::star::awt::Size SAL_CALL getPreferredPlayerWindowSize(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::media::XPlayerWindow > SAL_CALL createPlayerWindow( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::media::XFrameGrabber > SAL_CALL createFrameGrabber(  );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

    // ::cppu::OComponentHelper
	virtual void SAL_CALL disposing(void);

private:

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > mxContext;

    ::rtl::OUString         maURL;
    IGraphBuilder*          mpGB;
    IBaseFilter*            mpOMF;
    IMediaControl*          mpMC;
    IMediaEventEx*          mpME;
    IMediaSeeking*          mpMS;
    IMediaPosition*         mpMP;
    IBasicAudio*            mpBA;
    IBasicVideo*            mpBV;
    IVideoWindow*           mpVW;
    IDDrawExclModeVideo*    mpEV;
    long                    mnUnmutedVolume;
	int						mnFrameWnd;

    sal_Bool                mbMuted;
    sal_Bool                mbLooping;
	sal_Bool                mbAddWindow;

    void                    ImplLayoutVideoWindow();
};

} // namespace win
} // namespace avmedia

#endif // _PLAYER_HXX
