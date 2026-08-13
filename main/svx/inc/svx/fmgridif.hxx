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


#ifndef _SVX_FMGRIDIF_HXX
#define _SVX_FMGRIDIF_HXX

#include "svx/svxdllapi.h"

#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XContainer.hpp>
#include <com/sun/star/container/XContainerListener.hpp>
#include <com/sun/star/sdbc/XRowSetListener.hpp>
#include <com/sun/star/sdb/XRowSetSupplier.hpp>
#include <com/sun/star/form/XReset.hpp>
#include <com/sun/star/form/XBoundComponent.hpp>
#include <com/sun/star/form/XLoadListener.hpp>
#include <com/sun/star/form/XGridControl.hpp>
#include <com/sun/star/form/XGridPeer.hpp>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XStatusListener.hpp>
#include <com/sun/star/frame/XDispatchProviderInterception.hpp>
#include <com/sun/star/view/XSelectionChangeListener.hpp>
#include <com/sun/star/util/XModeSelector.hpp>
#include <com/sun/star/util/XModifyListener.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>

#include <tools/wintypes.hxx>
#include <toolkit/controls/unocontrol.hxx>
#include <toolkit/awt/vclxwindow.hxx>
#include <comphelper/uno3.hxx>
#include <cppuhelper/implbase10.hxx>

#if ! defined(INCLUDED_COMPHELPER_IMPLBASE_VAR_HXX_19)
#define INCLUDED_COMPHELPER_IMPLBASE_VAR_HXX_19
#define COMPHELPER_IMPLBASE_INTERFACE_NUMBER 19
#include <comphelper/implbase_var.hxx>
#endif


class DbGridColumn;

class OWeakSubObject : public ::cppu::OWeakObject
{
protected:
	::cppu::OWeakObject&	m_rParent;

public:
	OWeakSubObject(::cppu::OWeakObject& rParent) : m_rParent(rParent) { }

	virtual void SAL_CALL acquire() throw() { m_rParent.acquire(); }
	virtual void SAL_CALL release() throw() { m_rParent.release(); }
};

//==================================================================
// FmXModifyMultiplexer
//==================================================================
class FmXModifyMultiplexer	:public OWeakSubObject
							,public ::cppu::OInterfaceContainerHelper
							,public ::com::sun::star::util::XModifyListener
{
public:
	FmXModifyMultiplexer( ::cppu::OWeakObject& rSource, ::osl::Mutex& rMutex );
	DECLARE_UNO3_DEFAULTS(FmXModifyMultiplexer,OWeakSubObject);
	virtual ::com::sun::star::uno::Any	SAL_CALL queryInterface(const ::com::sun::star::uno::Type& _rType);

// ::com::sun::star::lang::XEventListener
	virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source);

// ::com::sun::star::util::XModifyListener
	virtual void SAL_CALL modified(const ::com::sun::star::lang::EventObject& Source);

// resolve ambiguity : both OWeakObject and OInterfaceContainerHelper have these memory operators
	void * SAL_CALL operator new( size_t size ) throw() { return OWeakSubObject::operator new(size); }
	void SAL_CALL operator delete( void * p ) throw() { OWeakSubObject::operator delete(p); }
};

//==================================================================
// FmXUpdateMultiplexer
//==================================================================
class FmXUpdateMultiplexer : public OWeakSubObject,
							 public ::cppu::OInterfaceContainerHelper,
							 public ::com::sun::star::form::XUpdateListener
{
public:
	FmXUpdateMultiplexer( ::cppu::OWeakObject& rSource, ::osl::Mutex& rMutex );
	DECLARE_UNO3_DEFAULTS(FmXUpdateMultiplexer,OWeakSubObject);

	virtual ::com::sun::star::uno::Any	SAL_CALL queryInterface(const ::com::sun::star::uno::Type& _rType);

// ::com::sun::star::lang::XEventListener
	virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source);

// ::com::sun::star::form::XUpdateListener
	virtual sal_Bool SAL_CALL approveUpdate(const ::com::sun::star::lang::EventObject &);
	virtual void SAL_CALL updated(const ::com::sun::star::lang::EventObject &);

