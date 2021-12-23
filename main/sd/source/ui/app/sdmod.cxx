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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sd.hxx"
#include <unotools/pathoptions.hxx>
#include <svl/languageoptions.hxx>
#ifndef _UNOTOOLS_UCBSTREAMHELPER_HXX
#include <unotools/ucbstreamhelper.hxx>
#endif
#include <tools/urlobj.hxx>
#include <vcl/virdev.hxx>
#include <sfx2/app.hxx>
#include <vcl/status.hxx>
#include <svl/intitem.hxx>
#include <sfx2/msg.hxx>
#include <sfx2/objface.hxx>
#include <sfx2/printer.hxx>
#include <svx/pszctrl.hxx>
#include <svx/zoomctrl.hxx>
#include <svx/modctrl.hxx>
#include <svl/zforlist.hxx>
#include <comphelper/processfactory.hxx>
#include <svtools/ehdl.hxx>

#include <svx/svxids.hrc>
#include <svl/srchitem.hxx>
#include <svx/svxerr.hxx>

#include <svx/xmlsecctrl.hxx>


#define _SD_DLL // fuer SD_MOD()
#include "sderror.hxx"
#include "sdmod.hxx"
#include "sddll.hxx"
#include "sdresid.hxx"
#include "optsitem.hxx"
#include "DrawDocShell.hxx"
#include "drawdoc.hxx"
#include "app.hrc"
#include "glob.hrc"
#include "strings.hrc"
#include "res_bmp.hrc"
#include "cfgids.hxx"
#include "tools/SdGlobalResourceContainer.hxx"

TYPEINIT1( SdModule, SfxModule );

#define SdModule
#include "sdslots.hxx"


SFX_IMPL_INTERFACE(SdModule, SfxModule, SdResId(STR_APPLICATIONOBJECTBAR))
{
	SFX_STATUSBAR_REGISTRATION(SdResId(RID_DRAW_STATUSBAR));
}

/*************************************************************************
|* Ctor
\************************************************************************/

SdModule::SdModule(SfxObjectFactory* pFact1, SfxObjectFactory* pFact2 )
:	SfxModule( SfxApplication::CreateResManager("sd"), sal_False,
				  pFact1, pFact2, NULL ),
	pTransferClip(NULL),
	pTransferDrag(NULL),
	pTransferSelection(NULL),
	pImpressOptions(NULL),
	pDrawOptions(NULL),
	pSearchItem(NULL),
	pNumberFormatter( NULL ),
	bWaterCan(sal_False),
	mpResourceContainer(new ::sd::SdGlobalResourceContainer())
{
	SetName( UniString::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM( "StarDraw" ) ) );	// Nicht uebersetzen!
	pSearchItem = new SvxSearchItem(SID_SEARCH_ITEM);
	pSearchItem->SetAppFlag(SVX_SEARCHAPP_DRAW);
	StartListening( *SFX_APP() );
	SvxErrorHandler::Get();
	mpErrorHdl = new SfxErrorHandler( RID_SD_ERRHDL,
										 ERRCODE_AREA_SD,
										 ERRCODE_AREA_SD_END,
										 GetResMgr() );

	// Create a new ref device and (by calling SetReferenceDevice())
	// set its resolution to 600 DPI. This leads to a visually better
	// formatting of text in small sizes (6 point and below.)
	VirtualDevice* pDevice = new VirtualDevice;
	mpVirtualRefDevice = pDevice;
	pDevice->SetMapMode( MAP_100TH_MM );
	pDevice->SetReferenceDevice ( VirtualDevice::REFDEV_MODE06 );
}



/*************************************************************************
|* Dtor
\************************************************************************/

SdModule::~SdModule()
{
	delete pSearchItem;

	if( pNumberFormatter )
	delete pNumberFormatter;

	::sd::DrawDocShell* pDocShell = PTR_CAST(::sd::DrawDocShell, SfxObjectShell::Current());
	if( pDocShell )
	{
		::sd::ViewShell* pViewShell = pDocShell->GetViewShell();
		if (pViewShell)
		{
			// Removing our event listener
			Application::RemoveEventListener( LINK( this, SdModule, EventListenerHdl ) );
		}
	}

	mpResourceContainer.reset();

	// Mark the module in the global AppData structure as deleted.
	SdModule** ppShellPointer = (SdModule**)GetAppData(SHL_DRAW);
	if (ppShellPointer != NULL)
		(*ppShellPointer) = NULL;

	delete mpErrorHdl;
	delete static_cast< VirtualDevice* >( mpVirtualRefDevice );
}


