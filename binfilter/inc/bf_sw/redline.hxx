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


#ifndef _REDLINE_HXX
#define _REDLINE_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _DATETIME_HXX //autogen
#include <tools/datetime.hxx>
#endif
#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif

#define _SVSTDARR_USHORTS
#include <bf_svtools/svstdarr.hxx>

#ifndef _PAM_HXX
#include <pam.hxx>
#endif
#ifndef _REDLENUM_HXX
#include <redlenum.hxx>
#endif
namespace binfilter {
class SfxItemSet; 



class SwRedlineExtraData
{
	SwRedlineExtraData( const SwRedlineExtraData& );
	SwRedlineExtraData& operator=( const SwRedlineExtraData& );

protected:
	SwRedlineExtraData() {}

public:
	virtual ~SwRedlineExtraData();
 	virtual SwRedlineExtraData* CreateNew() const = 0;

	virtual int operator == ( const SwRedlineExtraData& ) const;
};





class SwRedlineData
{
	friend class SwRedline;
	SwRedlineData* pNext;		// Verweis auf weitere Daten
	SwRedlineExtraData* pExtraData;

	String sComment;
	DateTime aStamp;
	SwRedlineType eType;
	USHORT nAuthor, nSeqNo;

public:
	SwRedlineData( const SwRedlineData& rCpy, BOOL bCpyNext = TRUE );

	// fuer sw3io: pNext/pExtraData gehen in eigenen Besitz ueber!
	SwRedlineData( SwRedlineType eT, USHORT nAut, const DateTime& rDT,
				   const String& rCmnt, SwRedlineData* pNxt,
					SwRedlineExtraData* pExtraData = 0 );

	~SwRedlineData();

	int operator==( const SwRedlineData& rCmp ) const
		{
			return nAuthor == rCmp.nAuthor &&
					eType == rCmp.eType &&
					sComment == rCmp.sComment &&
					(( !pNext && !rCmp.pNext ) ||
						( pNext && rCmp.pNext && *pNext == *rCmp.pNext )) &&
					(( !pExtraData && !rCmp.pExtraData ) ||
						( pExtraData && rCmp.pExtraData &&
							*pExtraData == *rCmp.pExtraData ));
		}
	int operator!=( const SwRedlineData& rCmp ) const
		{	return !operator==( rCmp ); }

	SwRedlineType GetType() const
		{ return SwRedlineType( eType & REDLINE_NO_FLAG_MASK); }
	SwRedlineType GetRealType() const 		{ return eType; }
	USHORT GetAuthor() const 				{ return nAuthor; }
	const String& GetComment() const 		{ return sComment; }
	const DateTime& GetTimeStamp() const 	{ return aStamp; }
	inline const SwRedlineData* Next() const{ return pNext;	}

	void SetTimeStamp( const DateTime& rDT)
		{ aStamp = rDT; aStamp.SetSec( 0 ); aStamp.Set100Sec( 0 ); }
	void SetComment( const String& rS ) 	{ sComment = rS; }
	void SetAutoFmtFlag()
		{ eType = SwRedlineType( eType | REDLINE_FORM_AUTOFMT ); }
	int CanCombine( const SwRedlineData& rCmp ) const
		{
			return nAuthor == rCmp.nAuthor &&
					eType == rCmp.eType &&
					sComment == rCmp.sComment &&
					GetTimeStamp() == rCmp.GetTimeStamp() &&
					(( !pNext && !rCmp.pNext ) ||
						( pNext && rCmp.pNext &&
						pNext->CanCombine( *rCmp.pNext ))) &&
					(( !pExtraData && !rCmp.pExtraData ) ||
						( pExtraData && rCmp.pExtraData &&
							*pExtraData == *rCmp.pExtraData ));
		}

	// ExtraData wird kopiert, der Pointer geht also NICHT in den Besitz
	// des RedlineObjectes!
	const SwRedlineExtraData* GetExtraData() const { return pExtraData; }

