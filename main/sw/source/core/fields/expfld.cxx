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

#include <hintids.hxx>
#include <unotools/collatorwrapper.hxx>
#include <unotools/charclass.hxx>
#include <editeng/unolingu.hxx>
#include <svx/pageitem.hxx>
#include <editeng/langitem.hxx>
#include <editeng/fontitem.hxx>
#include <com/sun/star/text/SetVariableType.hpp>
#include <unofield.hxx>
#include <frmfmt.hxx>
#include <fmtfld.hxx>
#include <txtfld.hxx>
#include <fmtanchr.hxx>
#include <txtftn.hxx>
#include <doc.hxx>
#include <layfrm.hxx>
#include <pagefrm.hxx>
#include <cntfrm.hxx>
#include <rootfrm.hxx>
#include <tabfrm.hxx>
#include <flyfrm.hxx>
#include <ftnfrm.hxx>
#include <rowfrm.hxx>
#include <expfld.hxx>
#include <usrfld.hxx>
#include <ndtxt.hxx>
#include <calc.hxx>
#include <pam.hxx>
#include <docfld.hxx>
#include <swcache.hxx>
#include <swtable.hxx>
#include <breakit.hxx>
#include <SwStyleNameMapper.hxx>
#include <unofldmid.h>
#include <numrule.hxx>
#include <switerator.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::text;
using ::rtl::OUString;

SV_IMPL_PTRARR( _SwSeqFldList, _SeqFldLstElem* )

//-----------------------------------------------------------------------------
sal_Int16 lcl_SubTypeToAPI(sal_uInt16 nSubType)
{
		sal_Int16 nRet = 0;
		switch(nSubType)
		{
			case nsSwGetSetExpType::GSE_EXPR:
                nRet = SetVariableType::VAR;      // 0
                break;
			case nsSwGetSetExpType::GSE_SEQ:
                nRet = SetVariableType::SEQUENCE; // 1
                break;
			case nsSwGetSetExpType::GSE_FORMULA:
                nRet = SetVariableType::FORMULA;  // 2
                break;
			case nsSwGetSetExpType::GSE_STRING:
                nRet = SetVariableType::STRING;   // 3
                break;
		}
		return nRet;
}
//-----------------------------------------------------------------------------
sal_Int32 lcl_APIToSubType(const uno::Any& rAny)
{
		sal_Int16 nVal = 0;
		rAny >>= nVal;
		sal_Int32 nSet = 0;
		switch(nVal)
		{
			case SetVariableType::VAR:		nSet = nsSwGetSetExpType::GSE_EXPR;  break;
			case SetVariableType::SEQUENCE: nSet = nsSwGetSetExpType::GSE_SEQ;  break;
			case SetVariableType::FORMULA:  nSet = nsSwGetSetExpType::GSE_FORMULA; break;
			case SetVariableType::STRING:	nSet = nsSwGetSetExpType::GSE_STRING;	break;
			default:
				DBG_ERROR("wrong value");
				nSet = -1;
		}
		return nSet;
}

//-----------------------------------------------------------------------------

void ReplacePoint( String& rTmpName, sal_Bool bWithCommandType )
{
    // replace first and last (if bWithCommandType: last two) dot Ersten und letzten Punkt ersetzen, da in Tabellennamen Punkte erlaubt sind
    // since table names may contain dots

	xub_StrLen nLen = rTmpName.Len();
	sal_Unicode *pStr = rTmpName.GetBufferAccess(), *pBackStr = pStr + nLen;

    long nBackCount = bWithCommandType ? 2 : 1;
    xub_StrLen i;

    for( i = nLen; i; --i, pBackStr-- )
		if( '.' == *pBackStr )
		{
			*pBackStr = DB_DELIM;
            if(!--nBackCount)
                break;
		}
	for( i = 0; i < nLen; ++i, ++pStr )
		if( '.' == *pStr )
		{
			*pStr = DB_DELIM;
			break;
		}
}

SwTxtNode* GetFirstTxtNode( const SwDoc& rDoc, SwPosition& rPos,
							const SwCntntFrm *pCFrm, Point &rPt )
{
	SwTxtNode* pTxtNode = 0;
	if ( !pCFrm )
	{
        const SwNodes& rNodes = rDoc.GetNodes();
        rPos.nNode = *rNodes.GetEndOfContent().StartOfSectionNode();
		SwCntntNode* pCNd;
        while( 0 != (pCNd = rNodes.GoNext( &rPos.nNode ) ) &&
				0 == ( pTxtNode = pCNd->GetTxtNode() ) )
						;
		ASSERT( pTxtNode, "wo ist der 1.TextNode" );
		rPos.nContent.Assign( pTxtNode, 0 );
	}
	else if ( !pCFrm->IsValid() )
	{
		pTxtNode = (SwTxtNode*)pCFrm->GetNode();
		rPos.nNode = *pTxtNode;
		rPos.nContent.Assign( pTxtNode, 0 );
	}
	else
	{
		pCFrm->GetCrsrOfst( &rPos, rPt );
		pTxtNode = rPos.nNode.GetNode().GetTxtNode();
	}
	return pTxtNode;
}

