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



#ifndef SC_VBA_SHEETOBJECT_HXX
#define SC_VBA_SHEETOBJECT_HXX

#include <memory>
#include <ooo/vba/excel/XButton.hpp>
#include <ooo/vba/excel/XControlObject.hpp>
#include <ooo/vba/excel/XSheetObject.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include "vbapalette.hxx"

namespace com { namespace sun { namespace star {
    namespace container { class XIndexContainer; }
    namespace drawing { class XControlShape; }
} } }

// ============================================================================

typedef InheritedHelperInterfaceImpl1< ov::excel::XCharacters > ScVbaButtonCharacters_BASE;

/** Simple implementation of the Characters symbol for drawing button objects. */
class ScVbaButtonCharacters : public ScVbaButtonCharacters_BASE
{
public:
	explicit ScVbaButtonCharacters(
        const css::uno::Reference< ov::XHelperInterface >& rxParent,
        const css::uno::Reference< css::uno::XComponentContext >& rxContext,
        const css::uno::Reference< css::beans::XPropertySet >& rxPropSet,
        const ScVbaPalette& rPalette,
        const css::uno::Any& rStart,
        const css::uno::Any& rLength );
	virtual ~ScVbaButtonCharacters();

	// XCharacters attributes
	virtual ::rtl::OUString SAL_CALL getCaption();
	virtual void SAL_CALL setCaption( const ::rtl::OUString& rCaption );
	virtual ::rtl::OUString SAL_CALL getText();
	virtual void SAL_CALL setText( const ::rtl::OUString& rText );
	virtual sal_Int32 SAL_CALL getCount();
	virtual css::uno::Reference< ov::excel::XFont > SAL_CALL getFont();
	virtual void SAL_CALL setFont( const css::uno::Reference< ov::excel::XFont >& rxFont );

	// XCharacters methods
	virtual void SAL_CALL Insert( const ::rtl::OUString& rString );
	virtual void SAL_CALL Delete();

	// XHelperInterface
    VBAHELPER_DECL_XHELPERINTERFACE

private:
    ::rtl::OUString getFullString() const;
    void setFullString( const ::rtl::OUString& rString );

private:
    ScVbaPalette maPalette;
    css::uno::Reference< css::beans::XPropertySet > mxPropSet;
    sal_Int32 mnStart;
    sal_Int32 mnLength;
};

// ============================================================================

typedef InheritedHelperInterfaceImpl1< ov::excel::XSheetObject > ScVbaSheetObject_BASE;

/** Base class for drawing objects embedded in sheets. */
class ScVbaSheetObjectBase : public ScVbaSheetObject_BASE
{
public:
    explicit ScVbaSheetObjectBase(
        const css::uno::Reference< ov::XHelperInterface >& rxParent,
        const css::uno::Reference< css::uno::XComponentContext >& rxContext,
        const css::uno::Reference< css::frame::XModel >& rxModel,
        const css::uno::Reference< css::drawing::XShape >& rxShape );

    // XSheetObject attributes
    virtual double SAL_CALL getLeft();
    virtual void SAL_CALL setLeft( double fLeft );
    virtual double SAL_CALL getTop();
    virtual void SAL_CALL setTop( double fTop );
    virtual double SAL_CALL getWidth();
    virtual void SAL_CALL setWidth( double fWidth );
    virtual double SAL_CALL getHeight();
    virtual void SAL_CALL setHeight( double fHeight );
    virtual ::rtl::OUString SAL_CALL getName();
    virtual void SAL_CALL setName( const ::rtl::OUString& rName );
    virtual sal_Int32 SAL_CALL getPlacement();
    virtual void SAL_CALL setPlacement( sal_Int32 nPlacement );
    virtual sal_Bool SAL_CALL getPrintObject();
    virtual void SAL_CALL setPrintObject( sal_Bool bPrintObject );

    /** Sets default properties after a new object has been created. */
    void setDefaultProperties( sal_Int32 nIndex );

protected:
    /** Derived classes return the base name used for new objects. */
    virtual ::rtl::OUString implGetBaseName() const = 0;
    /** Derived classes set default properties for new drawing objects. */
    virtual void implSetDefaultProperties();

protected:
    ScVbaPalette maPalette;
    css::uno::Reference< css::frame::XModel > mxModel;
    css::uno::Reference< css::drawing::XShape > mxShape;
    css::uno::Reference< css::beans::XPropertySet > mxShapeProps;
};

