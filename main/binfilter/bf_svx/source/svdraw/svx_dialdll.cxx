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



// include ---------------------------------------------------------------

#include <bf_svtools/solar.hrc>
#include "dialdll.hxx"
#include "dialmgr.hxx"
#define ITEMID_BRUSH 1
#include "brshitem.hxx"


#ifndef _SV_SVAPP_HXX //autogen
#include <vcl/svapp.hxx>
#endif



#ifndef _FILTER_HXX //autogen
#include <bf_svtools/filter.hxx>
#endif

#ifndef _WALLITEM_HXX
#include <bf_svtools/wallitem.hxx>
#endif
namespace binfilter {

// struct DialogsResMgr --------------------------------------------------
/*N*/ DialogsResMgr::DialogsResMgr()
/*N*/ :	pResMgr(0),
/*N*/ 	pGrapicFilter(0)
/*N*/ {
/*N*/ #ifndef SVX_LIGHT
/*N*/ 	ByteString aName( "bf_svx" );		//STRIP005
/*N*/ #else
/*N*/ 	ByteString aName( "bf_svl" );		//STRIP005
/*N*/ #endif
/*N*/     pResMgr = ResMgr::CreateResMgr( aName.GetBuffer(), Application::GetSettings().GetUILocale() );
/*N*/ }

// -----------------------------------------------------------------------
/*N*/ DialogsResMgr::~DialogsResMgr()
/*N*/ {
/*N*/ 	delete pGrapicFilter;
/*N*/ 	delete pResMgr;
/*N*/ }

// class SvxDialogDll ----------------------------------------------------
/*N*/ SvxDialogDll::SvxDialogDll()
/*N*/ {
/*N*/ 	DBG_ASSERT( !(*GetAppData(BF_SHL_SVX)), "Ctor, but pointer not null" );
/*N*/ 
/*N*/ 	(*(DialogsResMgr**)GetAppData(BF_SHL_SVX)) = new DialogsResMgr;
/*N*/ 	SvxBrushItem::InitSfxLink();  //  OV,30.06.1998
/*N*/ }

// -----------------------------------------------------------------------
/*N*/ SvxDialogDll::~SvxDialogDll()
/*N*/ {
/*N*/ 	DBG_ASSERT( (*GetAppData(BF_SHL_SVX)), "Dtor, pointer == null" );
/*N*/ 
/*N*/ 	delete (*(DialogsResMgr**)GetAppData(BF_SHL_SVX));
/*N*/ 	(*(DialogsResMgr**)GetAppData(BF_SHL_SVX)) = 0;
/*N*/ 	delete SfxBrushItemLink::Get();
/*N*/ 	(*(SfxBrushItemLink**)GetAppData(SHL_BRUSHITEM)) = 0;
/*N*/ }

// -----------------------------------------------------------------------


}
