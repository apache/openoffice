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



#ifndef SC_ADDINLIS_HXX
#define SC_ADDINLIS_HXX

#ifndef _SC_ADIASYNC_HXX
#include "adiasync.hxx"			// for ScAddInDocs PtrArr
#endif

#ifndef _COM_SUN_STAR_SHEET_XRESULTLISTENER_HPP_
#include <com/sun/star/sheet/XResultListener.hpp>
#endif

#ifndef _COM_SUN_STAR_SHEET_XVOLATILERESULT_HPP_
#include <com/sun/star/sheet/XVolatileResult.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif
namespace binfilter {



class ScDocument;


class ScAddInListener : public cppu::WeakImplHelper2<
							::com::sun::star::sheet::XResultListener,
							::com::sun::star::lang::XServiceInfo >,
						public SfxBroadcaster
{
private:
	::com::sun::star::uno::Reference< ::com::sun::star::sheet::XVolatileResult> xVolRes;
	::com::sun::star::uno::Any	aResult;
	ScAddInDocs*				pDocs;			// documents where this is used

	static List					aAllListeners;

							// always allocated via CreateListener
							ScAddInListener(
								::com::sun::star::uno::Reference<
									::com::sun::star::sheet::XVolatileResult> xVR,
								ScDocument* pD );

public:
	virtual					~ScAddInListener();

							// create Listener and put it into global list
	static ScAddInListener*	CreateListener(
								::com::sun::star::uno::Reference<
									::com::sun::star::sheet::XVolatileResult> xVR,
								ScDocument* pDoc );

	static ScAddInListener*	Get( ::com::sun::star::uno::Reference<
									::com::sun::star::sheet::XVolatileResult> xVR );
	static void				RemoveDocument( ScDocument* pDocument );

	BOOL					HasDocument( ScDocument* pDoc ) const	{ return pDocs->Seek_Entry( pDoc ); }
	void					AddDocument( ScDocument* pDoc )			{ pDocs->Insert( pDoc ); }
	const ::com::sun::star::uno::Any& GetResult() const				{ return aResult; }


							// XResultListener
	virtual void SAL_CALL	modified( const ::com::sun::star::sheet::ResultEvent& aEvent )
								throw(::com::sun::star::uno::RuntimeException);

							// XEventListener
	virtual void SAL_CALL	disposing( const ::com::sun::star::lang::EventObject& Source )
								throw(::com::sun::star::uno::RuntimeException);

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(  )
								throw(::com::sun::star::uno::RuntimeException);
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName )
								throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  )
								throw(::com::sun::star::uno::RuntimeException);
};


} //namespace binfilter
#endif

