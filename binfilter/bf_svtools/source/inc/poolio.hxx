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


#include <bf_svtools/bf_solar.h>
#include <bf_svtools/brdcst.hxx>


#ifndef DELETEZ
#define DELETEZ(pPtr) { delete pPtr; pPtr = 0; }
#endif

namespace binfilter
{

struct SfxPoolVersion_Impl
{
	USHORT			_nVer;
	USHORT			_nStart, _nEnd;
	USHORT* 		_pMap;

					SfxPoolVersion_Impl( USHORT nVer, USHORT nStart, USHORT nEnd,
										 USHORT *pMap )
					:	_nVer( nVer ),
						_nStart( nStart ),
						_nEnd( nEnd ),
						_pMap( pMap )
					{}
					SfxPoolVersion_Impl( const SfxPoolVersion_Impl &rOrig )
					:	_nVer( rOrig._nVer ),
						_nStart( rOrig._nStart ),
						_nEnd( rOrig._nEnd ),
						_pMap( rOrig._pMap )
					{}
};

SV_DECL_PTRARR( SfxPoolItemArrayBase_Impl, SfxPoolItem*, 0, 5 )
SV_DECL_PTRARR_DEL( SfxPoolVersionArr_Impl, SfxPoolVersion_Impl*, 0, 2 )

struct SfxPoolItemArray_Impl: public SfxPoolItemArrayBase_Impl
{
	USHORT	nFirstFree;

	SfxPoolItemArray_Impl (USHORT nInitSize = 0)
		: SfxPoolItemArrayBase_Impl( nInitSize ),
		  nFirstFree( 0 )
	{}
};

class SfxStyleSheetIterator;

struct SfxItemPool_Impl
{
	SfxBroadcaster					aBC;
	SfxPoolItemArray_Impl**			ppPoolItems;
	SfxPoolVersionArr_Impl			aVersions;
	USHORT							nVersion;
	USHORT							nLoadingVersion;
	USHORT							nInitRefCount; // 1, beim Laden ggf. 2
	USHORT							nVerStart, nVerEnd; // WhichRange in Versions
	USHORT							nStoringStart, nStoringEnd; // zu speichernder Range
	BYTE							nMajorVer, nMinorVer; // Pool selbst
	SfxMapUnit                      eDefMetric;
	FASTBOOL						bInSetItem;
	FASTBOOL						bStreaming; // in Load() bzw. Store()

	SfxItemPool_Impl( USHORT nStart, USHORT nEnd )
		: ppPoolItems (new SfxPoolItemArray_Impl*[ nEnd - nStart + 1])
        , nLoadingVersion(0)
        , nInitRefCount(0)
        , nVerStart(0)
        , nVerEnd(0)
        , nStoringStart(0)
        , nStoringEnd(0)
        , nMajorVer(0)
        , nMinorVer(0)
        , bInSetItem(false)
        , bStreaming(false)
	{
		memset( ppPoolItems, 0, sizeof( SfxPoolItemArray_Impl* ) * ( nEnd - nStart + 1) );
	}

	~SfxItemPool_Impl()
	{
		delete[] ppPoolItems;
	}

