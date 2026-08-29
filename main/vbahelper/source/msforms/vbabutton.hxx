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


#ifndef SC_VBA_BUTTON_HXX
#define SC_VBA_BUTTON_HXX
#include <cppuhelper/implbase1.hxx>
#include <ooo/vba/msforms/XCommandButton.hpp>

#include "vbacontrol.hxx"
#include <vbahelper/vbahelper.hxx>

typedef cppu::ImplInheritanceHelper1< ScVbaControl, ov::msforms::XCommandButton > ButtonImpl_BASE;

class ScVbaButton : public ButtonImpl_BASE
{
public:
    ScVbaButton( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::uno::XInterface >& xControl, const css::uno::Reference< css::frame::XModel >& xModel, ov::AbstractGeometryAttributes* pGeomHelper  );
   // Attributes
    virtual rtl::OUString SAL_CALL getCaption();
    virtual void SAL_CALL setCaption( const rtl::OUString& _caption );
    virtual sal_Bool SAL_CALL getAutoSize();
    virtual void SAL_CALL setAutoSize( sal_Bool bAutoSize );
    virtual sal_Bool SAL_CALL getCancel();
    virtual void SAL_CALL setCancel( sal_Bool bCancel );
    virtual sal_Bool SAL_CALL getDefault();
    virtual void SAL_CALL setDefault( sal_Bool bDefault );
    virtual sal_Int32 SAL_CALL getBackColor();
    virtual void SAL_CALL setBackColor( sal_Int32 nBackColor );
    virtual sal_Int32 SAL_CALL getForeColor();
    virtual void SAL_CALL setForeColor( sal_Int32 nForeColor );
    virtual css::uno::Reference< ov::msforms::XNewFont > SAL_CALL getFont();
    //XHelperInterface
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};
#endif //SC_VBA_BUTTON_HXX
