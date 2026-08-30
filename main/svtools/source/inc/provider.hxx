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



#ifndef _GOODIES_PROVIDER_HXX
#define _GOODIES_PROVIDER_HXX

#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/graphic/XGraphicProvider.hpp>
#include <com/sun/star/awt/XBitmap.hpp>

using namespace com::sun::star;

namespace unographic {

// -------------------
// - GraphicProvider -
// -------------------

class GraphicProvider : public ::cppu::WeakImplHelper1< ::com::sun::star::graphic::XGraphicProvider >
{
public:

	GraphicProvider();
	~GraphicProvider();

    static ::rtl::OUString getImplementationName_Static() throw();
    static ::com::sun::star::uno::Sequence< ::rtl::OUString >  getSupportedServiceNames_Static() throw();

protected:

	// XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService( const rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames();

	// XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

	// XGraphicProvider
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL queryGraphicDescriptor( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& MediaProperties );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > SAL_CALL queryGraphic( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& MediaProperties );
    virtual void SAL_CALL storeGraphic( const ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic >& Graphic, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& MediaProperties );

private:

	::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > implLoadMemory( const ::rtl::OUString& rResourceURL ) const;
	::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > implLoadGraphicObject( const ::rtl::OUString& rResourceURL ) const;
	::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > implLoadResource( const ::rtl::OUString& rResourceURL ) const;
	::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > implLoadRepositoryImage( const ::rtl::OUString& rResourceURL ) const;
	::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > implLoadBitmap( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XBitmap >& rBitmap ) const;
	::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > implLoadStandardImage( const ::rtl::OUString& rResourceURL ) const;
};

}

#endif
