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

#ifndef SC_VBA_QUERYTABLE_HXX
#define SC_VBA_QUERYTABLE_HXX

#include <ooo/vba/excel/XQueryTable.hpp>
#include <vbahelper/vbahelperinterface.hxx>

namespace css = ::com::sun::star;

// MS Excel VBA: Represents a worksheet table built from data returned from an external data source
// (e.g. SQL server or Microsoft Access database).
// Status: partial - only Refresh and Delete implemented per XQueryTable.idl.
// Missing (per MS docs): CommandText, CommandType, Connection, Destination, Name, Parameters,
// BackgroundQuery, RefreshOnFileOpen, RefreshPeriod, RefreshStyle, SaveData, SavePassword,
// EnableEditing, EnableRefresh, FieldNames, FillAdjacentFormulas, PreserveFormatting,
// AdjustColumnWidth, ListObject, WorkbookConnection, and TextFile*/Web* import properties.
// Events not implemented: AfterRefresh, BeforeRefresh.
typedef InheritedHelperInterfaceImpl1< ooo::vba::excel::XQueryTable > ScVbaQueryTableImpl_BASE;

class ScDocument;
#include <com/sun/star/uno/Sequence.hxx>
class ScVbaRange;

class ScVbaQueryTable : public ScVbaQueryTableImpl_BASE
{
private:
    ScDocument* m_pDocument;
    ScVbaRange* m_pParent;
public:
    // MS Excel VBA: Constructs QueryTable with parent, context, document, and range.
    // Status: implemented
    ScVbaQueryTable( const css::uno::Reference< ov::XHelperInterface >& xParent,
            const css::uno::Reference< css::uno::XComponentContext >& xContext,
            ScDocument* pDocument = NULL,
            ScVbaRange* pParent = NULL );
    // MS Excel VBA: Factory constructor for service instantiation.
    // Status: implemented
    ScVbaQueryTable( const css::uno::Sequence< css::uno::Any >& /*args*/,
            const css::uno::Reference< css::uno::XComponentContext >& xContext );
    // MS Excel VBA: Destructor.
    // Status: implemented
    virtual ~ScVbaQueryTable();

    // MS Excel VBA: Methods - QueryTable actions
    // MS VBA: Updates an external data range in a QueryTable object.
    // Parameters: BackgroundQuery - Used only with QueryTables based on SQL query results.
    //   True to return control as soon as connection is made and query submitted (background).
    //   False to return control only after all data has been fetched.
    //   If omitted, the BackgroundQuery property setting determines the query mode.
    // Return value: Boolean - True if query is successfully completed or started; False if
    //   user cancels a connection or parameter dialog box.
    // Remarks (MS): Refresh causes Excel to connect to the data source, execute the SQL query,
    //   and return data to the range. Unless called, the QueryTable doesn't communicate with
    //   the data source.
    // Status: partial - checks for area links in the destination range and updates them;
    //   returns True if a link was found/updated, False otherwise. Does not implement full
    //   SQL/ODBC connection semantics, parameter binding, or BackgroundQuery async behavior.
    virtual sal_Bool SAL_CALL Refresh( const css::uno::Any& aBackgroundQuery )
        throw ( css::uno::RuntimeException );
    // MS VBA: Deletes the QueryTable object.
    // Status: implemented - removes matching area link from LinkManager for the parent range.
    virtual void SAL_CALL Delete() throw ( css::uno::RuntimeException );

    // XHelperInterface
    // MS Excel VBA: Returns the implementation name for UNO service identification.
    // Status: implemented - returns "ScVbaQueryTable"
    virtual rtl::OUString& getServiceImplName();
    // MS Excel VBA: Returns the service names this implementation supports.
    // Status: implemented - returns "ooo.vba.excel.QueryTable"
    virtual css::uno::Sequence< rtl::OUString > getServiceNames();
};

#endif
