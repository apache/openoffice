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



#ifdef PCH
#include "ui_pch.hxx"
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

//------------------------------------------------------------------

#include <bf_svx/unolingu.hxx>
#include <bf_svtools/lingucfg.hxx>

#include <com/sun/star/linguistic2/XThesaurus.hpp>
#include <com/sun/star/lang/Locale.hpp>

using namespace ::com::sun::star;

#include "scmod.hxx"
namespace binfilter {

//------------------------------------------------------------------

//#define LINGUPROP_DEFLOCALE			"DefaultLocale"
//#define LINGUPROP_CJKLOCALE			"DefaultLocale_CJK"
//#define LINGUPROP_CTLLOCALE			"DefaultLocale_CTL"
#define LINGUPROP_AUTOSPELL			"IsSpellAuto"
#define LINGUPROP_HIDEAUTO			"IsSpellHide"

//------------------------------------------------------------------

// static
/*N*/ void ScModule::GetSpellSettings( USHORT& rDefLang, USHORT& rCjkLang, USHORT& rCtlLang,
/*N*/ 									BOOL& rAutoSpell, BOOL& rHideAuto )
/*N*/ {
/*N*/ 	//	use SvtLinguConfig instead of service LinguProperties to avoid
/*N*/ 	//	loading the linguistic component
/*N*/ 	SvtLinguConfig aConfig;
/*N*/ 
/*N*/ 	SvtLinguOptions aOptions;
/*N*/ 	aConfig.GetOptions( aOptions );
/*N*/ 
/*N*/ 	rDefLang = aOptions.nDefaultLanguage;
/*N*/ 	rCjkLang = aOptions.nDefaultLanguage_CJK;
/*N*/ 	rCtlLang = aOptions.nDefaultLanguage_CTL;
/*N*/ 	rAutoSpell = aOptions.bIsSpellAuto;
/*N*/ 	rHideAuto = FALSE;
/*N*/ }

// static

// static


// static


}
