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



#ifndef _SBA_FORMADAPTER_HXX
#define _SBA_FORMADAPTER_HXX

#ifndef _SBA_MULTIPLEX_HXX
#include "sbamultiplex.hxx"
#endif

#ifndef _COM_SUN_STAR_SDBC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XPARAMETERS_HPP_
#include <com/sun/star/sdbc/XParameters.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCOLUMNLOCATE_HPP_
#include <com/sun/star/sdbc/XColumnLocate.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCLOSEABLE_HPP_
#include <com/sun/star/sdbc/XCloseable.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XROWLOCATE_HPP_
#include <com/sun/star/sdbcx/XRowLocate.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XROWUPDATE_HPP_
#include <com/sun/star/sdbc/XRowUpdate.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSETUPDATE_HPP_
#include <com/sun/star/sdbc/XResultSetUpdate.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XROWSET_HPP_
#include <com/sun/star/sdbc/XRowSet.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XDELETEROWS_HPP_
#include <com/sun/star/sdbcx/XDeleteRows.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XRESULTSETMETADATASUPPLIER_HPP_
#include <com/sun/star/sdbc/XResultSetMetaDataSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XWARNINGSSUPPLIER_HPP_
#include <com/sun/star/sdbc/XWarningsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XCOLUMNSSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XSQLERRORBROADCASTER_HPP_
#include <com/sun/star/sdb/XSQLErrorBroadcaster.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XROWSETAPPROVEBROADCASTER_HPP_
#include <com/sun/star/sdb/XRowSetApproveBroadcaster.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XLOADABLE_HPP_
#include <com/sun/star/form/XLoadable.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_XRESULTSETACCESS_HPP_
#include <com/sun/star/sdb/XResultSetAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XFORM_HPP_
#include <com/sun/star/form/XForm.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XMULTIPROPERTYSET_HPP_
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XFASTPROPERTYSET_HPP_
#include <com/sun/star/beans/XFastPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXCONTAINER_HPP_
#include <com/sun/star/container/XIndexContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCONTAINER_HPP_
#include <com/sun/star/container/XContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XRESET_HPP_
#include <com/sun/star/form/XReset.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XSUBMIT_HPP_
#include <com/sun/star/form/XSubmit.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XDATABASEPARAMETERBROADCASTER_HPP_
#include <com/sun/star/form/XDatabaseParameterBroadcaster.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XENUMERATIONACCESS_HPP_
#include <com/sun/star/container/XEnumerationAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSTATE_HPP_
#include <com/sun/star/beans/XPropertyState.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XTABCONTROLLERMODEL_HPP_
#include <com/sun/star/awt/XTabControllerModel.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XPERSISTOBJECT_HPP_
#include <com/sun/star/io/XPersistObject.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMED_HPP_
#include <com/sun/star/container/XNamed.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XCANCELLABLE_HPP_
#include <com/sun/star/util/XCancellable.hpp>
#endif
#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif
#ifndef _COMPHELPER_UNO3_HXX_
#include <comphelper/uno3.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE12_HXX_
#include <cppuhelper/implbase12.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE10_HXX_
#include <cppuhelper/implbase10.hxx>
#endif
#ifndef _DBAUI_MODULE_DBU_HXX_
#include "moduledbu.hxx"
#endif

namespace dbaui
{
	//==================================================================
	// SbaXFormAdapter
	//==================================================================

