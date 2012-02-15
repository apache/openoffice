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

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _DOC_HXX
#include <doc.hxx>
#endif

#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _RDSWG_HXX
#include <rdswg.hxx>
#endif
#ifndef _DOCTXM_HXX
#include <doctxm.hxx>
#endif
#ifndef _SWSWERROR_H
#include <swerror.h>
#endif
#ifndef _POOLFMT_HXX
#include <poolfmt.hxx>
#endif
#ifndef _SWSTYLENAMEMAPPER_HXX
#include <SwStyleNameMapper.hxx>
#endif
namespace binfilter {


//////////////////////////////////////////////////////////////////////////////

// Achtung: Jede Routine nimmt an, dass der Record-Header bereits eingelesen
// ist. Sie arbeitet den Record so weit ab wie moeglich und schliesst den
// Vorgang mit end() ab.

//////////////////////////////////////////////////////////////////////////////

SwTOXBase* SwSwgReader::InTOXBase()
{
    // Den TOXtype einlesen
    BYTE eType;
    r >> eType;
    String aName( GetText() );
    if( !r ) return NULL;
    TOXTypes eTOXType = (TOXTypes) eType;
    // nach dem TOXType suchen
    USHORT nTOXType = pDoc->GetTOXTypeCount( eTOXType );
    const SwTOXType* pTOXType = NULL;
    for( USHORT n = 0; n < nTOXType; n++ )
    {
        const SwTOXType* p = pDoc->GetTOXType( eTOXType, n );
        if( p->GetTypeName() == aName )
        {
            pTOXType = p; break;
        }
    }
    // Falls nicht vorhanden, am Dokument einfuegen
    if( !pTOXType )
    {
        pDoc->InsertTOXType( SwTOXType ( eTOXType, aName ) );
        pTOXType = pDoc->GetTOXType( eTOXType, nTOXType );
    }
    // jetzt muss er da sein!
    if( !pTOXType )
    {
        Error(); return NULL;
    }
    // Die Variablen einlesen
    USHORT nCreaType;
    r >> nCreaType;
    String aTitle( GetText() );
    if( !r ) return NULL;
    // Die SwForm einlesen
    BYTE nPat, nTmpl;
    r >> nPat >> nTmpl;
    SwForm aForm(eType);
    if( nPat )
    {
        for( USHORT i = 0; i < nPat; i++ )
        {
            String aText( GetText() );
            if ( !r ) return NULL;
            if( aText.Len() )
                aText = SwForm::ConvertPatternFrom51( aText, eTOXType );
            if( i < aForm.GetFormMax() )
                aForm.SetPattern( i, aText );
        }
    }
    if( nTmpl )
    {
        for( USHORT i = 0; i < nTmpl; i++ )
        {
            String aText( GetText() );
            if ( !r ) return NULL;
            if( i < aForm.GetFormMax() )
            {
                if( !aText.Len() )
                {
                    // Bug 37672: falls keiner gefunden wird, nehme
                    //          die defaults
                    USHORT nPoolIdOffset = 0;
                    switch( eType )
                    {
                        case TOX_INDEX:
                            nPoolIdOffset = RES_POOLCOLL_TOX_IDXH -
                                            RES_POOLCOLL_REGISTER_BEGIN;
                            break;
                        case TOX_CONTENT:
                            if( 6 > i )
                                nPoolIdOffset = RES_POOLCOLL_TOX_CNTNTH -
                                                RES_POOLCOLL_REGISTER_BEGIN;
                            else
                                nPoolIdOffset = RES_POOLCOLL_TOX_CNTNT6 - 6 -
                                                RES_POOLCOLL_REGISTER_BEGIN;
                            break;
                        case TOX_USER:
                            nPoolIdOffset = RES_POOLCOLL_TOX_USERH -
                                            RES_POOLCOLL_REGISTER_BEGIN;
                            break;
                    }

                    aText = *SwStyleNameMapper::GetRegisterUINameArray()[ nPoolIdOffset + i ];
                }
                aForm.SetTemplate( i, aText );
            }
        }
    }

    //fill tab stop positions into the patterns
    aForm.AdjustTabStops(*pDoc);

    SwTOXBase* pBase = new SwTOXBase( pTOXType, aForm, nCreaType, aTitle );
    pBase->SetTOXName( pDoc->GetUniqueTOXBaseName( *pTOXType ) );
    // Zuletzt noch die Spezial-Variablen
    switch( eType )
    {
        case TOX_INDEX: {
            USHORT nOptions;
            r >> nOptions;
            pBase->SetOptions( nOptions );
            } break;
        case TOX_CONTENT: {
            USHORT nLevel;
            r >> nLevel;
            pBase->SetLevel( nLevel );
            } break;
        case TOX_USER:
            aName = GetText();
            pBase->SetTemplateName( aName );
            break;
    }
    if( !r )
    {
        delete pBase; return NULL;
    }
    return pBase;
}

// Verzeichnis-Block

void SwSwgReader::InTOX( SwNodeIndex& rPos, SwTxtNode* pNd )
{
    if( r.next() != SWG_TOXDESC )
        Error();
    else
    {
        long nSkip = 0;
        if( aHdr.nVersion >= SWG_VER_TOXDATA ) {
            if( r.next() != SWG_DATA ) {
                Error(); return;
            }
            nSkip = r.getskip();
        }
        SwTOXBase* pBase = InTOXBase();
        if( !pBase ) return;
        BOOL bInLastNode = FALSE;
        USHORT nOff1 = 0, nOff2 = 0;
        if( nSkip ) {
            // Neu ab Version 2.06: Anfangs- und Endbereich
            // Des TOX-Bereichs einlesen
            r >> bInLastNode >> nOff1 >> nOff2;
            r.skip( nSkip );
        }

        r.next();
        ULONG nStart = rPos.GetIndex();
        // Im Gegensatz zu einer Section wird dieser Text nicht als eigene
        // Section, sondern als ganz normaler Text verstanden.
        USHORT nNodes;
        r >> nNodes;
        r.next();
        for( USHORT i = 1; i <= nNodes && r.good(); i++)
        {
            switch( r.cur() )
            {
                case SWG_TEXTNODE:
                    FillTxtNode( pNd, rPos, 0 );
                    break;
                case SWG_GRFNODE:
                    InGrfNode( rPos );
                    break;
                case SWG_OLENODE:
                    InOleNode( rPos );
                    break;
                case SWG_TABLE:
                    InTable( rPos );
                    break;
                default:
                    // unbekannte Nodes
                    r.skipnext();
            }
            pNd = NULL;
        }
        // Den TOX aufspannen, falls da was ist
        // der PaM steht jetzt auf dem letzten eingelesenen Node
        BOOL bRes = TRUE;
        ULONG nEnd = rPos.GetIndex() - 1;
        if( bInLastNode ) nStart--;
        if( nStart <= nEnd )
            bRes = 0 != pDoc->InsertTableOf( nStart, nEnd, *pBase );
        delete pBase;
        if( !bRes && !bNew )
            Error( ERR_SWG_INTERNAL_ERROR );
    }
}


}
