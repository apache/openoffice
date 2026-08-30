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



#ifndef _GOODIES_RENDERER_HXX
#define _GOODIES_RENDERER_HXX

#include <tools/gen.hxx>
#include <comphelper/propertysethelper.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/awt/XDevice.hpp>
#include <com/sun/star/graphic/XGraphic.hpp>
#include <com/sun/star/graphic/XGraphicRenderer.hpp>


using namespace com::sun::star;

class OutputDevice;

namespace unographic {

// -------------------
// - GraphicRenderer -
// -------------------

class GraphicRendererVCL : public ::cppu::OWeakAggObject,
						   public ::com::sun::star::lang::XServiceInfo,
						   public ::com::sun::star::lang::XTypeProvider,
						   public ::comphelper::PropertySetHelper,
						   public ::com::sun::star::graphic::XGraphicRenderer
{
public:

	GraphicRendererVCL();
	~GraphicRendererVCL() throw();

    static ::rtl::OUString getImplementationName_Static() throw();
    static ::com::sun::star::uno::Sequence< ::rtl::OUString >  getSupportedServiceNames_Static() throw();

protected:

	static ::comphelper::PropertySetInfo* createPropertySetInfo();

	// XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType );
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL acquire() throw();
	virtual void SAL_CALL release() throw();

	// XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService( const rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames();

	// XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

	// PropertySetHelper
	virtual void _setPropertyValues( const comphelper::PropertyMapEntry** ppEntries, const ::com::sun::star::uno::Any* pValues );
	virtual void _getPropertyValues( const comphelper::PropertyMapEntry** ppEntries, ::com::sun::star::uno::Any* pValue );

	// XGraphicRenderer
    virtual void SAL_CALL render( const ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic >& Graphic );

private:

	::com::sun::star::uno::Reference< ::com::sun::star::awt::XDevice > mxDevice;

	OutputDevice*				mpOutDev;
	Rectangle					maDestRect;
	::com::sun::star::uno::Any	maRenderData;
};

}

#endif
