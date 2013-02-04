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

#define _SVSTDARR_STRINGSDTOR
#define _SVSTDARR_STRINGS
#include <bf_svtools/svstdarr.hxx>
#ifndef _SW_XMLSECTIONLIST_HXX
#include <SwXMLSectionList.hxx>
#endif
#ifndef _XMLOFF_XMLTOKEN_HXX
#include <bf_xmloff/xmltoken.hxx>
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include <bf_xmloff/nmspmap.hxx>
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include <bf_xmloff/xmlnmspe.hxx>
#endif
#ifndef _LEGACYBINFILTERMGR_HXX
#include <legacysmgr/legacy_binfilters_smgr.hxx>	//STRIP002 
#endif

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star;
using namespace ::rtl;
using namespace ::binfilter::xmloff::token;
#include <tools/debug.hxx>  //STRIP001 
namespace binfilter {
sal_Char __READONLY_DATA sXML_np__block_list[] = "_block-list";

// #110680#
/*N*/ SwXMLSectionList::SwXMLSectionList(
/*N*/ 	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xServiceFactory,
/*N*/ 	SvStrings & rNewSectionList)
/*N*/ :	SvXMLImport( xServiceFactory ),
/*N*/ 	rSectionList ( rNewSectionList )
/*N*/ {
/*?*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 GetNamespaceMap().Add( OUString ( RTL_CONSTASCII_USTRINGPARAM ( sXML_np__block_list ) ),
/*N*/ }






}
