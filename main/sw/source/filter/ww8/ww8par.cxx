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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil -*- */

#include <com/sun/star/embed/ElementModes.hpp>
#include <com/sun/star/embed/XStorage.hpp>

#include <hash_set>
#include <unotools/ucbstreamhelper.hxx>
#include <tools/solar.h>
#include <rtl/tencinfo.h>
#include <rtl/random.h>

#include <sot/storage.hxx>
#include <sfx2/docinf.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/request.hxx>
#include <sfx2/frame.hxx>
#include <tools/urlobj.hxx>
#include <unotools/tempfile.hxx>
#include <svtools/sfxecode.hxx>

#include <comphelper/docpasswordrequest.hxx>
#include <hintids.hxx>

#include <editeng/tstpitem.hxx>
#include <editeng/cscoitem.hxx>
#include <svx/svdobj.hxx>
#include <svx/svdpage.hxx>
#include <editeng/paperinf.hxx>
#include <editeng/lrspitem.hxx> // SvxLRSpaceItem
#include <editeng/ulspitem.hxx>
#include <editeng/langitem.hxx>
#include <editeng/opaqitem.hxx>
#include <editeng/charhiddenitem.hxx>
#include <filter/msfilter/svxmsbas.hxx>
#include <svx/unoapi.hxx>
#include <svx/svdoole2.hxx>
#include <filter/msfilter/msdffimp.hxx>
#include <svx/svdoashp.hxx>
#include <svx/svxerr.hxx>
#include <filter/msfilter/mscodec.hxx>
#include <svx/svdmodel.hxx>
#include <svx/svdogrp.hxx>
#include <svx/xflclit.hxx>

#include <unotools/fltrcfg.hxx>
#include <fmtfld.hxx>
#include <fmturl.hxx>
#include <fmtinfmt.hxx>
#include <IMark.hxx>
#include <reffld.hxx>
#include <fmthdft.hxx>
#include <fmtcntnt.hxx>
#include <fmtcnct.hxx>
#include <fmtanchr.hxx>
#include <fmtpdsc.hxx>
#include <ftninfo.hxx>
#include <fmtftn.hxx>
#include <txtftn.hxx>
#include <pam.hxx>              // fuer SwPam
#include <doc.hxx>
#include <ndtxt.hxx>            // class SwTxtNode
#include <pagedesc.hxx>         // class SwPageDesc
#include <paratr.hxx>
#include <fmtclds.hxx>
#include <fmtclbl.hxx>
#include <section.hxx>
#include <docsh.hxx>
#include <docufld.hxx>
#include <swfltopt.hxx>
#include <viewsh.hxx>
#include <shellres.hxx>
#include <mdiexp.hxx>           // Progress
#include <statstr.hrc>          // ResId fuer Statusleiste
#include <swerror.h>            // ERR_WW8_...
#include <swunodef.hxx>
#include <unodraw.hxx>
#include <swtable.hxx>          // class SwTableLines, ...
// #i18732#
#include <fmtfollowtextflow.hxx>
#include <fchrfmt.hxx>
#include <charfmt.hxx>


#include <com/sun/star/i18n/ForbiddenCharacters.hpp>
#include <comphelper/extract.hxx>
#include <comphelper/sequenceashashmap.hxx>
#include <fltini.hxx>

#include <algorithm>
#include <functional>
#include "writerhelper.hxx"
#include "writerwordglue.hxx"

#include "ndgrf.hxx"
#include <editeng/brshitem.hxx>
#include <editeng/editids.hrc>
#include <txtflcnt.hxx>
#include <fmtflcnt.hxx>
#include <txatbase.hxx>

#include "ww8par2.hxx"          // class WW8RStyle, class WW8AnchorPara

#include <frmatr.hxx>

#include <math.h>
#include <com/sun/star/beans/XPropertyContainer.hpp>

#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/document/XDocumentPropertiesSupplier.hpp>
#include <com/sun/star/document/XDocumentProperties.hpp>
#include <svl/itemiter.hxx>  //SfxItemIter

#include <stdio.h>

#ifdef DEBUG
#include <iostream>
#include <dbgoutsw.hxx>
#endif
#include <svx/hlnkitem.hxx>
#include "WW8Sttbf.hxx"
#include "WW8FibData.hxx"
#define MM_250 1417             // WW-Default fuer Hor. Seitenraender: 2.5 cm
#define MM_200 1134             // WW-Default fuer u.Seitenrand: 2.0 cm

using namespace ::com::sun::star;
using namespace sw::util;
using namespace sw::types;
using namespace nsHdFtFlags;

#define VT_EMPTY			0
#define VT_I4				3
#define VT_LPSTR			30
#define VT_LPWSTR			31
#define VT_BLOB 			65
#define VT_TYPEMASK 		0xFFF
/** Expands to a pointer behind the last element of a STATIC data array (like STL end()). */
#define STATIC_TABLE_END( array )   ((array)+STATIC_TABLE_SIZE(array))
/** Expands to the size of a STATIC data array. */
#define STATIC_TABLE_SIZE( array )  (sizeof(array)/sizeof(*(array)))

SwMacroInfo* GetMacroInfo( SdrObject* pObj, sal_Bool bCreate )             // static
{
	if ( pObj )
	{
		sal_uInt16 nCount = pObj->GetUserDataCount();
	    	for( sal_uInt16 i = 0; i < nCount; i++ )
	    	{
	       	SdrObjUserData* pData = pObj->GetUserData( i );
	        	if( pData && pData->GetInventor() == SW_DRAWLAYER
	              	&& pData->GetId() == SW_UD_IMAPDATA)
	            		return dynamic_cast<SwMacroInfo*>(pData);
	    	}
	    	if ( bCreate )
	    	{
	       	SwMacroInfo* pData = new SwMacroInfo;
	        	pObj->InsertUserData( pData, 0 );
	        	return pData;
	    	}
	}
	
    	return 0;
};

void lclGetAbsPath( String& rPath, sal_uInt16 nLevel, SwDocShell* pDocShell)
{
	String aTmpStr;
    	while( nLevel )
    	{
       	aTmpStr.AppendAscii( "../" );
        	--nLevel;
    	}
	if(aTmpStr.Len())
		aTmpStr += rPath;
	else
		aTmpStr = rPath;

	if(aTmpStr.Len())
	{
       	bool bWasAbs = false;
        	rPath = pDocShell->GetMedium()->GetURLObject().smartRel2Abs( aTmpStr, bWasAbs ).GetMainURL( INetURLObject::NO_DECODE );
        	// full path as stored in SvxURLField must be encoded
	} 
}

void lclIgnoreString32( SvMemoryStream& rStrm, bool b16Bit )
{
 	sal_uInt32 nChars;
    	rStrm >> nChars;
    	if( b16Bit )
       	nChars *= 2;
    	rStrm.SeekRel( nChars );
}

String SwWW8ImplReader::ReadRawUniString( SvMemoryStream& rStrm,sal_uInt16 nChars, bool b16Bit )
{
	// Fixed-size characters
	const sal_uInt8 WW8_NUL_C                   = '\x00';       /// NUL character.
	const sal_uInt16 WW8_NUL                    = WW8_NUL_C;    /// NUL character (unicode).
	String aRet;
	sal_Unicode         mcNulSubst = '\0';

	sal_uInt16 nCharsLeft = nChars;
	sal_Unicode* pcBuffer = new sal_Unicode[ nCharsLeft + 1 ];

	sal_Unicode* pcUniChar = pcBuffer;
       sal_Unicode* pcEndChar = pcBuffer + nCharsLeft;

       if( b16Bit )
       {
       	sal_uInt16 nReadChar;
            	for( ;  (pcUniChar < pcEndChar); ++pcUniChar )
            	{
              	rStrm >> (nReadChar);
                	(*pcUniChar) = (nReadChar == WW8_NUL) ? mcNulSubst : static_cast< sal_Unicode >( nReadChar );
            	}
        }
        else
        {
        	sal_uInt8 nReadChar;
            	for( ; (pcUniChar < pcEndChar); ++pcUniChar )
            	{
              	rStrm >> nReadChar ;
                	(*pcUniChar) = (nReadChar == WW8_NUL_C) ? mcNulSubst : static_cast< sal_Unicode >( nReadChar );
            	}
        }

       *pcEndChar = '\0';
       aRet.Append( pcBuffer );
	delete[] pcBuffer;
	return aRet;
}

void lclAppendString32( String& rString, SvMemoryStream& rStrm, sal_uInt32 nChars, bool b16Bit )
{
	sal_uInt16 nReadChars = ulimit_cast< sal_uInt16 >( nChars );
	String urlStr = SwWW8ImplReader::ReadRawUniString( rStrm, nReadChars, b16Bit );
    	rString.Append( urlStr );
}

void lclAppendString32( String& rString, SvMemoryStream& rStrm, bool b16Bit )
{
	sal_uInt32 nValue;
    	rStrm >>( nValue );
    	lclAppendString32( rString, rStrm, nValue, b16Bit );
}

void SwWW8ImplReader::ReadEmbeddedData( SvMemoryStream& rStrm, SwDocShell* pDocShell, struct HyperLinksTable& hlStr)
{
	// (0x01B8) HLINK -------------------------------------------------------------
	// const sal_uInt16 WW8_ID_HLINK               = 0x01B8;
	const sal_uInt32 WW8_HLINK_BODY             = 0x00000001;   /// Contains file link or URL.
	const sal_uInt32 WW8_HLINK_ABS              = 0x00000002;   /// Absolute path.
	const sal_uInt32 WW8_HLINK_DESCR            = 0x00000014;   /// Description.
	const sal_uInt32 WW8_HLINK_MARK             = 0x00000008;   /// Text mark.
	const sal_uInt32 WW8_HLINK_FRAME            = 0x00000080;   /// Target frame.
	const sal_uInt32 WW8_HLINK_UNC              = 0x00000100;   /// UNC path.

	//	sal_uInt8 maGuidStdLink[ 16 ] ={
	//		0xD0, 0xC9, 0xEA, 0x79, 0xF9, 0xBA, 0xCE, 0x11, 0x8C, 0x82, 0x00, 0xAA, 0x00, 0x4B, 0xA9, 0x0B };

	sal_uInt8 maGuidUrlMoniker[ 16 ] = {
	    0xE0, 0xC9, 0xEA, 0x79, 0xF9, 0xBA, 0xCE, 0x11, 0x8C, 0x82, 0x00, 0xAA, 0x00, 0x4B, 0xA9, 0x0B };

	sal_uInt8 maGuidFileMoniker[ 16 ] = {
		0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 };
 
	sal_uInt8*	aGuid = new sal_uInt8[ 16 ];
	sal_uInt32 nFlags;


    	rStrm.Read( aGuid, 16);
    	rStrm.SeekRel( 4 );
    	rStrm >> nFlags;

    	sal_uInt16 nLevel = 0;                  // counter for level to climb down in path
    	::std::auto_ptr< String > xLongName;    // link / file name
    	::std::auto_ptr< String > xShortName;   // 8.3-representation of file name
    	::std::auto_ptr< String > xTextMark;    // text mark

   	// description (ignore)
    	if( ::get_flag( nFlags, WW8_HLINK_DESCR ) )
       	lclIgnoreString32( rStrm, true );

   	// target frame
    	if( ::get_flag( nFlags, WW8_HLINK_FRAME ) )
	{       
		::std::auto_ptr< String > FrmName;
		FrmName.reset( new String );
		lclAppendString32(*FrmName , rStrm , true);
		hlStr.tarFrm = *FrmName;
	}

    	// UNC path
   	if( ::get_flag( nFlags, WW8_HLINK_UNC ) )
    	{
       	xLongName.reset( new String );
        	lclAppendString32( *xLongName, rStrm, true );
		lclGetAbsPath( *xLongName, 0 , pDocShell);
    	}
    	// file link or URL
    	else if( ::get_flag( nFlags, WW8_HLINK_BODY ) )
    	{
    		rStrm.Read( aGuid, 16);

        	if( (memcmp(aGuid, maGuidFileMoniker, 16) == 0) )
        	{
            		rStrm >> nLevel;
            		xShortName.reset( new String );
			lclAppendString32( *xShortName,rStrm, false );
			rStrm.SeekRel( 24 );

            		sal_uInt32 nStrLen;
            		rStrm >> nStrLen;
            		if( nStrLen )
            		{
		                rStrm >> nStrLen;
		                nStrLen /= 2;       
						rStrm.SeekRel( 2 );
		                xLongName.reset( new String );
				lclAppendString32( *xLongName, rStrm,nStrLen, true );
				lclGetAbsPath( *xLongName, nLevel, pDocShell);
            		}
            		else
				lclGetAbsPath( *xShortName, nLevel, pDocShell);
        	}
      		else 	if( (memcmp(aGuid, maGuidUrlMoniker, 16) == 0) )
        	{
			sal_uInt32 nStrLen;
            		rStrm >> nStrLen;
            		nStrLen /= 2;       
            		xLongName.reset( new String );
			lclAppendString32( *xLongName,rStrm, nStrLen, true );
            		if( !::get_flag( nFlags, WW8_HLINK_ABS ) )
				lclGetAbsPath( *xLongName, 0 ,pDocShell);
        	}
        	else
	        {
	            DBG_ERRORFILE( "WW8Hyperlink::ReadEmbeddedData - unknown content GUID" );
	        }
	}

	// text mark
    	if( ::get_flag( nFlags, WW8_HLINK_MARK ) )
    	{
       	xTextMark.reset( new String );
        	lclAppendString32( *xTextMark, rStrm, true );
    	}

	if( !xLongName.get() && xShortName.get() )
	{
		xLongName.reset( new String );
       	xLongName->Append(*xShortName);
	}
    	else if( !xLongName.get() && xTextMark.get() )
       	xLongName.reset( new String );

    	if( xLongName.get() )
    	{
       	if( xTextMark.get() )
        	{
            		if( xLongName->Len() == 0 )
                		xTextMark->SearchAndReplaceAll( '!', '.' );
            		xLongName->Append( '#' );
            		xLongName->Append( *xTextMark );
        	}
		hlStr.hLinkAddr = *xLongName;		
    	}
}

SwMSDffManager::SwMSDffManager( SwWW8ImplReader& rRdr )
    : SvxMSDffManager(*rRdr.pTableStream, rRdr.GetBaseURL(), rRdr.pWwFib->fcDggInfo,
        rRdr.pDataStream, 0, 0, COL_WHITE, 12, rRdr.pStrm,
        rRdr.maTracer.GetTrace()),
    rReader(rRdr), pFallbackStream(0), pOldEscherBlipCache(0)
{
    SetSvxMSDffSettings( GetSvxMSDffSettings() );
    nSvxMSDffOLEConvFlags = SwMSDffManager::GetFilterFlags();
}

sal_uInt32 SwMSDffManager::GetFilterFlags()
{
    sal_uInt32 nFlags(0);
    if (const SvtFilterOptions* pOpt = SvtFilterOptions::Get())
    {
        if (pOpt->IsMathType2Math())
            nFlags |= OLE_MATHTYPE_2_STARMATH;
        if (pOpt->IsExcel2Calc())
            nFlags |= OLE_EXCEL_2_STARCALC;
        if (pOpt->IsPowerPoint2Impress())
            nFlags |= OLE_POWERPOINT_2_STARIMPRESS;
        if (pOpt->IsWinWord2Writer())
            nFlags |= OLE_WINWORD_2_STARWRITER;
    }
    return nFlags;
}

/*
 * I would like to override the default OLE importing to add a test
 * and conversion of OCX controls from their native OLE type into our
 * native nonOLE Form Control Objects.
 *
 * cmc
 */
// #i32596# - consider new parameter <_nCalledByGroup>
SdrObject* SwMSDffManager::ImportOLE( long nOLEId,
                                      const Graphic& rGrf,
                                      const Rectangle& rBoundRect,
									  const Rectangle& rVisArea,
                                      const int _nCalledByGroup,
                                      sal_Int64 nAspect ) const
{
    // #i32596# - no import of OLE object, if it's inside a group.
    // NOTE: This can be undone, if grouping of Writer fly frames is possible or
    // if drawing OLE objects are allowed in Writer.
    if ( _nCalledByGroup > 0 )
    {
        return 0L;
    }

    SdrObject* pRet = 0;
    String sStorageName;
    SotStorageRef xSrcStg;
    uno::Reference < embed::XStorage > xDstStg;
    if( GetOLEStorageName( nOLEId, sStorageName, xSrcStg, xDstStg ))
    {
        SvStorageRef xSrc = xSrcStg->OpenSotStorage( sStorageName,
            STREAM_READWRITE| STREAM_SHARE_DENYALL );
        ASSERT(rReader.pFormImpl, "No Form Implementation!");
        STAR_REFERENCE( drawing::XShape ) xShape;
        if ( (!(rReader.bIsHeader || rReader.bIsFooter)) &&
            rReader.pFormImpl->ReadOCXStream(xSrc,&xShape,true))
        {
            pRet = GetSdrObjectFromXShape(xShape);
        }
        else
        {
            ErrCode nError = ERRCODE_NONE;
            pRet = CreateSdrOLEFromStorage( sStorageName, xSrcStg, xDstStg,
                rGrf, rBoundRect, rVisArea, pStData, nError, nSvxMSDffOLEConvFlags, nAspect );
        }
    }
    return pRet;
}

void SwMSDffManager::DisableFallbackStream()
{
    ASSERT(!pFallbackStream || !pOldEscherBlipCache,
        "if you're recursive, you're broken");
    pFallbackStream = pStData2;
    pOldEscherBlipCache = pEscherBlipCache;
    pEscherBlipCache = 0;
    pStData2 = 0;
}

void SwMSDffManager::EnableFallbackStream()
{
    pStData2 = pFallbackStream;
    pEscherBlipCache = pOldEscherBlipCache;
    pOldEscherBlipCache = 0;
    pFallbackStream = 0;
}

sal_uInt16 SwWW8ImplReader::GetToggleAttrFlags() const
{
	return pCtrlStck ? pCtrlStck->GetToggleAttrFlags() : 0;
}

sal_uInt16 SwWW8ImplReader::GetToggleBiDiAttrFlags() const
{
	return pCtrlStck ? pCtrlStck->GetToggleBiDiAttrFlags() : 0;
}

void SwWW8ImplReader::SetToggleAttrFlags(sal_uInt16 nFlags)
{
	if (pCtrlStck)
		pCtrlStck->SetToggleAttrFlags(nFlags);
}

void SwWW8ImplReader::SetToggleBiDiAttrFlags(sal_uInt16 nFlags)
{
	if (pCtrlStck)
		pCtrlStck->SetToggleBiDiAttrFlags(nFlags);
}


SdrObject* SwMSDffManager::ProcessObj(SvStream& rSt,
									   DffObjData& rObjData,
									   void* pData,
									   Rectangle& rTextRect,
									   SdrObject* pObj
									   )
{
	if( !rTextRect.IsEmpty() )
	{
		SvxMSDffImportData& rImportData = *(SvxMSDffImportData*)pData;
		SvxMSDffImportRec* pImpRec = new SvxMSDffImportRec;

		// fill Import Record with data
		pImpRec->nShapeId   = rObjData.nShapeId;
		pImpRec->eShapeType = rObjData.eShapeType;

		rObjData.bClientAnchor = maShapeRecords.SeekToContent( rSt,
											DFF_msofbtClientAnchor,
											SEEK_FROM_CURRENT_AND_RESTART );
		if( rObjData.bClientAnchor )
			ProcessClientAnchor( rSt,
					maShapeRecords.Current()->nRecLen,
					pImpRec->pClientAnchorBuffer, pImpRec->nClientAnchorLen );

		rObjData.bClientData = maShapeRecords.SeekToContent( rSt,
											DFF_msofbtClientData,
											SEEK_FROM_CURRENT_AND_RESTART );
		if( rObjData.bClientData )
			ProcessClientData( rSt,
					maShapeRecords.Current()->nRecLen,
					pImpRec->pClientDataBuffer, pImpRec->nClientDataLen );


        // process user (== Winword) defined parameters in 0xF122 record
        // #i84783# - set special value to determine, if property is provided or not.
        pImpRec->nLayoutInTableCell = 0xFFFFFFFF;
        if(    maShapeRecords.SeekToContent( rSt,
											 DFF_msofbtUDefProp,
											 SEEK_FROM_CURRENT_AND_RESTART )
			&& maShapeRecords.Current()->nRecLen )
		{
			sal_uInt32  nBytesLeft = maShapeRecords.Current()->nRecLen;
			sal_uInt32	nUDData;
			sal_uInt16  nPID;
			while( 5 < nBytesLeft )
			{
				rSt >> nPID;
				if ( rSt.GetError() != 0 )
					break;
				rSt >> nUDData;
				switch( nPID )
				{
					case 0x038F: pImpRec->nXAlign = nUDData; break;
					case 0x0390: pImpRec->nXRelTo = nUDData; break;
					case 0x0391: pImpRec->nYAlign = nUDData; break;
					case 0x0392: pImpRec->nYRelTo = nUDData; break;
                    case 0x03BF: pImpRec->nLayoutInTableCell = nUDData; break;
				}
				if ( rSt.GetError() != 0 )
					break;
				pImpRec->bHasUDefProp = sal_True;
				nBytesLeft  -= 6;
			}
		}

		//  Textrahmen, auch Title oder Outline
		sal_uInt32 nTextId = GetPropertyValue( DFF_Prop_lTxid, 0 );
		if( nTextId )
		{
			SfxItemSet aSet( pSdrModel->GetItemPool() );

            //Originally anything that as a mso_sptTextBox was created as a
            //textbox, this was changed to be created as a simple
            //rect to keep impress happy. For the rest of us we'd like to turn
            //it back into a textbox again.
            sal_Bool bIsSimpleDrawingTextBox = (pImpRec->eShapeType == mso_sptTextBox);
            if (!bIsSimpleDrawingTextBox)
            {
                //Either
                //a) its a simple text object or
                //b) its a rectangle with text and square wrapping.
                bIsSimpleDrawingTextBox =
                (
                    (pImpRec->eShapeType == mso_sptTextSimple) ||
                    (
                        (pImpRec->eShapeType == mso_sptRectangle)
                        // && (eWrapMode == mso_wrapSquare)
                        && ShapeHasText(pImpRec->nShapeId, rObjData.rSpHd.GetRecBegFilePos() )
                    )
                );
            }

            // Distance of Textbox to it's surrounding Autoshape
			sal_Int32 nTextLeft = GetPropertyValue( DFF_Prop_dxTextLeft, 91440L);
			sal_Int32 nTextRight = GetPropertyValue( DFF_Prop_dxTextRight, 91440L );
			sal_Int32 nTextTop = GetPropertyValue( DFF_Prop_dyTextTop, 45720L  );
			sal_Int32 nTextBottom = GetPropertyValue( DFF_Prop_dyTextBottom, 45720L );

			ScaleEmu( nTextLeft );
			ScaleEmu( nTextRight );
			ScaleEmu( nTextTop );
			ScaleEmu( nTextBottom );

            sal_Int32 nTextRotationAngle=0;
            bool bVerticalText = false;
            if ( IsProperty( DFF_Prop_txflTextFlow ) )
            {
                MSO_TextFlow eTextFlow = (MSO_TextFlow)(GetPropertyValue(
                    DFF_Prop_txflTextFlow) & 0xFFFF);
                switch( eTextFlow )
                {
                    case mso_txflBtoT:
                        nTextRotationAngle = 9000;
                    break;
                    case mso_txflVertN:
                    case mso_txflTtoBN:
                        nTextRotationAngle = 27000;
                        break;
                    case mso_txflTtoBA:
                        bVerticalText = true;
                    break;
                    case mso_txflHorzA:
                        bVerticalText = true;
                        nTextRotationAngle = 9000;
                    case mso_txflHorzN:
                    default :
                        break;
                }
            }

            if (nTextRotationAngle)
			{
                while (nTextRotationAngle > 360000)
                    nTextRotationAngle-=9000;
                switch (nTextRotationAngle)
                {
                    case 9000:
                        {
                            long nWidth = rTextRect.GetWidth();
                            rTextRect.Right() = rTextRect.Left() + rTextRect.GetHeight();
                            rTextRect.Bottom() = rTextRect.Top() + nWidth;

                            sal_Int32 nOldTextLeft = nTextLeft;
                            sal_Int32 nOldTextRight = nTextRight;
                            sal_Int32 nOldTextTop = nTextTop;
                            sal_Int32 nOldTextBottom = nTextBottom;

                            nTextLeft = nOldTextBottom;
                            nTextRight = nOldTextTop;
                            nTextTop = nOldTextLeft;
                            nTextBottom = nOldTextRight;
                        }
                        break;
                    case 27000:
                        {
                            long nWidth = rTextRect.GetWidth();
                            rTextRect.Right() = rTextRect.Left() + rTextRect.GetHeight();
                            rTextRect.Bottom() = rTextRect.Top() + nWidth;

                            sal_Int32 nOldTextLeft = nTextLeft;
                            sal_Int32 nOldTextRight = nTextRight;
                            sal_Int32 nOldTextTop = nTextTop;
                            sal_Int32 nOldTextBottom = nTextBottom;

                            nTextLeft = nOldTextTop;
                            nTextRight = nOldTextBottom;
                            nTextTop = nOldTextRight;
                            nTextBottom = nOldTextLeft;
                        }
                        break;
                    default:
                        break;
                }
			}

            if (bIsSimpleDrawingTextBox)
            {
                SdrObject::Free( pObj );
				pObj = new SdrRectObj(OBJ_TEXT, rTextRect);
            }

            // Die vertikalen Absatzeinrueckungen sind im BoundRect mit drin,
            // hier rausrechnen
            Rectangle aNewRect(rTextRect);
			aNewRect.Bottom() -= nTextTop + nTextBottom;
            aNewRect.Right() -= nTextLeft + nTextRight;

			// Nur falls es eine einfache Textbox ist, darf der Writer
			// das Objekt durch einen Rahmen ersetzen, ansonsten
			if( bIsSimpleDrawingTextBox )
			{
				SvxMSDffShapeInfo aTmpRec( 0, pImpRec->nShapeId );
				aTmpRec.bSortByShapeId = sal_True;

				sal_uInt16 nFound;
				if( GetShapeInfos()->Seek_Entry( &aTmpRec, &nFound ) )
				{
					SvxMSDffShapeInfo& rInfo = *GetShapeInfos()->GetObject(nFound);
					pImpRec->bReplaceByFly   = rInfo.bReplaceByFly;
					pImpRec->bLastBoxInChain = rInfo.bLastBoxInChain;
				}
			}

			if( bIsSimpleDrawingTextBox )
                ApplyAttributes( rSt, aSet, rObjData );

            bool bFitText = false;
            if (GetPropertyValue(DFF_Prop_FitTextToShape) & 2)
            {
                aSet.Put( SdrTextAutoGrowHeightItem( sal_True ) );
                aSet.Put( SdrTextMinFrameHeightItem(
                    aNewRect.Bottom() - aNewRect.Top() ) );
                aSet.Put( SdrTextMinFrameWidthItem(
                    aNewRect.Right() - aNewRect.Left() ) );
                bFitText = true;
            }
            else
            {
                aSet.Put( SdrTextAutoGrowHeightItem( sal_False ) );
                aSet.Put( SdrTextAutoGrowWidthItem( sal_False ) );
            }

			switch ( (MSO_WrapMode)
                GetPropertyValue( DFF_Prop_WrapText, mso_wrapSquare ) )
			{
				case mso_wrapNone :
    				aSet.Put( SdrTextAutoGrowWidthItem( sal_True ) );
                    pImpRec->bAutoWidth = true;
				break;
				case mso_wrapByPoints :
					aSet.Put( SdrTextContourFrameItem( sal_True ) );
				break;
				default:
					;
			}

			// Abstaende an den Raendern der Textbox setzen
			aSet.Put( SdrTextLeftDistItem( nTextLeft ) );
			aSet.Put( SdrTextRightDistItem( nTextRight ) );
			aSet.Put( SdrTextUpperDistItem( nTextTop ) );
			aSet.Put( SdrTextLowerDistItem( nTextBottom ) );
			pImpRec->nDxTextLeft	= nTextLeft;
			pImpRec->nDyTextTop		= nTextTop;
			pImpRec->nDxTextRight	= nTextRight;
			pImpRec->nDyTextBottom	= nTextBottom;

			// --> SJ 2009-03-06 : taking the correct default (which is mso_anchorTop)
			MSO_Anchor eTextAnchor =
                (MSO_Anchor)GetPropertyValue( DFF_Prop_anchorText, mso_anchorTop );

            SdrTextVertAdjust eTVA = bVerticalText
                                     ? SDRTEXTVERTADJUST_BLOCK
                                     : SDRTEXTVERTADJUST_CENTER;
            SdrTextHorzAdjust eTHA = bVerticalText
                                     ? SDRTEXTHORZADJUST_CENTER
                                     : SDRTEXTHORZADJUST_BLOCK;

			switch( eTextAnchor )
            {
                case mso_anchorTop:
                {
                    if ( bVerticalText )
                        eTHA = SDRTEXTHORZADJUST_RIGHT;
                    else
                        eTVA = SDRTEXTVERTADJUST_TOP;
                }
                break;
                case mso_anchorTopCentered:
                {
                    if ( bVerticalText )
                        eTHA = SDRTEXTHORZADJUST_RIGHT;
                    else
                        eTVA = SDRTEXTVERTADJUST_TOP;
                }
                break;
                case mso_anchorMiddle:
                break;
                case mso_anchorMiddleCentered:
                break;
                case mso_anchorBottom:
                {
                    if ( bVerticalText )
                        eTHA = SDRTEXTHORZADJUST_LEFT;
                    else
                        eTVA = SDRTEXTVERTADJUST_BOTTOM;
                }
                break;
                case mso_anchorBottomCentered:
                {
                    if ( bVerticalText )
                        eTHA = SDRTEXTHORZADJUST_LEFT;
                    else
                        eTVA = SDRTEXTVERTADJUST_BOTTOM;
                }
                break;
/*
				case mso_anchorTopBaseline:
				case mso_anchorBottomBaseline:
				case mso_anchorTopCenteredBaseline:
				case mso_anchorBottomCenteredBaseline:
				break;
*/
				default:
					;
			}

            aSet.Put( SdrTextVertAdjustItem( eTVA ) );
            aSet.Put( SdrTextHorzAdjustItem( eTHA ) );

            if (pObj != NULL)
            {
                pObj->SetMergedItemSet(aSet);
                pObj->SetModel(pSdrModel);
                
                if (bVerticalText && dynamic_cast< SdrTextObj* >( pObj ) )
                    dynamic_cast< SdrTextObj* >( pObj )->SetVerticalWriting(sal_True);
                
                if ( bIsSimpleDrawingTextBox )
                {
                    if ( nTextRotationAngle )
                    {
                        long nMinWH = rTextRect.GetWidth() < rTextRect.GetHeight() ?
                            rTextRect.GetWidth() : rTextRect.GetHeight();
                        nMinWH /= 2;
                        Point aPivot(rTextRect.TopLeft());
                        aPivot.X() += nMinWH;
                        aPivot.Y() += nMinWH;
                        double a = nTextRotationAngle * nPi180;
                        pObj->NbcRotate(aPivot, nTextRotationAngle, sin(a), cos(a));
                    }
                }

                if ( ( ( rObjData.nSpFlags & SP_FFLIPV ) || mnFix16Angle || nTextRotationAngle ) && dynamic_cast< SdrObjCustomShape* >( pObj ) )
                {
                    SdrObjCustomShape* pCustomShape = dynamic_cast< SdrObjCustomShape* >( pObj );
                    
                    double fExtraTextRotation = 0.0;
                    if ( mnFix16Angle && !( GetPropertyValue( DFF_Prop_FitTextToShape ) & 4 ) )
                    {	// text is already rotated, we have to take back the object rotation if DFF_Prop_RotateText is false
                        fExtraTextRotation = -mnFix16Angle;
                    }
                    if ( rObjData.nSpFlags & SP_FFLIPV )	// sj: in ppt the text is flipped, whereas in word the text
                    {										// remains unchanged, so we have to take back the flipping here
                        fExtraTextRotation += 18000.0;		// because our core will flip text if the shape is flipped.
                    }
                    fExtraTextRotation += nTextRotationAngle;
                    if ( !::basegfx::fTools::equalZero( fExtraTextRotation ) )
                    {
                        fExtraTextRotation /= 100.0;
                        SdrCustomShapeGeometryItem aGeometryItem( (SdrCustomShapeGeometryItem&)pCustomShape->GetMergedItem( SDRATTR_CUSTOMSHAPE_GEOMETRY ) );
                        const rtl::OUString	sTextRotateAngle( RTL_CONSTASCII_USTRINGPARAM ( "TextRotateAngle" ) );
                        com::sun::star::beans::PropertyValue aPropVal;
                        aPropVal.Name = sTextRotateAngle;
                        aPropVal.Value <<= fExtraTextRotation;
                        aGeometryItem.SetPropertyValue( aPropVal );
                        pCustomShape->SetMergedItem( aGeometryItem );
                    }
                }
                else if ( mnFix16Angle )
                {
                    // rotate text with shape ?
                    double a = mnFix16Angle * nPi180;
                    pObj->NbcRotate( rObjData.aBoundRect.Center(), mnFix16Angle,
                                     sin( a ), cos( a ) );
                }
            }
        }
		else if( !pObj )
		{
			// simple rectangular objects are ignored by ImportObj()  :-(
			// this is OK for Draw but not for Calc and Writer
			// cause here these objects have a default border
			pObj = new SdrRectObj(rTextRect);
			pObj->SetModel( pSdrModel );
            SfxItemSet aSet( pSdrModel->GetItemPool() );
            ApplyAttributes( rSt, aSet, rObjData );

			const SfxPoolItem* pPoolItem=NULL;
			SfxItemState eState = aSet.GetItemState( XATTR_FILLCOLOR,
													 sal_False, &pPoolItem );
			if( SFX_ITEM_DEFAULT == eState )
				aSet.Put( XFillColorItem( String(),
						  Color( mnDefaultColor ) ) );
			pObj->SetMergedItemSet(aSet);
		}

        //Means that fBehindDocument is set
        if (GetPropertyValue(DFF_Prop_fPrint) & 0x20)
		    pImpRec->bDrawHell = sal_True;
        else
		    pImpRec->bDrawHell = sal_False;
        if (GetPropertyValue(DFF_Prop_fPrint) & 0x02)
            pImpRec->bHidden = sal_True;
		pImpRec->nNextShapeId	= GetPropertyValue( DFF_Prop_hspNext, 0 );

		if ( nTextId )
		{
			pImpRec->aTextId.nTxBxS = (sal_uInt16)( nTextId >> 16 );
			pImpRec->aTextId.nSequence = (sal_uInt16)nTextId;
		}

		pImpRec->nDxWrapDistLeft = GetPropertyValue(
									DFF_Prop_dxWrapDistLeft, 114935L ) / 635L;
		pImpRec->nDyWrapDistTop = GetPropertyValue(
									DFF_Prop_dyWrapDistTop, 0 ) / 635L;
		pImpRec->nDxWrapDistRight = GetPropertyValue(
									DFF_Prop_dxWrapDistRight, 114935L ) / 635L;
		pImpRec->nDyWrapDistBottom = GetPropertyValue(
									DFF_Prop_dyWrapDistBottom, 0 ) / 635L;
        // 16.16 fraction times total image width or height, as appropriate.

        if (SeekToContent(DFF_Prop_pWrapPolygonVertices, rSt))
        {
            delete pImpRec->pWrapPolygon;
            pImpRec->pWrapPolygon = NULL;

            sal_uInt16 nNumElemVert, nNumElemMemVert, nElemSizeVert;
            rSt >> nNumElemVert >> nNumElemMemVert >> nElemSizeVert;
            if (nNumElemVert && ((nElemSizeVert == 8) || (nElemSizeVert == 4)))
            {
                pImpRec->pWrapPolygon = new Polygon(nNumElemVert);
                for (sal_uInt16 i = 0; i < nNumElemVert; ++i)
                {
                    sal_Int32 nX, nY;
                    if (nElemSizeVert == 8)
                        rSt >> nX >> nY;
                    else
                    {
                        sal_Int16 nSmallX, nSmallY;
                        rSt >> nSmallX >> nSmallY;
                        nX = nSmallX;
                        nY = nSmallY;
                    }
                    (*(pImpRec->pWrapPolygon))[i].X() = nX;
                    (*(pImpRec->pWrapPolygon))[i].Y() = nY;
                }
            }
        }

		pImpRec->nCropFromTop = GetPropertyValue(
									DFF_Prop_cropFromTop, 0 );
		pImpRec->nCropFromBottom = GetPropertyValue(
									DFF_Prop_cropFromBottom, 0 );
		pImpRec->nCropFromLeft = GetPropertyValue(
									DFF_Prop_cropFromLeft, 0 );
		pImpRec->nCropFromRight = GetPropertyValue(
									DFF_Prop_cropFromRight, 0 );

        sal_uInt32 nLineFlags = GetPropertyValue( DFF_Prop_fNoLineDrawDash );
        if ( !IsHardAttribute( DFF_Prop_fLine ) &&
             pImpRec->eShapeType == mso_sptPictureFrame )
        {
            nLineFlags &= ~0x08;
        }
        pImpRec->eLineStyle = (nLineFlags & 8)
                              ? (MSO_LineStyle)GetPropertyValue(
                                                    DFF_Prop_lineStyle,
                                                    mso_lineSimple )
                              : (MSO_LineStyle)USHRT_MAX;

		pImpRec->nFlags = rObjData.nSpFlags;

		if( pImpRec->nShapeId )
		{
			// Import-Record-Liste ergaenzen
			pImpRec->pObj = pObj;
			rImportData.aRecords.Insert( pImpRec );

			// Eintrag in Z-Order-Liste um Zeiger auf dieses Objekt ergaenzen
			/*Only store objects which are not deep inside the tree*/
			if( ( rObjData.nCalledByGroup == 0 )
				||
				( (rObjData.nSpFlags & SP_FGROUP)
				 && (rObjData.nCalledByGroup < 2) )
			  )
				StoreShapeOrder( pImpRec->nShapeId,
								( ( (sal_uLong)pImpRec->aTextId.nTxBxS ) << 16 )
									+ pImpRec->aTextId.nSequence, pObj );
		}
		else
			delete pImpRec;
	}

	sal_uInt32 nBufferSize = GetPropertyValue( DFF_Prop_pihlShape );
	 if( (0 < nBufferSize) && (nBufferSize <= 0xFFFF) && SeekToContent( DFF_Prop_pihlShape, rSt ) )
	{
		SvMemoryStream aMemStream;
		String aStrURL;
		struct HyperLinksTable hlStr;
		sal_uInt16 mnRawRecId,mnRawRecSize;
		aMemStream << sal_uInt16( 0 ) << static_cast< sal_uInt16 >( nBufferSize );

		// copy from DFF stream to memory stream
		::std::vector< sal_uInt8 > aBuffer( nBufferSize );
		sal_uInt8* pnData = &aBuffer.front();
		sal_uInt8 mnStreamSize;
		if( pnData && rSt.Read( pnData, nBufferSize ) == nBufferSize )
		{
			aMemStream.Write( pnData, nBufferSize );
			aMemStream.Seek( STREAM_SEEK_TO_END );
			mnStreamSize = aMemStream.Tell();
			aMemStream.Seek( STREAM_SEEK_TO_BEGIN );
			bool bRet =  4 <= mnStreamSize;
			if( bRet )
				aMemStream >> mnRawRecId >> mnRawRecSize;
			SwDocShell* pDocShell = rReader.mpDocShell;
			if(pDocShell)
			{
				rReader.ReadEmbeddedData( aMemStream, pDocShell, hlStr);
			}
		}

		if(pObj && hlStr.hLinkAddr.Len() > 0)
		{
			SwMacroInfo* pInfo = GetMacroInfo( pObj, true );
			if( pInfo )
			{
				pInfo->SetShapeId( rObjData.nShapeId );
				pInfo->SetHlink( hlStr.hLinkAddr );
				if(hlStr.tarFrm.Len() > 0)
					pInfo->SetTarFrm( hlStr.tarFrm );
				String aNameStr = GetPropertyString( DFF_Prop_wzName, rSt );
				if(aNameStr.Len() > 0)
					pInfo->SetName( aNameStr );
			}				
		}
    	}	
	
	return pObj;
}

/***************************************************************************
#  Spezial FastSave - Attribute
#**************************************************************************/

void SwWW8ImplReader::Read_StyleCode( sal_uInt16, const sal_uInt8* pData, short nLen )
{
    if (nLen < 0)
    {
        bCpxStyle = false;
        return;
    }
    sal_uInt16 nColl = 0;
    if (pWwFib->GetFIBVersion() <= ww::eWW2)
        nColl = *pData;
    else
        nColl = SVBT16ToShort(pData);
    if (nColl < nColls)
    {
        SetTxtFmtCollAndListLevel( *pPaM, pCollA[nColl] );
        bCpxStyle = true;
    }
}

// Read_Majority ist fuer Majority ( 103 ) und Majority50 ( 108 )
void SwWW8ImplReader::Read_Majority( sal_uInt16, const sal_uInt8* , short )
{
}

//-----------------------------------------
//            Stack
//-----------------------------------------
void SwWW8FltControlStack::NewAttr(const SwPosition& rPos,
    const SfxPoolItem& rAttr)
{
    ASSERT(RES_TXTATR_FIELD != rAttr.Which(), "probably don't want to put"
        "fields into the control stack");
    ASSERT(RES_TXTATR_ANNOTATION != rAttr.Which(), "probably don't want to put"
        "annotations into the control stack");
    ASSERT(RES_TXTATR_INPUTFIELD != rAttr.Which(), "probably don't want to put"
        "input fields into the control stack");
    ASSERT(RES_FLTR_REDLINE != rAttr.Which(), "probably don't want to put"
        "redlines into the control stack");
    SwFltControlStack::NewAttr(rPos, rAttr);
}

void SwWW8FltControlStack::SetAttr(const SwPosition& rPos, sal_uInt16 nAttrId,
    sal_Bool bTstEnde, long nHand, sal_Bool )
{
    //Doing a textbox, and using the control stack only as a temporary
    //collection point for properties which will are not to be set into
    //the real document
    if (rReader.pPlcxMan && rReader.pPlcxMan->GetDoingDrawTextBox())
    {
        sal_uInt16 nCnt = static_cast< sal_uInt16 >(Count());
        for (sal_uInt16 i=0; i < nCnt; ++i)
        {
            SwFltStackEntry* pEntry = (*this)[i];
            if (nAttrId == pEntry->pAttr->Which())
            {
                DeleteAndDestroy(i--);
                --nCnt;
            }
        }
    }
    else //Normal case, set the attribute into the document
        SwFltControlStack::SetAttr(rPos, nAttrId, bTstEnde, nHand);
}

long GetListFirstLineIndent(const SwNumFmt &rFmt)
{
    ASSERT( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_WIDTH_AND_POSITION,
            "<GetListFirstLineIndent> - misusage: position-and-space-mode does not equal LABEL_WIDTH_AND_POSITION" );

    SvxAdjust eAdj = rFmt.GetNumAdjust();
    long nReverseListIndented;
    if (eAdj == SVX_ADJUST_RIGHT)
        nReverseListIndented = -rFmt.GetCharTextDistance();
    else if (eAdj == SVX_ADJUST_CENTER)
        nReverseListIndented = rFmt.GetFirstLineOffset()/2;
    else
        nReverseListIndented = rFmt.GetFirstLineOffset();
    return nReverseListIndented;
}

long lcl_GetTrueMargin(const SvxLRSpaceItem &rLR, const SwNumFmt &rFmt,
    long &rFirstLinePos)
{
    ASSERT( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_WIDTH_AND_POSITION,
            "<lcl_GetTrueMargin> - misusage: position-and-space-mode does not equal LABEL_WIDTH_AND_POSITION" );

    const long nBodyIndent = rLR.GetTxtLeft();
    const long nFirstLineDiff = rLR.GetTxtFirstLineOfst();
    rFirstLinePos = nBodyIndent + nFirstLineDiff;

    const long nPseudoListBodyIndent = rFmt.GetAbsLSpace();
    const long nReverseListIndented = GetListFirstLineIndent(rFmt);
    long nExtraListIndent = nPseudoListBodyIndent + nReverseListIndented;

    return nExtraListIndent > 0 ? nExtraListIndent : 0;
}

void SyncIndentWithList( SvxLRSpaceItem &rLR, 
                         const SwNumFmt &rFmt,
                         const bool bFirstLineOfstSet,
                         const bool bLeftIndentSet )
{
    if ( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_WIDTH_AND_POSITION )
    {        
        long nWantedFirstLinePos;
        long nExtraListIndent = lcl_GetTrueMargin(rLR, rFmt, nWantedFirstLinePos);
        rLR.SetTxtLeft(nWantedFirstLinePos - nExtraListIndent);
        rLR.SetTxtFirstLineOfst(0);
    }
    else if ( rFmt.GetPositionAndSpaceMode() == SvxNumberFormat::LABEL_ALIGNMENT )
    {
        if ( !bFirstLineOfstSet && bLeftIndentSet && 
             rFmt.GetFirstLineIndent() != 0 )
        {
            rLR.SetTxtFirstLineOfst( rFmt.GetFirstLineIndent() );
        }
        else if ( bFirstLineOfstSet && !bLeftIndentSet &&
                  rFmt.GetIndentAt() != 0 )
        {
            rLR.SetTxtLeft( rFmt.GetIndentAt() );
        }
        else if (!bFirstLineOfstSet && !bLeftIndentSet )
        {
            if ( rFmt.GetFirstLineIndent() != 0 )
            {
                rLR.SetTxtFirstLineOfst( rFmt.GetFirstLineIndent() );
            }
            if ( rFmt.GetIndentAt() != 0 )
            {
                rLR.SetTxtLeft( rFmt.GetIndentAt() );
            }
        }
    }
}

const SwNumFmt* SwWW8FltControlStack::GetNumFmtFromStack(const SwPosition &rPos,
    const SwTxtNode &rTxtNode)
{
    const SwNumFmt *pRet = 0;
    const SfxPoolItem *pItem = GetStackAttr(rPos, RES_FLTR_NUMRULE);
    if (pItem && rTxtNode.GetNumRule())
    {
        String sName(((SfxStringItem*)pItem)->GetValue());
        if (rTxtNode.IsCountedInList())
        {
            const SwNumRule *pRule = pDoc->FindNumRulePtr(sName);
            sal_uInt8 nLvl = static_cast< sal_uInt8 >(rTxtNode.GetActualListLevel());
            pRet = &(pRule->Get(nLvl));
        }
    }
    return pRet;
}

//Modify here for #119405, by easyfan, 2012-05-24
sal_Int32 SwWW8FltControlStack::GetCurrAttrCP() const 
{
	return rReader.GetCurrAttrCP();
}
bool SwWW8FltControlStack::IsParaEndInCPs(sal_Int32 nStart,sal_Int32 nEnd,bool bSdOD) const 
{
	return rReader.IsParaEndInCPs(nStart,nEnd,bSdOD);
}
//End of modification, by easyfan
//Modify for #119405 by chengjh, 2012-08-16
//Clear the para end position recorded in reader intermittently for the least impact on loading performance
void SwWW8FltControlStack::ClearParaEndPosition()
{
	if ( Count() != 0 )
		return;

	rReader.ClearParaEndPosition();
}
bool SwWW8FltControlStack::CheckSdOD(sal_Int32 nStart,sal_Int32 nEnd)
{
	return rReader.IsParaEndInCPs(nStart,nEnd);
}
//End

void SwWW8ReferencedFltEndStack::SetAttrInDoc( const SwPosition& rTmpPos,
                                               SwFltStackEntry* pEntry )
{
    switch( pEntry->pAttr->Which() )
    {
    case RES_FLTR_BOOKMARK:
        {
            // suppress insertion of bookmark, which is recognized as an internal bookmark used for table-of-content 
            // and which is not referenced.
            bool bInsertBookmarkIntoDoc = true;

            SwFltBookmark* pFltBookmark = dynamic_cast<SwFltBookmark*>(pEntry->pAttr);
            if ( pFltBookmark != 0 && pFltBookmark->IsTOCBookmark() )
            {
                const String& rName = pFltBookmark->GetName();
                ::std::set< String, SwWW8::ltstr >::const_iterator aResult = aReferencedTOCBookmarks.find(rName);
                if ( aResult == aReferencedTOCBookmarks.end() )
                {
                    bInsertBookmarkIntoDoc = false;
                }
            }
            if ( bInsertBookmarkIntoDoc )
            {
                SwFltEndStack::SetAttrInDoc( rTmpPos, pEntry );
            }
            break;
        }
    default:
        SwFltEndStack::SetAttrInDoc( rTmpPos, pEntry );
        break;
    }

}

void SwWW8FltControlStack::SetAttrInDoc(const SwPosition& rTmpPos,
    SwFltStackEntry* pEntry)
{
    switch( pEntry->pAttr->Which() )
    {
        case RES_LR_SPACE:
            {
                /*
                 Loop over the affect nodes and
                 a) convert the word style absolute indent to indent relative
                  to any numbering indent active on the nodes
                 b) adjust the writer style tabstops relative to the old
                  paragraph indent to be relative to the new paragraph indent
                */
                using namespace sw::util;
                SwPaM aRegion(rTmpPos);
                if (pEntry->MakeRegion(pDoc, aRegion, false))
                {
                    SvxLRSpaceItem aNewLR( *(SvxLRSpaceItem*)pEntry->pAttr );
                    sal_uLong nStart = aRegion.Start()->nNode.GetIndex();
                    sal_uLong nEnd   = aRegion.End()->nNode.GetIndex();
                    for(; nStart <= nEnd; ++nStart)
                    {
                        SwNode* pNode = pDoc->GetNodes()[ nStart ];
                        if (!pNode || !pNode->IsTxtNode())
                            continue;

                        SwCntntNode* pNd = (SwCntntNode*)pNode;
                        SvxLRSpaceItem aOldLR = (const SvxLRSpaceItem&)pNd->GetAttr(RES_LR_SPACE);

                        SwTxtNode *pTxtNode = (SwTxtNode*)pNode;

                        const SwNumFmt *pNum = 0;
                        pNum = GetNumFmtFromStack( *aRegion.GetPoint(), *pTxtNode );
                        if (!pNum)
                        {        
                            pNum = GetNumFmtFromTxtNode(*pTxtNode);
                        }

                        if ( pNum )
                        {
                            const bool bFirstLineIndentSet =
                                ( rReader.maTxtNodesHavingFirstLineOfstSet.end() !=
                                    rReader.maTxtNodesHavingFirstLineOfstSet.find( pNode ) );
                            const bool bLeftIndentSet =
                                (  rReader.maTxtNodesHavingLeftIndentSet.end() !=
                                    rReader.maTxtNodesHavingLeftIndentSet.find( pNode ) );
                            SyncIndentWithList( aNewLR, *pNum,
                                                bFirstLineIndentSet,
                                                bLeftIndentSet );
                        }

                        if (aNewLR == aOldLR)
                            continue;

                        pNd->SetAttr(aNewLR);

                    }
                }
            }
            break;

        case RES_TXTATR_FIELD:
            ASSERT(sal_False, "What is a field doing in the control stack,"
                "probably should have been in the endstack");
            break;

        case RES_TXTATR_ANNOTATION:
            ASSERT(sal_False, "What is a annotation doing in the control stack,"
                "probably should have been in the endstack");
            break;

        case RES_TXTATR_INPUTFIELD:
            ASSERT(sal_False, "What is a input field doing in the control stack,"
                "probably should have been in the endstack");
            break;

        case RES_TXTATR_INETFMT:
            {
                SwPaM aRegion(rTmpPos);
                if (pEntry->MakeRegion(pDoc, aRegion, false))
                {
                    SwFrmFmt *pFrm;
                    //If we have just one single inline graphic then
                    //don't insert a field for the single frame, set
                    //the frames hyperlink field attribute directly.
                    if (0 != (pFrm = rReader.ContainsSingleInlineGraphic(aRegion)))
                    {
                        const SwFmtINetFmt *pAttr = (const SwFmtINetFmt *)
                            pEntry->pAttr;
                        SwFmtURL aURL;
                        aURL.SetURL(pAttr->GetValue(), false);
                        aURL.SetTargetFrameName(pAttr->GetTargetFrame());
                        pFrm->SetFmtAttr(aURL);
                    }
                    else
                    {
                        pDoc->InsertPoolItem(aRegion, *pEntry->pAttr, 0);
                    }
                }
            }
            break;
        default:
            SwFltControlStack::SetAttrInDoc(rTmpPos, pEntry);
            break;
    }
}

const SfxPoolItem* SwWW8FltControlStack::GetFmtAttr(const SwPosition& rPos,
    sal_uInt16 nWhich)
{
    const SfxPoolItem *pItem = GetStackAttr(rPos, nWhich);
    if (!pItem)
    {
        SwCntntNode const*const pNd = rPos.nNode.GetNode().GetCntntNode();
        if (!pNd)
            pItem = &pDoc->GetAttrPool().GetDefaultItem(nWhich);
        else
        {
            /*
            If we're hunting for the indent on a paragraph and need to use the
            parent style indent, then return the indent in msword format, and
            not writer format, because that's the style that the filter works
            in (naturally)
            */
            if (nWhich == RES_LR_SPACE)
            {
                SfxItemState eState = SFX_ITEM_DEFAULT;
                if (const SfxItemSet *pSet = pNd->GetpSwAttrSet())
                    eState = pSet->GetItemState(RES_LR_SPACE, false);
                if (eState != SFX_ITEM_SET && rReader.pCollA != NULL)
                    pItem = &(rReader.pCollA[rReader.nAktColl].maWordLR);
            }

            if (!pItem)
                pItem = &pNd->GetAttr(nWhich);
        }
    }
    return pItem;
}

const SfxPoolItem* SwWW8FltControlStack::GetStackAttr(const SwPosition& rPos,
    sal_uInt16 nWhich)
{
    SwNodeIndex aNode( rPos.nNode, -1 );
    sal_uInt16 nIdx = rPos.nContent.GetIndex();

    sal_uInt16 nSize = static_cast< sal_uInt16 >(Count());
    while (nSize)
    {
        const SwFltStackEntry* pEntry = (*this)[ --nSize ];
        if (pEntry->pAttr->Which() == nWhich)
        {
            if ( (pEntry->bLocked) || (
                (pEntry->nMkNode <= aNode) && (pEntry->nPtNode >= aNode) &&
                (pEntry->nMkCntnt <= nIdx) && (pEntry->nPtCntnt >= nIdx) ) )
            {
                return (const SfxPoolItem*)pEntry->pAttr;
            }
        }
    }
    return 0;
}