const SwTxtNode* GetBodyTxtNode( const SwDoc& rDoc, SwPosition& rPos,
								const SwFrm& rFrm )
{
	const SwLayoutFrm* pLayout = (SwLayoutFrm*)rFrm.GetUpper();
	const SwTxtNode* pTxtNode = 0;

	while( pLayout )
	{
		if( pLayout->IsFlyFrm() )
		{
			// hole das FlyFormat
			SwFrmFmt* pFlyFmt = ((SwFlyFrm*)pLayout)->GetFmt();
			ASSERT( pFlyFmt, "kein FlyFormat gefunden, wo steht das Feld" );

			const SwFmtAnchor &rAnchor = pFlyFmt->GetAnchor();

			if( FLY_AT_FLY == rAnchor.GetAnchorId() )
			{
				// und der Fly muss irgendwo angehaengt sein, also
				// den befragen
                pLayout = (SwLayoutFrm*)((SwFlyFrm*)pLayout)->GetAnchorFrm();
				continue;
			}
            else if ((FLY_AT_PARA == rAnchor.GetAnchorId()) ||
                     (FLY_AT_CHAR == rAnchor.GetAnchorId()) ||
                     (FLY_AS_CHAR == rAnchor.GetAnchorId()))
			{
				ASSERT( rAnchor.GetCntntAnchor(), "keine gueltige Position" );
				rPos = *rAnchor.GetCntntAnchor();
				pTxtNode = rPos.nNode.GetNode().GetTxtNode();
                if ( FLY_AT_PARA == rAnchor.GetAnchorId() )
                {
                    const_cast<SwTxtNode*>(pTxtNode)->MakeStartIndex(
                            &rPos.nContent );
// oder doch besser das Ende vom (Anker-)TextNode nehmen ??
//					((SwTxtNode*)pTxtNode)->MakeEndIndex( &rPos.nContent );
                }

				// noch nicht abbrechen, kann ja auch noch im
				// Header/Footer/Footnote/Fly stehen !!
                pLayout = ((SwFlyFrm*)pLayout)->GetAnchorFrm()
                            ? ((SwFlyFrm*)pLayout)->GetAnchorFrm()->GetUpper() : 0;
				continue;
			}
			else
			{
				pLayout->FindPageFrm()->GetCntntPosition(
												pLayout->Frm().Pos(), rPos );
				pTxtNode = rPos.nNode.GetNode().GetTxtNode();
			}
		}
		else if( pLayout->IsFtnFrm() )
		{
			// hole den Node vom Anker
			const SwTxtFtn* pFtn = ((SwFtnFrm*)pLayout)->GetAttr();
			pTxtNode = &pFtn->GetTxtNode();
			rPos.nNode = *pTxtNode;
			rPos.nContent = *pFtn->GetStart();
		}
		else if( pLayout->IsHeaderFrm() || pLayout->IsFooterFrm() )
		{
			const SwCntntFrm* pCntFrm;
			const SwPageFrm* pPgFrm = pLayout->FindPageFrm();
			if( pLayout->IsHeaderFrm() )
			{
				const SwTabFrm *pTab;
				if( 0 != ( pCntFrm = pPgFrm->FindFirstBodyCntnt()) &&
					0 != (pTab = pCntFrm->FindTabFrm()) && pTab->IsFollow() &&
                    pTab->GetTable()->GetRowsToRepeat() > 0 &&
                    pTab->IsInHeadline( *pCntFrm ) )
                {
					// take the next line
                    const SwLayoutFrm* pRow = pTab->GetFirstNonHeadlineRow();
                    pCntFrm = pRow->ContainsCntnt();
				}
			}
			else
				pCntFrm = pPgFrm->FindLastBodyCntnt();

			if( pCntFrm )
			{
				pTxtNode = pCntFrm->GetNode()->GetTxtNode();
				rPos.nNode = *pTxtNode;
				((SwTxtNode*)pTxtNode)->MakeEndIndex( &rPos.nContent );
			}
			else
			{
				Point aPt( pLayout->Frm().Pos() );
				aPt.Y()++;		// aus dem Header raus
				pCntFrm = pPgFrm->GetCntntPos( aPt, sal_False, sal_True, sal_False );
				pTxtNode = GetFirstTxtNode( rDoc, rPos, pCntFrm, aPt );
			}
		}
		else
		{
			pLayout = pLayout->GetUpper();
			continue;
		}
		break;		// gefunden und beende die Schleife
	}
	return pTxtNode;
}

/*--------------------------------------------------------------------
	Beschreibung: SwSetExpFieldType by JP
 --------------------------------------------------------------------*/

SwGetExpFieldType::SwGetExpFieldType(SwDoc* pDc)
	: SwValueFieldType( pDc, RES_GETEXPFLD )
{
}

SwFieldType* SwGetExpFieldType::Copy() const
{
	return new SwGetExpFieldType(GetDoc());
}

void SwGetExpFieldType::Modify( const SfxPoolItem*, const SfxPoolItem* pNew )
{
	if( pNew && RES_DOCPOS_UPDATE == pNew->Which() )
		NotifyClients( 0, pNew );
	// sonst nichts weiter expandieren
}

/*--------------------------------------------------------------------
	Beschreibung: SwGetExpField by JP
 --------------------------------------------------------------------*/

SwGetExpField::SwGetExpField(SwGetExpFieldType* pTyp, const String& rFormel,
							sal_uInt16 nSub, sal_uLong nFmt)
	: SwFormulaField( pTyp, nFmt, 0.0 ),
	bIsInBodyTxt( sal_True ),
	nSubType(nSub),
	bLateInitialization( false )
{
	SetFormula( rFormel );
}

String SwGetExpField::Expand() const
{
	if(nSubType & nsSwExtendedSubType::SUB_CMD)
		return GetFormula();
	else
		return sExpand;
}

String SwGetExpField::GetFieldName() const
{
    String aStr( SwFieldType::GetTypeStr(
        static_cast<sal_uInt16>(((nsSwGetSetExpType::GSE_FORMULA & nSubType) != 0)
                                            ? TYP_FORMELFLD
                                            : TYP_GETFLD ) ));
    aStr += ' ';
    aStr += GetFormula();
    return aStr;
}

SwField* SwGetExpField::Copy() const
{
	SwGetExpField *pTmp = new SwGetExpField((SwGetExpFieldType*)GetTyp(),
											GetFormula(), nSubType, GetFormat());
	pTmp->SetLanguage(GetLanguage());
	pTmp->SwValueField::SetValue(GetValue());
	pTmp->sExpand 		= sExpand;
	pTmp->bIsInBodyTxt 	= bIsInBodyTxt;
	pTmp->SetAutomaticLanguage(IsAutomaticLanguage());
	if( bLateInitialization )
		pTmp->SetLateInitialization();

	return pTmp;
}

