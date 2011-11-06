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


#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#define INCLUDED_SVTOOLS_PATHOPTIONS_HXX

#include <tools/string.hxx>
#include <com/sun/star/lang/Locale.hpp>
#include <bf_svtools/options.hxx>

#define SVT_SEARCHPATH_DELIMITER      ';'

namespace binfilter
{

// class SvtPathOptions --------------------------------------------------

class SvtPathOptions_Impl;
class  SvtPathOptions: public Options
{
private:
	SvtPathOptions_Impl*	pImp;

public:
	enum Pathes
	{
		PATH_ADDIN,
		PATH_AUTOCORRECT,
		PATH_AUTOTEXT,
		PATH_BACKUP,
		PATH_BASIC,
		PATH_BITMAP,
		PATH_CONFIG,
		PATH_DICTIONARY,
		PATH_FAVORITES,
		PATH_FILTER,
		PATH_GALLERY,
		PATH_GRAPHIC,
		PATH_HELP,
		PATH_LINGUISTIC,
		PATH_MODULE,
		PATH_PALETTE,
		PATH_PLUGIN,
		PATH_STORAGE,
		PATH_TEMP,
		PATH_TEMPLATE,
		PATH_USERCONFIG,
		PATH_USERDICTIONARY,
        PATH_WORK,
        PATH_UICONFIG,
        PATH_FINGERPRINT,
		PATH_COUNT // should always be the last element
	};

	SvtPathOptions();
	virtual ~SvtPathOptions();

	// get the pathes, not const because of using a mutex
	const String&	GetAddinPath() const;
	const String&	GetAutoCorrectPath() const;
	const String&	GetAutoTextPath() const;
	const String&	GetBackupPath() const;
	const String&	GetBasicPath() const;
	const String&	GetBitmapPath() const;
	const String&	GetConfigPath() const;
	const String&	GetDictionaryPath() const;
	const String&	GetFavoritesPath() const;
	const String&	GetFilterPath() const;
	const String&	GetGalleryPath() const;
	const String&	GetGraphicPath() const;
	const String&	GetHelpPath() const;
	const String&	GetLinguisticPath() const;
	const String&	GetModulePath() const;
	const String&	GetPalettePath() const;
	const String&	GetPluginPath() const;
	const String&	GetStoragePath() const;
	const String&	GetTempPath() const;
	const String&	GetTemplatePath() const;
	const String&	GetUserConfigPath() const;
	const String&	GetUserDictionaryPath() const;
	const String&	GetWorkPath() const;
    const String&   GetUIConfigPath() const;
    const String&   GetFingerprintPath() const;

	// set the pathes
	void			SetBasicPath( const String& rPath );

	String			SubstituteVariable( const String& rVar );
	String			UseVariable( const String& rVar );
	sal_Bool		SearchFile( String& rIniFile, Pathes ePath = PATH_USERCONFIG );
    sal_Bool        IsReadonly() const;
};

}

#endif // #ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX

