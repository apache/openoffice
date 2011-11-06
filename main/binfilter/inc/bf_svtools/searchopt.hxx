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



#ifndef _SVT_SEARCHOPT_HXX_
#define _SVT_SEARCHOPT_HXX_

#include <bf_svtools/bf_solar.h>

namespace binfilter
{

class SvtSearchOptions_Impl;

class  SvtSearchOptions
{
	SvtSearchOptions_Impl	*pImpl;

	// disallow copy-constructor and assignment-operator for now
	SvtSearchOptions( const SvtSearchOptions & );
	SvtSearchOptions & operator = ( const SvtSearchOptions & );

protected:

public:
	SvtSearchOptions();
	~SvtSearchOptions();

	//
	// General Options
	//

	BOOL	IsWholeWordsOnly() const;
	BOOL	IsBackwards() const;
	BOOL	IsUseRegularExpression() const;
	BOOL	IsSimilaritySearch() const;
	BOOL	IsUseAsianOptions() const;
	BOOL	IsMatchCase() const;			// also Japanese search option

	//
	// Japanese search options
	//

	BOOL	IsMatchFullHalfWidthForms() const;
	BOOL	IsMatchHiraganaKatakana() const;
	BOOL	IsMatchContractions() const;
	BOOL	IsMatchMinusDashChoon() const;
	BOOL	IsMatchRepeatCharMarks() const;
	BOOL	IsMatchVariantFormKanji() const;
	BOOL	IsMatchOldKanaForms() const;
	BOOL	IsMatchDiziDuzu() const;
	BOOL	IsMatchBavaHafa() const;
	BOOL	IsMatchTsithichiDhizi() const;
	BOOL	IsMatchHyuiyuByuvyu() const;
	BOOL	IsMatchSesheZeje() const;
	BOOL	IsMatchIaiya() const;
	BOOL	IsMatchKiku() const;
	BOOL	IsIgnorePunctuation() const;
	BOOL	IsIgnoreWhitespace() const;
	BOOL	IsIgnoreProlongedSoundMark() const;
	BOOL	IsIgnoreMiddleDot() const;
};

}

#endif

