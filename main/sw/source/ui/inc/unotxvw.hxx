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


#ifndef _UNOTXVW_HXX
#define _UNOTXVW_HXX
#include <sfx2/sfxbasecontroller.hxx>
#include <com/sun/star/embed/XEmbeddedObject.hpp>
#include <com/sun/star/text/XTextViewCursor.hpp>
#include <com/sun/star/text/XTextViewCursorSupplier.hpp>
#include <com/sun/star/text/XRubySelection.hpp>
#include <com/sun/star/view/XFormLayerAccess.hpp>
#include <com/sun/star/view/XScreenCursor.hpp>
#include <com/sun/star/view/XViewSettingsSupplier.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <com/sun/star/view/XLineCursor.hpp>
#include <com/sun/star/view/XViewCursor.hpp>
#include <com/sun/star/text/XPageCursor.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/datatransfer/XTransferableSupplier.hpp>
#include <cppuhelper/implbase8.hxx>	// helper for implementations
#include <svl/itemprop.hxx>
#include "calbck.hxx"
#include "TextCursorHelper.hxx"
#include <comphelper/uno3.hxx>

#include <sfx2/objsh.hxx>

class SdrObject;
class SwView;

typedef ::com::sun::star::uno::Reference< ::com::sun::star::view::XSelectionChangeListener > * XSelectionChangeListenerPtr;
SV_DECL_PTRARR_DEL( SelectionChangeListenerArr, XSelectionChangeListenerPtr, 4, 4 )

/******************************************************************************
 *
 ******************************************************************************/
class SwXTextView :
	public ::com::sun::star::view::XSelectionSupplier,
	public ::com::sun::star::lang::XServiceInfo,
	public ::com::sun::star::view::XFormLayerAccess,
	public ::com::sun::star::text::XTextViewCursorSupplier,
	public ::com::sun::star::text::XRubySelection,
	public ::com::sun::star::view::XViewSettingsSupplier,
    public ::com::sun::star::beans::XPropertySet,
    public ::com::sun::star::datatransfer::XTransferableSupplier,
	public SfxBaseController
{
	SelectionChangeListenerArr aSelChangedListeners;

    SwView*                     m_pView;
    const SfxItemPropertySet*   m_pPropSet;   // property map for SwXTextView properties
                                        // (not related to pxViewSettings!)

	::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > * 		pxViewSettings;
	::com::sun::star::uno::Reference< ::com::sun::star::text::XTextViewCursor > * 	pxTextViewCursor;


    SdrObject* GetControl(
        const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel > & Model,
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >& xToFill  );

protected:
	virtual ~SwXTextView();
public:
	SwXTextView(SwView* pSwView);


    virtual 	::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType );
    virtual void SAL_CALL acquire(  ) throw();
    virtual void SAL_CALL release(  ) throw();

    //XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

	//XSelectionSupplier
	virtual ::com::sun::star::uno::Any SAL_CALL getSelection(void);
	virtual sal_Bool SAL_CALL select(const ::com::sun::star::uno::Any& aInterface);
	virtual void SAL_CALL addSelectionChangeListener(const ::com::sun::star::uno::Reference< ::com::sun::star::view::XSelectionChangeListener > & xListener);
	virtual void SAL_CALL removeSelectionChangeListener(const ::com::sun::star::uno::Reference< ::com::sun::star::view::XSelectionChangeListener > & xListener);

    // XFormLayerAccess
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormController > SAL_CALL getFormController( const ::com::sun::star::uno::Reference< ::com::sun::star::form::XForm >& Form );
    virtual ::sal_Bool SAL_CALL isFormDesignMode(  );
    virtual void SAL_CALL setFormDesignMode( ::sal_Bool DesignMode );

    // XControlAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >  SAL_CALL getControl(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel > & Model);

	//XTextViewCursorSupplier
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextViewCursor >  SAL_CALL getViewCursor(void);

	//XViewSettings
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >  SAL_CALL getViewSettings(void);

	//XRubySelection
    virtual ::com::sun::star::uno::Sequence<
			::com::sun::star::uno::Sequence<
			::com::sun::star::beans::PropertyValue > > SAL_CALL getRubyList( sal_Bool bAutomatic );

	virtual void SAL_CALL setRubyList(
		const ::com::sun::star::uno::Sequence<
		::com::sun::star::uno::Sequence<
		::com::sun::star::beans::PropertyValue > >& RubyList, sal_Bool bAutomatic );

    //XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

    //XTransferableSupplier
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > SAL_CALL getTransferable(  );
    virtual void SAL_CALL insertTransferable( const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable >& xTrans );

	void					NotifySelChanged();
    void                    NotifyDBChanged();

    SwView*                 GetView() {return m_pView;}
    void                    Invalidate();

    // temporary document used for PDF export of selections/multi-selections
    SfxObjectShellLock      BuildTmpSelectionDoc();
};