void SwGetExpField::ChangeExpansion( const SwFrm& rFrm, const SwTxtFld& rFld )
{
	if( bIsInBodyTxt )		// nur Felder in Footer, Header, FootNote, Flys
		return;

	ASSERT( !rFrm.IsInDocBody(), "Flag ist nicht richtig, Frame steht im DocBody" );

	// bestimme mal das Dokument (oder geht es noch einfacher?)
	const SwTxtNode* pTxtNode = &rFld.GetTxtNode();
	SwDoc& rDoc = *(SwDoc*)pTxtNode->GetDoc();

	// einen Index fuers bestimmen vom TextNode anlegen
	SwPosition aPos( SwNodeIndex( rDoc.GetNodes() ) );
	pTxtNode = GetBodyTxtNode( rDoc, aPos, rFrm );

	// Wenn kein Layout vorhanden, kommt es in Kopf und Fusszeilen dazu
	// das ChangeExpansion uebers Layout-Formatieren aufgerufen wird
	// aber kein TxtNode vorhanden ist
	//
	if(!pTxtNode)
		return;
	// #i82544#
	if( bLateInitialization )
	{
		SwFieldType* pSetExpFld = rDoc.GetFldType(RES_SETEXPFLD, GetFormula(), sal_False);
		if( pSetExpFld )
		{
			bLateInitialization = false;
			if( !(GetSubType() & nsSwGetSetExpType::GSE_STRING) &&
				static_cast< SwSetExpFieldType* >(pSetExpFld)->GetType() == nsSwGetSetExpType::GSE_STRING )
			SetSubType( nsSwGetSetExpType::GSE_STRING );
		}
	}

	_SetGetExpFld aEndFld( aPos.nNode, &rFld, &aPos.nContent );
	if(GetSubType() & nsSwGetSetExpType::GSE_STRING)
	{
		SwHash** ppHashTbl;
		sal_uInt16 nSize;
		rDoc.FldsToExpand( ppHashTbl, nSize, aEndFld );
		LookString( ppHashTbl, nSize, GetFormula(), sExpand );
		::DeleteHashTable( ppHashTbl, nSize );		// HashTabelle loeschen
	}
	else
	{
		// fuelle den Calculator mit den Werten
		SwCalc aCalc( rDoc );
		rDoc.FldsToCalc(aCalc, aEndFld);

		// Wert berechnen
		SetValue(aCalc.Calculate(GetFormula()).GetDouble());

		// Auswertung nach Format
		sExpand = ((SwValueFieldType*)GetTyp())->ExpandValue(
								GetValue(), GetFormat(), GetLanguage());
	}
}

String SwGetExpField::GetPar2() const
{
	return GetFormula();
}

void SwGetExpField::SetPar2(const String& rStr)
{
	SetFormula(rStr);
}

sal_uInt16 SwGetExpField::GetSubType() const
{
	return nSubType;
}

void SwGetExpField::SetSubType(sal_uInt16 nType)
{
	nSubType = nType;
}

void SwGetExpField::SetLanguage(sal_uInt16 nLng)
{
	if (nSubType & nsSwExtendedSubType::SUB_CMD)
		SwField::SetLanguage(nLng);
	else
		SwValueField::SetLanguage(nLng);
}

sal_Bool SwGetExpField::QueryValue( uno::Any& rAny, sal_uInt16 nWhichId ) const
{
	switch( nWhichId )
	{
	case FIELD_PROP_DOUBLE:
		rAny <<= GetValue();
		break;
	case FIELD_PROP_FORMAT:
		rAny <<= (sal_Int32)GetFormat();
		break;
	case FIELD_PROP_USHORT1:
		 rAny <<= (sal_Int16)nSubType;
		break;
	case FIELD_PROP_PAR1:
	 	rAny <<= OUString( GetFormula() );
		break;
	case FIELD_PROP_SUBTYPE:
		{
			sal_Int16 nRet = lcl_SubTypeToAPI(GetSubType() & 0xff);
			rAny <<= nRet;
		}
		break;
	case FIELD_PROP_BOOL2:
		{
			sal_Bool bTmp = 0 != (nSubType & nsSwExtendedSubType::SUB_CMD);
			rAny.setValue(&bTmp, ::getBooleanCppuType());
		}
		break;
	case FIELD_PROP_PAR4:
		rAny <<= rtl::OUString(GetExpStr());
		break;
	default:
		return SwField::QueryValue(rAny, nWhichId);
	}
	return sal_True;
}

sal_Bool SwGetExpField::PutValue( const uno::Any& rAny, sal_uInt16 nWhichId )
{
	sal_Int32 nTmp = 0;
	String sTmp;
	switch( nWhichId )
	{
	case FIELD_PROP_DOUBLE:
		SwValueField::SetValue(*(double*) rAny.getValue());
		break;
	case FIELD_PROP_FORMAT:
		rAny >>= nTmp;
		SetFormat(nTmp);
		break;
	case FIELD_PROP_USHORT1:
		 rAny >>= nTmp;
		 nSubType = static_cast<sal_uInt16>(nTmp);
		break;
	case FIELD_PROP_PAR1:
	 	SetFormula( ::GetString( rAny, sTmp ));
		break;
	case FIELD_PROP_SUBTYPE:
		nTmp = lcl_APIToSubType(rAny);
		if( nTmp >=0 )
			SetSubType( static_cast<sal_uInt16>((GetSubType() & 0xff00) | nTmp));
		break;
	case FIELD_PROP_BOOL2:
		if(*(sal_Bool*) rAny.getValue())
			nSubType |= nsSwExtendedSubType::SUB_CMD;
		else
			nSubType &= (~nsSwExtendedSubType::SUB_CMD);
		break;
	case FIELD_PROP_PAR4:
		ChgExpStr(::GetString( rAny, sTmp ));
		break;
	default:
		return SwField::PutValue(rAny, nWhichId);
	}
	return sal_True;
}

