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



#include <bf_so3/ipenv.hxx>
#include <bf_svtools/eitem.hxx>

#define _SFX_INTERNO_CXX


#ifndef _SVTOOLS_IMGDEF_HXX
#include <bf_svtools/imgdef.hxx>
#endif

#include "request.hxx"
#include "docfac.hxx"
#include "interno.hxx"
#include "fltfnc.hxx"

namespace binfilter {

//-------------------------------------------------------------------------

#define SVIPWINDOW(p) ((SvInPlaceWindow&)p->GetWindow())

//-------------------------------------------------------------------------
/*N*/ SV_IMPL_FACTORY(SfxInPlaceObjectFactory){}};
/*N*/ TYPEINIT1(SfxInPlaceObjectFactory,SvFactory);
/*N*/ SO2_IMPL_CLASS1(SfxInPlaceObject,SfxInPlaceObjectFactory,SvInPlaceObject,
/*N*/         SvGlobalName(0x3C8A87D0L,0x9B53,0x11d3,0x9E,0xCE,0x00,0x50,0x04,0xD7,0x6C,0x4E))//STRIP002 SvGlobalName(0x3C8A87D0L,0x9B53,0x11d3,0x9E,0xCE,0x00,0x50,0x04,0xD7,0x6C,0x4D))
/*?*/ void SfxInPlaceObject::TestMemberObjRef(BOOL){}
/*?*/ void SfxInPlaceObject::TestMemberInvariant(BOOL){}
/*?*/ ::IUnknown* SfxInPlaceObject::GetMemberInterface( const SvGlobalName & ){ return 0L; }

/*N*/ SfxInPlaceObject::SfxInPlaceObject()
/*N*/ :	pObjShell(0)
/*N*/ {
/*N*/ 	bTriggerLinkTimer = TRUE;
/*N*/ 	bDisableViewScaling = FALSE;
/*N*/ 	pNote = 0;
/*N*/ 
/*N*/     SvVerbList* pVerbs = new SvVerbList;
/*N*/     SetVerbList( pVerbs, TRUE );
/*N*/ }

/*N*/ void SfxInPlaceObject::SetShell( SfxObjectShell *pObjSh )
/*N*/ {
/*N*/     pObjShell = pObjSh;
/*N*/ }

//--------------------------------------------------------------------

/*N*/ SfxInPlaceObject::~SfxInPlaceObject()
/*N*/ {
/*N*/ }

//--------------------------------------------------------------------

/*N*/ void SfxInPlaceObject::FillClass( SvGlobalName * pClassName,
/*N*/ 								  ULONG * pClipFormat,
/*N*/ 								  String * pAppName,
/*N*/ 								  String * pLongUserName,
/*N*/ 								  String * pUserName,
/*N*/ 								  long nVersion
/*N*/ ) const
/*N*/ {
/*N*/ 	SvPersist::FillClass( pClassName, pClipFormat, pAppName,
/*N*/ 							pLongUserName, pUserName, nVersion );
/*N*/ 	DBG_ASSERT( pObjShell,
/*N*/ 				"SfxInPlaceObject::FillClass(): you must call SetShell() bevor" );
/*N*/ 	SfxObjectFactory &rFact = pObjShell->GetFactory();
/*N*/ 
/*N*/ 	for( USHORT n = 0; n < rFact.GetFilterCount(); n++ )
/*N*/ 	{
/*N*/ 		const SfxFilter * pFilter = rFact.GetFilter( n );
/*N*/ 
/*N*/ 		if ( pFilter->IsOwnFormat() &&
/*N*/ 			 pFilter->GetVersion() == (ULONG)nVersion )
/*N*/ 		{
/*N*/ 			*pClassName     = *GetSvFactory();
/*N*/ 			*pClipFormat    = pFilter->GetFormat();
/*N*/             *pLongUserName  = pFilter->GetFilterName();
/*N*/             *pUserName      = pFilter->GetFilterName();
/*N*/ 			break;
/*N*/ 		}
/*N*/ 	}
/*N*/ 	*pAppName = String::CreateFromAscii("Office");
/*N*/ }

//--------------------------------------------------------------------

/*N*/ void SfxInPlaceObject::SetModified( BOOL bSet )
/*N*/ {
/*N*/ 	if ( !bSet || !pObjShell->IsReadOnlyMedium() )
/*N*/ 	{
/*N*/ 		SvInPlaceObject::SetModified( bSet );
/*N*/         if( bSet && bTriggerLinkTimer && IsEnableSetModified() )
/*N*/ 		{
/*N*/ 			ViewChanged( ASPECT_CONTENT );
/*N*/ 		}
/*N*/ 	}
/*N*/ }

//--------------------------------------------------------------------

/*N*/ void SfxInPlaceObject::UpdateLinks()
/*N*/ {
/*N*/ 	SendViewChanged();
/*N*/ }

//--------------------------------------------------------------------