/* -----------------17.09.98 12:52-------------------
 *
 * --------------------------------------------------*/

typedef cppu::WeakImplHelper8<
                            ::com::sun::star::text::XTextViewCursor,
							::com::sun::star::lang::XServiceInfo,
							::com::sun::star::text::XPageCursor,
							::com::sun::star::view::XScreenCursor,
							::com::sun::star::view::XViewCursor,
							::com::sun::star::view::XLineCursor,
							::com::sun::star::beans::XPropertySet,
							::com::sun::star::beans::XPropertyState
							> SwXTextViewCursor_Base;

class SwXTextViewCursor : public SwXTextViewCursor_Base,
public SwClient,
public OTextCursorHelper
{
    SwView*                         m_pView;
    const SfxItemPropertySet*       m_pPropSet;
protected:
    sal_Bool    IsTextSelection( sal_Bool bAllowTables = sal_True ) const;
    virtual     ~SwXTextViewCursor();
public:
	SwXTextViewCursor(SwView* pVw);

	DECLARE_XINTERFACE()

	//XTextViewCursor
	virtual sal_Bool SAL_CALL isVisible(void);
	virtual void SAL_CALL setVisible(sal_Bool bVisible);
	virtual ::com::sun::star::awt::Point SAL_CALL getPosition(void);

	//XTextCursor - neu
    virtual void SAL_CALL collapseToStart(  );
    virtual void SAL_CALL collapseToEnd(  );
    virtual sal_Bool SAL_CALL isCollapsed(  );
    virtual sal_Bool SAL_CALL goLeft( sal_Int16 nCount, sal_Bool bExpand );
    virtual sal_Bool SAL_CALL goRight( sal_Int16 nCount, sal_Bool bExpand );
    virtual void SAL_CALL gotoStart( sal_Bool bExpand );
    virtual void SAL_CALL gotoEnd( sal_Bool bExpand );
    virtual void SAL_CALL gotoRange( const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange >& xRange, sal_Bool bExpand );

	//XPageCursor
	virtual sal_Bool SAL_CALL jumpToFirstPage(void);
	virtual sal_Bool SAL_CALL jumpToLastPage(void);
	virtual sal_Bool SAL_CALL jumpToPage(sal_Int16 nPage);
	virtual sal_Bool SAL_CALL jumpToNextPage(void);
	virtual sal_Bool SAL_CALL jumpToPreviousPage(void);
	virtual sal_Bool SAL_CALL jumpToEndOfPage(void);
	virtual sal_Bool SAL_CALL jumpToStartOfPage(void);
	virtual sal_Int16 SAL_CALL getPage(void);

	//XTextRange
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XText >  SAL_CALL getText(void);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL  getStart(void);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL   getEnd(void);
	virtual rtl::OUString SAL_CALL  getString(void);
	virtual void SAL_CALL  setString(const rtl::OUString& aString);

	//XScreenCursor
	virtual sal_Bool SAL_CALL screenDown(void);
	virtual sal_Bool SAL_CALL screenUp(void);

	//XViewCursor
	virtual sal_Bool SAL_CALL goDown(sal_Int16 nCount, sal_Bool bExpand);
    virtual sal_Bool SAL_CALL goUp(sal_Int16 nCount, sal_Bool bExpand);
//    virtual sal_Bool goLeft(sal_Int16 nCount, sal_Bool bExpand) throw( ::com::sun::star::uno::RuntimeException );
//    virtual sal_Bool goRight(sal_Int16 nCount, sal_Bool bExpand) throw( ::com::sun::star::uno::RuntimeException );

	//XLineCursor
    virtual sal_Bool SAL_CALL isAtStartOfLine(void);
    virtual sal_Bool SAL_CALL isAtEndOfLine(void);
    virtual void SAL_CALL gotoEndOfLine(sal_Bool bExpand);
    virtual void SAL_CALL gotoStartOfLine(sal_Bool bExpand);

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

	//XPropertyState
    virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState( const ::rtl::OUString& PropertyName );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL getPropertyStates( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyName );
    virtual void SAL_CALL setPropertyToDefault( const ::rtl::OUString& PropertyName );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault( const ::rtl::OUString& aPropertyName );

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

    static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();

	//XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier );

    void    Invalidate(){m_pView = 0;}

	// ITextCursorHelper
	virtual const SwPaM*		GetPaM() const;
	virtual SwPaM*				GetPaM();
	virtual const SwDoc* 		GetDoc() const;
	virtual SwDoc* 				GetDoc();
};
#endif
