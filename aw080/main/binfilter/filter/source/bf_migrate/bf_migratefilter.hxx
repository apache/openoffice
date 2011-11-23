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



#ifndef _BF_MIGRATEFILTER_HXX
#define _BF_MIGRATEFILTER_HXX

#ifndef _COM_SUN_STAR_DOCUMENT_XFILTER_HPP_
#include <com/sun/star/document/XFilter.hpp>
#endif

#ifndef _COM_SUN_STAR_DOCUMENT_XIMPORTER_HPP_
#include <com/sun/star/document/XImporter.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XINITIALIZATION_HPP_
#include <com/sun/star/lang/XInitialization.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#include <cppuhelper/implbase4.hxx>
namespace binfilter {


/* This component will be instantiated for both import or export. Whether it calls
 * setSourceDocument or setTargetDocument determines which Impl function the filter
 * member calls */

class bf_MigrateFilter : public cppu::WeakImplHelper4
< 
	com::sun::star::document::XFilter,
	com::sun::star::document::XImporter,
	com::sun::star::lang::XInitialization,
	com::sun::star::lang::XServiceInfo
>
{
protected:
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > mxMSF;
	::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent > mxDoc;
	::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > mxLegServFact;
	::rtl::OUString msFilterName;

	sal_Bool importImpl(const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aDescriptor) 
		throw (::com::sun::star::uno::RuntimeException);

	sal_Bool getContactToLegacyProcessServiceFactory()
		throw (::com::sun::star::uno::RuntimeException);

public:
	bf_MigrateFilter(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > &rxMSF)
	:	mxMSF(rxMSF) 
	{
	}

	virtual ~bf_MigrateFilter() 
	{
	}

	// XFilter
    virtual sal_Bool SAL_CALL filter(const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aDescriptor) 
		throw (::com::sun::star::uno::RuntimeException);

	virtual void SAL_CALL cancel() 
		throw (::com::sun::star::uno::RuntimeException);

	// XImporter
    virtual void SAL_CALL setTargetDocument(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& xDoc) 
		throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);

	// XInitialization
    virtual void SAL_CALL initialize(const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments) 
		throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

	// XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName() 
		throw (::com::sun::star::uno::RuntimeException);

	virtual sal_Bool SAL_CALL supportsService(const ::rtl::OUString& ServiceName) 
		throw (::com::sun::star::uno::RuntimeException);

	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames() 
		throw (::com::sun::star::uno::RuntimeException);
};

::rtl::OUString bf_MigrateFilter_getImplementationName()
	throw ( ::com::sun::star::uno::RuntimeException );

sal_Bool SAL_CALL bf_MigrateFilter_supportsService(const ::rtl::OUString& ServiceName) 
	throw ( ::com::sun::star::uno::RuntimeException );

::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL bf_MigrateFilter_getSupportedServiceNames() 
	throw ( ::com::sun::star::uno::RuntimeException );

::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
	SAL_CALL bf_MigrateFilter_createInstance(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rSMgr)
	throw ( ::com::sun::star::uno::Exception );

}//end of namespace binfilter
#endif // _BF_MIGRATEFILTER_HXX
// eof