/*N*/ BOOL SfxInPlaceObject::InitNew( SvStorage * pStor)
/*N*/ {
/*N*/ 	if( SvInPlaceObject::InitNew( pStor ) )
/*N*/ 	{
/*N*/ 		return TRUE;
/*N*/ 	}
/*N*/ 	return FALSE;
/*N*/ }

//--------------------------------------------------------------------

/*N*/ BOOL SfxInPlaceObject::Load( SvStorage * pStor )
/*N*/ {
/*N*/ 	if( SvInPlaceObject::Load( pStor ) )
/*N*/ 	{
/*N*/ 		// Spaeter mal Childs on demand
/*N*/ //        if( LoadChilds() )
/*N*/ 			return TRUE;
/*N*/ 	}
/*N*/ 	return FALSE;
/*N*/ }

//--------------------------------------------------------------------

/*N*/ BOOL SfxInPlaceObject::Save()
/*N*/ {
/*N*/ 	if( !SaveChilds() )
/*N*/ 		return FALSE;
/*N*/ 	if( SvInPlaceObject::Save() )
/*N*/         return pObjShell->SfxObjectShell::Save();
/*N*/ 	return FALSE;
/*N*/ }

//--------------------------------------------------------------------

/*N*/ BOOL SfxInPlaceObject::SaveAs( SvStorage * pNewStor )
/*N*/ {
/*N*/ 	if( !SaveAsChilds( pNewStor ) )
/*N*/ 		return FALSE;
/*N*/     if( SvInPlaceObject::SaveAs( pNewStor ) )
/*N*/         return pObjShell->SfxObjectShell::SaveAs(pNewStor);
/*N*/ 	return FALSE;
/*N*/ }

//--------------------------------------------------------------------

/*N*/ void SfxInPlaceObject::HandsOff()
/*N*/ {
/*N*/ 	SvInPlaceObject::HandsOff();
/*N*/ }

//--------------------------------------------------------------------

/*N*/ BOOL SfxInPlaceObject::SaveCompleted( SvStorage * pStor )
/*N*/ {
/*N*/ 	if( !SaveCompletedChilds( pStor ) )
/*N*/ 		return FALSE;
/*N*/ 	if( SvInPlaceObject::SaveCompleted( pStor ) )
/*N*/ 	{
/*N*/ 		return TRUE;
/*N*/ 	}
/*N*/ 	return FALSE;
/*N*/ }

//--------------------------------------------------------------------

/*N*/ void SfxInPlaceObject::SetVisArea( const Rectangle & rRect )
/*N*/ {
/*N*/ 	if( GetVisArea() != rRect )
/*N*/ 	{
/*N*/ 		Size aSize (GetVisArea().GetSize());
/*N*/ 		// Groesse des Objects setzen
/*N*/ 		SvInPlaceObject::SetVisArea( rRect );
/*N*/ 
/*N*/ 		SetModified( TRUE );
/*N*/ 
/*N*/         if ( GetIPEnv() && GetIPEnv()->GetEditWin() )
/*?*/             ViewChanged( ASPECT_CONTENT );
/*N*/ 
/*N*/ 		// OutPlace die Gr"o\se des MDI-Fensters anpassen
/*N*/ 		// Unbedingt den Gr"o\senvergleich machen, spart nicht nur Zeit, sondern
/*N*/ 		// vermeidet auch Rundungsfehler !
/*N*/ 	}
/*N*/ }

//--------------------------------------------------------------------

/*N*/ ErrCode SfxInPlaceObject::Verb( long nVerb, SvEmbeddedClient *pCaller,
/*N*/ 								Window *pWindow, const Rectangle* pRect)
/*N*/ {
         return FALSE;
/*N*/ }

//--------------------------------------------------------------------

/*?*/ BOOL SfxInPlaceObject::Verb( long nVerb, const Rectangle* pRect )
/*?*/ {
/*?*/ 	return (BOOL)DoEmbed( TRUE );
/*?*/ }

//--------------------------------------------------------------------

/*?*/ void SfxInPlaceObject::Open( BOOL bOpen )
/*?*/ {
/*?*/ 	SvInPlaceObject::Open( bOpen );
/*?*/ }

//-------------------------------------------------------------------------

/*N*/ void SfxInPlaceObject::InPlaceActivate( BOOL bActivate )
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }

//--------------------------------------------------------------------

/*N*/ void SfxInPlaceObject::Embed( BOOL bEmbed )
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }

//--------------------------------------------------------------------

/*N*/ void SfxInPlaceObject::DocumentNameChanged( const String & rDocName )
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001 
/*N*/ }

//--------------------------------------------------------------------

/*N*/ SfxForceLinkTimer_Impl::SfxForceLinkTimer_Impl( SfxObjectShell *pObj )
/*N*/ {
/*N*/ 	pInObj = pObj->GetInPlaceObject();
/*N*/ 	if( pInObj )
/*N*/ 	{
/*N*/ 		pInObj->SetTriggerLinkTimer( FALSE );
/*N*/ 		pInObj->UpdateLinks();
/*N*/ 	}
/*N*/ }
}
