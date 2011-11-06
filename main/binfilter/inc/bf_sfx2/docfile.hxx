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


#ifndef _SFXDOCFILE_HXX
#define _SFXDOCFILE_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _COM_SUN_STAR_IO_XOUTPUTSTREAM_HPP_
#include <com/sun/star/io/XOutputStream.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XINPUTSTREAM_HPP_
#include <com/sun/star/io/XInputStream.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENT_HPP_
#include <com/sun/star/ucb/XContent.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCOMMANDENVIRONMENT_HPP_
#include <com/sun/star/ucb/XCommandEnvironment.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XINTERACTIONHANDLER_HPP_
#include <com/sun/star/task/XInteractionHandler.hpp>
#endif
#ifndef _SVSTOR_HXX //autogen
#include <bf_so3/svstor.hxx>
#endif
#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif
#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _LIST_HXX //autogen
#include <tools/list.hxx>
#endif
#ifndef _SFXLSTNER_HXX //autogen
#include <bf_svtools/lstner.hxx>
#endif

#include <bf_svtools/cancel.hxx>
#include <cppuhelper/weak.hxx>

#include <bf_sfx2/sfxuno.hxx>
#include <bf_sfx2/docinf.hxx>
class Timer;
class DateTime;
class SvStringsDtor;
class INetURLObject;
namespace binfilter {
class SvKeyValueIterator;

class SfxItemSet;

class SfxObjectFactory;
class SfxFilter;
class SfxMedium_Impl;
class SfxObjectShell;
class SfxFrame;
class LoadEnvironment_Impl;

class SfxLoadEnvironment;

#define SFX_TFPRIO_SYNCHRON                        0
#define SFX_TFPRIO_DOC                            10
#define SFX_TFPRIO_VISIBLE_LOWRES_GRAPHIC         20
#define SFX_TFPRIO_VISIBLE_HIGHRES_GRAPHIC        21
#define SFX_TFPRIO_PLUGINS                        40
#define SFX_TFPRIO_INVISIBLE_LOWRES_GRAPHIC       50
#define SFX_TFPRIO_INVISIBLE_HIGHRES_GRAPHIC      51
#define SFX_TFPRIO_DOWNLOADS                      60

#if _SOLAR__PRIVATE
#ifndef STRING_LIST
#define STRING_LIST
DECLARE_LIST( StringList, String* )//STRIP008 DECLARE_LIST( StringList, String* );
#endif
#endif

//____________________________________________________________________________________________________________________________________
//	defines for namespaces
//____________________________________________________________________________________________________________________________________

#define	OUSTRING					::rtl::OUString
#define	XMULTISERVICEFACTORY		::com::sun::star::lang::XMultiServiceFactory
#define	XSERVICEINFO				::com::sun::star::lang::XServiceInfo
#define	OWEAKOBJECT					::cppu::OWeakObject
#define	REFERENCE					::com::sun::star::uno::Reference
#define	XINTERFACE					::com::sun::star::uno::XInterface
#define	SEQUENCE					::com::sun::star::uno::Sequence
#define	EXCEPTION					::com::sun::star::uno::Exception
#define	RUNTIMEEXCEPTION			::com::sun::star::uno::RuntimeException
#define	ANY							::com::sun::star::uno::Any

class SfxPoolCancelManager	:	public SfxCancelManager	,
								public SfxCancellable	,
								public SfxListener		,
								public SvRefBase
{
	SfxCancelManagerWeak 	wParent;

							~SfxPoolCancelManager();
public:
							SfxPoolCancelManager( SfxCancelManager* pParent, const String& rName );
	virtual void 			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );
	virtual void 			Cancel();
};

SV_DECL_IMPL_REF( SfxPoolCancelManager )

struct SfxVersionInfo
{
	String					aName;
	String					aComment;
	SfxStamp				aCreateStamp;

							SfxVersionInfo();
							SfxVersionInfo( const SfxVersionInfo& rInfo )
							{ *this = rInfo; }

