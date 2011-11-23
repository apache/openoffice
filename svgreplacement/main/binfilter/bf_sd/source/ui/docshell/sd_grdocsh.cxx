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



#ifndef _SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif
#define ITEMID_SEARCH                   SID_SEARCH_ITEM
#ifndef _SFXAPP_HXX
#include <bf_sfx2/app.hxx>
#endif


#ifndef _SO_CLSIDS_HXX //autogen
#include <comphelper/classids.hxx>
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

#include "strings.hrc"
#include "glob.hrc"

#include "bf_sd/grdocsh.hxx"
#include "sdresid.hxx"

namespace binfilter
{

TYPEINIT1(SdGraphicDocShell, SdDrawDocShell);

SFX_IMPL_OBJECTFACTORY_LOD(SdGraphicDocShell, sdraw, SvGlobalName(BF_SO3_SDRAW_CLASSID), Sd)

SdGraphicDocShell::SdGraphicDocShell(SfxObjectCreateMode eMode, BOOL bDataObject, DocumentType eDocType)
: SdDrawDocShell(eMode, bDataObject, eDocType)
{
	SetStyleFamily( 2 );
}

SdGraphicDocShell::~SdGraphicDocShell()
{
}

}
