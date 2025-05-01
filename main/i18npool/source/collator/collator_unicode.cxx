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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_i18npool.hxx"

// generated list of languages
#include "lrl_include.hxx"

#include <rtl/ustrbuf.hxx>
#include <collator_unicode.hxx>
#include <localedata.hxx>
#include <com/sun/star/i18n/CollatorOptions.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;
using namespace ::rtl;

namespace com { namespace sun { namespace star { namespace i18n {

Collator_Unicode::Collator_Unicode()
{
	implementationName = "com.sun.star.i18n.Collator_Unicode";
	collator = NULL;
	uca_base = NULL;
    hModule = NULL;
}

Collator_Unicode::~Collator_Unicode()
{
	if (collator) ucol_close(collator);
	if (uca_base) ucol_close(uca_base);
    if (hModule) osl_unloadModule(hModule);
}

sal_Int32 SAL_CALL
Collator_Unicode::compareSubstring( const OUString& str1, sal_Int32 off1, sal_Int32 len1,
	const OUString& str2, sal_Int32 off2, sal_Int32 len2) throw(RuntimeException)
{
    return ucol_strcoll(collator, reinterpret_cast<const UChar *>(str1.getStr()) + off1, len1, reinterpret_cast<const UChar *>(str2.getStr()) + off2, len2);	// UChar != sal_Unicode in MinGW
}

sal_Int32 SAL_CALL
Collator_Unicode::compareString( const OUString& str1, const OUString& str2) throw(RuntimeException)
{
    return ucol_strcoll(collator, reinterpret_cast<const UChar *>(str1.getStr()), -1, reinterpret_cast<const UChar *>(str2.getStr()), -1);	// UChar != sal_Unicode in MinGW
}

extern "C" { static void SAL_CALL thisModule() {} }

sal_Int32 SAL_CALL
Collator_Unicode::loadCollatorAlgorithm(const OUString& rAlgorithm, const lang::Locale& rLocale, sal_Int32 options)
	throw(RuntimeException)
{
	if (!collator) {
        UErrorCode status = U_ZERO_ERROR;
        UParseError parseError;
        OUString rule = LocaleData().getCollatorRuleByAlgorithm(rLocale, rAlgorithm);
        if (rule.getLength() > 0) {
            collator = ucol_openRules(reinterpret_cast<const UChar *>(rule.getStr()), -1, UCOL_DEFAULT, UCOL_DEFAULT_STRENGTH, &parseError, &status);	// UChar != sal_Unicode in MinGW
			if (! U_SUCCESS(status)) throw RuntimeException();
		}
		if (!collator && OUString::createFromAscii(LOCAL_RULE_LANGS).indexOf(rLocale.Language) >= 0) {
			OUStringBuffer aBuf;
#ifdef SAL_DLLPREFIX
			aBuf.appendAscii(SAL_DLLPREFIX);
#endif
			aBuf.appendAscii( "collator_data" ).appendAscii( SAL_DLLEXTENSION );
			hModule = osl_loadModuleRelative( &thisModule, aBuf.makeStringAndClear().pData, SAL_LOADMODULE_DEFAULT );
			if (hModule) {
				const sal_uInt8* (*func)() = NULL;
				aBuf.appendAscii("get_").append(rLocale.Language).appendAscii("_");
				if (rLocale.Language.equalsAscii("zh")) {
					OUString func_base = aBuf.makeStringAndClear();
					if (OUString::createFromAscii("TW HK MO").indexOf(rLocale.Country) >= 0)
						func=(const sal_uInt8* (*)()) osl_getFunctionSymbol(hModule, 
									(func_base + OUString::createFromAscii("TW_") + rAlgorithm).pData);
					if (!func) 
						func=(const sal_uInt8* (*)()) osl_getFunctionSymbol(hModule, (func_base + rAlgorithm).pData);
				} else {
					if (rLocale.Language.equalsAscii("ja")) {
						// replace algrithm name to implementation name.
						if (rAlgorithm.equalsAscii("phonetic (alphanumeric first)") )
							aBuf.appendAscii("phonetic_alphanumeric_first");
						else if (rAlgorithm.equalsAscii("phonetic (alphanumeric last)"))
							aBuf.appendAscii("phonetic_alphanumeric_last");
						else
							aBuf.append(rAlgorithm);
					} else {
						aBuf.append(rAlgorithm);
					}
					func=(const sal_uInt8* (*)()) osl_getFunctionSymbol(hModule, aBuf.makeStringAndClear().pData);
				}
				if (func) {
					const sal_uInt8* ruleImage=func();
                    uca_base = ucol_open("root", &status);
					if (! U_SUCCESS(status)) throw RuntimeException();
					collator = ucol_openBinary(reinterpret_cast<const uint8_t*>(ruleImage), -1, uca_base, &status);
					if (! U_SUCCESS(status)) throw RuntimeException();
				}
			}
		}
		if (!collator) {
			/** ICU collators are loaded using a locale only.
				ICU uses Variant as collation algorithm name (like de__PHONEBOOK
				locale), note the empty territory (Country) designator in this special
				case here. The icu::Locale constructor changes the algorithm name to
				uppercase itself, so we don't have to bother with that.
			*/
            /** "The Locale constructor (in C++ and Java) taking multiple strings behaves exactly as if those strings
                 were concatenated, with the ‘_’ separator inserted between two adjacent non-empty strings, and
                 the result passed to uloc_getName." -- https://unicode-org.github.io/icu/userguide/locale/
            */
            OUStringBuffer locale;
            if (!rLocale.Language.isEmpty()) {
                locale.append(rLocale.Language);
                locale.appendAscii("_");
                if (!rLocale.Country.isEmpty()) {
                    locale.append(rLocale.Country);
                }
                if (!rAlgorithm.isEmpty()) {
                    locale.appendAscii("_");
                    locale.append(rAlgorithm);
                }
            }
            char icuLocale[1024];
            uloc_getName(OUStringToOString(locale.makeStringAndClear(), RTL_TEXTENCODING_ASCII_US).getStr(), icuLocale, sizeof(icuLocale), &status);
            if (! U_SUCCESS(status)) throw RuntimeException();
			// load ICU collator
            collator = ucol_open(icuLocale, &status);
			if (! U_SUCCESS(status)) throw RuntimeException();
		}
    }

	if (options & CollatorOptions::CollatorOptions_IGNORE_CASE_ACCENT)
        ucol_setStrength(collator, UCOL_PRIMARY);
	else if (options & CollatorOptions::CollatorOptions_IGNORE_CASE)
        ucol_setStrength(collator, UCOL_SECONDARY);
    else
        ucol_setStrength(collator, UCOL_TERTIARY);

	return(0);
}


OUString SAL_CALL
Collator_Unicode::getImplementationName() throw( RuntimeException )
{
    return OUString::createFromAscii(implementationName);
}

sal_Bool SAL_CALL
Collator_Unicode::supportsService(const rtl::OUString& rServiceName) throw( RuntimeException )
{
    return !rServiceName.compareToAscii(implementationName);
}

Sequence< OUString > SAL_CALL
Collator_Unicode::getSupportedServiceNames() throw( RuntimeException )
{
    Sequence< OUString > aRet(1);
    aRet[0] = OUString::createFromAscii(implementationName);
    return aRet;
}

} } } }
