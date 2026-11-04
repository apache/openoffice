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


#ifndef SC_VBA_FORMATCONDITIONS_HXX
#define SC_VBA_FORMATCONDITIONS_HXX
#include <ooo/vba/excel/XFormatConditions.hpp>
#include <ooo/vba/excel/XStyles.hpp>
#include <ooo/vba/excel/XRange.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/table/CellAddress.hpp>
#include <com/sun/star/sheet/XSheetConditionalEntries.hpp>
#include <vbahelper/vbacollectionimpl.hxx>

typedef CollTestImplHelper< ov::excel::XFormatConditions > ScVbaFormatConditions_BASE;
// MS Excel VBA: Collection of FormatCondition objects for a range of cells.
// Status: implemented - wraps UNO XSheetConditionalEntries
class ScVbaFormatConditions: public ScVbaFormatConditions_BASE
{
    css::table::CellAddress maCellAddress;
    css::uno::Reference< css::sheet::XSheetConditionalEntries > mxSheetConditionalEntries;
    css::uno::Reference< ov::excel::XStyles > mxStyles;
    css::uno::Reference< ov::excel::XRange > mxRangeParent;
    css::uno::Reference< css::beans::XPropertySet > mxParentRangePropertySet;
public:
    // MS Excel VBA: Constructs FormatConditions with parent, context, conditional entries, and model.
    // Status: implemented
    ScVbaFormatConditions( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext > & xContext, const css::uno::Reference< css::sheet::XSheetConditionalEntries >&, const css::uno::Reference< css::frame::XModel >&  );
    // MS Excel VBA: Factory constructor for service instantiation.
    // Status: implemented
    ScVbaFormatConditions( const css::uno::Sequence< css::uno::Any >& /*args*/, const css::uno::Reference< css::uno::XComponentContext >& xContext );
    // MS Excel VBA: Notifies the parent range that conditions changed.
    // Status: implemented
    void notifyRange();
    // MS Excel VBA: Adds a new FormatCondition to the collection with style.
    // Status: implemented - creates with Type, Operator, Formula1, Formula2, and XlStyle
    virtual css::uno::Reference< ov::excel::XFormatCondition > Add( ::sal_Int32 Type, const css::uno::Any& Operator, const css::uno::Any& Formula1, const css::uno::Any& Formula2, const css::uno::Reference< ov::excel::XStyle >& _xCalcStyle );
    // MS Excel VBA: Converts a VBA formula reference to A1 notation.
    // Status: implemented
    rtl::OUString getA1Formula(const css::uno::Any& _aFormula);
    // MS Excel VBA: Returns the style name for this collection.
    // Status: implemented
    rtl::OUString getStyleName();
    // MS Excel VBA: Removes a format condition by style name.
    // Status: implemented
    void removeFormatCondition( const rtl::OUString& _sStyleName, sal_Bool _bRemoveStyle);
    // MS Excel VBA: Returns the underlying conditional entries container.
    // Status: implemented
    css::uno::Reference< css::sheet::XSheetConditionalEntries > getSheetConditionalEntries() { return mxSheetConditionalEntries; }
    // XFormatConditions
    // MS Excel VBA: Deletes all conditional formatting conditions in this collection.
    // Status: implemented
    virtual void SAL_CALL Delete(  );
    // MS Excel VBA: Adds a new FormatCondition to the collection.
    // Status: implemented - Type, Operator, Formula1, Formula2 map to UNO condition
    virtual css::uno::Reference< ov::excel::XFormatCondition > SAL_CALL Add( ::sal_Int32 Type, const css::uno::Any& Operator, const css::uno::Any& Formula1, const css::uno::Any& Formula2 );
    // XEnumerationAccess
    // MS Excel VBA: Returns the type of objects in this collection.
    // Status: implemented
    virtual css::uno::Type SAL_CALL getElementType();
    // MS Excel VBA: Creates an enumeration for iterating the collection.
    // Status: implemented
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration();
    // MS Excel VBA: Creates a new FormatCondition object from a source Any.
    // Status: implemented
    virtual css::uno::Any createCollectionObject(const css::uno::Any&);
    // XHelperInterface
    // MS Excel VBA: Returns the implementation name for UNO service identification.
    // Status: implemented - returns "ScVbaFormatConditions"
    virtual rtl::OUString& getServiceImplName();
    // MS Excel VBA: Returns the service names this implementation supports.
    // Status: implemented - returns "ooo.vba.excel.FormatConditions"
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif //SC_VBA_AXES_HXX