// resolve ambiguity : both OWeakObject and OInterfaceContainerHelper have these memory operators
	void * SAL_CALL operator new( size_t size ) throw() { return OWeakSubObject::operator new(size); }
	void SAL_CALL operator delete( void * p ) throw() { OWeakSubObject::operator delete(p); }
};

//==================================================================
// FmXSelectionMultiplexer
//==================================================================
class FmXSelectionMultiplexer	:public OWeakSubObject
								,public ::cppu::OInterfaceContainerHelper
								,public ::com::sun::star::view::XSelectionChangeListener
{
public:
	FmXSelectionMultiplexer( ::cppu::OWeakObject& rSource, ::osl::Mutex& rMutex );
	DECLARE_UNO3_DEFAULTS(FmXSelectionMultiplexer, OWeakSubObject);

	virtual ::com::sun::star::uno::Any	SAL_CALL queryInterface(const ::com::sun::star::uno::Type& _rType);

// ::com::sun::star::lang::XEventListener
	virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source);

// ::com::sun::star::view::XSelectionChangeListener
    virtual void SAL_CALL selectionChanged( const ::com::sun::star::lang::EventObject& aEvent );

// resolve ambiguity : both OWeakObject and OInterfaceContainerHelper have these memory operators
	void * SAL_CALL operator new( size_t size ) throw() { return OWeakSubObject::operator new(size); }
	void SAL_CALL operator delete( void * p ) throw() { OWeakSubObject::operator delete(p); }
};

//==================================================================
// FmXGridControlMultiplexer
//==================================================================
class FmXGridControlMultiplexer	:public OWeakSubObject
								,public ::cppu::OInterfaceContainerHelper
								,public ::com::sun::star::form::XGridControlListener
{
public:
	FmXGridControlMultiplexer( ::cppu::OWeakObject& rSource, ::osl::Mutex& rMutex );
	DECLARE_UNO3_DEFAULTS( FmXGridControlMultiplexer, OWeakSubObject );

	virtual ::com::sun::star::uno::Any	SAL_CALL queryInterface(const ::com::sun::star::uno::Type& _rType);

// ::com::sun::star::lang::XEventListener
	virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source);

// ::com::sun::star::view::XSelectionChangeListener
    virtual void SAL_CALL columnChanged( const ::com::sun::star::lang::EventObject& _event );

// resolve ambiguity : both OWeakObject and OInterfaceContainerHelper have these memory operators
	void * SAL_CALL operator new( size_t size ) throw() { return OWeakSubObject::operator new(size); }
	void SAL_CALL operator delete( void * p ) throw() { OWeakSubObject::operator delete(p); }
};

//==================================================================
// FmXContainerMultiplexer
//==================================================================
class FmXContainerMultiplexer : public OWeakSubObject,
								public ::cppu::OInterfaceContainerHelper,
								public ::com::sun::star::container::XContainerListener
{
public:
	FmXContainerMultiplexer( ::cppu::OWeakObject& rSource, ::osl::Mutex& rMutex);
	DECLARE_UNO3_DEFAULTS(FmXContainerMultiplexer,OWeakSubObject);
	virtual ::com::sun::star::uno::Any	SAL_CALL queryInterface(const ::com::sun::star::uno::Type& _rType);

// ::com::sun::star::lang::XEventListener
	virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source);

// ::com::sun::star::container::XContainerListener
	virtual void SAL_CALL elementInserted(const ::com::sun::star::container::ContainerEvent& Event);
	virtual void SAL_CALL elementRemoved(const ::com::sun::star::container::ContainerEvent& Event);
	virtual void SAL_CALL elementReplaced(const ::com::sun::star::container::ContainerEvent& Event);

// resolve ambiguity : both OWeakObject and OInterfaceContainerHelper have these memory operators
	void * SAL_CALL operator new( size_t size ) throw() { return OWeakSubObject::operator new(size); }
	void SAL_CALL operator delete( void * p ) throw() { OWeakSubObject::operator delete(p); }
};