bool SwWW8FltRefStack::IsFtnEdnBkmField(
    const SwFmtFld& rFmtFld,
    sal_uInt16& rBkmNo)
{
    const SwField* pFld = rFmtFld.GetField();
    sal_uInt16 nSubType;
    if(pFld && (RES_GETREFFLD == pFld->Which())
        && ((REF_FOOTNOTE == (nSubType = pFld->GetSubType())) || (REF_ENDNOTE  == nSubType))
        && ((SwGetRefField*)pFld)->GetSetRefName().Len())
    {
        const IDocumentMarkAccess* const pMarkAccess = pDoc->getIDocumentMarkAccess();
        IDocumentMarkAccess::const_iterator_t ppBkmk =
            pMarkAccess->findMark( ((SwGetRefField*)pFld)->GetSetRefName() );
        if(ppBkmk != pMarkAccess->getAllMarksEnd())
        {
            // find Sequence No of corresponding Foot-/Endnote
            rBkmNo = ppBkmk - pMarkAccess->getAllMarksBegin();
            return true;
        }
    }
    return false;
}

void SwWW8FltRefStack::SetAttrInDoc(const SwPosition& rTmpPos,
        SwFltStackEntry* pEntry)
{
    switch( pEntry->pAttr->Which() )
    {
        /*
        Look up these in our lists of bookmarks that were changed to
        variables, and replace the ref field with a var field, otherwise
        do normal (?) strange stuff
        */
        case RES_TXTATR_FIELD:
        case RES_TXTATR_ANNOTATION:
        case RES_TXTATR_INPUTFIELD:
        {
            SwNodeIndex aIdx(pEntry->nMkNode, 1);
            SwPaM aPaM(aIdx, pEntry->nMkCntnt);

            SwFmtFld& rFmtFld   = *(SwFmtFld*)pEntry->pAttr;
            SwField* pFld = rFmtFld.GetField();

            if (!RefToVar(pFld,pEntry))
            {
                sal_uInt16 nBkmNo;
                if( IsFtnEdnBkmField(rFmtFld, nBkmNo) )
                {
                    ::sw::mark::IMark const * const pMark = (pDoc->getIDocumentMarkAccess()->getAllMarksBegin() + nBkmNo)->get();

                    const SwPosition& rBkMrkPos = pMark->GetMarkPos();

                    SwTxtNode* pTxt = rBkMrkPos.nNode.GetNode().GetTxtNode();
                    if( pTxt && rBkMrkPos.nContent.GetIndex() )
                    {
                        SwTxtAttr* const pFtn = pTxt->GetTxtAttrForCharAt(
                            rBkMrkPos.nContent.GetIndex()-1, RES_TXTATR_FTN );
                        if( pFtn )
                        {
                            sal_uInt16 nRefNo = ((SwTxtFtn*)pFtn)->GetSeqRefNo();

                            ((SwGetRefField*)pFld)->SetSeqNo( nRefNo );

                            if( pFtn->GetFtn().IsEndNote() )
                                ((SwGetRefField*)pFld)->SetSubType(REF_ENDNOTE);
                        }
                    }
                }
            }

            pDoc->InsertPoolItem(aPaM, *pEntry->pAttr, 0);
            MoveAttrs(*aPaM.GetPoint());
        }
        break;
        case RES_FLTR_TOX:
            SwFltEndStack::SetAttrInDoc(rTmpPos, pEntry);
            break;
        default:
        case RES_FLTR_BOOKMARK:
            ASSERT(sal_False, "EndStck used with non field, not what we want");
            SwFltEndStack::SetAttrInDoc(rTmpPos, pEntry);
            break;
    }
}

/*
 For styles we will do our tabstop arithmetic in word style and adjust them to
 writer style after all the styles have been finished and the dust settles as
 to what affects what.

 For explicit attributes we turn the adjusted writer tabstops back into 0 based
 word indexes and we'll turn them back into writer indexes when setting them
 into the document. If explicit left indent exist which affects them, then this
 is handled when the explict left indent is set into the document
*/
void SwWW8ImplReader::Read_Tab(sal_uInt16 , const sal_uInt8* pData, short nLen)
{
    if (nLen < 0)
    {
        pCtrlStck->SetAttr(*pPaM->GetPoint(), RES_PARATR_TABSTOP);
        return;
    }

    short i;
    const sal_uInt8* pDel = pData + 1;                       // Del - Array
    sal_uInt8 nDel = pData[0];
    const sal_uInt8* pIns = pData + 2*nDel + 2;          // Ins - Array
    sal_uInt8 nIns = pData[nDel*2+1];
    WW8_TBD* pTyp = (WW8_TBD*)(pData + 2*nDel + 2*nIns + 2);// Typ - Array

    SvxTabStopItem aAttr(0, 0, SVX_TAB_ADJUST_DEFAULT, RES_PARATR_TABSTOP);

    const SwTxtFmtColl* pSty = 0;
    sal_uInt16 nTabBase;
    if (pAktColl)               // StyleDef
    {
        nTabBase = pCollA[nAktColl].nBase;
        if (nTabBase < nColls)              // Based On
            pSty = (const SwTxtFmtColl*)pCollA[nTabBase].pFmt;
    }
    else
    {                                       // Text
        nTabBase = nAktColl;
        pSty = (const SwTxtFmtColl*)pCollA[nAktColl].pFmt;
    }

    bool bFound = false;
	::std::hash_set<size_t> aLoopWatch;
    while (pSty && !bFound)
    {
        const SfxPoolItem* pTabs;
        bFound = pSty->GetAttrSet().GetItemState(RES_PARATR_TABSTOP, false,
            &pTabs) == SFX_ITEM_SET;
        if( bFound )
            aAttr = *((const SvxTabStopItem*)pTabs);
        else
        {

            sal_uInt16 nOldTabBase = nTabBase;
            // If based on another
            if (nTabBase < nColls)
                nTabBase = pCollA[nTabBase].nBase;

            if (
                    nTabBase < nColls &&
                    nOldTabBase != nTabBase &&
                    nTabBase != ww::stiNil
               )
            {
				// #i61789: Stop searching when next style is the same as the
				// current one (prevent loop)
				aLoopWatch.insert(reinterpret_cast<size_t>(pSty));
                pSty = (const SwTxtFmtColl*)pCollA[nTabBase].pFmt;

				if (aLoopWatch.find(reinterpret_cast<size_t>(pSty)) !=
					aLoopWatch.end())
					pSty = 0;
            }
            else
                pSty = 0;                           // gib die Suche auf
        }
    }

    SvxTabStop aTabStop;
    for (i=0; i < nDel; ++i)
    {
        sal_uInt16 nPos = aAttr.GetPos(SVBT16ToShort(pDel + i*2));
        if( nPos != SVX_TAB_NOTFOUND )
            aAttr.Remove( nPos, 1 );
    }

    for (i=0; i < nIns; ++i)
    {
        short nPos = SVBT16ToShort(pIns + i*2);
        aTabStop.GetTabPos() = nPos;
        switch( SVBT8ToByte( pTyp[i].aBits1 ) & 0x7 )       // pTyp[i].jc
        {
            case 0:
                aTabStop.GetAdjustment() = SVX_TAB_ADJUST_LEFT;
                break;
            case 1:
                aTabStop.GetAdjustment() = SVX_TAB_ADJUST_CENTER;
                break;
            case 2:
                aTabStop.GetAdjustment() = SVX_TAB_ADJUST_RIGHT;
                break;
            case 3:
                aTabStop.GetAdjustment() = SVX_TAB_ADJUST_DECIMAL;
                break;
            case 4:
                continue;                   // ignoriere Bar
        }

        switch( SVBT8ToByte( pTyp[i].aBits1 ) >> 3 & 0x7 )
        {
            case 0:
                aTabStop.GetFill() = ' ';
                break;
            case 1:
                aTabStop.GetFill() = '.';
                break;
            case 2:
                aTabStop.GetFill() = '-';
                break;
            case 3:
            case 4:
                aTabStop.GetFill() = '_';
                break;
        }

        sal_uInt16 nPos2 = aAttr.GetPos( nPos );
        if (nPos2 != SVX_TAB_NOTFOUND)
            aAttr.Remove(nPos2, 1); // sonst weigert sich das Insert()
        aAttr.Insert(aTabStop);
    }

    if (nIns || nDel)
        NewAttr(aAttr);
    else
    {
        //Here we have a tab definition which inserts no extra tabs, or deletes
        //no existing tabs. An older version of writer is probably the creater
        //of the document  :-( . So if we are importing a style we can just
        //ignore it. But if we are importing into text we cannot as during
        //text SwWW8ImplReader::Read_Tab is called at the begin and end of
        //the range the attrib affects, and ignoring it would upset the
        //balance
        if (!pAktColl)  //not importing into a style
        {
            using namespace sw::util;
            SvxTabStopItem aOrig = pSty ?
            ItemGet<SvxTabStopItem>(*pSty, RES_PARATR_TABSTOP) :
            DefaultItemGet<SvxTabStopItem>(rDoc, RES_PARATR_TABSTOP);
            NewAttr(aOrig);
        }
    }
}

//-----------------------------------------
//              DOP
//-----------------------------------------

void SwWW8ImplReader::ImportDop()
{
    maTracer.EnterEnvironment(sw::log::eDocumentProperties);
    // correct the LastPrinted date in DocumentInfo
    uno::Reference<document::XDocumentPropertiesSupplier> xDPS(
        mpDocShell->GetModel(), uno::UNO_QUERY_THROW);
    uno::Reference<document::XDocumentProperties> xDocuProps(
        xDPS->getDocumentProperties());
    DBG_ASSERT(xDocuProps.is(), "DocumentProperties is null");
    if (xDocuProps.is())
    {
        DateTime aLastPrinted(
            sw::ms::DTTM2DateTime(pWDop->dttmLastPrint));
       ::util::DateTime uDT(aLastPrinted.Get100Sec(),
            aLastPrinted.GetSec(), aLastPrinted.GetMin(),
            aLastPrinted.GetHour(), aLastPrinted.GetDay(),
            aLastPrinted.GetMonth(), aLastPrinted.GetYear());
        xDocuProps->setPrintDate(uDT);
    }

    //
    // COMPATIBILITY FLAGS START
    //

	// #i78951# - remember the unknown compatibility options
	// so as to export them out
	rDoc.Setn32DummyCompatabilityOptions1( pWDop->GetCompatabilityOptions());
	rDoc.Setn32DummyCompatabilityOptions2( pWDop->GetCompatabilityOptions2());

    // Abstand zwischen zwei Absaetzen ist die SUMME von unterem
    // Abst. des ersten und oberem Abst. des zweiten
    rDoc.set(IDocumentSettingAccess::PARA_SPACE_MAX, pWDop->fDontUseHTMLAutoSpacing);
    rDoc.set(IDocumentSettingAccess::PARA_SPACE_MAX_AT_PAGES, true );
    maTracer.Log(sw::log::eDontUseHTMLAutoSpacing);
    // move tabs on alignment
    rDoc.set(IDocumentSettingAccess::TAB_COMPAT, true);
    // #i24363# tab stops relative to indent
    rDoc.set(IDocumentSettingAccess::TABS_RELATIVE_TO_INDENT, false);
    maTracer.Log(sw::log::eTabStopDistance);
    // #i18732# - adjust default of option 'FollowTextFlow'
    rDoc.SetDefault( SwFmtFollowTextFlow( sal_False ) );

    // Import Default-Tabs
    long nDefTabSiz = pWDop->dxaTab;
    if( nDefTabSiz < 56 )
        nDefTabSiz = 709;

    // wir wollen genau einen DefaultTab
    SvxTabStopItem aNewTab( 1, sal_uInt16(nDefTabSiz), SVX_TAB_ADJUST_DEFAULT, RES_PARATR_TABSTOP );
    ((SvxTabStop&)aNewTab[0]).GetAdjustment() = SVX_TAB_ADJUST_DEFAULT;

    rDoc.GetAttrPool().SetPoolDefaultItem( aNewTab );

    if (!pWDop->fUsePrinterMetrics)
        maTracer.Log(sw::log::ePrinterMetrics);

    if (!pWDop->fNoLeading)
        maTracer.Log(sw::log::eExtraLeading);

    rDoc.set(IDocumentSettingAccess::USE_VIRTUAL_DEVICE, !pWDop->fUsePrinterMetrics);
    rDoc.set(IDocumentSettingAccess::USE_HIRES_VIRTUAL_DEVICE, true);
    rDoc.set(IDocumentSettingAccess::ADD_FLY_OFFSETS, true );
    rDoc.set(IDocumentSettingAccess::ADD_EXT_LEADING, !pWDop->fNoLeading);

    rDoc.set(IDocumentSettingAccess::OLD_NUMBERING, false);

    // #i47448#
    rDoc.set(IDocumentSettingAccess::IGNORE_FIRST_LINE_INDENT_IN_NUMBERING, false);

    // #i49277#
    rDoc.set(IDocumentSettingAccess::DO_NOT_JUSTIFY_LINES_WITH_MANUAL_BREAK, !pWDop->fExpShRtn); // #i56856#
    // #i53199#
    rDoc.set(IDocumentSettingAccess::DO_NOT_RESET_PARA_ATTRS_FOR_NUM_FONT, false);

    rDoc.set(IDocumentSettingAccess::OLD_LINE_SPACING, false);

    // #i25901#- set new compatibility option 'Add paragraph and table spacing at bottom of table cells'
    rDoc.set(IDocumentSettingAccess::ADD_PARA_SPACING_TO_TABLE_CELLS, true);

    // #i11860# - set new compatibility option 'Use former object positioning' to <sal_False>
    rDoc.set(IDocumentSettingAccess::USE_FORMER_OBJECT_POS, false);

    // #i27767# - set new compatibility option 'Conder Wrapping mode when positioning object' to <sal_True>
    rDoc.set(IDocumentSettingAccess::CONSIDER_WRAP_ON_OBJECT_POSITION, true);

    // #i13832#, #i24135#
    rDoc.set(IDocumentSettingAccess::USE_FORMER_TEXT_WRAPPING, false);

    rDoc.set(IDocumentSettingAccess::TABLE_ROW_KEEP, true);

    // #i3952#
    rDoc.set(IDocumentSettingAccess::IGNORE_TABS_AND_BLANKS_FOR_LINE_CALCULATION, true);

    //
    // COMPATIBILITY FLAGS END
    //

    if (!pWDop->fNoLeading)
        maTracer.Log(sw::log::eExtraLeading);

    //import magic doptypography information, if its there
    if (pWwFib->nFib > 105)
        ImportDopTypography(pWDop->doptypography);

    // disable form design mode to be able to use imported controls directly
    // #i31239# always disable form design mode, not only in protected docs
    {
        using namespace com::sun::star;

        uno::Reference<lang::XComponent> xModelComp(mpDocShell->GetModel(),
           uno::UNO_QUERY);
        uno::Reference<beans::XPropertySet> xDocProps(xModelComp,
           uno::UNO_QUERY);
        if (xDocProps.is())
        {
            uno::Reference<beans::XPropertySetInfo> xInfo =
                xDocProps->getPropertySetInfo();
            sal_Bool bValue = false;
            if (xInfo.is())
            {
                if (xInfo->hasPropertyByName(C2U("ApplyFormDesignMode")))
                {
                    xDocProps->setPropertyValue(C2U("ApplyFormDesignMode"),
                                                cppu::bool2any(bValue));
                }
            }
        }
    }

    mpDocShell->SetModifyPasswordHash(pWDop->lKeyProtDoc);

    {
        const SvtFilterOptions* pOpt = SvtFilterOptions::Get();
        const sal_Bool bUseEnhFields=(pOpt && pOpt->IsUseEnhancedFields());
        if (bUseEnhFields)
        {
            rDoc.set(IDocumentSettingAccess::PROTECT_FORM, pWDop->fProtEnabled );
        }
    }

    maTracer.LeaveEnvironment(sw::log::eDocumentProperties);
}

void SwWW8ImplReader::ImportDopTypography(const WW8DopTypography &rTypo)
{
    using namespace com::sun::star;
    switch (rTypo.iLevelOfKinsoku)
    {
        case 2: //custom
            {
                i18n::ForbiddenCharacters aForbidden(rTypo.rgxchFPunct,
                    rTypo.rgxchLPunct);
                rDoc.setForbiddenCharacters(rTypo.GetConvertedLang(),
                        aForbidden);
                //Obviously cannot set the standard level 1 for japanese, so
                //bail out now while we can.
                if (rTypo.GetConvertedLang() == LANGUAGE_JAPANESE)
                    return;
            }
            break;
        default:
            break;
    }

    /*
    This MS hack means that level 2 of japanese is not in operation, so we put
    in what we know are the MS defaults, there is a complementary reverse
    hack in the writer. Its our default as well, but we can set it anyway
    as a flag for later.
    */
    if (!rTypo.reserved2)
    {
        i18n::ForbiddenCharacters aForbidden(rTypo.GetJapanNotBeginLevel1(),
            rTypo.GetJapanNotEndLevel1());
        rDoc.setForbiddenCharacters(LANGUAGE_JAPANESE,aForbidden);
    }

    rDoc.set(IDocumentSettingAccess::KERN_ASIAN_PUNCTUATION, rTypo.fKerningPunct);
    rDoc.setCharacterCompressionType(static_cast<SwCharCompressType>(rTypo.iJustification));
}

//-----------------------------------------
//      Fuss- und Endnoten

//-----------------------------------------

WW8ReaderSave::WW8ReaderSave(SwWW8ImplReader* pRdr ,WW8_CP nStartCp) :
    maTmpPos(*pRdr->pPaM->GetPoint()),
	mpOldStck(pRdr->pCtrlStck),
    mpOldAnchorStck(pRdr->pAnchorStck),
	mpOldRedlines(pRdr->mpRedlineStack),
    mpOldPlcxMan(pRdr->pPlcxMan),
	mpWFlyPara(pRdr->pWFlyPara),
    mpSFlyPara(pRdr->pSFlyPara),
	mpPreviousNumPaM(pRdr->pPreviousNumPaM),
    mpPrevNumRule(pRdr->pPrevNumRule),
	mpTableDesc(pRdr->pTableDesc),
    mnInTable(pRdr->nInTable),
	mnAktColl(pRdr->nAktColl),
    mcSymbol(pRdr->cSymbol),
    mbIgnoreText(pRdr->bIgnoreText),
	mbSymbol(pRdr->bSymbol),
    mbHdFtFtnEdn(pRdr->bHdFtFtnEdn),
	mbTxbxFlySection(pRdr->bTxbxFlySection),
    mbAnl(pRdr->bAnl),
	mbInHyperlink(pRdr->bInHyperlink),
    mbPgSecBreak(pRdr->bPgSecBreak),
	mbWasParaEnd(pRdr->bWasParaEnd),
    mbHasBorder(pRdr->bHasBorder),
    mbFirstPara(pRdr->bFirstPara)
{
    pRdr->bSymbol = false;
    pRdr->bHdFtFtnEdn = true;
    pRdr->bTxbxFlySection = pRdr->bAnl = pRdr->bPgSecBreak = pRdr->bWasParaEnd
        = pRdr->bHasBorder = false;
    pRdr->bFirstPara = true;
    pRdr->nInTable = 0;
    pRdr->pWFlyPara = 0;
    pRdr->pSFlyPara = 0;
    pRdr->pPreviousNumPaM = 0;
    pRdr->pPrevNumRule = 0;
    pRdr->pTableDesc = 0;
    pRdr->nAktColl = 0;


    pRdr->pCtrlStck = new SwWW8FltControlStack(&pRdr->rDoc, pRdr->nFieldFlags,
        *pRdr);

    pRdr->mpRedlineStack = new sw::util::RedlineStack(pRdr->rDoc);

    pRdr->pAnchorStck = new SwWW8FltAnchorStack(&pRdr->rDoc, pRdr->nFieldFlags);

    // rette die Attributverwaltung: dies ist noetig, da der neu anzulegende
    // PLCFx Manager natuerlich auf die gleichen FKPs zugreift, wie der alte
    // und deren Start-End-Positionen veraendert...
    if (pRdr->pPlcxMan)
        pRdr->pPlcxMan->SaveAllPLCFx(maPLCFxSave);

    if (nStartCp != -1)
    {
        pRdr->pPlcxMan = new WW8PLCFMan(pRdr->pSBase,
            mpOldPlcxMan->GetManType(), nStartCp);
    }

    maOldApos.push_back(false);
    maOldApos.swap(pRdr->maApos);
    maOldFieldStack.swap(pRdr->maFieldStack);
}

void WW8ReaderSave::Restore( SwWW8ImplReader* pRdr )
{
    pRdr->pWFlyPara = mpWFlyPara;
    pRdr->pSFlyPara = mpSFlyPara;
    pRdr->pPreviousNumPaM = mpPreviousNumPaM;
    pRdr->pPrevNumRule = mpPrevNumRule;
    pRdr->pTableDesc = mpTableDesc;
    pRdr->cSymbol = mcSymbol;
    pRdr->bSymbol = mbSymbol;
    pRdr->bIgnoreText = mbIgnoreText;
    pRdr->bHdFtFtnEdn = mbHdFtFtnEdn;
    pRdr->bTxbxFlySection = mbTxbxFlySection;
    pRdr->nInTable = mnInTable;
    pRdr->bAnl = mbAnl;
    pRdr->bInHyperlink = mbInHyperlink;
    pRdr->bWasParaEnd = mbWasParaEnd;
    pRdr->bPgSecBreak = mbPgSecBreak;
    pRdr->nAktColl = mnAktColl;
    pRdr->bHasBorder = mbHasBorder;
    pRdr->bFirstPara = mbFirstPara;

    // schliesse alle Attribute, da sonst Attribute
    // entstehen koennen, die aus dem Fly rausragen
    pRdr->DeleteCtrlStk();
    pRdr->pCtrlStck = mpOldStck;

    pRdr->mpRedlineStack->closeall(*pRdr->pPaM->GetPoint());
    delete pRdr->mpRedlineStack;
    pRdr->mpRedlineStack = mpOldRedlines;

    pRdr->DeleteAnchorStk();
    pRdr->pAnchorStck = mpOldAnchorStck;

    *pRdr->pPaM->GetPoint() = maTmpPos;

    if (mpOldPlcxMan != pRdr->pPlcxMan)
    {
        delete pRdr->pPlcxMan;
        pRdr->pPlcxMan = mpOldPlcxMan;
    }
    if (pRdr->pPlcxMan)
        pRdr->pPlcxMan->RestoreAllPLCFx(maPLCFxSave);
    pRdr->maApos.swap(maOldApos);
    pRdr->maFieldStack.swap(maOldFieldStack);
}

void SwWW8ImplReader::Read_HdFtFtnText( const SwNodeIndex* pSttIdx,
    long nStartCp, long nLen, ManTypes nType )
{
    // rettet Flags u.ae. u. setzt sie zurueck
    WW8ReaderSave aSave( this );

    pPaM->GetPoint()->nNode = pSttIdx->GetIndex() + 1;      //
    pPaM->GetPoint()->nContent.Assign( pPaM->GetCntntNode(), 0 );

    // dann Text fuer Header, Footer o. Footnote einlesen

    ReadText( nStartCp, nLen, nType );              // Sepx dabei ignorieren
    aSave.Restore( this );
}

//Use authornames, if not available fall back to initials.
long SwWW8ImplReader::Read_And(WW8PLCFManResult* pRes)
{
    WW8PLCFx_SubDoc* pSD = pPlcxMan->GetAtn();
    if( !pSD )
        return 0;

    String sAuthor;
    if( bVer67 )
    {
        const WW67_ATRD* pDescri = (const WW67_ATRD*)pSD->GetData();
        const String* pA = GetAnnotationAuthor(SVBT16ToShort(pDescri->ibst));
        if (pA)
            sAuthor = *pA;
        else
            sAuthor = String(pDescri->xstUsrInitl + 1, pDescri->xstUsrInitl[0],
                RTL_TEXTENCODING_MS_1252);
    }
    else
    {
        const WW8_ATRD* pDescri = (const WW8_ATRD*)pSD->GetData();

        if (const String* pA = GetAnnotationAuthor(SVBT16ToShort(pDescri->ibst)))
            sAuthor = *pA;
        else
        {
            sal_uInt16 nLen = SVBT16ToShort(pDescri->xstUsrInitl[0]);
            for(sal_uInt16 nIdx = 1; nIdx <= nLen; ++nIdx)
                sAuthor += SVBT16ToShort(pDescri->xstUsrInitl[nIdx]);
        }
    }

    sal_uInt32 nDateTime = 0;

    if (sal_uInt8 * pExtended = pPlcxMan->GetExtendedAtrds()) // Word < 2002 has no date data for comments
    {
        sal_uLong nIndex = pSD->GetIdx() & 0xFFFF; //Index is (stupidly) multiplexed for WW8PLCFx_SubDocs
        if (pWwFib->lcbAtrdExtra/18 > nIndex)
            nDateTime = SVBT32ToUInt32(*(SVBT32*)(pExtended+(nIndex*18)));
    }

    DateTime aDate = sw::ms::DTTM2DateTime(nDateTime);

    String sTxt;
    OutlinerParaObject *pOutliner = ImportAsOutliner( sTxt, pRes->nCp2OrIdx,
        pRes->nCp2OrIdx + pRes->nMemLen, MAN_AND );

    this->pFmtOfJustInsertedApo = 0;
    SwPostItField aPostIt(
        (SwPostItFieldType*)rDoc.GetSysFldType(RES_POSTITFLD),
        sTxt,
        sAuthor,
        aEmptyStr,
        aEmptyStr,
        aDate );
    aPostIt.SetTextObject(pOutliner);

    pCtrlStck->NewAttr(*pPaM->GetPoint(), SvxCharHiddenItem(false, RES_CHRATR_HIDDEN));
    rDoc.InsertPoolItem(*pPaM, SwFmtFld(aPostIt), 0);
    pCtrlStck->SetAttr(*pPaM->GetPoint(), RES_CHRATR_HIDDEN);

    return 0;
}

void SwWW8ImplReader::Read_HdFtTextAsHackedFrame(long nStart, long nLen,
    SwFrmFmt &rHdFtFmt, sal_uInt16 nPageWidth)
{
    const SwNodeIndex* pSttIdx = rHdFtFmt.GetCntnt().GetCntntIdx();
    ASSERT(pSttIdx, "impossible");
    if (!pSttIdx)
        return;

    SwPosition aTmpPos(*pPaM->GetPoint());

    pPaM->GetPoint()->nNode = pSttIdx->GetIndex() + 1;
    pPaM->GetPoint()->nContent.Assign(pPaM->GetCntntNode(), 0);

    SwFlyFrmFmt *pFrame = rDoc.MakeFlySection(FLY_AT_PARA, pPaM->GetPoint());

    SwFmtAnchor aAnch( pFrame->GetAnchor() );
    aAnch.SetType( FLY_AT_PARA );
    pFrame->SetFmtAttr( aAnch );
    SwFmtFrmSize aSz(ATT_MIN_SIZE, nPageWidth, MINLAY);
    SwFrmSize eFrmSize = ATT_MIN_SIZE;
    if( eFrmSize != aSz.GetWidthSizeType() )
        aSz.SetWidthSizeType( eFrmSize );
    pFrame->SetFmtAttr(aSz);
    pFrame->SetFmtAttr(SwFmtSurround(SURROUND_THROUGHT));
    pFrame->SetFmtAttr(SwFmtHoriOrient(0, text::HoriOrientation::LEFT)); //iFOO
 
    // #i43427# - send frame for header/footer into background.
    pFrame->SetFmtAttr( SvxOpaqueItem( RES_OPAQUE, false ) );
    SdrObject* pFrmObj = CreateContactObject( pFrame );
    ASSERT( pFrmObj,
            "<SwWW8ImplReader::Read_HdFtTextAsHackedFrame(..)> - missing SdrObject instance" );
    if ( pFrmObj )
    {
        pFrmObj->SetOrdNum( 0L );
    }
    MoveInsideFly(pFrame);

    const SwNodeIndex* pHackIdx = pFrame->GetCntnt().GetCntntIdx();

    Read_HdFtFtnText(pHackIdx, nStart, nLen - 1, MAN_HDFT);

    MoveOutsideFly(pFrame, aTmpPos);
}

