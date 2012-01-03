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



#include <stdio.h>


#include <bf_so3/svstor.hxx>
#include "bf_so3/soerr.hxx"
#include <sot/storinfo.hxx>
#include <sot/stg.hxx>

#include <tools/debug.hxx>

// Freigeben, wenn SD-Storages verwendet werden sollen
#define SDSTORAGES
#define SD_ERROR	DBG_ERROR( "use ole2, while SDSTORAGES defined" )

// kein Profiling
#undef DBG_PROFSTART
#define DBG_PROFSTART(John)
#undef DBG_PROFSTOP
#define DBG_PROFSTOP(John)

namespace binfilter {

/************** class SvStorageStream ***********************************/
SV_IMPL_FACTORY(SvStorageStreamFactory)
	{
	}
};
TYPEINIT1(SvStorageStreamFactory,SvFactory);

SO2_IMPL_STANDARD_CLASS2_DLL(SvStorageStream,SvStorageStreamFactory,SvObject,SotStorageStream,
							 0x89F1CAA0L, 0x7010, 0x101B,
							 0x80,0x4C,0xFD,0xFD,0xFD,0xFD,0xFD,0xFD )

::IUnknown * SvStorageStream::GetMemberInterface( const SvGlobalName & )
{
	return NULL;
}

void SvStorageStream::TestMemberObjRef( BOOL /*bFree*/ )
{
}

#ifdef TEST_INVARIANT
void SvStorageStream::TestMemberInvariant( BOOL /*bPrint*/ )
{
}
#endif


/************************************************************************
|*    SvStorageStream::SvStorageStream()
|*
|*    Beschreibung
*************************************************************************/
SvStorageStream::SvStorageStream( BaseStorageStream * pStm )
	: SotStorageStream( pStm )
	, pObjI( NULL )
	, pRelease( NULL )
{
}

SvStorageStream::SvStorageStream()
	: pObjI( NULL )
	, pRelease( NULL )
{
}

/************************************************************************
|*    SvStorageStream::~SvStorageStream()
|*
|*    Beschreibung
*************************************************************************/
SvStorageStream::~SvStorageStream()
{
    //SetBufferSize( 0 );
}

/*************************************************************************
|*    SvStorageStream::GetStream()
|*
|*    Beschreibung
*************************************************************************/
USHORT SvStorageStream::IsA() const
{
	return ID_STORAGESTREAM;
}

void SvStorageStream::SyncSvStream()
{
	SotStorageStream::SyncSvStream();
}

ULONG SvStorageStream::GetData( void* pData, ULONG nSize )
{
    return SotStorageStream::GetData( pData, nSize );
}

ULONG SvStorageStream::PutData( const void* pData, ULONG nSize )
{
    return SotStorageStream::PutData( pData, nSize );
}

ULONG SvStorageStream::SeekPos( ULONG nPos )
{
    return SotStorageStream::SeekPos( nPos );
}

void SvStorageStream::FlushData()
{
    SotStorageStream::FlushData();
}

void SvStorageStream::SetSize( ULONG nNewSize )
{
    SotStorageStream::SetSize( nNewSize );
}

BOOL SvStorageStream::Commit()
{
    return SotStorageStream::Commit();
}

BOOL SvStorageStream::Revert()
{
    return SotStorageStream::Revert();
}

/************** class SvStorage ******************************************
*************************************************************************/
SV_IMPL_FACTORY(SvStorageFactory)
	{
	}
};
TYPEINIT1(SvStorageFactory,SvFactory);

SO2_IMPL_STANDARD_CLASS2_DLL(SvStorage,SvStorageFactory,SvObject,SotStorage,
							 0xCD956821L, 0x70B5, 0x101B,
							 0x80,0x4C,0xFD,0xFD,0xFD,0xFD,0xFD,0xFD )

::IUnknown * SvStorage::GetMemberInterface( const SvGlobalName & )
{
	return NULL;
}

void SvStorage::TestMemberObjRef( BOOL /*bFree*/ )
{
}

#ifdef TEST_INVARIANT
void SvStorage::TestMemberInvariant( BOOL bPrint )
{
	(void)bPrint;
}
#endif

