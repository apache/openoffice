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



#ifndef SC_CELLSH_HXX
#define SC_CELLSH_HXX

#ifndef _SFX_SHELL_HXX //autogen
#include <bf_sfx2/shell.hxx>
#endif
#include "shellids.hxx"
#ifndef _SFXMODULE_HXX //autogen
#include <bf_sfx2/module.hxx>
#endif

#ifndef _SVDMARK_HXX //autogen
#include <bf_svx/svdmark.hxx>
#endif

#ifndef _LINK_HXX 
#include <tools/link.hxx>
#endif

#ifndef SC_FORMATSH_HXX
#include "formatsh.hxx"
#endif
class TransferableDataHelper;
class TransferableClipboardListener;
namespace binfilter {

class SvxClipboardFmtItem;


class ScCellShell: public ScFormatShell
{
private:
	TransferableClipboardListener* pClipEvtLstnr;
	BOOL		bPastePossible;

	void		GetPossibleClipboardFormats( SvxClipboardFmtItem& rFormats );

	DECL_LINK( ClipboardChanged, TransferableDataHelper* );

public:

	TYPEINFO();
	SFX_DECL_INTERFACE(SCID_CELL_SHELL);

				ScCellShell(ScViewData* pData);
	virtual		~ScCellShell();

	void		Execute(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 void		Execute(SfxRequest &);
	void		GetState(SfxItemSet &);

	void		ExecuteEdit( SfxRequest& rReq ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 void		ExecuteEdit( SfxRequest& rReq );
	void		ExecuteTrans( SfxRequest& rReq ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 void		ExecuteTrans( SfxRequest& rReq );

	void		GetBlockState( SfxItemSet& rSet );
	void		GetCellState( SfxItemSet& rSet );

	void		ExecuteDB( SfxRequest& rReq ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 void		ExecuteDB( SfxRequest& rReq );
	void		GetDBState( SfxItemSet& rSet );

	void		ExecImageMap(SfxRequest& rReq);		// ImageMap
	void		GetImageMapState(SfxItemSet& rSet);

	void		GetClipState( SfxItemSet& rSet );
	void		GetHLinkState( SfxItemSet& rSet ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 void		GetHLinkState( SfxItemSet& rSet );

	void		ExecuteCursor( SfxRequest& rReq ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 void		ExecuteCursor( SfxRequest& rReq );
	void		ExecuteCursorSel( SfxRequest& rReq ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 void		ExecuteCursorSel( SfxRequest& rReq );
	void		ExecutePage( SfxRequest& rReq ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 void		ExecutePage( SfxRequest& rReq );
	void		ExecutePageSel( SfxRequest& rReq ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 void		ExecutePageSel( SfxRequest& rReq );
	void		ExecuteMove( SfxRequest& rReq ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 void		ExecuteMove( SfxRequest& rReq );
	void		GetStateCursor( SfxItemSet& rSet ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 void		GetStateCursor( SfxItemSet& rSet );

};

} //namespace binfilter
#endif