SwSetExpFieldType::SwSetExpFieldType( SwDoc* pDc, const String& rName, sal_uInt16 nTyp )
	: SwValueFieldType( pDc, RES_SETEXPFLD ),
	sName( rName ),
	pOutlChgNd( 0 ),
	sDelim( String::CreateFromAscii( "." ) ),
	nType(nTyp), nLevel( UCHAR_MAX ),
	bDeleted( sal_False )
{
	if( ( nsSwGetSetExpType::GSE_SEQ | nsSwGetSetExpType::GSE_STRING ) & nType )
		EnableFormat(sal_False);	// Numberformatter nicht einsetzen
}

SwFieldType* SwSetExpFieldType::Copy() const
{
	SwSetExpFieldType* pNew = new SwSetExpFieldType(GetDoc(), sName, nType);
	pNew->bDeleted = bDeleted;
	pNew->sDelim = sDelim;
	pNew->nLevel = nLevel;

	return pNew;
}

const String& SwSetExpFieldType::GetName() const
{
	return sName;
}

void SwSetExpFieldType::Modify( const SfxPoolItem*, const SfxPoolItem* )
{
	return;		// nicht weiter expandieren
}

void SwSetExpFieldType::SetSeqFormat(sal_uLong nFmt)
{
	SwIterator<SwFmtFld,SwFieldType> aIter(*this);
	for( SwFmtFld* pFmtFld = aIter.First(); pFmtFld; pFmtFld = aIter.Next() )
		pFmtFld->GetField()->ChangeFormat( nFmt );
}

sal_uLong SwSetExpFieldType::GetSeqFormat()
{
	if( !GetDepends() )
		return SVX_NUM_ARABIC;

	SwField *pFld = ((SwFmtFld*)GetDepends())->GetField();
	return pFld->GetFormat();
}

sal_uInt16 SwSetExpFieldType::SetSeqRefNo( SwSetExpField& rFld )
{
	if( !GetDepends() || !(nsSwGetSetExpType::GSE_SEQ & nType) )
		return USHRT_MAX;

extern void InsertSort( SvUShorts& rArr, sal_uInt16 nIdx, sal_uInt16* pInsPos = 0 );
	SvUShorts aArr( 64 );

	sal_uInt16 n;

	// dann testmal, ob die Nummer schon vergeben ist oder ob eine neue
	// bestimmt werden muss.
	SwIterator<SwFmtFld,SwFieldType> aIter( *this );
	const SwTxtNode* pNd;
	for( SwFmtFld* pF = aIter.First(); pF; pF = aIter.Next() )
		if( pF->GetField() != &rFld && pF->GetTxtFld() &&
			0 != ( pNd = pF->GetTxtFld()->GetpTxtNode() ) &&
			pNd->GetNodes().IsDocNodes() )
			InsertSort( aArr, ((SwSetExpField*)pF->GetField())->GetSeqNumber() );


	// teste erstmal ob die Nummer schon vorhanden ist:
	sal_uInt16 nNum = rFld.GetSeqNumber();
	if( USHRT_MAX != nNum )
	{
		for( n = 0; n < aArr.Count(); ++n )
			if( aArr[ n ] > nNum )
				return nNum;			// nicht vorhanden -> also benutzen
			else if( aArr[ n ] == nNum )
				break;					// schon vorhanden -> neue erzeugen

		if( n == aArr.Count() )
			return nNum;			// nicht vorhanden -> also benutzen
	}

	// alle Nummern entsprechend geflag, also bestimme die richtige Nummer
	for( n = 0; n < aArr.Count(); ++n )
		if( n != aArr[ n ] )
			break;

	rFld.SetSeqNumber( n );
	return n;
}

sal_uInt16 SwSetExpFieldType::GetSeqFldList( SwSeqFldList& rList )
{
	if( rList.Count() )
		rList.Remove( 0, rList.Count() );

	SwIterator<SwFmtFld,SwFieldType> aIter( *this );
	const SwTxtNode* pNd;
	for( SwFmtFld* pF = aIter.First(); pF; pF = aIter.Next() )
		if( pF->GetTxtFld() &&
			0 != ( pNd = pF->GetTxtFld()->GetpTxtNode() ) &&
			pNd->GetNodes().IsDocNodes() )
		{
            _SeqFldLstElem* pNew = new _SeqFldLstElem(
                    pNd->GetExpandTxt( 0, (*pF->GetTxtFld()->GetStart()) + 1 ),
                    ((SwSetExpField*)pF->GetField())->GetSeqNumber() );
			rList.InsertSort( pNew );
		}

	return rList.Count();
}


void SwSetExpFieldType::SetChapter( SwSetExpField& rFld, const SwNode& rNd )
{
	const SwTxtNode* pTxtNd = rNd.FindOutlineNodeOfLevel( nLevel );
	if( pTxtNd )
	{
        SwNumRule * pRule = pTxtNd->GetNumRule();

        if (pRule)
        {
            // --> OD 2005-11-02 #i51089 - TUNING#
            if ( pTxtNd->GetNum() )
            {
                const SwNodeNum & aNum = *(pTxtNd->GetNum());

                // nur die Nummer besorgen, ohne Pre-/Post-fixstrings
                String sNumber( pRule->MakeNumString(aNum, sal_False ));

                if( sNumber.Len() )
                    rFld.ChgExpStr( ( sNumber += sDelim ) += rFld.GetExpStr() );
            }
            else
            {
                ASSERT( false,
                        "<SwSetExpFieldType::SetChapter(..)> - text node with numbering rule, but without number. This is a serious defect -> inform OD" );
            }
        }
	}
}

sal_Bool SwSetExpFieldType::QueryValue( uno::Any& rAny, sal_uInt16 nWhichId ) const
{
	switch( nWhichId )
	{
	case FIELD_PROP_SUBTYPE:
		{
			sal_Int16 nRet = lcl_SubTypeToAPI(GetType());
			rAny <<= nRet;
		}
		break;
	case FIELD_PROP_PAR2:
		rAny <<= OUString(GetDelimiter());
		break;
	case FIELD_PROP_SHORT1:
		{
			sal_Int8 nRet = nLevel < MAXLEVEL? nLevel : -1;
			rAny <<= nRet;
		}
		break;
	default:
		DBG_ERROR("illegal property");
	}
	return sal_True;
}

sal_Bool SwSetExpFieldType::PutValue( const uno::Any& rAny, sal_uInt16 nWhichId )
{
	switch( nWhichId )
	{
	case FIELD_PROP_SUBTYPE:
		{
			sal_Int32 nSet = lcl_APIToSubType(rAny);
			if(nSet >=0)
				SetType(static_cast<sal_uInt16>(nSet));
		}
		break;
	case FIELD_PROP_PAR2:
		{
			String sTmp;
			if( ::GetString( rAny, sTmp ).Len() )
//				SetDelimiter( sTmp.GetChar( 0 ));
				SetDelimiter( sTmp );
			else
				SetDelimiter(String::CreateFromAscii( " "));
		}
		break;
	case FIELD_PROP_SHORT1:
		{
			sal_Int8 nLvl = 0;
			rAny >>= nLvl;
			if(nLvl < 0 || nLvl >= MAXLEVEL)
				SetOutlineLvl(UCHAR_MAX);
			else
				SetOutlineLvl(nLvl);
		}
		break;
	default:
		DBG_ERROR("illegal property");
	}
	return sal_True;
}

sal_Bool SwSeqFldList::InsertSort( _SeqFldLstElem* pNew )
{
	sal_Unicode* p = pNew->sDlgEntry.GetBufferAccess();
	while( *p )
	{
		if( *p < 0x20 )
			*p = 0x20;
		++p;
	}

	sal_uInt16 nPos;
	sal_Bool bRet = SeekEntry( *pNew, &nPos );
	if( !bRet )
		C40_INSERT( _SeqFldLstElem, pNew, nPos );
	return bRet;
}

sal_Bool SwSeqFldList::SeekEntry( const _SeqFldLstElem& rNew, sal_uInt16* pP )
{
	sal_uInt16 nO = Count(), nM, nU = 0;
	if( nO > 0 )
	{
		CollatorWrapper & rCaseColl = ::GetAppCaseCollator(),
						& rColl = ::GetAppCollator();
		const CharClass& rCC = GetAppCharClass();

		//#59900# Die Sortierung soll die Nummer korrekt einordnen
		//also "10" nach "9" und nicht "10" nach "1"
		const String& rTmp2 = rNew.sDlgEntry;
		xub_StrLen nFndPos2 = 0;
		String sNum2( rTmp2.GetToken( 0, ' ', nFndPos2 ));
		sal_Bool bIsNum2IsNumeric = rCC.isAsciiNumeric( sNum2 );
		sal_Int32 nNum2 = bIsNum2IsNumeric ? sNum2.ToInt32() : 0;

		nO--;
		while( nU <= nO )
		{
			nM = nU + ( nO - nU ) / 2;

			//#59900# Die Sortierung soll die Nummer korrekt einordnen
			//also "10" nach "9" und nicht "10" nach "1"
			const String& rTmp1 = (*((_SeqFldLstElem**)pData + nM))->sDlgEntry;
			xub_StrLen nFndPos1 = 0;
			String sNum1( rTmp1.GetToken( 0, ' ', nFndPos1 ));
			sal_Int32 nCmp;

			if( bIsNum2IsNumeric && rCC.isNumeric( sNum1 ) )
			{
				sal_Int32 nNum1 = sNum1.ToInt32();
				nCmp = nNum2 - nNum1;
				if( 0 == nCmp )
					nCmp = rCaseColl.compareString( rTmp2.Copy( nFndPos2 ),
										 			  rTmp1.Copy( nFndPos1 ));
			}
			else
				nCmp = rColl.compareString( rTmp2, rTmp1 );

			if( 0 == nCmp )
			{
				if( pP ) *pP = nM;
				return sal_True;
			}
			else if( 0 < nCmp )
				nU = nM + 1;
			else if( nM == 0 )
				break;
			else
				nO = nM - 1;
		}
	}
	if( pP ) *pP = nU;
	return sal_False;
}

/*--------------------------------------------------------------------
	Beschreibung: SwSetExpField by JP
 --------------------------------------------------------------------*/

SwSetExpField::SwSetExpField(SwSetExpFieldType* pTyp, const String& rFormel,
										sal_uLong nFmt)
	: SwFormulaField( pTyp, nFmt, 0.0 ), nSeqNo( USHRT_MAX ),
	nSubType(0)
{
	SetFormula(rFormel);
	// SubType ignorieren !!!
	bInput = sal_False;
	if( IsSequenceFld() )
	{
		SwValueField::SetValue(1.0);
		if( !rFormel.Len() )
		{
			String sFormel(rFormel);
			sFormel += pTyp->GetName();
			sFormel += '+';
			sFormel += '1';
			SetFormula(sFormel);
		}
	}
}

String SwSetExpField::Expand() const
{
	String aStr;
	if (nSubType & nsSwExtendedSubType::SUB_CMD)
	{	// Der CommandString ist gefragt
		aStr = GetTyp()->GetName();
		aStr.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " = " ));
		aStr += GetFormula();
	}
	else if(!(nSubType & nsSwExtendedSubType::SUB_INVISIBLE))
	{   // Der Wert ist sichtbar
		aStr = sExpand;
	}
	return aStr;
}

