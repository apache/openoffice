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


#ifndef SC_VBA_FORMATCONDITION_HXX
#define SC_VBA_FORMATCONDITION_HXX
#include <ooo/vba/excel/XFormatCondition.hpp>
#include <ooo/vba/excel/XFormatConditions.hpp>
#include <ooo/vba/excel/XStyle.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/sheet/XSheetConditionalEntries.hpp>
#include <com/sun/star/sheet/XSheetConditionalEntry.hpp>
#include <com/sun/star/sheet/XSheetCondition.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include "vbacondition.hxx"

// MS Excel VBA: A condition that can be checked for a conditional formatting rule.
// Status: implemented - wraps UNO XSheetConditionalEntry
typedef ScVbaCondition< ov::excel::XFormatCondition >  ScVbaFormatCondition_BASE;
class ScVbaFormatCondition : public ScVbaFormatCondition_BASE
{
protected:
    rtl::OUString msStyleName;
    css::uno::Reference< css::sheet::XSheetConditionalEntry > mxSheetConditionalEntry;
    css::uno::Reference< css::sheet::XSheetConditionalEntries > mxSheetConditionalEntries;
    css::uno::Reference< ov::excel::XFormatConditions> moFormatConditions;
    css::uno::Reference< ov::excel::XStyle > mxStyle;
    css::uno::Reference< css::beans::XPropertySet > mxParentRangePropertySet;
public:
    // MS Excel VBA: Constructs a FormatCondition from its parts (parent, context, condition, style, collection, range props).
    // Status: implemented
    ScVbaFormatCondition(  const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext > & xContext, const css::uno::Reference< css::sheet::XSheetConditionalEntry >& _xSheetConditionalEntry, const css::uno::Reference< ov::excel::XStyle >&,  const css::uno::Reference< ov::excel::XFormatConditions >& _xFormatConditions, const css::uno::Reference< css::beans::XPropertySet >& _xPropertySet );
    // MS Excel VBA: Factory constructor for service instantiation.
    // Status: implemented
    ScVbaFormatCondition(  const css::uno::Sequence< css::uno::Any >& /*args*/, const css::uno::Reference< css::uno::XComponentContext >& xContext );

    // MS Excel VBA: Notifies the parent range that a condition changed.
    // Status: implemented
    void notifyRange();
    // MS Excel VBA: Maps a VBA condition type to UNO ConditionOperator.
    // Status: implemented
    static css::sheet::ConditionOperator retrieveAPIType(sal_Int32 _nVBAType, const css::uno::Reference< css::sheet::XSheetCondition >& _xSheetCondition );

    // MS Excel VBA: Methods - FormatCondition actions
    // MS Excel VBA: Deletes the formatting condition and its associated XStyle.
    // Status: implemented
    virtual void SAL_CALL Delete(  );
    // MS Excel VBA: Modifies an existing conditional formatting rule's type and parameters.
    // Status: implemented - updates Type, Operator, Formula1, Formula2
    virtual void SAL_CALL Modify( ::sal_Int32 Type, const css::uno::Any& Operator, const css::uno::Any& Formula1, const css::uno::Any& Formula2 );
    // MS Excel VBA: Returns the condition type (xlCellValue or xlExpression).
    // Status: implemented
    virtual ::sal_Int32 SAL_CALL Type(  );
    // MS Excel VBA: Returns the comparison operator type for the condition.
    // Status: implemented - non-const overload for internal use
    virtual ::sal_Int32 Operator( sal_Bool  );
    // MS Excel VBA: Returns the comparison operator type for the condition.
    // Status: implemented
    virtual ::sal_Int32 SAL_CALL Operator(  );
    // MS Excel VBA: Returns the first formula (or value) for the condition.
    // Status: implemented
    virtual rtl::OUString SAL_CALL Formula1(  );
    // MS Excel VBA: Returns the second formula (or value) for the condition.
    // Status: implemented
    virtual rtl::OUString SAL_CALL Formula2(  );
    // MS Excel VBA: Sets the first formula (or value) for the condition.
    // Status: implemented
    virtual void setFormula1( const css::uno::Any& _aFormula1);
    // MS Excel VBA: Sets the second formula (or value) for the condition.
    // Status: implemented
    virtual void setFormula2( const css::uno::Any& _aFormula2);
    // MS Excel VBA: Returns the Interior object (fill color, pattern) for the condition.
    // Status: implemented
    virtual css::uno::Reference< ::ooo::vba::excel::XInterior > SAL_CALL Interior(  );
    // MS Excel VBA: Returns the Borders collection for the condition.
    // Status: implemented
    virtual css::uno::Any SAL_CALL Borders( const css::uno::Any& Index );
    // MS Excel VBA: Returns the Font object for the condition's text.
    // Status: implemented
    virtual css::uno::Reference< ::ooo::vba::excel::XFont > SAL_CALL Font(  );
    // XHelperInterface
    // MS Excel VBA: Returns the implementation name for UNO service identification.
    // Status: implemented - returns "ScVbaFormatCondition"
    virtual rtl::OUString& getServiceImplName();
    // MS Excel VBA: Returns the service names this implementation supports.
    // Status: implemented - returns "ooo.vba.excel.FormatCondition"
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif
