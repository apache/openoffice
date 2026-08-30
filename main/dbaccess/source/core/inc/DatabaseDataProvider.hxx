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


#ifndef DBA_DATABASEDATAPROVIDER_HXX_INCLUDED
#define DBA_DATABASEDATAPROVIDER_HXX_INCLUDED

#include "sal/config.h"

#include "com/sun/star/uno/XComponentContext.hpp"
#include "com/sun/star/lang/XServiceInfo.hpp"
#include "com/sun/star/chart2/data/XDatabaseDataProvider.hpp"
#include "com/sun/star/chart2/XInternalDataProvider.hpp"
#include <com/sun/star/chart/XComplexDescriptionAccess.hpp>
#include <com/sun/star/sdbc/XRowSet.hpp>
#include <com/sun/star/sdbc/XParameters.hpp>
#include <com/sun/star/container/XChild.hpp>

#include "cppuhelper/compbase4.hxx"
#include "cppuhelper/basemutex.hxx"
#include "cppuhelper/propertysetmixin.hxx"
#include <cppuhelper/implementationentry.hxx>

#include <comphelper/sequence.hxx>

#include "connectivity/parameters.hxx"
#include "connectivity/filtermanager.hxx"


namespace dbaccess
{

class DatabaseDataSource;
typedef ::cppu::WeakComponentImplHelper4<   ::com::sun::star::chart2::data::XDatabaseDataProvider
                                          , ::com::sun::star::container::XChild
                                          , ::com::sun::star::chart::XComplexDescriptionAccess
                                          , ::com::sun::star::lang::XServiceInfo > TDatabaseDataProvider;

class DatabaseDataProvider: private ::cppu::BaseMutex,
                            public TDatabaseDataProvider,
                            public ::cppu::PropertySetMixin< ::com::sun::star::chart2::data::XDatabaseDataProvider >
{
public:
    explicit DatabaseDataProvider(::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & context);

    // ::com::sun::star::lang::XServiceInfo - static methods
	static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static(void);
	static ::rtl::OUString getImplementationName_Static(void);
	static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
		SAL_CALL Create(::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > const & context);

private:
    // ::com::sun::star::uno::XInterface:
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(::com::sun::star::uno::Type const & type);
    virtual void SAL_CALL acquire() throw ()
        { TDatabaseDataProvider::acquire(); }
    virtual void SAL_CALL release() throw ()
        { TDatabaseDataProvider::release(); }

    // ::com::sun::star::lang::XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

    // ::com::sun::star::chart2::data::XDataProvider:
    virtual ::sal_Bool SAL_CALL createDataSourcePossible(const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > & aArguments);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSource > SAL_CALL createDataSource(const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > & aArguments);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL detectArguments(const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSource > & xDataSource);
    virtual ::sal_Bool SAL_CALL createDataSequenceByRangeRepresentationPossible(const ::rtl::OUString & aRangeRepresentation);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSequence > SAL_CALL createDataSequenceByRangeRepresentation(const ::rtl::OUString & aRangeRepresentation);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XRangeSelection > SAL_CALL getRangeSelection();

    // ::com::sun::star::chart2::data::XRangeXMLConversion:
    virtual ::rtl::OUString SAL_CALL convertRangeToXML(const ::rtl::OUString & aRangeRepresentation);
    virtual ::rtl::OUString SAL_CALL convertRangeFromXML(const ::rtl::OUString & aXMLRange);

    // ::com::sun::star::lang::XInitialization:
    virtual void SAL_CALL initialize(const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > & aArguments);

    // ::com::sun::star::beans::XPropertySet:
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo();
    virtual void SAL_CALL setPropertyValue(const ::rtl::OUString & aPropertyName, const ::com::sun::star::uno::Any & aValue);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(const ::rtl::OUString & PropertyName);
    virtual void SAL_CALL addPropertyChangeListener(const ::rtl::OUString & aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener > & xListener);
    virtual void SAL_CALL removePropertyChangeListener(const ::rtl::OUString & aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener > & aListener);
    virtual void SAL_CALL addVetoableChangeListener(const ::rtl::OUString & PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener > & aListener);
    virtual void SAL_CALL removeVetoableChangeListener(const ::rtl::OUString & PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener > & aListener);

    // ::com::sun::star::chart2::data::XDatabaseDataProvider:
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getMasterFields();
    virtual void SAL_CALL setMasterFields(const ::com::sun::star::uno::Sequence< ::rtl::OUString > & the_value);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getDetailFields();
    virtual void SAL_CALL setDetailFields(const ::com::sun::star::uno::Sequence< ::rtl::OUString > & the_value);
    virtual ::rtl::OUString SAL_CALL getCommand();
    virtual void SAL_CALL setCommand(const ::rtl::OUString & the_value);
    virtual ::sal_Int32 SAL_CALL getCommandType();
    virtual void SAL_CALL setCommandType(::sal_Int32 the_value);
    virtual ::rtl::OUString SAL_CALL getFilter();
    virtual void SAL_CALL setFilter(const ::rtl::OUString & the_value);
    virtual ::sal_Bool SAL_CALL getApplyFilter();
    virtual void SAL_CALL setApplyFilter( ::sal_Bool _applyfilter );
    virtual ::rtl::OUString SAL_CALL getHavingClause();
    virtual void SAL_CALL setHavingClause( const ::rtl::OUString& _havingclause );
    virtual ::rtl::OUString SAL_CALL getGroupBy();
    virtual void SAL_CALL setGroupBy( const ::rtl::OUString& _groupby );
    virtual ::rtl::OUString SAL_CALL getOrder();
    virtual void SAL_CALL setOrder( const ::rtl::OUString& _order );
    virtual ::sal_Bool SAL_CALL getEscapeProcessing();
    virtual void SAL_CALL setEscapeProcessing(::sal_Bool the_value);
    virtual ::sal_Int32 SAL_CALL getRowLimit();
    virtual void SAL_CALL setRowLimit( ::sal_Int32 _rowlimit );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > SAL_CALL getActiveConnection();
    virtual void SAL_CALL setActiveConnection(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > & the_value);
    virtual ::rtl::OUString SAL_CALL getDataSourceName();
    virtual void SAL_CALL setDataSourceName( const ::rtl::OUString& _datasourcename );

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

    // com::sun::star::sdbc::XRowSet
    virtual void SAL_CALL execute();
    virtual void SAL_CALL addRowSetListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSetListener>& _rxListener);
    virtual void SAL_CALL removeRowSetListener(const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSetListener>& _rxListener);

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
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getStatement();