void SwWW8ImplReader::Read_HdFtText(long nStart, long nLen, SwFrmFmt* pHdFtFmt)
{
    const SwNodeIndex* pSttIdx = pHdFtFmt->GetCntnt().GetCntntIdx();
    if (!pSttIdx)
        return;

    SwPosition aTmpPos( *pPaM->GetPoint() );    // merke alte Cursorposition

    Read_HdFtFtnText(pSttIdx, nStart, nLen - 1, MAN_HDFT);

    *pPaM->GetPoint() = aTmpPos;
}


bool SwWW8ImplReader::isValid_HdFt_CP(WW8_CP nHeaderCP) const
{
    //each CP of Plcfhdd MUST be less than FibRgLw97.ccpHdd
    return (nHeaderCP < pWwFib->ccpHdr) ? true : false;
}

bool SwWW8ImplReader::HasOwnHeaderFooter(sal_uInt8 nWhichItems, sal_uInt8 grpfIhdt,
    int nSect)
{
    if (pHdFt)
    {
        WW8_CP start;
        long nLen;
        sal_uInt8 nNumber = 5;

        for( sal_uInt8 nI = 0x20; nI; nI >>= 1, nNumber-- )
        {
            if (nI & nWhichItems)
            {
                bool bOk = true;
                if( bVer67 )
                    bOk = ( pHdFt->GetTextPos(grpfIhdt, nI, start, nLen ) && nLen >= 2 );
                else
                {
                    pHdFt->GetTextPosExact( static_cast< short >(nNumber + (nSect+1)*6), start, nLen);
                    bOk = ( 2 <= nLen ) && isValid_HdFt_CP(start);
                }

                if (bOk)
                    return true;
            }
        }
    }
    return false;
}

void SwWW8ImplReader::Read_HdFt(bool bIsTitle, int nSect,
    const SwPageDesc *pPrev, const wwSection &rSection)
{
    sal_uInt8 nWhichItems = 0;
    SwPageDesc *pPD = 0;
    if (!bIsTitle)
    {
        nWhichItems =
            rSection.maSep.grpfIhdt & ~(WW8_HEADER_FIRST | WW8_FOOTER_FIRST);
        pPD = rSection.mpPage;
    }
    else
    {
        // Always read title page header/footer data - it could be used by following sections
        nWhichItems = ( WW8_HEADER_FIRST | WW8_FOOTER_FIRST );
        pPD = rSection.mpTitlePage;
    }

    sal_uInt8 grpfIhdt = rSection.maSep.grpfIhdt;


    if( pHdFt )
    {
        WW8_CP start;
        long nLen;
        sal_uInt8 nNumber = 5;

        for( sal_uInt8 nI = 0x20; nI; nI >>= 1, nNumber-- )
        {
            if (nI & nWhichItems)
            {
                bool bOk = true;
                if( bVer67 )
                    bOk = ( pHdFt->GetTextPos(grpfIhdt, nI, start, nLen ) && nLen >= 2 );
                else
                {
                    pHdFt->GetTextPosExact( static_cast< short >(nNumber + (nSect+1)*6), start, nLen);
                    bOk = ( 2 <= nLen ) && isValid_HdFt_CP(start);
                }

                bool bUseLeft
                    = (nI & ( WW8_HEADER_EVEN | WW8_FOOTER_EVEN )) ? true: false;
                bool bFooter
                    = (nI & ( WW8_FOOTER_EVEN | WW8_FOOTER_ODD | WW8_FOOTER_FIRST )) ? true: false;

                SwFrmFmt* pFmt = bUseLeft ? &pPD->GetLeft() : &pPD->GetMaster();

                SwFrmFmt* pHdFtFmt;
                if (bFooter)
                {
                    bIsFooter = true;
                    //#i17196# Cannot have left without right
                    if (!pPD->GetMaster().GetFooter().GetFooterFmt())
                        pPD->GetMaster().SetFmtAttr(SwFmtFooter(true));
                    if (bUseLeft)
                        pPD->GetLeft().SetFmtAttr(SwFmtFooter(true));
                    pHdFtFmt = (SwFrmFmt*)pFmt->GetFooter().GetFooterFmt();
                }
                else
                {
                    bIsHeader = true;
                    //#i17196# Cannot have left without right
                    if (!pPD->GetMaster().GetHeader().GetHeaderFmt())
                        pPD->GetMaster().SetFmtAttr(SwFmtHeader(true));
                    if (bUseLeft)
                        pPD->GetLeft().SetFmtAttr(SwFmtHeader(true));
                    pHdFtFmt = (SwFrmFmt*)pFmt->GetHeader().GetHeaderFmt();
                }

                if (bOk)
                {
                    bool bHackRequired = false;
                    if (bIsHeader && rSection.IsFixedHeightHeader())
                        bHackRequired = true;
                    else if (bIsFooter && rSection.IsFixedHeightFooter())
                        bHackRequired = true;

                    if (bHackRequired)
                    {
                        Read_HdFtTextAsHackedFrame(start, nLen, *pHdFtFmt,
                            static_cast< sal_uInt16 >(rSection.GetTextAreaWidth()) );
                    }
                    else
                        Read_HdFtText(start, nLen, pHdFtFmt);
                }
                else if (!bOk && pPrev)
                    CopyPageDescHdFt(pPrev, pPD, nI);

                bIsHeader = bIsFooter = false;
            }
        }
    }
    maTracer.LeaveEnvironment(sw::log::eDocumentProperties);
}

bool wwSectionManager::SectionIsProtected(const wwSection &rSection) const
{
    return (mrReader.pWwFib->fReadOnlyRecommended && !rSection.IsNotProtected());
}

void wwSectionManager::SetHdFt(wwSection &rSection, int nSect,
    const wwSection *pPrevious)
{
    // Header / Footer nicht da
    if (!rSection.maSep.grpfIhdt)
        return;

    ASSERT(rSection.mpPage, "makes no sense to call with a main page");
    if (rSection.mpPage)
    {
        mrReader.Read_HdFt(false, nSect, pPrevious ? pPrevious->mpPage : 0,
                rSection);
    }

    if (rSection.mpTitlePage)
    {
        // 2 Pagedescs noetig: 1.Seite und folgende
        // 1. Seite einlesen
        mrReader.Read_HdFt(true, nSect, pPrevious ? pPrevious->mpTitlePage : 0,
            rSection);
    }

    // Kopf / Fuss - Index Updaten
    // Damit der Index auch spaeter noch stimmt
    if (mrReader.pHdFt)
        mrReader.pHdFt->UpdateIndex(rSection.maSep.grpfIhdt);

}

class AttribHere : public std::unary_function<const xub_StrLen*, bool>
{
private:
    xub_StrLen nPosition;
public:
    AttribHere(xub_StrLen nPos) : nPosition(nPos) {}
    bool operator()(const xub_StrLen *pPosition) const
    {
        return (*pPosition >= nPosition);
    }
};

void SwWW8ImplReader::AppendTxtNode(SwPosition& rPos)
{
    SwTxtNode* pTxt = pPaM->GetNode()->GetTxtNode();

    const SwNumRule* pRule = NULL;

    if (pTxt != NULL)
        pRule = sw::util::GetNumRuleFromTxtNode(*pTxt);

    if (
         pRule && !pWDop->fDontUseHTMLAutoSpacing &&
         (bParaAutoBefore || bParaAutoAfter)
       )
    {
        // If after spacing is set to auto, set the after space to 0
        if (bParaAutoAfter)
            SetLowerSpacing(*pPaM, 0);

        // If the previous textnode had numbering and
        // and before spacing is set to auto, set before space to 0
        if(pPrevNumRule && bParaAutoBefore)
            SetUpperSpacing(*pPaM, 0);

        // If the previous numbering rule was different we need
        // to insert a space after the previous paragraph
        if((pRule != pPrevNumRule) && pPreviousNumPaM)
            SetLowerSpacing(*pPreviousNumPaM, GetParagraphAutoSpace(pWDop->fDontUseHTMLAutoSpacing));

        // cache current paragraph
        if(pPreviousNumPaM)
            delete pPreviousNumPaM, pPreviousNumPaM = 0;

        pPreviousNumPaM = new SwPaM(*pPaM);
        pPrevNumRule = pRule;
    }
    else if(!pRule && pPreviousNumPaM)
    {
        // If the previous paragraph has numbering but the current one does not
        // we need to add a space after the previous paragraph
        SetLowerSpacing(*pPreviousNumPaM, GetParagraphAutoSpace(pWDop->fDontUseHTMLAutoSpacing));
        delete pPreviousNumPaM, pPreviousNumPaM = 0;
        pPrevNumRule = 0;
    }
    else
    {
        // clear paragraph cache
        if(pPreviousNumPaM)
            delete pPreviousNumPaM, pPreviousNumPaM = 0;
        pPrevNumRule = pRule;
    }

    // If this is the first paragraph in the document and
    // Auto-spacing before paragraph is set,
    // set the upper spacing value to 0
    if(bParaAutoBefore && bFirstPara && !pWDop->fDontUseHTMLAutoSpacing)
        SetUpperSpacing(*pPaM, 0);

    bFirstPara = false;

    rDoc.AppendTxtNode(rPos);

    //We can flush all anchored graphics at the end of a paragraph.
    pAnchorStck->Flush();
}

bool SwWW8ImplReader::SetSpacing(SwPaM &rMyPam, int nSpace, bool bIsUpper )
{
        bool bRet = false;
        const SwPosition* pSpacingPos = rMyPam.GetPoint();

        const SvxULSpaceItem* pULSpaceItem = (const SvxULSpaceItem*)pCtrlStck->GetFmtAttr(*pSpacingPos, RES_UL_SPACE);

        if(pULSpaceItem != 0)
        {
            SvxULSpaceItem aUL(*pULSpaceItem);

            if(bIsUpper)
                aUL.SetUpper( static_cast< sal_uInt16 >(nSpace) );
            else
                aUL.SetLower( static_cast< sal_uInt16 >(nSpace) );

            xub_StrLen nEnd = pSpacingPos->nContent.GetIndex();
            rMyPam.GetPoint()->nContent.Assign(rMyPam.GetCntntNode(), 0);
            pCtrlStck->NewAttr(*pSpacingPos, aUL);
            rMyPam.GetPoint()->nContent.Assign(rMyPam.GetCntntNode(), nEnd);
            pCtrlStck->SetAttr(*pSpacingPos, RES_UL_SPACE);
            bRet = true;
        }
        return bRet;
}

bool SwWW8ImplReader::SetLowerSpacing(SwPaM &rMyPam, int nSpace)
{
    return SetSpacing(rMyPam, nSpace, false);
}

bool SwWW8ImplReader::SetUpperSpacing(SwPaM &rMyPam, int nSpace)
{
    return SetSpacing(rMyPam, nSpace, true);
}

sal_uInt16 SwWW8ImplReader::TabRowSprm(int nLevel) const
{
    if (bVer67)
        return 25;
    return nLevel ? 0x244C : 0x2417;
}

void SwWW8ImplReader::EndSpecial()
{
    // Frame / Table / Anl
    if (bAnl)
        StopAllAnl();                  // -> bAnl = false

    while(maApos.size() > 1)
    {
        StopTable();
        maApos.pop_back();
        --nInTable;
        if (maApos[nInTable] == true)
            StopApo();
    }

    if (maApos[0] == true)
        StopApo();

    ASSERT(!nInTable, "unclosed table!");
}

bool SwWW8ImplReader::ProcessSpecial(bool &rbReSync, WW8_CP nStartCp)
{
    // Frame / Table / Anl
    if (bInHyperlink)
        return false;

    rbReSync = false;

    ASSERT(nInTable >= 0,"nInTable < 0!");

    // TabRowEnd
    bool bTableRowEnd = (pPlcxMan->HasParaSprm(bVer67 ? 25 : 0x2417) != 0 );

// es muss leider fuer jeden Absatz zuerst nachgesehen werden,
// ob sich unter den sprms
// das sprm 29 (bzw. 0x261B) befindet, das ein APO einleitet.
// Alle weiteren sprms  beziehen sich dann naemlich auf das APO und nicht
// auf den normalen Text drumrum.
// Dasselbe gilt fuer eine Tabelle ( sprm 24 (bzw. 0x2416) )
// und Anls ( sprm 13 ).
// WW: Tabelle in APO geht ( Beide Anfaende treten gleichzeitig auf )
// WW: APO in Tabelle geht nicht
// d.h. Wenn eine Tabelle Inhalt eines Apo ist, dann muss der
// Apo-Anfang zuerst bearbeitet werden, damit die Tabelle im Apo steht
// und nicht umgekehrt. Am Ende muss dagegen zuerst das Tabellenende
// bearbeitet werden, da die Apo erst nach der Tabelle abgeschlossen
// werden darf ( sonst wird das Apo-Ende nie gefunden ).
// Dasselbe gilt fuer Fly / Anl, Tab / Anl, Fly / Tab / Anl.
//
// Wenn die Tabelle in einem Apo steht, fehlen im TabRowEnd-Bereich
// die Apo-Angaben. Damit hier die Apo nicht beendet wird, wird
// ProcessApo dann nicht aufgerufen.

// KHZ: When there is a table inside the Apo the Apo-flags are also
//      missing for the 2nd, 3rd... paragraphs of each cell.


//  1st look for in-table flag, for 2000+ there is a subtable flag to
//  be considered, the sprm 6649 gives the level of the table
    sal_uInt8 nCellLevel = 0;

    if (bVer67)
        nCellLevel = 0 != pPlcxMan->HasParaSprm(24);
    else
    {
        nCellLevel = 0 != pPlcxMan->HasParaSprm(0x2416);
        if (!nCellLevel)
            nCellLevel = 0 != pPlcxMan->HasParaSprm(0x244B);
    }

    WW8_TablePos *pTabPos=0;
    WW8_TablePos aTabPos;
    WW8PLCFx_Cp_FKP* pPap = 0;
    bool bTableHasPositionInfo = false;
    
    if (nCellLevel && !bVer67)
    {
        WW8PLCFxSave1 aSave;
        pPlcxMan->GetPap()->Save( aSave );
        rbReSync = true;
        pPap = pPlcxMan->GetPapPLCF();
        WW8_CP nMyStartCp=nStartCp;

        if (const sal_uInt8 *pLevel = pPlcxMan->HasParaSprm(0x6649))
            nCellLevel = *pLevel;

        bool bHasRowEnd = SearchRowEnd(pPap, nMyStartCp, nCellLevel-1);

        //Bad Table, remain unchanged in level, e.g. #i19667#
        if (!bHasRowEnd)
            nCellLevel = static_cast< sal_uInt8 >(nInTable);

        sal_uLong idstart = rDoc.GetNodes().GetEndOfContent().StartOfSectionIndex(); // get the node index
        sal_uLong idcur = 0;
        if ( pPaM && pPaM->GetPoint() )
            idcur = pPaM->GetPoint()->nNode.GetIndex();
    
        // Memory first table position info
        if ( !pFirstTablePap && idstart + 1 == idcur )
            pFirstTablePap = pPap;

        if ( bHasRowEnd  && ParseTabPos(&aTabPos,pPap) )
        {
            // If table front don't have some content and it is doc first table, ignore table text wrapping property
            bTableHasPositionInfo = true;
            if ( pFirstTablePap != pPap )
                pTabPos = &aTabPos;
        }

        pPlcxMan->GetPap()->Restore( aSave );
    }

//  then look if we are in an Apo

    ApoTestResults aApo = TestApo(nCellLevel, bTableRowEnd, pTabPos, !(pFirstTablePap == pPap && bTableHasPositionInfo));

    //look to see if we are in a Table, but Table in foot/end note not allowed
    bool bStartTab = (nInTable < nCellLevel) && !bFtnEdn;

    bool bStopTab = bWasTabRowEnd && (nInTable > nCellLevel) && !bFtnEdn;

    bWasTabRowEnd = false;  // must be deactivated right here to prevent next
                            // WW8TabDesc::TableCellEnd() from making nonsense

    if (nInTable && !bTableRowEnd && !bStopTab && (nInTable == nCellLevel && aApo.HasStartStop()))
        bStopTab = bStartTab = true;    // Required to stop and start table

//  Dann auf Anl (Nummerierung) testen
//  und dann alle Ereignisse in der richtigen Reihenfolge bearbeiten

    if( bAnl && !bTableRowEnd )
    {
        const sal_uInt8* pSprm13 = pPlcxMan->HasParaSprm( 13 );
        if( pSprm13 )
        {                                   // Noch Anl ?
            sal_uInt8 nT = static_cast< sal_uInt8 >(GetNumType( *pSprm13 ));
            if( ( nT != WW8_Pause && nT != nWwNumType ) // Anl-Wechsel
                || aApo.HasStartStop()                  // erzwungenes Anl-Ende
                || bStopTab || bStartTab )
            {
                StopAnlToRestart(nT);  // Anl-Restart ( = Wechsel ) ueber sprms
            }
            else
            {
                NextAnlLine( pSprm13 );                 // naechste Anl-Zeile
            }
        }
        else
        {                                           // Anl normal zuende
            StopAllAnl();                                  // Wirkliches Ende
        }
    }
    if (bStopTab)
    {
        StopTable();
        maApos.pop_back();
        --nInTable;
    }
    if (aApo.mbStopApo)
    {
        StopApo();
        maApos[nInTable] = false;
    }

    if (aApo.mbStartApo)
    {
        maApos[nInTable] = StartApo(aApo, pTabPos);
        // nach StartApo ist ein ReSync noetig ( eigentlich nur, falls die Apo
        // ueber eine FKP-Grenze geht
        rbReSync = true;
    }
    if (bStartTab)
    {
        WW8PLCFxSave1 aSave;
        pPlcxMan->GetPap()->Save( aSave );

        if (bAnl)                           // Nummerierung ueber Zellengrenzen
            StopAllAnl();                   // fuehrt zu Absturz -> keine Anls
                                            // in Tabellen
        while (nInTable < nCellLevel)
        {
            if (StartTable(nStartCp))
                ++nInTable;
            else
                break;

            maApos.push_back(false);
        }
        // nach StartTable ist ein ReSync noetig ( eigentlich nur, falls die
        // Tabelle ueber eine FKP-Grenze geht
        rbReSync = true;
        pPlcxMan->GetPap()->Restore( aSave );
    }
    return bTableRowEnd;
}

CharSet SwWW8ImplReader::GetCurrentCharSet()
{
    /*
    #i2015
    If the hard charset is set use it, if not see if there is an open
    character run that has set the charset, if not then fallback to the
    current underlying paragraph style.
    */
    CharSet eSrcCharSet = eHardCharSet;
    if (eSrcCharSet == RTL_TEXTENCODING_DONTKNOW)
    {
        if (!maFontSrcCharSets.empty())
            eSrcCharSet = maFontSrcCharSets.top();
        if ((eSrcCharSet == RTL_TEXTENCODING_DONTKNOW) && (nCharFmt != -1))
            eSrcCharSet = pCollA[nCharFmt].GetCharSet();
        if ((eSrcCharSet == RTL_TEXTENCODING_DONTKNOW) && StyleExists(nAktColl))
            eSrcCharSet = pCollA[nAktColl].GetCharSet();
        if (eSrcCharSet == RTL_TEXTENCODING_DONTKNOW)
        { // patch from cmc for #i52786#
            /*
             #i22206#/#i52786#
             The (default) character set used for a run of text is the default
             character set for the version of Word that last saved the document.

             This is a bit tentative, more might be required if the concept is correct.
             When later version of word write older 6/95 documents the charset is
             correctly set in the character runs involved, so its hard to reproduce
             documents that require this to be sure of the process involved.
            */
            const SvxLanguageItem *pLang =
                (const SvxLanguageItem*)GetFmtAttr(RES_CHRATR_LANGUAGE);
            if (pLang)
            {
                switch (pLang->GetLanguage())
                {
                    case LANGUAGE_CZECH:
                    case LANGUAGE_HUNGARIAN:
                    case LANGUAGE_POLISH:
                        eSrcCharSet = RTL_TEXTENCODING_MS_1250;
                        break;
                    case LANGUAGE_RUSSIAN:
                        eSrcCharSet = RTL_TEXTENCODING_MS_1251;
                        break;
                    case LANGUAGE_GREEK:
                        eSrcCharSet = RTL_TEXTENCODING_MS_1253;
                        break;
                    case LANGUAGE_TURKISH:
                        eSrcCharSet = RTL_TEXTENCODING_MS_1254;
                        break;
                    default:
                        eSrcCharSet = RTL_TEXTENCODING_MS_1252;
                        break;
                }
            }
        }
    }
    return eSrcCharSet;
}

//Takashi Ono for CJK
CharSet SwWW8ImplReader::GetCurrentCJKCharSet()
{
    /*
    #i2015
    If the hard charset is set use it, if not see if there is an open
    character run that has set the charset, if not then fallback to the
    current underlying paragraph style.
    */
    CharSet eSrcCharSet = eHardCharSet;
    if (eSrcCharSet == RTL_TEXTENCODING_DONTKNOW)
    {
        if (!maFontSrcCJKCharSets.empty())
            eSrcCharSet = maFontSrcCJKCharSets.top();
        if (pCollA != NULL)
        {
            if ((eSrcCharSet == RTL_TEXTENCODING_DONTKNOW) && (nCharFmt != -1))
                eSrcCharSet = pCollA[nCharFmt].GetCJKCharSet();
            if (eSrcCharSet == RTL_TEXTENCODING_DONTKNOW)
                eSrcCharSet = pCollA[nAktColl].GetCJKCharSet();
        }
        if (eSrcCharSet == RTL_TEXTENCODING_DONTKNOW)
        { // patch from cmc for #i52786#
            /*
             #i22206#/#i52786#
             The (default) character set used for a run of text is the default
             character set for the version of Word that last saved the document.

             This is a bit tentative, more might be required if the concept is correct.
             When later version of word write older 6/95 documents the charset is
             correctly set in the character runs involved, so its hard to reproduce
             documents that require this to be sure of the process involved.
            */
            const SvxLanguageItem *pLang =
                (const SvxLanguageItem*)GetFmtAttr(RES_CHRATR_LANGUAGE);
            if (pLang)
            {
                switch (pLang->GetLanguage())
                {
                    case LANGUAGE_CZECH:
                        eSrcCharSet = RTL_TEXTENCODING_MS_1250;
                        break;
                    default:
                        eSrcCharSet = RTL_TEXTENCODING_MS_1252;
                        break;
                }
            }
        }
    }
    return eSrcCharSet;
}

void SwWW8ImplReader::PostProcessAttrs()
{
    if (mpPostProcessAttrsInfo != NULL)
    {
        SfxItemIter aIter(mpPostProcessAttrsInfo->mItemSet);

        const SfxPoolItem * pItem = aIter.GetCurItem();
        if (pItem != NULL)
        {
            do
            {
                pCtrlStck->NewAttr(*mpPostProcessAttrsInfo->mPaM.GetPoint(),
                                   *pItem);
                pCtrlStck->SetAttr(*mpPostProcessAttrsInfo->mPaM.GetMark(),
                                   pItem->Which(), true);
            }
            while (!aIter.IsAtEnd() && 0 != (pItem = aIter.NextItem()));
        }

        delete mpPostProcessAttrsInfo;
        mpPostProcessAttrsInfo = NULL;
    }
}

/*
 #i9241#
 It appears that some documents that are in a baltic 8 bit encoding which has
 some undefined characters can have use made of those characters, in which
 case they default to CP1252. If not then its perhaps that the font encoding
 is only in use for 6/7 and for 8+ if we are in 8bit mode then the encoding
 is always 1252.

 So a encoding converter that on an undefined character attempts to
 convert from 1252 on the undefined character
*/
sal_Size Custom8BitToUnicode(rtl_TextToUnicodeConverter hConverter,
    sal_Char *pIn, sal_Size nInLen, sal_Unicode *pOut, sal_Size nOutLen)
{
    const sal_uInt32 nFlags =
        RTL_TEXTTOUNICODE_FLAGS_UNDEFINED_ERROR |
        RTL_TEXTTOUNICODE_FLAGS_MBUNDEFINED_ERROR |
        RTL_TEXTTOUNICODE_FLAGS_INVALID_IGNORE |
        RTL_TEXTTOUNICODE_FLAGS_FLUSH;

    const sal_uInt32 nFlags2 =
        RTL_TEXTTOUNICODE_FLAGS_UNDEFINED_IGNORE |
        RTL_TEXTTOUNICODE_FLAGS_MBUNDEFINED_IGNORE |
        RTL_TEXTTOUNICODE_FLAGS_INVALID_IGNORE |
        RTL_TEXTTOUNICODE_FLAGS_FLUSH;

    sal_Size nDestChars=0;
    sal_Size nConverted=0;

    do
    {
        sal_uInt32 nInfo = 0;
        sal_Size nThisConverted=0;

        nDestChars += rtl_convertTextToUnicode(hConverter, 0,
            pIn+nConverted, nInLen-nConverted,
            pOut+nDestChars, nOutLen-nDestChars,
            nFlags, &nInfo, &nThisConverted);

        ASSERT(nInfo == 0, "A character conversion failed!");

        nConverted += nThisConverted;

        if (
            nInfo & RTL_TEXTTOUNICODE_FLAGS_UNDEFINED_ERROR ||
            nInfo & RTL_TEXTTOUNICODE_FLAGS_MBUNDEFINED_ERROR
           )
        {
            sal_Size nOtherConverted;
            rtl_TextToUnicodeConverter hCP1252Converter =
                rtl_createTextToUnicodeConverter(RTL_TEXTENCODING_MS_1252);
            nDestChars += rtl_convertTextToUnicode(hCP1252Converter, 0,
                pIn+nConverted, 1,
                pOut+nDestChars, nOutLen-nDestChars,
                nFlags2, &nInfo, &nOtherConverted);
            rtl_destroyTextToUnicodeConverter(hCP1252Converter);
            nConverted+=1;
        }
    } while (nConverted < nInLen);

    return nDestChars;
}

bool SwWW8ImplReader::LangUsesHindiNumbers(sal_uInt16 nLang)
{
	bool bResult = false;

	switch (nLang)
	{
		case 0x1401: // Arabic(Algeria)
		case 0x3c01: // Arabic(Bahrain)
		case 0xc01: // Arabic(Egypt)
		case 0x801: // Arabic(Iraq)
		case 0x2c01: // Arabic (Jordan)
		case 0x3401: // Arabic(Kuwait)
		case 0x3001: // Arabic(Lebanon)
		case 0x1001: // Arabic(Libya)
		case 0x1801: // Arabic(Morocco)
		case 0x2001: // Arabic(Oman)
		case 0x4001: // Arabic(Qatar)
		case 0x401: // Arabic(Saudi Arabia)
		case 0x2801: // Arabic(Syria)
		case 0x1c01: // Arabic(Tunisia)
		case 0x3801: // Arabic(U.A.E)
		case 0x2401: // Arabic(Yemen)
			bResult = true;
			break;
		default:
			break;
	}

	return bResult;
}

