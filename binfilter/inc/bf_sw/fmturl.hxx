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


#ifndef _FMTURL_HXX
#define _FMTURL_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _FORMAT_HXX //autogen
#include <format.hxx>
#endif
class IntlWrapper; 

namespace binfilter {
class ImageMap; 


// URL, ServerMap und ClientMap

class SwFmtURL: public SfxPoolItem
{
	String	  sTargetFrameName;	// in diesen Frame soll die URL
	String	  sURL;				//Einfache URL
	String	  sName;			// Name des Anchors
	ImageMap *pMap;				//ClientSide Images

	BOOL      bIsServerMap;		//mit der URL eine ServerSideImageMap

	SwFmtURL& operator=( const SwFmtURL& );

public:
	SwFmtURL();
	SwFmtURL( const SwFmtURL& );
	~SwFmtURL();

	// "pure virtual Methoden" vom SfxPoolItem
	virtual int             operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVer) const;
	virtual SvStream&		Store(SvStream &, USHORT nIVer) const;
	virtual USHORT			GetVersion( USHORT nFFVer ) const;
	virtual	BOOL        	 QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 ) const;
	virtual	BOOL			 PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMemberId = 0 );

	void SetTargetFrameName( const String& rStr ) { sTargetFrameName = rStr; }
	void SetURL( const String &rURL, BOOL bServerMap );
	void SetMap( const ImageMap *pM );	//Pointer wird kopiert!

	const String   &GetTargetFrameName()const { return sTargetFrameName; }
	const String   &GetURL()			const { return sURL; }
		  BOOL      IsServerMap()		const { return bIsServerMap; }
	const ImageMap *GetMap() 		    const { return pMap; }
		  ImageMap *GetMap()				  { return pMap; }

	const String& GetName() const 			{ return sName; }
	void SetName( const String& rNm )		{ sName = rNm; }
};


#if ! (defined(MACOSX) && ( __GNUC__ < 3 ))
// GrP moved to gcc_outl.cxx; revisit with gcc3
inline const SwFmtURL &SwAttrSet::GetURL(BOOL bInP) const
	{ return (const SwFmtURL&)Get( RES_URL,bInP); }

inline const SwFmtURL &SwFmt::GetURL(BOOL bInP) const
	{ return aSet.GetURL(bInP); }
#endif

} //namespace binfilter
#endif

