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


#ifndef SC_VBA_SHAPERANGE_HXX
#define SC_VBA_SHAPERANGE_HXX

#include <com/sun/star/drawing/XShapes.hpp>
#include <ooo/vba/msforms/XShapeRange.hpp>
#include <com/sun/star/drawing/XDrawPage.hpp>
#include <vbahelper/vbacollectionimpl.hxx>
#include <ooo/vba/msforms/XLineFormat.hpp>
#include <ooo/vba/msforms/XFillFormat.hpp>

typedef CollTestImplHelper< ov::msforms::XShapeRange > ScVbaShapeRange_BASE;

class VBAHELPER_DLLPUBLIC ScVbaShapeRange : public ScVbaShapeRange_BASE
{
private:
    css::uno::Reference< css::drawing::XDrawPage > m_xDrawPage;
    css::uno::Reference< css::drawing::XShapes > m_xShapes;
    sal_Int32 m_nShapeGroupCount;
protected:
    css::uno::Reference< css::frame::XModel > m_xModel;
	virtual rtl::OUString& getServiceImplName();
	virtual css::uno::Sequence<rtl::OUString> getServiceNames();
	css::uno::Reference< css::drawing::XShapes > getShapes() ;
public:
    ScVbaShapeRange( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::container::XIndexAccess >& xShapes, const css::uno::Reference< css::drawing::XDrawPage>& xDrawShape, const css::uno::Reference< css::frame::XModel >& xModel );

    // Methods
    virtual void SAL_CALL Select(  );
    virtual css::uno::Reference< ::ooo::vba::msforms::XShape > SAL_CALL Group();
    virtual void SAL_CALL IncrementRotation( double Increment );
    virtual void SAL_CALL IncrementLeft( double Increment ) ;
    virtual void SAL_CALL IncrementTop( double Increment );
    virtual double SAL_CALL getHeight();
    virtual void SAL_CALL setHeight( double _height );
    virtual double SAL_CALL getWidth();
    virtual void SAL_CALL setWidth( double _width );
    virtual double SAL_CALL getLeft();
    virtual void SAL_CALL setLeft( double _left );
    virtual double SAL_CALL getTop();
    virtual void SAL_CALL setTop( double _top );
    virtual css::uno::Reference< ov::msforms::XLineFormat > SAL_CALL getLine();
    virtual css::uno::Reference< ov::msforms::XFillFormat > SAL_CALL getFill();
    virtual ::sal_Bool SAL_CALL getLockAspectRatio();
    virtual void SAL_CALL setLockAspectRatio( ::sal_Bool _lockaspectratio );
    virtual ::sal_Bool SAL_CALL getLockAnchor();
    virtual void SAL_CALL setLockAnchor( ::sal_Bool _lockanchor );
    virtual ::sal_Int32 SAL_CALL getRelativeHorizontalPosition();
    virtual void SAL_CALL setRelativeHorizontalPosition( ::sal_Int32 _relativehorizontalposition );
    virtual ::sal_Int32 SAL_CALL getRelativeVerticalPosition();
    virtual void SAL_CALL setRelativeVerticalPosition( ::sal_Int32 _relativeverticalposition );
    virtual css::uno::Any SAL_CALL SAL_CALL TextFrame(  );
    virtual css::uno::Any SAL_CALL SAL_CALL WrapFormat(  );
    //XEnumerationAccess
    virtual css::uno::Type SAL_CALL getElementType();
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration();
    // ScVbaCollectionBaseImpl
    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource );
};

#endif//SC_VBA_SHAPERANGE_HXX