/************************************************************************
|*
|*    SvStorage::SvStorage()
|*
|*    Beschreibung      Es muss ein I... Objekt an SvObject uebergeben
|*                      werden, da es sonst selbst ein IUnknown anlegt und
|*                      festlegt, dass alle weiteren I... Objekte mit
|*                      delete zerstoert werden (Owner() == TRUE).
|*                      Es werden aber nur IStorage Objekte benutzt und nicht
|*                      selbst implementiert, deshalb wird so getan, als ob
|*                      das IStorage Objekt von aussen kam und es wird mit
|*                      Release() freigegeben.
|*                      Die CreateStorage Methoden werden benoetigt, um
|*                      ein IStorage Objekt vor dem Aufruf von SvObject
|*                      zu erzeugen (Own, !Own automatik).
|*                      Hat CreateStorage ein Objekt erzeugt, dann wurde
|*                      der RefCounter schon um 1 erhoet.
|*                      Die Uebergabe erfolgt in pStorageCTor. Die Variable
|*                      ist NULL, wenn es nicht geklappt hat.
|*    Ersterstellung    MM 23.06.94
|*    Letzte Aenderung  MM 23.06.94
|*
*************************************************************************/
#define INIT_SVSTORAGE()            		\
	, pRelease( NULL )						\
    , pObjI( NULL )

SvStorage::SvStorage()
	: SotStorage()
	INIT_SVSTORAGE()
{
}


#define ERASEMASK  ( STREAM_TRUNC | STREAM_WRITE | STREAM_SHARE_DENYALL )
SvStorage::SvStorage( const String & rName, StreamMode nMode,
					 StorageMode nStorageMode )
	: SotStorage( rName, nMode, nStorageMode )
	INIT_SVSTORAGE()
{
}

SvStorage::SvStorage( BOOL bUCBStorage, const String & rName, StreamMode nMode,
					 StorageMode nStorageMode )
	: SotStorage( bUCBStorage, rName, nMode, nStorageMode )
	INIT_SVSTORAGE()
{
}

SvStorage::SvStorage( BaseStorage * pStor )
	: SotStorage( pStor )
	INIT_SVSTORAGE()
{
}

SvStorage::SvStorage( SvStream & rStm )
	: SotStorage( rStm )
	INIT_SVSTORAGE()
{
}

SvStorage::SvStorage( BOOL bUCBStorage, SvStream & rStm )
    : SotStorage( bUCBStorage, rStm )
	INIT_SVSTORAGE()
{
}

SvStorage::SvStorage( SvStream * pStm, BOOL bDelete )
	: SotStorage( pStm, bDelete )
	INIT_SVSTORAGE()
{
}

/*************************************************************************
|*    SvStorage::~SvStorage()
|*
|*    Beschreibung
*************************************************************************/
SvStorage::~SvStorage()
{
}

BOOL SvStorage::IsStorageFile( const String & rFileName )
{
	return SotStorage::IsStorageFile( rFileName );
}

const String & SvStorage::GetName() const
{
    return SotStorage::GetName();
}

void SvStorage::SetClass( const SvGlobalName & rName,
						  ULONG nOriginalClipFormat,
						  const String & rUserTypeName )
{
    SotStorage::SetClass( rName, nOriginalClipFormat, rUserTypeName );
}

void SvStorage::SetConvertClass( const SvGlobalName & rName,
								 ULONG nOriginalClipFormat,
								 const String & rUserTypeName )
{
    SotStorage::SetConvertClass( rName, nOriginalClipFormat, rUserTypeName );
}

SvGlobalName SvStorage::GetClassName()
{
    return SotStorage::GetClassName();
}

ULONG SvStorage::GetFormat()
{
    return SotStorage::GetFormat();
}

String SvStorage::GetUserName()
{
    return SotStorage::GetUserName();
}

BOOL SvStorage::ShouldConvert()
{
    return SotStorage::ShouldConvert();
}

void SvStorage::FillInfoList( SvStorageInfoList * pFillList ) const
{
    SotStorage::FillInfoList( pFillList );
}

BOOL SvStorage::CopyTo( SotStorage * pDestStg )
{
    return SotStorage::CopyTo( pDestStg );
}

BOOL SvStorage::Commit()
{
    return SotStorage::Commit();
}

BOOL SvStorage::Revert()
{
    return SotStorage::Revert();
}