    // container::XChild
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getParent(  );
    virtual void SAL_CALL setParent( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& Parent );

    // ____ XComplexDescriptionAccess ____
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::rtl::OUString > > SAL_CALL        getComplexRowDescriptions();
    virtual void SAL_CALL setComplexRowDescriptions(        const ::com::sun::star::uno::Sequence<        ::com::sun::star::uno::Sequence< ::rtl::OUString > >& aRowDescriptions );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::rtl::OUString > > SAL_CALL        getComplexColumnDescriptions();
    virtual void SAL_CALL setComplexColumnDescriptions(         const ::com::sun::star::uno::Sequence<        ::com::sun::star::uno::Sequence< ::rtl::OUString > >& aColumnDescriptions );

    // ____ XChartDataArray (base of XComplexDescriptionAccess) ____
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< double > > SAL_CALL getData();
    virtual void SAL_CALL setData(        const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< double > >& aData );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getRowDescriptions();
    virtual void SAL_CALL setRowDescriptions(        const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aRowDescriptions );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getColumnDescriptions();
    virtual void SAL_CALL setColumnDescriptions(        const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aColumnDescriptions );

    // ____ XChartData (base of XChartDataArray) ____
    virtual void SAL_CALL addChartDataChangeEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartDataChangeEventListener >& aListener );
    virtual void SAL_CALL removeChartDataChangeEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartDataChangeEventListener >& aListener );
    virtual double SAL_CALL getNotANumber();
    virtual ::sal_Bool SAL_CALL isNotANumber(double nNumber );
private:
    DatabaseDataProvider(DatabaseDataProvider &); // not defined
    void operator =(DatabaseDataProvider &); // not defined

    virtual ~DatabaseDataProvider() {}

    // This function is called upon disposing the component,
    // if your component needs special work when it becomes
    // disposed, do it here.
    virtual void SAL_CALL disposing();

    void impl_fillRowSet_throw();
    void impl_executeRowSet_throw(::osl::ResettableMutexGuard& _rClearForNotifies);
    bool impl_fillParameters_nothrow( ::osl::ResettableMutexGuard& _rClearForNotifies);
    void impl_fillInternalDataProvider_throw(sal_Bool _bHasCategories,const ::com::sun::star::uno::Sequence< ::rtl::OUString >& i_aColumnNames);
    void impl_invalidateParameter_nothrow();
    ::com::sun::star::uno::Any impl_getNumberFormatKey_nothrow(const ::rtl::OUString & _sRangeRepresentation) const;

    template <typename T> void set(	 const ::rtl::OUString& _sProperty
										,const T& _Value
										,T& _member)
	{
		BoundListeners l;
		{
			::osl::MutexGuard aGuard(m_aMutex);
            if ( _member != _Value )
            {
			    prepareSet(_sProperty, ::com::sun::star::uno::makeAny(_member), ::com::sun::star::uno::makeAny(_Value), &l);
			    _member = _Value;
            }
		}
		l.notify();
	}

    ::dbtools::ParameterManager m_aParameterManager;
    ::dbtools::FilterManager    m_aFilterManager;
    ::std::map< ::rtl::OUString, ::com::sun::star::uno::Any>                                m_aNumberFormats;

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >            m_xContext;
    ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >                 m_xActiveConnection;
    ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet >                     m_xRowSet;
    ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XInternalDataProvider >     m_xInternal;
    ::com::sun::star::uno::Reference< ::com::sun::star::chart::XComplexDescriptionAccess >  m_xComplexDescriptionAccess;
    ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XRangeXMLConversion>  m_xRangeConversion;
    ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler>          m_xHandler;
    // the object doin' most of the work - an SDB-rowset
	::com::sun::star::uno::Reference< ::com::sun::star::uno::XAggregation> 		            m_xAggregate;
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>                m_xAggregateSet;
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface> 		            m_xParent;
    ::com::sun::star::uno::Sequence< ::rtl::OUString >                                      m_MasterFields;
    ::com::sun::star::uno::Sequence< ::rtl::OUString >                                      m_DetailFields;

    ::rtl::OUString     m_Command;
    ::rtl::OUString     m_DataSourceName;
    ::sal_Int32         m_CommandType;
    sal_Int32           m_RowLimit;
    ::rtl::OUString     m_Filter;
    ::rtl::OUString     m_HavingClause;
    ::rtl::OUString     m_Order;
    ::rtl::OUString     m_GroupBy;
    ::sal_Bool          m_EscapeProcessing;
    ::sal_Bool          m_ApplyFilter;
};

} // namespace dbaccess

#endif // DBA_DATABASEDATAPROVIDER_HXX_INCLUDED