	SfxVersionInfo& 		operator=( const SfxVersionInfo &rInfo )
							{
								aName = rInfo.aName;
								aComment = rInfo.aComment;
								aCreateStamp = rInfo.aCreateStamp;
								return *this;
							}
};
DECLARE_LIST( _SfxVersionTable, SfxVersionInfo* )
class SfxVersionTableDtor : public _SfxVersionTable
{
public:
							SfxVersionTableDtor( const sal_uInt16 nInitSz=0, const sal_uInt16 nReSz=1 )
								: _SfxVersionTable( nInitSz, nReSz )
							{}

							SfxVersionTableDtor( const SfxVersionTableDtor &rCpy )
							{ *this = rCpy; }

							~SfxVersionTableDtor()
							{ DelDtor(); }

	void 					DelDtor();
	SvStream&				Read( SvStream & );
};

class SfxMedium : public SvRefBase
{
	sal_uInt32          eError;
	sal_Bool            bDirect:1,
						bRoot:1,
						bSetFilter:1,
						bTriedStorage;
	StreamMode          nStorOpenMode;
	INetURLObject*      pURLObj;
	String              aName;
	SvGlobalName        aFilterClass;
	SvStream*			pInStream;
    SvStream*           pOutStream;
	SvStorageRef        aStorage;
	const SfxFilter*	pFilter;
	SfxItemSet*			pSet;
	SfxMedium_Impl*		pImp;
	String           	aLogicName;
	String           	aLongName;
	sal_Bool            bRemote;

#if _SOLAR__PRIVATE
    void                SetIsRemote_Impl();
    void                CloseInStream_Impl();
    sal_Bool            CloseOutStream_Impl();
	DECL_STATIC_LINK( 	SfxMedium, UCBHdl_Impl, sal_uInt32 * );
#endif

public:

	SvCompatWeakHdl*    GetHdl();

						SfxMedium();
						SfxMedium( const String &rName,
								   StreamMode nOpenMode,
								   sal_Bool bDirect,
								   const SfxFilter *pFilter = 0,
								   SfxItemSet *pSet = 0 );
                        SfxMedium( SvStorage *pTheStorage, sal_Bool bRoot = sal_False );
						~SfxMedium();

    void                UseInteractionHandler( BOOL );
    ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler >
						GetInteractionHandler();

    void                SetLoadTargetFrame(SfxFrame* pFrame );
	SfxFrame*           GetLoadTargetFrame() const;
	void                CancelTransfers();

	void                SetReferer( const String& rRefer );
	void                SetTransferPriority( sal_uInt16 nPrio );
    void                SetFilter( const SfxObjectFactory &rFact, const String & rFilter );
	void			    SetFilter(const SfxFilter *pFlt, sal_Bool bResetOrig = sal_False);
	const SfxFilter *   GetFilter() const { return pFilter; }
	const String&       GetOrigURL() const;
	SfxItemSet	*		GetItemSet() const;
	void                Close();
	void                ReOpen();
	const String&       GetName() const {return aLogicName;}
#if defined SINIX && defined GCC && defined C272
	const INetURLObject& GetURLObject();
#else
	const INetURLObject& GetURLObject() const;
#endif
    ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XContent > GetContent() const;
	const String&       GetPhysicalName() const;
    sal_Bool            IsTemporary() const;
    sal_Bool            IsRemote();
	sal_Bool            IsOpen() const { return aStorage.Is() || pInStream; }
	void                StartDownload();
	void                DownLoad( const Link& aLink = Link());
	void                SetDoneLink( const Link& rLink );
	void                SetDataAvailableLink( const Link& rLink );

	sal_uInt32          GetMIMEAndRedirect( String& );
	sal_uInt32          GetErrorCode() const;
	sal_uInt32          GetError() const
						{ return ERRCODE_TOERROR(GetErrorCode()); }
	sal_uInt32			GetLastStorageCreationState();

