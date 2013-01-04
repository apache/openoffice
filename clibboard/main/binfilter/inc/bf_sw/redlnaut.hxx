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


#ifndef _REDLNAUT_HXX
#define _REDLNAUT_HXX

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _COLOR_HXX //autogen
#include <tools/color.hxx>
#endif

#include "swtypes.hxx"
namespace binfilter {
class SfxItemSet; 



class SwRedlineAuthor
{
	String sAuthor;
	Color aChgLineColor;
	SfxItemSet *pInsAttrSet, *pDelAttrSet, *pFmtAttrSet;
	SwHoriOrient eChgLineOrient;
	BYTE cDelChar;
public:
	SwRedlineAuthor( SwAttrPool& rPool, const String& );
	SwRedlineAuthor( const SwRedlineAuthor& );
	~SwRedlineAuthor();

	SwRedlineAuthor& operator=( const SwRedlineAuthor& );

	const String& GetAuthor() const { return sAuthor; }

	SfxItemSet& GetInsAttrSet() const { return *pInsAttrSet; }
	SfxItemSet& GetDelAttrSet() const { return *pDelAttrSet; }
	SfxItemSet& GetFmtAttrSet() const { return *pFmtAttrSet; }

	const Color& GetChgLineColor() const { return aChgLineColor; }
	void SetChgLineColor( const Color& rCol ) { aChgLineColor = rCol; }

	SwHoriOrient GetChgLineOrient() const { return eChgLineOrient; }
	void SetChgLineOrient( SwHoriOrient eVal ) { eChgLineOrient = eVal; }

	BYTE GetDelChar() const { return cDelChar; }
	void SetDelChar( BYTE cCh = 0 ) { cDelChar = cCh; }
};


} //namespace binfilter
#endif
