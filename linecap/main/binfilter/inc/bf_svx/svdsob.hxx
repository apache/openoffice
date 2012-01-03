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



#ifndef _SVDSOB_HXX
#define _SVDSOB_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _COM_SUN_STAR_UNO_ANY_HXX_ 
#include <com/sun/star/uno/Any.hxx>
#endif

#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif

#ifndef _STRING_H
#define _STRING_H
#endif
namespace binfilter {

////////////////////////////////////////////////////////////////////////////////////////////////////
/*
  Deklaration eines statischen Mengentyps. Die Menge kann die Elemente
  0..255 aufnehmen und verbraucht stets 32 Bytes.
*/

class SetOfByte {
protected:
	BYTE aData[32];
public:
	SetOfByte(FASTBOOL bInitVal=FALSE)                  { memset(aData,bInitVal ? 0xFF : 0x00,sizeof(aData)); }
	FASTBOOL operator==(const SetOfByte& rCmpSet) const { return (memcmp(aData,rCmpSet.aData,sizeof(aData))==0); }
	FASTBOOL operator!=(const SetOfByte& rCmpSet) const { return (memcmp(aData,rCmpSet.aData,sizeof(aData))!=0); }
	void     Set  (BYTE a)                              { aData[a/8]|=1<<a%8; }
	void     Clear(BYTE a)                              { aData[a/8]&=~(1<<a%8); }
	void     Set  (BYTE a, FASTBOOL b)                  { if (b) Set(a); else Clear(a); }
	FASTBOOL IsSet(BYTE a) const                        { return (aData[a/8]&1<<a%8)!=0; }
	FASTBOOL IsSet(USHORT a) const                      { return (a<=255) && (aData[a/8]&1<<a%8)!=0; }
	void     SetAll()                                   { memset(aData,0xFF,sizeof(aData)); }
	void     ClearAll()                                 { memset(aData,0x00,sizeof(aData)); }
	FASTBOOL IsEmpty() const;
	void     operator&=(const SetOfByte& r2ndSet);
	friend inline SvStream& operator<<(SvStream& rOut, const SetOfByte& rSet);
	friend inline SvStream& operator>>(SvStream& rIn, SetOfByte& rSet);

 	void	PutValue( const ::com::sun::star::uno::Any & rAny );		// initialize this set with a uno sequence of sal_Int8
	void	QueryValue( ::com::sun::star::uno::Any & rAny ) const;	// returns a uno sequence of sal_Int8
};

inline SvStream& operator<<(SvStream& rOut, const SetOfByte& rSet)
{
	rOut.Write((char*)rSet.aData,32);
	return rOut;
}

inline SvStream& operator>>(SvStream& rIn, SetOfByte& rSet)
{
	rIn.Read((char*)rSet.aData,32);
	return rIn;
}

}//end of namespace binfilter
#endif // _SVDSOB_HXX

