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



#ifndef _FRM_DATABASEFORM_HXX_
#define _FRM_DATABASEFORM_HXX_

#include "propertybaghelper.hxx"
#include <com/sun/star/sdbc/XDataSource.hpp>
#include <com/sun/star/sdb/XSQLErrorListener.hpp>
#include <com/sun/star/io/XPersistObject.hpp>
#include <com/sun/star/sdb/XSQLErrorBroadcaster.hpp>
#include <com/sun/star/form/FormSubmitMethod.hpp>
#include <com/sun/star/form/FormSubmitEncoding.hpp>
#include <com/sun/star/form/XSubmit.hpp>
#include <com/sun/star/form/XReset.hpp>
#include <com/sun/star/form/XDatabaseParameterBroadcaster2.hpp>
#include <com/sun/star/sdb/XCompletedExecution.hpp>
#include <com/sun/star/sdbc/XRowSet.hpp>
#include <com/sun/star/sdbcx/XDeleteRows.hpp>
#include <com/sun/star/sdbc/XResultSetUpdate.hpp>
#include <com/sun/star/sdb/XRowSetApproveListener.hpp>
#include <com/sun/star/sdb/XRowSetApproveBroadcaster.hpp>
#include <com/sun/star/form/NavigationBarMode.hpp>
#include <com/sun/star/form/XLoadable.hpp>
#include <com/sun/star/form/XLoadListener.hpp>
#include <com/sun/star/form/XForm.hpp>
#include <com/sun/star/awt/XTabControllerModel.hpp>
#include <com/sun/star/script/XEventAttacherManager.hpp>
#include <com/sun/star/sdbc/XRowSetListener.hpp>
#include <com/sun/star/sdb/XResultSetAccess.hpp>
#include <com/sun/star/sdbc/XCloseable.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/sdbc/XParameters.hpp>
#include <com/sun/star/sdbc/XConnection.hpp>
#include <com/sun/star/task/XInteractionHandler.hpp>
#include <com/sun/star/beans/XPropertyContainer.hpp>
#include <com/sun/star/beans/XPropertyAccess.hpp>
#include <com/sun/star/sdbc/XWarningsSupplier.hpp>


#include <tools/link.hxx>
#include "InterfaceContainer.hxx"

#include <connectivity/parameters.hxx>
#include <connectivity/filtermanager.hxx>
#include <connectivity/warningscontainer.hxx>

#ifndef FORMS_SOURCE_MISC_LISTENERCONTAINERS_HXX
#include "listenercontainers.hxx"
#endif
#include "ids.hxx"
#include <comphelper/propmultiplex.hxx>
#include <comphelper/uno3.hxx>
#include <comphelper/proparrhlp.hxx>
#include <cppuhelper/implbase12.hxx>
#include <cppuhelper/implbase4.hxx>
#include <cppuhelper/implbase7.hxx>

namespace com { namespace sun { namespace star { namespace sdbc {
	class SQLException;
} } } }

class Timer;
class INetMIMEMessage;

//.........................................................................
namespace frm
{
//.........................................................................

//========================================================================
//= HTML tools
//========================================================================

const sal_uInt16 SUCCESSFUL_REPRESENT_TEXT			= 0x0001;
const sal_uInt16 SUCCESSFUL_REPRESENT_FILE			= 0x0002;

//------------------------------------------------------------------------------
class HtmlSuccessfulObj
{
public:
	::rtl::OUString		aName;
	::rtl::OUString		aValue;
	sal_uInt16			nRepresentation;

	HtmlSuccessfulObj( const ::rtl::OUString& _rName, const ::rtl::OUString& _rValue,
		sal_uInt16 _nRepresent = SUCCESSFUL_REPRESENT_TEXT )
		:aName( _rName )
		,aValue( _rValue )
		,nRepresentation( _nRepresent )
	{
	}

