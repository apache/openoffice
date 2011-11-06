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



#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "cfgmgr.hxx"
#include "sfx.hrc"
#include "cfgimpl.hxx"
#include <tools/debug.hxx>
namespace binfilter {
// class SfxConfigItem ---------------------------------------------------

/*N*/ SfxConfigItem::SfxConfigItem( USHORT nType, SfxConfigManager* pCfgMgr )
/*N*/ 	: m_pCfgMgr( pCfgMgr )
/*N*/ 	, m_nType( nType )
/*N*/ 	, m_bDefault( TRUE )
/*N*/     , m_bModified( FALSE )
/*N*/     , m_bInitialized( FALSE )
/*N*/ {
/*N*/     if ( pCfgMgr )
/*N*/         pCfgMgr->AddConfigItem( *this );
/*N*/ }

/*N*/ SfxConfigItem::~SfxConfigItem()
/*N*/ {
/*N*/     if ( m_pCfgMgr )
/*N*/         m_pCfgMgr->RemoveConfigItem( *this );
/*N*/ }

// ----------------------------------------------------------------------
/*N*/ BOOL SfxConfigItem::Initialize()
/*N*/ {
/*N*/ 	BOOL bRet = TRUE;
/*N*/ 	if ( !m_pCfgMgr )
/*N*/ 		UseDefault();
/*N*/ 	else
/*N*/         bRet = m_pCfgMgr->LoadConfigItem( *this );
/*N*/     m_bModified = FALSE;
/*N*/     m_bInitialized = TRUE;
/*N*/ 	return bRet;
/*N*/ }

// ----------------------------------------------------------------------
/*N*/ BOOL SfxConfigItem::StoreConfig()
/*N*/ {
/*N*/ 	BOOL bRet = FALSE;
/*N*/     if ( m_pCfgMgr && m_bModified )
/*?*/         bRet = m_pCfgMgr->StoreConfigItem( *this );
/*N*/ 	m_bModified = FALSE;
/*N*/ 	return bRet;
/*N*/ }

// ------------------------------------------------------------------------
/*N*/ void SfxConfigItem::SetDefault( BOOL bOn )
/*N*/ {
/*N*/     if ( m_bInitialized && (!bOn || !m_bDefault) )
/*?*/ 		SetModified( TRUE );
/*N*/ 	m_bDefault = bOn;
/*N*/ }

//-------------------------------------------------------------------------
/*N*/ void SfxConfigItem::SetModified( BOOL bMod )
/*N*/ {
/*N*/     if ( m_bInitialized )
/*N*/     {
/*N*/         m_bModified = bMod;
/*N*/         if ( bMod && m_pCfgMgr )
/*?*/             m_pCfgMgr->SetModified( bMod );
/*N*/     }
/*N*/ }

/*?*/ BOOL SfxConfigItem::ReInitialize()
/*?*/ {DBG_BF_ASSERT(0, "STRIP"); return FALSE; //STRIP001 
/*?*/ }

/*?*/ String SfxConfigItem::GetStreamName( USHORT nType )
/*?*/ {
/*?*/     return String();
/*?*/ }

/*N*/ void SfxConfigItem::Connect( SfxConfigManager* pMgr )
/*N*/ {
/*N*/ 	m_pCfgMgr = pMgr; pMgr->AddConfigItem(*this);
/*N*/ }
}