BOOL SvStorage::IsStorage( const String & rEleName ) const
{
    return SotStorage::IsStorage( rEleName );
}

BOOL SvStorage::IsStream( const String & rEleName ) const
{
    return SotStorage::IsStream( rEleName );
}

BOOL SvStorage::IsContained( const String & rEleName ) const
{
    return SotStorage::IsContained( rEleName );
}

BOOL SvStorage::Remove( const String & rEleName )
{
    return SotStorage::Remove( rEleName );
}

BOOL SvStorage::Rename( const String & rEleName, const String & rNewName )
{
    return SotStorage::Rename( rEleName, rNewName );
}

BOOL SvStorage::CopyTo( const String & rEleName,
						SotStorage * pNewSt, const String & rNewName )
{
    return SotStorage::CopyTo( rEleName, pNewSt, rNewName );
}

BOOL SvStorage::MoveTo( const String & rEleName,
						SotStorage * pNewSt, const String & rNewName )
{
    return SotStorage::MoveTo( rEleName, pNewSt, rNewName );
}

/*************************************************************************
|*    SvStorage::OpenStream()
|*
|*    Beschreibung
*************************************************************************/
SotStorageStream * SvStorage::OpenSotStream( const String & rEleName,
										 StreamMode nMode,
										 StorageMode nStorageMode )
{
	DBG_ASSERT( !nStorageMode, "StorageModes ignored" );

	SvStorageStream * pStm = NULL;
	DBG_ASSERT( Owner(), "must be owner" );
	// volle Ole-Patches einschalten
	// egal was kommt, nur exclusiv gestattet
	nMode |= STREAM_SHARE_DENYALL;
	ULONG nE = m_pOwnStg->GetError();
    BaseStorageStream * p = m_pOwnStg->OpenStream( rEleName, nMode,
						(nStorageMode & STORAGE_TRANSACTED) ? FALSE : TRUE );
	pStm = new SvStorageStream( p );
	if( !nE )
		m_pOwnStg->ResetError(); // kein Fehler setzen
	return pStm;
}

/*************************************************************************
|*    SvStorage::OpenStorage()
|*
|*    Beschreibung
*************************************************************************/
SotStorage * SvStorage::OpenSotStorage( const String & rEleName,
									StreamMode nMode,
									StorageMode nStorageMode )
{
	SvStorage * pStor = NULL;
	DBG_ASSERT( Owner(), "must be owner" );
	nMode |= STREAM_SHARE_DENYALL;
	ULONG nE = m_pOwnStg->GetError();
    BaseStorage * p = m_pOwnStg->OpenStorage( rEleName, nMode,
					(nStorageMode & STORAGE_TRANSACTED) ? FALSE : TRUE );
	pStor = new SvStorage( p );
	if( !nE )
		m_pOwnStg->ResetError(); // kein Fehler setzen
	return pStor;
}

SvStorage * SvStorage::OpenUCBStorage( const String & rEleName,
									StreamMode nMode,
									StorageMode nStorageMode )
{
	SvStorage * pStor = NULL;
	DBG_ASSERT( Owner(), "must be owner" );
	nMode |= STREAM_SHARE_DENYALL;
	ULONG nE = m_pOwnStg->GetError();
    BaseStorage * p = m_pOwnStg->OpenUCBStorage( rEleName, nMode,
					(nStorageMode & STORAGE_TRANSACTED) ? FALSE : TRUE );
	pStor = new SvStorage( p );
	if( !nE )
		m_pOwnStg->ResetError(); // kein Fehler setzen
	return pStor;
}

SvStorage * SvStorage::OpenOLEStorage( const String & rEleName,
									StreamMode nMode,
									StorageMode nStorageMode )
{
	SvStorage * pStor = NULL;
	DBG_ASSERT( Owner(), "must be owner" );
	nMode |= STREAM_SHARE_DENYALL;
	ULONG nE = m_pOwnStg->GetError();
    BaseStorage * p = m_pOwnStg->OpenOLEStorage( rEleName, nMode,
					(nStorageMode & STORAGE_TRANSACTED) ? FALSE : TRUE );
	pStor = new SvStorage( p );
	if( !nE )
		m_pOwnStg->ResetError(); // kein Fehler setzen
	return pStor;
}
}
