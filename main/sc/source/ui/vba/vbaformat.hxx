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


#ifndef SC_VBA_FORMAT_HXX
#define SC_VBA_FORMAT_HXX
#include <ooo/vba/excel/XFormat.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#include <com/sun/star/util/XNumberFormats.hpp>
#include <com/sun/star/util/XNumberFormatTypes.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/Locale.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <vbahelper/vbahelperinterface.hxx>

class ScCellRangesBase;

template< typename Ifc1 >
class ScVbaFormat : public InheritedHelperInterfaceImpl1< Ifc1 >
{
typedef InheritedHelperInterfaceImpl1< Ifc1 > ScVbaFormat_BASE;
	css::lang::Locale m_aDefaultLocale;
protected:
	css::lang::Locale getDefaultLocale() { return m_aDefaultLocale; }
	css::uno::Reference< css::beans::XPropertySet > mxPropertySet;
	css::uno::Reference< css::util::XNumberFormatsSupplier > mxNumberFormatsSupplier;
	css::uno::Reference< css::util::XNumberFormats > xNumberFormats;
	css::uno::Reference< css::util::XNumberFormatTypes > xNumberFormatTypes;
	css::uno::Reference< css::frame::XModel > mxModel;
	css::uno::Reference< css::beans::XPropertyState > xPropertyState;
	sal_Bool mbCheckAmbiguoity;
	sal_Bool mbAddIndent;
	//NumberFormatter oNumberFormatter = null;
	css::uno::Reference< css::lang::XMultiServiceFactory > xMultiServiceFactory;
	bool isAmbiguous(const rtl::OUString& _sPropertyName);
	css::uno::Reference< css::beans::XPropertyState > getXPropertyState();
	void initializeNumberFormats();
	void setNumberFormat( css::lang::Locale _aLocale, const rtl::OUString& _sFormatString);
	SfxItemSet*  getCurrentDataSet( );
protected:
	virtual ScCellRangesBase* getCellRangesBase();
public:
	ScVbaFormat( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext > & xContext, const css::uno::Reference< css::beans::XPropertySet >& _xPropertySet, const css::uno::Reference< css::frame::XModel >& xModel, bool bCheckAmbiguoity );
	virtual ~ScVbaFormat() {}
	virtual css::uno::Reference< ov::XHelperInterface > thisHelperIface() = 0;
	 void SAL_CALL setAddIndent( const css::uno::Any& _BAddIndent) { _BAddIndent >>= mbAddIndent; }
	 css::uno::Any SAL_CALL getAddIndent() { return css::uno::makeAny( mbAddIndent ); }
    	// Interface Methods
	virtual css::uno::Any SAL_CALL Borders( const css::uno::Any& Index );
	virtual css::uno::Reference< ::ooo::vba::excel::XFont > SAL_CALL Font(  );
	virtual css::uno::Reference< ::ooo::vba::excel::XInterior > SAL_CALL Interior(  );
	virtual void SAL_CALL setNumberFormat( const css::uno::Any& NumberFormat );
	virtual css::uno::Any SAL_CALL getNumberFormat(  );
	virtual void SAL_CALL setNumberFormatLocal( const css::uno::Any& NumberFormatLocal );
	virtual css::uno::Any SAL_CALL getNumberFormatLocal(  );
	virtual void SAL_CALL setIndentLevel( const css::uno::Any& IndentLevel );
	virtual css::uno::Any SAL_CALL getIndentLevel(  );
	virtual void SAL_CALL setHorizontalAlignment( const css::uno::Any& HorizontalAlignment );
	virtual css::uno::Any SAL_CALL getHorizontalAlignment(  );
	virtual void SAL_CALL setVerticalAlignment( const css::uno::Any& VerticalAlignment );
	virtual css::uno::Any SAL_CALL getVerticalAlignment(  );
	virtual void SAL_CALL setOrientation( const css::uno::Any& Orientation );
	virtual css::uno::Any SAL_CALL getOrientation(  );
	virtual void SAL_CALL setShrinkToFit( const css::uno::Any& ShrinkToFit );
	virtual css::uno::Any SAL_CALL getShrinkToFit(  );
	virtual void SAL_CALL setWrapText( const css::uno::Any& WrapText );
	virtual css::uno::Any SAL_CALL getWrapText(  );
	virtual void SAL_CALL setLocked( const css::uno::Any& Locked );
	virtual css::uno::Any SAL_CALL getLocked(  );
	virtual void SAL_CALL setFormulaHidden( const css::uno::Any& FormulaHidden );
	virtual css::uno::Any SAL_CALL getFormulaHidden(  );
	virtual void SAL_CALL setMergeCells( const css::uno::Any& MergeCells ) = 0;
	virtual css::uno::Any SAL_CALL getMergeCells(  ) = 0;
	virtual void SAL_CALL setReadingOrder( const css::uno::Any& ReadingOrder );
	virtual css::uno::Any SAL_CALL getReadingOrder(  );
	// XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif
