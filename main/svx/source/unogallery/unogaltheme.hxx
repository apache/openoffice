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



#ifndef _SVX_UNOGALTHEME_HXX
#define _SVX_UNOGALTHEME_HXX

#include <list>

#include <cppuhelper/implbase1.hxx>
#include <svl/lstner.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/gallery/XGalleryTheme.hpp>

class Gallery;
class GalleryTheme;
struct GalleryObject;
namespace unogallery { class GalleryItem; }

namespace unogallery {

// ----------------
// - GalleryTheme -
// ----------------

class GalleryTheme : public ::cppu::WeakImplHelper1< ::com::sun::star::gallery::XGalleryTheme >,
					 public SfxListener
{
	friend class ::unogallery::GalleryItem;

public:

	GalleryTheme( const ::rtl::OUString& rThemeName );
	~GalleryTheme();

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

	// XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
    virtual ::sal_Bool SAL_CALL hasElements();

    // XIndexAccess
	virtual ::sal_Int32 SAL_CALL getCount(  );
    virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( ::sal_Int32 Index );

	// XGalleryThemes
    virtual ::rtl::OUString SAL_CALL getName(  );
    virtual void SAL_CALL update(  );
    virtual ::sal_Int32 SAL_CALL insertURLByIndex( const ::rtl::OUString& URL, ::sal_Int32 Index );
    virtual ::sal_Int32 SAL_CALL insertGraphicByIndex( const ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic >& Graphic, ::sal_Int32 Index );
    virtual ::sal_Int32 SAL_CALL insertDrawingByIndex( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& Drawing, ::sal_Int32 Index );
    virtual void SAL_CALL removeByIndex( ::sal_Int32 Index );

	// SfxListener
	virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

private:

	typedef ::std::list< ::unogallery::GalleryItem* > GalleryItemList;

	GalleryItemList	maItemList;
	::Gallery*		mpGallery;
	::GalleryTheme*	mpTheme;

	::GalleryTheme*	implGetTheme() const;

	void 			implReleaseItems( GalleryObject* pObj );

	void			implRegisterGalleryItem( ::unogallery::GalleryItem& rItem );
	void			implDeregisterGalleryItem( ::unogallery::GalleryItem& rItem );
};

}

#endif
