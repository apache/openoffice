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



#ifndef _NUMFMUNO_HXX
#define _NUMFMUNO_HXX

#include <com/sun/star/util/XNumberFormatter.hpp>
#include <com/sun/star/util/XNumberFormatPreviewer.hpp>
#include <com/sun/star/util/XNumberFormats.hpp>
#include <com/sun/star/util/XNumberFormatTypes.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/XPropertyAccess.hpp>
#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implbase3.hxx>
#include <comphelper/sharedmutex.hxx>
#include <rtl/ref.hxx>

class SvNumberformat;
class SvNumberFormatter;
class SvNumberFormatsSupplierObj;


//	SvNumberFormatterServiceObj wird global als Service angemeldet

class SvNumberFormatterServiceObj : public cppu::WeakImplHelper3<
										com::sun::star::util::XNumberFormatter,
										com::sun::star::util::XNumberFormatPreviewer,
										com::sun::star::lang::XServiceInfo>
{
private:
    ::rtl::Reference< SvNumberFormatsSupplierObj >  xSupplier;
    mutable ::comphelper::SharedMutex               m_aMutex;

public:
						SvNumberFormatterServiceObj();
	virtual				~SvNumberFormatterServiceObj();

						// XNumberFormatter
	virtual void SAL_CALL attachNumberFormatsSupplier(
							const ::com::sun::star::uno::Reference<
								::com::sun::star::util::XNumberFormatsSupplier >& xSupplier );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatsSupplier >
							SAL_CALL getNumberFormatsSupplier();
	virtual sal_Int32 SAL_CALL detectNumberFormat( sal_Int32 nKey, const ::rtl::OUString& aString );
	virtual double SAL_CALL convertStringToNumber( sal_Int32 nKey, const ::rtl::OUString& aString );
	virtual ::rtl::OUString SAL_CALL convertNumberToString( sal_Int32 nKey, double fValue );
	virtual ::com::sun::star::util::Color SAL_CALL queryColorForNumber( sal_Int32 nKey,
							double fValue, ::com::sun::star::util::Color aDefaultColor );
	virtual ::rtl::OUString SAL_CALL formatString( sal_Int32 nKey, const ::rtl::OUString& aString );
	virtual ::com::sun::star::util::Color SAL_CALL queryColorForString( sal_Int32 nKey,
							const ::rtl::OUString& aString,
								::com::sun::star::util::Color aDefaultColor );
	virtual ::rtl::OUString SAL_CALL getInputString( sal_Int32 nKey, double fValue );

						// XNumberFormatPreviewer
	virtual ::rtl::OUString SAL_CALL convertNumberToPreviewString(
							const ::rtl::OUString& aFormat, double fValue,
							const ::com::sun::star::lang::Locale& nLocale, sal_Bool bAllowEnglish );
	virtual ::com::sun::star::util::Color SAL_CALL queryPreviewColorForNumber(
							const ::rtl::OUString& aFormat, double fValue,
							const ::com::sun::star::lang::Locale& nLocale, sal_Bool bAllowEnglish,
							::com::sun::star::util::Color aDefaultColor );

						// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(  );
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class SvNumberFormatsObj : public cppu::WeakImplHelper3<
										com::sun::star::util::XNumberFormats,
										com::sun::star::util::XNumberFormatTypes,
										com::sun::star::lang::XServiceInfo>
{
private:
	SvNumberFormatsSupplierObj&         rSupplier;
    mutable ::comphelper::SharedMutex   m_aMutex;

public:
                    SvNumberFormatsObj(SvNumberFormatsSupplierObj& pParent, ::comphelper::SharedMutex& _rMutex);
	virtual			~SvNumberFormatsObj();


						// XNumberFormats
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > SAL_CALL
							getByKey( sal_Int32 nKey );
	virtual ::com::sun::star::uno::Sequence< sal_Int32 > SAL_CALL queryKeys( sal_Int16 nType,
							const ::com::sun::star::lang::Locale& nLocale, sal_Bool bCreate );
	virtual sal_Int32 SAL_CALL queryKey( const ::rtl::OUString& aFormat,
							const ::com::sun::star::lang::Locale& nLocale, sal_Bool bScan );
	virtual sal_Int32 SAL_CALL addNew( const ::rtl::OUString& aFormat,
							const ::com::sun::star::lang::Locale& nLocale );
	virtual sal_Int32 SAL_CALL addNewConverted( const ::rtl::OUString& aFormat,
							const ::com::sun::star::lang::Locale& nLocale,
							const ::com::sun::star::lang::Locale& nNewLocale );
	virtual void SAL_CALL removeByKey( sal_Int32 nKey );
	virtual ::rtl::OUString SAL_CALL generateFormat( sal_Int32 nBaseKey,
							const ::com::sun::star::lang::Locale& nLocale, sal_Bool bThousands,
							sal_Bool bRed, sal_Int16 nDecimals, sal_Int16 nLeading );

						// XNumberFormatTypes
	virtual sal_Int32 SAL_CALL getStandardIndex( const ::com::sun::star::lang::Locale& nLocale );
	virtual sal_Int32 SAL_CALL getStandardFormat( sal_Int16 nType,
							const ::com::sun::star::lang::Locale& nLocale );
	virtual sal_Int32 SAL_CALL getFormatIndex( sal_Int16 nIndex,
							const ::com::sun::star::lang::Locale& nLocale );
	virtual sal_Bool SAL_CALL isTypeCompatible( sal_Int16 nOldType, sal_Int16 nNewType );
	virtual sal_Int32 SAL_CALL getFormatForLocale( sal_Int32 nKey,
							const ::com::sun::star::lang::Locale& nLocale );

						// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(  );
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

private:
    SvNumberFormatsObj();   // never implemented
};


class SvNumberFormatObj : public cppu::WeakImplHelper3<
										com::sun::star::beans::XPropertySet,
										com::sun::star::beans::XPropertyAccess,
										com::sun::star::lang::XServiceInfo>
{
private:
	SvNumberFormatsSupplierObj&	        rSupplier;
	sal_uLong						        nKey;
    mutable ::comphelper::SharedMutex   m_aMutex;

public:
    SvNumberFormatObj( SvNumberFormatsSupplierObj& rParent, sal_uLong nK, const ::comphelper::SharedMutex& _rMutex );
	virtual			~SvNumberFormatObj();

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo(  );
	virtual void SAL_CALL	setPropertyValue( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Any& aValue );
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(
									const ::rtl::OUString& PropertyName );
	virtual void SAL_CALL	addPropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& xListener );
	virtual void SAL_CALL	removePropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& aListener );
	virtual void SAL_CALL	addVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );
	virtual void SAL_CALL	removeVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );

							// XPropertyAccess
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL
							getPropertyValues();
	virtual void SAL_CALL	setPropertyValues( const ::com::sun::star::uno::Sequence<
								::com::sun::star::beans::PropertyValue >& aProps );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(  );
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class SvNumberFormatSettingsObj : public cppu::WeakImplHelper2<
										com::sun::star::beans::XPropertySet,
										com::sun::star::lang::XServiceInfo>
{
private:
	SvNumberFormatsSupplierObj&         rSupplier;
    mutable ::comphelper::SharedMutex   m_aMutex;

public:
                    SvNumberFormatSettingsObj( SvNumberFormatsSupplierObj& rParent, const ::comphelper::SharedMutex& _rMutex);
	virtual			~SvNumberFormatSettingsObj();


							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo(  );
	virtual void SAL_CALL	setPropertyValue( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Any& aValue );
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(
									const ::rtl::OUString& PropertyName );
	virtual void SAL_CALL	addPropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& xListener );
	virtual void SAL_CALL	removePropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& aListener );
	virtual void SAL_CALL	addVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );
	virtual void SAL_CALL	removeVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(  );
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};



#endif