/*--------------------------------------------------------------------
	@return the field name
 --------------------------------------------------------------------*/

String SwSetExpField::GetFieldName() const
{
    SwFldTypesEnum const nStrType( (IsSequenceFld())
                            ? TYP_SEQFLD
                            : (bInput)
                                ? TYP_SETINPFLD
                                : TYP_SETFLD   );

    String aStr( SwFieldType::GetTypeStr( static_cast<sal_uInt16>(nStrType) ) );
    aStr += ' ';
    aStr += GetTyp()->GetName();

    // Sequence: without formula
    if (TYP_SEQFLD != nStrType)
    {
        aStr.AppendAscii( RTL_CONSTASCII_STRINGPARAM( " = " ) );
        aStr += GetFormula();
    }
    return aStr;
}

SwField* SwSetExpField::Copy() const
{
	SwSetExpField *pTmp = new SwSetExpField((SwSetExpFieldType*)GetTyp(),
											GetFormula(), GetFormat());
	pTmp->SwValueField::SetValue(GetValue());
	pTmp->sExpand   	= sExpand;
	pTmp->SetAutomaticLanguage(IsAutomaticLanguage());
	pTmp->SetLanguage(GetLanguage());
	pTmp->aPText		= aPText;
	pTmp->bInput		= bInput;
	pTmp->nSeqNo		= nSeqNo;
	pTmp->SetSubType(GetSubType());

	return pTmp;
}

void SwSetExpField::SetSubType(sal_uInt16 nSub)
{
	((SwSetExpFieldType*)GetTyp())->SetType(nSub & 0xff);
	nSubType = nSub & 0xff00;

	DBG_ASSERT( (nSub & 0xff) != 3, "SubType ist illegal!" );
}

sal_uInt16 SwSetExpField::GetSubType() const
{
	return ((SwSetExpFieldType*)GetTyp())->GetType() | nSubType;
}

void SwSetExpField::SetValue( const double& rAny )
{
	SwValueField::SetValue(rAny);

	if( IsSequenceFld() )
		sExpand = FormatNumber( (sal_uInt16)GetValue(), GetFormat() );
	else
		sExpand = ((SwValueFieldType*)GetTyp())->ExpandValue( rAny,
												GetFormat(), GetLanguage());
}

void SwGetExpField::SetValue( const double& rAny )
{
	SwValueField::SetValue(rAny);
	sExpand = ((SwValueFieldType*)GetTyp())->ExpandValue( rAny, GetFormat(),
															GetLanguage());
}
/* -------------------------------------------------
	Description: Find the index of the reference text
	following the current field
 --------------------------------------------------*/
xub_StrLen SwGetExpField::GetReferenceTextPos( const SwFmtFld& rFmt, SwDoc& rDoc)
{
	//
	const SwTxtFld* pTxtFld = rFmt.GetTxtFld();
	const SwTxtNode& rTxtNode = pTxtFld->GetTxtNode();
	//
	xub_StrLen nRet = *pTxtFld->GetStart() + 1;
	String sNodeText = rTxtNode.GetTxt();
	sNodeText.Erase(0, nRet);
	if(sNodeText.Len())
	{
		// now check if sNodeText starts with a non-alphanumeric character plus a blank
		sal_uInt16 nSrcpt = pBreakIt->GetRealScriptOfText( sNodeText, 0 );

		static sal_uInt16 nIds[] =
		{
			RES_CHRATR_LANGUAGE, RES_CHRATR_LANGUAGE,
			RES_CHRATR_FONT, RES_CHRATR_FONT,
			RES_CHRATR_CJK_LANGUAGE, RES_CHRATR_CJK_LANGUAGE,
			RES_CHRATR_CJK_FONT, RES_CHRATR_CJK_FONT,
			RES_CHRATR_CTL_LANGUAGE, RES_CHRATR_CTL_LANGUAGE,
			RES_CHRATR_CTL_FONT, RES_CHRATR_CTL_FONT,
			0, 0
		};
		SwAttrSet aSet(rDoc.GetAttrPool(), nIds);
		rTxtNode.GetAttr(aSet, nRet, nRet+1);

		if( RTL_TEXTENCODING_SYMBOL != ((SvxFontItem&)aSet.Get(
				GetWhichOfScript( RES_CHRATR_FONT, nSrcpt )) ).GetCharSet() )
		{
			LanguageType eLang = ((SvxLanguageItem&)aSet.Get(
				GetWhichOfScript( RES_CHRATR_LANGUAGE, nSrcpt )) ).GetLanguage();
			CharClass aCC( SvxCreateLocale( eLang ));
			sal_Unicode c0 = sNodeText.GetChar(0);
			sal_Bool bIsAlphaNum = aCC.isAlphaNumeric( sNodeText, 0 );
			if( !bIsAlphaNum ||
				(c0 == ' ' || c0 == '\t'))
			{
				nRet++;
				if( sNodeText.Len() > 1 &&
					(sNodeText.GetChar(1) == ' ' ||
					 sNodeText.GetChar(1) == '\t'))
					nRet++;
			}
		}
	}
	return nRet;
}


/*--------------------------------------------------------------------
	Beschreibung: Parameter setzen
 --------------------------------------------------------------------*/

const String& SwSetExpField::GetPar1() const
{
	return ((SwSetExpFieldType*)GetTyp())->GetName();
}

String SwSetExpField::GetPar2() const
{
	sal_uInt16 nType = ((SwSetExpFieldType*)GetTyp())->GetType();

	if (nType & nsSwGetSetExpType::GSE_STRING)
		return GetFormula();
	return GetExpandedFormula();
}

void SwSetExpField::SetPar2(const String& rStr)
{
	sal_uInt16 nType = ((SwSetExpFieldType*)GetTyp())->GetType();

	if( !(nType & nsSwGetSetExpType::GSE_SEQ) || rStr.Len() )
	{
		if (nType & nsSwGetSetExpType::GSE_STRING)
			SetFormula(rStr);
		else
			SetExpandedFormula(rStr);
	}
}