	typedef ::cppu::WeakImplHelper12<	::com::sun::star::sdbc::XResultSetMetaDataSupplier
									,	::com::sun::star::sdb::XResultSetAccess
									,	::com::sun::star::sdbc::XResultSetUpdate
									,	::com::sun::star::sdbc::XRowSet
									,	::com::sun::star::sdb::XRowSetApproveBroadcaster
									,	::com::sun::star::sdbcx::XRowLocate
									,	::com::sun::star::sdbc::XRowUpdate
									,	::com::sun::star::sdbc::XRow
									,	::com::sun::star::sdbcx::XColumnsSupplier
									,	::com::sun::star::sdbc::XColumnLocate
									// --- stardiv::one::form::component::DatabaseForm ---
									,	::com::sun::star::sdbc::XParameters
									,	::com::sun::star::sdbcx::XDeleteRows
									>	SbaXFormAdapter_BASE1;
	typedef ::cppu::ImplHelper12	<	::com::sun::star::sdbc::XWarningsSupplier
									,	::com::sun::star::sdbc::XCloseable
									,	::com::sun::star::form::XLoadable
									,	::com::sun::star::sdb::XSQLErrorBroadcaster
									,	::com::sun::star::form::XDatabaseParameterBroadcaster
										// --- stardiv::one::form::component::Form ---
									,		::com::sun::star::form::XForm
									,		::com::sun::star::form::XSubmit
									,		::com::sun::star::awt::XTabControllerModel
											// --- stardiv::one::form::FormComponent ---
									,			::com::sun::star::lang::XComponent
									,			::com::sun::star::beans::XFastPropertySet
											// already present : ::com::sun::star::form::XFormComponent (base of ::com::sun::star::form::XForm)
									,			::com::sun::star::beans::XMultiPropertySet
									,			::com::sun::star::container::XNamed
									>	SbaXFormAdapter_BASE2;
	typedef ::cppu::ImplHelper10	<			::com::sun::star::io::XPersistObject
									,			::com::sun::star::beans::XPropertySet
										// --- stardiv::one::data::DatabaseCursor ---
									,		::com::sun::star::util::XCancellable
										// already present : ::com::sun::star::beans::XPropertySet
											// --- stardiv::one::data::DatabaseComponent ---
											// already present : ::com::sun::star::lang::XComponent
											// already present : ::com::sun::star::container::XChild (base of ::com::sun::star::form::XForm)
									// interfaces I don't know the service which they belong to ;)
									// (they are supported by FmXDatabaseForm, se we support it, too)
									,	::com::sun::star::beans::XPropertyState
									,	::com::sun::star::form::XReset
									,	::com::sun::star::container::XNameContainer
									,	::com::sun::star::container::XIndexContainer
									,	::com::sun::star::container::XContainer
									,	::com::sun::star::container::XEnumerationAccess
									// interfaces we need because of other reasons
									,	::com::sun::star::beans::XPropertyChangeListener
									>	SbaXFormAdapter_BASE3;

