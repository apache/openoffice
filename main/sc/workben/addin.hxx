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



#ifndef SC_ADDIN_HXX
#define SC_ADDIN_HXX

#include <com/sun/star/sheet/XAddIn.hpp>
#include <com/sun/star/lang/XServiceName.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <stardiv/starcalc/test/XTestAddIn.hpp>

#include <cppuhelper/implbase4.hxx>	// helper for implementations


com::sun::star::uno::Reference<com::sun::star::uno::XInterface> ScTestAddIn_CreateInstance(
	const com::sun::star::uno::Reference<com::sun::star::lang::XMultiServiceFactory>& );


class ScTestAddIn : public cppu::WeakImplHelper4<
						com::sun::star::sheet::XAddIn,
						stardiv::starcalc::test::XTestAddIn,
						com::sun::star::lang::XServiceName,
						com::sun::star::lang::XServiceInfo >
{
private:
	com::sun::star::uno::Reference<com::sun::star::sheet::XVolatileResult>	xAlphaResult;	//! Test
	com::sun::star::uno::Reference<com::sun::star::sheet::XVolatileResult>	xNumResult;		//! Test
	com::sun::star::lang::Locale											aFuncLoc;

public:
							ScTestAddIn();
	virtual					~ScTestAddIn();

//							SMART_UNO_DECLARATION( ScTestAddIn, UsrObject );
//	friend Reflection *		ScTestAddIn_getReflection();
//	virtual BOOL			queryInterface( Uik, XInterfaceRef& );
//	virtual XIdlClassRef	getIdlClass(void);

	static UString			getImplementationName_Static();
	static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static();

							// XAddIn
    virtual ::rtl::OUString SAL_CALL getProgrammaticFuntionName( const ::rtl::OUString& aDisplayName );
    virtual ::rtl::OUString SAL_CALL getDisplayFunctionName( const ::rtl::OUString& aProgrammaticName );
    virtual ::rtl::OUString SAL_CALL getFunctionDescription( const ::rtl::OUString& aProgrammaticName );
    virtual ::rtl::OUString SAL_CALL getDisplayArgumentName( const ::rtl::OUString& aProgrammaticFunctionName, sal_Int32 nArgument );
    virtual ::rtl::OUString SAL_CALL getArgumentDescription( const ::rtl::OUString& aProgrammaticFunctionName, sal_Int32 nArgument );
    virtual ::rtl::OUString SAL_CALL getProgrammaticCategoryName( const ::rtl::OUString& aProgrammaticFunctionName );
    virtual ::rtl::OUString SAL_CALL getDisplayCategoryName( const ::rtl::OUString& aProgrammaticFunctionName );

							// XLocalizable
    virtual void SAL_CALL setLocale( const ::com::sun::star::lang::Locale& eLocale );
    virtual ::com::sun::star::lang::Locale SAL_CALL getLocale(  );

							// XTestAddIn
    virtual sal_Int32 SAL_CALL countParams( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArgs );
    virtual double SAL_CALL addOne( double fValue );
    virtual ::rtl::OUString SAL_CALL repeatStr( const ::rtl::OUString& aStr, sal_Int32 nCount );
    virtual ::rtl::OUString SAL_CALL getDateString( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& xCaller, double fValue );
    virtual sal_Int32 SAL_CALL getColorValue( const ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange >& xRange );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< double > > SAL_CALL transpose( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< double > >& aMatrix );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< sal_Int32 > > SAL_CALL transposeInt( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< sal_Int32 > >& aMatrix );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XVolatileResult > SAL_CALL callAsync( const ::rtl::OUString& aString );
    virtual ::rtl::OUString SAL_CALL repeatMultiple( sal_Int32 nCount, const ::com::sun::star::uno::Any& aFirst, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aFollow );
    virtual ::com::sun::star::uno::Any SAL_CALL getStrOrVal( sal_Int32 nFlag );

							// XServiceName
    virtual ::rtl::OUString SAL_CALL getServiceName(  );

							// XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );
};


#endif
