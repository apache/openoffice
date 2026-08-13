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



#ifndef _SVX_UNOGALTHEMEPROVIDER_HXX
#define _SVX_UNOGALTHEMEPROVIDER_HXX

#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/gallery/XGalleryThemeProvider.hpp>

#include <svx/svxdllapi.h>

class Gallery;

namespace unogallery {

// -------------------------
// - some helper functions -
// -------------------------

::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL GalleryThemeProvider_createInstance(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rSMgr);
	::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL GalleryThemeProvider_getSupportedServiceNames() throw();
::rtl::OUString SAL_CALL GalleryThemeProvider_getImplementationName() throw();

// -----------------
// - GalleryThemes -
// -----------------

class GalleryThemeProvider : public ::cppu::WeakImplHelper2< ::com::sun::star::lang::XInitialization,
															 ::com::sun::star::gallery::XGalleryThemeProvider >
{
public:

	GalleryThemeProvider();
	~GalleryThemeProvider();

    SVX_DLLPUBLIC static ::rtl::OUString getImplementationName_Static() throw();
    SVX_DLLPUBLIC static ::com::sun::star::uno::Sequence< ::rtl::OUString >  getSupportedServiceNames_Static() throw();

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

	// XNameAccess
    virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
    virtual ::sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

	// XInitialization
    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

	// XGalleryThemeProvider
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::gallery::XGalleryTheme > SAL_CALL insertNewByName( const ::rtl::OUString& ThemeName );
    virtual void SAL_CALL removeByName( const ::rtl::OUString& ThemeName );

private:

	Gallery*	mpGallery;
	sal_Bool	mbHiddenThemes;
};

}

#endif
