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



#ifndef _OUTLEENG_HXX
#define _OUTLEENG_HXX

#ifndef _OUTLINER_HXX
#include <outliner.hxx>
#endif
#ifndef _EDITENG_HXX
#include <editeng.hxx>
#endif
namespace binfilter {

typedef EENotify* EENotifyPtr;
SV_DECL_PTRARR_DEL( NotifyList, EENotifyPtr, 1, 1 )//STRIP008 ;

class OutlinerEditEng : public EditEngine
{
	Outliner* 			pOwner;

public:
						OutlinerEditEng( Outliner* pOwner, SfxItemPool* pPool );
						~OutlinerEditEng();


	virtual void 		ParagraphInserted( USHORT nNewParagraph );
	virtual void 		ParagraphDeleted( USHORT nDeletedParagraph );
	
	// #101498#
	virtual void 		DrawingText(const Point& rStartPos, const XubString& rText, USHORT nTextStart, USHORT nTextLen, const sal_Int32* pDXArray, const SvxFont& rFont, USHORT nPara, USHORT nIndex, BYTE nRightToLeft);

	virtual void 		ParaAttribsChanged( USHORT nPara );
	virtual void 		ParagraphHeightChanged( USHORT nPara );
	virtual XubString	GetUndoComment( USHORT nUndoId ) const;

	virtual XubString	CalcFieldValue( const SvxFieldItem& rField, USHORT nPara, USHORT nPos, Color*& rTxtColor, Color*& rFldColor );

	virtual Rectangle 	GetBulletArea( USHORT nPara );

    // belongs into class Outliner, move there before incompatible update!
    Link                aOutlinerNotifyHdl; 
    NotifyList          aNotifyCache;
};

}//end of namespace binfilter
#endif

