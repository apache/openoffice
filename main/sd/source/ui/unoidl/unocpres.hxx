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


#ifndef _SD_UNOCPRES_HXX
#define _SD_UNOCPRES_HXX

#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/container/XIndexContainer.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <cppuhelper/interfacecontainer.h>
#include <osl/mutex.hxx>

#include <cppuhelper/implbase5.hxx>
#include <comphelper/servicehelper.hxx>

///////////////////////////////////////////////////////////////////////////////

class SdXImpressDocument;
class SdView;
class SdDrawViewShell;
class SdCustomShow;

class SdXCustomPresentation :	public ::cppu::WeakImplHelper5< ::com::sun::star::container::XIndexContainer,
																::com::sun::star::container::XNamed,
																::com::sun::star::lang::XUnoTunnel,
																::com::sun::star::lang::XComponent,
																::com::sun::star::lang::XServiceInfo >
{
private:
	SdCustomShow* 		mpSdCustomShow;
	SdXImpressDocument*	mpModel;

	// for xComponent
	::osl::Mutex aDisposeContainerMutex;
	::cppu::OInterfaceContainerHelper aDisposeListeners;
	sal_Bool bDisposing;

public:
	SdXCustomPresentation() throw();
	SdXCustomPresentation( SdCustomShow* mpSdCustomShow, SdXImpressDocument* pMyModel) throw();
	virtual ~SdXCustomPresentation() throw();

	// internal
	void Invalidate() { mpSdCustomShow = NULL; }
	SdCustomShow* GetSdCustomShow() const throw() { return mpSdCustomShow; }
	void SetSdCustomShow( SdCustomShow* pShow ) throw() { mpSdCustomShow = pShow; }
	SdXImpressDocument* GetModel() const throw() { return mpModel; }

	// uno helper
	UNO3_GETIMPLEMENTATION_DECL(SdXCustomPresentation)

	// XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	// XIndexContainer
    virtual void SAL_CALL insertByIndex( sal_Int32 Index, const ::com::sun::star::uno::Any& Element );
    virtual void SAL_CALL removeByIndex( sal_Int32 Index );

	// XIndexReplace
    virtual void SAL_CALL replaceByIndex( sal_Int32 Index, const ::com::sun::star::uno::Any& Element );

	// XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
    virtual sal_Bool SAL_CALL hasElements();

	// XIndexAccess
    virtual sal_Int32 SAL_CALL getCount() ;
    virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

	// XNamed
    virtual ::rtl::OUString SAL_CALL getName(  );
    virtual void SAL_CALL setName( const ::rtl::OUString& aName );

	// XComponent
    virtual void SAL_CALL dispose(  );
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener );
};

// --------------------------------------------------------------------------
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>

#include <cppuhelper/implbase3.hxx>

#include "unomodel.hxx"
#include "drawdoc.hxx"

class List;

class SdXCustomPresentationAccess : public ::cppu::WeakImplHelper3<	::com::sun::star::container::XNameContainer,
																	::com::sun::star::lang::XSingleServiceFactory,
																	::com::sun::star::lang::XServiceInfo >
{
private:
	SdXImpressDocument& mrModel;

	// intern
	inline List* GetCustomShowList() const throw();
	SdCustomShow * getSdCustomShow( const ::rtl::OUString& Name ) const throw();

public:
	SdXCustomPresentationAccess(SdXImpressDocument& rMyModel) throw();
	~SdXCustomPresentationAccess() throw();

	// XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	// XSingleServiceFactory
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL createInstance(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL createInstanceWithArguments( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

	// XNameContainer
    virtual void SAL_CALL insertByName( const ::rtl::OUString& aName, const ::com::sun::star::uno::Any& aElement );
    virtual void SAL_CALL removeByName( const ::rtl::OUString& Name );

	// XNameReplace
    virtual void SAL_CALL replaceByName( const ::rtl::OUString& aName, const ::com::sun::star::uno::Any& aElement );

	// XNameAccess
    virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
    virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

	// XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
    virtual sal_Bool SAL_CALL hasElements();
};

inline List * SdXCustomPresentationAccess::GetCustomShowList() const throw()
{
	if(mrModel.GetDoc())
		return mrModel.GetDoc()->GetCustomShowList(sal_False);
	else
		return NULL;
};

#endif
