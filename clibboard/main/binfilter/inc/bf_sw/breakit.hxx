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



#ifndef _BREAKIT_HXX
#define _BREAKIT_HXX

#include <bf_svtools/bf_solar.h>
#ifndef INCLUDED_I18NPOOL_LANG_H
#include <i18npool/lang.h>
#endif

/*************************************************************************
 *						class SwBreakIt
 *************************************************************************/

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _COM_SUN_STAR_I18N_XBREAKITERATOR_HPP_
#include <com/sun/star/i18n/XBreakIterator.hpp>
#endif

#ifndef _COM_SUN_STAR_I18N_FORBIDDENCHARACTERS_HDL_
#include <com/sun/star/i18n/ForbiddenCharacters.hdl>
#endif
class String; 
namespace binfilter {



class SwBreakIt
{
public:
	::com::sun::star::uno::Reference < ::com::sun::star::i18n::XBreakIterator > xBreak;
private:
	::com::sun::star::lang::Locale* pLocale;
	::com::sun::star::i18n::ForbiddenCharacters* pForbidden;
	LanguageType aLast;          // language of the current locale
	LanguageType aForbiddenLang; // language of the current forbiddenChar struct

	void _GetLocale( const LanguageType aLang );
	void _GetForbidden( const LanguageType	aLang );
public:
	SwBreakIt();
	~SwBreakIt() { delete pLocale; delete pForbidden; }
	::com::sun::star::lang::Locale& GetLocale( const LanguageType aLang )
	{
		if( aLast != aLang )
			_GetLocale( aLang );
		return *pLocale;
	}
	::com::sun::star::i18n::ForbiddenCharacters& GetForbidden( const LanguageType aLang )
	{
		if( !pForbidden || aForbiddenLang != aLang )
			_GetForbidden( aLang );
		return *pForbidden;
	}

	USHORT GetRealScriptOfText( const String& rTxt, xub_StrLen nPos ) const;
};

extern SwBreakIt* pBreakIt;


} //namespace binfilter
#endif

