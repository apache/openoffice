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

#ifndef _SD_SlideShowImpl_HXX_
#define _SD_SlideShowImpl_HXX_

#include "sal/config.h"
#include "com/sun/star/uno/XComponentContext.hpp"
#include "cppuhelper/compbase1.hxx"
#include "cppuhelper/compbase2.hxx"
#include "cppuhelper/basemutex.hxx"
#include "cppuhelper/propertysetmixin.hxx"
#include <com/sun/star/awt/XActivateListener.hpp>
#include <com/sun/star/presentation/XSlideShow.hpp>
#include <com/sun/star/presentation/XSlideShowView.hpp>
#include <com/sun/star/presentation/XSlideShowListener.hpp>
#include <com/sun/star/presentation/XSlideShowController.hpp>
#include "com/sun/star/presentation/XShapeEventListener.hpp"
#include <com/sun/star/awt/WindowEvent.hpp>
#include <com/sun/star/awt/XWindowListener.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/util/XModifyListener.hpp>
#include <com/sun/star/awt/XPaintListener.hpp>
#include <com/sun/star/awt/XPointer.hpp>
#include <com/sun/star/drawing/XDrawPagesSupplier.hpp>
#include <com/sun/star/animations/XAnimationNodeSupplier.hpp>
#include <com/sun/star/presentation/ClickAction.hpp>
#include <com/sun/star/media/XManager.hpp>
#include <com/sun/star/media/XPlayer.hpp>

#include <toolkit/helper/vclunohelper.hxx>

#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/tools/canvastools.hxx>
#include <vcl/help.hxx>
#include <tools/urlobj.hxx>
#include <unotools/pathoptions.hxx>
#include <unotools/saveopt.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/viewfrm.hxx>
#include <basic/sbstar.hxx>
#include <svx/svdpagv.hxx>
#include <svx/fmshell.hxx>

#ifndef _SVX_SVXIDS_HRC
#include <svx/svxids.hrc>
#endif
#include "sdmod.hxx"
#include "cusshow.hxx"
#include "ViewShellBase.hxx"
#include "PresentationViewShell.hxx"
#include "ViewShell.hxx"
#include "drawview.hxx"
#include "drawdoc.hxx"

#include "showwindow.hxx"

#include "optsitem.hxx"
#include "FrameView.hxx"
#include "DrawDocShell.hxx"

#ifndef _SD_APP_HRC_
#include "app.hrc"
#endif

#include "slideshow.hxx"

class SfxViewFrame;
class SfxRequest;

namespace css = ::com::sun::star;

namespace sd
{
class SlideShowView;
class AnimationSlideController;
class PaneHider;

// --------------------------------------------------------------------

struct PresentationSettingsEx : public PresentationSettings
{
	sal_Bool mbRehearseTimings;
	sal_Bool mbPreview;
	::Window* mpParentWindow;
	css::uno::Reference< css::drawing::XDrawPage > mxStartPage;
	css::uno::Reference< css::animations::XAnimationNode > mxAnimationNode;

	PresentationSettingsEx( const PresentationSettingsEx& );
	PresentationSettingsEx( PresentationSettings& );

	void SetArguments( const css::uno::Sequence< css::beans::PropertyValue >& rArguments );

