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



#ifndef _OUTLOBJ_HXX
#define _OUTLOBJ_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

#ifndef _RSCSFX_HXX
#include <rsc/rscsfx.hxx>
#endif

#ifndef _SFXITEMSET_HXX
#include <bf_svtools/itemset.hxx>
#endif

#include <bf_svx/eeitem.hxx>
class SvStream;
namespace binfilter {

class SfxItemPool;
class SfxStyleSheetPool; 
class Outliner;
class EditTextObject;

class OutlinerParaObject
{
	friend class Outliner;

	EditTextObject* 			pText;
	USHORT* 					pDepthArr;
	sal_uInt32					nCount;
	BOOL						bIsEditDoc;
								OutlinerParaObject( USHORT nParaCount );

public:
								OutlinerParaObject( const OutlinerParaObject& );
								~OutlinerParaObject();

	OutlinerParaObject* 		Clone() const;

	USHORT						GetOutlinerMode() const;
	void						SetOutlinerMode( USHORT n );

	BOOL						IsVertical() const;
	void						SetVertical( BOOL bVertical );

	sal_uInt32					Count() const 					{ return nCount; }
	USHORT 						GetDepth( USHORT nPara ) const	{ return pDepthArr[nPara]; }
	const EditTextObject& 		GetTextObject() const 			{ return *pText; }
	void						ClearPortionInfo();
	BOOL						IsEditDoc() const 				{ return bIsEditDoc; }
	void 						Store( SvStream& ) const;
	static OutlinerParaObject*	Create( SvStream&, SfxItemPool* pTextObjectPool = 0 );

	BOOL						ChangeStyleSheets( 	const XubString& rOldName, SfxStyleFamily eOldFamily,
													const XubString& rNewName, SfxStyleFamily eNewFamily );
	void						ChangeStyleSheetName( SfxStyleFamily eFamily, const XubString& rOldName, const XubString& rNewName );

	void						MergeParaAttribs( const SfxItemSet& rAttribs, USHORT nStart = EE_CHAR_START, USHORT nEnd = EE_CHAR_END );

	void 						PrepareStore( SfxStyleSheetPool* pStyleSheetPool );
	void 						FinishStore();
	void 						FinishLoad( SfxStyleSheetPool* pStyleSheetPool );
};

}//end of namespace binfilter
#endif
