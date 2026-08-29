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



#ifndef SC_VBA_HYPERLINK_HXX
#define SC_VBA_HYPERLINK_HXX

#include <ooo/vba/excel/XHyperlink.hpp>
#include <com/sun/star/table/XCell.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <ooo/vba/excel/XRange.hpp>

#include <vbahelper/vbahelperinterface.hxx>

typedef InheritedHelperInterfaceImpl1< ov::excel::XHyperlink > HyperlinkImpl_BASE;

class ScVbaHyperlink : public HyperlinkImpl_BASE
{
public:
    ScVbaHyperlink(
        const css::uno::Sequence< css::uno::Any >& rArgs,
        const css::uno::Reference< css::uno::XComponentContext >& rxContext );

    ScVbaHyperlink(
        const css::uno::Reference< ov::XHelperInterface >& rxAnchor,
        const css::uno::Reference< css::uno::XComponentContext >& rxContext,
        const css::uno::Any& rAddress, const css::uno::Any& rSubAddress,
        const css::uno::Any& rScreenTip, const css::uno::Any& rTextToDisplay );

    virtual ~ScVbaHyperlink();

    // Attributes
    virtual ::rtl::OUString SAL_CALL getName();
    virtual void SAL_CALL setName( const ::rtl::OUString& rName );
    virtual ::rtl::OUString SAL_CALL getAddress();
    virtual void SAL_CALL setAddress( const ::rtl::OUString& rAddress );
    virtual ::rtl::OUString SAL_CALL getSubAddress();
    virtual void SAL_CALL setSubAddress( const ::rtl::OUString& rSubAddress );
    virtual ::rtl::OUString SAL_CALL getScreenTip();
    virtual void SAL_CALL setScreenTip( const ::rtl::OUString& rScreenTip );
    virtual ::rtl::OUString SAL_CALL getTextToDisplay();
    virtual void SAL_CALL setTextToDisplay( const ::rtl::OUString& rTextToDisplay );
    virtual sal_Int32 SAL_CALL getType();
    virtual css::uno::Reference< ov::excel::XRange > SAL_CALL getRange();
    virtual css::uno::Reference< ov::msforms::XShape > SAL_CALL getShape();

    // XHelperInterface
    VBAHELPER_DECL_XHELPERINTERFACE

private:
    typedef ::std::pair< ::rtl::OUString, ::rtl::OUString > UrlComponents;

    void ensureTextField();
    UrlComponents getUrlComponents();
    void setUrlComponents( const UrlComponents& rUrlComp );

private:
    css::uno::Reference< css::table::XCell > mxCell;
    css::uno::Reference< css::beans::XPropertySet > mxTextField;
    ::rtl::OUString maScreenTip;
    long mnType;
};

#endif /* SC_VBA_HYPERLINK_HXX */
