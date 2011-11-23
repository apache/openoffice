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

#ifndef _COM_SUN_STAR_I18N_SCRIPTTYPE_HDL_
#include <com/sun/star/i18n/ScriptType.hdl>
#endif
#ifndef _UNOTOOLS_LOCALEDATAWRAPPER_HXX
#include <unotools/localedatawrapper.hxx>
#endif
#ifndef _SVX_LINGU_HXX
#include <bf_svx/unolingu.hxx>
#endif
#ifndef _SVX_SCRIPTTYPEITEM_HXX
#include <bf_svx/scripttypeitem.hxx>
#endif

#ifndef _BREAKIT_HXX
#include <breakit.hxx>
#endif
#ifndef _LEGACYBINFILTERMGR_HXX
#include <legacysmgr/legacy_binfilters_smgr.hxx>	//STRIP002 
#endif

#ifndef _SWTYPES_HXX
#include "swtypes.hxx"
#endif

namespace binfilter {
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::i18n;


/*N*/ SwBreakIt::SwBreakIt()
/*N*/ 	: pLocale( NULL ), pForbidden( NULL )
/*N*/ {
/*N*/ 	_GetLocale( (LanguageType)GetAppLanguage() );
/*N*/ 	Reference< XMultiServiceFactory > xMSF = ::legacy_binfilters::getLegacyProcessServiceFactory();
/*N*/ 	Reference < XInterface > xI = xMSF->createInstance(
/*N*/ 		::rtl::OUString::createFromAscii( "com.sun.star.i18n.BreakIterator" ) );
/*N*/ 	if ( xI.is() )
/*N*/ 	{
/*N*/ 		Any x = xI->queryInterface( ::getCppuType((const Reference< XBreakIterator >*)0) );
/*N*/ 		x >>= xBreak;
/*N*/ 	}
/*N*/ }

/*N*/ void SwBreakIt::_GetLocale( const LanguageType aLang )
/*N*/ {
/*N*/ 	aLast = aLang;
/*N*/ 	delete pLocale;
/*N*/ 	pLocale = new Locale( SvxCreateLocale( aLast ) );
/*N*/ }

/*N*/ void SwBreakIt::_GetForbidden( const LanguageType aLang )
/*N*/ {
/*N*/ 	aForbiddenLang = aLang;
/*N*/ 	Reference< XMultiServiceFactory > xMSF = ::legacy_binfilters::getLegacyProcessServiceFactory();
/*N*/ 	LocaleDataWrapper aWrap( xMSF, GetLocale( aLang ) );
/*N*/ 	delete pForbidden;
/*N*/ 	pForbidden = new ForbiddenCharacters( aWrap.getForbiddenCharacters() );
/*N*/ }

/*N*/ USHORT SwBreakIt::GetRealScriptOfText( const String& rTxt,
/*N*/ 										xub_StrLen nPos ) const
/*N*/ {
/*N*/ 	USHORT nScript = ScriptType::WEAK;
/*N*/ 	if( xBreak.is() && rTxt.Len() )
/*N*/ 	{
/*N*/ 		if( nPos && nPos == rTxt.Len() )
/*N*/ 			--nPos;
/*N*/ 		nScript = xBreak->getScriptType( rTxt, nPos );
/*N*/ 		sal_Int32 nChgPos;
/*N*/ 		if( ScriptType::WEAK == nScript && nPos &&
/*N*/ 			0 < (nChgPos = xBreak->beginOfScript( rTxt, nPos, nScript )) )
/*N*/ 			nScript = xBreak->getScriptType( rTxt, nChgPos-1 );
/*N*/ 
/*N*/ 		if( ScriptType::WEAK == nScript && rTxt.Len() >
/*N*/ 			( nChgPos = xBreak->endOfScript( rTxt, nPos, nScript ) ) &&
/*N*/ 			0 <= nChgPos )
/*N*/ 			nScript = xBreak->getScriptType( rTxt, nChgPos );
/*N*/ 	}
/*N*/ 	if( ScriptType::WEAK == nScript )
/*N*/ 	  nScript = GetI18NScriptTypeOfLanguage( (USHORT)GetAppLanguage() );
/*N*/ 	return nScript;
/*N*/ }


}