sal_Unicode SwWW8ImplReader::TranslateToHindiNumbers(sal_Unicode nChar)
{
    if (nChar >= 0x0030 && nChar <= 0x0039)
        return nChar + 0x0630;

    return nChar;
}

// Returnwert: true for no Sonderzeichen
bool SwWW8ImplReader::ReadPlainChars(WW8_CP& rPos, long nEnd, long nCpOfs)
{
    // Unicode-Flag neu setzen und notfalls File-Pos korrigieren
    // merke: Seek kostet nicht viel, da inline geprueft wird,
    //        ob die korrekte FilePos nicht schon erreicht ist.
    WW8_FC nStreamPos = pSBase->WW8Cp2Fc(nCpOfs+rPos, &bIsUnicode);
    pStrm->Seek( nStreamPos );

    xub_StrLen nLen;
    if (nEnd - rPos <= (STRING_MAXLEN-1))
        nLen = writer_cast<xub_StrLen>(nEnd - rPos);
    else
        nLen = STRING_MAXLEN-1;
    ASSERT(nLen, "String is 0");
    if (!nLen)
        return true;

    const CharSet eSrcCharSet = bVer67 ? GetCurrentCharSet() :
        RTL_TEXTENCODING_MS_1252;
    const CharSet eSrcCJKCharSet = bVer67 ? GetCurrentCJKCharSet() :
        RTL_TEXTENCODING_MS_1252;

    // (re)alloc UniString data
    String sPlainCharsBuf;

    sal_Unicode* pBuffer = sPlainCharsBuf.AllocBuffer( nLen );
    sal_Unicode* pWork = pBuffer;

    sal_Char* p8Bits = NULL;

    rtl_TextToUnicodeConverter hConverter = 0;
    if (!bIsUnicode || bVer67)
        hConverter = rtl_createTextToUnicodeConverter(eSrcCharSet);

    if (!bIsUnicode)
        p8Bits = new sal_Char[nLen];

    // read the stream data
    sal_uInt8   nBCode = 0;
    sal_uInt16 nUCode;
    xub_StrLen nL2;

	sal_uInt16 nCTLLang = 0;
	const SfxPoolItem * pItem = GetFmtAttr(RES_CHRATR_CTL_LANGUAGE);
	if (pItem != NULL)
		nCTLLang = dynamic_cast<const SvxLanguageItem *>(pItem)->GetLanguage();

    for( nL2 = 0; nL2 < nLen; ++nL2, ++pWork )
    {
        if (bIsUnicode)
            *pStrm >> nUCode;   // unicode  --> read 2 bytes
        else
        {
            *pStrm >> nBCode;   // old code --> read 1 byte
            nUCode = nBCode;
        }

        if (pStrm->GetError())
        {
            rPos = WW8_CP_MAX-10;     // -> eof or other error
            sPlainCharsBuf.ReleaseBufferAccess( 0 );
            delete [] p8Bits;
            return true;
        }

        if ((32 > nUCode) || (0xa0 == nUCode))
        {
            pStrm->SeekRel( bIsUnicode ? -2 : -1 );
            break;              // Sonderzeichen < 32, == 0xa0 gefunden
        }

        if (bIsUnicode)
        {
            if (!bVer67)
                *pWork = nUCode;
            else
            {
                if (nUCode >= 0x3000)       //0x8000 ?
                {
                    sal_Char aTest[2];
                    aTest[0] = static_cast< sal_Char >((nUCode & 0xFF00) >> 8);
                    aTest[1] = static_cast< sal_Char >(nUCode & 0x00FF);
                    String aTemp(aTest, 2, eSrcCJKCharSet);
                    ASSERT(aTemp.Len() == 1, "so much for that theory");
                    *pWork = aTemp.GetChar(0);
                }
                else
                {
                    sal_Char cTest = static_cast< sal_Char >(nUCode & 0x00FF);
                    Custom8BitToUnicode(hConverter, &cTest, 1, pWork, 1);
                }
            }
        }
        else
            p8Bits[nL2] = nBCode;
    }

    if (nL2)
    {
        xub_StrLen nEndUsed = nL2;

        if (!bIsUnicode)
            nEndUsed = Custom8BitToUnicode(hConverter, p8Bits, nL2, pBuffer, nLen);

        for( xub_StrLen nI = 0; nI < nLen; ++nI, ++pBuffer )
            if (m_bRegardHindiDigits && bBidi && LangUsesHindiNumbers(nCTLLang))
                *pBuffer = TranslateToHindiNumbers(*pBuffer);

        sPlainCharsBuf.ReleaseBufferAccess( nEndUsed );

        AddTextToParagraph(sPlainCharsBuf);
        rPos += nL2;
        if (!maApos.back()) //a para end in apo doesn't count
            bWasParaEnd = false;            //kein CR
    }

    if (hConverter)
        rtl_destroyTextToUnicodeConverter(hConverter);
    delete [] p8Bits;
    return nL2 >= nLen;
}

bool SwWW8ImplReader::AddTextToParagraph(const String& rAddString)
{
    const SwTxtNode* pNd = pPaM->GetCntntNode()->GetTxtNode();
    if (rAddString.Len())
    {
/*
#ifdef DEBUG
//!! does not compile with debug=t -> unresolved external (dbg_out),
//!! sommeone who knows what he wants to get should fix this
//        ::std::clog << "<addTextToParagraph>" << dbg_out(rAddString)
//        << "</addTextToParagraph>" << ::std::endl;
#endif
*/
        if ((pNd->GetTxt().Len() + rAddString.Len()) < STRING_MAXLEN -1)
        {
            rDoc.InsertString(*pPaM, rAddString);
        }
        else
        {

            if (pNd->GetTxt().Len()< STRING_MAXLEN -1)
            {
                String sTempStr (rAddString,0,
                    STRING_MAXLEN - pNd->GetTxt().Len() -1);
                rDoc.InsertString(*pPaM, sTempStr);
                sTempStr = rAddString.Copy(sTempStr.Len(),
                    rAddString.Len() - sTempStr.Len());
                AppendTxtNode(*pPaM->GetPoint());
                rDoc.InsertString(*pPaM, sTempStr);
            }
            else
            {
                AppendTxtNode(*pPaM->GetPoint());
                rDoc.InsertString(*pPaM, rAddString);
            }
        }

        bReadTable = false;
    }

    return true;
}

// Returnwert: true for para end
bool SwWW8ImplReader::ReadChars(WW8_CP& rPos, WW8_CP nNextAttr, long nTextEnd,
    long nCpOfs)
{
    long nEnd = ( nNextAttr < nTextEnd ) ? nNextAttr : nTextEnd;

    if (bSymbol || bIgnoreText)
    {
        if( bSymbol )   // Spezialzeichen einfuegen
        {
            for(sal_uInt16 nCh = 0; nCh < nEnd - rPos; ++nCh)
            {
                rDoc.InsertString( *pPaM, cSymbol );
            }
            pCtrlStck->SetAttr( *pPaM->GetPoint(), RES_CHRATR_FONT );
        }
        pStrm->SeekRel( nEnd- rPos );
        rPos = nEnd;    // ignoriere bis Attributende
        return false;
    }

    while (true)
    {
        if (ReadPlainChars(rPos, nEnd, nCpOfs))
            return false;                   // Fertig

        bool bStartLine = ReadChar(rPos, nCpOfs);
        rPos++;
        if (bPgSecBreak || bStartLine || rPos == nEnd)  // CR oder Fertig
        {
            return bStartLine;
        }
    }
}

bool SwWW8ImplReader::HandlePageBreakChar()
{
    bool bParaEndAdded = false;
    //#i1909# section/page breaks should not occur in tables, word
    //itself ignores them in this case.
    if (!nInTable)
    {
        //xushanchuan add for issue106569
        sal_Bool IsTemp=sal_True;
        SwTxtNode* pTemp = pPaM->GetNode()->GetTxtNode();
        if ( pTemp && !( pTemp->GetTxt().Len() ) && ( bFirstPara || bFirstParaOfPage ) )
        {
            IsTemp = sal_False;
            AppendTxtNode(*pPaM->GetPoint());
            pTemp->SetAttr(*GetDfltAttr(RES_PARATR_NUMRULE));
        }
        //xushanchuan end
        bPgSecBreak = true;
        pCtrlStck->KillUnlockedAttrs(*pPaM->GetPoint());
        /*
        If its a 0x0c without a paragraph end before it, act like a
        paragraph end, but nevertheless, numbering (and perhaps other
        similar constructs) do not exist on the para.
        */
        //xushanchuan add for issue106569
        if (!bWasParaEnd && IsTemp)
            //xushanchuan end
        {
            bParaEndAdded = true;
            if (0 >= pPaM->GetPoint()->nContent.GetIndex())
            {
                if (SwTxtNode* pTxtNode = pPaM->GetNode()->GetTxtNode())
                {
                    pTxtNode->SetAttr(
                        *GetDfltAttr(RES_PARATR_NUMRULE));
                }
            }
        }
    }
    return bParaEndAdded;
}

bool SwWW8ImplReader::ReadChar(long nPosCp, long nCpOfs)
{
    bool bNewParaEnd = false;
    // Unicode-Flag neu setzen und notfalls File-Pos korrigieren
    // merke: Seek kostet nicht viel, da inline geprueft wird,
    //        ob die korrekte FilePos nicht schon erreicht ist.
    pStrm->Seek( pSBase->WW8Cp2Fc(nCpOfs+nPosCp, &bIsUnicode) );

    sal_uInt8   nBCode;
    sal_uInt16 nWCharVal;
    if( bIsUnicode )
        *pStrm >> nWCharVal;    // unicode  --> read 2 bytes
    else
    {
        *pStrm  >>  nBCode;     // old code --> read 1 byte
        nWCharVal = nBCode;
    }

    sal_Char cInsert = '\x0';
    bool bRet = false;
	//xushanchuan add for issue106569
	if ( 0xc != nWCharVal )
		bFirstParaOfPage = false;
	//xushanchuan end
    switch (nWCharVal)
    {
        case 0:
            {
                // Seitennummer
                SwPageNumberField aFld(
                    (SwPageNumberFieldType*)rDoc.GetSysFldType(
                    RES_PAGENUMBERFLD ), PG_RANDOM, SVX_NUM_ARABIC);
                rDoc.InsertPoolItem(*pPaM, SwFmtFld(aFld), 0);
            }
            break;
        case 0xe:
            // if there is only one column word treats a column break like a pagebreak.
            if (maSectionManager.CurrentSectionColCount() < 2)
                bRet = HandlePageBreakChar();
            else if (!nInTable)
            {
                // Always insert a txtnode for a column break, e.g. ##
                AppendTxtNode(*pPaM->GetPoint());
                rDoc.InsertPoolItem(*pPaM,
                    SvxFmtBreakItem(SVX_BREAK_COLUMN_BEFORE, RES_BREAK), 0);
            }
            break;
        case 0x7:
            bNewParaEnd = true;
            TabCellEnd();       // table cell end (Flags abfragen!)
            break;
        case 0xf:
            if( !bSpec )        // "Satellit"
                cInsert = '\xa4';
            break;
        case 0x14:
            if( !bSpec )        // "Para-Ende"-Zeichen
                cInsert = '\xb5';
            break;
        case 0x15:
            if( !bSpec )        // Juristenparagraph
            {
                cp_set::iterator aItr = maTOXEndCps.find((WW8_CP)nPosCp);
	         if (aItr == maTOXEndCps.end())
                    cInsert = '\xa7';
	         else
		      maTOXEndCps.erase(aItr);
            }
            break;
        case 0x9:
            cInsert = '\x9';    // Tab
            break;
        case 0xb:
            cInsert = '\xa';    // Hard NewLine
            break;
        case 0xc:
            bRet = HandlePageBreakChar();
            break;
        case 0x1e:   // Non-breaking hyphen
            rDoc.InsertString( *pPaM, CHAR_HARDHYPHEN );
            break;
        case 0x1f:   // Non-required hyphens
            rDoc.InsertString( *pPaM, CHAR_SOFTHYPHEN );
            break;
        case 0xa0:   // Non-breaking spaces
            rDoc.InsertString( *pPaM, CHAR_HARDBLANK  );
            break;
        case 0x1:
            /*
            Current thinking is that if bObj is set then we have a
            straightforward "traditional" ole object, otherwise we have a
            graphic preview of an associated ole2 object (or a simple
            graphic of course)
            */
			//normally in the canvas field, the code is 0x8 0x1.
			//in a special case, the code is 0x1 0x1, which yields a simple picture
			{
				bool bReadObj = IsInlineEscherHack();
				if( bReadObj )
				{
					long nCurPos = pStrm->Tell();
					sal_uInt8   nByteCode;
					sal_uInt16 nWordCode;

					if( bIsUnicode )
						*pStrm >> nWordCode;
					else
					{
						*pStrm >> nByteCode;
						nWordCode = nByteCode;
					}
					if( nWordCode == 0x1 )
						bReadObj = false;
					pStrm->Seek( nCurPos );	
				}
				if( !bReadObj )
				{
					SwFrmFmt *pResult = 0;
					if (bObj)
						pResult = ImportOle();
					else if (bSpec)
						pResult = ImportGraf();

					// If we have a bad 0x1 insert a space instead.
					if (!pResult)
					{
						cInsert = ' ';
						ASSERT(!bObj && !bEmbeddObj && !nObjLocFc,
							"WW8: Please report this document, it may have a "
							"missing graphic");
					}
					else
					{
						// reset the flags.
						bObj = bEmbeddObj = false;
						nObjLocFc = 0;
					}
				}
			}
			break;
        case 0x8:
            if( !bObj )
                Read_GrafLayer( nPosCp );
            break;
        case 0xd:
            bNewParaEnd = bRet = true;
            if (nInTable > 1)
            {
                /*
                #i9666#/#i23161#
                Yes complex, if there is an entry in the undocumented PLCF
                which I believe to be a record of cell and row boundaries
                see if the magic bit which I believe to mean cell end is
                set. I also think btw that the third byte of the 4 byte
                value is the level of the cell
                */
                WW8PLCFspecial* pTest = pPlcxMan->GetMagicTables();
                if (pTest && pTest->SeekPosExact(nPosCp+1+nCpOfs) &&
                    pTest->Where() == nPosCp+1+nCpOfs)
                {
                    WW8_FC nPos;
                    void *pData;
                    pTest->Get(nPos, pData);
                    sal_uInt32 nData = SVBT32ToUInt32(*(SVBT32*)pData);
                    if (nData & 0x2)    //Might be how it works
                    {
                        TabCellEnd();
                        bRet = false;
                    }
                }
                else if (bWasTabCellEnd)
                {
                    TabCellEnd();
                    bRet = false;
                }
            }

            bWasTabCellEnd = false;

            break;              // line end
        case 0x5:               // Annotation reference
        case 0x13:
            break;
        case 0x2:
            if (!maFtnStack.empty())
                cInsert = 0x2;
            break;                  // Auto-Fussnoten-Nummer
#if OSL_DEBUG_LEVEL > 1
        default:
            ::std::clog << "<unknownValue val=\"" << nWCharVal << "\">" << ::std::endl;
            break;
#endif
    }

    if( '\x0' != cInsert )
    {
        String sInsert = ByteString::ConvertToUnicode(cInsert,
            RTL_TEXTENCODING_MS_1252 );
        AddTextToParagraph(sInsert);
    }
    if (!maApos.back()) //a para end in apo doesn't count
        bWasParaEnd = bNewParaEnd;
    return bRet;
}

void SwWW8ImplReader::ProcessAktCollChange(WW8PLCFManResult& rRes,
    bool* pStartAttr, bool bCallProcessSpecial)
{
    sal_uInt16 nOldColl = nAktColl;
    nAktColl = pPlcxMan->GetColl();

    // Invalid Style-Id
    if (nAktColl >= nColls || !pCollA[nAktColl].pFmt || !pCollA[nAktColl].bColl)
    {
        nAktColl = 0;
        bParaAutoBefore = false;
        bParaAutoAfter = false;
    }
    else
    {
        bParaAutoBefore = pCollA[nAktColl].bParaAutoBefore;
        bParaAutoAfter = pCollA[nAktColl].bParaAutoAfter;
    }

    bool bTabRowEnd = false;
    if( pStartAttr && bCallProcessSpecial && !bInHyperlink )
    {
        bool bReSync;
        // Frame / Table / Autonumbering List Level
        bTabRowEnd = ProcessSpecial(bReSync, rRes.nAktCp+pPlcxMan->GetCpOfs());
        if( bReSync )
            *pStartAttr = pPlcxMan->Get( &rRes ); // hole Attribut-Pos neu
    }

    if (!bTabRowEnd && StyleExists(nAktColl))
    {
        SetTxtFmtCollAndListLevel( *pPaM, pCollA[ nAktColl ]);
        ChkToggleAttr(pCollA[ nOldColl ].n81Flags, pCollA[ nAktColl ].n81Flags);
        ChkToggleBiDiAttr(pCollA[nOldColl].n81BiDiFlags,
            pCollA[nAktColl].n81BiDiFlags);
    }
}

long SwWW8ImplReader::ReadTextAttr(WW8_CP& rTxtPos, bool& rbStartLine)
{
    long nSkipChars = 0;
    WW8PLCFManResult aRes;

    ASSERT(pPaM->GetNode()->GetTxtNode(), "Missing txtnode");
    bool bStartAttr = pPlcxMan->Get(&aRes); // hole Attribut-Pos
    aRes.nAktCp = rTxtPos;              // Akt. Cp-Pos

    bool bNewSection = (aRes.nFlags & MAN_MASK_NEW_SEP) && !bIgnoreText;
    if ( bNewSection )	// neue Section
    {
        ASSERT(pPaM->GetNode()->GetTxtNode(), "Missing txtnode");
        // PageDesc erzeugen und fuellen
        maSectionManager.CreateSep(rTxtPos, bPgSecBreak);
                                            // -> 0xc war ein Sectionbreak, aber
                                            // kein Pagebreak;
        bPgSecBreak = false;                // PageDesc erzeugen und fuellen
        ASSERT(pPaM->GetNode()->GetTxtNode(), "Missing txtnode");
    }

    // neuer Absatz ueber Plcx.Fkp.papx
    if ( (aRes.nFlags & MAN_MASK_NEW_PAP)|| rbStartLine )
    {
        ProcessAktCollChange( aRes, &bStartAttr,
            MAN_MASK_NEW_PAP == (aRes.nFlags & MAN_MASK_NEW_PAP) &&
            !bIgnoreText );
        rbStartLine = false;
    }

    // position of last CP that's to be ignored
    long nSkipPos = -1;

    if( 0 < aRes.nSprmId )                      // leere Attrs ignorieren
    {
        if( ( eFTN > aRes.nSprmId ) || ( 0x0800 <= aRes.nSprmId ) )
        {
            if( bStartAttr )                            // WW-Attribute
            {
                if( aRes.nMemLen >= 0 )
                    ImportSprm(aRes.pMemPos, aRes.nSprmId);
            }
            else
                EndSprm( aRes.nSprmId );        // Attr ausschalten
        }
        else if( aRes.nSprmId < 0x800 ) // eigene Hilfs-Attribute
        {
            if (bStartAttr)
            {
                nSkipChars = ImportExtSprm(&aRes);
                if (
                    (aRes.nSprmId == eFTN) || (aRes.nSprmId == eEDN) ||
                    (aRes.nSprmId == eFLD) || (aRes.nSprmId == eAND)
                   )
                {
                    // Felder/Ftn-/End-Note hier ueberlesen
                    rTxtPos += nSkipChars;
                    nSkipPos = rTxtPos-1;
                }
            }
            else
                EndExtSprm( aRes.nSprmId );
        }
    }

    pStrm->Seek(pSBase->WW8Cp2Fc( pPlcxMan->GetCpOfs() + rTxtPos, &bIsUnicode));

    // Find next Attr position (and Skip attributes of field contents if needed)
    if (nSkipChars && !bIgnoreText)
        pCtrlStck->MarkAllAttrsOld();
    bool bOldIgnoreText = bIgnoreText;
    bIgnoreText = true;
    sal_uInt16 nOldColl = nAktColl;
    bool bDoPlcxManPlusPLus = true;
    long nNext;
    do
    {
        if( bDoPlcxManPlusPLus )
            (*pPlcxMan)++;
        nNext = pPlcxMan->Where();

        if (mpPostProcessAttrsInfo &&
            mpPostProcessAttrsInfo->mnCpStart == nNext)
        {
            mpPostProcessAttrsInfo->mbCopy = true;
        }

        if( (0 <= nNext) && (nSkipPos >= nNext) )
        {
            nNext = ReadTextAttr( rTxtPos, rbStartLine );
            bDoPlcxManPlusPLus = false;
            bIgnoreText = true;
        }

        if (mpPostProcessAttrsInfo &&
            nNext > mpPostProcessAttrsInfo->mnCpEnd)
        {
            mpPostProcessAttrsInfo->mbCopy = false;
        }
    }
    while( nSkipPos >= nNext );
    bIgnoreText    = bOldIgnoreText;
    if( nSkipChars )
    {
        pCtrlStck->KillUnlockedAttrs( *pPaM->GetPoint() );
        if( nOldColl != pPlcxMan->GetColl() )
            ProcessAktCollChange(aRes, 0, false);
    }

    return nNext;
}
//Modify here for #119405, by easyfan, 2012-05-24
//Revised 2012.8.16 for the complex attribute presentation of 0x0D in MS
bool SwWW8ImplReader::IsParaEndInCPs(sal_Int32 nStart, sal_Int32 nEnd,bool bSdOD) const
{
	//Modify for #119405 by chengjh, 2012-08-16
	//Revised for performance consideration
	if (nStart == -1 || nEnd == -1 || nEnd < nStart )
		return false;
	
	for (cp_vector::const_reverse_iterator aItr = maEndParaPos.rbegin(); aItr!= maEndParaPos.rend(); aItr++)
	//End
	{
		//Revised 2012.8.16,to the 0x0D,the attribute will have two situations
		//*********within***********exact******//
		//*********but also sample with only left and the position of 0x0d is the edge of the right side***********//
		if ( bSdOD && ( (nStart < *aItr && nEnd > *aItr) || ( nStart == nEnd && *aItr == nStart)) )
			return true;
		else if ( !bSdOD &&  (nStart < *aItr && nEnd >= *aItr) ) 
			return true;
	}
	
	return false;
}
//End of modification, by easyfan
//Modify for #119405 by chengjh, 2012-08-16
//Clear the para end position recorded in reader intermittently for the least impact on loading performance
void SwWW8ImplReader::ClearParaEndPosition()
{
	if ( maEndParaPos.size() > 0 )
		maEndParaPos.clear();
}
//End
void SwWW8ImplReader::ReadAttrs(WW8_CP& rNext, WW8_CP& rTxtPos, bool& rbStartLine)
{
    if( rTxtPos >= rNext )
    {           // Stehen Attribute an ?

        do
        {
		//Modify here for #119405, by easyfan, 2012-05-24
        	maCurrAttrCP = rTxtPos;
		//End of modification, by easyfan
            rNext = ReadTextAttr( rTxtPos, rbStartLine );
        }
        while( rTxtPos >= rNext );

    }
    else if ( rbStartLine )
    {
// keine Attribute, aber trotzdem neue Zeile
// wenn eine Zeile mit einem Seitenumbruch aufhoert und sich keine
// Absatzattribute / Absatzvorlagen aendern, ist das Zeilenende
// nicht im Plcx.Fkp.papx eingetragen, d.h. ( nFlags & MAN_MASK_NEW_PAP )
// ist false. Deshalb muss als Sonderbehandlung hier die Vorlage gesetzt
// werden.
        if (!bCpxStyle && nAktColl < nColls)
            SetTxtFmtCollAndListLevel(*pPaM, pCollA[nAktColl]);
        rbStartLine = false;
    }
}

// CloseAttrEnds zum Lesen nur der Attributenden am Ende eines Textes oder
// Textbereiches ( Kopfzeile, Fussnote, ...). Attributanfaenge, Felder
// werden ignoriert.
void SwWW8ImplReader::CloseAttrEnds()
{
    //If there are any unclosed sprms then copy them to
    //another stack and close the ones that must be closed
    std::stack<sal_uInt16> aStack;
    pPlcxMan->TransferOpenSprms(aStack);

    while (!aStack.empty())
    {
        sal_uInt16 nSprmId = aStack.top();
        if ((0 < nSprmId) && (( eFTN > nSprmId) || (0x0800 <= nSprmId)))
            EndSprm(nSprmId);
        aStack.pop();
    }

    EndSpecial();
}

