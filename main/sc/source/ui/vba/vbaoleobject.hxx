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


#ifndef SC_VBA_OLEOBJECT_HXX
#define SC_VBA_OLEOBJECT_HXX

#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/drawing/XControlShape.hpp>
#include <ooo/vba/excel/XOLEObject.hpp>
#include <ooo/vba/msforms/XControl.hpp>

#include <vbahelper/vbahelperinterface.hxx>

typedef InheritedHelperInterfaceImpl1< ov::excel::XOLEObject > OLEObjectImpl_BASE;

class ScVbaOLEObject : public OLEObjectImpl_BASE
{
protected:
    css::uno::Reference< css::drawing::XControlShape > m_xControlShape;
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
    css::uno::Reference< ov::msforms::XControl> m_xControl;
public:
    ScVbaOLEObject( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext,
            css::uno::Reference< css::drawing::XControlShape > xControlShape );

    // XOLEObject Attributes
    virtual css::uno::Reference< css::uno::XInterface > SAL_CALL getObject();
    virtual sal_Bool SAL_CALL getEnabled();
    virtual void SAL_CALL setEnabled( ::sal_Bool _enabled );
    virtual sal_Bool SAL_CALL getVisible();
    virtual void SAL_CALL setVisible( ::sal_Bool _visible );
    virtual double SAL_CALL getLeft();
    virtual void SAL_CALL setLeft( double _left );
    virtual double SAL_CALL getTop();
    virtual void SAL_CALL setTop( double _top );
    virtual double SAL_CALL getHeight();
    virtual void SAL_CALL setHeight( double _height );
    virtual double SAL_CALL getWidth();
    virtual void SAL_CALL setWidth( double _width );

};
#endif //SC_VBA_OLEOBJECT_HXX