	class SbaXFormAdapter
		:public SbaXFormAdapter_BASE1
		,public SbaXFormAdapter_BASE2
		,public SbaXFormAdapter_BASE3
	{
	private:
        OModuleClient                m_aModuleClient;
		::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet > 							m_xMainForm;
		::osl::Mutex						m_aMutex;

		SbaXLoadMultiplexer					m_aLoadListeners;
		SbaXRowSetMultiplexer				m_aRowSetListeners;
		SbaXRowSetApproveMultiplexer		m_aRowSetApproveListeners;
		SbaXSQLErrorMultiplexer				m_aErrorListeners;
		SbaXParameterMultiplexer			m_aParameterListeners;
		SbaXSubmitMultiplexer				m_aSubmitListeners;
		SbaXResetMultiplexer				m_aResetListeners;

		SbaXPropertyChangeMultiplexer		m_aPropertyChangeListeners;
		SbaXVetoableChangeMultiplexer		m_aVetoablePropertyChangeListeners;
		SbaXPropertiesChangeMultiplexer		m_aPropertiesChangeListeners;

		::cppu::OInterfaceContainerHelper	m_aDisposeListeners;
		::cppu::OInterfaceContainerHelper	m_aContainerListeners;

		// hierarchy administration
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > 						m_xParent;
		::std::vector<	::com::sun::star::uno::Reference< ::com::sun::star::form::XFormComponent > >	m_aChildren;
		::std::vector< ::rtl::OUString >																m_aChildNames;

		// properties
		::rtl::OUString					m_sName;
		sal_Int32						m_nNamePropHandle;

	public:
		::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet >  getAttachedForm() const { return m_xMainForm; }

	public:
		SbaXFormAdapter();
		~SbaXFormAdapter();

	//	::com::sun::star::uno::Reference< ::com::sun::star::reflection::XIdlClass >  getIdlClass();
	//	::com::sun::star::uno::Sequence<::com::sun::star::uno::Reference< ::com::sun::star::reflection::XIdlClass > > getIdlClasses();

		void AttachForm(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet >& xNewMaster);

		// UNO
		DECLARE_UNO3_DEFAULTS(SbaXFormAdapter, SbaXFormAdapter_BASE1);
		virtual ::com::sun::star::uno::Any	SAL_CALL queryInterface(const ::com::sun::star::uno::Type& _rType);

		// XTypeProvider
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
		virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

	// ::com::sun::star::sdbc::XCloseable
		virtual void SAL_CALL close();

	// ::com::sun::star::sdbc::XResultSetMetaDataSupplier
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSetMetaData > SAL_CALL getMetaData();

	// ::com::sun::star::sdbc::XColumnLocate
		virtual sal_Int32 SAL_CALL findColumn(const ::rtl::OUString& columnName);

	// ::com::sun::star::sdbcx::XColumnsSupplier
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > SAL_CALL getColumns();

	// ::com::sun::star::sdbc::XRow
		virtual sal_Bool SAL_CALL wasNull();
		virtual ::rtl::OUString SAL_CALL getString(sal_Int32 columnIndex);
		virtual sal_Bool SAL_CALL getBoolean(sal_Int32 columnIndex);
		virtual sal_Int8 SAL_CALL getByte(sal_Int32 columnIndex);
		virtual sal_Int16 SAL_CALL getShort(sal_Int32 columnIndex);
		virtual sal_Int32 SAL_CALL getInt(sal_Int32 columnIndex);
		virtual sal_Int64 SAL_CALL getLong(sal_Int32 columnIndex);
		virtual float SAL_CALL getFloat(sal_Int32 columnIndex);
		virtual double SAL_CALL getDouble(sal_Int32 columnIndex);
		virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getBytes(sal_Int32 columnIndex);
		virtual ::com::sun::star::util::Date SAL_CALL getDate(sal_Int32 columnIndex);
		virtual ::com::sun::star::util::Time SAL_CALL getTime(sal_Int32 columnIndex);
		virtual ::com::sun::star::util::DateTime SAL_CALL getTimestamp(sal_Int32 columnIndex);
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >  SAL_CALL getBinaryStream(sal_Int32 columnIndex);
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >  SAL_CALL getCharacterStream(sal_Int32 columnIndex);
		virtual ::com::sun::star::uno::Any SAL_CALL getObject(sal_Int32 columnIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >& typeMap);
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRef >  SAL_CALL getRef(sal_Int32 columnIndex);
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XBlob >  SAL_CALL getBlob(sal_Int32 columnIndex);
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XClob >  SAL_CALL getClob(sal_Int32 columnIndex);
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XArray >  SAL_CALL getArray(sal_Int32 columnIndex);

	// ::com::sun::star::sdbcx::XRowLocate
		virtual ::com::sun::star::uno::Any SAL_CALL getBookmark();
		virtual sal_Bool SAL_CALL moveToBookmark(const ::com::sun::star::uno::Any& bookmark);
		virtual sal_Bool SAL_CALL moveRelativeToBookmark(const ::com::sun::star::uno::Any& bookmark, sal_Int32 rows);
		virtual sal_Int32 SAL_CALL compareBookmarks(const ::com::sun::star::uno::Any& first, const ::com::sun::star::uno::Any& second);
		virtual sal_Bool SAL_CALL hasOrderedBookmarks();
		virtual sal_Int32 SAL_CALL hashBookmark(const ::com::sun::star::uno::Any& bookmark);

	// ::com::sun::star::sdbc::XRowUpdate
		virtual void SAL_CALL updateNull(sal_Int32 columnIndex);
		virtual void SAL_CALL updateBoolean(sal_Int32 columnIndex, sal_Bool x);
		virtual void SAL_CALL updateByte(sal_Int32 columnIndex, sal_Int8 x);
		virtual void SAL_CALL updateShort(sal_Int32 columnIndex, sal_Int16 x);
		virtual void SAL_CALL updateInt(sal_Int32 columnIndex, sal_Int32 x);
		virtual void SAL_CALL updateLong(sal_Int32 columnIndex, sal_Int64 x);
		virtual void SAL_CALL updateFloat(sal_Int32 columnIndex, float x);
		virtual void SAL_CALL updateDouble(sal_Int32 columnIndex, double x);
		virtual void SAL_CALL updateString(sal_Int32 columnIndex, const ::rtl::OUString& x);
		virtual void SAL_CALL updateBytes(sal_Int32 columnIndex, const ::com::sun::star::uno::Sequence< sal_Int8 >& x);
		virtual void SAL_CALL updateDate(sal_Int32 columnIndex, const ::com::sun::star::util::Date& x);
		virtual void SAL_CALL updateTime(sal_Int32 columnIndex, const ::com::sun::star::util::Time& x);
		virtual void SAL_CALL updateTimestamp(sal_Int32 columnIndex, const ::com::sun::star::util::DateTime& x);
		virtual void SAL_CALL updateBinaryStream(sal_Int32 columnIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& x, sal_Int32 length);
		virtual void SAL_CALL updateCharacterStream(sal_Int32 columnIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& x, sal_Int32 length);
		virtual void SAL_CALL updateObject(sal_Int32 columnIndex, const ::com::sun::star::uno::Any& x);
		virtual void SAL_CALL updateNumericObject(sal_Int32 columnIndex, const ::com::sun::star::uno::Any& x, sal_Int32 scale);

	// ::com::sun::star::sdbc::XResultSet
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
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >  SAL_CALL getStatement();

	// ::com::sun::star::sdbc::XResultSetUpdate
		virtual void SAL_CALL insertRow();
		virtual void SAL_CALL updateRow();
		virtual void SAL_CALL deleteRow();
		virtual void SAL_CALL cancelRowUpdates();
		virtual void SAL_CALL moveToInsertRow();
		virtual void SAL_CALL moveToCurrentRow();

	// ::com::sun::star::sdbc::XRowSet
		virtual void SAL_CALL execute();
		virtual void SAL_CALL addRowSetListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSetListener >& listener);
		virtual void SAL_CALL removeRowSetListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSetListener >& listener);

	// ::com::sun::star::sdbcx::XDeleteRows
		virtual ::com::sun::star::uno::Sequence<sal_Int32> SAL_CALL deleteRows(const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& rows);

	// ::com::sun::star::sdbc::XWarningsSupplier
		virtual ::com::sun::star::uno::Any SAL_CALL getWarnings();
		virtual void SAL_CALL clearWarnings();

	// ::com::sun::star::sdb::XRowSetApproveBroadcaster
		virtual void SAL_CALL addRowSetApproveListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XRowSetApproveListener >& listener);
		virtual void SAL_CALL removeRowSetApproveListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XRowSetApproveListener >& listener);

	// com::sun::star::sdbc::XSQLErrorBroadcaster
		virtual void SAL_CALL addSQLErrorListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XSQLErrorListener >& _rListener);
		virtual void SAL_CALL removeSQLErrorListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdb::XSQLErrorListener >& _rListener);

	// ::com::sun::star::sdb::XResultSetAccess
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XResultSet > SAL_CALL createResultSet();

	// com::sun::star::form::XLoadable
		virtual void SAL_CALL load();
		virtual void SAL_CALL unload();
		virtual void SAL_CALL reload();
		virtual sal_Bool SAL_CALL isLoaded();
		virtual void SAL_CALL addLoadListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XLoadListener >& aListener);
		virtual void SAL_CALL removeLoadListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XLoadListener >& aListener);

	// ::com::sun::star::sdbc::XParameters
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
		virtual void SAL_CALL setBinaryStream(sal_Int32 parameterIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& x, sal_Int32 length);
		virtual void SAL_CALL setCharacterStream(sal_Int32 parameterIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& x, sal_Int32 length);
		virtual void SAL_CALL setObject(sal_Int32 parameterIndex, const ::com::sun::star::uno::Any& x);
		virtual void SAL_CALL setObjectWithInfo(sal_Int32 parameterIndex, const ::com::sun::star::uno::Any& x, sal_Int32 targetSqlType, sal_Int32 scale);
		virtual void SAL_CALL setRef(sal_Int32 parameterIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRef >& x);
		virtual void SAL_CALL setBlob(sal_Int32 parameterIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XBlob >& x);
		virtual void SAL_CALL setClob(sal_Int32 parameterIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XClob >& x);
		virtual void SAL_CALL setArray(sal_Int32 parameterIndex, const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XArray >& x);
		virtual void SAL_CALL clearParameters();

		// ::com::sun::star::form::XDatabaseParameterBroadcaster
		virtual void SAL_CALL addParameterListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XDatabaseParameterListener >& aListener);
		virtual void SAL_CALL removeParameterListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XDatabaseParameterListener >& aListener);

		// ::com::sun::star::container::XChild
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getParent();
		virtual void SAL_CALL setParent(const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& Parent);

		// ::com::sun::star::form::XSubmit
		virtual void SAL_CALL submit(const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >& aControl, const ::com::sun::star::awt::MouseEvent& aMouseEvt);
		virtual void SAL_CALL addSubmitListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XSubmitListener >& aListener);
		virtual void SAL_CALL removeSubmitListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XSubmitListener >& aListener);

		// ::com::sun::star::awt::XTabControllerModel
		virtual sal_Bool SAL_CALL getGroupControl();
		virtual void SAL_CALL setGroupControl(sal_Bool GroupControl);
		virtual void SAL_CALL setControlModels(const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >  >& Controls);
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >  > SAL_CALL getControlModels();
		virtual void SAL_CALL setGroup(const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >  >& _rGroup, const ::rtl::OUString& GroupName);
		virtual sal_Int32 SAL_CALL getGroupCount();
		virtual void SAL_CALL getGroup(sal_Int32 nGroup, ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >  >& _rGroup, ::rtl::OUString& Name);
		virtual void SAL_CALL getGroupByName(const ::rtl::OUString& Name, ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >  >& _rGroup);

		// ::com::sun::star::lang::XComponent
		virtual void SAL_CALL dispose();
		virtual void SAL_CALL addEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener);
		virtual void SAL_CALL removeEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener);

		// ::com::sun::star::beans::XFastPropertySet
		virtual void SAL_CALL setFastPropertyValue(sal_Int32 nHandle, const ::com::sun::star::uno::Any& aValue);
		virtual ::com::sun::star::uno::Any SAL_CALL getFastPropertyValue(sal_Int32 nHandle);

		// ::com::sun::star::container::XNamed
		virtual ::rtl::OUString SAL_CALL getName();
		virtual void SAL_CALL setName(const ::rtl::OUString& aName);

		// ::com::sun::star::io::XPersistObject
		virtual ::rtl::OUString SAL_CALL getServiceName();
		virtual void SAL_CALL write(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream >& _rxOutStream);
		virtual void SAL_CALL read(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream >& _rxInStream);

		// ::com::sun::star::beans::XMultiPropertySet
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >  SAL_CALL getPropertySetInfo();
		virtual void SAL_CALL setPropertyValues(const ::com::sun::star::uno::Sequence< ::rtl::OUString >& PropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& Values);
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getPropertyValues(const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames);
		virtual void SAL_CALL addPropertiesChangeListener(const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener);
		virtual void SAL_CALL removePropertiesChangeListener(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& Listener);
		virtual void SAL_CALL firePropertiesChangeEvent(const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener);

		// ::com::sun::star::beans::XPropertySet
		virtual void SAL_CALL setPropertyValue(const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue);
		virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(const ::rtl::OUString& PropertyName);
		virtual void SAL_CALL addPropertyChangeListener(const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener);
		virtual void SAL_CALL removePropertyChangeListener(const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener);
		virtual void SAL_CALL addVetoableChangeListener(const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener);
		virtual void SAL_CALL removeVetoableChangeListener(const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener);

		// ::com::sun::star::util::XCancellable
		virtual void SAL_CALL cancel();

		// ::com::sun::star::beans::XPropertyState
		virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState(const ::rtl::OUString& PropertyName);
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL getPropertyStates(const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyName);
		virtual void SAL_CALL setPropertyToDefault(const ::rtl::OUString& PropertyName);
		virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault(const ::rtl::OUString& aPropertyName);

		// ::com::sun::star::form::XReset
		virtual void SAL_CALL reset();
		virtual void SAL_CALL addResetListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XResetListener >& aListener);
		virtual void SAL_CALL removeResetListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XResetListener >& aListener);

		// ::com::sun::star::container::XNameContainer
		virtual void SAL_CALL insertByName(const ::rtl::OUString& aName, const ::com::sun::star::uno::Any& aElement);
		virtual void SAL_CALL removeByName(const ::rtl::OUString& Name);

		// ::com::sun::star::container::XNameReplace
		virtual void SAL_CALL replaceByName(const ::rtl::OUString& aName, const ::com::sun::star::uno::Any& aElement);

		// ::com::sun::star::container::XNameAccess
		virtual ::com::sun::star::uno::Any SAL_CALL getByName(const ::rtl::OUString& aName);
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
		virtual sal_Bool SAL_CALL hasByName(const ::rtl::OUString& aName);

		// ::com::sun::star::container::XElementAccess
		virtual ::com::sun::star::uno::Type SAL_CALL getElementType() ;
		virtual sal_Bool SAL_CALL hasElements();

		// ::com::sun::star::container::XIndexContainer
		virtual void SAL_CALL insertByIndex(sal_Int32 _rIndex, const ::com::sun::star::uno::Any& Element);
		virtual void SAL_CALL removeByIndex(sal_Int32 _rIndex);

		// ::com::sun::star::container::XIndexReplace
		virtual void SAL_CALL replaceByIndex(sal_Int32 _rIndex, const ::com::sun::star::uno::Any& Element);

		// ::com::sun::star::container::XIndexAccess
		virtual sal_Int32 SAL_CALL getCount();
		virtual ::com::sun::star::uno::Any SAL_CALL getByIndex(sal_Int32 _rIndex);

		// ::com::sun::star::container::XContainer
		virtual void SAL_CALL addContainerListener(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener >& xListener);
		virtual void SAL_CALL removeContainerListener(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XContainerListener >& xListener);

		// ::com::sun::star::container::XEnumerationAccess
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration >  SAL_CALL createEnumeration();

		// ::com::sun::star::lang::XEventListener
		virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source);

		// ::com::sun::star::beans::XPropertyChangeListener
		virtual void SAL_CALL propertyChange(const ::com::sun::star::beans::PropertyChangeEvent& evt);

	protected:
		// container handling
		void implInsert(const ::com::sun::star::uno::Any& aElement, sal_Int32 nIndex, const ::rtl::OUString* pNewElName = NULL);
		sal_Int32 implGetPos(const ::rtl::OUString& rName);

		void StopListening();
		void StartListening();
	};
}
#endif // _SBA_FORMADAPTER_HXX
