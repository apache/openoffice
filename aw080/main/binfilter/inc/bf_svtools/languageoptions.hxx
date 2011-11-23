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


#ifndef _SVTOOLS_LANGUAGEOPTIONS_HXX
#define _SVTOOLS_LANGUAGEOPTIONS_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#ifndef _SFXBRDCST_HXX
#include <bf_svtools/brdcst.hxx>
#endif
#ifndef _SFXLSTNER_HXX
#include <bf_svtools/lstner.hxx>
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
#ifndef INCLUDED_I18NPOOL_LANG_H
#include <i18npool/lang.h>
#endif

namespace binfilter
{

// class SvtLanguageOptions ----------------------------------------------------

// these defines can be ored
#define SCRIPTTYPE_LATIN		0x01
#define SCRIPTTYPE_ASIAN		0x02
#define SCRIPTTYPE_COMPLEX		0x04

class SvtCJKOptions;
class SvtCTLOptions;

class  SvtLanguageOptions : public SfxBroadcaster, public SfxListener
{
private:
    SvtCJKOptions*	m_pCJKOptions;
    SvtCTLOptions*	m_pCTLOptions;

public:
    enum EOption
    {
        // cjk options
        E_CJKFONT,
        E_VERTICALTEXT,
        E_ASIANTYPOGRAPHY,
        E_JAPANESEFIND,
        E_RUBY,
        E_CHANGECASEMAP,
        E_DOUBLELINES,
        E_EMPHASISMARKS,
        E_VERTICALCALLOUT,
        E_ALLCJK,
        // ctl options
        E_CTLFONT,
        E_CTLSEQUENCECHECKING,
        E_CTLCURSORMOVEMENT,
        E_CTLTEXTNUMERALS
    };

    // returns for a language the scripttype
    static sal_uInt16 GetScriptTypeOfLanguage( sal_uInt16 nLang );
};
/** #i42730# Gives access to the Windows 16bit system locale
 */
class  SvtSystemLanguageOptions : public utl::ConfigItem
{
private:
    ::rtl::OUString m_sWin16SystemLocale;

public:
    SvtSystemLanguageOptions();
    ~SvtSystemLanguageOptions();

    virtual void    Commit();
	void Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames );

    LanguageType GetWin16SystemLanguage();
};

}

#endif // _SVTOOLS_LANGUAGEOPTIONS_HXX

