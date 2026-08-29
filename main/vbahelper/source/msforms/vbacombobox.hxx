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


#ifndef SC_VBA_COMBOBOX_HXX
#define SC_VBA_COMBOBOX_HXX
#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/script/XDefaultProperty.hpp>
#include <ooo/vba/msforms/XComboBox.hpp>
#include <comphelper/proparrhlp.hxx>
#include <comphelper/propertycontainer.hxx>
#include <com/sun/star/beans/PropertyAttribute.hpp>

#include "vbacontrol.hxx"
#include "vbalistcontrolhelper.hxx"
#include <vbahelper/vbahelper.hxx>

typedef cppu::ImplInheritanceHelper2<ScVbaControl, ov::msforms::XComboBox, css::script::XDefaultProperty > ComboBoxImpl_BASE;
class ScVbaComboBox : public ComboBoxImpl_BASE
{
	std::auto_ptr< ListControlHelper > mpListHelper;
	rtl::OUString sSourceName;
	rtl::OUString msDftPropName;
	bool mbDialogType;

public:
	ScVbaComboBox( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::uno::XInterface >& xControl, const css::uno::Reference< css::frame::XModel >& xModel, ov::AbstractGeometryAttributes* pGeomHelper, bool bDialogType = false );

	// Attributes
	virtual css::uno::Any SAL_CALL getListIndex();
	virtual ::sal_Int32 SAL_CALL getListCount();
	virtual void SAL_CALL setListIndex( const css::uno::Any& _value );
	virtual css::uno::Any SAL_CALL getValue();
	virtual void SAL_CALL setValue( const css::uno::Any& _value );
	virtual ::rtl::OUString SAL_CALL getText();
	virtual void SAL_CALL setText( const ::rtl::OUString& _text );
    virtual sal_Int32 SAL_CALL getStyle();
	virtual void SAL_CALL setStyle( sal_Int32 nStyle );
    virtual sal_Int32 SAL_CALL getDropButtonStyle();
	virtual void SAL_CALL setDropButtonStyle( sal_Int32 nDropButtonStyle );
    virtual sal_Int32 SAL_CALL getDragBehavior();
	virtual void SAL_CALL setDragBehavior( sal_Int32 nDragBehavior );
    virtual sal_Int32 SAL_CALL getEnterFieldBehavior();
	virtual void SAL_CALL setEnterFieldBehavior( sal_Int32 nEnterFieldBehavior );
    virtual sal_Int32 SAL_CALL getListStyle();
	virtual void SAL_CALL setListStyle( sal_Int32 nListStyle );
    virtual sal_Int32 SAL_CALL getTextAlign();
	virtual void SAL_CALL setTextAlign( sal_Int32 nTextAlign );
    virtual sal_Int32 SAL_CALL getTextLength();
    virtual css::uno::Reference< ov::msforms::XNewFont > SAL_CALL getFont();

	// Methods
	virtual void SAL_CALL AddItem( const css::uno::Any& pvargItem, const css::uno::Any& pvargIndex );
	virtual void SAL_CALL removeItem( const css::uno::Any& index );
	virtual void SAL_CALL Clear(  );
	virtual css::uno::Any SAL_CALL List( const css::uno::Any& pvargIndex, const css::uno::Any& pvarColumn );
	// XControl
    virtual void SAL_CALL setRowSource( const rtl::OUString& _rowsource );

	// XDefaultProperty
        ::rtl::OUString SAL_CALL getDefaultPropertyName(  ) { return ::rtl::OUString::createFromAscii("Value"); }
	//XHelperInterface
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif //
