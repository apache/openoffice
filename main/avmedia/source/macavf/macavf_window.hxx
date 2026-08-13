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

#ifndef MACAVF_WINDOW_HXX
#define MACAVF_WINDOW_HXX

#include "macavf_common.hxx"

#include <cppuhelper/interfacecontainer.h>

#include "com/sun/star/media/XPlayerWindow.hdl"

// ---------------
// - MyMediaView -
// ---------------

@interface MyMediaView : NSView
@property (nonatomic, readonly, strong) AVPlayer* player;
@property (nonatomic, readonly, strong) AVPlayerLayer* playerLayer;
@property (nonatomic, retain) NSURL* videoURL;
- (void) play;
@end

namespace avmedia { namespace macavf {

// ---------------
// - Window -
// ---------------

class Player;

class Window
:   public MacAVObserverHandler
,   public ::cppu::WeakImplHelper2 < ::com::sun::star::media::XPlayerWindow,
                                     ::com::sun::star::lang::XServiceInfo >
{
public:

            Window( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& i_rxContext,
                    Player& i_rPlayer,
                    NSView* i_pParentView
                    );
    virtual ~Window();

    bool    create( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );
    void    processGraphEvent();
    void    updatePointer();

    // XPlayerWindow
    virtual void SAL_CALL update(  );
    virtual sal_Bool SAL_CALL setZoomLevel( ::com::sun::star::media::ZoomLevel ZoomLevel );
    virtual ::com::sun::star::media::ZoomLevel SAL_CALL getZoomLevel(  );
    virtual void SAL_CALL setPointerType( sal_Int32 nPointerType );

    // XWindow
    virtual void SAL_CALL setPosSize( sal_Int32 X, sal_Int32 Y, sal_Int32 Width, sal_Int32 Height, sal_Int16 Flags );
    virtual ::com::sun::star::awt::Rectangle SAL_CALL getPosSize(  );
    virtual void SAL_CALL setVisible( sal_Bool Visible );
    virtual void SAL_CALL setEnable( sal_Bool Enable );
    virtual void SAL_CALL setFocus(  );
    virtual void SAL_CALL addWindowListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowListener >& xListener );
    virtual void SAL_CALL removeWindowListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowListener >& xListener );
    virtual void SAL_CALL addFocusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& xListener );
    virtual void SAL_CALL removeFocusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFocusListener >& xListener );
    virtual void SAL_CALL addKeyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XKeyListener >& xListener );
    virtual void SAL_CALL removeKeyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XKeyListener >& xListener );
    virtual void SAL_CALL addMouseListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMouseListener >& xListener );
    virtual void SAL_CALL removeMouseListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMouseListener >& xListener );
    virtual void SAL_CALL addMouseMotionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMouseMotionListener >& xListener );
    virtual void SAL_CALL removeMouseMotionListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XMouseMotionListener >& xListener );
    virtual void SAL_CALL addPaintListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XPaintListener >& xListener );
    virtual void SAL_CALL removePaintListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XPaintListener >& xListener );

    // XComponent
    virtual void SAL_CALL dispose(  );
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

    virtual bool handleObservation( NSString* pKeyPath );

private:

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > mxContext;

    ::osl::Mutex                                maMutex;
    ::cppu::OMultiTypeInterfaceContainerHelper  maListeners;
    ::com::sun::star::media::ZoomLevel          meZoomLevel;
    Player&                                     mrPlayer;
    int                                         mnPointerType;

    NSView*                                     mpView; // parent-view == movie-view
    AVPlayerLayer*                              mpPlayerLayer;

    void                                        ImplLayoutVideoWindow();
};

} // namespace macavf
} // namespace avmedia

#endif // _WINDOW_HXX
