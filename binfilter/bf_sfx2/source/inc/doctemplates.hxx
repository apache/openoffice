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



#ifndef _SFX_DOCTEMPLATES_HXX_
#define _SFX_DOCTEMPLATES_HXX_

#ifndef  _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif
#ifndef  _CPPUHELPER_IMPLBASE3_HXX_
#include <cppuhelper/implbase3.hxx>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif
 
#ifndef  _COM_SUN_STAR_FRAME_XDOCUMENTTEMPLATES_HPP_
#include <com/sun/star/frame/XDocumentTemplates.hpp>
#endif
#ifndef  _COM_SUN_STAR_FRAME_XSTORABLE_HPP_
#include <com/sun/star/frame/XStorable.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif

#ifndef  _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif
#ifndef  _COM_SUN_STAR_LANG_XLOCALIZABLE_HPP_
#include <com/sun/star/lang/XLocalizable.hpp>
#endif
#ifndef  _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef  _COM_SUN_STAR_UCB_XCONTENT_HPP_
#include <com/sun/star/ucb/XContent.hpp>
#endif

#ifndef  _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
 
#ifndef  _COM_SUN_STAR_UNO_RUNTIMEEXCEPTION_HPP_
#include <com/sun/star/uno/RuntimeException.hpp>
#endif
#ifndef  _COM_SUN_STAR_UNO_XINTERFACE_HPP_
#include <com/sun/star/uno/XInterface.hpp>
#endif

#ifndef _UCBHELPER_CONTENT_HXX
#include <ucbhelper/content.hxx>
#endif

#ifndef _SFX_SFXUNO_HXX
#include <sfxuno.hxx>
#endif
namespace binfilter {

//--------------------------------------------------------------------------------------------------------

#define LOCALE						::com::sun::star::lang::Locale
#define REFERENCE					::com::sun::star::uno::Reference
#define RUNTIMEEXCEPTION			::com::sun::star::uno::RuntimeException
#define PROPERTYVALUE				::com::sun::star::beans::PropertyValue
#define XCONTENT					::com::sun::star::ucb::XContent
#define XDOCUMENTTEMPLATES			::com::sun::star::frame::XDocumentTemplates
#define XINTERFACE					::com::sun::star::uno::XInterface
#define XLOCALIZABLE				::com::sun::star::lang::XLocalizable
#define XMODEL						::com::sun::star::frame::XModel
#define XMULTISERVICEFACTORY		::com::sun::star::lang::XMultiServiceFactory
#define XNAMEACCESS					::com::sun::star::container::XNameAccess
#define XSERVICEINFO				::com::sun::star::lang::XServiceInfo
#define XSTORABLE					::com::sun::star::frame::XStorable

//--------------------------------------------------------------------------------------------------------

class SfxDocTplService_Impl;

class SfxDocTplService: public ::cppu::WeakImplHelper3< XLOCALIZABLE, XDOCUMENTTEMPLATES, XSERVICEINFO >
{
	SfxDocTplService_Impl		*pImp;

public:
									SFX_DECL_XSERVICEINFO

									SfxDocTplService( const REFERENCE < ::com::sun::star::lang::XMultiServiceFactory >& xFactory );
								   ~SfxDocTplService();

	// --- XLocalizable ---
    void SAL_CALL					setLocale( const LOCALE & eLocale ) throw( RUNTIMEEXCEPTION );
    LOCALE SAL_CALL					getLocale() throw( RUNTIMEEXCEPTION );

	// --- XDocumentTemplates ---
    REFERENCE< XCONTENT > SAL_CALL	getContent() throw( RUNTIMEEXCEPTION );
    sal_Bool SAL_CALL				storeTemplate( const ::rtl::OUString& GroupName,
												   const ::rtl::OUString& TemplateName,
												   const REFERENCE< XSTORABLE >& Storable ) throw( RUNTIMEEXCEPTION );
    sal_Bool SAL_CALL				addTemplate( const ::rtl::OUString& GroupName,
												 const ::rtl::OUString& TemplateName,
												 const ::rtl::OUString& SourceURL ) throw( RUNTIMEEXCEPTION );
    sal_Bool SAL_CALL				removeTemplate( const ::rtl::OUString& GroupName,
													const ::rtl::OUString& TemplateName ) throw( RUNTIMEEXCEPTION );
    sal_Bool SAL_CALL				renameTemplate( const ::rtl::OUString& GroupName,
													const ::rtl::OUString& OldTemplateName,
													const ::rtl::OUString& NewTemplateName ) throw( RUNTIMEEXCEPTION );
    sal_Bool SAL_CALL				addGroup( const ::rtl::OUString& GroupName ) throw( RUNTIMEEXCEPTION );
    sal_Bool SAL_CALL				removeGroup( const ::rtl::OUString& GroupName ) throw( RUNTIMEEXCEPTION );
    sal_Bool SAL_CALL				renameGroup( const ::rtl::OUString& OldGroupName,
												 const ::rtl::OUString& NewGroupName ) throw( RUNTIMEEXCEPTION );
    void SAL_CALL					update() throw( RUNTIMEEXCEPTION );
};

}//end of namespace binfilter
#endif
