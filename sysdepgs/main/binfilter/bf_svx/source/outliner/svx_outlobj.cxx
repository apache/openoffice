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




#ifdef _MSC_VER
#pragma hdrstop
#endif


#define _OUTLINER_CXX
#include <outlobj.hxx>
#include <outleeng.hxx>

#ifndef _EDITOBJ_HXX //autogen
#include <editobj.hxx>
#endif




namespace binfilter {

/*N*/ DBG_NAME(OutlinerParaObject)

/*N*/ OutlinerParaObject::OutlinerParaObject( USHORT nParaCount )
/*N*/ {
/*N*/ 	DBG_CTOR(OutlinerParaObject,0);
/*N*/ 
/*N*/ 	bIsEditDoc = TRUE;
/*N*/ 	pDepthArr = new USHORT[ nParaCount ];
/*N*/ 	nCount = (ULONG)nParaCount;
/*N*/ }

/*N*/ OutlinerParaObject::OutlinerParaObject( const OutlinerParaObject& rObj )
/*N*/ {
/*N*/ 	DBG_CTOR(OutlinerParaObject,0);
/*N*/ 
/*N*/ 	bIsEditDoc = rObj.bIsEditDoc;
/*N*/ 	nCount = rObj.nCount;
/*N*/ 	pDepthArr = new USHORT[ (USHORT)nCount ];
/*N*/ 	memcpy( pDepthArr, rObj.pDepthArr, (size_t)(sizeof(USHORT)*nCount) );
/*N*/ 	pText = rObj.pText->Clone();
/*N*/ }


/*N*/ OutlinerParaObject::~OutlinerParaObject()
/*N*/ {
/*N*/ 	DBG_DTOR(OutlinerParaObject,0);
/*N*/ 
/*N*/ 	delete pText;
/*N*/ 	delete[] pDepthArr;
/*N*/ }

/*N*/ void OutlinerParaObject::ClearPortionInfo()
/*N*/ {
/*N*/ 	DBG_CHKTHIS(OutlinerParaObject,0);
/*N*/ 	pText->ClearPortionInfo();
/*N*/ }

/*N*/ OutlinerParaObject* OutlinerParaObject::Clone() const
/*N*/ {
/*N*/ 	DBG_CHKTHIS(OutlinerParaObject,0);
/*N*/ 	OutlinerParaObject* pObj = new OutlinerParaObject(*this);
/*N*/ 	return pObj;
/*N*/ }

/*N*/ void OutlinerParaObject::ChangeStyleSheetName( SfxStyleFamily eFamily, const XubString& rOldName, const XubString& rNewName )
/*N*/ {
/*N*/ 	DBG_CHKTHIS(OutlinerParaObject,0);
/*N*/ 	pText->ChangeStyleSheetName( eFamily, rOldName, rNewName );
/*N*/ }

/*N*/ BOOL OutlinerParaObject::ChangeStyleSheets( const XubString& rOldName, SfxStyleFamily eOldFamily, const XubString& rNewName, SfxStyleFamily eNewFamily )
/*N*/ {
/*N*/ 	DBG_CHKTHIS(OutlinerParaObject,0);
/*N*/ 	return pText->ChangeStyleSheets( rOldName, eOldFamily, rNewName, eNewFamily );
/*N*/ }

/*N*/ void OutlinerParaObject::Store(SvStream& rStream ) const
/*N*/ {
/*N*/ 	rStream << nCount;
/*N*/ 	rStream << (sal_uInt32) 0x42345678;
/*N*/ 	pText->Store( rStream );
/*N*/ 
/*N*/ 	for( USHORT nPos=0; nPos < nCount; nPos++ )
/*N*/ 		rStream << pDepthArr[ nPos ];
/*N*/ 
/*N*/ 	rStream << bIsEditDoc;
/*N*/ }

/*N*/ OutlinerParaObject* OutlinerParaObject::Create( SvStream& rStream, SfxItemPool* pTextObjectPool )
/*N*/ {
/*N*/ 	OutlinerParaObject* pPObj = NULL;
/*N*/ 	USHORT nVersion = 0;
/*N*/ 
/*N*/ 	sal_uInt32 nCount;
/*N*/ 	rStream >> nCount;
/*N*/ 
/*N*/ 	sal_uInt32 nSyncRef;
/*N*/ 	rStream >> nSyncRef;
/*N*/ 	if( nSyncRef == 0x12345678 )
/*N*/ 		nVersion = 1;
/*N*/ 	else if( nSyncRef == 0x22345678 )
/*N*/ 		nVersion = 2;
/*N*/ 	else if( nSyncRef == 0x32345678 )
/*N*/ 		nVersion = 3;
/*N*/ 	else if ( nSyncRef == 0x42345678 )
/*N*/ 		nVersion = 4;
/*N*/ 
/*N*/ 	if ( nVersion )
/*N*/ 	{
/*N*/ 		pPObj = new OutlinerParaObject( (USHORT)nCount );
/*N*/ 		if( nVersion <= 3 )
/*N*/ 		{
/*N*/ 			EditTextObject* pAllText = 0;
/*N*/ 			USHORT nCurPara = 0;
/*N*/ 			while ( nCount )
/*N*/ 			{
/*N*/ 				EditTextObject* pText = EditTextObject::Create( rStream, NULL );
/*N*/ 				DBG_ASSERT(pText,"CreateEditTextObject failed");
/*N*/ 				sal_uInt32 nSync = 0;
/*N*/ 				rStream >> nSync;
/*N*/ 				DBG_ASSERT(nSync==nSyncRef,"Stream out of sync");
/*N*/ 				USHORT nDepth;
/*N*/ 				rStream >> nDepth;
/*N*/ 				Paragraph* pPara = new Paragraph( nDepth );
/*N*/ 				if( nVersion == 1 )
/*N*/ 				{
/*?*/ 					// Bullet ueberlesen
/*?*/ 					USHORT nFlags;
/*?*/ 					rStream >> nFlags;
/*?*/ 
/*?*/ 					if ( nFlags & 0x0001 )	// Bitmap
/*?*/ 					{
/*?*/ 						Bitmap aBmp;
/*?*/ 						rStream >> aBmp;
/*?*/ 					}
/*?*/ 					else
/*?*/ 					{
/*?*/ 						Color aColor;
/*?*/ 						rStream >> aColor;
/*?*/ 						rStream.SeekRel( 16 );
/*?*/ 						String aName;
/*?*/ 						rStream.ReadByteString(aName); 
/*?*/ 						rStream.SeekRel( 12 );
/*?*/ 					}
/*?*/ 					long nDummy;
/*?*/ 					rStream >> nDummy;
/*?*/ 					rStream >> nDummy;
/*N*/ 				}
/*N*/ 				pPara->bVisible = TRUE;
/*N*/ 				if( !pAllText )
/*N*/ 					pAllText = pText;
/*N*/ 				else
/*N*/ 				{
/*N*/ 					pAllText->Insert( *pText, 0xffff );
/*N*/ 					delete pText;
/*N*/ 				}
/*N*/ 				pPObj->pDepthArr[ nCurPara ] = pPara->GetDepth();
/*N*/ 				delete pPara;
/*N*/ 				nCount--;
/*N*/ 				nCurPara++;
/*N*/ 				if( nCount )
/*N*/ 				{
/*N*/ 					sal_uInt32 nSync = 0;
/*N*/ 					rStream >> nSync;
/*N*/ 					DBG_ASSERT(nSync==nSyncRef,"Stream out of sync");
/*N*/ 				}
/*N*/ 			}
/*N*/ 			if( nVersion == 3 )
/*N*/ 				rStream >> pPObj->bIsEditDoc;
/*N*/ 			pPObj->pText = pAllText;
/*N*/ 		}
/*N*/ 		else // nVersion >= 4
/*N*/ 		{
/*N*/ 			pPObj->pText = EditTextObject::Create( rStream, pTextObjectPool );
/*N*/ 			for( USHORT nCur=0; nCur < nCount; nCur++ )
/*N*/ 				rStream >> pPObj->pDepthArr[ nCur ];
/*N*/ 			rStream >> pPObj->bIsEditDoc;
/*N*/ 
/*N*/ 			if ( pPObj->pText->GetVersion() < 501 )
/*N*/ 				pPObj->pText->AdjustImportedLRSpaceItems( pPObj->bIsEditDoc );
/*N*/ 
/*N*/ 			// MT: Bei der naechsten Version mal eine Recordlaenge einfuehren!
/*N*/ 		}
/*N*/ 	}
/*N*/ 	return pPObj;
/*N*/ }

/*N*/ USHORT OutlinerParaObject::GetOutlinerMode() const
/*N*/ {
/*N*/ 	return pText->GetUserType();
/*N*/ }

/*N*/ void OutlinerParaObject::SetOutlinerMode( USHORT n )
/*N*/ {
/*N*/ 	pText->SetUserType( n );
/*N*/ }




/*N*/ void OutlinerParaObject::MergeParaAttribs( const SfxItemSet& rAttribs, USHORT nStart, USHORT nEnd )
/*N*/ {
/*N*/ 	pText->MergeParaAttribs( rAttribs, nStart, nEnd );
/*N*/ }

/*N*/ void OutlinerParaObject::PrepareStore( SfxStyleSheetPool* pStyleSheetPool )
/*N*/ {
/*N*/ 	pText->PrepareStore( pStyleSheetPool );
/*N*/ }

/*N*/ void OutlinerParaObject::FinishStore()
/*N*/ {
/*N*/ 	pText->FinishStore();
/*N*/ }

/*N*/ void OutlinerParaObject::FinishLoad( SfxStyleSheetPool* pStyleSheetPool )
/*N*/ {
/*N*/ 	pText->FinishLoad( pStyleSheetPool );
/*N*/ }

/*N*/ void OutlinerParaObject::SetVertical( BOOL bVertical )
/*N*/ {
/*N*/ 	pText->SetVertical( bVertical );
/*N*/ }

/*N*/ BOOL OutlinerParaObject::IsVertical() const
/*N*/ {
/*N*/ 	return pText->IsVertical();
/*N*/ }

}