//==================================================================
// FmXGridControl
//==================================================================
typedef ::cppu::ImplHelper10<   ::com::sun::star::form::XBoundComponent,
								::com::sun::star::form::XGridControl,
								::com::sun::star::util::XModifyBroadcaster,
								::com::sun::star::container::XIndexAccess,
								::com::sun::star::container::XEnumerationAccess,
								::com::sun::star::util::XModeSelector,
								::com::sun::star::container::XContainer,
								::com::sun::star::frame::XDispatchProvider,
								::com::sun::star::frame::XDispatchProviderInterception,
								::com::sun::star::view::XSelectionSupplier
							>	FmXGridControl_BASE;

class FmXGridPeer;
class SVX_DLLPUBLIC FmXGridControl	:public UnoControl
						,public FmXGridControl_BASE
{
    FmXModifyMultiplexer	    m_aModifyListeners;
    FmXUpdateMultiplexer	    m_aUpdateListeners;
    FmXContainerMultiplexer	    m_aContainerListeners;
    FmXSelectionMultiplexer	    m_aSelectionListeners;
    FmXGridControlMultiplexer   m_aGridControlListeners;

protected:
	sal_uInt16	m_nPeerCreationLevel;
	sal_Bool	m_bInDraw;

	::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >	m_xServiceFactory;

public:
	FmXGridControl(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&);
	virtual ~FmXGridControl();

	// UNO Anbindung
	DECLARE_UNO3_AGG_DEFAULTS(FmXGridControl, UnoControl);
	virtual ::com::sun::star::uno::Any	SAL_CALL queryAggregation(const ::com::sun::star::uno::Type& _rType);

// XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type> SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence<sal_Int8> SAL_CALL getImplementationId(  );

// ::com::sun::star::lang::XComponent
	virtual void SAL_CALL dispose();

// ::com::sun::star::lang::XServiceInfo
	virtual sal_Bool SAL_CALL supportsService(const ::rtl::OUString& ServiceName) throw();
	virtual ::rtl::OUString	SAL_CALL getImplementationName() throw();
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() throw();

// ::com::sun::star::awt::XControl
	virtual void SAL_CALL createPeer(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XToolkit >& _rToolkit, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer >& Parent);
	virtual sal_Bool SAL_CALL setModel(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >& Model);
	virtual void SAL_CALL setDesignMode(sal_Bool bOn);

// ::com::sun::star::awt::XView
	virtual void SAL_CALL draw( sal_Int32 x, sal_Int32 y );

// ::com::sun::star::form::XBoundComponent
	virtual void SAL_CALL addUpdateListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XUpdateListener >& l);
	virtual void SAL_CALL removeUpdateListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XUpdateListener >& l);
	virtual sal_Bool SAL_CALL commit();

// ::com::sun::star::container::XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
    virtual sal_Bool SAL_CALL hasElements(  );

// ::com::sun::star::container::XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration >  SAL_CALL createEnumeration();

// ::com::sun::star::container::XIndexAccess
	virtual sal_Int32 SAL_CALL getCount();
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex(sal_Int32 _rIndex);

// ::com::sun::star::form::XGridControl
    virtual void SAL_CALL addGridControlListener( const ::com::sun::star::uno::Reference< ::com::sun::star::form::XGridControlListener >& _listener );
    virtual void SAL_CALL removeGridControlListener( const ::com::sun::star::uno::Reference< ::com::sun::star::form::XGridControlListener >& _listener );

// ::com::sun::star::form::XGrid (base of XGridControl)
	virtual sal_Int16 SAL_CALL getCurrentColumnPosition();
	virtual void SAL_CALL setCurrentColumnPosition(sal_Int16 nPos);

// ::com::sun::star::form::XGridFieldDataSupplier (base of XGridControl)
    virtual ::com::sun::star::uno::Sequence< sal_Bool > SAL_CALL queryFieldDataType( const ::com::sun::star::uno::Type& xType );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL queryFieldData( sal_Int32 nRow, const ::com::sun::star::uno::Type& xType );

// UnoControl
	virtual ::rtl::OUString	GetComponentServiceName();

// ::com::sun::star::util::XModifyBroadcaster
	virtual void SAL_CALL addModifyListener(const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& l);
	virtual void SAL_CALL removeModifyListener(const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& l);

// ::com::sun::star::util::XModeSelector
	virtual void SAL_CALL setMode(const ::rtl::OUString& Mode);
	virtual ::rtl::OUString SAL_CALL getMode();
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL getSupportedModes();
	virtual sal_Bool SAL_CALL supportsMode(const ::rtl::OUString& Mode);