	void SetPropertyValue( const ::rtl::OUString& rProperty, const css::uno::Any& rValue );
};

// --------------------------------------------------------------------

struct WrappedShapeEventImpl
{
	css::presentation::ClickAction meClickAction;
	sal_Int32 mnVerb;
	::rtl::OUString maStrBookmark;
	WrappedShapeEventImpl() : meClickAction( css::presentation::ClickAction_NONE ), mnVerb( 0 ) {};
};

typedef boost::shared_ptr< WrappedShapeEventImpl > WrappedShapeEventImplPtr;
typedef std::map< css::uno::Reference< css::drawing::XShape >, WrappedShapeEventImplPtr > WrappedShapeEventImplMap;

// --------------------------------------------------------------------

class SlideShowListenerProxy : private ::cppu::BaseMutex,
		public ::cppu::WeakImplHelper2< css::presentation::XSlideShowListener, css::presentation::XShapeEventListener >
{
public:
	SlideShowListenerProxy( const rtl::Reference< SlideshowImpl >& xController, const css::uno::Reference< css::presentation::XSlideShow >& xSlideShow );
	virtual ~SlideShowListenerProxy();

	void addAsSlideShowListener();
	void removeAsSlideShowListener();

	void addSlideShowListener( const css::uno::Reference< css::presentation::XSlideShowListener >& Listener );
	void removeSlideShowListener( const css::uno::Reference< css::presentation::XSlideShowListener >& Listener );

	void addShapeEventListener( const css::uno::Reference< css::drawing::XShape >& xShape );
	void removeShapeEventListener( const css::uno::Reference< css::drawing::XShape >& xShape );

	// css::animations::XAnimationListener
	virtual void SAL_CALL beginEvent( const css::uno::Reference< css::animations::XAnimationNode >& Node );
	virtual void SAL_CALL endEvent( const css::uno::Reference< css::animations::XAnimationNode >& Node );
	virtual void SAL_CALL repeat( const css::uno::Reference< css::animations::XAnimationNode >& Node, ::sal_Int32 Repeat );

	// css::presentation::XSlideShowListener:
	virtual void SAL_CALL paused();
	virtual void SAL_CALL resumed();
	virtual void SAL_CALL slideTransitionStarted();
	virtual void SAL_CALL slideTransitionEnded();
	virtual void SAL_CALL slideAnimationsEnded();
	virtual void SAL_CALL slideEnded(sal_Bool bReverse);
	virtual void SAL_CALL hyperLinkClicked(const ::rtl::OUString & hyperLink);

	// css::lang::XEventListener:
	virtual void SAL_CALL disposing(const css::lang::EventObject & Source);

	// css::presentation::XShapeEventListener:
	virtual void SAL_CALL click(const css::uno::Reference< css::drawing::XShape > & xShape, const css::awt::MouseEvent & aOriginalEvent);

	::cppu::OInterfaceContainerHelper maListeners;

	rtl::Reference< SlideshowImpl > mxController;
	css::uno::Reference< css::presentation::XSlideShow > mxSlideShow;
};

// --------------------------------------------------------------------

typedef ::cppu::WeakComponentImplHelper2< css::presentation::XSlideShowController, css::container::XIndexAccess > SlideshowImplBase;

class SlideshowImpl : private ::cppu::BaseMutex, public SlideshowImplBase
{
friend class SlideShow;
friend class SlideShowView;

public:
	explicit SlideshowImpl( const css::uno::Reference< css::presentation::XPresentation2 >& xPresentation, ViewShell* pViewSh, ::sd::View* pView, SdDrawDocument* pDoc, ::Window* pParentWindow);

	// css::presentation::XSlideShowController:
	virtual ::sal_Bool SAL_CALL getAlwaysOnTop();
	virtual void SAL_CALL setAlwaysOnTop( ::sal_Bool _alwaysontop );
	virtual ::sal_Bool SAL_CALL getMouseVisible();
	virtual void SAL_CALL setMouseVisible( ::sal_Bool _mousevisible );
	virtual ::sal_Bool SAL_CALL getUsePen();
	virtual void SAL_CALL setUsePen( ::sal_Bool _usepen );
	virtual ::sal_Int32 SAL_CALL getPenColor();
	virtual void SAL_CALL setPenColor( ::sal_Int32 _pencolor );
	virtual void SAL_CALL setUseEraser( ::sal_Bool _usepen );
	virtual double SAL_CALL getPenWidth();
	virtual void SAL_CALL setPenWidth( double dStrokeWidth );
	virtual void SAL_CALL setEraseAllInk( bool bEraseAllInk );
	virtual void SAL_CALL setEraseInk( sal_Int32 nEraseInkSize );
	virtual void SAL_CALL setPenMode( bool bSwitchPenMode);
	virtual void SAL_CALL setEraserMode( bool bSwitchEraserMode );
	virtual ::sal_Bool SAL_CALL isRunning(  );
	virtual ::sal_Int32 SAL_CALL getSlideCount(  );
	virtual css::uno::Reference< css::drawing::XDrawPage > SAL_CALL getSlideByIndex( ::sal_Int32 Index );
	virtual void SAL_CALL addSlideShowListener( const css::uno::Reference< css::presentation::XSlideShowListener >& Listener );
	virtual void SAL_CALL removeSlideShowListener( const css::uno::Reference< css::presentation::XSlideShowListener >& Listener );
	virtual void SAL_CALL gotoNextEffect(  );
	virtual void SAL_CALL gotoPreviousEffect(  );
	virtual void SAL_CALL gotoFirstSlide(  );
	virtual void SAL_CALL gotoNextSlide(  );
	virtual void SAL_CALL gotoPreviousSlide(  );
	virtual void SAL_CALL gotoLastSlide(  );
	virtual void SAL_CALL gotoBookmark( const ::rtl::OUString& Bookmark );
	virtual void SAL_CALL gotoSlide( const css::uno::Reference< css::drawing::XDrawPage >& Page );
	virtual void SAL_CALL gotoSlideIndex( ::sal_Int32 Index );
	virtual void SAL_CALL stopSound(  );
	virtual void SAL_CALL pause(  );
	virtual void SAL_CALL resume(  );
	virtual ::sal_Bool SAL_CALL isPaused(  );
	virtual void SAL_CALL blankScreen( ::sal_Int32 Color );
	virtual void SAL_CALL activate(  );
	virtual void SAL_CALL deactivate(  );
	virtual ::sal_Bool SAL_CALL isActive(  );
	virtual css::uno::Reference< css::drawing::XDrawPage > SAL_CALL getCurrentSlide(  );
	virtual ::sal_Int32 SAL_CALL getCurrentSlideIndex(  );
	virtual ::sal_Int32 SAL_CALL getNextSlideIndex(  );
	virtual ::sal_Bool SAL_CALL isEndless(  );
	virtual ::sal_Bool SAL_CALL isFullScreen(  );
	virtual css::uno::Reference< css::presentation::XSlideShow > SAL_CALL getSlideShow(  );

