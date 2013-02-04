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



#ifndef _SCHMOD_HXX
#define _SCHMOD_HXX

#ifndef _SFXLSTNER_HXX
#include <bf_svtools/lstner.hxx>
#endif

#include "schdll0.hxx"        // fuer SchModuleDummy
#include "schdll.hxx"
#include "objfac.hxx"

#include "glob.hxx"
#include "schopt.hxx"
class SvFactory;
namespace binfilter {

class SvxErrorHandler;
class E3dObjFactory;
class SchTransferable;

/*************************************************************************
|*
|* This subclass of <SfxModule> (which is a subclass of <SfxShell>) is
|* linked to the DLL. One instance of this class exists while the DLL is
|* loaded.
|*
|* SdModule is like to be compared with the <SfxApplication>-subclass.
|*
|* Remember: Don`t export this class! It uses DLL-internal symbols.
|*
\************************************************************************/

class SchModule : public SchModuleDummy, public SfxListener
{
protected:
	XOutdevItemPool*	pXOutDevPool;

	SchDragServer*		pDragData;
	SchDragServer*		pClipboardData;
	// new clipborad format
	SchTransferable*	mpTransferDragDrop;
	SchTransferable*	mpTransferClipboard;
	SchTransferable*	mpTransferSelectionClipbd;

	SchObjFactory*		pSchObjFactory;
	E3dObjFactory*		pE3dFactory;
	SchOptions*			pChartOptions;

public:
	TYPEINFO();

	SchModule(SvFactory* pObjFact);
	virtual ~SchModule();

	SchDragServer *GetDragData()							{ return pDragData; }
	void SetDragData(SchDragServer* pData)					{ pDragData = pData; }

	SchDragServer *GetClipboardData()						{ return pClipboardData; }
	void SetClipboardData(SchDragServer* pData)				{ pClipboardData = pData; }

	// new clipboard format
	SchTransferable* GetDragTransferable()					{ return mpTransferDragDrop; }
	void SetDragTransferable( SchTransferable* pTrans )		{ mpTransferDragDrop = pTrans; }

	SchTransferable* GetClipboardTransferable()				{ return mpTransferClipboard; }
	void SetClipboardTransferable( SchTransferable* pTrans) { mpTransferClipboard = pTrans; }

	SchTransferable* GetSelectionClipboardTransferable()	{ return mpTransferSelectionClipbd; }
	void SetSelectionClipboardTransferable( SchTransferable* pTrans) { mpTransferSelectionClipbd = pTrans; }

	SchOptions*          GetSchOptions();

	// virtual methods for options dialog (impl. see appopt.cxx)

	virtual void		Notify( SfxBroadcaster& rBC, const SfxHint& rHint );
};




#ifndef SCH_MOD1
#define SCH_MOD1() ( *(SchModule**) GetAppData(BF_SHL_SCH) )
#endif

} //namespace binfilter
#endif                                 // _SCHMOD_HXX