bool SwWW8ImplReader::ReadText(long nStartCp, long nTextLen, ManTypes nType)
{
    sw::log::Environment eContext = sw::log::eMainText;
    if (nType == MAN_MAINTEXT)
        eContext = sw::log::eMainText;
    else
        eContext = sw::log::eSubDoc;
    maTracer.EnterEnvironment(eContext);

    bool bJoined=false;

    bool bStartLine = true;
    short nCrCount = 0;
    short nDistance = 0;

    bWasParaEnd = false;
    nAktColl    =  0;
    pAktItemSet =  0;
    nCharFmt    = -1;
    bSpec = false;
    bPgSecBreak = false;

    pPlcxMan = new WW8PLCFMan( pSBase, nType, nStartCp );
    long nCpOfs = pPlcxMan->GetCpOfs(); // Offset fuer Header/Footer, Footnote

    WW8_CP nNext = pPlcxMan->Where();
    SwTxtNode* pPreviousNode = 0;
    sal_uInt8 nDropLines = 0;
    SwCharFmt* pNewSwCharFmt = 0;
    const SwCharFmt* pFmt = 0;
    pStrm->Seek( pSBase->WW8Cp2Fc( nStartCp + nCpOfs, &bIsUnicode ) );

    WW8_CP l = nStartCp;
    while ( l<nStartCp+nTextLen )
    {
        ReadAttrs( nNext, l, bStartLine );// behandelt auch Section-Breaks
        ASSERT(pPaM->GetNode()->GetTxtNode(), "Missing txtnode");

        if (mpPostProcessAttrsInfo != NULL)
            PostProcessAttrs();

        if( l>= nStartCp + nTextLen )
            break;

        bStartLine = ReadChars(l, nNext, nStartCp+nTextLen, nCpOfs);

        // If the previous paragraph was a dropcap then do not
        // create a new txtnode and join the two paragraphs together

        if (bStartLine && !pPreviousNode) // Zeilenende
        {
            bool bSplit = true;
            if (mbCareFirstParaEndInToc)
            {
                mbCareFirstParaEndInToc = false;
                if (pPaM->End() && pPaM->End()->nNode.GetNode().GetTxtNode() &&  pPaM->End()->nNode.GetNode().GetTxtNode()->Len() == 0)
                    bSplit = false;
            }
            if (mbCareLastParaEndInToc)
            {
                mbCareLastParaEndInToc = false;
                if (pPaM->End() && pPaM->End()->nNode.GetNode().GetTxtNode() &&  pPaM->End()->nNode.GetNode().GetTxtNode()->Len() == 0)
                    bSplit = false;
            }
            if (bSplit)
            {
                // #119405# - We will record the CP of a paragraph end ('0x0D'), if current loading contents is from main stream;
                if (mbOnLoadingMain)
                    maEndParaPos.push_back(l-1);
                AppendTxtNode(*pPaM->GetPoint());
            }
        }

        if (pPreviousNode && bStartLine)
        {
            SwTxtNode* pEndNd = pPaM->GetNode()->GetTxtNode();
            const xub_StrLen nDropCapLen = pPreviousNode->GetTxt().Len();

            // Need to reset the font size and text position for the dropcap
            {
                SwPaM aTmp(*pEndNd, 0, *pEndNd, nDropCapLen+1);
                pCtrlStck->Delete(aTmp);
            }

            // Get the default document dropcap which we can use as our template
            const SwFmtDrop* defaultDrop =
                (const SwFmtDrop*) GetFmtAttr(RES_PARATR_DROP);
            SwFmtDrop aDrop(*defaultDrop);

            aDrop.GetLines() = nDropLines;
            aDrop.GetDistance() = nDistance;
            aDrop.GetChars() = writer_cast<sal_uInt8>(nDropCapLen);
            // Word has no concept of a "whole word dropcap"
            aDrop.GetWholeWord() = false;

            if (pFmt)
			    aDrop.SetCharFmt(const_cast<SwCharFmt*>(pFmt));
			else if(pNewSwCharFmt)
				aDrop.SetCharFmt(const_cast<SwCharFmt*>(pNewSwCharFmt));

            SwPosition aStart(*pEndNd);
            pCtrlStck->NewAttr(aStart, aDrop);
            pCtrlStck->SetAttr(*pPaM->GetPoint(), RES_PARATR_DROP);
            pPreviousNode = 0;
        }
        else if (bDropCap)
        {
            // If we have found a dropcap store the textnode
            pPreviousNode = pPaM->GetNode()->GetTxtNode();

            const sal_uInt8 *pDCS;

            if (bVer67)
                pDCS = pPlcxMan->GetPapPLCF()->HasSprm(46);
            else
                pDCS = pPlcxMan->GetPapPLCF()->HasSprm(0x442C);

            if (pDCS)
                nDropLines = (*pDCS) >> 3;
            else    // There is no Drop Cap Specifier hence no dropcap
                pPreviousNode = 0;

            if (const sal_uInt8 *pDistance = pPlcxMan->GetPapPLCF()->HasSprm(0x842F))
                nDistance = SVBT16ToShort( pDistance );
            else
                nDistance = 0;

            const SwFmtCharFmt *pSwFmtCharFmt = 0;

            if(pAktItemSet)
                pSwFmtCharFmt = &(ItemGet<SwFmtCharFmt>(*pAktItemSet, RES_TXTATR_CHARFMT));

            if(pSwFmtCharFmt)
                pFmt = pSwFmtCharFmt->GetCharFmt();

            if(pAktItemSet && !pFmt)
            {
                String sPrefix(CREATE_CONST_ASC( "WW8Dropcap"));
                sPrefix += String::CreateFromInt32( nDropCap++ );
                pNewSwCharFmt = rDoc.MakeCharFmt(sPrefix, (SwCharFmt*)rDoc.GetDfltCharFmt());
                 pAktItemSet->ClearItem(RES_CHRATR_ESCAPEMENT);
                pNewSwCharFmt->SetFmtAttr( *pAktItemSet );
            }

            delete pAktItemSet;
            pAktItemSet = 0;
            bDropCap=false;
        }

        if (bStartLine || bWasTabRowEnd)
        {
            // alle 64 CRs aufrufen not for Header u. ae.
            if ((nCrCount++ & 0x40) == 0 && nType == MAN_MAINTEXT)
            {
                nProgress = (sal_uInt16)( l * 100 / nTextLen );
                ::SetProgressState(nProgress, mpDocShell); // Update
            }
        }

        // If we have encountered a 0x0c which indicates either section of
        // pagebreak then look it up to see if it is a section break, and
        // if it is not then insert a page break. If it is a section break
        // it will be handled as such in the ReadAttrs of the next loop
        if (bPgSecBreak)
        {
            // We need only to see if a section is ending at this cp,
            // the plcf will already be sitting on the correct location
            // if it is there.
            WW8PLCFxDesc aTemp;
            aTemp.nStartPos = aTemp.nEndPos = WW8_CP_MAX;
            if (pPlcxMan->GetSepPLCF())
                pPlcxMan->GetSepPLCF()->GetSprms(&aTemp);
            if ((aTemp.nStartPos != l) && (aTemp.nEndPos != l))
            {
                // #i39251# - insert text node for page break, if no one inserted.
                // #i43118# - refine condition: the anchor control stack has to have entries,
                // otherwise it's not needed to insert a text node.
                if ( !bStartLine && pAnchorStck->Count() > 0 )
                {
                    AppendTxtNode(*pPaM->GetPoint());
                }
                rDoc.InsertPoolItem(*pPaM,
                    SvxFmtBreakItem(SVX_BREAK_PAGE_BEFORE, RES_BREAK), 0);
                bFirstParaOfPage = true;//xushanchuan add for issue106569
                bPgSecBreak = false;
            }
        }
    }

    if (pPaM->GetPoint()->nContent.GetIndex())
        AppendTxtNode(*pPaM->GetPoint());

    if (!bInHyperlink)
        bJoined = JoinNode(*pPaM);

    CloseAttrEnds();

    delete pPlcxMan, pPlcxMan = 0;
    maTracer.LeaveEnvironment(eContext);
    return bJoined;
}

/***************************************************************************
#           class SwWW8ImplReader
#**************************************************************************/

SwWW8ImplReader::SwWW8ImplReader(sal_uInt8 nVersionPara, SvStorage* pStorage,
    SvStream* pSt, SwDoc& rD, const String& rBaseURL, bool bNewDoc) 
    : mpDocShell(rD.GetDocShell())
    , maTracer(*(mpDocShell->GetMedium()))
    , pStg(pStorage)
    , pStrm(pSt)
    , pTableStream(0)
    , pDataStream(0)
    , rDoc(rD)
    , maSectionManager(*this)
    , maInsertedTables(rD)
    , maSectionNameGenerator(rD,CREATE_CONST_ASC("WW"))
    , maGrfNameGenerator(bNewDoc,String('G'))
    , maParaStyleMapper(rD)
    , maCharStyleMapper(rD)
    , maTxtNodesHavingFirstLineOfstSet()
    , maTxtNodesHavingLeftIndentSet()
    , pMSDffManager(0)
    , mpAtnNames(0)
    , pAuthorInfos(0)
    , sBaseURL(rBaseURL)
    , m_bRegardHindiDigits( false )
    , mbNewDoc(bNewDoc)
    , nDropCap(0)
    , nIdctHint(0)
    , bBidi(false)
    , bReadTable(false)
    , mbLoadingTOXCache(false)
    , mnEmbeddedTOXLevel(0)
    , mbLoadingTOXHyperlink(false)
    , mpPosAfterTOC(0)
    , mbCareFirstParaEndInToc(false)
    , mbCareLastParaEndInToc(false)
    , maTOXEndCps()
	//Modify here for #119405, by easyfan, 2012-05-24
	,maCurrAttrCP(-1),
	mbOnLoadingMain(false)
	//End of modification, by easyfan
{
    pStrm->SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );
    nWantedVersion = nVersionPara;
    pCtrlStck   = 0;
    mpRedlineStack = 0;
    pReffedStck = 0;
    pReffingStck = 0;
    pAnchorStck = 0;
    pFonts = 0;
    pSBase = 0;
    pPlcxMan = 0;
    pStyles = 0;
    pAktColl = 0;
    pLstManager = 0;
    pAktItemSet = 0;
    pCollA = 0;
    pDfltTxtFmtColl = 0;
    pStandardFmtColl = 0;
    pHdFt = 0;
    pWFlyPara = 0;
    pSFlyPara = 0;
    pFlyFmtOfJustInsertedGraphic   = 0;
    pFmtOfJustInsertedApo = 0;
    pPreviousNumPaM = 0;
    pPrevNumRule = 0;
    nColls = nAktColl = 0;
    nObjLocFc = nPicLocFc = 0;
    nInTable=0;
    bReadNoTbl = bPgSecBreak = bSpec = bObj = bTxbxFlySection
               = bHasBorder = bSymbol = bIgnoreText
               = bWasTabRowEnd = bWasTabCellEnd = false;
    bShdTxtCol = bCharShdTxtCol = bAnl = bHdFtFtnEdn = bFtnEdn
               = bIsHeader = bIsFooter = bIsUnicode = bCpxStyle = bStyNormal =
                 bWWBugNormal  = false;

    mpPostProcessAttrsInfo = 0;

    bNoAttrImport = bPgChpLevel = bEmbeddObj = false;
    bAktAND_fNumberAcross = false;
    bNoLnNumYet = true;
    bInHyperlink = false;
    bWasParaEnd = false;
    bDropCap = false;
    bFirstPara = true;
	  bFirstParaOfPage = false;//xushanchuan add for issue106569
    bParaAutoBefore = false;
    bParaAutoAfter = false;
    nProgress = 0;
    nSwNumLevel = nWwNumType = 0xff;
    pTableDesc = 0;
    pNumOlst = 0;
    pNode_FLY_AT_PARA = 0;
    pDrawModel = 0;
    pDrawPg = 0;
    mpDrawEditEngine = 0;
    pWWZOrder = 0;
    pFormImpl = 0;
    pNumFldType = 0;
    nFldNum = 0;

    nLFOPosition = USHRT_MAX;
    nListLevel = WW8ListManager::nMaxLevel;
    eHardCharSet = RTL_TEXTENCODING_DONTKNOW;

    nPgChpDelim = nPgChpLevel = 0;

    maApos.push_back(false);
}

void SwWW8ImplReader::DeleteStk(SwFltControlStack* pStck)
{
    if( pStck )
    {
        pStck->SetAttr( *pPaM->GetPoint(), 0, false);
        pStck->SetAttr( *pPaM->GetPoint(), 0, false);
        delete pStck;
    }
    else
    {
        ASSERT( sal_False, "WW stack already deleted" );
    }
}

void wwSectionManager::SetSegmentToPageDesc(const wwSection &rSection,
    bool bTitlePage, bool bIgnoreCols)
{
    SwPageDesc &rPage = bTitlePage ? *rSection.mpTitlePage : *rSection.mpPage;

    SetNumberingType(rSection, rPage);

    SwFrmFmt &rFmt = rPage.GetMaster();
	
    if(mrReader.pWDop->fUseBackGroundInAllmodes)  // #i56806# Make sure mrReader is initialized
		mrReader.GrafikCtor(); 
	

    if (mrReader.pWDop->fUseBackGroundInAllmodes && mrReader.pMSDffManager)
    {
        Rectangle aRect(0, 0, 100, 100); //A dummy, we don't care about the size
        SvxMSDffImportData aData(aRect);
        SdrObject* pObject = 0;
        if (mrReader.pMSDffManager->GetShape(0x401, pObject, aData))
        {
            SvxMSDffImportRec * pRec = aData.GetRecord(0);

            // Only handle shape if it is a background shape
            if ((pRec->nFlags & 0x400) != 0)
            {
                SfxItemSet aSet(rFmt.GetAttrSet());
                mrReader.MatchSdrItemsIntoFlySet(pObject, aSet, mso_lineSimple,
                                                 mso_sptRectangle, aRect);
                rFmt.SetFmtAttr(aSet.Get(RES_BACKGROUND));
            }
        }
    }
    wwULSpaceData aULData;
    GetPageULData(rSection, bTitlePage, aULData);
    SetPageULSpaceItems(rFmt, aULData, rSection);

    SetPage(rPage, rFmt, rSection, bIgnoreCols);

    bool bSetBorder = false;
    switch (rSection.maSep.pgbApplyTo)
    {
        case 0:
        case 3:
            bSetBorder = true;
            break;
        case 1:
            bSetBorder = bTitlePage;
            break;
        case 2:
            bSetBorder = !bTitlePage;
            break;
    }
    if (bSetBorder)
        mrReader.SetPageBorder(rFmt, rSection);

    mrReader.SetDocumentGrid(rFmt, rSection);
}

void wwSectionManager::SetUseOn(wwSection &rSection)
{
    bool bEven = (rSection.maSep.grpfIhdt & (WW8_HEADER_EVEN|WW8_FOOTER_EVEN)) ?
        true : false;

    bool bMirror = mrReader.pWDop->fMirrorMargins ||
        mrReader.pWDop->doptypography.f2on1;

    UseOnPage eUseBase = bMirror ? nsUseOnPage::PD_MIRROR : nsUseOnPage::PD_ALL;
    UseOnPage eUse = eUseBase;
    if (!bEven)
        eUse = (UseOnPage)(eUse | nsUseOnPage::PD_HEADERSHARE | nsUseOnPage::PD_FOOTERSHARE);

    ASSERT(rSection.mpPage, "Makes no sense to call me with no pages to set");
    if (rSection.mpPage)
        rSection.mpPage->WriteUseOn(eUse);
    if (rSection.mpTitlePage)
    {
        rSection.mpTitlePage->WriteUseOn(
            (UseOnPage) (eUseBase | nsUseOnPage::PD_HEADERSHARE | nsUseOnPage::PD_FOOTERSHARE));
    }
}

//Set the page descriptor on this node, handle the different cases for a text
//node or a table
void GiveNodePageDesc(SwNodeIndex &rIdx, const SwFmtPageDesc &rPgDesc,
    SwDoc &rDoc)
{
    /*
    If its a table here, apply the pagebreak to the table
    properties, otherwise we add it to the para at this
    position
    */
    if (rIdx.GetNode().IsTableNode())
    {
        SwTable& rTable =
            rIdx.GetNode().GetTableNode()->GetTable();
        SwFrmFmt* pApply = rTable.GetFrmFmt();
        ASSERT(pApply, "impossible");
        if (pApply)
            pApply->SetFmtAttr(rPgDesc);
    }
    else
    {
        SwPosition aPamStart(rIdx);
        aPamStart.nContent.Assign(
            rIdx.GetNode().GetCntntNode(), 0);
        SwPaM aPage(aPamStart);

        rDoc.InsertPoolItem(aPage, rPgDesc, 0);
    }
}

//Map a word section with to either one or two writer page descriptors
//depending on if the word section has a title page
SwFmtPageDesc wwSectionManager::SetSwFmtPageDesc(mySegIter &rIter,
    mySegIter &rStart, bool bIgnoreCols)
{
    SwFmtPageDesc aEmpty;
    // Always read title page header/footer data - it could be used by following sections
    {
        if (IsNewDoc() && rIter == rStart)
        {
            rIter->mpTitlePage =
                mrReader.rDoc.GetPageDescFromPool(RES_POOLPAGE_FIRST);
        }
        else
        {
            sal_uInt16 nPos = mrReader.rDoc.MakePageDesc(
                ViewShell::GetShellRes()->GetPageDescName(mnDesc)
                , 0, false);
            rIter->mpTitlePage = &mrReader.rDoc._GetPageDesc(nPos);
        }
        ASSERT(rIter->mpTitlePage, "no page!");
        if (!rIter->mpTitlePage)
            return aEmpty;

        SetSegmentToPageDesc(*rIter, true, bIgnoreCols);
    }

    if (IsNewDoc() && rIter == rStart)
    {
        rIter->mpPage =
            mrReader.rDoc.GetPageDescFromPool(RES_POOLPAGE_STANDARD);
    }
    else
    {
        sal_uInt16 nPos = mrReader.rDoc.MakePageDesc(
            ViewShell::GetShellRes()->GetPageDescName(mnDesc,
                false, rIter->HasTitlePage()),
                rIter->mpTitlePage, false);
        rIter->mpPage = &mrReader.rDoc._GetPageDesc(nPos);
    }
    ASSERT(rIter->mpPage, "no page!");
    if (!rIter->mpPage)
        return aEmpty;

    //Set page before hd/ft
    const wwSection *pPrevious = 0;

    mySegIter aPrev = rIter;
    while( aPrev!= rStart )
    {
        aPrev--;
        pPrevious = &(*(aPrev));
        if( aPrev->IsContinous())
            continue;
        else{
            break;
        }
    }

    SetHdFt(*rIter, std::distance(rStart, rIter), pPrevious);
    SetUseOn(*rIter);

    //Set hd/ft after set page
    if (rIter->mpTitlePage)
        SetSegmentToPageDesc(*rIter, true, bIgnoreCols);
    SetSegmentToPageDesc(*rIter, false, bIgnoreCols);

    SwFmtPageDesc aRet(rIter->HasTitlePage() ?
        rIter->mpTitlePage : rIter->mpPage);

    rIter->mpPage->SetFollow(rIter->mpPage);

    if (rIter->mpTitlePage)
        rIter->mpTitlePage->SetFollow(rIter->mpPage);

    if (rIter->PageRestartNo())
        aRet.SetNumOffset(rIter->PageStartAt());

    ++mnDesc;
    return aRet;
}

bool wwSectionManager::IsNewDoc() const
{
    return mrReader.mbNewDoc;
}

void wwSectionManager::InsertSegments()
{
    const SvtFilterOptions* pOpt = SvtFilterOptions::Get();
    sal_Bool bUseEnhFields=(pOpt && pOpt->IsUseEnhancedFields());
    mySegIter aEnd = maSegments.end();
    mySegIter aStart = maSegments.begin();
    for (mySegIter aIter = aStart; aIter != aEnd; ++aIter)
    {
        mySegIter aNext = aIter+1;
        mySegIter aPrev = (aIter == aStart) ? aIter : aIter-1;

        // If two following sections are different in following properties, Word will interprete a continuous
        // section break between them as if it was a section break next page.
        bool bThisAndPreviousAreCompatible = ((aIter->GetPageWidth() == aPrev->GetPageWidth()) &&
            (aIter->GetPageHeight() == aPrev->GetPageHeight()) && (aIter->IsLandScape() == aPrev->IsLandScape()));

        bool bInsertSection = (aIter != aStart) ? (aIter->IsContinous() &&  bThisAndPreviousAreCompatible): false;
        bool bInsertPageDesc = !bInsertSection;
        bool bProtected = SectionIsProtected(*aIter); // do we really  need this ?? I guess I have a different logic in editshell which disales this...
        if (bUseEnhFields && mrReader.pWDop->fProtEnabled && aIter->IsNotProtected())
        {
            // here we have the special case that the whole document is protected, with the execption of this section.
            // I want to address this when I do the section rework, so for the moment we disable the overall protection then...
            mrReader.rDoc.set(IDocumentSettingAccess::PROTECT_FORM, false );
        }


        if (bInsertPageDesc)
        {
            /*
             If a cont section follows this section then we won't be
             creating a page desc with 2+ cols as we cannot host a one
             col section in a 2+ col pagedesc and make it look like
             word. But if the current section actually has columns then
             we are forced to insert a section here as well as a page
             descriptor.
            */

            bool bIgnoreCols = false;
            bool bThisAndNextAreCompatible = (aNext != aEnd) ? ((aIter->GetPageWidth() == aNext->GetPageWidth()) &&
                (aIter->GetPageHeight() == aNext->GetPageHeight()) && (aIter->IsLandScape() == aNext->IsLandScape())) : true;

            if (((aNext != aEnd && aNext->IsContinous() && bThisAndNextAreCompatible) || bProtected))
            {
                bIgnoreCols = true;
                if ((aIter->NoCols() > 1) || bProtected)
                    bInsertSection = true;
            }

            SwFmtPageDesc aDesc(SetSwFmtPageDesc(aIter, aStart, bIgnoreCols));
            if (!aDesc.GetPageDesc())
                continue;
            GiveNodePageDesc(aIter->maStart, aDesc, mrReader.rDoc);
        }

        SwTxtNode* pTxtNd = 0;
        if (bInsertSection)
        {
            //Start getting the bounds of this section
            SwPaM aSectPaM(*mrReader.pPaM);
            SwNodeIndex aAnchor(aSectPaM.GetPoint()->nNode);
            if (aNext != aEnd)
            {
                aAnchor = aNext->maStart;
                aSectPaM.GetPoint()->nNode = aAnchor;
                aSectPaM.GetPoint()->nContent.Assign(
                    aNext->maStart.GetNode().GetCntntNode(), 0);
                aSectPaM.Move(fnMoveBackward);
            }

            const SwPosition* pPos  = aSectPaM.GetPoint();
            SwTxtNode const*const pSttNd = pPos->nNode.GetNode().GetTxtNode();
            const SwTableNode* pTableNd = pSttNd ? pSttNd->FindTableNode() : 0;
            if (pTableNd)
            {
                pTxtNd =
                    mrReader.rDoc.GetNodes().MakeTxtNode(aAnchor,
                    mrReader.rDoc.GetTxtCollFromPool( RES_POOLCOLL_TEXT ));

                aSectPaM.GetPoint()->nNode = SwNodeIndex(*pTxtNd);
                aSectPaM.GetPoint()->nContent.Assign(
                    aSectPaM.GetCntntNode(), 0);
            }

            aSectPaM.SetMark();

            aSectPaM.GetPoint()->nNode = aIter->maStart;
            aSectPaM.GetPoint()->nContent.Assign(
                aSectPaM.GetCntntNode(), 0);
            //End getting the bounds of this section, quite a job eh ?

            SwSectionFmt *pRet = InsertSection(aSectPaM, *aIter);
            //The last section if continuous is always unbalanced
            if (pRet)
            {
                //Set the columns to be UnBalanced if that compatibility option
                //is set
                if (mrReader.pWDop->fNoColumnBalance)
                    pRet->SetFmtAttr(SwFmtNoBalancedColumns(true));
                else
                {
                    //Otherwise set to unbalanced if the following section is
                    //not continuous, (which also means that the last section
                    //is unbalanced)
                    if (aNext == aEnd || !aNext->IsContinous())
                        pRet->SetFmtAttr(SwFmtNoBalancedColumns(true));
                }
            }

            bool bHasOwnHdFt = false;
            /*
             In this nightmare scenario the continuous section has its own
             headers and footers so we will try and find a hard page break
             between here and the end of the section and put the headers and
             footers there.
            */
            if (!bInsertPageDesc)
            {
               bHasOwnHdFt =
                mrReader.HasOwnHeaderFooter(
                 aIter->maSep.grpfIhdt & ~(WW8_HEADER_FIRST | WW8_FOOTER_FIRST),
                 aIter->maSep.grpfIhdt, std::distance(aStart, aIter)
                );
            }
            if (bHasOwnHdFt)
            {
				// #i40766# Need to cache the page descriptor in case there is
				// no page break in the section
				SwPageDesc *pOrig = aIter->mpPage;
				SwPageDesc *pOrigTitle = aIter->mpTitlePage;
				bool bFailed = true;
                SwFmtPageDesc aDesc(SetSwFmtPageDesc(aIter, aStart, true));
                if (aDesc.GetPageDesc())
                {
                    sal_uLong nStart = aSectPaM.Start()->nNode.GetIndex();
                    sal_uLong nEnd   = aSectPaM.End()->nNode.GetIndex();
                    for(; nStart <= nEnd; ++nStart)
                    {
                        SwNode* pNode = mrReader.rDoc.GetNodes()[nStart];
                        if (!pNode)
                            continue;
                        if (sw::util::HasPageBreak(*pNode))
                        {
                            SwNodeIndex aIdx(*pNode);
                            GiveNodePageDesc(aIdx, aDesc, mrReader.rDoc);
							bFailed = false;
                            break;
                        }
                    }
                }
				if(bFailed)
				{
					aIter->mpPage = pOrig;
					aIter->mpTitlePage = pOrigTitle;
				}
            }
        }

        if (pTxtNd)
        {
            SwNodeIndex aIdx(*pTxtNd);
            SwPosition aPos(aIdx);
            SwPaM aTest(aPos);
            mrReader.rDoc.DelFullPara(aTest);
            pTxtNd = 0;
        }
    }
}

void SwWW8ImplReader::StoreMacroCmds()
{
    if (pWwFib->lcbCmds)
    {
        maTracer.Log(sw::log::eContainsWordBasic);

        pTableStream->Seek(pWwFib->fcCmds);

        uno::Reference < embed::XStorage > xRoot(mpDocShell->GetStorage());
        try
        {
            uno::Reference < io::XStream > xStream =
                    xRoot->openStreamElement( CREATE_CONST_ASC(SL::aMSMacroCmds), embed::ElementModes::READWRITE );
            SvStream* pStream = ::utl::UcbStreamHelper::CreateStream( xStream );

            sal_uInt8 *pBuffer = new sal_uInt8[pWwFib->lcbCmds];
            pTableStream->Read(pBuffer, pWwFib->lcbCmds);
            pStream->Write(pBuffer, pWwFib->lcbCmds);
            delete[] pBuffer;
            delete pStream;
        }
        catch ( uno::Exception& )
        {
        }
    }
}

void SwWW8ImplReader::ReadDocVars()
{
    std::vector<String> aDocVarStrings;
    std::vector<ww::bytes> aDocVarStringIds;
    std::vector<String> aDocValueStrings;
    WW8ReadSTTBF(!bVer67, *pTableStream, pWwFib->fcStwUser,
        pWwFib->lcbStwUser, bVer67 ? 2 : 0, eStructCharSet,
        aDocVarStrings, &aDocVarStringIds, &aDocValueStrings);
    if (!bVer67) {
        using namespace ::com::sun::star;

        uno::Reference<document::XDocumentPropertiesSupplier> xDPS(
            mpDocShell->GetModel(), uno::UNO_QUERY_THROW);
        uno::Reference<document::XDocumentProperties> xDocProps(
            xDPS->getDocumentProperties());
        DBG_ASSERT(xDocProps.is(), "DocumentProperties is null");
        uno::Reference<beans::XPropertyContainer> xUserDefinedProps =
            xDocProps->getUserDefinedProperties();
        DBG_ASSERT(xUserDefinedProps.is(), "UserDefinedProperties is null");

        for(size_t i=0; i<aDocVarStrings.size(); i++)
        {
            uno::Any aDefaultValue;
            ::rtl::OUString name(aDocVarStrings[i]);
            uno::Any aValue;
            aValue <<= ::rtl::OUString(aDocValueStrings[i]);
            try {
                xUserDefinedProps->addProperty( name,
                    beans::PropertyAttribute::REMOVEABLE,
                    aValue );
            } catch (uno::Exception &) {
                // ignore
            }
        }
	}
}

//-----------------------------------------
//      Document Info
//-----------------------------------------

void SwWW8ImplReader::ReadDocInfo()
{
    if( pStg )
    {
        uno::Reference<document::XDocumentPropertiesSupplier> xDPS(
            mpDocShell->GetModel(), uno::UNO_QUERY_THROW);
        uno::Reference<document::XDocumentProperties> xDocProps(
            xDPS->getDocumentProperties());
        DBG_ASSERT(xDocProps.is(), "DocumentProperties is null");

        if (xDocProps.is()) {
            sfx2::LoadOlePropertySet(xDocProps, pStg);
        }
    }
}