sal_Bool SwSetExpField::PutValue( const uno::Any& rAny, sal_uInt16 nWhichId )
{
	sal_Int32 nTmp32 = 0;
	sal_Int16 nTmp16 = 0;
	String sTmp;
	switch( nWhichId )
	{
	case FIELD_PROP_BOOL2:
		if(*(sal_Bool*)rAny.getValue())
			nSubType &= ~nsSwExtendedSubType::SUB_INVISIBLE;
		else
			nSubType |= nsSwExtendedSubType::SUB_INVISIBLE;
		break;
	case FIELD_PROP_FORMAT:
		rAny >>= nTmp32;
		SetFormat(nTmp32);
		break;
	case FIELD_PROP_USHORT2:
		{
			rAny >>= nTmp16;
			if(nTmp16 <= SVX_NUMBER_NONE )
				SetFormat(nTmp16);
			else {
				//exception(wrong_value)
				;
            }
		}
		break;
	case FIELD_PROP_USHORT1:
		rAny >>= nTmp16;
		nSeqNo = nTmp16;
		break;
	case FIELD_PROP_PAR1:
		SetPar1( SwStyleNameMapper::GetUIName(
							::GetString( rAny, sTmp ), nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL ) );
		break;
	case FIELD_PROP_PAR2:
		{
			OUString uTmp;
			rAny >>= uTmp;
			//I18N - if the formula contains only "TypeName+1"
			//and it's one of the initially created sequence fields
			//then the localized names has to be replaced by a programmatic name
			OUString sMyFormula = SwXFieldMaster::LocalizeFormula(*this, uTmp, sal_False);
			SetFormula( sMyFormula );
		}
		break;
	case FIELD_PROP_DOUBLE:
		{
			double fVal = 0.0;
			rAny >>= fVal;
			SetValue(fVal);
		}
		break;
	case FIELD_PROP_SUBTYPE:
		nTmp32 = lcl_APIToSubType(rAny);
		if(nTmp32 >= 0)
			SetSubType(static_cast<sal_uInt16>((GetSubType() & 0xff00) | nTmp32));
		break;
	case FIELD_PROP_PAR3:
		::GetString( rAny, aPText );
		break;
	case FIELD_PROP_BOOL3:
		if(*(sal_Bool*) rAny.getValue())
			nSubType |= nsSwExtendedSubType::SUB_CMD;
		else
			nSubType &= (~nsSwExtendedSubType::SUB_CMD);
		break;
	case FIELD_PROP_BOOL1:
		SetInputFlag(*(sal_Bool*) rAny.getValue());
		break;
	case FIELD_PROP_PAR4:
		ChgExpStr( ::GetString( rAny, sTmp ));
		break;
	default:
        return SwField::PutValue(rAny, nWhichId);
    }
	return sal_True;
}


sal_Bool SwSetExpField::QueryValue( uno::Any& rAny, sal_uInt16 nWhichId ) const
{
    switch( nWhichId )
	{
	case FIELD_PROP_BOOL2:
		{
			sal_Bool bVal = 0 == (nSubType & nsSwExtendedSubType::SUB_INVISIBLE);
			rAny.setValue(&bVal, ::getBooleanCppuType());
		}
		break;
	case FIELD_PROP_FORMAT:
		rAny <<= (sal_Int32)GetFormat();
		break;
	case FIELD_PROP_USHORT2:
		rAny <<= (sal_Int16)GetFormat();
		break;
	case FIELD_PROP_USHORT1:
		rAny <<= (sal_Int16)nSeqNo;
		break;
	case FIELD_PROP_PAR1:
		rAny <<= OUString ( SwStyleNameMapper::GetProgName(GetPar1(), nsSwGetPoolIdFromName::GET_POOLID_TXTCOLL ) );
		break;
	case FIELD_PROP_PAR2:
		{
			//I18N - if the formula contains only "TypeName+1"
			//and it's one of the initially created sequence fields
			//then the localized names has to be replaced by a programmatic name
			OUString sMyFormula = SwXFieldMaster::LocalizeFormula(*this, GetFormula(), sal_True);
			rAny <<= OUString( sMyFormula );
		}
		break;
	case FIELD_PROP_DOUBLE:
		rAny <<= (double)GetValue();
		break;
	case FIELD_PROP_SUBTYPE:
		{
			sal_Int16 nRet = 0;
				nRet = lcl_SubTypeToAPI(GetSubType() & 0xff);
			rAny <<= nRet;
		}
		break;
	case FIELD_PROP_PAR3:
		rAny <<= OUString( aPText );
		break;
	case FIELD_PROP_BOOL3:
		{
			sal_Bool bTmp = 0 != (nSubType & nsSwExtendedSubType::SUB_CMD);
			rAny.setValue(&bTmp, ::getBooleanCppuType());
		}
		break;
	case FIELD_PROP_BOOL1:
		{
			sal_Bool bTmp = GetInputFlag();
			rAny.setValue(&bTmp, ::getBooleanCppuType());
		}
		break;
	case FIELD_PROP_PAR4:
		rAny <<= rtl::OUString(GetExpStr());
		break;
	default:
        return SwField::QueryValue(rAny, nWhichId);
    }
	return sal_True;
}



/*--------------------------------------------------------------------
	Beschreibung: Eingabefeld Type
 ---------------------------------------------------------------------*/

SwInputFieldType::SwInputFieldType( SwDoc* pD )
    : SwFieldType( RES_INPUTFLD )
    , pDoc( pD )
{
}

SwFieldType* SwInputFieldType::Copy() const
{
    SwInputFieldType* pType = new SwInputFieldType( pDoc );
    return pType;
}

/*--------------------------------------------------------------------
	Beschreibung: Eingabefeld
 --------------------------------------------------------------------*/

