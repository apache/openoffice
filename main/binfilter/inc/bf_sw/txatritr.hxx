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


#ifndef _TXATRITR_HXX
#define _TXATRITR_HXX

#include <bf_svtools/bf_solar.h>
#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#include <tools/debug.hxx>
class String; 
namespace binfilter {

class SfxPoolItem;
class SwTxtNode;
class SwTxtAttr;



class SwScriptIterator
{

public:
    SwScriptIterator( const String& rStr, xub_StrLen nStart = 0,
                      sal_Bool bFrwrd = sal_True ){DBG_BF_ASSERT(0, "STRIP");};


};


class SwTxtAttrIterator
{
	const SfxPoolItem *pParaItem, *pCurItem;

public:
SwTxtAttrIterator( const SwTxtNode& rTxtNd, USHORT nWhichId,//STRIP001 	SwTxtAttrIterator( const SwTxtNode& rTxtNd, USHORT nWhichId,
xub_StrLen nStart = 0 ){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 						xub_StrLen nStart = 0 );
	const SfxPoolItem& GetAttr() const	{ return *pCurItem; }
};


#ifdef ITEMID_LANGUAGE

class SwLanguageIterator : public SwTxtAttrIterator
{
public:
	SwLanguageIterator( const SwTxtNode& rTxtNd, xub_StrLen nStart = 0 )
		: SwTxtAttrIterator( rTxtNd, RES_CHRATR_LANGUAGE, nStart )
	{}

	sal_uInt16 GetLanguage() const
		{ return ((SvxLanguageItem&)GetAttr()).GetValue(); }
};

#endif


} //namespace binfilter
#endif
