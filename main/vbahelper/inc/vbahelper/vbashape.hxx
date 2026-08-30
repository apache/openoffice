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


#ifndef SC_VBA_SHAPE_HXX
#define SC_VBA_SHAPE_HXX

#include <ooo/vba/office/MsoShapeType.hpp>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/drawing/XShapes.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <ooo/vba/msforms/XShape.hpp>
#include <ooo/vba/msforms/XLineFormat.hpp>
#include <cppuhelper/implbase2.hxx>

#include <vbahelper/vbahelperinterface.hxx>

typedef  ::cppu::WeakImplHelper2< ov::msforms::XShape, css::lang::XEventListener > ListeningShape;

typedef InheritedHelperInterfaceImpl< ListeningShape > ScVbaShape_BASE;

class VBAHELPER_DLLPUBLIC ScVbaShape : public ScVbaShape_BASE
{
private:
protected:
    std::auto_ptr< ov::ShapeHelper > m_pShapeHelper;
    css::uno::Reference< css::drawing::XShape > m_xShape;
    css::uno::Reference< css::drawing::XShapes > m_xShapes;
    css::uno::Reference< css::beans::XPropertySet > m_xPropertySet;
    sal_Int32 m_nType;
    css::uno::Reference< css::frame::XModel > m_xModel;
    css::uno::Any m_aRange;
	virtual void addListeners();
	virtual void removeShapeListener();
	virtual void removeShapesListener();
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
public:
    ScVbaShape( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::drawing::XShape >& xShape, const css::uno::Reference< css::drawing::XShapes >& xShapes, const css::uno::Reference< css::frame::XModel >& xModel, sal_Int32 nType );
    ScVbaShape( const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::drawing::XShape >& xShape, const css::uno::Reference< css::frame::XModel >& xModel );
    virtual ~ScVbaShape();
    css::uno::Any getRange() { return m_aRange; };
    void setRange( css::uno::Any aRange ) { m_aRange = aRange; };

    static sal_Int32 getType( const css::uno::Reference< css::drawing::XShape > xShape );

    // Attributes
    virtual rtl::OUString SAL_CALL getName();
    virtual void SAL_CALL setName( const rtl::OUString& _name );
    virtual double SAL_CALL getHeight();
    virtual void SAL_CALL setHeight( double _height );
    virtual double SAL_CALL getWidth();
    virtual void SAL_CALL setWidth( double _width );
    virtual double SAL_CALL getLeft();
    virtual void SAL_CALL setLeft( double _left );
    virtual double SAL_CALL getTop();
    virtual void SAL_CALL setTop( double _top );
    virtual sal_Bool SAL_CALL getVisible();
    virtual void SAL_CALL setVisible( sal_Bool _visible );
    virtual sal_Int32 SAL_CALL getZOrderPosition();
    virtual sal_Int32 SAL_CALL getType();
    virtual double SAL_CALL getRotation();
    virtual void SAL_CALL setRotation( double _rotation );
    virtual css::uno::Reference< ov::msforms::XLineFormat > SAL_CALL getLine();
    virtual css::uno::Reference< ov::msforms::XFillFormat > SAL_CALL getFill();
    virtual css::uno::Reference< ov::msforms::XPictureFormat > SAL_CALL getPictureFormat();
    virtual ::sal_Bool SAL_CALL getLockAspectRatio();
    virtual void SAL_CALL setLockAspectRatio( ::sal_Bool _lockaspectratio );
    virtual ::sal_Bool SAL_CALL getLockAnchor();
    virtual void SAL_CALL setLockAnchor( ::sal_Bool _lockanchor );
    virtual ::sal_Int32 SAL_CALL getRelativeHorizontalPosition();
    virtual void SAL_CALL setRelativeHorizontalPosition( ::sal_Int32 _relativehorizontalposition );
    virtual ::sal_Int32 SAL_CALL getRelativeVerticalPosition();
    virtual void SAL_CALL setRelativeVerticalPosition( ::sal_Int32 _relativeverticalposition );

    // Methods
    virtual css::uno::Any SAL_CALL SAL_CALL TextFrame(  );
    virtual css::uno::Any SAL_CALL SAL_CALL WrapFormat(  );
    virtual void SAL_CALL Delete();
    virtual void SAL_CALL ZOrder( sal_Int32 ZOrderCmd );
    virtual void SAL_CALL IncrementRotation( double Increment );
    virtual void SAL_CALL IncrementLeft( double Increment );
    virtual void SAL_CALL IncrementTop( double Increment );
    virtual void SAL_CALL ScaleHeight( double Factor, sal_Bool RelativeToOriginalSize, sal_Int32 Scale );
    virtual void SAL_CALL ScaleWidth( double Factor, sal_Bool RelativeToOriginalSize, sal_Int32 Scale );
    // Replace??
    virtual void SAL_CALL Select( const css::uno::Any& Replace );
    virtual css::uno::Any SAL_CALL ShapeRange( const css::uno::Any& index );
    // XEventListener
    virtual void SAL_CALL disposing( const css::lang::EventObject& rEventObject );
};
#endif//SC_VBA_SHAPE_HXX
