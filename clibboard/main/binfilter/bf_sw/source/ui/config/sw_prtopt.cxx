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

#ifndef _PRTOPT_HXX
#include <prtopt.hxx>
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
namespace binfilter {

using namespace utl;
using namespace rtl;
using namespace ::com::sun::star::uno;

/*N*/ #define C2U(cChar) OUString::createFromAscii(cChar)
/*--------------------------------------------------------------------
	 Beschreibung: Ctor
 --------------------------------------------------------------------*/

/*N*/ Sequence<OUString> SwPrintOptions::GetPropertyNames()
/*N*/ {
/*N*/ 	static const char* aPropNames[] =
/*N*/ 	{
/*N*/ 		"Content/Graphic",				//  0
/*N*/ 		"Content/Table",            	//  1
/*N*/ 		"Content/Control",          	//  2
/*N*/ 		"Content/Background",       	//  3
/*N*/ 		"Content/PrintBlack",       	//  4
/*N*/ 		"Content/Note",             	//  5
/*N*/ 		"Page/Reversed",            	//  6
/*N*/ 		"Page/Brochure",            	//  7
/*N*/ 		"Output/SinglePrintJob",    	//  8
/*N*/ 		"Output/Fax",               	//  9
/*N*/ 		"Papertray/FromPrinterSetup",	// 10
/*N*/ 		"Content/Drawing",          	// 11 not in SW/Web
/*N*/ 		"Page/LeftPage",            	// 12 not in SW/Web
/*N*/ 		"Page/RightPage"            	// 13 not in SW/Web
/*N*/ 	};
/*N*/ 	const int nCount = bIsWeb ? 11 : 14;
/*N*/ 	Sequence<OUString> aNames(nCount);
/*N*/ 	OUString* pNames = aNames.getArray();
/*N*/ 	for(int i = 0; i < nCount; i++)
/*N*/ 	{
/*N*/ 		pNames[i] = OUString::createFromAscii(aPropNames[i]);
/*N*/ 	}
/*N*/ 	return aNames;
/*N*/ }
/* -----------------------------06.09.00 16:44--------------------------------

 ---------------------------------------------------------------------------*/
/*N*/ SwPrintOptions::SwPrintOptions(sal_Bool bWeb) :
/*N*/     ConfigItem(bWeb ? C2U("Office.WriterWeb/Print") :  C2U("Office.Writer/Print"),
/*N*/         CONFIG_MODE_DELAYED_UPDATE|CONFIG_MODE_RELEASE_TREE),
/*N*/ 	bIsWeb(bWeb)
/*N*/ {
/*N*/     bPrintPageBackground = !bWeb;
/*N*/     bPrintBlackFont = bWeb;
/*N*/     Sequence<OUString> aNames = GetPropertyNames();
/*N*/ 	Sequence<Any> aValues = GetProperties(aNames);
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
/*N*/ 					case  0: bPrintGraphic 		= *(sal_Bool*)pValues[nProp].getValue(); break;
/*N*/ 					case  1: bPrintTable		 	= *(sal_Bool*)pValues[nProp].getValue();  break;
/*N*/ 					case  2: bPrintControl	  	= *(sal_Bool*)pValues[nProp].getValue()	;  break;
/*N*/ 					case  3: bPrintPageBackground= *(sal_Bool*)pValues[nProp].getValue();  break;
/*N*/ 					case  4: bPrintBlackFont	    = *(sal_Bool*)pValues[nProp].getValue();  break;
/*N*/                     case  5:
/*N*/                     {
/*N*/                         sal_Int32 nTmp;
/*N*/                         pValues[nProp] >>=  nTmp;
/*N*/                         nPrintPostIts = (sal_Int16)nTmp;
/*N*/                     }
/*N*/                     break;
/*N*/                     case  6: bPrintReverse      = *(sal_Bool*)pValues[nProp].getValue();  break;
/*N*/ 					case  7: bPrintProspect      = *(sal_Bool*)pValues[nProp].getValue();  break;
/*N*/ 					case  8: bPrintSingleJobs	= *(sal_Bool*)pValues[nProp].getValue();  break;
/*N*/ 					case  9: pValues[nProp] >>= sFaxName;  break;
/*N*/ 					case 10: bPaperFromSetup	= *(sal_Bool*)pValues[nProp].getValue();  break;
/*N*/ 					case 11: bPrintDraw		  	= *(sal_Bool*)pValues[nProp].getValue()	;  break;
/*N*/ 					case 12: bPrintLeftPage		= *(sal_Bool*)pValues[nProp].getValue();  break;
/*N*/ 					case 13: bPrintRightPage	    = *(sal_Bool*)pValues[nProp].getValue();  break;
/*N*/ 				}
/*N*/ 			}
/*N*/ 		}
/*N*/ 	}
/*N*/ }
/* -----------------------------06.09.00 16:50--------------------------------

 ---------------------------------------------------------------------------*/
/*N*/ SwPrintOptions::~SwPrintOptions()
/*N*/ {
/*N*/ }
/* -----------------------------06.09.00 16:43--------------------------------

 ---------------------------------------------------------------------------*/

	void SwPrintOptions::Commit() {}
	void SwPrintOptions::Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames ) {}



}