// ::com::sun::star::container::XContainer
	virtual void SAL_CALL addContainerListener(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener >& l);
	virtual void SAL_CALL removeContainerListener(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener >& l);

// ::com::sun::star::frame::XDispatchProvider
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >  SAL_CALL queryDispatch(const ::com::sun::star::util::URL& aURL, const ::rtl::OUString& aTargetFrameName, sal_Int32 nSearchFlags);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >  > SAL_CALL queryDispatches(const ::com::sun::star::uno::Sequence< ::com::sun::star::frame::DispatchDescriptor >& aDescripts);

// ::com::sun::star::frame::XDispatchProviderInterception
    virtual void SAL_CALL registerDispatchProviderInterceptor(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterceptor >& xInterceptor);
    virtual void SAL_CALL releaseDispatchProviderInterceptor(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterceptor >& xInterceptor);

// ::com::sun::star::view::XSelectionSupplier
    virtual sal_Bool SAL_CALL select( const ::com::sun::star::uno::Any& aSelection );
    virtual ::com::sun::star::uno::Any SAL_CALL getSelection(  );
    virtual void SAL_CALL addSelectionChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::view::XSelectionChangeListener >& xListener );
    virtual void SAL_CALL removeSelectionChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::view::XSelectionChangeListener >& xListener );

protected:
	virtual FmXGridPeer*	imp_CreatePeer(Window* pParent);
		// ImplCreatePeer waere besser ;) geht aber nicht, da dann nicht exportiert

};

//==================================================================
// FmXGridPeer -> Peer fuers Gridcontrol
//==================================================================
typedef ::comphelper::ImplHelper19  <   ::com::sun::star::form::XGridPeer,
                                        ::com::sun::star::form::XBoundComponent,
                                        ::com::sun::star::form::XGridControl,
                                        ::com::sun::star::sdb::XRowSetSupplier,
                                        ::com::sun::star::util::XModifyBroadcaster,
                                        ::com::sun::star::beans::XPropertyChangeListener,
                                        ::com::sun::star::container::XContainerListener,
                                        ::com::sun::star::sdbc::XRowSetListener,
                                        ::com::sun::star::form::XLoadListener,
                                        ::com::sun::star::view::XSelectionChangeListener,
                                        ::com::sun::star::container::XIndexAccess,
                                        ::com::sun::star::container::XEnumerationAccess,
                                        ::com::sun::star::util::XModeSelector,
                                        ::com::sun::star::container::XContainer,
                                        ::com::sun::star::frame::XStatusListener,
                                        ::com::sun::star::frame::XDispatchProvider,
                                        ::com::sun::star::frame::XDispatchProviderInterception,
                                        ::com::sun::star::form::XResetListener,
                                        ::com::sun::star::view::XSelectionSupplier
                                    >	FmXGridPeer_BASE;
class FmGridControl;
class SVX_DLLPUBLIC FmXGridPeer	:public VCLXWindow
					,public FmXGridPeer_BASE
{
	::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer >	m_xColumns;
	::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet > 				m_xCursor;
	::cppu::OInterfaceContainerHelper		m_aModifyListeners,
											m_aUpdateListeners,
											m_aContainerListeners,
											m_aSelectionListeners,
                                            m_aGridControlListeners;

	::rtl::OUString			m_aMode;
	sal_Int32				m_nCursorListening;

	::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterceptor >	m_xFirstDispatchInterceptor;

	sal_Bool								m_bInterceptingDispatch;

	sal_Bool*								m_pStateCache;
		// one bool for each supported url
	::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > *						m_pDispatchers;
		// one dispatcher for each supported url
		// (I would like to have a vector here but including the stl in an exported file seems
		// very risky to me ....)

    class GridListenerDelegator;
    friend class GridListenerDelegator;
    GridListenerDelegator*  m_pGridListener;

protected:
	::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >	m_xServiceFactory;
	::osl::Mutex																		m_aMutex;

public:
	FmXGridPeer(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&);
	~FmXGridPeer();

	// spaeter Constructor, immer nach dem realen Constructor zu rufen !
	void Create(Window* pParent, WinBits nStyle);

// UNO Anbindung
	DECLARE_UNO3_DEFAULTS(FmXGridPeer, VCLXWindow);
	virtual ::com::sun::star::uno::Any	SAL_CALL queryInterface(const ::com::sun::star::uno::Type& _rType);

// XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type> SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence<sal_Int8> SAL_CALL getImplementationId(  );

// ::com::sun::star::lang::XUnoTunnel
	static const ::com::sun::star::uno::Sequence< sal_Int8 >&	getUnoTunnelImplementationId() throw();
	static FmXGridPeer*											getImplementation( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxIFace ) throw();
	sal_Int64													SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& _rIdentifier );

