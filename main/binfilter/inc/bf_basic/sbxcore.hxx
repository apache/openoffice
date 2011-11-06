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



#ifndef _SBXCORE_HXX
#define _SBXCORE_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _RTTI_HXX //autogen
#include <tools/rtti.hxx>
#endif
#ifndef _REF_HXX
#include <tools/ref.hxx>
#endif

#ifndef _DEBUG_HXX
#include <tools/debug.hxx>
#endif

#include "sbxdef.hxx"

class SvStream;
class String;
class UniString;

// Das nachfolgende Makro definiert die vier  (fuenf) notwendigen Methoden
// innerhalb eines SBX-Objekts. LoadPrivateData() und StorePrivateData()
// muessen selbst implementiert werden. Sie sind fuer das Laden/Speichern
// der Daten der abgeleiteten Klasse notwendig. Load() und Store() duerfen
// nicht ueberlagert werden.

// Diese Version des Makros definiert keine Load/StorePrivateData()-Methoden

#define SBX_DECL_PERSIST_NODATA( nCre, nSbxId, nVer )		\
	virtual UINT32 GetCreator() const { return nCre;   }	\
	virtual UINT16 GetVersion() const { return nVer;   }	\
	virtual UINT16 GetSbxId() const	  { return nSbxId; }

#define SBX_DECL_PERSIST_NODATA_()							\
	virtual UINT32 GetCreator() const;						\
	virtual UINT16 GetVersion() const;						\
	virtual UINT16 GetSbxId() const;

// Diese Version des Makros definiert Load/StorePrivateData()-Methoden

#define SBX_DECL_PERSIST( nCre, nSbxId, nVer )				\
	virtual BOOL LoadPrivateData( SvStream&, USHORT ); 		\
	virtual BOOL StorePrivateData( SvStream& ) const;  		\
	SBX_DECL_PERSIST_NODATA( nCre, nSbxId, nVer )

#define SBX_DECL_PERSIST_()									\
	virtual BOOL LoadPrivateData( SvStream&, USHORT ); 		\
	virtual BOOL StorePrivateData( SvStream& ) const;  		\
	SBX_DECL_PERSIST_NODATA_()

#define SBX_IMPL_PERSIST( C, nCre, nSbxId, nVer )			\
	UINT32 C::GetCreator() const { return nCre;   }			\
	UINT16 C::GetVersion() const { return nVer;   }			\
	UINT16 C::GetSbxId() const	 { return nSbxId; }

namespace binfilter {

class SbxBase;
class SbxFactory;
class SbxObject;

class SbxBaseImpl;

class SbxBase : virtual public SvRefBase
{
	SbxBaseImpl* mpSbxBaseImpl;	// Impl data

	virtual BOOL LoadData( SvStream&, USHORT );
	virtual BOOL StoreData( SvStream& ) const;
protected:
	USHORT nFlags;			// Flag-Bits

	SbxBase();
	SbxBase( const SbxBase& );
	SbxBase& operator=( const SbxBase& );
	virtual ~SbxBase();
	SBX_DECL_PERSIST(0,0,0);
public:
	TYPEINFO();
	inline void		SetFlags( USHORT n );
	inline USHORT	GetFlags() const;
	inline void		SetFlag( USHORT n );
	inline void		ResetFlag( USHORT n );
	inline BOOL		IsSet( USHORT n ) const;
	inline BOOL		IsReset( USHORT n ) const;
	inline BOOL		CanRead() const;
	inline BOOL		CanWrite() const;
	inline BOOL		IsModified() const;
	inline BOOL		IsConst() const;
	inline BOOL		IsHidden() const;
	inline BOOL		IsVisible() const;

	virtual BOOL IsFixed() const;
	virtual void SetModified( BOOL );

	virtual SbxDataType  GetType()  const;
	virtual SbxClassType GetClass() const;

	virtual void Clear();

	static SbxBase* Load( SvStream& );
	static void Skip( SvStream& );
	BOOL Store( SvStream& );
	virtual BOOL LoadCompleted();
	virtual BOOL StoreCompleted();

	static SbxError GetError();
	static void SetError( SbxError );
	static BOOL IsError();
	static void ResetError();

	// Setzen der Factory fuer Load/Store/Create
	static void AddFactory( SbxFactory* );
	static void RemoveFactory( SbxFactory* );

	static SbxBase* Create( UINT16, UINT32=SBXCR_SBX );
	static SbxObject* CreateObject( const String& );
	// Sbx-Loesung als Ersatz fuer SfxBroadcaster::Enable()
	static BOOL StaticIsEnabledBroadcasting( void );
};

#ifndef SBX_BASE_DECL_DEFINED
#define SBX_BASE_DECL_DEFINED
SV_DECL_REF(SbxBase)
#endif

inline void SbxBase::SetFlags( USHORT n )
{ nFlags = n; }

inline USHORT SbxBase::GetFlags() const
{ return nFlags; }

inline void SbxBase::SetFlag( USHORT n )
{ nFlags |= n; }

inline void SbxBase::ResetFlag( USHORT n )
{ nFlags &= ~n; }

inline BOOL SbxBase::IsSet( USHORT n ) const
{ return BOOL( ( nFlags & n ) != 0 ); }

inline BOOL SbxBase::IsReset( USHORT n ) const
{ return BOOL( ( nFlags & n ) == 0 ); }

inline BOOL SbxBase::CanRead() const
{ return IsSet( SBX_READ ); }

inline BOOL SbxBase::CanWrite() const
{ return IsSet( SBX_WRITE ); }

inline BOOL SbxBase::IsModified() const
{ return IsSet( SBX_MODIFIED ); }

inline BOOL SbxBase::IsConst() const
{ return IsSet( SBX_CONST ); }

inline BOOL SbxBase::IsHidden() const
{ return IsSet( SBX_HIDDEN ); }

inline BOOL SbxBase::IsVisible() const
{ return IsReset( SBX_INVISIBLE ); }

}

#endif
