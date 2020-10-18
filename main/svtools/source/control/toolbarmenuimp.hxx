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



#include <vos/mutex.hxx>
#include <vcl/image.hxx>
#include <vcl/menu.hxx>

#include <cppuhelper/compbase4.hxx>
#include <cppuhelper/compbase5.hxx>
#include <comphelper/broadcasthelper.hxx>

#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#include <com/sun/star/accessibility/XAccessibleComponent.hpp>
#include <com/sun/star/accessibility/XAccessibleSelection.hpp>
#include <com/sun/star/accessibility/XAccessibleEventBroadcaster.hpp>
#include <com/sun/star/lang/DisposedException.hpp>

#include <rtl/ref.hxx>

#include <vector>

#include "framestatuslistener.hxx"

#include "svtools/valueset.hxx"

namespace svtools {

struct ToolbarMenu_Impl;
class ToolbarMenu;
class ToolbarMenuEntry;

typedef ::std::vector< ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener > > EventListenerVector;
typedef std::vector< ToolbarMenuEntry * > ToolbarMenuEntryVector;

const int EXTRAITEMHEIGHT = 0; // 4;
const int SEPARATOR_HEIGHT = 4;
const int TITLE_ID = -1;
const int BORDER_X = 0;
const int BORDER_Y = 0;

// --------------------
// - ToolbarMenuEntry -
// --------------------

class ToolbarMenuEntry
{
public:
	ToolbarMenu& mrMenu;

	int	mnEntryId;
	MenuItemBits mnBits;
	Size maSize;

	bool mbHasText;
	bool mbHasImage;
	bool mbChecked;
	bool mbEnabled;

	String maText;
	Image maImage;
	Control* mpControl;
	Rectangle maRect;

	::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > mxAccContext;

public:
	ToolbarMenuEntry( ToolbarMenu& rMenu, int nEntryId, const String& rText, MenuItemBits nBits );
	ToolbarMenuEntry( ToolbarMenu& rMenu, int nEntryId, const Image& rImage, MenuItemBits nBits );
	ToolbarMenuEntry( ToolbarMenu& rMenu, int nEntryId, const Image& rImage, const String& rText, MenuItemBits nBits );
	ToolbarMenuEntry( ToolbarMenu& rMenu, int nEntryId, Control* pControl, MenuItemBits nBits );
	~ToolbarMenuEntry();

	void init( int nEntryId, MenuItemBits nBits );

    const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext >& GetAccessible( bool bCreate = false );

