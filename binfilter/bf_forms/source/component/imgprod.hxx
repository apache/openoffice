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



#ifndef _PRODUCE_HXX
#define _PRODUCE_HXX

#include <tools/link.hxx>
#include <tools/string.hxx>
#include <tools/list.hxx>

#ifndef _COM_SUN_STAR_AWT_IMAGESTATUS_HPP_
#include <com/sun/star/awt/ImageStatus.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XIMAGECONSUMER_HPP_
#include <com/sun/star/awt/XImageConsumer.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XIMAGEPRODUCER_HPP_
#include <com/sun/star/awt/XImageProducer.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XINITIALIZATION_HPP_
#include <com/sun/star/lang/XInitialization.hpp>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif


// -----------------
// - ImageProducer -
// -----------------



class SvStream;
class ImageConsumer;
class Graphic;
namespace binfilter
{
class GraphicFilter;
}

namespace com { namespace sun { namespace star { namespace io {
	class XInputStream;
}}}}


class ImageProducer :	public ::com::sun::star::awt::XImageProducer, 
						public ::com::sun::star::lang::XInitialization, 
						public ::cppu::OWeakObject
{
private:

	::rtl::OUString	maURL;
	List			maConsList;
	Graphic*		mpGraphic;
	SvStream*		mpStm;
	binfilter::GraphicFilter*	mpFilter;
	sal_uInt32		mnTransIndex;
	sal_uInt32		mnStatus;
	sal_Bool		mbConsInit;
	sal_Bool		mbStmDel;
	Link			maErrorHdl;
	sal_uInt32		mnLastError;

	sal_uInt32		mnExtra2;

	sal_Bool		mbAsync;
	sal_Bool		mbExtra1;
	sal_Bool		mbExtra2;
	sal_Bool		mbExtra3;

	sal_Bool		ImplImportGraphic( Graphic& rGraphic );
	void			ImplUpdateData( const Graphic& rGraphic );
	void			ImplInitConsumer( const Graphic& rGraphic );
	void			ImplUpdateConsumer( const Graphic& rGraphic );

public:

					ImageProducer();
					~ImageProducer();

	void			SetImage( const ::rtl::OUString& rPath );
	void			SetImage( SvStream& rStm );

	void			SetErrorHandler( const Link& rErrorHdl ) { maErrorHdl = rErrorHdl; }
	const Link&		GetErrorHandler() const { return maErrorHdl; }

	sal_uInt32		GetLastError() const { return mnLastError; }
	void			ResetLastError() { mnLastError = 0; }

	void			NewDataAvailable();

	// ::com::sun::star::uno::XInterface
    ::com::sun::star::uno::Any					SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);
	void										SAL_CALL acquire() throw()	{ OWeakObject::acquire(); }
	void										SAL_CALL release() throw()	{ OWeakObject::release(); }
	
	// MT: ???
	void			setImage( ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > & rStmRef );

	// ::com::sun::star::awt::XImageProducer
    void SAL_CALL addConsumer( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XImageConsumer >& rxConsumer ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL removeConsumer( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XImageConsumer >& rxConsumer ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL startProduction(  ) throw(::com::sun::star::uno::RuntimeException);

    // ::com::sun::star::lang::XInitialization
	void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

};

#endif // _PRODUCE_HXX