	void DeleteItems()
	{
		delete[] ppPoolItems; ppPoolItems = 0;
	}
};

// -----------------------------------------------------------------------

// IBM-C-Set mag keine doppelten Defines
#ifdef DBG
#  undef DBG
#endif

#if defined(DBG_UTIL) && defined(MSC)
#define SFX_TRACE(s,p) \
		{ \
			ByteString aPtr(RTL_CONSTASCII_STRINGPARAM("0x0000:0x0000")); \
			_snprintf(const_cast< sal_Char *>(aPtr.GetBuffer()), aPtr.Len(), \
					   "%lp", p ); \
			aPtr.Insert(s, 0); \
			DbgTrace( aPtr.GetBuffer() ); \
		}
#define DBG(x) x
#else
#define SFX_TRACE(s,p)
#define DBG(x)
#endif

#define CHECK_FILEFORMAT( rStream, nTag ) \
	{   USHORT nFileTag; \
		rStream >> nFileTag; \
		if ( nTag != nFileTag ) \
        { \
            DBG_ERROR( #nTag ); /*! s.u. */ \
			/*! error-code setzen und auswerten! */ \
			(rStream).SetError(SVSTREAM_FILEFORMAT_ERROR); \
			pImp->bStreaming = FALSE; \
            return rStream; \
        } \
    }

#define CHECK_FILEFORMAT_RELEASE( rStream, nTag, pPointer ) \
   {   USHORT nFileTag; \
       rStream >> nFileTag; \
       if ( nTag != nFileTag ) \
        { \
            DBG_ERROR( #nTag ); /*! s.u. */ \
           /*! error-code setzen und auswerten! */ \
           (rStream).SetError(SVSTREAM_FILEFORMAT_ERROR); \
           pImp->bStreaming = FALSE; \
           delete pPointer; \
            return rStream; \
        } \
    }

#define CHECK_FILEFORMAT2( rStream, nTag1, nTag2 ) \
	{   USHORT nFileTag; \
		rStream >> nFileTag; \
		if ( nTag1 != nFileTag && nTag2 != nFileTag ) \
        { \
            DBG_ERROR( #nTag1 ); /*! s.u. */ \
			/*! error-code setzen und auswerten! */ \
			(rStream).SetError(SVSTREAM_FILEFORMAT_ERROR); \
			pImp->bStreaming = FALSE; \
            return rStream; \
        } \
    }

#define SFX_ITEMPOOL_VER_MAJOR      	BYTE(2)
#define SFX_ITEMPOOL_VER_MINOR      	BYTE(0)

#define SFX_ITEMPOOL_TAG_STARTPOOL_4	USHORT(0x1111)
#define SFX_ITEMPOOL_TAG_STARTPOOL_5	USHORT(0xBBBB)
#define SFX_ITEMPOOL_TAG_ITEMPOOL 		USHORT(0xAAAA)
#define SFX_ITEMPOOL_TAG_ITEMS			USHORT(0x2222)
#define SFX_ITEMPOOL_TAG_ITEM			USHORT(0x7777)
#define SFX_ITEMPOOL_TAG_SIZES			USHORT(0x3333)
#define SFX_ITEMPOOL_TAG_DEFAULTS		USHORT(0x4444)
#define SFX_ITEMPOOL_TAG_VERSIONMAP 	USHORT(0x5555)
#define SFX_ITEMPOOL_TAG_HEADER 		USHORT(0x6666)
#define SFX_ITEMPOOL_TAG_ENDPOOL		USHORT(0xEEEE)
#define SFX_ITEMPOOL_TAG_TRICK4OLD  	USHORT(0xFFFF)

#define SFX_ITEMPOOL_REC				BYTE(0x01)
#define SFX_ITEMPOOL_REC_HEADER 	    BYTE(0x10)
#define SFX_ITEMPOOL_REC_VERSIONMAP 	USHORT(0x0020)
#define SFX_ITEMPOOL_REC_WHICHIDS   	USHORT(0x0030)
#define SFX_ITEMPOOL_REC_ITEMS  		USHORT(0x0040)
#define SFX_ITEMPOOL_REC_DEFAULTS		USHORT(0x0050)

#define SFX_ITEMSET_REC					BYTE(0x02)

#define SFX_STYLES_REC                  BYTE(0x03)
#define SFX_STYLES_REC_HEADER		USHORT(0x0010)
#define SFX_STYLES_REC_STYLES		USHORT(0x0020)

//========================================================================

inline USHORT SfxItemPool::GetIndex_Impl(USHORT nWhich) const
{
	DBG_CHKTHIS(SfxItemPool, 0);
	DBG_ASSERT(nWhich >= nStart && nWhich <= nEnd, "Which-Id nicht im Pool-Bereich");
	return nWhich - nStart;
}

}

