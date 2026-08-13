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


#ifndef SC_VBA_LISTBOX_HXX
#define SC_VBA_LISTBOX_HXX
#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/script/XDefaultProperty.hpp>
#include <ooo/vba/msforms/XListBox.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>

#include "vbacontrol.hxx"
#include <vbahelper/vbapropvalue.hxx>
#include "vbalistcontrolhelper.hxx"
#include <vbahelper/vbahelper.hxx>

typedef cppu::ImplInheritanceHelper2<ScVbaControl, ov::msforms::XListBox, css::script::XDefaultProperty > ListBoxImpl_BASE;
class ScVbaListBox : public ListBoxImpl_BASE
    ,public PropListener
{
	std::auto_ptr< ListControlHelper > mpListHelper;
	rtl::OUString sSourceName;
	rtl::OUString msDftPropName;

    sal_Int16 m_nIndex;

public:
	ScVbaListBox( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::uno::XInterface >& xControl, const css::uno::Reference< css::frame::XModel >& xModel, ov::AbstractGeometryAttributes* pGeomHelper );

	// Attributes
	virtual css::uno::Any SAL_CALL getListIndex();
	virtual void SAL_CALL setListIndex( const css::uno::Any& _value );
	virtual ::sal_Int32 SAL_CALL getListCount();
	virtual css::uno::Any SAL_CALL getValue();
	virtual void SAL_CALL setValue( const css::uno::Any& _value );
	virtual rtl::OUString SAL_CALL getText();
	virtual void SAL_CALL setText( const ::rtl::OUString& _text );
    virtual sal_Bool SAL_CALL getMultiSelect();
    virtual void SAL_CALL setMultiSelect( sal_Bool _multiselect );
    virtual css::uno::Reference< ov::msforms::XNewFont > SAL_CALL getFont();

	// Methods
    virtual css::uno::Any SAL_CALL Selected( ::sal_Int32 index );
	virtual void SAL_CALL AddItem( const css::uno::Any& pvargItem, const css::uno::Any& pvargIndex );
    virtual void SAL_CALL removeItem( const css::uno::Any& index );
	virtual void SAL_CALL Clear(  );
	virtual css::uno::Any SAL_CALL List( const css::uno::Any& pvargIndex, const css::uno::Any& pvarColumn );
	// XControl
    virtual void SAL_CALL setRowSource( const rtl::OUString& _rowsource );

	// XDefaultProperty
    rtl::OUString SAL_CALL getDefaultPropertyName(  ) { return ::rtl::OUString::createFromAscii("Value"); }

    //XHelperInterface
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();

    //PropListener
    virtual void setValueEvent( const css::uno::Any& value );
    virtual css::uno::Any getValueEvent();


};

#endif //