/*************************************************************************
|* get notifications
\************************************************************************/

void SdModule::Notify( SfxBroadcaster&, const SfxHint& rHint )
{
	if( rHint.ISA( SfxSimpleHint ) &&
		( (SfxSimpleHint&) rHint ).GetId() == SFX_HINT_DEINITIALIZING )
	{
		delete pImpressOptions, pImpressOptions = NULL;
		delete pDrawOptions, pDrawOptions = NULL;
	}
}

/*************************************************************************
|* return options
\************************************************************************/

SdOptions* SdModule::GetSdOptions(DocumentType eDocType)
{
	SdOptions* pOptions = NULL;

	if (eDocType == DOCUMENT_TYPE_DRAW)
	{
		if (!pDrawOptions)
			pDrawOptions = new SdOptions( SDCFG_DRAW );

		pOptions = pDrawOptions;
	}
	else if (eDocType == DOCUMENT_TYPE_IMPRESS)
	{
		if (!pImpressOptions)
			pImpressOptions = new SdOptions( SDCFG_IMPRESS );

		pOptions = pImpressOptions;
	}
	if( pOptions )
	{
		sal_uInt16 nMetric = pOptions->GetMetric();

		::sd::DrawDocShell* pDocSh = PTR_CAST(::sd::DrawDocShell, SfxObjectShell::Current() );
		SdDrawDocument* pDoc = NULL;
		if (pDocSh)
			pDoc = pDocSh->GetDoc();

		if( nMetric != 0xffff && pDoc && eDocType == pDoc->GetDocumentType() )
			PutItem( SfxUInt16Item( SID_ATTR_METRIC, nMetric ) );
	}

	return(pOptions);
}

/*************************************************************************
|* Optionen-Stream fuer interne Options oeffnen und zurueckgeben;
|* falls der Stream zum Lesen geoeffnet wird, aber noch nicht
|* angelegt wurde, wird ein 'leeres' RefObject zurueckgegeben
\************************************************************************/

SvStorageStreamRef SdModule::GetOptionStream( const String& rOptionName,
											  SdOptionStreamMode eMode )
{
	::sd::DrawDocShell*		pDocSh = PTR_CAST(::sd::DrawDocShell, SfxObjectShell::Current() );
	SvStorageStreamRef	xStm;

	if( pDocSh )
	{
		DocumentType	eType = pDocSh->GetDoc()->GetDocumentType();
		String			aStmName;

		if( !xOptionStorage.Is() )
		{
			INetURLObject aURL( SvtPathOptions().GetUserConfigPath() );

			aURL.Append( UniString::CreateFromAscii( RTL_CONSTASCII_STRINGPARAM( "drawing.cfg" ) ) );

			SvStream* pStm = ::utl::UcbStreamHelper::CreateStream( aURL.GetMainURL( INetURLObject::NO_DECODE ), STREAM_READWRITE );

			if( pStm )
				xOptionStorage = new SvStorage( pStm, sal_True );
		}

		if( DOCUMENT_TYPE_DRAW == eType )
			aStmName.AssignAscii( RTL_CONSTASCII_STRINGPARAM( "Draw_" ) );
		else
			aStmName.AssignAscii( RTL_CONSTASCII_STRINGPARAM( "Impress_" ) );

		aStmName += rOptionName;

		if( SD_OPTION_STORE == eMode || xOptionStorage->IsContained( aStmName ) )
			xStm = xOptionStorage->OpenSotStream( aStmName );
	}

	return xStm;
}

/*************************************************************************
|*
\************************************************************************/

SvNumberFormatter* SdModule::GetNumberFormatter()
{
	if( !pNumberFormatter )
		pNumberFormatter = new SvNumberFormatter( ::comphelper::getProcessServiceFactory(), LANGUAGE_SYSTEM );

	return pNumberFormatter;
}

/*************************************************************************
|*
\************************************************************************/

OutputDevice* SdModule::GetVirtualRefDevice (void)
{
	return mpVirtualRefDevice;
}

/** This method is deprecated and only an alias to
	<member>GetVirtualRefDevice()</member>. The given argument is ignored.
*/
OutputDevice* SdModule::GetRefDevice (::sd::DrawDocShell& )
{
	return GetVirtualRefDevice();
}