SwInputField::SwInputField( SwInputFieldType* pFieldType,
                            const String& rContent,
                            const String& rPrompt,
                            sal_uInt16 nSub,
                            sal_uLong nFmt,
                            bool bIsFormField )
    : SwField( pFieldType, nFmt, LANGUAGE_SYSTEM, false )
    , aContent(rContent)
    , aPText(rPrompt)
    , nSubType(nSub)
    , mbIsFormField( bIsFormField )
    , mpFmtFld( NULL )
{
}

SwInputField::~SwInputField()
{
}


void SwInputField::SetFmtFld( SwFmtFld& rFmtFld )
{
    mpFmtFld = &rFmtFld;
}

SwFmtFld* SwInputField::GetFmtFld()
{
    return mpFmtFld;
}


const String& SwInputField::getContent() const
{
    return aContent;
}


void SwInputField::LockNotifyContentChange()
{
    if ( GetFmtFld() != NULL )
    {
        SwTxtInputFld* pTxtInputFld = dynamic_cast< SwTxtInputFld* >(GetFmtFld()->GetTxtFld());
        if ( pTxtInputFld != NULL )
        {
            pTxtInputFld->LockNotifyContentChange();
        }
    }
}


void SwInputField::UnlockNotifyContentChange()
{
    if ( GetFmtFld() != NULL )
    {
        SwTxtInputFld* pTxtInputFld = dynamic_cast< SwTxtInputFld* >(GetFmtFld()->GetTxtFld());
        if ( pTxtInputFld != NULL )
        {
            pTxtInputFld->UnlockNotifyContentChange();
        }
    }
}

void SwInputField::applyFieldContent( const String& rNewFieldContent )
{
    if ( (nSubType & 0x00ff) == INP_TXT )
    {
        aContent = rNewFieldContent;
    }
    else if( (nSubType & 0x00ff) == INP_USR )
    {
        SwUserFieldType* pUserTyp = static_cast<SwUserFieldType*>(
            static_cast<SwInputFieldType*>(GetTyp())->GetDoc()->GetFldType( RES_USERFLD, getContent(), false ) );
        if( pUserTyp )
        {
            pUserTyp->SetContent( rNewFieldContent );

            // trigger update of the corresponding User Fields and other related Input Fields
            {
                LockNotifyContentChange();
                pUserTyp->UpdateFlds();
                UnlockNotifyContentChange();
            }
        }
    }
}

String SwInputField::GetFieldName() const
{
    String aStr(SwField::GetFieldName());
    if ((nSubType & 0x00ff) == INP_USR)
    {
        aStr += GetTyp()->GetName();
        aStr += ' ';
        aStr += getContent();
    }
    return aStr;
}

SwField* SwInputField::Copy() const
{
    SwInputField* pFld =
        new SwInputField(
            static_cast<SwInputFieldType*>(GetTyp()),
            getContent(),
            aPText,
            GetSubType(),
            GetFormat(),
            mbIsFormField );

    pFld->SetHelp( aHelp );
    pFld->SetToolTip( aToolTip );

    pFld->SetAutomaticLanguage(IsAutomaticLanguage());
    return pFld;
}

String SwInputField::Expand() const
{
    String sRet;
    if ( (nSubType & 0x00ff) == INP_TXT )
    {
        sRet = getContent();
    }
    else if( (nSubType & 0x00ff) == INP_USR )
    {
        SwUserFieldType* pUserTyp = static_cast<SwUserFieldType*>(
            static_cast<SwInputFieldType*>(GetTyp())->GetDoc()->GetFldType( RES_USERFLD, getContent(), false ) );
        if( pUserTyp )
            sRet = pUserTyp->GetContent();
    }
    return sRet;
}


bool SwInputField::isFormField() const
{
    return mbIsFormField
           || aHelp.Len() > 0
           || aToolTip.Len() > 0;
}


sal_Bool SwInputField::QueryValue( uno::Any& rAny, sal_uInt16 nWhichId ) const
{
    switch( nWhichId )
    {
    case FIELD_PROP_PAR1:
        rAny <<= OUString( getContent() );
        break;
    case FIELD_PROP_PAR2:
        rAny <<= OUString( aPText );
        break;
    case FIELD_PROP_PAR3:
        rAny <<= OUString( aHelp );
        break;
    case FIELD_PROP_PAR4:
        rAny <<= OUString( aToolTip );
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}

sal_Bool SwInputField::PutValue( const uno::Any& rAny, sal_uInt16 nWhichId )
{
    switch( nWhichId )
    {
    case FIELD_PROP_PAR1:
        {
            ::GetString( rAny, aContent );
        }
        break;
    case FIELD_PROP_PAR2:
        ::GetString( rAny, aPText );
        break;
    case FIELD_PROP_PAR3:
        ::GetString( rAny, aHelp );
        break;
    case FIELD_PROP_PAR4:
        ::GetString( rAny, aToolTip );
        break;
    default:
        DBG_ERROR("illegal property");
    }
    return sal_True;
}


void SwInputField::SetPar1(const String& rStr)
{
	aContent = rStr;
}

const String& SwInputField::GetPar1() const
{
	return getContent();
}


void SwInputField::SetPar2(const String& rStr)
{
	aPText = rStr;
}

String SwInputField::GetPar2() const
{
	return aPText;
}

void SwInputField::SetHelp(const String & rStr)
{
	aHelp = rStr;
}

String SwInputField::GetHelp() const
{
	return aHelp;
}

void SwInputField::SetToolTip(const String & rStr)
{
	aToolTip = rStr;
}

String SwInputField::GetToolTip() const
{
	return aToolTip;
}

sal_uInt16 SwInputField::GetSubType() const
{
	return nSubType;
}

void SwInputField::SetSubType(sal_uInt16 nSub)
{
	nSubType = nSub;
}

/* vim: set noet sw=4 ts=4: */
