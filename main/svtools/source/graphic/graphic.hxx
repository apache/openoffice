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



#ifndef _GOODIES_GRAPHIC_HXX
#define _GOODIES_GRAPHIC_HXX

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/graphic/XGraphic.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/awt/XBitmap.hpp>

#include "descriptor.hxx"
#include "transformer.hxx"

using namespace com::sun::star;

class Graphic;

namespace unographic {

// -------------------
// - GraphicProvider -
// -------------------

class Graphic : public ::com::sun::star::graphic::XGraphic,
				public ::com::sun::star::awt::XBitmap,
				public ::com::sun::star::lang::XUnoTunnel,
				public ::unographic::GraphicDescriptor,
				public ::unographic::GraphicTransformer
{
public:

	Graphic();
	~Graphic() throw();

    using unographic::GraphicDescriptor::init;
	void init( const ::Graphic& rGraphic ) throw();

	static const ::Graphic* getImplementation( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& rxIFace ) throw();
	static ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId_Static(  );
	static ::rtl::OUString getImplementationName_Static() throw();
	static ::com::sun::star::uno::Sequence< ::rtl::OUString >  getSupportedServiceNames_Static() throw();

protected:

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

	// XGraphic
    virtual ::sal_Int8 SAL_CALL getType(  );

    // XBitmap
    virtual ::com::sun::star::awt::Size SAL_CALL getSize(  );
    virtual ::com::sun::star::uno::Sequence< ::sal_Int8 > SAL_CALL getDIB(  );
    virtual ::com::sun::star::uno::Sequence< ::sal_Int8 > SAL_CALL getMaskDIB(  );

	// XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& rId );

private:

	::Graphic* mpGraphic;
};

}

#endif
