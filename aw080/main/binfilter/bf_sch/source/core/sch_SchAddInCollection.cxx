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



#include "SchAddInCollection.hxx"

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCONTENTENUMERATIONACCESS_HPP_
#include <com/sun/star/container/XContentEnumerationAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICENAME_HPP_
#include <com/sun/star/lang/XServiceName.hpp>
#endif
#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif
#include<tools/debug.hxx> //STRIP001 
#ifndef _LEGACYBINFILTERMGR_HXX
#include <legacysmgr/legacy_binfilters_smgr.hxx>	//STRIP002 
#endif
namespace binfilter {
using namespace ::com::sun::star;

/*N*/ SchAddInCollection::SchAddInCollection() :
/*N*/         mbInitialized( sal_False )
/*N*/ {
/*N*/ }

/*N*/ SchAddInCollection::~SchAddInCollection()
/*N*/ {}

/*N*/ void SchAddInCollection::Initialize()
/*N*/ {
/*N*/     // collect all service names of available AddIns
/*N*/ 
/*N*/     try
/*N*/     {
/*N*/         uno::Reference< lang::XMultiServiceFactory > xFactory = ::legacy_binfilters::getLegacyProcessServiceFactory();
/*N*/         uno::Reference< container::XContentEnumerationAccess > xEnumAcc( xFactory, uno::UNO_QUERY );
/*N*/         if( xEnumAcc.is())
/*N*/         {
/*N*/             uno::Reference< container::XEnumeration > xEnum =
/*N*/                 xEnumAcc->createContentEnumeration( ::rtl::OUString::createFromAscii( "com.sun.star.chart.Diagram" ));
/*N*/             if( xEnum.is())
/*N*/             {
/*N*/                 // clear possibly existing list
/*N*/                 if( mbInitialized )
/*N*/                     maServiceNames.realloc( 0 );
/*N*/ 
/*N*/                 while( xEnum->hasMoreElements())
/*N*/                 {
/*N*/                     // create Instance to collect the service name of the addin
/*N*/                     uno::Any aFactoryAny = xEnum->nextElement();
/*N*/                     uno::Reference< uno::XInterface > xFactoryIntf;
/*N*/                     aFactoryAny >>= xFactoryIntf;
/*N*/                     if( xFactoryIntf.is())
/*N*/                     {
/*N*/                         uno::Reference< lang::XSingleServiceFactory > xAddInFactory( xFactoryIntf, uno::UNO_QUERY );
/*N*/                         if( xAddInFactory.is())
/*N*/                         {
/*N*/                             uno::Reference< lang::XServiceName > xServicName(
/*N*/                                 xAddInFactory->createInstance(), uno::UNO_QUERY );
/*N*/                             if( xServicName.is())
/*N*/                             {
/*N*/                                 sal_Int32 nIndex = maServiceNames.getLength();
/*N*/                                 maServiceNames.realloc( nIndex + 1 );
/*N*/                                 maServiceNames[ nIndex ] = xServicName->getServiceName();
/*N*/                             }
/*N*/                         }
/*N*/                     }
/*N*/                 }
/*N*/             }
/*N*/         }
/*N*/         mbInitialized = sal_True;
/*N*/     }
/*N*/     catch( const uno::Exception& aEx )
/*N*/     {
/*N*/         OSL_ENSURE( false,
/*N*/                     ::rtl::OUStringToOString(
/*N*/                         ::rtl::OUString(
/*N*/                             RTL_CONSTASCII_USTRINGPARAM(
/*N*/                                 "Couldn't initialize add-in collection: " )) +
/*N*/                         aEx.Message,
/*N*/                         RTL_TEXTENCODING_ASCII_US ).getStr() );
/*N*/     }
/*N*/ }

/*N*/ uno::Reference< util::XRefreshable > SchAddInCollection::GetAddInByName( const ::rtl::OUString& rName )
/*N*/ {
/*N*/     if( ! mbInitialized )
/*?*/         {DBG_BF_ASSERT(0, "STRIP");} //STRIP001 Initialize();
/*N*/ 
/*N*/     const sal_Int32 nSize = maServiceNames.getLength();
/*N*/     for( sal_Int32 i = 0; i < nSize; i++ )
/*N*/     {
/*N*/         if( rName.equalsIgnoreAsciiCase( maServiceNames[ i ] ))
/*N*/         {
/*N*/             uno::Reference< lang::XMultiServiceFactory > xFactory = ::legacy_binfilters::getLegacyProcessServiceFactory();
/*N*/             return uno::Reference< util::XRefreshable >( xFactory->createInstance( rName ), uno::UNO_QUERY );
/*N*/         }
/*N*/     }
/*N*/ 
/*N*/     return uno::Reference< util::XRefreshable >();
/*N*/ }

/*N*/ uno::Sequence< ::rtl::OUString > SchAddInCollection::GetAddInNames()
/*N*/ {
/*N*/     if( ! mbInitialized )
/*N*/         Initialize();
/*N*/ 
/*N*/     return maServiceNames;
/*N*/ }
}