sal_uLong SwWW8ImplReader::CoreLoad(WW8Glossary *pGloss, const SwPosition &rPos)
{
    sal_uLong nErrRet = 0;

	rDoc.SetDocumentType( SwDoc::DOCTYPE_MSWORD );
    if (mbNewDoc && pStg && !pGloss)
        ReadDocInfo();

    ::ww8::WW8FibData * pFibData = new ::ww8::WW8FibData();

    if (pWwFib->fReadOnlyRecommended)
        pFibData->setReadOnlyRecommended(true);
    else
        pFibData->setReadOnlyRecommended(false);

    if (pWwFib->fWriteReservation)
        pFibData->setWriteReservation(true);
    else
        pFibData->setWriteReservation(false);

    ::sw::tExternalDataPointer pExternalFibData(pFibData);

    rDoc.setExternalData(::sw::FIB, pExternalFibData);

    ::sw::tExternalDataPointer pSttbfAsoc
          (new ::ww8::WW8Sttb<ww8::WW8Struct>(*pTableStream, pWwFib->fcSttbfAssoc, pWwFib->lcbSttbfAssoc));

    rDoc.setExternalData(::sw::STTBF_ASSOC, pSttbfAsoc);

    if (pWwFib->fWriteReservation || pWwFib->fReadOnlyRecommended)
    {
        SwDocShell * pDocShell = rDoc.GetDocShell();
        if (pDocShell)
            pDocShell->SetReadOnlyUI(sal_True);
    }

    pPaM = new SwPaM(rPos);

    pCtrlStck = new SwWW8FltControlStack( &rDoc, nFieldFlags, *this );

    mpRedlineStack = new sw::util::RedlineStack(rDoc);

    /*
        RefFldStck: Keeps track of bookmarks which may be inserted as
        variables intstead.
    */
    pReffedStck = new SwWW8ReferencedFltEndStack(&rDoc, nFieldFlags);
    pReffingStck = new SwWW8FltRefStack(&rDoc, nFieldFlags);

    pAnchorStck = new SwWW8FltAnchorStack(&rDoc, nFieldFlags);

    sal_uInt16 nPageDescOffset = rDoc.GetPageDescCnt();

    SwNodeIndex aSttNdIdx( rDoc.GetNodes() );
    SwRelNumRuleSpaces aRelNumRule(rDoc, mbNewDoc);

    sal_uInt16 eMode = nsRedlineMode_t::REDLINE_SHOW_INSERT;

    mpSprmParser = new wwSprmParser(pWwFib->GetFIBVersion());

    // praktische Hilfsvariablen besetzen:
    bVer6  = (6 == pWwFib->nVersion);
    bVer7  = (7 == pWwFib->nVersion);
    bVer67 = bVer6 || bVer7;
    bVer8  = (8 == pWwFib->nVersion);

    eTextCharSet = WW8Fib::GetFIBCharset(pWwFib->chse);
    eStructCharSet = WW8Fib::GetFIBCharset(pWwFib->chseTables);

    bWWBugNormal = pWwFib->nProduct == 0xc03d;

    if (!mbNewDoc)
        aSttNdIdx = pPaM->GetPoint()->nNode;

    ::StartProgress(STR_STATSTR_W4WREAD, 0, 100, mpDocShell);

#ifdef DEBUGDUMP
    //experimental embedded ttf dumper
    if (pWwFib->lcbSttbttmbd && (7 < pWwFib->nVersion))
    {
        pTableStream->Seek(pWwFib->fcSttbttmbd);
        sal_uInt16 nZeros;
        *pTableStream >> nZeros;
        sal_uInt16 nNoEntries;
        *pTableStream >> nNoEntries;
        sal_uInt32 nUnknown1;
        *pTableStream >> nUnknown1;
        sal_uInt16 nUnknown2;
        *pTableStream >> nUnknown2;
        std::vector<sal_uInt32> aOffsets;
        for (sal_uInt16 nI = 0; nI < nNoEntries; ++nI)
        {
            sal_uInt32 nOffset;
            *pTableStream >> nOffset;
            aOffsets.push_back(nOffset);
            sal_uInt32 nUnknown3;
            *pTableStream >> nUnknown3;
            sal_uInt32 nUnknown4;
            *pTableStream >> nUnknown4;
        }
        typedef std::vector<sal_uInt32>::iterator myIter;
        myIter aEnd = aOffsets.end();
        myIter aIter = aOffsets.begin();
        while (aIter != aEnd)
        {
            sal_uInt32 nOffset = *aIter;
            sal_uInt32 nLen = STREAM_SEEK_TO_END;
            ++aIter;
            pStrm->Seek(nOffset);
            if (aIter != aEnd)
                nLen = *aIter - nOffset;
            SvStream *pDbg = sw::hack::CreateDebuggingStream(CREATE_CONST_ASC(".ttf.dump"));
            sw::hack::DumpStream(*pStrm, *pDbg, nLen);
            delete pDbg;
        }
    }
#endif

    // read Font Table
    pFonts = new WW8Fonts( *pTableStream, *pWwFib );

    // Document Properties
    pWDop = new WW8Dop( *pTableStream, pWwFib->nFib, pWwFib->fcDop,
        pWwFib->lcbDop );

    if (mbNewDoc)
        ImportDop();

    /*
        Import revisioning data: author names
    */
    if( pWwFib->lcbSttbfRMark )
    {
        ReadRevMarkAuthorStrTabl( *pTableStream,
                                    pWwFib->fcSttbfRMark,
                                    pWwFib->lcbSttbfRMark, rDoc );
    }

    // M.M. Initialize our String/ID map for Linked Sections
    std::vector<String> aLinkStrings;
    std::vector<ww::bytes> aStringIds;

    WW8ReadSTTBF(!bVer67, *pTableStream, pWwFib->fcSttbFnm,
        pWwFib->lcbSttbFnm, bVer67 ? 2 : 0, eStructCharSet,
        aLinkStrings, &aStringIds);

    for (size_t i=0; i < aLinkStrings.size(); ++i)
    {
        ww::bytes stringId = aStringIds[i];
        WW8_STRINGID *stringIdStruct = (WW8_STRINGID*)(&stringId[0]);
        aLinkStringMap[SVBT16ToShort(stringIdStruct->nStringId)] =
            aLinkStrings[i];
    }

	ReadDocVars(); // import document variables as meta information.

    ::SetProgressState(nProgress, mpDocShell);    // Update

    pLstManager = new WW8ListManager( *pTableStream, *this );

    /*
        zuerst(!) alle Styles importieren   (siehe WW8PAR2.CXX)
            VOR dem Import der Listen !!
    */
    ::SetProgressState(nProgress, mpDocShell);    // Update
    pStyles = new WW8RStyle( *pWwFib, this );   // Styles
    pStyles->Import();

    /*
        zu guter Letzt: (siehe ebenfalls WW8PAR3.CXX)
        ===============
        alle Styles durchgehen und ggfs. zugehoeriges Listen-Format
        anhaengen NACH dem Import der Styles und NACH dem Import der
        Listen !!
    */
    ::SetProgressState(nProgress, mpDocShell);    // Update
    pStyles->PostProcessStyles();

    if (pCollA)
        SetOutlineStyles();

    pSBase = new WW8ScannerBase(pStrm,pTableStream,pDataStream,pWwFib);

    if ( !pSBase->IsValid() )
    {
        return ERR_SWG_READ_ERROR;
    }

    static const SvxExtNumType eNumTA[16] =
    {
        SVX_NUM_ARABIC, SVX_NUM_ROMAN_UPPER, SVX_NUM_ROMAN_LOWER,
        SVX_NUM_CHARS_UPPER_LETTER_N, SVX_NUM_CHARS_LOWER_LETTER_N,
        SVX_NUM_ARABIC, SVX_NUM_ARABIC, SVX_NUM_ARABIC,
        SVX_NUM_ARABIC, SVX_NUM_ARABIC, SVX_NUM_ARABIC,
        SVX_NUM_ARABIC, SVX_NUM_ARABIC, SVX_NUM_ARABIC,
        SVX_NUM_ARABIC, SVX_NUM_ARABIC
    };

    if (pSBase->AreThereFootnotes())
    {
        static const SwFtnNum eNumA[4] =
        {
            FTNNUM_DOC, FTNNUM_CHAPTER, FTNNUM_PAGE, FTNNUM_DOC
        };

        SwFtnInfo aInfo;
        aInfo = rDoc.GetFtnInfo();      // Copy-Ctor privat

        aInfo.ePos = FTNPOS_PAGE;
        aInfo.eNum = eNumA[pWDop->rncFtn];
        aInfo.aFmt.SetNumberingType( static_cast< sal_uInt16 >(eNumTA[pWDop->nfcFtnRef]) );
        if( pWDop->nFtn )
            aInfo.nFtnOffset = pWDop->nFtn - 1;
        rDoc.SetFtnInfo( aInfo );
    }
    if( pSBase->AreThereEndnotes() )
    {
        SwEndNoteInfo aInfo;
        aInfo = rDoc.GetEndNoteInfo();  // parallel zu Ftn

        // Ich kann nicht setzen, wann neu nummerieren...
        //  aInfo.eNum = eNumA[pWDop->pDop->rncEdn];
        aInfo.aFmt.SetNumberingType( static_cast< sal_uInt16 >(eNumTA[pWDop->nfcEdnRef]) );
        if( pWDop->nEdn )
            aInfo.nFtnOffset = pWDop->nEdn - 1;
        rDoc.SetEndNoteInfo( aInfo );
    }

    if( pWwFib->lcbPlcfhdd )
        pHdFt = new WW8PLCF_HdFt( pTableStream, *pWwFib, *pWDop );

    if (!mbNewDoc)
    {
        // inserting into an existing document:
        // As only complete paragraphs are inserted, the current one
        // needs to be splitted - once or even twice.
        const SwPosition* pPos = pPaM->GetPoint();

        // split current paragraph to get new paragraph for the insertion
        rDoc.SplitNode( *pPos, false );

        // another split, if insertion position was not at the end of the current paragraph.
        SwTxtNode const*const pTxtNd = pPos->nNode.GetNode().GetTxtNode();
        if ( pTxtNd->GetTxt().Len() )
        {
            rDoc.SplitNode( *pPos, false );
            // move PaM back to the newly empty paragraph
            pPaM->Move( fnMoveBackward );
        }

        // suppress insertion of tables inside footnotes.
        const sal_uLong nNd = pPos->nNode.GetIndex();
        bReadNoTbl = ( nNd < rDoc.GetNodes().GetEndOfInserts().GetIndex() &&
                       rDoc.GetNodes().GetEndOfInserts().StartOfSectionIndex() < nNd );

    }

    ::SetProgressState(nProgress, mpDocShell);    // Update

    // loop for each glossary entry and add dummy section node
    if (pGloss)
    {
        WW8PLCF aPlc(pTableStream, pWwFib->fcPlcfglsy, pWwFib->lcbPlcfglsy, 0);
        if ( aPlc.IsValid() )
        {
            WW8_CP nStart, nEnd;
            void* pDummy;

            for (int i=0; i<pGloss->GetNoStrings(); i++,aPlc++)
            {
                SwNodeIndex aIdx( rDoc.GetNodes().GetEndOfContent());
                SwTxtFmtColl* pColl =
                    rDoc.GetTxtCollFromPool(RES_POOLCOLL_STANDARD,
                    false);
                SwStartNode *pNode =
                    rDoc.GetNodes().MakeTextSection(aIdx,
                    SwNormalStartNode,pColl);
                pPaM->GetPoint()->nNode = pNode->GetIndex()+1;
                pPaM->GetPoint()->nContent.Assign(pPaM->GetCntntNode(),0);
                aPlc.Get( nStart, nEnd, pDummy );
                ReadText(nStart,nEnd-nStart-1,MAN_MAINTEXT);
            }
        }
    }
    else //ordinary case
    {
        //Modify here for #119405, by easyfan, 2012-05-24
        mbOnLoadingMain = true;
        //End of modification, by easyfan
        ReadText( 0, pWwFib->ccpText, MAN_MAINTEXT );
        //Modify here for #119405, by easyfan, 2012-05-24
        mbOnLoadingMain = false;
        //End of modification, by easyfan
    }

    ::SetProgressState(nProgress, mpDocShell);    // Update

    if (pDrawPg && pMSDffManager && pMSDffManager->GetShapeOrders())
    {
        // Hilfsarray zum Verketten der (statt SdrTxtObj) eingefuegten
        // Rahmen
        SvxMSDffShapeTxBxSort aTxBxSort;

        // korrekte Z-Order der eingelesen Escher-Objekte sicherstellen
        sal_uInt16 nShapeCount = pMSDffManager->GetShapeOrders()->Count();

        for (sal_uInt16 nShapeNum=0; nShapeNum < nShapeCount; nShapeNum++)
        {
            SvxMSDffShapeOrder *pOrder =
                pMSDffManager->GetShapeOrders()->GetObject(nShapeNum);
            // Pointer in neues Sort-Array einfuegen
            if (pOrder->nTxBxComp && pOrder->pFly)
                aTxBxSort.Insert(pOrder);
        }
        // zu verkettende Rahmen jetzt verketten
        sal_uInt16 nTxBxCount = aTxBxSort.Count();
        if( nTxBxCount )
        {
            SwFmtChain aChain;
            for (sal_uInt16 nTxBxNum=0; nTxBxNum < nTxBxCount; nTxBxNum++)
            {
                SvxMSDffShapeOrder *pOrder =
                    aTxBxSort.GetObject(nTxBxNum);

                // Fly-Frame-Formate initialisieren
                SwFlyFrmFmt* pFlyFmt     = pOrder->pFly;
                SwFlyFrmFmt* pNextFlyFmt = 0;
                SwFlyFrmFmt* pPrevFlyFmt = 0;
                // ggfs. Nachfolger ermitteln
                if( 1+nTxBxNum < nTxBxCount )
                {
                    SvxMSDffShapeOrder *pNextOrder =
                        aTxBxSort.GetObject(nTxBxNum+1);
                    if ((0xFFFF0000 & pOrder->nTxBxComp)
                           == (0xFFFF0000 & pNextOrder->nTxBxComp))
                        pNextFlyFmt = pNextOrder->pFly;
                }
                // ggfs. Vorgaenger ermitteln
                if( nTxBxNum )
                {
                    SvxMSDffShapeOrder *pPrevOrder =
                        aTxBxSort.GetObject(nTxBxNum-1);
                    if ((0xFFFF0000 & pOrder->nTxBxComp)
                           == (0xFFFF0000 & pPrevOrder->nTxBxComp))
                        pPrevFlyFmt = pPrevOrder->pFly;
                }
                // Falls Nachfolger oder Vorgaenger vorhanden,
                // die Verkettung am Fly-Frame-Format eintragen
                if (pNextFlyFmt || pPrevFlyFmt)
                {
                    aChain.SetNext( pNextFlyFmt );
                    aChain.SetPrev( pPrevFlyFmt );
                    pFlyFmt->SetFmtAttr( aChain );
                }
            }

        }

    }

    if (mbNewDoc)
    {
        if( pWDop->fRevMarking )
            eMode |= nsRedlineMode_t::REDLINE_ON;
        if( pWDop->fRMView )
            eMode |= nsRedlineMode_t::REDLINE_SHOW_DELETE;
        if (pStg && !pGloss) /*meaningless for a glossary, cmc*/
        {
            const SvtFilterOptions* pVBAFlags = SvtFilterOptions::Get();
            maTracer.EnterEnvironment(sw::log::eMacros);
            SvxImportMSVBasic aVBasic(*mpDocShell, *pStg,
                            pVBAFlags->IsLoadWordBasicCode(),
                            pVBAFlags->IsLoadWordBasicStorage() );
            String s1(CREATE_CONST_ASC("Macros"));
            String s2(CREATE_CONST_ASC("VBA"));
            int nRet = aVBasic.Import( s1, s2 );
            if( 2 & nRet )
            {
                maTracer.Log(sw::log::eContainsVisualBasic);
                rDoc.SetContainsMSVBasic(true);
            }

            StoreMacroCmds();

            maTracer.LeaveEnvironment(sw::log::eMacros);
        }
    }

    maInsertedTables.DelAndMakeTblFrms();
    maSectionManager.InsertSegments();

    if (pCollA)
        delete[] pCollA;

    DELETEZ( pStyles );

    if( pFormImpl )
        DeleteFormImpl();
    GrafikDtor();
    DELETEZ( pMSDffManager );
    DELETEZ( pHdFt );
    //For i120928,delay the destruct action
    //DELETEZ( pLstManager );
    DELETEZ( pSBase );
    delete pWDop;
    DELETEZ( pFonts );
    delete mpAtnNames;
    DELETEZ( pAuthorInfos );
    delete mpSprmParser;
    ::EndProgress(mpDocShell);

    pDataStream = 0;
    pTableStream = 0;

    DeleteCtrlStk();
    mpRedlineStack->closeall(*pPaM->GetPoint());
    delete mpRedlineStack;
    DeleteAnchorStk();
    DeleteRefStks();

    //For i120928,achieve the graphics from the special bookmark with is for graphic bullet
    {
        std::vector<const SwGrfNode*> vecBulletGrf;
        std::vector<SwFrmFmt*> vecFrmFmt;

        IDocumentMarkAccess* const pMarkAccess =
            rDoc.getIDocumentMarkAccess();
        if ( pMarkAccess )
        {
            IDocumentMarkAccess::const_iterator_t ppBkmk =
                pMarkAccess->findBookmark( C2U("_PictureBullets") );
            if ( ppBkmk != pMarkAccess->getBookmarksEnd()
                 && IDocumentMarkAccess::GetType( *(ppBkmk->get()) ) == IDocumentMarkAccess::BOOKMARK )
            {
                SwTxtNode* pTxtNode = ppBkmk->get()->GetMarkStart().nNode.GetNode().GetTxtNode();
                if ( pTxtNode )
                {
                    const SwpHints* pHints = pTxtNode->GetpSwpHints();
                    for( sal_uInt16 nHintPos = 0; pHints && nHintPos < pHints->Count(); ++nHintPos)
                    {
                        const SwTxtAttr *pHt = (*pHints)[nHintPos];
                        const xub_StrLen st = *(pHt->GetStart());
                        if( pHt
                            && pHt->Which() == RES_TXTATR_FLYCNT
                            && (st >= ppBkmk->get()->GetMarkStart().nContent.GetIndex()) )
                        {
                            SwFrmFmt* pFrmFmt = pHt->GetFlyCnt().GetFrmFmt();
                            vecFrmFmt.push_back(pFrmFmt);
                            const SwNodeIndex* pNdIdx = pFrmFmt->GetCntnt().GetCntntIdx();
                            const SwNodes* pNodesArray = (pNdIdx != NULL)
                                                         ? &(pNdIdx->GetNodes())
                                                         : NULL;
                            const SwGrfNode *pGrf = (pNodesArray != NULL)
                                                    ? dynamic_cast<const SwGrfNode*>((*pNodesArray)[pNdIdx->GetIndex() + 1])
                                                    : NULL;
                            vecBulletGrf.push_back(pGrf);
                        }
                    }
                    // update graphic bullet information
                    sal_uInt16 nCount = pLstManager->GetWW8LSTInfoNum();
                    for (sal_uInt16 i = 0; i < nCount; ++i)
                    {
                        SwNumRule* pRule = pLstManager->GetNumRule(i);
                        for (sal_uInt16 j = 0; j < MAXLEVEL; ++j)
                        {
                            SwNumFmt aNumFmt(pRule->Get(j));
                            const sal_Int16 nType = aNumFmt.GetNumberingType();
                            const sal_uInt16 nGrfBulletCP = aNumFmt.GetGrfBulletCP();
                            if ( nType == SVX_NUM_BITMAP
                                 && vecBulletGrf.size() > nGrfBulletCP
                                 && vecBulletGrf[nGrfBulletCP] != NULL )
                            {
                                Graphic aGraphic = vecBulletGrf[nGrfBulletCP]->GetGrf();
                                SvxBrushItem aBrush(aGraphic, GPOS_AREA, SID_ATTR_BRUSH);
                                Font aFont = numfunc::GetDefBulletFont();
                                int nHeight = aFont.GetHeight() * 12;
                                Size aPrefSize( aGraphic.GetPrefSize());
                                if (aPrefSize.Height() * aPrefSize.Width() != 0 )
                                {
                                    int nWidth = (nHeight * aPrefSize.Width()) / aPrefSize.Height();
                                    Size aSize(nWidth, nHeight);
                                    aNumFmt.SetGraphicBrush(&aBrush, &aSize);
                                }
                                else
                                {
                                    aNumFmt.SetNumberingType(SVX_NUM_CHAR_SPECIAL);
                                    aNumFmt.SetBulletChar(0x2190);
                                }
                                pRule->Set( j, aNumFmt );
                            }
                        }
                    }
                    // Remove additional pictures
                    for (sal_uInt16 i = 0; i < vecFrmFmt.size(); ++i)
                    {
                        rDoc.DelLayoutFmt(vecFrmFmt[i]);
                    }
                }
            }
            DELETEZ( pLstManager );
        }
    }

    UpdateFields();

    // delete the pam before the call for hide all redlines (Bug 73683)
    if (mbNewDoc)
      rDoc.SetRedlineMode((RedlineMode_t)( eMode ));

    UpdatePageDescs(rDoc, nPageDescOffset);

    delete pPaM, pPaM = 0;
    return nErrRet;
}

sal_uLong SwWW8ImplReader::SetSubStreams(SvStorageStreamRef &rTableStream,
    SvStorageStreamRef &rDataStream)
{
    sal_uLong nErrRet = 0;
    // 6 stands for "6 OR 7",  7 stand for "ONLY 7"
    switch (pWwFib->nVersion)
    {
        case 6:
        case 7:
            pTableStream = pStrm;
            pDataStream = pStrm;
            break;
        case 8:
            if(!pStg)
            {
                ASSERT( pStg, "Version 8 muss immer einen Storage haben!" );
                nErrRet = ERR_SWG_READ_ERROR;
                break;
            }

            rTableStream = pStg->OpenSotStream( String::CreateFromAscii(
                pWwFib->fWhichTblStm ? SL::a1Table : SL::a0Table),
                STREAM_STD_READ);

            pTableStream = &rTableStream;
            pTableStream->SetNumberFormatInt( NUMBERFORMAT_INT_LITTLEENDIAN );

            rDataStream = pStg->OpenSotStream(CREATE_CONST_ASC(SL::aData),
                STREAM_STD_READ | STREAM_NOCREATE );

            if (rDataStream.Is() && SVSTREAM_OK == rDataStream->GetError())
            {
                pDataStream = &rDataStream;
                pDataStream->SetNumberFormatInt(NUMBERFORMAT_INT_LITTLEENDIAN);
            }
            else
                pDataStream = pStrm;
            break;
        default:
            // Programm-Fehler!
            ASSERT( sal_False, "It was forgotten to encode nVersion!" );
            nErrRet = ERR_SWG_READ_ERROR;
            break;
    }
    return nErrRet;
}

namespace
{
    utl::TempFile *MakeTemp(SvFileStream &rSt)
    {
        utl::TempFile *pT = new utl::TempFile;
        pT->EnableKillingFile();
        rSt.Open(pT->GetFileName(), STREAM_READWRITE | STREAM_SHARE_DENYWRITE);
        return pT;
    }

#define WW_BLOCKSIZE 0x200

    void DecryptRC4(msfilter::MSCodec_Std97& rCtx, SvStream &rIn, SvStream &rOut)
    {
        rIn.Seek(STREAM_SEEK_TO_END);
        sal_uLong nLen = rIn.Tell();
        rIn.Seek(0);

        sal_uInt8 in[WW_BLOCKSIZE];
        for (sal_uLong nI = 0, nBlock = 0; nI < nLen; nI += WW_BLOCKSIZE, ++nBlock)
        {
            sal_uLong nBS = (nLen - nI > WW_BLOCKSIZE) ? WW_BLOCKSIZE : nLen - nI;
            rIn.Read(in, nBS);
            rCtx.InitCipher(nBlock);
            rCtx.Decode(in, nBS, in, nBS);
            rOut.Write(in, nBS);
        }
    }

    void DecryptXOR(msfilter::MSCodec_XorWord95 &rCtx, SvStream &rIn, SvStream &rOut)
    {
        sal_uLong nSt = rIn.Tell();
        rIn.Seek(STREAM_SEEK_TO_END);
        sal_uLong nLen = rIn.Tell();
        rIn.Seek(nSt);

        rCtx.InitCipher();
        rCtx.Skip(nSt);

        sal_uInt8 in[0x4096];
        for (sal_uLong nI = nSt; nI < nLen; nI += 0x4096)
        {
            sal_uLong nBS = (nLen - nI > 0x4096 ) ? 0x4096 : nLen - nI;
            rIn.Read(in, nBS);
            rCtx.Decode(in, nBS);
            rOut.Write(in, nBS);
        }
    }

    //moan, copy and paste :-(
    String QueryPasswordForMedium(SfxMedium& rMedium)
    {
        String aPassw;

        using namespace com::sun::star;

        const SfxItemSet* pSet = rMedium.GetItemSet();
        const SfxPoolItem *pPasswordItem;

        if(pSet && SFX_ITEM_SET == pSet->GetItemState(SID_PASSWORD, sal_True, &pPasswordItem))
            aPassw = ((const SfxStringItem *)pPasswordItem)->GetValue();
        else
        {
            try
            {
                uno::Reference< task::XInteractionHandler > xHandler( rMedium.GetInteractionHandler() );
                if( xHandler.is() )
                {
                    ::comphelper::DocPasswordRequest* pRequest = new ::comphelper::DocPasswordRequest(
                        ::comphelper::DocPasswordRequestType_MS, task::PasswordRequestMode_PASSWORD_ENTER,
                        INetURLObject( rMedium.GetOrigURL() ).GetName( INetURLObject::DECODE_WITH_CHARSET ) );
                    uno::Reference< task::XInteractionRequest > xRequest( pRequest );

                    xHandler->handle( xRequest );

                    if( pRequest->isPassword() )
                        aPassw = pRequest->getPassword();
                }
            }
            catch( uno::Exception& )
            {
            }
        }

        return aPassw;
    }

    uno::Sequence< beans::NamedValue > InitXorWord95Codec( ::msfilter::MSCodec_XorWord95& rCodec, SfxMedium& rMedium, WW8Fib* pWwFib )
    {
        uno::Sequence< beans::NamedValue > aEncryptionData;
        SFX_ITEMSET_ARG( rMedium.GetItemSet(), pEncryptionData, SfxUnoAnyItem, SID_ENCRYPTIONDATA, sal_False );
        if ( pEncryptionData && ( pEncryptionData->GetValue() >>= aEncryptionData ) && !rCodec.InitCodec( aEncryptionData ) )
            aEncryptionData.realloc( 0 );

        if ( !aEncryptionData.getLength() )
        {
            String sUniPassword = QueryPasswordForMedium( rMedium );

            ByteString sPassword(sUniPassword, WW8Fib::GetFIBCharset( pWwFib->chseTables ) );

            xub_StrLen nLen = sPassword.Len();
            if( nLen <= 15 )
            {
                sal_uInt8 pPassword[16];
                memset( pPassword, 0, sizeof( pPassword ) );

                for (xub_StrLen nChar = 0; nChar < sPassword.Len(); ++nChar )
                    pPassword[nChar] = sPassword.GetChar(nChar);

                rCodec.InitKey( pPassword );
                aEncryptionData = rCodec.GetEncryptionData();

                // the export supports RC4 algorithm only, so we have to generate the related EncryptionData as well,
                // so that Save can export the document without asking for a password;
                // as result there will be EncryptionData for both algorithms in the MediaDescriptor
                ::msfilter::MSCodec_Std97 aCodec97;

                // Generate random number with a seed of time as salt.
                TimeValue aTime;
                osl_getSystemTime( &aTime );
                rtlRandomPool aRandomPool = rtl_random_createPool();
                rtl_random_addBytes ( aRandomPool, &aTime, 8 ); 

                sal_uInt8 pDocId[ 16 ];
                rtl_random_getBytes( aRandomPool, pDocId, 16 );

                rtl_random_destroyPool( aRandomPool );

                sal_uInt16 pStd97Pass[16];
                memset( pStd97Pass, 0, sizeof( pStd97Pass ) );
                for (xub_StrLen nChar = 0; nChar < nLen; ++nChar )
                    pStd97Pass[nChar] = sUniPassword.GetChar(nChar);

                aCodec97.InitKey( pStd97Pass, pDocId );

                // merge the EncryptionData, there should be no conflicts
                ::comphelper::SequenceAsHashMap aEncryptionHash( aEncryptionData );
                aEncryptionHash.update( ::comphelper::SequenceAsHashMap( aCodec97.GetEncryptionData() ) );
                aEncryptionHash >> aEncryptionData;
            }
        }

        return aEncryptionData;
    }