    sal_Int32 getAccessibleChildCount() throw (::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > getAccessibleChild( sal_Int32 index ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
	void selectAccessibleChild( sal_Int32 nChildIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);

	bool HasCheck() const
	{
		return mbChecked || ( mnBits & ( MIB_RADIOCHECK | MIB_CHECKABLE | MIB_AUTOCHECK ) );
	}
};

// ---------------
// - ToolbarMenuAcc -
// ---------------

typedef ::cppu::WeakComponentImplHelper5<
    ::com::sun::star::accessibility::XAccessible,
    ::com::sun::star::accessibility::XAccessibleEventBroadcaster,
    ::com::sun::star::accessibility::XAccessibleContext,
    ::com::sun::star::accessibility::XAccessibleComponent,
    ::com::sun::star::accessibility::XAccessibleSelection >
    ToolbarMenuAccComponentBase;

class ToolbarMenuAcc :
    public ::comphelper::OBaseMutex,
    public ToolbarMenuAccComponentBase
{
public:

    ToolbarMenuAcc( ToolbarMenu_Impl& rParent );
    ~ToolbarMenuAcc();

    void                FireAccessibleEvent( short nEventId, const ::com::sun::star::uno::Any& rOldValue, const ::com::sun::star::uno::Any& rNewValue );
    bool                HasAccessibleListeners() const { return( mxEventListeners.size() > 0 ); }

public:
    // XAccessible
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > SAL_CALL getAccessibleContext(  ) throw (::com::sun::star::uno::RuntimeException);

    // XAccessibleEventBroadcaster
    using cppu::WeakComponentImplHelper5<com::sun::star::accessibility::XAccessible, com::sun::star::accessibility::XAccessibleEventBroadcaster, com::sun::star::accessibility::XAccessibleContext, com::sun::star::accessibility::XAccessibleComponent, com::sun::star::accessibility::XAccessibleSelection>::addEventListener;
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
    using cppu::WeakComponentImplHelper5<com::sun::star::accessibility::XAccessible, com::sun::star::accessibility::XAccessibleEventBroadcaster, com::sun::star::accessibility::XAccessibleContext, com::sun::star::accessibility::XAccessibleComponent, com::sun::star::accessibility::XAccessibleSelection>::removeEventListener;
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);

    // XAccessibleContext
    virtual sal_Int32 SAL_CALL getAccessibleChildCount(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleParent(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getAccessibleIndexInParent(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int16 SAL_CALL getAccessibleRole(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getAccessibleDescription(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getAccessibleName(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL getAccessibleStateSet(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::lang::Locale SAL_CALL getLocale(  ) throw (::com::sun::star::accessibility::IllegalAccessibleComponentStateException, ::com::sun::star::uno::RuntimeException);

    // XAccessibleComponent
    virtual sal_Bool SAL_CALL containsPoint( const ::com::sun::star::awt::Point& aPoint ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::awt::Rectangle SAL_CALL getBounds(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::awt::Point SAL_CALL getLocation(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::awt::Size SAL_CALL getSize(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL grabFocus(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getAccessibleKeyBinding(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getForeground(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getBackground(  ) throw (::com::sun::star::uno::RuntimeException);

    // XAccessibleSelection
    virtual void SAL_CALL selectAccessibleChild( sal_Int32 nChildIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL isAccessibleChildSelected( sal_Int32 nChildIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL clearAccessibleSelection(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL selectAllAccessibleChildren(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getSelectedAccessibleChildCount(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getSelectedAccessibleChild( sal_Int32 nSelectedChildIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL deselectAccessibleChild( sal_Int32 nSelectedChildIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);

	DECL_LINK( WindowEventListener, VclSimpleEvent* );

private:
    EventListenerVector mxEventListeners;
    ToolbarMenu_Impl* mpParent;
    /// The current FOCUSED state.
    bool mbIsFocused;

	void ProcessWindowEvent( const VclWindowEvent& rVclWindowEvent );

    /** Tell all listeners that the object is dying.  This callback is
        usually called from the WeakComponentImplHelper class.
    */
    virtual void SAL_CALL disposing (void);

    /** Check whether or not the object has been disposed (or is in the
        state of being disposed).  If that is the case then
        DisposedException is thrown to inform the (indirect) caller of the
        foul deed.
    */
    void ThrowIfDisposed (void) throw (::com::sun::star::lang::DisposedException);
};

// -----------------------
// - ToolbarMenuEntryAcc -
// -----------------------

typedef ::cppu::WeakComponentImplHelper4< ::com::sun::star::accessibility::XAccessible,
                                                     ::com::sun::star::accessibility::XAccessibleEventBroadcaster,
                                                     ::com::sun::star::accessibility::XAccessibleContext,
                                                     ::com::sun::star::accessibility::XAccessibleComponent > ToolbarMenuEntryAccBase;

class ToolbarMenuEntryAcc : public ::comphelper::OBaseMutex,
							public ToolbarMenuEntryAccBase
{
public:
    ToolbarMenuEntryAcc( ToolbarMenuEntry* pParent );
    ~ToolbarMenuEntryAcc();

    void    FireAccessibleEvent( short nEventId, const ::com::sun::star::uno::Any& rOldValue, const ::com::sun::star::uno::Any& rNewValue );
    bool    HasAccessibleListeners() const { return( mxEventListeners.size() > 0 ); }

    // XAccessible
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext > SAL_CALL getAccessibleContext(  ) throw (::com::sun::star::uno::RuntimeException);

    // XAccessibleEventBroadcaster
    using ToolbarMenuEntryAccBase::addEventListener;
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);
    using ToolbarMenuEntryAccBase::removeEventListener;
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleEventListener >& xListener ) throw (::com::sun::star::uno::RuntimeException);

    // XAccessibleContext
    virtual sal_Int32 SAL_CALL getAccessibleChildCount(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleChild( sal_Int32 i ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleParent(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getAccessibleIndexInParent(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int16 SAL_CALL getAccessibleRole(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getAccessibleDescription(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getAccessibleName(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleRelationSet > SAL_CALL getAccessibleRelationSet(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleStateSet > SAL_CALL getAccessibleStateSet(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::lang::Locale SAL_CALL getLocale(  ) throw (::com::sun::star::accessibility::IllegalAccessibleComponentStateException, ::com::sun::star::uno::RuntimeException);

    // XAccessibleComponent
    virtual sal_Bool SAL_CALL containsPoint( const ::com::sun::star::awt::Point& aPoint ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::awt::Rectangle SAL_CALL getBounds(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::awt::Point SAL_CALL getLocation(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::awt::Size SAL_CALL getSize(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL grabFocus(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getAccessibleKeyBinding(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getForeground(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getBackground(  ) throw (::com::sun::star::uno::RuntimeException);

private:
    EventListenerVector	   mxEventListeners;
    ::vos::OMutex		   maMutex;
    ToolbarMenuEntry*      mpParent;

	/** Tell all listeners that the object is dying.  This callback is
        usually called from the WeakComponentImplHelper class.
    */
    virtual void SAL_CALL disposing (void);
};

// -----------------------------------------------------------------------------

struct ToolbarMenu_Impl
{
	ToolbarMenu& mrMenu;

	::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >              mxFrame;
	rtl::Reference< svt::FrameStatusListener >										 mxStatusListener;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > mxServiceManager;
	rtl::Reference< ToolbarMenuAcc >												 mxAccessible;
	::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > mxOldSelection;

	ToolbarMenuEntryVector	maEntryVector;

	int mnCheckPos;
    int mnImagePos;
    int mnTextPos;

	int mnHighlightedEntry;
	int mnSelectedEntry;
	int mnLastColumn;

	Size maSize;

	Link			maSelectHdl;

	ToolbarMenu_Impl( ToolbarMenu& rMenu, const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame );
	~ToolbarMenu_Impl();

	void setAccessible( ToolbarMenuAcc* pAccessible );

	void fireAccessibleEvent( short nEventId, const ::com::sun::star::uno::Any& rOldValue, const ::com::sun::star::uno::Any& rNewValue );
	bool hasAccessibleListeners();

	sal_Int32 getAccessibleChildCount() throw (::com::sun::star::uno::RuntimeException);
	::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > getAccessibleChild( sal_Int32 index ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
	::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > getAccessibleChild( Control* pControl, sal_Int32 childIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);

	void selectAccessibleChild( sal_Int32 nChildIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
	sal_Bool isAccessibleChildSelected( sal_Int32 nChildIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
	void clearAccessibleSelection();

	ToolbarMenuEntry* implGetEntry( int nEntry ) const;
	void notifyHighlightedEntry();

	void implHighlightControl( sal_uInt16 nCode, Control* pControl );
};

}