	HtmlSuccessfulObj()
	{
	}
};


DECLARE_STL_VECTOR(HtmlSuccessfulObj, HtmlSuccessfulObjList);

//========================================================================
class OGroupManager;
class OFormSubmitResetThread;
typedef ::cppu::ImplHelper12    <   ::com::sun::star::form::XForm
								,   ::com::sun::star::awt::XTabControllerModel
								,   ::com::sun::star::form::XLoadListener
								,   ::com::sun::star::sdbc::XRowSetListener
								,   ::com::sun::star::sdb::XRowSetApproveListener
								,   ::com::sun::star::form::XDatabaseParameterBroadcaster2
								,   ::com::sun::star::sdb::XSQLErrorListener
								,   ::com::sun::star::sdb::XSQLErrorBroadcaster
								,   ::com::sun::star::form::XReset
								,   ::com::sun::star::form::XSubmit
								,   ::com::sun::star::form::XLoadable
                                ,   ::com::sun::star::container::XNamed
                                >   ODatabaseForm_BASE1;


typedef ::cppu::ImplHelper4 <   ::com::sun::star::lang::XServiceInfo
                            ,   ::com::sun::star::beans::XPropertyContainer
                            ,   ::com::sun::star::beans::XPropertyAccess
                            ,   ::com::sun::star::sdbc::XWarningsSupplier
                            >   ODatabaseForm_BASE2;

typedef ::cppu::ImplHelper7<	::com::sun::star::sdbc::XCloseable,
								::com::sun::star::sdbc::XRowSet,
								::com::sun::star::sdb::XCompletedExecution,
								::com::sun::star::sdb::XRowSetApproveBroadcaster,
								::com::sun::star::sdbc::XResultSetUpdate,
								::com::sun::star::sdbcx::XDeleteRows,
								::com::sun::star::sdbc::XParameters > ODatabaseForm_BASE3;


class ODatabaseForm	:public OFormComponents
					,public OPropertySetAggregationHelper
					,public OPropertyChangeListener
					,public ODatabaseForm_BASE1
					,public ODatabaseForm_BASE2
					,public ODatabaseForm_BASE3
                    ,public IPropertyBagHelperContext
{
	friend class OFormSubmitResetThread;

	OImplementationIdsRef				m_aHoldIdHelper;

		// listener administration
	::cppu::OInterfaceContainerHelper	m_aLoadListeners;
	::cppu::OInterfaceContainerHelper	m_aRowSetApproveListeners;
	::cppu::OInterfaceContainerHelper	m_aRowSetListeners;
	::cppu::OInterfaceContainerHelper	m_aSubmitListeners;
	::cppu::OInterfaceContainerHelper	m_aErrorListeners;
    ResetListeners                      m_aResetListeners;
	::osl::Mutex						m_aResetSafety;
	::com::sun::star::uno::Any			m_aCycle;
	::com::sun::star::uno::Any			m_aIgnoreResult; // set when we are a subform and our master form positioned on a new row
	::com::sun::star::uno::Sequence< ::rtl::OUString >						m_aMasterFields;
	::com::sun::star::uno::Sequence< ::rtl::OUString >						m_aDetailFields;

	// the object doin' most of the work - an SDB-rowset
	::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation> 		m_xAggregate;
	// same object, interface as member because of performance reasons
	::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet> 			m_xAggregateAsRowSet;

    PropertyBagHelper           m_aPropertyBagHelper;
    ::dbtools::WarningsContainer    m_aWarnings;
	OPropertyChangeMultiplexer*	m_pAggregatePropertyMultiplexer;
	// Verwaltung der ControlGruppen
	OGroupManager*				m_pGroupManager;
    ::dbtools::ParameterManager m_aParameterManager;
    ::dbtools::FilterManager    m_aFilterManager;
	Timer*						m_pLoadTimer;

	OFormSubmitResetThread*		m_pThread;
	::rtl::OUString				m_sCurrentErrorContext;
				// will be used as additional context information
				// when an exception is catched and forwarded to the listeners

	sal_Int32						m_nResetsPending;
//	<overwritten_properties>
	sal_Int32					m_nPrivileges;
    sal_Bool                    m_bInsertOnly;
//	</overwritten_properties>

//	<properties>
    ::com::sun::star::uno::Any  m_aControlBorderColorFocus;
    ::com::sun::star::uno::Any  m_aControlBorderColorMouse;
    ::com::sun::star::uno::Any  m_aControlBorderColorInvalid;
    ::com::sun::star::uno::Any  m_aDynamicControlBorder;
	::rtl::OUString				m_sName;
	::rtl::OUString				m_aTargetURL;
	::rtl::OUString				m_aTargetFrame;
	::com::sun::star::form::FormSubmitMethod	m_eSubmitMethod;
	::com::sun::star::form::FormSubmitEncoding	m_eSubmitEncoding;
	::com::sun::star::form::NavigationBarMode	m_eNavigation;
	sal_Bool					m_bAllowInsert : 1;
	sal_Bool					m_bAllowUpdate : 1;
	sal_Bool					m_bAllowDelete : 1;
//	</properties>
	sal_Bool					m_bLoaded : 1;
	sal_Bool					m_bSubForm : 1;
	sal_Bool					m_bForwardingConnection : 1;	// sal_True if we're setting the ActiveConnection on the aggregate
	sal_Bool					m_bSharingConnection : 1;		// sal_True if the connection we're using is shared with out parent

public:
	ODatabaseForm(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rxFactory);
	ODatabaseForm( const ODatabaseForm& _cloneSource );
	~ODatabaseForm();

	// UNO binding
	DECLARE_UNO3_AGG_DEFAULTS(ODatabaseForm, OFormComponents);
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation(const ::com::sun::star::uno::Type& _rType);

	// XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type> SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence<sal_Int8> SAL_CALL getImplementationId(  );

	// ::com::sun::star::lang::XComponent
	virtual void SAL_CALL disposing();

	// property handling
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
	virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();
	virtual void SAL_CALL getFastPropertyValue(::com::sun::star::uno::Any& rValue, sal_Int32 nHandle ) const;
	virtual sal_Bool SAL_CALL convertFastPropertyValue(::com::sun::star::uno::Any& rConvertedValue, ::com::sun::star::uno::Any& rOldValue, sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue );
	virtual void SAL_CALL setFastPropertyValue_NoBroadcast(sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue);

	::com::sun::star::uno::Any	SAL_CALL getFastPropertyValue( sal_Int32 nHandle );
	void fire( sal_Int32 * pnHandles, const ::com::sun::star::uno::Any * pNewValues, const ::com::sun::star::uno::Any * pOldValues, sal_Int32 nCount, sal_Bool bVetoable );

    // IPropertyBagHelperContext
    virtual ::osl::Mutex&   getMutex();
    virtual void            describeFixedAndAggregateProperties(
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& _out_rFixedProperties,
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& _out_rAggregateProperties
    ) const;
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XMultiPropertySet >
                            getPropertiesInterface();

	// com::sun::star::beans::XPropertyState
	virtual	::com::sun::star::beans::PropertyState getPropertyStateByHandle(sal_Int32 nHandle);
	virtual	void setPropertyToDefaultByHandle(sal_Int32 nHandle);
	virtual ::com::sun::star::uno::Any getPropertyDefaultByHandle(sal_Int32 nHandle) const;

	// com::sun::star::sdbc::XSQLErrorBroadcaster
    virtual void SAL_CALL addSQLErrorListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XSQLErrorListener>& _rxListener);
    virtual void SAL_CALL removeSQLErrorListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XSQLErrorListener>& _rxListener);

	// com::sun::star::form::XForm
	// nothing to implement

	// com::sun::star::form::XReset
    virtual void SAL_CALL reset();
    virtual void SAL_CALL addResetListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XResetListener>& _rxListener);
    virtual void SAL_CALL removeResetListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XResetListener>& _rxListener);

	// com::sun::star::form::XSubmit
    virtual void SAL_CALL submit(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& aControl, const ::com::sun::star::awt::MouseEvent& aMouseEvt);
    virtual void SAL_CALL addSubmitListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XSubmitListener>& _rxListener);
    virtual void SAL_CALL removeSubmitListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XSubmitListener>& _rxListener);

	// com::sun::star::container::XChild
	virtual InterfaceRef SAL_CALL getParent() { return OFormComponents::getParent(); }
	virtual void SAL_CALL setParent(const InterfaceRef& Parent);

	// com::sun::star::container::XNamed
    virtual ::rtl::OUString SAL_CALL getName();
    virtual void SAL_CALL setName(const ::rtl::OUString& aName);

	// com::sun::star::awt::XTabControllerModel
    virtual sal_Bool SAL_CALL getGroupControl();
    virtual void SAL_CALL setGroupControl(sal_Bool /*_bGroupControl*/) { }
    virtual void SAL_CALL setControlModels(const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel > >& _rControls);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel > > SAL_CALL getControlModels();
    virtual void SAL_CALL setGroup(const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel > >& _rGroup, const ::rtl::OUString& _rGroupName);
    virtual sal_Int32 SAL_CALL getGroupCount();
    virtual void SAL_CALL getGroup(sal_Int32 _nGroup, ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel > >& _rxGroup, ::rtl::OUString& _rName);
    virtual void SAL_CALL getGroupByName(const ::rtl::OUString& _rName, ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel > >& _rxGroup);

	// com::sun::star::lang::XEventListener
	virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& _rSource);

	// com::sun::star::form::XLoadListener
    virtual void SAL_CALL loaded(const ::com::sun::star::lang::EventObject& aEvent);
    virtual void SAL_CALL unloading(const ::com::sun::star::lang::EventObject& aEvent);
	virtual void SAL_CALL unloaded(const ::com::sun::star::lang::EventObject& aEvent);
    virtual void SAL_CALL reloading(const ::com::sun::star::lang::EventObject& aEvent);
    virtual void SAL_CALL reloaded(const ::com::sun::star::lang::EventObject& aEvent);

	// com::sun::star::form::XLoadable
	virtual void SAL_CALL load();
	virtual void SAL_CALL unload();
    virtual void SAL_CALL reload();
	virtual sal_Bool SAL_CALL isLoaded();
	virtual void SAL_CALL addLoadListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XLoadListener>& _rxListener);
	virtual void SAL_CALL removeLoadListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XLoadListener>& _rxListener);

	// com::sun::star::sdbc::XCloseable
	virtual void SAL_CALL close();

	// com::sun::star::sdbc::XRowSetListener
	virtual void SAL_CALL cursorMoved(const ::com::sun::star::lang::EventObject& event);
	virtual void SAL_CALL rowChanged(const ::com::sun::star::lang::EventObject& event);
	virtual void SAL_CALL rowSetChanged(const ::com::sun::star::lang::EventObject& event);

	// com::sun::star::sdb::XRowSetApproveListener
    virtual sal_Bool SAL_CALL approveCursorMove(const ::com::sun::star::lang::EventObject& event);
    virtual sal_Bool SAL_CALL approveRowChange(const ::com::sun::star::sdb::RowChangeEvent& event);
    virtual sal_Bool SAL_CALL approveRowSetChange(const ::com::sun::star::lang::EventObject& event);

	// com::sun::star::sdb::XRowSetApproveBroadcaster
    virtual void SAL_CALL addRowSetApproveListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XRowSetApproveListener>& _rxListener);
    virtual void SAL_CALL removeRowSetApproveListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XRowSetApproveListener>& _rxListener);

	// com::sun:star::form::XDatabaseParameterBroadcaster2
	virtual void SAL_CALL addDatabaseParameterListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XDatabaseParameterListener>& _rxListener);
	virtual void SAL_CALL removeDatabaseParameterListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XDatabaseParameterListener>& _rxListener);

	// com::sun:star::form::XDatabaseParameterBroadcaster
	virtual void SAL_CALL addParameterListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XDatabaseParameterListener>& _rxListener);
	virtual void SAL_CALL removeParameterListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XDatabaseParameterListener>& _rxListener);

	// com::sun::star::sdbc::XRowSet
    virtual void SAL_CALL execute();
    virtual void SAL_CALL addRowSetListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSetListener>& _rxListener);
    virtual void SAL_CALL removeRowSetListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSetListener>& _rxListener);

	// com::sun::star::sdb::XCompletedExecution
    virtual void SAL_CALL executeWithCompletion( const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler >& handler );

	// com::sun::star::sdbc::XResultSet
    virtual sal_Bool SAL_CALL next();
    virtual sal_Bool SAL_CALL isBeforeFirst();
    virtual sal_Bool SAL_CALL isAfterLast();
    virtual sal_Bool SAL_CALL isFirst();
    virtual sal_Bool SAL_CALL isLast();
    virtual void SAL_CALL beforeFirst();
    virtual void SAL_CALL afterLast();
    virtual sal_Bool SAL_CALL first();
    virtual sal_Bool SAL_CALL last();
    virtual sal_Int32 SAL_CALL getRow();
    virtual sal_Bool SAL_CALL absolute(sal_Int32 row);
    virtual sal_Bool SAL_CALL relative(sal_Int32 rows);
    virtual sal_Bool SAL_CALL previous();
    virtual void SAL_CALL refreshRow();
	virtual sal_Bool SAL_CALL rowUpdated();
    virtual sal_Bool SAL_CALL rowInserted();
    virtual sal_Bool SAL_CALL rowDeleted();
    virtual InterfaceRef SAL_CALL getStatement();

	// com::sun::star::sdbc::XResultSetUpdate
    virtual void SAL_CALL insertRow();
    virtual void SAL_CALL updateRow();
    virtual void SAL_CALL deleteRow();
    virtual void SAL_CALL cancelRowUpdates();
    virtual void SAL_CALL moveToInsertRow();
    virtual void SAL_CALL moveToCurrentRow();

	// com::sun::star::sdbcx::XDeleteRows
	virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL deleteRows(const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any>& rows);

	// com::sun::star::lang::XServiceInfo
	virtual sal_Bool SAL_CALL supportsService(const ::rtl::OUString& ServiceName);
	virtual ::rtl::OUString	SAL_CALL getImplementationName();
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	// com::sun::star::lang::XServiceInfo - static version
	static	::rtl::OUString	SAL_CALL getImplementationName_Static();
	static	::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_Static();
	static	::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getCurrentServiceNames_Static();
	static	::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getCompatibleServiceNames_Static();
	static	::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL Create( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory );

	// com::sun::star::io::XPersistObject
    virtual ::rtl::OUString SAL_CALL getServiceName();
    virtual void SAL_CALL write(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream>& _rxOutStream);
    virtual void SAL_CALL read(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream>& _rxInStream);

	// com::sun::star::sdbc::XSQLErrorListener
    virtual void SAL_CALL errorOccured(const ::com::sun::star::sdb::SQLErrorEvent& aEvent);

	// com::sun::star::sdbc::XParameters
	virtual void SAL_CALL setNull(sal_Int32 parameterIndex, sal_Int32 sqlType);
    virtual void SAL_CALL setObjectNull(sal_Int32 parameterIndex, sal_Int32 sqlType, const ::rtl::OUString& typeName);
    virtual void SAL_CALL setBoolean(sal_Int32 parameterIndex, sal_Bool x);
    virtual void SAL_CALL setByte(sal_Int32 parameterIndex, sal_Int8 x);
    virtual void SAL_CALL setShort(sal_Int32 parameterIndex, sal_Int16 x);
    virtual void SAL_CALL setInt(sal_Int32 parameterIndex, sal_Int32 x);
    virtual void SAL_CALL setLong(sal_Int32 parameterIndex, sal_Int64 x);
    virtual void SAL_CALL setFloat(sal_Int32 parameterIndex, float x);
    virtual void SAL_CALL setDouble(sal_Int32 parameterIndex, double x);
    virtual void SAL_CALL setString(sal_Int32 parameterIndex, const ::rtl::OUString& x);
    virtual void SAL_CALL setBytes(sal_Int32 parameterIndex, const ::com::sun::star::uno::Sequence< sal_Int8 >& x);
    virtual void SAL_CALL setDate(sal_Int32 parameterIndex, const ::com::sun::star::util::Date& x);
    virtual void SAL_CALL setTime(sal_Int32 parameterIndex, const ::com::sun::star::util::Time& x);
    virtual void SAL_CALL setTimestamp(sal_Int32 parameterIndex, const ::com::sun::star::util::DateTime& x);
    virtual void SAL_CALL setBinaryStream(sal_Int32 parameterIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream>& x, sal_Int32 length);
	virtual void SAL_CALL setCharacterStream(sal_Int32 parameterIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream>& x, sal_Int32 length);
    virtual void SAL_CALL setObject(sal_Int32 parameterIndex, const ::com::sun::star::uno::Any& x);
    virtual void SAL_CALL setObjectWithInfo(sal_Int32 parameterIndex, const ::com::sun::star::uno::Any& x, sal_Int32 targetSqlType, sal_Int32 scale);
    virtual void SAL_CALL setRef(sal_Int32 parameterIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRef>& x);
    virtual void SAL_CALL setBlob(sal_Int32 parameterIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XBlob>& x);
    virtual void SAL_CALL setClob(sal_Int32 parameterIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XClob>& x);
    virtual void SAL_CALL setArray(sal_Int32 parameterIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XArray>& x);
	virtual void SAL_CALL clearParameters();

    // XPropertyChangeListener
    virtual void SAL_CALL propertyChange( const ::com::sun::star::beans::PropertyChangeEvent& evt );

    // XPropertyContainer
    virtual void SAL_CALL addProperty( const ::rtl::OUString& Name, ::sal_Int16 Attributes, const ::com::sun::star::uno::Any& DefaultValue );
    virtual void SAL_CALL removeProperty( const ::rtl::OUString& Name );

    // XPropertyAccess
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL getPropertyValues(  );
    virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aProps );
    using OPropertySetAggregationHelper::setPropertyValues;

    // XWarningsSupplier
    virtual ::com::sun::star::uno::Any SAL_CALL getWarnings(  );
    virtual void SAL_CALL clearWarnings(  );

    // XCloneable
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloneable > SAL_CALL createClone(  );

    inline void submitNBC( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& Control, const ::com::sun::star::awt::MouseEvent& MouseEvt );