// ============================================================================

typedef ::cppu::ImplInheritanceHelper1< ScVbaSheetObjectBase, ov::excel::XControlObject > ScVbaControlObject_BASE;

class ScVbaControlObjectBase : public ScVbaControlObject_BASE
{
public:
    /** Specifies the listener used for OnAction events. */
    enum ListenerType
    {
        LISTENER_ACTION,        /// XActionListener.actionPerformed
        LISTENER_MOUSE,         /// XMouseListener.mouseReleased
        LISTENER_TEXT,          /// XTextListener.textChanged
        LISTENER_VALUE,         /// XAdjustmentListener.adjustmentValueChanged
        LISTENER_CHANGE         /// XChangeListener.changed
    };

    explicit ScVbaControlObjectBase(
        const css::uno::Reference< ov::XHelperInterface >& rxParent,
        const css::uno::Reference< css::uno::XComponentContext >& rxContext,
        const css::uno::Reference< css::frame::XModel >& rxModel,
        const css::uno::Reference< css::container::XIndexContainer >& rxFormIC,
        const css::uno::Reference< css::drawing::XControlShape >& rxControlShape,
        ListenerType eListenerType );

    // XSheetObject attributes
    virtual ::rtl::OUString SAL_CALL getName();
    virtual void SAL_CALL setName( const ::rtl::OUString& rName );
    virtual ::rtl::OUString SAL_CALL getOnAction();
    virtual void SAL_CALL setOnAction( const ::rtl::OUString& rMacroName );
    virtual sal_Bool SAL_CALL getPrintObject();
    virtual void SAL_CALL setPrintObject( sal_Bool bPrintObject );

    // XControlObject attributes
    virtual sal_Bool SAL_CALL getAutoSize();
    virtual void SAL_CALL setAutoSize( sal_Bool bAutoSize );

protected:
    sal_Int32 getModelIndexInForm() const;

protected:
    css::uno::Reference< css::container::XIndexContainer > mxFormIC;
    css::uno::Reference< css::beans::XPropertySet > mxControlProps;
    ::rtl::OUString maListenerType;
    ::rtl::OUString maEventMethod;
};

// ============================================================================

typedef ::cppu::ImplInheritanceHelper1< ScVbaControlObjectBase, ov::excel::XButton > ScVbaButton_BASE;

class ScVbaButton : public ScVbaButton_BASE
{
public:
    explicit ScVbaButton(
        const css::uno::Reference< ov::XHelperInterface >& rxParent,
        const css::uno::Reference< css::uno::XComponentContext >& rxContext,
        const css::uno::Reference< css::frame::XModel >& rxModel,
        const css::uno::Reference< css::container::XIndexContainer >& rxFormIC,
        const css::uno::Reference< css::drawing::XControlShape >& rxControlShape );

    // XButton attributes
    virtual ::rtl::OUString SAL_CALL getCaption();
    virtual void SAL_CALL setCaption( const ::rtl::OUString& rCaption );
    virtual css::uno::Reference< ov::excel::XFont > SAL_CALL getFont();
    virtual void SAL_CALL setFont( const css::uno::Reference< ov::excel::XFont >& rxFont );
    virtual sal_Int32 SAL_CALL getHorizontalAlignment();
    virtual void SAL_CALL setHorizontalAlignment( sal_Int32 nAlign );
    virtual sal_Int32 SAL_CALL getVerticalAlignment();
    virtual void SAL_CALL setVerticalAlignment( sal_Int32 nAlign );
    virtual sal_Int32 SAL_CALL getOrientation();
    virtual void SAL_CALL setOrientation( sal_Int32 nOrientation );

    // XButton methods
    css::uno::Reference< ov::excel::XCharacters > SAL_CALL Characters(
        const css::uno::Any& rStart, const css::uno::Any& rLength );

    // XHelperInterface
    VBAHELPER_DECL_XHELPERINTERFACE

protected:
    virtual ::rtl::OUString implGetBaseName() const;
    virtual void implSetDefaultProperties();
};

// ============================================================================

#endif
