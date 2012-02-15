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



#ifndef SC_FUNCUNO_HXX
#define SC_FUNCUNO_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XFUNCTIONACCESS_HPP_
#include <com/sun/star/sheet/XFunctionAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#ifndef _CPPUHELPER_IMPLBASE3_HXX_
#include <cppuhelper/implbase3.hxx>
#endif

#ifndef _SFXLSTNER_HXX
#include <bf_svtools/lstner.hxx>
#endif
namespace binfilter {

class ScDocument;
class ScDocOptions;


::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
	ScFunctionAccess_CreateInstance(
		const ::com::sun::star::uno::Reference<
			::com::sun::star::lang::XMultiServiceFactory >& );


class ScTempDocCache
{
private:
	ScDocument*		pDoc;
	BOOL			bInUse;

public:
				ScTempDocCache();
				~ScTempDocCache();

	ScDocument*	GetDocument() const		{ return pDoc; }
	BOOL		IsInUse() const			{ return bInUse; }
	void		SetInUse( BOOL bSet )	{ bInUse = bSet; }

	void		SetDocument( ScDocument* pNew );
	void		Clear();
};

class ScFunctionAccess : public cppu::WeakImplHelper3<
										::com::sun::star::sheet::XFunctionAccess,
										::com::sun::star::beans::XPropertySet,
										::com::sun::star::lang::XServiceInfo>,
						 public SfxListener
{
private:
	ScTempDocCache	aDocCache;
	ScDocOptions* 	pOptions;
	BOOL			bInvalid;

public:
							ScFunctionAccess();
	virtual					~ScFunctionAccess();

	static ::rtl::OUString	getImplementationName_Static();
	static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XFunctionAccess
	virtual ::com::sun::star::uno::Any SAL_CALL callFunction(
									const ::rtl::OUString& aName,
									const ::com::sun::star::uno::Sequence<
										::com::sun::star::uno::Any >& aArguments )
								throw(::com::sun::star::container::NoSuchElementException,
									::com::sun::star::lang::IllegalArgumentException,
									::com::sun::star::uno::RuntimeException);

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo()
								throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	setPropertyValue( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Any& aValue )
								throw(::com::sun::star::beans::UnknownPropertyException,
									::com::sun::star::beans::PropertyVetoException,
									::com::sun::star::lang::IllegalArgumentException,
									::com::sun::star::lang::WrappedTargetException,
									::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(
									const ::rtl::OUString& PropertyName )
								throw(::com::sun::star::beans::UnknownPropertyException,
									::com::sun::star::lang::WrappedTargetException,
									::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	addPropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& xListener )
								throw(::com::sun::star::beans::UnknownPropertyException,
									::com::sun::star::lang::WrappedTargetException,
									::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	removePropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& aListener )
								throw(::com::sun::star::beans::UnknownPropertyException,
									::com::sun::star::lang::WrappedTargetException,
									::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	addVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener )
								throw(::com::sun::star::beans::UnknownPropertyException,
									::com::sun::star::lang::WrappedTargetException,
									::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	removeVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener )
								throw(::com::sun::star::beans::UnknownPropertyException,
									::com::sun::star::lang::WrappedTargetException,
									::com::sun::star::uno::RuntimeException);

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName()
								throw(::com::sun::star::uno::RuntimeException);
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName )
								throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames()
								throw(::com::sun::star::uno::RuntimeException);
};


} //namespace binfilter
#endif