	// XIndexAccess
	virtual ::sal_Int32 SAL_CALL getCount(  );
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( ::sal_Int32 Index );
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
	virtual ::sal_Bool SAL_CALL hasElements(  );

	// will be called from the SlideShowListenerProxy when this event is fired from the XSlideShow
	void slideEnded(const bool bReverse);
	void hyperLinkClicked(const ::rtl::OUString & hyperLink);
	void click(const css::uno::Reference< css::drawing::XShape > & xShape, const css::awt::MouseEvent & aOriginalEvent);

	// ends the presentation async
	void endPresentation();

	ViewShell* getViewShell() const { return mpViewShell; }

	void paint( const Rectangle& rRect );
	bool keyInput(const KeyEvent& rKEvt);
	void mouseButtonUp(const MouseEvent& rMEvt);

private:
	SlideshowImpl(SlideshowImpl &); // not defined
	void operator =(SlideshowImpl &); // not defined

	virtual ~SlideshowImpl();

	// overload WeakComponentImplHelperBase::disposing()
	// This function is called upon disposing the component,
	// if your component needs special work when it becomes
	// disposed, do it here.
	virtual void SAL_CALL disposing();

	// internal
	bool startShow( PresentationSettingsEx* pPresSettings );
	bool startPreview(
		const css::uno::Reference< css::drawing::XDrawPage >& xDrawPage,
		const css::uno::Reference< css::animations::XAnimationNode >& xAnimationNode,
		::Window* pParent );

	ShowWindow* getShowWindow() const { return mpShowWindow; }

		/** forces an async call to update in the main thread */
	void startUpdateTimer();

	double update();

	void createSlideList( bool bAll, bool bStartWithActualSlide, const String& rPresSlide );

	void displayCurrentSlide (const bool bSkipAllMainSequenceEffects = false);

	void displaySlideNumber( sal_Int32 nSlide );
	void displaySlideIndex( sal_Int32 nIndex );
	sal_Int32 getCurrentSlideNumber();
	sal_Int32 getFirstSlideNumber();
	sal_Int32 getLastSlideNumber();
	inline bool isInputFreezed() const { return mbInputFreeze; }

	void jumpToBookmark( const String& sBookmark );

	void hideChildWindows();
	void showChildWindows();

	void resize( const Size& rSize );

	void setActiveXToolbarsVisible( sal_Bool bVisible );

	DECL_LINK( updateHdl, Timer* );
	DECL_LINK( PostYieldListener, void* );
	DECL_LINK( ReadyForNextInputHdl, Timer* );
	DECL_LINK( endPresentationHdl, void* );
	DECL_LINK( ContextMenuSelectHdl, Menu * );
	DECL_LINK( ContextMenuHdl, void* );
	DECL_LINK( deactivateHdl, Timer* );
	DECL_LINK( EventListenerHdl, VclSimpleEvent* );

