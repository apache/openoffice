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

#ifndef _CNTFRM_HXX
#include <cntfrm.hxx>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _NDTXT_HXX
#include <ndtxt.hxx>
#endif
#ifndef _FMTFLD_HXX //autogen
#include <fmtfld.hxx>
#endif
#ifndef _TXTFLD_HXX //autogen
#include <txtfld.hxx>
#endif
#ifndef _EXPFLD_HXX
#include <expfld.hxx>
#endif
#ifndef _DOCFLD_HXX
#include <docfld.hxx> 	// fuer _SetGetExpFld
#endif
#ifndef _UNOFLDMID_H
#include <unofldmid.h>
#endif
namespace binfilter {

extern String& GetString( const ::com::sun::star::uno::Any& rAny, String& rStr ); //STRIP008

using namespace ::com::sun::star;
using namespace ::rtl;


/*N*/ SwTblFieldType::SwTblFieldType(SwDoc* pDocPtr)
/*N*/ 	: SwValueFieldType( pDocPtr, RES_TABLEFLD )
/*N*/ {}


/*N*/ SwFieldType* SwTblFieldType::Copy() const
/*N*/ {
        return new SwTblFieldType(GetDoc());
/*N*/ }



/*N*/ void SwTblField::CalcField( SwTblCalcPara& rCalcPara )
/*N*/ {
/*N*/ 	if( rCalcPara.rCalc.IsCalcError() )		// ist schon Fehler gesetzt ?
/*?*/ 		return;
/*N*/ 
/*N*/ 	// erzeuge aus den BoxNamen die Pointer
/*N*/ 	BoxNmToPtr( rCalcPara.pTbl );
/*N*/ 	String sFml( MakeFormel( rCalcPara ));
/*N*/ 	SetValue( rCalcPara.rCalc.Calculate( sFml ).GetDouble() );
/*N*/ 	ChgValid( !rCalcPara.IsStackOverFlow() );		// ist der Wert wieder gueltig?
/*N*/ }



/*N*/ SwTblField::SwTblField( SwTblFieldType* pType, const String& rFormel,
/*N*/ 						USHORT nType, ULONG nFmt )
/*N*/ 	: SwValueField( pType, nFmt ), SwTableFormula( rFormel ),
/*N*/ 	nSubType(nType), sExpand( '0' )
/*N*/ {
/*N*/ }


/*N*/ SwField* SwTblField::Copy() const
/*N*/ {
/*N*/ 	SwTblField* pTmp = new SwTblField( (SwTblFieldType*)GetTyp(),
/*N*/ 										SwTableFormula::GetFormula(), nSubType, GetFormat() );
/*N*/ 	pTmp->sExpand 	  = sExpand;
/*N*/ 	pTmp->SwValueField::SetValue(GetValue());
/*N*/ 	pTmp->SwTableFormula::operator=( *this );
/*N*/     pTmp->SetAutomaticLanguage(IsAutomaticLanguage());
/*N*/ 	return pTmp;
/*N*/ }


/*N*/ String SwTblField::GetCntnt(BOOL bName) const
/*N*/ {
/*N*/ 	if( bName )
/*N*/ 	{
/*?*/ 		String aStr(GetTyp()->GetName());
/*?*/ 		aStr += ' ';
/*?*/ 
/*?*/ 		USHORT nOldSubType = nSubType;
/*?*/ 		SwTblField* pThis = (SwTblField*)this;
/*?*/ 		pThis->nSubType |= SUB_CMD;
/*?*/ 		aStr += Expand();
/*?*/ 		pThis->nSubType = nOldSubType;
/*?*/ 
/*?*/ 		return aStr;
/*N*/ 	}
/*N*/ 	return Expand();
/*N*/ }

// suche den TextNode, in dem das Feld steht
/*N*/ const SwNode* SwTblField::GetNodeOfFormula() const
/*N*/ {
/*N*/ 	if( !GetTyp()->GetDepends() )
/*N*/ 		return 0;
/*N*/ 
/*N*/ 	SwClientIter aIter( *GetTyp() );
/*N*/ 	SwClient * pLast = aIter.GoStart();
/*N*/ 	if( pLast ) 	// konnte zum Anfang gesprungen werden ??
/*N*/ 		do {
/*N*/ 			const SwFmtFld* pFmtFld = (SwFmtFld*)pLast;
/*N*/ 			if( this == pFmtFld->GetFld() )
/*N*/ 				return (SwTxtNode*)&pFmtFld->GetTxtFld()->GetTxtNode();
/*N*/ 
/*N*/ 		} while( 0 != ( pLast = aIter++ ));
/*N*/ 	return 0;
/*N*/ }


/*N*/ String SwTblField::Expand() const
/*N*/ {
/*N*/ 	String aStr;
/*N*/ 	if (nSubType & SUB_CMD)
/*N*/ 	{
/*N*/ 		if( EXTRNL_NAME != GetNameType() )
/*N*/ 		{
/*?*/ 			const SwNode* pNd = GetNodeOfFormula();
/*?*/ 			const SwTableNode* pTblNd = pNd ? pNd->FindTableNode() : 0;
/*?*/ 			if( pTblNd )
/*?*/ 				((SwTblField*)this)->PtrToBoxNm( &pTblNd->GetTable() );
/*N*/ 		}
/*N*/ 		if( EXTRNL_NAME == GetNameType() )
/*N*/ 			aStr = SwTableFormula::GetFormula();
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		aStr = sExpand;
/*N*/ 		if(nSubType & GSE_STRING)
/*N*/ 		{
/*?*/ 			// es ist ein String
/*?*/ 			aStr = sExpand;
/*?*/ 			aStr.Erase( 0,1 );
/*?*/ 			aStr.Erase( aStr.Len()-1, 1 );
/*N*/ 		}
/*N*/ 	}
/*N*/ 	return aStr;
/*N*/ }

/*N*/ USHORT SwTblField::GetSubType() const
/*N*/ {
/*N*/ 	return nSubType;
/*N*/ }

void SwTblField::SetSubType(USHORT nType)
{
    nSubType = nType;
}


/*N*/ void SwTblField::SetValue( const double& rVal )
/*N*/ {
/*N*/ 	SwValueField::SetValue(rVal);
/*N*/ 	sExpand = ((SwValueFieldType*)GetTyp())->ExpandValue(rVal, GetFormat(), GetLanguage());
/*N*/ }

/*--------------------------------------------------------------------
	Beschreibung: Parameter setzen
 --------------------------------------------------------------------*/


/*N*/ String SwTblField::GetPar2() const
/*N*/ {
/*N*/ 	return SwTableFormula::GetFormula();
/*N*/ }


void SwTblField::SetPar2(const String& rStr)
{
    SetFormula( rStr );
}


/*-----------------04.03.98 10:33-------------------

--------------------------------------------------*/
/*N*/ BOOL SwTblField::QueryValue( uno::Any& rAny, BYTE nMId ) const
/*N*/ {
/*N*/     nMId &= ~CONVERT_TWIPS;
/*N*/ 	BOOL bRet = TRUE;
/*N*/ 	switch ( nMId )
/*N*/ 	{
/*N*/ 	case FIELD_PROP_PAR2:
/*N*/ 		{
/*N*/ 			USHORT nOldSubType = nSubType;
/*N*/ 			SwTblField* pThis = (SwTblField*)this;
/*N*/ 			pThis->nSubType |= SUB_CMD;
/*N*/ 			rAny <<= ::rtl::OUString( Expand() );
/*N*/ 			pThis->nSubType = nOldSubType;
/*N*/ 		}
/*N*/ 		break;
/*N*/ 	case FIELD_PROP_BOOL1:
/*N*/ 		{
/*N*/         	BOOL bFormula = 0 != (SUB_CMD & nSubType);
/*N*/         	rAny.setValue(&bFormula, ::getBooleanCppuType());
/*N*/ 		}
/*N*/ 		break;
/*N*/ 	case FIELD_PROP_PAR1:
/*?*/ 		rAny <<= ::rtl::OUString(GetExpStr());
/*?*/ 		break;
/*N*/ 	case FIELD_PROP_FORMAT:
/*N*/         rAny <<= (sal_Int32)GetFormat();
/*N*/ 		break;
/*N*/ 	default:
/*?*/         bRet = sal_False;
/*N*/ 	}
/*N*/     return bRet;
/*N*/ }
/*-----------------04.03.98 10:33-------------------

--------------------------------------------------*/
BOOL SwTblField::PutValue( const uno::Any& rAny, BYTE nMId )
{
    nMId &= ~CONVERT_TWIPS;
    BOOL bRet = TRUE;
    String sTmp;
    switch ( nMId )
    {
    case FIELD_PROP_PAR2:
        SetFormula( ::binfilter::GetString( rAny, sTmp ));
        break;
    case FIELD_PROP_BOOL1:
        if(*(sal_Bool*)rAny.getValue())
            nSubType = GSE_FORMULA|SUB_CMD;
        else
            nSubType = GSE_FORMULA;
        break;
    case FIELD_PROP_PAR1:
        ChgExpStr( ::binfilter::GetString( rAny, sTmp ));
        break;
    case FIELD_PROP_FORMAT:
        {
            sal_Int32 nTmp;
            rAny >>= nTmp;
            SetFormat(nTmp);
        }
        break;
    default:
        bRet = sal_False;
    }
    return bRet;
}




}
