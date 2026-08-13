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


#ifndef MACAVF_PLAYER_HXX
#define MACAVF_PLAYER_HXX

#include <osl/conditn.h>
#include "macavf_common.hxx"

#include "com/sun/star/media/XPlayer.hdl"

namespace avmedia { namespace macavf {

/*
// ----------
// - Player -
// ----------
*/

class Player
:   public MacAVObserverHandler
,   public ::cppu::WeakImplHelper2< ::com::sun::star::media::XPlayer,
                                    ::com::sun::star::lang::XServiceInfo >
{
public:
    explicit Player( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& );
    virtual  ~Player();

    bool create( const ::rtl::OUString& rURL );
    bool create( AVAsset* );

    // XPlayer
    virtual void SAL_CALL start();
    virtual void SAL_CALL stop();
    virtual sal_Bool SAL_CALL isPlaying();
    virtual double SAL_CALL getDuration();
    virtual void SAL_CALL setMediaTime( double fTime );
    virtual double SAL_CALL getMediaTime();
    virtual void SAL_CALL setStopTime( double fTime );
    virtual double SAL_CALL getStopTime();
    virtual void SAL_CALL setRate( double fRate );
    virtual double SAL_CALL getRate();
    virtual void SAL_CALL setPlaybackLoop( sal_Bool bSet );
    virtual sal_Bool SAL_CALL isPlaybackLoop();
    virtual void SAL_CALL setMute( sal_Bool bSet );
    virtual sal_Bool SAL_CALL isMute();
    virtual void SAL_CALL setVolumeDB( sal_Int16 nVolumeDB );
    virtual sal_Int16 SAL_CALL getVolumeDB();
    virtual ::com::sun::star::awt::Size SAL_CALL getPreferredPlayerWindowSize(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::media::XPlayerWindow > SAL_CALL createPlayerWindow( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::media::XFrameGrabber > SAL_CALL createFrameGrabber(  );
    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

    AVPlayer* getAVPlayer() const { return mpPlayer; }
    virtual bool handleObservation( NSString* pKeyPath );

private:
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > mxContext;

    AVPlayer*           mpPlayer;

    float               mfUnmutedVolume;
    double              mfStopTime;

    bool                mbMuted;
    bool                mbLooping;
};

} // namespace macavf
} // namespace avmedia

#endif // MACAVF_PLAYER_HXX
