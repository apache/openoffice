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



#ifndef _SVX_ASIANCFG_HXX
#include <asiancfg.hxx>
#endif
#ifndef _SVARRAY_HXX //autogen
#include <bf_svtools/svarray.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
namespace binfilter {

//-----------------------------------------------------------------------------
using namespace utl;
using namespace rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;

#define C2U(cChar) OUString::createFromAscii(cChar)
/*N*/ const sal_Char sStartEndCharacters[] = "StartEndCharacters";
/*N*/ const sal_Char sStartCharacters[] = "StartCharacters";
/*N*/ const sal_Char sEndCharacters[] = "EndCharacters";

//-----------------------------------------------------------------------------
/*N*/ struct SvxForbiddenStruct_Impl
/*N*/ {
/*N*/ 	Locale 		aLocale;
/*N*/ 	OUString	sStartChars;
/*N*/ 	OUString	sEndChars;
/*N*/ };
//-----------------------------------------------------------------------------
/*N*/ typedef SvxForbiddenStruct_Impl* SvxForbiddenStruct_ImplPtr;
/*N*/ SV_DECL_PTRARR_DEL(SvxForbiddenStructArr, SvxForbiddenStruct_ImplPtr, 2, 2)//STRIP008 ;
/*N*/ SV_IMPL_PTRARR(SvxForbiddenStructArr, SvxForbiddenStruct_ImplPtr);
//-----------------------------------------------------------------------------
/*N*/ struct SvxAsianConfig_Impl
/*N*/ {
/*N*/ 	sal_Bool 	bKerningWesternTextOnly;
/*N*/ 	sal_Int16	nCharDistanceCompression;
/*N*/ 
/*N*/ 	SvxForbiddenStructArr	aForbiddenArr;
/*N*/ 
/*N*/ 	SvxAsianConfig_Impl() :
/*N*/ 		bKerningWesternTextOnly(sal_True),
/*N*/ 		nCharDistanceCompression(0) {}
/*N*/ };
/* -----------------------------16.01.01 15:36--------------------------------

 ---------------------------------------------------------------------------*/
/*N*/ Sequence<OUString> lcl_GetPropertyNames()
/*N*/ {
/*N*/ 	Sequence<OUString> aNames(2);
/*N*/ 	OUString* pNames = aNames.getArray();
/*N*/ 	pNames[0] = C2U("IsKerningWesternTextOnly");
/*N*/ 	pNames[1] = C2U("CompressCharacterDistance");
/*N*/ 	return aNames;;
/*N*/ }
// ---------------------------------------------------------------------------
/*N*/ SvxAsianConfig::SvxAsianConfig(sal_Bool bEnableNotify) :
/*N*/ 	::utl::ConfigItem(C2U("Office.Common/AsianLayout")),
/*N*/ 	pImpl(new SvxAsianConfig_Impl)
/*N*/ {
/*N*/ 	if(bEnableNotify)
/*N*/ 		EnableNotification(lcl_GetPropertyNames());
/*N*/ 	Load();
/*N*/ }
/* -----------------------------16.01.01 15:36--------------------------------

 ---------------------------------------------------------------------------*/
/*N*/ SvxAsianConfig::~SvxAsianConfig()
/*N*/ {
/*N*/ 	delete pImpl;
/*N*/ }
/* -----------------------------17.01.01 09:57--------------------------------

 ---------------------------------------------------------------------------*/
/*N*/ void SvxAsianConfig::Load()
/*N*/ {
/*N*/ 	Sequence<Any> aValues = GetProperties(lcl_GetPropertyNames());
/*N*/ 	const Any* pValues = aValues.getConstArray();
/*N*/ 	if(pValues[0].hasValue())
/*N*/ 		pImpl->bKerningWesternTextOnly = *(sal_Bool*) pValues[0].getValue();
/*N*/ 	pValues[1] >>= pImpl->nCharDistanceCompression;
/*N*/ 
/*N*/ 	pImpl->aForbiddenArr.DeleteAndDestroy(0, pImpl->aForbiddenArr.Count());
/*N*/ 	OUString sPropPrefix(C2U(sStartEndCharacters));
/*N*/ 	Sequence<OUString> aNodes = GetNodeNames(sPropPrefix);
/*N*/ 
/*N*/ 	Sequence<OUString> aPropNames(aNodes.getLength() * 2);
/*N*/ 	OUString* pNames = aPropNames.getArray();
/*N*/ 	sal_Int32 nName = 0;
/*N*/ 	sPropPrefix += C2U("/");
/*N*/ 	sal_Int32 nNode;
/*N*/ 	const OUString* pNodes = aNodes.getConstArray();
/*N*/ 	for(nNode = 0; nNode < aNodes.getLength(); nNode++)
/*N*/ 	{
/*?*/ 		OUString sStart(sPropPrefix);
/*?*/ 		sStart += pNodes[nNode];
/*?*/ 		sStart += C2U("/");
/*?*/ 		pNames[nName] = sStart; 	pNames[nName++] += C2U("StartCharacters");
/*?*/ 		pNames[nName] = sStart; 	pNames[nName++] += C2U("EndCharacters");
/*N*/ 	}
/*N*/ 	Sequence<Any> aNodeValues = GetProperties(aPropNames);
/*N*/ 	const Any* pNodeValues = aNodeValues.getConstArray();
/*N*/ 	nName = 0;
/*N*/ 	for(nNode = 0; nNode < aNodes.getLength(); nNode++)
/*N*/ 	{
/*?*/ 		SvxForbiddenStruct_ImplPtr pInsert = new SvxForbiddenStruct_Impl;
/*?*/ 		pInsert->aLocale.Language = pNodes[nNode].copy(0, 2);
/*?*/ 		DBG_ASSERT(pInsert->aLocale.Language.getLength(), "illegal language");
/*?*/ 		pInsert->aLocale.Country = pNodes[nNode].copy(3, 2);
/*?*/ 
/*?*/ 		pNodeValues[nName++] >>= pInsert->sStartChars;
/*?*/ 		pNodeValues[nName++] >>= pInsert->sEndChars;
/*?*/ 		pImpl->aForbiddenArr.Insert(pInsert, pImpl->aForbiddenArr.Count());
/*N*/ 	}
/*N*/ }
/* -----------------------------16.01.01 15:36--------------------------------

 ---------------------------------------------------------------------------*/
/*N*/ sal_Bool 	SvxAsianConfig::IsKerningWesternTextOnly() const
/*N*/ {
/*N*/ 	return pImpl->bKerningWesternTextOnly;
/*N*/ }
/* -----------------------------16.01.01 15:36--------------------------------

 ---------------------------------------------------------------------------*/
/*N*/ sal_Int16	SvxAsianConfig::GetCharDistanceCompression() const
/*N*/ {
/*N*/ 	return pImpl->nCharDistanceCompression;
/*N*/ }
/* -----------------------------16.01.01 15:36--------------------------------

 ---------------------------------------------------------------------------*/
/*N*/ uno::Sequence<lang::Locale> SvxAsianConfig::GetStartEndCharLocales()
/*N*/ {
/*N*/ 	Sequence<Locale> aRet(pImpl->aForbiddenArr.Count());
/*N*/ 	Locale* pRet = aRet.getArray();
/*N*/ 	for(sal_uInt16 i = 0; i < pImpl->aForbiddenArr.Count(); i++)
/*N*/ 	{
/*?*/ 		pRet[i] = pImpl->aForbiddenArr[i]->aLocale;
/*N*/ 	}
/*N*/ 	return aRet;
/*N*/ }

void SvxAsianConfig::Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames ) {}
void SvxAsianConfig::Commit() {}
}