	void                SetError( sal_uInt32 nError ) { eError = nError; }

	void                CloseInStream();
	sal_Bool            CloseOutStream();

	sal_Bool            IsRoot() const { return bRoot; }
	void				CloseStorage();

	StreamMode			GetOpenMode() const { return nStorOpenMode; }
    void                SetOpenMode( StreamMode nStorOpen, sal_Bool bDirect, sal_Bool bDontClose = sal_False );
	sal_Bool			IsDirect() const { return bDirect? sal_True: sal_False; }

	SvStream*           GetInStream();
	SvStream*           GetOutStream();

	sal_Bool            Commit();
	sal_Bool            TryStorage();
    sal_Bool            IsStorage();
	SvStorage*          GetStorage();
    SvStorage*          GetOutputStorage( BOOL bUCBStorage = FALSE );
	void				ResetError();
	sal_Bool            UsesCache() const;
	void                SetUsesCache( sal_Bool );
	void                SetDontCreateCancellable();
	sal_Bool			IsAllowedForExternalBrowser() const;
	long				GetFileVersion() const;

	const SfxVersionTableDtor* GetVersionList();
	sal_Bool			IsReadOnly();

    ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >  GetInputStream();

	void				CreateTempFile();
	void				CreateTempFileNoCopy();
	void				TryToSwitchToRepairedTemp();

    void                SetLoadEnvironment( SfxLoadEnvironment* pEnv );
    SfxLoadEnvironment* GetLoadEnvironment() const;

	::rtl::OUString		GetCharset();
	const String&		GetBaseURL();

#if _SOLAR__PRIVATE
    ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > GetInputStream_Impl();
    SvStorage*          GetStorage_Impl( BOOL bUCBStorage );
	SfxPoolCancelManager*   GetCancelManager_Impl() const;
	void                SetCancelManager_Impl( SfxPoolCancelManager* pMgr );

	SvKeyValueIterator* GetHeaderAttributes_Impl();
	void                SetLoadEnvironment_Impl( LoadEnvironment_Impl* pEnv );
	LoadEnvironment_Impl* GetLoadEnvironment_Impl() const;

	// Diese Protokolle liefern MIME Typen
	sal_Bool            SupportsMIME_Impl() const;

	void                Init_Impl();
	void                ForceSynchronStream_Impl( sal_Bool bSynchron );

	void                GetMedium_Impl();
	void                Transfer_Impl();
	void                CreateFileStream();
	void				SetUpdatePickList(sal_Bool);

	void                SetLongName(const String &rName)
						{ aLongName = rName; }
	const String &      GetLongName() const { return aLongName; }
	ErrCode             CheckOpenMode_Impl( sal_Bool bSilent, sal_Bool bAllowRO = sal_True );
	sal_Bool			IsDownloadDone_Impl();
	void				ClearBackup_Impl();
    void                Done_Impl( ErrCode );
    void                DataAvailable_Impl();
    void                Cancel_Impl();
	void                SetPhysicalName_Impl(const String& rName);

	void 				DoInternalBackup_Impl( const ::ucbhelper::Content& aOriginalContent,
												const String& aPrefix,
												const String& aExtension,
												const String& aDestDir );

	sal_Bool 			TransactedTransferForFS_Impl( const INetURLObject& aSource, 
					 		const INetURLObject& aDest, 
					 		const ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XCommandEnvironment >& xComEnv );

#endif

	static SfxVersionTableDtor* GetVersionList( SvStorage* pStor );
};

SV_DECL_IMPL_REF( SfxMedium )
SV_DECL_COMPAT_WEAK( SfxMedium )

#ifndef SFXMEDIUM_LIST
#define SFXMEDIUM_LIST
DECLARE_LIST( SfxMediumList, SfxMedium* )//STRIP008 DECLARE_LIST( SfxMediumList, SfxMedium* );
#endif

}//end of namespace binfilter
#endif