// ::com::sun::star::form::XGridPeer
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer > SAL_CALL getColumns(  );
    virtual void SAL_CALL setColumns( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer >& aColumns );

// ::com::sun::star::lang::XComponent
    virtual void SAL_CALL dispose();

// ::com::sun::star::lang::XEventListener
	virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source);

// ::com::sun::star::form::XBoundComponent
	virtual void SAL_CALL addUpdateListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XUpdateListener >& l);
	virtual void SAL_CALL removeUpdateListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XUpdateListener >& l);
	virtual sal_Bool SAL_CALL commit();

// ::com::sun::star::container::XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
    virtual sal_Bool SAL_CALL hasElements(  );

// ::com::sun::star::container::XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL createEnumeration();

// ::com::sun::star::container::XIndexAccess
	virtual sal_Int32 SAL_CALL getCount();
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex(sal_Int32 _rIndex);

// ::com::sun::star::beans::XPropertyChangeListener
	virtual void SAL_CALL SAL_CALL propertyChange(const ::com::sun::star::beans::PropertyChangeEvent& evt);

// ::com::sun::star::form::XLoadListener
	virtual void SAL_CALL loaded(const ::com::sun::star::lang::EventObject& rEvent);
	virtual void SAL_CALL unloaded(const ::com::sun::star::lang::EventObject& rEvent);
    virtual void SAL_CALL unloading(const ::com::sun::star::lang::EventObject& aEvent);
	virtual void SAL_CALL reloading(const ::com::sun::star::lang::EventObject& aEvent);
    virtual void SAL_CALL reloaded(const ::com::sun::star::lang::EventObject& aEvent);

// ::com::sun::star::sdbc::XRowSetListener
    virtual void SAL_CALL cursorMoved(const ::com::sun::star::lang::EventObject& event);
    virtual void SAL_CALL rowChanged(const ::com::sun::star::lang::EventObject& event);
    virtual void SAL_CALL rowSetChanged(const ::com::sun::star::lang::EventObject& event);

// ::com::sun::star::container::XContainerListener
	virtual void SAL_CALL elementInserted(const ::com::sun::star::container::ContainerEvent& Event);
	virtual void SAL_CALL elementRemoved(const ::com::sun::star::container::ContainerEvent& Event);
	virtual void SAL_CALL elementReplaced(const ::com::sun::star::container::ContainerEvent& Event);

// VCLXWindow
    virtual void SAL_CALL setProperty( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Any& Value );
    virtual ::com::sun::star::uno::Any SAL_CALL getProperty( const ::rtl::OUString& PropertyName );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext >
					CreateAccessibleContext();

// ::com::sun::star::form::XGridControl
    virtual void SAL_CALL addGridControlListener( const ::com::sun::star::uno::Reference< ::com::sun::star::form::XGridControlListener >& _listener );
    virtual void SAL_CALL removeGridControlListener( const ::com::sun::star::uno::Reference< ::com::sun::star::form::XGridControlListener >& _listener );

// ::com::sun::star::form::XGrid (base of XGridControl)
	virtual sal_Int16 SAL_CALL getCurrentColumnPosition();
	virtual void SAL_CALL setCurrentColumnPosition(sal_Int16 nPos);

// ::com::sun::star::form::XGridFieldDataSupplier (base of XGridControl)
    virtual ::com::sun::star::uno::Sequence< sal_Bool > SAL_CALL queryFieldDataType( const ::com::sun::star::uno::Type& xType );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL queryFieldData( sal_Int32 nRow, const ::com::sun::star::uno::Type& xType );

