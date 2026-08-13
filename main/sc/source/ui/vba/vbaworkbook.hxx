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
	static css::uno::Sequence< sal_Int32 > ColorData;
	void initColorData( const css::uno::Sequence< sal_Int32 >& sColors );
	void init();
protected:

	ScVbaWorkbook( 	const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext);
public:
	ScVbaWorkbook( 	const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext,
			css::uno::Reference< css::frame::XModel > xModel );
	ScVbaWorkbook( 	css::uno::Sequence< css::uno::Any > const& aArgs, css::uno::Reference< css::uno::XComponentContext >const& xContext );
	virtual ~ScVbaWorkbook() {}

    // Attributes
	virtual ::sal_Bool SAL_CALL getProtectStructure();
	virtual css::uno::Reference< ov::excel::XWorksheet > SAL_CALL getActiveSheet();
	virtual ::sal_Bool SAL_CALL getPrecisionAsDisplayed();
	virtual void SAL_CALL setPrecisionAsDisplayed( sal_Bool _precisionAsDisplayed );

	// Methods
	virtual css::uno::Any SAL_CALL Worksheets( const css::uno::Any& aIndex );
	virtual css::uno::Any SAL_CALL Sheets( const css::uno::Any& aIndex );
	virtual css::uno::Any SAL_CALL Windows( const css::uno::Any& aIndex );
    virtual void SAL_CALL Activate();
    // Amelia Wang
    virtual css::uno::Any SAL_CALL Names( const css::uno::Any& aIndex );

	virtual css::uno::Any SAL_CALL Styles( const css::uno::Any& Item );
	virtual void SAL_CALL ResetColors(  );
	virtual css::uno::Any SAL_CALL Colors( const css::uno::Any& Index );
	virtual ::sal_Int32 SAL_CALL FileFormat(  );
	virtual void SAL_CALL SaveCopyAs( const rtl::OUString& Filename );
    // code name
    virtual ::rtl::OUString SAL_CALL getCodeName();

	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif /* SC_VBA_WORKBOOK_HXX */
