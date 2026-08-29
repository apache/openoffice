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


#ifndef SW_VBA_XWRAPFORMAT_HXX
#define SW_VBA_XWRAPFORMAT_HXX

#include <com/sun/star/drawing/XShape.hpp>
#include <ooo/vba/word/XWrapFormat.hpp>
#include <vbahelper/vbahelperinterface.hxx>

typedef InheritedHelperInterfaceImpl1< ooo::vba::word::XWrapFormat > SwVbaWrapFormat_BASE;

class SwVbaWrapFormat : public SwVbaWrapFormat_BASE
{
private:
    css::uno::Reference< css::drawing::XShape > m_xShape;
    css::uno::Reference< css::beans::XPropertySet > m_xPropertySet;
    sal_Int32 mnWrapFormatType;
    sal_Int32 mnSide;

private:
    void makeWrap();
    float getDistance( const rtl::OUString& sName );
    void setDistance( const rtl::OUString& sName, float _distance );

public:
    SwVbaWrapFormat(  css::uno::Sequence< css::uno::Any > const& aArgs, css::uno::Reference< css::uno::XComponentContext >const& xContext );

    virtual ::sal_Int32 SAL_CALL getType();
    virtual void SAL_CALL setType( ::sal_Int32 _type );
    virtual ::sal_Int32 SAL_CALL getSide();
    virtual void SAL_CALL setSide( ::sal_Int32 _side );
    virtual float SAL_CALL getDistanceTop();
    virtual void SAL_CALL setDistanceTop( float _distancetop );
    virtual float SAL_CALL getDistanceBottom();
    virtual void SAL_CALL setDistanceBottom( float _distancebottom );
    virtual float SAL_CALL getDistanceLeft();
    virtual void SAL_CALL setDistanceLeft( float _distanceleft );
    virtual float SAL_CALL getDistanceRight();
    virtual void SAL_CALL setDistanceRight( float _distanceright );

    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();
};

#endif//SW_VBA_XWRAPFORMAT_HXX
