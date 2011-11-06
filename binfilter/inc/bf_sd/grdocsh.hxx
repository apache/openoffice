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



#ifndef _SD_GRDOCSH_HXX
#define _SD_GRDOCSH_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFX_OBJFAC_HXX //autogen
#include <bf_sfx2/docfac.hxx>
#endif
#ifndef _SFX_INTERNO_HXX //autogen
#include <bf_sfx2/interno.hxx>
#endif
#ifndef _SFX_OBJSH_HXX //autogen
#include <bf_sfx2/objsh.hxx>
#endif

#ifndef _SD_DOCSHELL_HXX
#include "docshell.hxx"
#endif
#ifndef _SD_GLOB_HXX
#include "glob.hxx"
#endif
#ifndef _SDMOD_HXX
#include "sdmod.hxx"
#endif
#ifndef _PRESENTATION_HXX
#include "pres.hxx"
#endif
namespace binfilter {


/*************************************************************************
|*
|* Document-Shell fuer Draw-Dokumente
|*
\************************************************************************/

class SdGraphicDocShell : public SdDrawDocShell
{
 public:
	TYPEINFO();

	SFX_DECL_OBJECTFACTORY_DLL(SdGraphicDocShell, SD_MOD());

	SdGraphicDocShell(SfxObjectCreateMode eMode = SFX_CREATE_MODE_EMBEDDED,
				   BOOL bSdDataObj=FALSE, DocumentType=DOCUMENT_TYPE_DRAW);
	SdGraphicDocShell(SdDrawDocument* pDoc, SfxObjectCreateMode eMode = SFX_CREATE_MODE_EMBEDDED,
				   BOOL bSdDataObj=FALSE, DocumentType=DOCUMENT_TYPE_DRAW);
	~SdGraphicDocShell();
};

#ifndef SV_DECL_SDGRAPHICDOCSHELL_DEFINED
#define SV_DECL_SDGRAPHICDOCSHELL_DEFINED
SV_DECL_REF(SdGraphicDocShell)
#endif

SV_IMPL_REF (SdGraphicDocShell)


} //namespace binfilter
#endif          // _SD_GRDOCSH_HXX

