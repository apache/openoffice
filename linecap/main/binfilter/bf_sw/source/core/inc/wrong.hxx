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



#ifndef	_WRONG_HXX
#define	_WRONG_HXX

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_XUB_STRLEN
#define _SVSTDARR_ULONGS
#include <bf_svtools/svstdarr.hxx>
#endif
/*N*/ #include <tools/debug.hxx> //for stripping
namespace binfilter {
#define WRPOS( nIdx ) ((xub_StrLen)( GetObject( nIdx ) ))
#define WRLEN( nIdx ) ((xub_StrLen)( aLen.GetObject( nIdx ) ))

class SwWrongList : public SvXub_StrLens
{
	SvXub_StrLens aLen;
	xub_StrLen nBeginInvalid;	// Start des ungueltigen Bereichs
	xub_StrLen nEndInvalid;		// Ende des ungueltigen Bereichs
	void ShiftLeft( xub_StrLen &rPos, xub_StrLen nStart, xub_StrLen nEnd )
	{ if( rPos > nStart ) rPos = rPos > nEnd ? rPos - nEnd + nStart : nStart; }
	void ShiftRight( xub_StrLen &rPos, xub_StrLen nStart, xub_StrLen nEnd )
	{ if( rPos >= nStart ) rPos += nStart - nEnd; }
	void _Invalidate( xub_StrLen nBegin, xub_StrLen nEnd );
public:
	inline SwWrongList() :
		SvXub_StrLens(5,5), aLen(5,5), nBeginInvalid( STRING_LEN ){}
	inline xub_StrLen GetBeginInv() const { return nBeginInvalid; }
	inline xub_StrLen GetEndInv() const { return nEndInvalid; }
	inline BOOL InsideInvalid( xub_StrLen nChk ) const
		{ return nChk >= nBeginInvalid && nChk <= nEndInvalid; }
	inline void SetInvalid( xub_StrLen nBegin, xub_StrLen nEnd )
		{ nBeginInvalid = nBegin; nEndInvalid = nEnd; }
	inline void Validate(){ nBeginInvalid = STRING_LEN; }
	inline void Invalidate( xub_StrLen nBegin, xub_StrLen nEnd )
		{ if( STRING_LEN == GetBeginInv() ) SetInvalid( nBegin, nEnd );
		  else _Invalidate( nBegin, nEnd ); }
	BOOL InvalidateWrong();
	void Move( xub_StrLen nPos, long nDiff ){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void Move( xub_StrLen nPos, long nDiff );

    // Divide the list into two part, the wrong words until nSplitPos will be
    // removed and transferred to a new SwWrongList.
    // Join the next SwWrongList, nInsertPos is my own text length, where
    // the other wrong list has to be inserted.

	inline xub_StrLen Len( USHORT nIdx ) const { return WRLEN( nIdx );	}
	inline xub_StrLen Pos( USHORT nIdx ) const { return WRPOS( nIdx );	}
	inline void Insert( xub_StrLen nNewPos, xub_StrLen nNewLen, USHORT nWhere )
	{SvXub_StrLens::Insert( nNewPos, nWhere ); aLen.Insert( nNewLen, nWhere );}

// Wer braucht dies?
	void Insert( ULONG nNew )
	;//	{ SvULongs::Insert( nNew, GetPos( (USHORT)( nNew & WRMASK ) ) ); }
};


} //namespace binfilter
#endif