// ::com::sun::star::sdb::XRowSetSupplier
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet >  SAL_CALL getRowSet();
    virtual void SAL_CALL setRowSet(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet >& xDataSource);

// ::com::sun::star::util::XModifyBroadcaster
	virtual void SAL_CALL addModifyListener(const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& l);
	virtual void SAL_CALL removeModifyListener(const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& l);

// UnoControl
	virtual void SAL_CALL SAL_CALL setDesignMode(sal_Bool bOn);
	virtual sal_Bool SAL_CALL isDesignMode();

// ::com::sun::star::view::XSelectionChangeListener
	virtual void SAL_CALL selectionChanged(const ::com::sun::star::lang::EventObject& aEvent);

	void CellModified();

// PropertyListening
	void updateGrid(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet >& _rDatabaseCursor);
	void startCursorListening();
	void stopCursorListening();

// ::com::sun::star::util::XModeSelector
	virtual void SAL_CALL setMode(const ::rtl::OUString& Mode);
	virtual ::rtl::OUString SAL_CALL getMode();
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedModes();
	virtual sal_Bool SAL_CALL supportsMode(const ::rtl::OUString& Mode);

// ::com::sun::star::container::XContainer
	virtual void SAL_CALL addContainerListener(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener >& l);
	virtual void SAL_CALL removeContainerListener(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener >& l);

	void columnVisible(DbGridColumn* pColumn);
	void columnHidden(DbGridColumn* pColumn);

// ::com::sun::star::awt::XView
	virtual void SAL_CALL draw( sal_Int32 x, sal_Int32 y );

// ::com::sun::star::frame::XDispatchProvider
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >  SAL_CALL queryDispatch(const ::com::sun::star::util::URL& aURL, const ::rtl::OUString& aTargetFrameName, sal_Int32 nSearchFlags);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >  > SAL_CALL queryDispatches(const ::com::sun::star::uno::Sequence< ::com::sun::star::frame::DispatchDescriptor >& aDescripts);

// ::com::sun::star::frame::XDispatchProviderInterception
    virtual void SAL_CALL registerDispatchProviderInterceptor(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterceptor >& xInterceptor);
    virtual void SAL_CALL releaseDispatchProviderInterceptor(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterceptor >& xInterceptor);

// ::com::sun::star::frame::XStatusListener
    virtual void SAL_CALL statusChanged(const ::com::sun::star::frame::FeatureStateEvent& Event);

// ::com::sun::star::form::XResetListener
    virtual sal_Bool SAL_CALL approveReset(const ::com::sun::star::lang::EventObject& rEvent);
    virtual void SAL_CALL resetted(const ::com::sun::star::lang::EventObject& rEvent);

// ::com::sun::star::view::XSelectionSupplier
    virtual sal_Bool SAL_CALL select( const ::com::sun::star::uno::Any& aSelection );
    virtual ::com::sun::star::uno::Any SAL_CALL getSelection(  );
    virtual void SAL_CALL addSelectionChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::view::XSelectionChangeListener >& xListener );
    virtual void SAL_CALL removeSelectionChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::view::XSelectionChangeListener >& xListener );

protected:
	virtual FmGridControl*	imp_CreateControl(Window* pParent, WinBits nStyle);

	static ::com::sun::star::uno::Sequence< ::com::sun::star::util::URL>&		getSupportedURLs();
	static ::com::sun::star::uno::Sequence<sal_uInt16>&	getSupportedGridSlots();
	void	ConnectToDispatcher();
	void	DisConnectFromDispatcher();
	void	UpdateDispatches();	// will connect if not already connected and just update else

	/**	If a derived class wants to listen at some column properties, it doesn't have
		to overload all methods affecting columns (setColumns, elementInserted, elementRemoved ...)
		Instead it may use addColumnListeners and removeColumnListeners which are called in all
		the cases.
	*/
	virtual void addColumnListeners(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& xCol);
	virtual void removeColumnListeners(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& xCol);

	void selectionChanged();
    void columnChanged();

	DECL_LINK(OnQueryGridSlotState, void*);
	DECL_LINK(OnExecuteGridSlot, void*);
};



#endif // _SVX_FMGRID_HXX
