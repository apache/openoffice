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



#include <bf_sfx2/app.hxx>
#include <bf_svx/editstat.hxx>
#include <bf_svx/unolingu.hxx>
#include <bf_svx/eeitem.hxx>
#include <bf_svx/forbiddencharacterstable.hxx>

#include "sdoutl.hxx"
#include "drawdoc.hxx"
#include "sdmod.hxx"

class SfxStyleSheetPool;

namespace binfilter {

SdOutliner::SdOutliner( SdDrawDocument* pDoc, USHORT nMode )
: SdrOutliner( &pDoc->GetItemPool(), nMode )
{
	SetStyleSheetPool((SfxStyleSheetPool*) pDoc->GetStyleSheetPool());
	SetEditTextObjectPool( &pDoc->GetItemPool() );
	SetCalcFieldValueHdl(LINK(SD_MOD(), SdModule, CalcFieldValueHdl));
	SetForbiddenCharsTable( pDoc->GetForbiddenCharsTable() );

	ULONG nCntrl = GetControlWord();
	nCntrl |= EE_CNTRL_ALLOWBIGOBJS;
	nCntrl |= EE_CNTRL_URLSFXEXECUTE;
	nCntrl |= EE_CNTRL_MARKFIELDS;
	nCntrl |= EE_CNTRL_AUTOCORRECT;
	nCntrl |= EE_CNTRL_NOREDLINES;
	nCntrl &= ~EE_CNTRL_ONLINESPELLING;

	SetControlWord(nCntrl);

	SetDefaultLanguage( Application::GetSettings().GetLanguage() );
}

SdOutliner::~SdOutliner()
{
}

}