	// helper
	void receiveRequest(SfxRequest& rReq);

	/** called only by the slideshow view when the first paint event occurs.
		This actually starts the slideshow. */
	void onFirstPaint();

	long getRestoreSlide() const { return mnRestoreSlide; }

private:
	bool startShowImpl(
		const css::uno::Sequence< css::beans::PropertyValue >& aProperties );

	SfxViewFrame* getViewFrame() const;
	SfxDispatcher* getDispatcher() const;
	SfxBindings* getBindings() const;

	sal_Int32 getSlideNumberForBookmark( const rtl::OUString& rStrBookmark );

	void removeShapeEvents();
	void registerShapeEvents( sal_Int32 nSlideNumber );
	void registerShapeEvents( css::uno::Reference< css::drawing::XShapes >& xShapes );

	css::uno::Reference< css::presentation::XSlideShow > createSlideShow() const;

	void setAutoSaveState( bool bOn );
	void gotoPreviousSlide (const bool bSkipAllMainSequenceEffects);

	/** Called by PostYieldListener and updateHdl handlers this method is
		responsible to call the slideshow update() method and, depending on
		its return value, wait for a certain amount of time before another
		call to update() is scheduled.
	*/
	sal_Int32 updateSlideShow (void);

	css::uno::Reference< css::presentation::XSlideShow > mxShow;
	comphelper::ImplementationReference< ::sd::SlideShowView, css::presentation::XSlideShowView > mxView;
	css::uno::Reference< css::frame::XModel > mxModel;

	Timer maUpdateTimer;
	Timer maInputFreezeTimer;
	Timer maDeactivateTimer;

	::sd::View* mpView;
	ViewShell* mpViewShell;
	DrawDocShell* mpDocSh;
	SdDrawDocument* mpDoc;

	SfxItemSet*		mpNewAttr;
	::Window*		mpParentWindow;
	ShowWindow*		mpShowWindow;
	PushButton* 	mpTimeButton;

	boost::shared_ptr< AnimationSlideController > mpSlideController;

	long			mnRestoreSlide;
	Point			maSlideOrigin;
	Point			maPopupMousePos;
	Size			maSlideSize;
	Size			maPresSize;
	AnimationMode   meAnimationMode;
	String			maCharBuffer;
	Pointer 		maOldPointer;
	Pointer 		maPencil;
	std::vector< ::sd::Window* > maDrawModeWindows;
	::sd::Window*	mpOldActiveWindow;
	Link			maStarBASICGlobalErrorHdl;
	unsigned long	mnChildMask;
	bool			mbGridVisible;
	bool			mbBordVisible;
	bool			mbSlideBorderVisible;
	bool			mbSetOnlineSpelling;
	bool			mbDisposed;
	bool			mbMouseIsDrawing;
	bool			mbAutoSaveWasOn;
	bool			mbRehearseTimings;
	bool			mbDesignMode;
	bool			mbIsPaused;
	bool			mbWasPaused; // used to cache pause state during context menu
	bool			mbInputFreeze;
	sal_Bool		mbActive;

	PresentationSettings maPresSettings;
	sal_Int32		mnUserPaintColor;

	bool			mbUsePen;
	double			mdUserPaintStrokeWidth;

	// used in updateHdl to prevent recursive calls
	sal_Int32		mnEntryCounter;

	sal_Int32		mnLastSlideNumber;
	WrappedShapeEventImplMap	maShapeEventMap;

	::rtl::OUString msOnClick;
	::rtl::OUString msBookmark;
	::rtl::OUString msVerb;

	css::uno::Reference< css::drawing::XDrawPage > mxPreviewDrawPage;
	css::uno::Reference< css::animations::XAnimationNode > mxPreviewAnimationNode;

	css::uno::Reference< css::media::XPlayer > mxPlayer;

	::std::auto_ptr<PaneHider> mpPaneHider;

	sal_uLong	mnEndShowEvent;
	sal_uLong	mnContextMenuEvent;
	sal_Int32 mnUpdateEvent;

	css::uno::Reference< css::presentation::XPresentation2 > mxPresentation;
	::rtl::Reference< SlideShowListenerProxy > mxListenerProxy;
};

} // namespace ::sd

#endif

/* vim: set noet sw=4 ts=4: */
