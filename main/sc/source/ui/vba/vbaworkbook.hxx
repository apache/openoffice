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


#ifndef SC_VBA_WORKBOOK_HXX
#define SC_VBA_WORKBOOK_HXX

#include <com/sun/star/frame/XModel.hpp>
#include <ooo/vba/excel/XWorkbook.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <vbahelper/vbadocumentbase.hxx>

class ScModelObj;

typedef cppu::ImplInheritanceHelper1< VbaDocumentBase, ov::excel::XWorkbook > ScVbaWorkbook_BASE;

class ScVbaWorkbook : public ScVbaWorkbook_BASE
{
	// MS Excel VBA: Static color data array shared across all workbook instances.
	static css::uno::Sequence< sal_Int32 > ColorData;
    // MS Excel VBA: Initializes the color data from the default palette.
    // Status: implemented
	void initColorData( const css::uno::Sequence< sal_Int32 >& sColors );
    // MS Excel VBA: Initializes this workbook (resets colors if needed).
    // Status: implemented
	void init();
protected:

    // MS Excel VBA: Protected constructor for use as ActiveSheet base class.
    // Status: implemented
	ScVbaWorkbook( 	const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext);
public:
    // MS Excel VBA: Constructs the Workbook object with parent, context, and model.
    // Status: implemented
	ScVbaWorkbook( 	const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext,
			css::uno::Reference< css::frame::XModel > xModel );
    // MS Excel VBA: Constructs the Workbook object from arguments (for service instantiation).
    // Status: implemented
	ScVbaWorkbook( 	css::uno::Sequence< css::uno::Any > const& aArgs, css::uno::Reference< css::uno::XComponentContext >const& xContext );
    // MS Excel VBA: Destructor.
    // Status: implemented
	virtual ~ScVbaWorkbook() {}

    // Attributes
    // MS Excel VBA: Returns whether the workbook structure is protected (VBA project).
    // Status: implemented - queries UNO XProtectable
	virtual ::sal_Bool SAL_CALL getProtectStructure();
    // MS Excel VBA: Returns the active sheet in this workbook.
    // Status: implemented - returns the XWorksheet from the view or creates one
	virtual css::uno::Reference< ov::excel::XWorksheet > SAL_CALL getActiveSheet();
    // MS Excel VBA: Returns whether precision is set as displayed.
    // Status: implemented - reads from document options
	virtual ::sal_Bool SAL_CALL getPrecisionAsDisplayed();
    // MS Excel VBA: Sets whether precision is set as displayed.
    // Status: implemented - sets in document options
	virtual void SAL_CALL setPrecisionAsDisplayed( sal_Bool _precisionAsDisplayed );

	// Methods
    // MS Excel VBA: Returns the Worksheets collection or a single worksheet by index.
    // Status: implemented - delegates to ScVbaWorksheets
	virtual css::uno::Any SAL_CALL Worksheets( const css::uno::Any& aIndex );
    // MS Excel VBA: Returns the Sheets collection or a single sheet by index.
    // Status: implemented - aliases to Worksheets()
	virtual css::uno::Any SAL_CALL Sheets( const css::uno::Any& aIndex );
    // MS Excel VBA: Returns the Windows collection or a single window by index.
    // Status: implemented - delegates to ScVbaWindows
	virtual css::uno::Any SAL_CALL Windows( const css::uno::Any& aIndex );
    // MS Excel VBA: Activates this workbook.
    // Status: implemented - delegates to VbaDocumentBase::Activate
    virtual void SAL_CALL Activate();
    // MS Excel VBA: Returns the Names collection or a single name by index.
    // Status: implemented - delegates to ScVbaNames
    // Amelia Wang
    virtual css::uno::Any SAL_CALL Names( const css::uno::Any& aIndex );

    // MS Excel VBA: Returns the Styles collection or a single style by index.
    // Status: implemented - delegates to ScVbaStyles
	virtual css::uno::Any SAL_CALL Styles( const css::uno::Any& Item );
    // MS Excel VBA: Resets the color palette to its default values.
    // Status: implemented - loads default palette from ScVbaPalette
	virtual void SAL_CALL ResetColors(  );
    // MS Excel VBA: Returns the color at the given index, or all colors if Index is omitted.
    // Status: implemented - uses ColorData array
	virtual css::uno::Any SAL_CALL Colors( const css::uno::Any& Index );
    // MS Excel VBA: Returns the file format of the workbook.
    // Status: implemented - maps filter name to XlFileFormat constant
	virtual ::sal_Int32 SAL_CALL FileFormat(  );
    // MS Excel VBA: Saves a copy of the workbook to the specified filename.
    // Status: implemented - uses storeToURL with MS Excel 97 filter
	virtual void SAL_CALL SaveCopyAs( const rtl::OUString& Filename );
    // MS Excel VBA: Saves the workbook with specified options.
    // Status: partial - only uses FileName and FileFormat parameters; Password/WriteResPassword/ReadOnlyRecommended/CreateBackup ignored
	virtual void SAL_CALL SaveAs( const css::uno::Any& FileName, const css::uno::Any& FileFormat, const css::uno::Any& Password, const css::uno::Any& WriteResPassword, const css::uno::Any& ReadOnlyRecommended, const css::uno::Any& CreateBackup );
    // code name
    // MS Excel VBA: Returns the code name of the workbook.
    // Status: implemented - reads "CodeName" property from model
    virtual ::rtl::OUString SAL_CALL getCodeName();

	// XHelperInterface
    // MS Excel VBA: Returns the implementation name of this service.
    // Status: implemented - returns "ScVbaWorkbook"
	virtual rtl::OUString& getServiceImplName();
    // MS Excel VBA: Returns the service names supported by this implementation.
    // Status: implemented - returns "ooo.vba.excel.Workbook"
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif /* SC_VBA_WORKBOOK_HXX */
