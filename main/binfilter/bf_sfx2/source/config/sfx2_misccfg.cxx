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

#ifndef _ZFORLIST_HXX //autogen
#include <bf_svtools/zforlist.hxx>
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#include "misccfg.hxx"
namespace binfilter {

#define DEFAULT_TAB 2000

#define DEF_INCH	2540L
#define DEF_RELTWIP	1440L

using namespace rtl;
using namespace ::com::sun::star::uno;

#define C2U(cChar) OUString::createFromAscii(cChar)
/*--------------------------------------------------------------------
	 Beschreibung:
 --------------------------------------------------------------------*/
/*N*/ SfxMiscCfg::SfxMiscCfg() :
/*N*/ 	ConfigItem(C2U("Office.Common") ),
/*N*/ 	nYear2000( SvNumberFormatter::GetYear2000Default() ),
/*N*/ 	bNotFound (FALSE),
/*N*/ 	bPaperSize(FALSE),
/*N*/ 	bPaperOrientation (FALSE)
/*N*/ {
/*N*/ 	Load();
/*N*/ }
/* -----------------------------02.03.01 15:31--------------------------------

 ---------------------------------------------------------------------------*/
/*N*/ SfxMiscCfg::~SfxMiscCfg()
/*N*/ {
/*N*/ }
/* -----------------------------02.03.01 15:31--------------------------------

 ---------------------------------------------------------------------------*/
/*N*/ const Sequence<OUString>& SfxMiscCfg::GetPropertyNames()
/*N*/ {
/*N*/ 	static Sequence<OUString> aNames;
/*N*/ 	if(!aNames.getLength())
/*N*/ 	{
/*N*/ 		static const char* aPropNames[] =
/*N*/ 		{
/*N*/    			"Print/Warning/PaperSize",				//  0
/*N*/    			"Print/Warning/PaperOrientation",		//  1
/*N*/    			"Print/Warning/NotFound",				//  2
/*N*/ 			"DateFormat/TwoDigitYear",            	//  3
/*N*/ 		};
/*N*/         const int nCount = 4;
/*N*/ 		aNames.realloc(nCount);
/*N*/ 		OUString* pNames = aNames.getArray();
/*N*/ 		for(int i = 0; i < nCount; i++)
/*N*/ 			pNames[i] = OUString::createFromAscii(aPropNames[i]);
/*N*/ 	}
/*N*/ 	return aNames;
/*N*/ }
/* -----------------------------02.03.01 15:31--------------------------------

 ---------------------------------------------------------------------------*/
/*N*/ void SfxMiscCfg::Load()
/*N*/ {
/*N*/ 	const Sequence<OUString>& aNames = GetPropertyNames();
/*N*/ 	Sequence<Any> aValues = GetProperties(aNames);
/*N*/ 	EnableNotification(aNames);
/*N*/ 	const Any* pValues = aValues.getConstArray();
/*N*/ 	DBG_ASSERT(aValues.getLength() == aNames.getLength(), "GetProperties failed");
/*N*/ 	if(aValues.getLength() == aNames.getLength())
/*N*/ 	{
/*N*/ 		for(int nProp = 0; nProp < aNames.getLength(); nProp++)
/*N*/ 		{
/*N*/ 			if(pValues[nProp].hasValue())
/*N*/ 			{
/*N*/ 				switch(nProp)
/*N*/ 				{
/*N*/ 					case  0: bPaperSize 	   = *(sal_Bool*)pValues[nProp].getValue(); break;		//"Print/Warning/PaperSize",
/*N*/                     case  1: bPaperOrientation = *(sal_Bool*)pValues[nProp].getValue();  break;     //"Print/Warning/PaperOrientation",
/*N*/                     case  2: bNotFound         = *(sal_Bool*)pValues[nProp].getValue()  ;  break;   //"Print/Warning/NotFound",
/*N*/ 					case  3: pValues[nProp] >>= nYear2000;break;                                    //"DateFormat/TwoDigitYear",
/*N*/ 				}
/*N*/ 			}
/*N*/ 		}
/*N*/ 	}
/*N*/ }
/* -----------------------------02.03.01 15:31--------------------------------

 ---------------------------------------------------------------------------*/
/*?*/ void SfxMiscCfg::Notify( const ::com::sun::star::uno::Sequence<rtl::OUString>& aPropertyNames)
/*?*/ {
/*?*/ 	Load();
/*?*/ }
/* -----------------------------02.03.01 15:31--------------------------------

 ---------------------------------------------------------------------------*/
/*?*/ void SfxMiscCfg::Commit()
/*?*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*?*/ }

}
