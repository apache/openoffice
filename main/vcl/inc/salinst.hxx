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



#ifndef _SV_SALINST_HXX
#define _SV_SALINST_HXX

#include "com/sun/star/uno/Reference.hxx"

#include "vcl/sv.h"
#include "vcl/displayconnectiondispatch.hxx"
#include "vcl/dllapi.h"

#include "tools/string.hxx"

#include "rtl/ref.hxx"
#include "rtl/string.hxx"

#include <list>

struct SystemParentData;
struct SalPrinterQueueInfo;
struct SalStatus;
struct ImplJobSetup;
class SalGraphics;
class SalFrame;
class SalObject;
class SalMenu;
class SalMenuItem;
class SalVirtualDevice;
class SalInfoPrinter;
class SalPrinter;
class SalTimer;
class ImplPrnQueueList;
class SalI18NImeStatus;
class SalSystem;
class SalBitmap;
struct SalItemParams;
class SalSession;
struct SystemGraphicsData;
struct SystemWindowData;
class Menu;

namespace vos { class IMutex; }

// ---------------
// - SalInstance -
// ---------------

class VCL_PLUGIN_PUBLIC SalInstance
{
private:
    rtl::Reference< vcl::DisplayConnectionDispatch > m_pEventInst;

public:
    SalInstance() {}
    virtual ~SalInstance();

    // Frame
    // DisplayName for Unix ???
    virtual SalFrame*      	CreateChildFrame( SystemParentData* pParent, sal_uLong nStyle ) = 0;
    virtual SalFrame*      	CreateFrame( SalFrame* pParent, sal_uLong nStyle ) = 0;
    virtual void				DestroyFrame( SalFrame* pFrame ) = 0;

    // Object (System Child Window)
    virtual SalObject*			CreateObject( SalFrame* pParent, SystemWindowData* pWindowData, sal_Bool bShow = sal_True ) = 0;
    virtual void				DestroyObject( SalObject* pObject ) = 0;

    // VirtualDevice
    // nDX and nDY in Pixel
    // nBitCount: 0 == Default(=as window) / 1 == Mono
    // pData allows for using a system dependent graphics or device context
    virtual SalVirtualDevice*	CreateVirtualDevice( SalGraphics* pGraphics,
                                                     long nDX, long nDY,
                                                     sal_uInt16 nBitCount, const SystemGraphicsData *pData = NULL ) = 0;
    virtual void				DestroyVirtualDevice( SalVirtualDevice* pDevice ) = 0;

    // Printer
    // pSetupData->mpDriverData can be 0
    // pSetupData must be updated with the current
    // JobSetup
    virtual SalInfoPrinter*	CreateInfoPrinter( SalPrinterQueueInfo* pQueueInfo,
                                               ImplJobSetup* pSetupData ) = 0;
    virtual void				DestroyInfoPrinter( SalInfoPrinter* pPrinter ) = 0;
    virtual SalPrinter*		CreatePrinter( SalInfoPrinter* pInfoPrinter ) = 0;
    virtual void				DestroyPrinter( SalPrinter* pPrinter ) = 0;

    virtual void				GetPrinterQueueInfo( ImplPrnQueueList* pList ) = 0;
    virtual void				GetPrinterQueueState( SalPrinterQueueInfo* pInfo ) = 0;
    virtual void				DeletePrinterQueueInfo( SalPrinterQueueInfo* pInfo ) = 0;
    virtual String             GetDefaultPrinter() = 0;

    // SalTimer
    virtual SalTimer*			CreateSalTimer() = 0;
    // SalI18NImeStatus
    virtual SalI18NImeStatus*	CreateI18NImeStatus() = 0;
    // SalSystem
    virtual SalSystem*			CreateSalSystem() = 0;
    // SalBitmap
    virtual SalBitmap*			CreateSalBitmap() = 0;

    // YieldMutex
    virtual vos::IMutex*		GetYieldMutex() = 0;
    virtual sal_uLong				ReleaseYieldMutex() = 0;
    virtual void				AcquireYieldMutex( sal_uLong nCount ) = 0;
    // return true, if yield mutex is owned by this thread, else false
    virtual bool                CheckYieldMutex() = 0;

	// wait next event and dispatch
    // must returned by UserEvent (SalFrame::PostEvent)
    // and timer
    virtual void				Yield( bool bWait, bool bHandleAllCurrentEvents ) = 0;
    virtual bool				AnyInput( sal_uInt16 nType ) = 0;

                            // Menus
    virtual SalMenu*        CreateMenu( sal_Bool bMenuBar, Menu* pMenu );
    virtual void            DestroyMenu( SalMenu* pMenu);
    virtual SalMenuItem*    CreateMenuItem( const SalItemParams* pItemData );
    virtual void            DestroyMenuItem( SalMenuItem* pItem );

    // may return NULL to disable session management
    virtual SalSession*		CreateSalSession() = 0;

    // methods for XDisplayConnection

    void				SetEventCallback( rtl::Reference< vcl::DisplayConnectionDispatch > const & pInstance )
    { m_pEventInst = pInstance; }
    bool				CallEventCallback( void* pEvent, int nBytes )
    { return m_pEventInst.is() && m_pEventInst->dispatchEvent( pEvent, nBytes ); }
    bool				CallErrorCallback( void* pEvent, int nBytes )
    { return m_pEventInst.is() && m_pEventInst->dispatchErrorEvent( pEvent, nBytes ); }

    enum ConnectionIdentifierType { AsciiCString, Blob };
    virtual void*				GetConnectionIdentifier( ConnectionIdentifierType& rReturnedType, int& rReturnedBytes ) = 0;

    // this is a vehicle for PrintFontManager to bridge the gap between vcl and libvclplug_*
    // this is only necessary because PrintFontManager is an exported vcl API and therefore
    // needs to be in libvcl while libvclplug_* do not contain exported C++ API
    virtual void        FillFontPathList( std::list< rtl::OString >& o_rFontPaths );

    // dtrans implementation
    virtual com::sun::star::uno::Reference< com::sun::star::uno::XInterface >
        CreateClipboard( const com::sun::star::uno::Sequence< com::sun::star::uno::Any >& i_rArguments );
    virtual com::sun::star::uno::Reference< com::sun::star::uno::XInterface > CreateDragSource();
    virtual com::sun::star::uno::Reference< com::sun::star::uno::XInterface > CreateDropTarget();
    virtual void        AddToRecentDocumentList(const rtl::OUString& rFileUrl, const rtl::OUString& rMimeType) = 0;
};

// called from SVMain
SalInstance* CreateSalInstance();
void DestroySalInstance( SalInstance* pInst );

// -------------------------
// - SalInstance-Functions -
// -------------------------

void SalAbort( const XubString& rErrorText );

VCL_PLUGIN_PUBLIC const ::rtl::OUString& SalGetDesktopEnvironment();

// -----------
// - SalData -
// -----------

void InitSalData();                         // called from Application-Ctor
void DeInitSalData();                       // called from Application-Dtor

void InitSalMain();
void DeInitSalMain();

// ----------
// - SVMain -
// ----------

// Callbacks (indepen in \sv\source\app\svmain.cxx)
VCL_DLLPUBLIC sal_Bool SVMain();

#endif // _SV_SALINST_HXX
