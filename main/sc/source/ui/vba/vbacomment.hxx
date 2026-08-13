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


#ifndef SC_VBA_COMMENT_HXX
#define SC_VBA_COMMENT_HXX

#include <cppuhelper/implbase1.hxx>

#include <ooo/vba/excel/XComment.hpp>
#include <ooo/vba/excel/XApplication.hpp>
#include <ooo/vba/msforms/XShape.hpp>
#include <com/sun/star/sheet/XSheetAnnotations.hpp>
#include <com/sun/star/sheet/XSheetAnnotation.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include <vbahelper/vbahelperinterface.hxx>

typedef InheritedHelperInterfaceImpl1< ov::excel::XComment > ScVbaComment_BASE;

class ScVbaComment : public ScVbaComment_BASE
{
	css::uno::Reference< css::frame::XModel > mxModel;
	css::uno::Reference< css::table::XCellRange > mxRange;

private:
	css::uno::Reference< css::sheet::XSheetAnnotation > SAL_CALL getAnnotation();
	css::uno::Reference< css::sheet::XSheetAnnotations > SAL_CALL getAnnotations();
	sal_Int32 SAL_CALL getAnnotationIndex();
	css::uno::Reference< ov::excel::XComment > SAL_CALL getCommentByIndex( sal_Int32 Index );
public:
    ScVbaComment(
        const css::uno::Reference< ov::XHelperInterface >& xParent,
        const css::uno::Reference< css::uno::XComponentContext >& xContext,
        const css::uno::Reference< css::frame::XModel >& xModel,
        const css::uno::Reference< css::table::XCellRange >& xRange );

    virtual ~ScVbaComment() {}

    // Attributes
    virtual rtl::OUString SAL_CALL getAuthor();
    virtual void SAL_CALL setAuthor( const rtl::OUString& _author );
    virtual css::uno::Reference< ov::msforms::XShape > SAL_CALL getShape();
    virtual sal_Bool SAL_CALL getVisible();
    virtual void SAL_CALL setVisible( sal_Bool _visible );

    // Methods
    virtual void SAL_CALL Delete();
    virtual css::uno::Reference< ov::excel::XComment > SAL_CALL Next();
    virtual css::uno::Reference< ov::excel::XComment > SAL_CALL Previous();
    virtual rtl::OUString SAL_CALL Text( const css::uno::Any& Text, const css::uno::Any& Start, const css::uno::Any& Overwrite );
    // XHelperInterface
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif /* SC_VBA_COMMENT_HXX */
