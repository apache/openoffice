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



#ifndef _XMLEOHLP_HXX
#define _XMLEOHLP_HXX

#ifndef _CPPUHELPER_COMPBASE2_HXX_
#include <cppuhelper/compbase2.hxx>
#endif
#ifndef _SVSTOR_HXX
#include <bf_so3/svstor.hxx>
#endif
#include <map>

#ifndef _COM_SUN_STAR_DOCUMENT_XEMBEDDEDOBJECTRESOLVER_HPP_
#include <com/sun/star/document/XEmbeddedObjectResolver.hpp>
#endif
class SvGlobalName;
namespace binfilter {
class SvPersist;

// -----------------------------
// - SvXMLEmbeddedObjectHelper -
// -----------------------------

enum SvXMLEmbeddedObjectHelperMode
{
	EMBEDDEDOBJECTHELPER_MODE_READ = 0,
	EMBEDDEDOBJECTHELPER_MODE_WRITE = 1
};

// -----------------------------
// - SvXMLEmbeddedObjectHelper -
// -----------------------------

struct OUStringLess;
class OutputStorageWrapper_Impl; 

class SvXMLEmbeddedObjectHelper : public ::cppu::WeakComponentImplHelper2<
	::com::sun::star::document::XEmbeddedObjectResolver,
	::com::sun::star::container::XNameAccess >
{
	typedef ::std::map< ::rtl::OUString, OutputStorageWrapper_Impl*,
						 OUStringLess > SvXMLEmbeddedObjectHelper_Impl;
private:

	::osl::Mutex				maMutex;

	const ::rtl::OUString		maDefaultContainerStorageName;
	::rtl::OUString				maCurContainerStorageName;

	SvStorage*					mpRootStorage;	// package
	SvPersist*					mpDocPersist;
	SvStorageRef				mxContainerStorage;	// container sub package for
												// objects

	SvXMLEmbeddedObjectHelperMode		meCreateMode;
	SvXMLEmbeddedObjectHelper_Impl		*mpStreamMap;
	void*						mpDummy2;

	sal_Bool					ImplGetStorageNames(
									const ::rtl::OUString& rURLStr, 
								    ::rtl::OUString& rContainerStorageName, 
									::rtl::OUString& rObjectStorageName,
									sal_Bool bInternalToExternal ) const;

	SvStorageRef				ImplGetContainerStorage(
									const ::rtl::OUString& rStorageName );
	SvStorageRef				ImplGetObjectStorage(
									const ::rtl::OUString& rContainerStorageName, 
								    const ::rtl::OUString& rObjectStorageName,
									sal_Bool bUCBStorage );
	String 						ImplGetUniqueName( SvStorage* pStg,
											  	   const sal_Char* p ) const;
	sal_Bool					ImplReadObject(
									const ::rtl::OUString& rContainerStorageName, 
									::rtl::OUString& rObjName,
									const SvGlobalName *pClassId,
								    SvStorage *pTempStor );

	::rtl::OUString				ImplInsertEmbeddedObjectURL(
									const ::rtl::OUString& rURLStr );

protected:						

								SvXMLEmbeddedObjectHelper();
								~SvXMLEmbeddedObjectHelper();
	void						Init( SvStorage *pRootStorage, 
									  SvPersist& rDocPersist,
									  SvXMLEmbeddedObjectHelperMode eCreateMode );

    virtual void SAL_CALL		disposing();

public:							
								SvXMLEmbeddedObjectHelper(
									SvPersist& rDocPersist, 
									SvXMLEmbeddedObjectHelperMode eCreateMode );

	static SvXMLEmbeddedObjectHelper*	Create(
									SvStorage& rRootStorage, 
									SvPersist& rDocPersist,
									SvXMLEmbeddedObjectHelperMode eCreateMode,
									sal_Bool bDirect = sal_True );
	static SvXMLEmbeddedObjectHelper*	Create(
									SvPersist& rDocPersist,
									SvXMLEmbeddedObjectHelperMode eCreateMode );
	static void					Destroy( SvXMLEmbeddedObjectHelper* pSvXMLEmbeddedObjectHelper );

	void						Flush();

	// XEmbeddedObjectResolver
    virtual ::rtl::OUString SAL_CALL resolveEmbeddedObjectURL( const ::rtl::OUString& aURL ) throw(::com::sun::star::uno::RuntimeException);

	// XNameAccess
    virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName ) throw (::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName ) throw (::com::sun::star::uno::RuntimeException);

	// XNameAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasElements(  ) throw (::com::sun::star::uno::RuntimeException);
};

}//end of namespace binfilter
#endif
