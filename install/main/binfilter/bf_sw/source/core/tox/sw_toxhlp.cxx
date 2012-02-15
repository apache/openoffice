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




#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#ifndef _COM_SUN_STAR_I18N_XINDEXENTRYSUPPLIER_HPP_
#include <com/sun/star/i18n/XExtendedIndexEntrySupplier.hpp>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _TOXWRAP_HXX
#include <toxwrap.hxx>
#endif
#ifndef _LEGACYBINFILTERMGR_HXX
#include <legacysmgr/legacy_binfilters_smgr.hxx>	//STRIP002 
#endif
namespace binfilter {

 IndexEntrySupplierWrapper::IndexEntrySupplierWrapper()
 {
     ::com::sun::star::uno::Reference<
             ::com::sun::star::lang::XMultiServiceFactory > rxMSF =
                                     ::legacy_binfilters::getLegacyProcessServiceFactory();
 
    try {
        STAR_REFERENCE( uno::XInterface ) xI =
            rxMSF->createInstance( ::rtl::OUString::createFromAscii(
                                 "com.sun.star.i18n.IndexEntrySupplier" ) );
        if( xI.is() )
        {
            UNO_NMSPC::Any x = xI->queryInterface( ::getCppuType(
                     (const ::com::sun::star::uno::Reference< ::com::sun::star::i18n::XExtendedIndexEntrySupplier>*)0) );
            x >>= xIES;
        }
    }
    catch ( UNO_NMSPC::Exception& e )
    {
 #ifdef DBG_UTIL
        ByteString aMsg( "IndexEntrySupplierWrapper: Exception caught\n" );
        aMsg += ByteString( String( e.Message ), RTL_TEXTENCODING_UTF8 );
        DBG_ERRORFILE( aMsg.GetBuffer() );
 #endif
    }
 }

 IndexEntrySupplierWrapper::~IndexEntrySupplierWrapper()
 {
 }

 STAR_NMSPC::uno::Sequence< ::rtl::OUString >
 IndexEntrySupplierWrapper::GetAlgorithmList( const STAR_NMSPC::lang::Locale& rLcl ) const
 {
     ::com::sun::star::uno::Sequence< ::rtl::OUString > sRet;
 
    try {
         sRet = xIES->getAlgorithmList( rLcl );
    }
    catch ( UNO_NMSPC::Exception& e )
    {
 #ifdef DBG_UTIL
         ByteString aMsg( "getAlgorithmList: Exception caught\n" );
        aMsg += ByteString( String( e.Message ), RTL_TEXTENCODING_UTF8 );
        DBG_ERRORFILE( aMsg.GetBuffer() );
 #endif
    }
    return sRet;
 }

 sal_Bool IndexEntrySupplierWrapper::LoadAlgorithm(
         const STAR_NMSPC::lang::Locale& rLcl,
         const String& sSortAlgorithm, long nOptions ) const
 {
     sal_Bool bRet = sal_False;
    try {
         bRet = xIES->loadAlgorithm( rLcl, sSortAlgorithm, nOptions );
    }
    catch ( UNO_NMSPC::Exception& e )
    {
 #ifdef DBG_UTIL
         ByteString aMsg( "loadAlgorithm: Exception caught\n" );
        aMsg += ByteString( String( e.Message ), RTL_TEXTENCODING_UTF8 );
        DBG_ERRORFILE( aMsg.GetBuffer() );
 #endif
    }
     return bRet;
 }

 sal_Int16 IndexEntrySupplierWrapper::CompareIndexEntry(
             const String& rTxt1, const String& rTxtReading1,
             const STAR_NMSPC::lang::Locale& rLocale1,
             const String& rTxt2, const String& rTxtReading2,
             const STAR_NMSPC::lang::Locale& rLocale2 ) const
 {
     sal_Int16 nRet = 0;
    try {
         nRet = xIES->compareIndexEntry( rTxt1, rTxtReading1, rLocale1,
                                         rTxt2, rTxtReading2, rLocale2 );
    }
    catch ( UNO_NMSPC::Exception& e )
    {
 #ifdef DBG_UTIL
         ByteString aMsg( "compareIndexEntry: Exception caught\n" );
        aMsg += ByteString( String( e.Message ), RTL_TEXTENCODING_UTF8 );
        DBG_ERRORFILE( aMsg.GetBuffer() );
 #endif
    }
     return nRet;
 }
}
