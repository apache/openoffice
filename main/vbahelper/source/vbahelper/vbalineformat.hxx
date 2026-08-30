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


#ifndef SC_VBA_XLINEFORMAT_HXX
#define SC_VBA_XLINEFORMAT_HXX

#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <ooo/vba/msforms/XLineFormat.hpp>
#include <vbahelper/vbahelperinterface.hxx>

typedef InheritedHelperInterfaceImpl1< ov::msforms::XLineFormat > ScVbaLineFormat_BASE;

class ScVbaLineFormat : public ScVbaLineFormat_BASE
{
private:
    css::uno::Reference< css::drawing::XShape > m_xShape;
    css::uno::Reference< css::beans::XPropertySet > m_xPropertySet;
    sal_Int32 m_nLineDashStyle;
    double m_nLineWeight;
protected:
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
    sal_Int32 convertLineStartEndNameToArrowheadStyle( rtl::OUString sLineName );
    rtl::OUString convertArrowheadStyleToLineStartEndName( sal_Int32 nArrowheadStyle );
    sal_Int32 calculateArrowheadSize();
public:
    ScVbaLineFormat( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::drawing::XShape > xShape );

    // Attributes
    virtual sal_Int32 SAL_CALL getBeginArrowheadStyle();
    virtual void SAL_CALL setBeginArrowheadStyle( sal_Int32 _beginarrowheadstyle );
    virtual sal_Int32 SAL_CALL getBeginArrowheadLength();
    virtual void SAL_CALL setBeginArrowheadLength( sal_Int32 _beginarrowheadlength );
    virtual sal_Int32 SAL_CALL getBeginArrowheadWidth();
    virtual void SAL_CALL setBeginArrowheadWidth( sal_Int32 _beginarrowheadwidth );
    virtual sal_Int32 SAL_CALL getEndArrowheadStylel();
    virtual void SAL_CALL setEndArrowheadStylel( sal_Int32 _endarrowheadstylel );
    virtual sal_Int32 SAL_CALL getEndArrowheadLength();
    virtual void SAL_CALL setEndArrowheadLength( sal_Int32 _endarrowheadlength );
    virtual sal_Int32 SAL_CALL getEndArrowheadWidth();
    virtual void SAL_CALL setEndArrowheadWidth( sal_Int32 _endarrowheadwidth );
    virtual double SAL_CALL getWeight();
    virtual void SAL_CALL setWeight( double _weight );
    virtual sal_Bool SAL_CALL getVisible();
    virtual void SAL_CALL setVisible( sal_Bool _visible );
    virtual double SAL_CALL getTransparency();
    virtual void SAL_CALL setTransparency( double _transparency );
    virtual sal_Int16 SAL_CALL getStyle();
    virtual void SAL_CALL setStyle( sal_Int16 _style );
    virtual sal_Int32 SAL_CALL getDashStyle();
    virtual void SAL_CALL setDashStyle( sal_Int32 _dashstyle );

    // Methods
    virtual css::uno::Reference< ov::msforms::XColorFormat > SAL_CALL BackColor();
    virtual css::uno::Reference< ov::msforms::XColorFormat > SAL_CALL ForeColor();
};

#endif//SC_VBA_XLINEFORMAT_HXX