	// fuers UI-seitige zusammenfassen von Redline-Actionen. Wird z.Z. nur
	// fuers Autoformat mit Redline benoetigt. Der Wert != 0 bedeutet dabei,
	// das es noch weitere geben kann!
	USHORT GetSeqNo() const 					{ return nSeqNo; }
	void SetSeqNo( USHORT nNo ) 				{ nSeqNo = nNo; }
};


class SwRedline : public SwPaM
{
	SwRedlineData* pRedlineData;
	SwNodeIndex* pCntntSect;
	BOOL bDelLastPara : 1;
	BOOL bIsLastParaDelete : 1;
	BOOL bIsVisible : 1;


public:
	SwRedline( const SwRedlineData& rData, const SwPosition& rPos );
	// fuer sw3io: pData geht in eigenen Besitz ueber!
	SwRedline(SwRedlineData* pData, const SwPosition& rPos, BOOL bVsbl,
			   BOOL bDelLP, BOOL bIsPD) :
		SwPaM( rPos ), pRedlineData( pData ), pCntntSect( 0 ), 
		bDelLastPara( bDelLP ), bIsLastParaDelete( bIsPD ), bIsVisible( bVsbl )
	{}
	SwRedline( const SwRedline& );
	virtual ~SwRedline();

	SwNodeIndex* GetContentIdx() const { return pCntntSect; }
	// fuers Undo
 	void SetContentIdx( const SwNodeIndex* );

	BOOL IsVisible() const { return bIsVisible; }
	BOOL IsDelLastPara() const { return bDelLastPara; }
	BOOL IsLastParaDelete() const { return bIsLastParaDelete; }

	// das BOOL besagt, ob nach dem setzen der Pos kein Bereich mehr
	// aufgespannt ist. -> TRUE, ansonten Bereich und FALSE
	void SetStart( const SwPosition& rPos, SwPosition* pSttPtr = 0 )
	{
		if( !pSttPtr ) pSttPtr = Start();
		*pSttPtr = rPos;
	}
	void SetEnd( const SwPosition& rPos, SwPosition* pEndPtr = 0 )
	{
		if( !pEndPtr ) pEndPtr = End();
		*pEndPtr = rPos;
	}
	// liegt eine gueltige Selektion vor?
/*N*/ 	BOOL HasValidRange() const; //SW50.SDW

	USHORT GetAuthor() const { return pRedlineData->GetAuthor(); }
	const DateTime& GetTimeStamp() const { return pRedlineData->GetTimeStamp(); }
	SwRedlineType GetType() const { return pRedlineData->GetType(); }
	SwRedlineType GetRealType() const { return pRedlineData->GetRealType(); }
	const SwRedlineData& GetRedlineData() const { return *pRedlineData; }
	int operator==( const SwRedlineData& rCmp ) const
		{ return *pRedlineData == rCmp; }
	int operator!=( const SwRedlineData& rCmp ) const
		{ return *pRedlineData != rCmp; }
	void SetAutoFmtFlag()				{ pRedlineData->SetAutoFmtFlag(); }

    USHORT GetStackCount() const;
	const String& GetAuthorString( USHORT nPos = 0 ) const;
    const DateTime& GetTimeStamp( USHORT nPos ) const;
    SwRedlineType GetRealType( USHORT nPos ) const;
    SwRedlineType GetType( USHORT nPos ) const
        { return SwRedlineType( GetRealType( nPos ) & REDLINE_NO_FLAG_MASK); }
    const String& GetComment( USHORT nPos ) const;

	const String& GetComment() const 	{ return pRedlineData->GetComment(); }
	void SetComment( const String& rS ) { pRedlineData->SetComment( rS ); }

	// ExtraData wird kopiert, der Pointer geht also NICHT in den Besitz
	// des RedlineObjectes!
	const SwRedlineExtraData* GetExtraData() const
		{ return pRedlineData->GetExtraData(); }

	// fuers UI-seitige zusammenfassen von Redline-Actionen. Wird z.Z. nur
	// fuers Autoformat mit Redline benoetigt. Der Wert != 0 bedeutet dabei,
	// das es noch weitere geben kann!
	USHORT GetSeqNo() const 			{ return pRedlineData->GetSeqNo(); }
	void SetSeqNo( USHORT nNo ) 		{ pRedlineData->SetSeqNo( nNo ); }

	// Beim Hide/ShowOriginal wird 2 mal ueber die Liste gelaufen, damit
	//	die Del-Redlines per Copy und Delete versteckt werden. Beim Move
	//	wird sonst die Attributierung falsch behandelt.
	// Alle anderen Aufrufer muessen immer 0 angeben.
	void Show( USHORT nLoop = 0 );
	void Hide( USHORT nLoop = 0 );

    // calculates the intersection with text node number nNdIdx


	BOOL IsOwnRedline( const SwRedline& rRedl ) const
		{ return GetAuthor() == rRedl.GetAuthor(); }
	BOOL CanCombine( const SwRedline& rRedl ) const;


 	int operator==( const SwRedline& ) const;
	int operator<( const SwRedline& ) const;
};

} //namespace binfilter
#endif