protected:
    // OPropertySetAggregationHelper overridables
    virtual void SAL_CALL forwardingPropertyValue( sal_Int32 _nHandle );
    virtual void SAL_CALL forwardedPropertyValue( sal_Int32 _nHandle, bool _bSuccess );

    // OInterfaceContainer overridables
    virtual void implInserted( const ElementDescription* _pElement );
	virtual void implRemoved(const InterfaceRef& _rxObject);

	// OPropertyChangeListener
	virtual void _propertyChanged( const ::com::sun::star::beans::PropertyChangeEvent& );

private:
    sal_Bool executeRowSet(::osl::ResettableMutexGuard& _rClearForNotifies, sal_Bool bMoveToFirst = sal_True,
					const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler >& _rxCompletionHandler = ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler >());
	bool	fillParameters(::osl::ResettableMutexGuard& _rClearForNotifies,
					const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler >& _rxCompletionHandler = ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler >());
	void	updateParameterInfo();
	bool	hasValidParent() const;

	// impl methods
	void	load_impl(sal_Bool bCausedByParentForm, sal_Bool bMoveToFirst = sal_True,
		const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler >& _rxCompletionHandler = ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler >());
	void	reload_impl(sal_Bool bMoveToFirst,
		const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler >& _rxCompletionHandler = ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler >());
	void	submit_impl(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& Control, const ::com::sun::star::awt::MouseEvent& MouseEvt, bool _bAproveByListeners);
	void	reset_impl(bool _bAproveByListeners);

	sal_Bool	implEnsureConnection();

	// connection sharing

	/// checks if we can re-use (aka share) the connection of the given parent
	sal_Bool	canShareConnection( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxParentProps );

	/// starts sharing the connection with the parent
	void		doShareConnection( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxParentProps );

	/// stops sharing the connection with the parent
	void		stopSharingConnection( );

	/// called when the connection which we share with our parent is being disposed
	void		disposingSharedConnection( const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConn );

	/// checks if we currently share our connection with our parent
	sal_Bool	isSharingConnection( ) const { return m_bSharingConnection; }

    /** calls our row set approval listeners

        @param _rEvent
            the event to notify
        @param _bAllowSQLException
            <TRUE/> if SQLExceptions are allowed to leave the method
        @param _rGuard
            the guard to be cleared before actually calling into the listeners, but after making
            a copy of the listeners array to operate on.
        @return
            <TRUE/> if and only if the execution has been approved
    */
    bool    impl_approveRowChange_throw(
        const ::com::sun::star::lang::EventObject& _rEvent,
        const bool _bAllowSQLException,
        ::osl::ClearableMutexGuard& _rGuard
    );

	/// invalidate all our parameter-related stuff
	void		invlidateParameters();

	void		saveInsertOnlyState( );
	void		restoreInsertOnlyState( );

	// error handling
	void	onError(const ::com::sun::star::sdb::SQLErrorEvent& _rEvent);
	void	onError(const ::com::sun::star::sdbc::SQLException&, const ::rtl::OUString& _rContextDescription);

	// HTML tools
    ::rtl::OUString         GetDataEncoded(bool _bURLEncoded,const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& SubmitButton, const ::com::sun::star::awt::MouseEvent& MouseEvt);
	::rtl::OUString			GetDataURLEncoded(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& SubmitButton, const ::com::sun::star::awt::MouseEvent& MouseEvt);
	::rtl::OUString			GetDataTextEncoded(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& SubmitButton, const ::com::sun::star::awt::MouseEvent& MouseEvt);
	::com::sun::star::uno::Sequence<sal_Int8>	GetDataMultiPartEncoded(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& SubmitButton, const ::com::sun::star::awt::MouseEvent& MouseEvt,
											 ::rtl::OUString& rContentType);

	void AppendComponent(HtmlSuccessfulObjList& rList, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& xComponentSet, const ::rtl::OUString& rNamePrefix,
					 const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& rxSubmitButton, const ::com::sun::star::awt::MouseEvent& MouseEvt);

	void FillSuccessfulList(HtmlSuccessfulObjList& rList, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& rxSubmitButton, const ::com::sun::star::awt::MouseEvent& MouseEvt);

	void InsertTextPart(INetMIMEMessage& rParent, const ::rtl::OUString& rName, const ::rtl::OUString& rData);
	sal_Bool InsertFilePart(INetMIMEMessage& rParent, const ::rtl::OUString& rName, const ::rtl::OUString& rFileName);
	void Encode(::rtl::OUString& rString) const;

	::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > getConnection();

    void    impl_createLoadTimer();

    void    impl_construct();

	DECL_LINK( OnTimeout, void* );
protected:
    using OPropertySetHelper::getPropertyValues;
};

inline void ODatabaseForm::submitNBC(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl>& Control, const ::com::sun::star::awt::MouseEvent& MouseEvt)
{
	submit_impl(Control, MouseEvt, sal_False);
}

//.........................................................................
}	// namespace frm
//.........................................................................

#endif // _FRM_DATABASEFORM_HXX_
