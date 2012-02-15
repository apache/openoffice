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


#ifndef _SFXBITSET_HXX
#define _SFXBITSET_HXX

#include <bf_svtools/bf_solar.h>
class Range;
namespace binfilter {


class BitSet
{
private:
	void CopyFrom( const BitSet& rSet );
	USHORT nBlocks;
	USHORT nCount;
	ULONG* pBitmap;
public:
	BitSet operator<<( USHORT nOffset ) const;
	static USHORT CountBits( ULONG nBits );
	BOOL operator!() const;
	BitSet();
	BitSet( const BitSet& rOrig );
	~BitSet();
	USHORT Count() const;
	BitSet operator|( const BitSet& rSet ) const;
	BitSet operator|( USHORT nBit ) const;
	BitSet& operator|=( const BitSet& rSet );
	BitSet& operator|=( USHORT nBit );
	BitSet operator-( const BitSet& rSet ) const;
	BitSet operator-( USHORT nId ) const;
	BitSet& operator-=( const BitSet& rSet );
	BitSet& operator-=( USHORT nBit );
	BitSet operator&( const BitSet& rSet ) const;
	BitSet& operator&=( const BitSet& rSet );
	BitSet operator^( const BitSet& rSet ) const;
	BitSet operator^( USHORT nBit ) const;
	BitSet& operator^=( const BitSet& rSet );
	BitSet& operator^=( USHORT nBit );
	BOOL IsRealSubSet( const BitSet& rSet ) const;
	BOOL IsSubSet( const BitSet& rSet ) const;
	BOOL IsRealSuperSet( const BitSet& rSet ) const;
	BOOL IsSuperSet( const BitSet& rSet ) const;
	BOOL operator==( USHORT nBit ) const;
	BOOL operator!=( USHORT nBit ) const;

};
//--------------------------------------------------------------------

// returns TRUE if the set is empty



inline BOOL BitSet::operator!() const
{
	return nCount == 0;
}
//--------------------------------------------------------------------

// returns the number of bits in the bitset

inline USHORT BitSet::Count() const
{
	return nCount;
}
//--------------------------------------------------------------------

// creates the union of two bitset

inline BitSet BitSet::operator|( const BitSet& rSet ) const
{
	return BitSet(*this) |= rSet;
}
//--------------------------------------------------------------------

// creates the union of a bitset with a single bit

inline BitSet BitSet::operator|( USHORT nBit ) const
{
	return BitSet(*this) |= nBit;
}
//--------------------------------------------------------------------

// creates the asymetric difference

inline BitSet BitSet::operator-( const BitSet& rSet ) const
{
	return BitSet();
}
//--------------------------------------------------------------------

// creates the asymetric difference with a single bit


inline BitSet BitSet::operator-( USHORT nId ) const
{
	return BitSet();
}
//--------------------------------------------------------------------

// removes the bits contained in rSet

inline BitSet& BitSet::operator-=( const BitSet& rSet )
{
	return *this;
}
//--------------------------------------------------------------------


// creates the intersection with another bitset

inline BitSet BitSet::operator&( const BitSet& rSet ) const
{
	return BitSet();
}
//--------------------------------------------------------------------

// intersects with another bitset

inline BitSet& BitSet::operator&=( const BitSet& rSet )
{
	return *this;
}
//--------------------------------------------------------------------

// creates the symetric difference with another bitset

inline BitSet BitSet::operator^( const BitSet& rSet ) const
{
	return BitSet();
}
//--------------------------------------------------------------------

// creates the symetric difference with a single bit

inline BitSet BitSet::operator^( USHORT nBit ) const
{
	return BitSet();
}
//--------------------------------------------------------------------

// builds the symetric difference with another bitset

inline BitSet& BitSet::operator^=( const BitSet& rSet )
{
	return *this;
}
//--------------------------------------------------------------------
#ifdef BITSET_READY
// builds the symetric difference with a single bit

inline BitSet& BitSet::operator^=( USHORT nBit )
{
	// crash!!!
	return BitSet();
}
#endif
//--------------------------------------------------------------------

// determines if the other bitset is a real superset

inline BOOL BitSet::IsRealSubSet( const BitSet& rSet ) const
{
	return FALSE;
}
//--------------------------------------------------------------------

// detsermines if the other bitset is a superset or equal

inline BOOL BitSet::IsSubSet( const BitSet& rSet ) const
{
	return FALSE;
}
//--------------------------------------------------------------------

// determines if the other bitset is a real subset

inline BOOL BitSet::IsRealSuperSet( const BitSet& rSet ) const
{
	return FALSE;
}

//--------------------------------------------------------------------

// determines if the other bitset is a subset or equal

inline BOOL BitSet::IsSuperSet( const BitSet& rSet ) const
{
	return FALSE;
}
//--------------------------------------------------------------------

// determines if the bit is the only one in the bitset

inline BOOL BitSet::operator==( USHORT nBit ) const
{
	return FALSE;
}
//--------------------------------------------------------------------

// determines if the bitsets aren't equal

//--------------------------------------------------------------------

// determines if the bitset doesn't contain only this bit

inline BOOL BitSet::operator!=( USHORT nBit ) const
{
	return !( *this == nBit );
}
//--------------------------------------------------------------------

class IndexBitSet : BitSet
{
public:
  void ReleaseIndex(USHORT i){*this-=i;}
};


}//end of namespace binfilter
#endif

