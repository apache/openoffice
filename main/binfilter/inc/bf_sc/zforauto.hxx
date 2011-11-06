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



#ifndef _ZFORAUTO_HXX_
#define _ZFORAUTO_HXX_

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif

#ifndef INCLUDED_I18NPOOL_LANG_H
#include <i18npool/lang.h>
#endif
namespace binfilter {




class ScNumFormatAbbrev
{
	String sFormatstring;
	LanguageType eLnge;
	LanguageType eSysLnge;
public:
	ScNumFormatAbbrev();
	ScNumFormatAbbrev(const ScNumFormatAbbrev& aFormat);
	ScNumFormatAbbrev(ULONG nFormat, SvNumberFormatter& rFormatter) {DBG_BF_ASSERT(0, "STRIP"); } //STRIP001 	ScNumFormatAbbrev(ULONG nFormat, SvNumberFormatter& rFormatter);
	void Load( SvStream& rStream );			// Laden der Zahlenformate
	void Save( SvStream& rStream ) const;	// Speichern der Zahlenformate
};



} //namespace binfilter
#endif


