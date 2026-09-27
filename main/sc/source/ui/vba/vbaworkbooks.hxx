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


#ifndef SC_VBA_WORKBOOKS_HXX
#define SC_VBA_WORKBOOKS_HXX


#include <vbahelper/vbacollectionimpl.hxx>
#include <ooo/vba/excel/XWorkbooks.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <vbahelper/vbadocumentsbase.hxx>
#include "excelvbahelper.hxx"


class ScModelObj;

typedef cppu::ImplInheritanceHelper1< VbaDocumentsBase, ov::excel::XWorkbooks > ScVbaWorkbooks_BASE;

class ScVbaWorkbooks : public ScVbaWorkbooks_BASE
{
private:
    // MS Excel VBA: Maps a file type string to a Calc filter type string.
    // Status: implemented
	rtl::OUString 	getFileFilterType( const rtl::OUString& rString );
    // MS Excel VBA: Returns true if the file is a text or CSV file (or unknown).
    // Status: implemented
	bool 	isTextFile( const rtl::OUString& rString );
    // MS Excel VBA: Returns true if the file is a Calc spreadsheet file.
    // Status: implemented
	bool 	isSpreadSheetFile( const rtl::OUString& rString );
    // MS Excel VBA: Returns a reference to the current delimiter (default comma).
    // Status: implemented
	static sal_Int16& getCurrentDelim(){ static sal_Int16 nDelim = 44; return nDelim; }
public:
    // MS Excel VBA: Constructs the Workbooks collection.
    // Status: implemented
	ScVbaWorkbooks( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext );
    // MS Excel VBA: Destructor.
    // Status: implemented
	virtual ~ScVbaWorkbooks() {}

	// XEnumerationAccess
    // MS Excel VBA: Returns the element type of the collection (XWorkbook).
    // Status: implemented
	virtual css::uno::Type SAL_CALL getElementType();
    // MS Excel VBA: Creates an enumeration over all open workbooks.
    // Status: implemented
	virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration();

	// ScVbaWorkbooks_BASE
    // MS Excel VBA: Creates a VBA Workbook object from a data source.
    // Status: implemented
	virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource );
    // MS Excel VBA: Returns the implementation name of this service.
    // Status: implemented - returns "ScVbaWorkbooks"
	virtual rtl::OUString& getServiceImplName();
    // MS Excel VBA: Returns the service names supported by this implementation.
    // Status: implemented - returns "ooo.vba.excel.Workbooks"
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();

	// XWorkbooks
    // MS Excel VBA: Creates and adds a new workbook to the collection.
    // Status: partial - supports Template type (0 = default, new sheets) and filename; chart sheets not supported
	virtual css::uno::Any SAL_CALL Add( const css::uno::Any& Template );
    // MS Excel VBA: Closes all open workbooks.
    // Status: implemented - delegates to closeDocuments()
	virtual void SAL_CALL Close(  );
    // MS Excel VBA: Opens a workbook file and returns it.
    // Status: partial - supports text/CSV and spreadsheet files; many parameters ignored (UpdateLinks, Password, etc.)
	virtual css::uno::Any SAL_CALL Open( const ::rtl::OUString& Filename, const css::uno::Any& UpdateLinks, const css::uno::Any& ReadOnly, const css::uno::Any& Format, const css::uno::Any& Password, const css::uno::Any& WriteResPassword, const css::uno::Any& IgnoreReadOnlyRecommended, const css::uno::Any& Origin, const css::uno::Any& Delimiter, const css::uno::Any& Editable, const css::uno::Any& Notify, const css::uno::Any& Converter, const css::uno::Any& AddToMru );
};

#endif /* SC_VBA_WORKBOOKS_HXX */