    uno::Sequence< beans::NamedValue > InitStd97Codec( ::msfilter::MSCodec_Std97& rCodec, sal_uInt8 pDocId[16], SfxMedium& rMedium )
    {
        uno::Sequence< beans::NamedValue > aEncryptionData;
        SFX_ITEMSET_ARG( rMedium.GetItemSet(), pEncryptionData, SfxUnoAnyItem, SID_ENCRYPTIONDATA, sal_False );
        if ( pEncryptionData && ( pEncryptionData->GetValue() >>= aEncryptionData ) && !rCodec.InitCodec( aEncryptionData ) )
            aEncryptionData.realloc( 0 );

        if ( !aEncryptionData.getLength() )
        {
            String sUniPassword = QueryPasswordForMedium( rMedium );

            xub_StrLen nLen = sUniPassword.Len();
            if ( nLen <= 15 )
            {
                sal_Unicode pPassword[16];
                memset( pPassword, 0, sizeof( pPassword ) );
                for (xub_StrLen nChar = 0; nChar < nLen; ++nChar )
                    pPassword[nChar] = sUniPassword.GetChar(nChar);

                rCodec.InitKey( pPassword, pDocId );
                aEncryptionData = rCodec.GetEncryptionData();
            }
        }

        return aEncryptionData;
    }
}

sal_uLong SwWW8ImplReader::LoadThroughDecryption(SwPaM& rPaM ,WW8Glossary *pGloss)
{
    sal_uLong nErrRet = 0;
    if (pGloss)
        pWwFib = pGloss->GetFib();
    else
        pWwFib = new WW8Fib(*pStrm, nWantedVersion);

    if (pWwFib->nFibError)
        nErrRet = ERR_SWG_READ_ERROR;

    SvStorageStreamRef xTableStream, xDataStream;

    if (!nErrRet)
        nErrRet = SetSubStreams(xTableStream, xDataStream);

    utl::TempFile *pTempMain = 0;
    utl::TempFile *pTempTable = 0;
    utl::TempFile *pTempData = 0;
    SvFileStream aDecryptMain;
    SvFileStream aDecryptTable;
    SvFileStream aDecryptData;

    bool bDecrypt = false;
    enum {RC4, XOR, Other} eAlgo = Other;
    if (pWwFib->fEncrypted && !nErrRet)
    {
        if (!pGloss)
        {
            bDecrypt = true;
            if (8 != pWwFib->nVersion)
                eAlgo = XOR;
            else
            {
                if (pWwFib->nKey != 0)
                    eAlgo = XOR;
                else
                {
                    pTableStream->Seek(0);
                    sal_uInt32 nEncType;
                    *pTableStream >> nEncType;
                    if (nEncType == 0x10001)
                        eAlgo = RC4;
                }
            }
        }
    }

    if (bDecrypt)
    {
        nErrRet = ERRCODE_SVX_WRONGPASS;
        SfxMedium* pMedium = mpDocShell->GetMedium();

        if ( pMedium )
        {
            switch (eAlgo)
            {
                default:
                    nErrRet = ERRCODE_SVX_READ_FILTER_CRYPT;
                    break;
                case XOR:
                {
                    msfilter::MSCodec_XorWord95 aCtx;
                    uno::Sequence< beans::NamedValue > aEncryptionData = InitXorWord95Codec( aCtx, *pMedium, pWwFib );

                    // if initialization has failed the EncryptionData should be empty
                    if ( aEncryptionData.getLength() && aCtx.VerifyKey( pWwFib->nKey, pWwFib->nHash ) )
                    {
                        nErrRet = 0;
                        pTempMain = MakeTemp(aDecryptMain);

                        pStrm->Seek(0);
                        size_t nUnencryptedHdr =
                            (8 == pWwFib->nVersion) ? 0x44 : 0x34;
                        sal_uInt8 *pIn = new sal_uInt8[nUnencryptedHdr];
                        pStrm->Read(pIn, nUnencryptedHdr);
                        aDecryptMain.Write(pIn, nUnencryptedHdr);
                        delete [] pIn;

                        DecryptXOR(aCtx, *pStrm, aDecryptMain);

                        if (!pTableStream || pTableStream == pStrm)
                            pTableStream = &aDecryptMain;
                        else
                        {
                            pTempTable = MakeTemp(aDecryptTable);
                            DecryptXOR(aCtx, *pTableStream, aDecryptTable);
                            pTableStream = &aDecryptTable;
                        }

                        if (!pDataStream || pDataStream == pStrm)
                            pDataStream = &aDecryptMain;
                        else
                        {
                            pTempData = MakeTemp(aDecryptData);
                            DecryptXOR(aCtx, *pDataStream, aDecryptData);
                            pDataStream = &aDecryptData;
                        }

                        pMedium->GetItemSet()->ClearItem( SID_PASSWORD );
                        pMedium->GetItemSet()->Put( SfxUnoAnyItem( SID_ENCRYPTIONDATA, uno::makeAny( aEncryptionData ) ) );
                    }
                }
                break;
                case RC4:
                {
                    msfilter::MSCodec_Std97 aCtx;

                    sal_uInt8 aDocId[ 16 ];
                    pTableStream->Read(aDocId, 16);
                    sal_uInt8 aSaltData[ 16 ];
                    pTableStream->Read(aSaltData, 16);
                    sal_uInt8 aSaltHash[ 16 ];
                    pTableStream->Read(aSaltHash, 16);

                    // if initialization has failed the EncryptionData should be empty
                    uno::Sequence< beans::NamedValue > aEncryptionData = InitStd97Codec( aCtx, aDocId, *pMedium );
                    if ( aEncryptionData.getLength() && aCtx.VerifyKey( aSaltData, aSaltHash ) )
                    {
                        nErrRet = 0;

                        pTempTable = MakeTemp(aDecryptTable);
                        DecryptRC4(aCtx, *pTableStream, aDecryptTable);
                        pTableStream = &aDecryptTable;

                        pTempMain = MakeTemp(aDecryptMain);
                        DecryptRC4(aCtx, *pStrm, aDecryptMain);

                        if (!pDataStream || pDataStream == pStrm)
                            pDataStream = &aDecryptMain;
                        else
                        {
                            pTempData = MakeTemp(aDecryptData);
                            DecryptRC4(aCtx, *pDataStream, aDecryptData);
                            pDataStream = &aDecryptData;
                        }

                        pMedium->GetItemSet()->ClearItem( SID_PASSWORD );
                        pMedium->GetItemSet()->Put( SfxUnoAnyItem( SID_ENCRYPTIONDATA, uno::makeAny( aEncryptionData ) ) );
                    }
                }
                break;
            }
        }

        if (nErrRet == 0)
        {
            pStrm = &aDecryptMain;

            delete pWwFib;
            pWwFib = new WW8Fib(*pStrm, nWantedVersion);
            if (pWwFib->nFibError)
                nErrRet = ERR_SWG_READ_ERROR;
        }
    }

    if (!nErrRet)
        nErrRet = CoreLoad(pGloss, *rPaM.GetPoint());

    delete pTempMain;
    delete pTempTable;
    delete pTempData;

    if (!pGloss)
        delete pWwFib;
    return nErrRet;
}

class outlineeq : public std::unary_function<const SwTxtFmtColl*, bool>
{
private:
    sal_uInt8 mnNum;
public:
    outlineeq(sal_uInt8 nNum) : mnNum(nNum) {}
    bool operator()(const SwTxtFmtColl *pTest) const
    {
        //return pTest->GetOutlineLevel() == mnNum;	//#outline level,zhaojianwei
		return pTest->IsAssignedToListLevelOfOutlineStyle() && pTest->GetAssignedOutlineStyleLevel() == mnNum;	//<-end,zhaojianwei
    }
};


void SwWW8ImplReader::SetOutlineStyles()
{

    // If we are inserted into a document then don't clobber existing outline levels.
    sal_uInt16 nOutlineStyleListLevelWithAssignment = 0;
    if ( !mbNewDoc )
    {
        sw::ParaStyles aOutLined( sw::util::GetParaStyles( rDoc ) );
        sw::util::SortByAssignedOutlineStyleListLevel( aOutLined );
        sw::ParaStyles::reverse_iterator aEnd = aOutLined.rend();
        for ( sw::ParaStyles::reverse_iterator aIter = aOutLined.rbegin(); aIter < aEnd; ++aIter )
        {
            if ( ( *aIter )->IsAssignedToListLevelOfOutlineStyle() )
                nOutlineStyleListLevelWithAssignment |= 1 << ( *aIter )->GetAssignedOutlineStyleLevel();
            else
                break;
        }
    }

    // Check applied WW8 list styles at WW8 Built-In Heading Styles
    // - Choose the list style which occurs most often as the one which provides
    //   the list level properties for the Outline Style.
    // - Populate temporary list of WW8 Built-In Heading Styles for further iteration
    std::vector< SwWW8StyInf* > aWW8BuiltInHeadingStyles;
    const SwNumRule* pChosenWW8ListStyle = NULL;
    {
        std::map< const SwNumRule*, int > aWW8ListStyleCounts;
        for ( sal_uInt16 nI = 0; nI < nColls; ++nI )
        {
            SwWW8StyInf& rSI = pCollA[nI];

            if ( !rSI.IsWW8BuiltInHeadingStyle()
                 || !rSI.HasWW8OutlineLevel() )
            {
                continue;
            }

            aWW8BuiltInHeadingStyles.push_back( &rSI );

            const SwNumRule* pWW8ListStyle = rSI.GetOutlineNumrule();
            if ( pWW8ListStyle != NULL )
            {
                std::map< const SwNumRule*, int >::iterator aCountIter =
                        aWW8ListStyleCounts.find( pWW8ListStyle );
                if ( aCountIter == aWW8ListStyleCounts.end() )
                {
                    aWW8ListStyleCounts[pWW8ListStyle] = 1;
                }
                else
                {
                    ++(aCountIter->second);
                }
            }
        }

        int nCurrentMaxCount = 0;
        std::map< const SwNumRule*, int >::iterator aCountIterEnd =
            aWW8ListStyleCounts.end();
        for ( std::map< const SwNumRule*, int >::iterator aIter = aWW8ListStyleCounts.begin();
              aIter != aCountIterEnd;
              ++aIter )
        {
            if ( aIter->second > nCurrentMaxCount )
            {
                nCurrentMaxCount = aIter->second;
                pChosenWW8ListStyle = aIter->first;
            }
        }
    }

    // - set list level properties of Outline Style - ODF's list style applied by default to headings
    // - assign corresponding Heading Paragraph Styles to the Outline Style
    // - If a heading Paragraph Styles is not applying the WW8 list style which had been chosen as
    //   the one which provides the list level properties for the Outline Style, its assignment to
    //   the Outline Style is removed. A potential applied WW8 list style is assigned directly and
    //   its default outline level is applied.
    SwNumRule aOutlineRule( *rDoc.GetOutlineNumRule() );
    bool bAppliedChangedOutlineStyle = false;
    std::vector< SwWW8StyInf* >::iterator aStylesIterEnd =
        aWW8BuiltInHeadingStyles.end();
    for ( std::vector< SwWW8StyInf* >::iterator aStyleIter = aWW8BuiltInHeadingStyles.begin();
          aStyleIter != aStylesIterEnd;
          ++aStyleIter )
    {
        SwWW8StyInf* pStyleInf = (*aStyleIter);

        const sal_uInt16 nOutlineStyleListLevelOfWW8BuiltInHeadingStyle = 1 << pStyleInf->mnWW8OutlineLevel;
        if ( nOutlineStyleListLevelOfWW8BuiltInHeadingStyle & nOutlineStyleListLevelWithAssignment )
        {
            continue;
        }

        if ( pChosenWW8ListStyle != NULL
             && pStyleInf->mnWW8OutlineLevel == pStyleInf->nListLevel )
        {
            const SwNumFmt& rRule = pChosenWW8ListStyle->Get( pStyleInf->mnWW8OutlineLevel );
            aOutlineRule.Set( pStyleInf->mnWW8OutlineLevel, rRule );
            bAppliedChangedOutlineStyle = true;
        }

        // in case that there are more styles on this level ignore them
        nOutlineStyleListLevelWithAssignment |= nOutlineStyleListLevelOfWW8BuiltInHeadingStyle;

        SwTxtFmtColl* pTxtFmtColl = static_cast<SwTxtFmtColl*>(pStyleInf->pFmt);
        if ( pStyleInf->GetOutlineNumrule() != pChosenWW8ListStyle
             || ( pStyleInf->nListLevel < WW8ListManager::nMaxLevel
                  && pStyleInf->mnWW8OutlineLevel != pStyleInf->nListLevel ) )
        {
            // WW8 Built-In Heading Style does not apply the chosen one.
            // --> delete assignment to OutlineStyle, but keep its current outline level
            pTxtFmtColl->DeleteAssignmentToListLevelOfOutlineStyle( false );
            // Apply existing WW8 list style a normal list style at the Paragraph Style
            if ( pStyleInf->GetOutlineNumrule() != NULL )
            {
                pTxtFmtColl->SetFmtAttr( SwNumRuleItem( pStyleInf->GetOutlineNumrule()->GetName() ) );
            }
            // apply default outline level of WW8 Built-in Heading Style
            const sal_uInt8 nOutlineLevel =
                    SwWW8StyInf::WW8OutlineLevelToOutlinelevel( pStyleInf->mnWW8OutlineLevel );
            pTxtFmtColl->SetFmtAttr( SfxUInt16Item( RES_PARATR_OUTLINELEVEL, nOutlineLevel ) );
        }
        else
        {
            pTxtFmtColl->AssignToListLevelOfOutlineStyle( pStyleInf->mnWW8OutlineLevel );
        }
    }

    if ( bAppliedChangedOutlineStyle )
    {
        rDoc.SetOutlineNumRule(aOutlineRule);
    }
}

const String* SwWW8ImplReader::GetAnnotationAuthor(sal_uInt16 nIdx)
{
    if (!mpAtnNames && pWwFib->lcbGrpStAtnOwners)
    {
        // Authoren bestimmen: steht im TableStream
        mpAtnNames = new ::std::vector<String>;
        SvStream& rStrm = *pTableStream;

        long nOldPos = rStrm.Tell();
        rStrm.Seek( pWwFib->fcGrpStAtnOwners );

        long nRead = 0, nCount = pWwFib->lcbGrpStAtnOwners;
        while (nRead < nCount)
        {
            if( bVer67 )
            {
                mpAtnNames->push_back(WW8ReadPString(rStrm, false));
                nRead += mpAtnNames->rbegin()->Len() + 1;   // Laenge + sal_uInt8 Count
            }
            else
            {
                mpAtnNames->push_back(WW8Read_xstz(rStrm, 0, false));
                // UNICode: doppelte Laenge + sal_uInt16 Count
                nRead += mpAtnNames->rbegin()->Len() * 2 + 2;
            }
        }
        rStrm.Seek( nOldPos );
    }

    const String *pRet = 0;
    if (mpAtnNames && nIdx < mpAtnNames->size())
        pRet = &((*mpAtnNames)[nIdx]);
    return pRet;
}

sal_uLong SwWW8ImplReader::LoadDoc( SwPaM& rPaM,WW8Glossary *pGloss)
{
    sal_uLong nErrRet = 0;

    {
        static const sal_Char* aNames[ 13 ] = {
            "WinWord/WW", "WinWord/WW8", "WinWord/WWFT",
            "WinWord/WWFLX", "WinWord/WWFLY",
            "WinWord/WWF",
            "WinWord/WWFA0", "WinWord/WWFA1", "WinWord/WWFA2",
            "WinWord/WWFB0", "WinWord/WWFB1", "WinWord/WWFB2",
            "WinWord/RegardHindiDigits"
        };
        sal_uInt32 aVal[ 13 ];

        SwFilterOptions aOpt( 13, aNames, aVal );

        nIniFlags = aVal[ 0 ];
        nIniFlags1= aVal[ 1 ];
        // schiebt Flys um x twips nach rechts o. links
        nIniFlyDx = aVal[ 3 ];
        nIniFlyDy = aVal[ 4 ];

        nFieldFlags = aVal[ 5 ];
        nFieldTagAlways[0] = aVal[ 6 ];
        nFieldTagAlways[1] = aVal[ 7 ];
        nFieldTagAlways[2] = aVal[ 8 ];
        nFieldTagBad[0] = aVal[ 9 ];
        nFieldTagBad[1] = aVal[ 10 ];
        nFieldTagBad[2] = aVal[ 11 ];
        m_bRegardHindiDigits = aVal[ 12 ] > 0;
    }

    sal_uInt16 nMagic;
    *pStrm >> nMagic;

    // beachte: 6 steht fuer "6 ODER 7",  7 steht fuer "NUR 7"
    switch (nWantedVersion)
    {
        case 6:
        case 7:
            if (
                (0xa5dc != nMagic && 0xa5db != nMagic) &&
                (nMagic < 0xa697 || nMagic > 0xa699)
               )
            {
                //JP 06.05.99: teste auf eigenen 97-Fake!
                if (pStg && 0xa5ec == nMagic)
                {
                    sal_uLong nCurPos = pStrm->Tell();
                    if (pStrm->Seek(nCurPos + 22))
                    {
                        sal_uInt32 nfcMin;
                        *pStrm >> nfcMin;
                        if (0x300 != nfcMin)
                            nErrRet = ERR_WW6_NO_WW6_FILE_ERR;
                    }
                    pStrm->Seek( nCurPos );
                }
                else
                    nErrRet = ERR_WW6_NO_WW6_FILE_ERR;
            }
            break;
        case 8:
            if (0xa5ec != nMagic)
                nErrRet = ERR_WW8_NO_WW8_FILE_ERR;
            break;
        default:
            nErrRet = ERR_WW8_NO_WW8_FILE_ERR;
            ASSERT( sal_False, "It was forgotten to encode nVersion!" );
            break;
    }

    if (!nErrRet)
        nErrRet = LoadThroughDecryption(rPaM ,pGloss);

    rDoc.PropagateOutlineRule();

    return nErrRet;
}

extern "C" SAL_DLLPUBLIC_EXPORT Reader* SAL_CALL ImportDOC()
{
    return new WW8Reader();
}

sal_uLong WW8Reader::Read(SwDoc &rDoc, const String& rBaseURL, SwPaM &rPam, const String & /* FileName */)
{
    sal_uInt16 nOldBuffSize = 32768;
    bool bNew = !bInsertMode;               // Neues Doc ( kein Einfuegen )


    SvStorageStreamRef refStrm;         // damit uns keiner den Stream klaut
    SvStream* pIn = pStrm;

    sal_uLong nRet = 0;
    sal_uInt8 nVersion = 8;

    String sFltName = GetFltName();
    if( sFltName.EqualsAscii( "WW6" ) )
    {
        if (pStrm)
            nVersion = 6;
        else
        {
            ASSERT(sal_False, "WinWord 95 Reader-Read without stream");
            nRet = ERR_SWG_READ_ERROR;
        }
    }
    else
    {
        if( sFltName.EqualsAscii( "CWW6" ) )
            nVersion = 6;
        else if( sFltName.EqualsAscii( "CWW7" ) )
            nVersion = 7;

        if( pStg )
        {
            nRet = OpenMainStream( refStrm, nOldBuffSize );
            pIn = &refStrm;
        }
        else
        {
            ASSERT(sal_False, "WinWord 95/97 Reader-Read without storage");
            nRet = ERR_SWG_READ_ERROR;
        }
    }

    if( !nRet )
    {
        //JP 18.01.96: Alle Ueberschriften sind normalerweise ohne
        //              Kapitelnummer. Darum hier explizit abschalten
        //              weil das Default jetzt wieder auf AN ist.
        if (bNew)
        {
            Reader::SetNoOutlineNum( rDoc );
            // MIB 27.09.96: Umrandung uns Abstaende aus Frm-Vorlagen entf.
            Reader::ResetFrmFmts( rDoc );
        }
        SwWW8ImplReader* pRdr = new SwWW8ImplReader(nVersion, pStg, pIn, rDoc,
            rBaseURL, bNew);
        try
        {
            nRet = pRdr->LoadDoc( rPam );
        }
        catch( const std::exception& )
        {
            nRet = ERR_WW8_NO_WW8_FILE_ERR;
        }
        delete pRdr;

        if( refStrm.Is() )
        {
            refStrm->SetBufferSize( nOldBuffSize );
            refStrm.Clear();
        }
        else if (pIn)
            pIn->ResetError();

    }
    return nRet;
}

int WW8Reader::GetReaderType()
{
    return SW_STORAGE_READER | SW_STREAM_READER;
}

sal_Bool WW8Reader::HasGlossaries() const
{
    return true;
}

sal_Bool WW8Reader::ReadGlossaries(SwTextBlocks& rBlocks, sal_Bool bSaveRelFiles) const
{
    bool bRet=false;

    WW8Reader *pThis = const_cast<WW8Reader *>(this);

    sal_uInt16 nOldBuffSize = 32768;
    SvStorageStreamRef refStrm;
    if (!pThis->OpenMainStream(refStrm, nOldBuffSize))
    {
        WW8Glossary aGloss( refStrm, 8, pStg );
        bRet = aGloss.Load( rBlocks, bSaveRelFiles ? true : false);
    }
    return bRet ? true : false;
}

sal_Bool SwMSDffManager::GetOLEStorageName(long nOLEId, String& rStorageName,
    SvStorageRef& rSrcStorage, uno::Reference < embed::XStorage >& rDestStorage) const
{
    bool bRet = false;

    long nPictureId = 0;
    if (rReader.pStg)
    {
        // dann holen wir uns mal ueber den TextBox-PLCF die richtigen
        // Char Start-/End-Positionen. In dem Bereich sollte dann
        // das EinbettenFeld und die entsprechenden Sprms zu finden
        // sein. Wir brauchen hier aber nur das Sprm fuer die Picture Id
        long nOldPos = rReader.pStrm->Tell();
        {
            // #i32596# - consider return value of method
            // <rReader.GetTxbxTextSttEndCp(..)>. If it returns false, method
            // wasn't successful. Thus, continue in this case.
            // Note: Ask MM for initialization of <nStartCp> and <nEndCp>.
            // Note: Ask MM about assertions in method <rReader.GetTxbxTextSttEndCp(..)>.
            WW8_CP nStartCp, nEndCp;
            if ( rReader.GetTxbxTextSttEndCp(nStartCp, nEndCp,
                            static_cast<sal_uInt16>((nOLEId >> 16) & 0xFFFF),
                            static_cast<sal_uInt16>(nOLEId & 0xFFFF)) )
            {
                WW8PLCFxSaveAll aSave;
                memset( &aSave, 0, sizeof( aSave ) );
                rReader.pPlcxMan->SaveAllPLCFx( aSave );

                nStartCp += rReader.nDrawCpO;
                nEndCp   += rReader.nDrawCpO;
                WW8PLCFx_Cp_FKP* pChp = rReader.pPlcxMan->GetChpPLCF();
                wwSprmParser aSprmParser(rReader.pWwFib->GetFIBVersion());
                while (nStartCp <= nEndCp && !nPictureId)
                {
                    WW8PLCFxDesc aDesc;
                    pChp->SeekPos( nStartCp );
                    pChp->GetSprms( &aDesc );

                    if (aDesc.nSprmsLen && aDesc.pMemPos)   // Attribut(e) vorhanden
                    {
                        long nLen = aDesc.nSprmsLen;
                        const sal_uInt8* pSprm = aDesc.pMemPos;

                        while (nLen >= 2 && !nPictureId)
                        {
                            sal_uInt16 nId = aSprmParser.GetSprmId(pSprm);
                            sal_uInt16 nSL = aSprmParser.GetSprmSize(nId, pSprm);

                            if( nLen < nSL )
                                break;              // nicht mehr genug Bytes uebrig

                            if( 0x6A03 == nId && 0 < nLen )
                            {
                                nPictureId = SVBT32ToUInt32(pSprm +
                                    aSprmParser.DistanceToData(nId));
                                bRet = true;
                            }
                            pSprm += nSL;
                            nLen -= nSL;
                        }
                    }
                    nStartCp = aDesc.nEndPos;
                }

                rReader.pPlcxMan->RestoreAllPLCFx( aSave );
            }
        }
        rReader.pStrm->Seek( nOldPos );
    }

    if( bRet )
    {
        rStorageName = '_';
        rStorageName += String::CreateFromInt32(nPictureId);
        rSrcStorage = rReader.pStg->OpenSotStorage(CREATE_CONST_ASC(
            SL::aObjectPool));
        if (!rReader.mpDocShell)
            bRet=false;
        else
            rDestStorage = rReader.mpDocShell->GetStorage();
    }
    return bRet;
}

sal_Bool SwMSDffManager::ShapeHasText(sal_uLong, sal_uLong) const
{
    // Zur Zeit des Einlesens einer einzelnen Box, die womoeglich Teil einer
    // Gruppe ist, liegen noch nicht genuegend Informationen vor, um
    // entscheiden zu koennen, ob wir sie nicht doch als Textfeld benoetigen.
    // Also vorsichtshalber mal alle umwandeln:
    return true;
}

bool SwWW8ImplReader::InEqualOrHigherApo(int nLvl) const
{
    if (nLvl)
        --nLvl;
    // #i60827# - check size of <maApos> to assure that <maApos.begin() + nLvl> can be performed.
    if ( sal::static_int_cast< sal_Int32>(nLvl) >= sal::static_int_cast< sal_Int32>(maApos.size()) )
    {
        return false;
    }
    mycApoIter aIter = std::find(maApos.begin() + nLvl, maApos.end(), true);
    if (aIter != maApos.end())
        return true;
    else
        return false;
}

bool SwWW8ImplReader::InEqualApo(int nLvl) const
{
    //If we are in a table, see if an apo was inserted at the level below
    //the table.
    if (nLvl)
        --nLvl;
    return maApos[nLvl];
}

namespace sw
{
    namespace hack
    {
        Position::Position(const SwPosition &rPos)
            : maPtNode(rPos.nNode), mnPtCntnt(rPos.nContent.GetIndex())
        {
        }

        Position::Position(const Position &rPos)
            : maPtNode(rPos.maPtNode), mnPtCntnt(rPos.mnPtCntnt)
        {
        }

        Position::operator SwPosition() const
        {
            SwPosition aRet(maPtNode);
            aRet.nContent.Assign(maPtNode.GetNode().GetCntntNode(), mnPtCntnt);
            return aRet;
        }
    }
}

/* vi:set tabstop=4 shiftwidth=4 expandtab: */

SwMacroInfo::SwMacroInfo() :
    SdrObjUserData( SW_DRAWLAYER, SW_UD_IMAPDATA, 0 )
{
}

SwMacroInfo::~SwMacroInfo()
{
}

SdrObjUserData* SwMacroInfo::Clone( SdrObject* /*pObj*/ ) const
{
   return new SwMacroInfo( *this );
}



