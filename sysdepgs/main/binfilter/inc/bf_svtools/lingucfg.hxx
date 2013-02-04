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



#ifndef _SVTOOLS_LINGUCFG_HXX_
#define _SVTOOLS_LINGUCFG_HXX_

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUES_HPP_
#include <com/sun/star/beans/PropertyValues.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_ANY_H_
#include <com/sun/star/uno/Any.h>
#endif

#include <bf_svtools/bf_solar.h>
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
#ifndef _OSL_MUTEX_HXX_ 
#include <osl/mutex.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_OPTIONS_HXX
#include <bf_svtools/options.hxx>
#endif

namespace binfilter
{

class SvtLinguConfigItem;

//////////////////////////////////////////////////////////////////////

struct  SvtLinguOptions
{
	::com::sun::star::uno::Sequence< rtl::OUString >	aActiveDics;
    ::com::sun::star::uno::Sequence< rtl::OUString >    aActiveConvDics;

    BOOL                                                bROActiveDics;
    BOOL                                                bROActiveConvDics;

	// Hyphenator service specific options
	INT16	nHyphMinLeading,
			nHyphMinTrailing,
			nHyphMinWordLength;

    BOOL    bROHyphMinLeading,
            bROHyphMinTrailing,
            bROHyphMinWordLength;

	// misc options (non-service specific)
	INT16	nDefaultLanguage;
	INT16	nDefaultLanguage_CJK;
	INT16	nDefaultLanguage_CTL;

    BOOL    bRODefaultLanguage;
    BOOL    bRODefaultLanguage_CJK;
    BOOL    bRODefaultLanguage_CTL;

	// spelling options (non-service specific)
	BOOL	bIsSpellSpecial;
	BOOL	bIsSpellAuto;
	BOOL	bIsSpellReverse;

    BOOL    bROIsSpellSpecial;
    BOOL    bROIsSpellAuto;
    BOOL    bROIsSpellReverse;

	// hyphenation options (non-service specific)
	BOOL	bIsHyphSpecial;
	BOOL	bIsHyphAuto;

    BOOL    bROIsHyphSpecial;
    BOOL    bROIsHyphAuto;

	// common to SpellChecker, Hyphenator and Thesaurus service
	BOOL	bIsUseDictionaryList;
	BOOL	bIsIgnoreControlCharacters;

    BOOL    bROIsUseDictionaryList;
    BOOL    bROIsIgnoreControlCharacters;

	// SpellChecker service specific options
	BOOL	bIsSpellWithDigits,
			bIsSpellUpperCase,
			bIsSpellCapitalization;

    BOOL    bROIsSpellWithDigits,
            bROIsSpellUpperCase,
            bROIsSpellCapitalization;

    // text conversion specific options
    BOOL    bIsIgnorePostPositionalWord;
    BOOL    bIsAutoCloseDialog;
    BOOL    bIsShowEntriesRecentlyUsedFirst;
    BOOL    bIsAutoReplaceUniqueEntries;
    BOOL    bIsDirectionToSimplified;
    BOOL    bIsUseCharacterVariants;
    BOOL    bIsTranslateCommonTerms;
    BOOL    bIsReverseMapping;

    BOOL    bROIsIgnorePostPositionalWord;
    BOOL    bROIsAutoCloseDialog;
    BOOL    bROIsShowEntriesRecentlyUsedFirst;
    BOOL    bROIsAutoReplaceUniqueEntries;
    BOOL    bROIsDirectionToSimplified;
    BOOL    bROIsUseCharacterVariants;
    BOOL    bROIsTranslateCommonTerms;
    BOOL    bROIsReverseMapping;

    // check value need to determine if the configuration needs to updatet
    // or not (used for a quick check if data files have been changed/added
    // or deleted
    INT32   nDataFilesChangedCheckValue;
    BOOL    bRODataFilesChangedCheckValue;

	SvtLinguOptions();
};

//////////////////////////////////////////////////////////////////////

class  SvtLinguConfig: public Options
{

	// disallow copy-constructor and assignment-operator for now
	SvtLinguConfig( const SvtLinguConfig & );
	SvtLinguConfig & operator = ( const SvtLinguConfig & );

    // returns static object
     SvtLinguConfigItem &   GetConfigItem();

    SvtLinguConfigItem &   GetConfigItem() const    { return const_cast< SvtLinguConfig * >( this )->GetConfigItem(); }

public:
    SvtLinguConfig();
    virtual ~SvtLinguConfig();

    //
    // borrowed from utl::ConfigItem
    //
    com::sun::star::uno::Sequence< rtl::OUString >
        GetNodeNames( const rtl::OUString &rNode );

    BOOL    GetOptions( SvtLinguOptions &rOptions ) const;
};

//////////////////////////////////////////////////////////////////////

}

#endif

