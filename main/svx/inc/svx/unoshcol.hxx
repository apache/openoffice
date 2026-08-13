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


#ifndef _SVX_UNOSHGRP_HXX
#define _SVX_UNOSHGRP_HXX

#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/drawing/XShapes.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <osl/mutex.hxx>
#include <cppuhelper/interfacecontainer.hxx>

#include <cppuhelper/implbase3.hxx>
#include "svx/svxdllapi.h"

class XShapeList;

class SvxShapeCollectionMutex
{
public:
	::osl::Mutex maMutex;
};

SVX_DLLPUBLIC com::sun::star::uno::Reference< com::sun::star::uno::XInterface > SvxShapeCollection_NewInstance() throw();

/***********************************************************************
*                                                                      *
***********************************************************************/
class SVX_DLLPUBLIC SvxShapeCollection :	public ::cppu::WeakAggImplHelper3<
											::com::sun::star::drawing::XShapes,
											::com::sun::star::lang::XServiceInfo,
											::com::sun::star::lang::XComponent
											>,
							public SvxShapeCollectionMutex
{
private:
	cppu::OInterfaceContainerHelper maShapeContainer;

	cppu::OBroadcastHelper mrBHelper;

	SVX_DLLPRIVATE virtual void disposing() throw();

public:
	SvxShapeCollection() throw();
	virtual ~SvxShapeCollection() throw();

	// XInterface
	virtual void SAL_CALL release() throw();

	// XComponent
	virtual void SAL_CALL dispose();
	virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener );
	virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener );

	// XIndexAccess
    virtual sal_Int32 SAL_CALL getCount() ;
    virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

	// XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
    virtual sal_Bool SAL_CALL hasElements();

	// XShapes
    virtual void SAL_CALL add( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape );
    virtual void SAL_CALL remove( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape );

	// XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
	static com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static();
	static ::rtl::OUString getImplementationName_Static();
};

::com::sun::star::uno::Reference< com::sun::star::uno::XInterface > SAL_CALL SvxShapeCollection_createInstance( const com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& rSMgr );
#endif
