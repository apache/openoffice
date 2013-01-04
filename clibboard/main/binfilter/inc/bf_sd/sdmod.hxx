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



#ifndef _SDMOD_HXX
#define _SDMOD_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SDDLL_HXX
#define _SD_DLL             // fuer SD_MOD()
#include "sddll.hxx"        // fuer SdModuleDummy
#endif
#ifndef _SD_GLOB_HXX
#include "glob.hxx"
#endif
#ifndef _PRESENTATION_HXX
#include "pres.hxx"
#endif
#ifndef _SVSTOR_HXX
#include <bf_so3/svstor.hxx>
#endif
#ifndef _SFXLSTNER_HXX
#include <bf_svtools/lstner.hxx>
#endif
#ifndef _COM_SUN_STAR_TEXT_WRITINGMODE_HPP_ 
#include <com/sun/star/text/WritingMode.hpp>
#endif
class SvFactory;
class OutputDevice;

namespace binfilter {

class SfxErrorHandler;
class SvNumberFormatter;
class SdOptions;
class BasicIDE;
class SdAppLinkHdl; //STRIP008 ;
class SvxErrorHandler;
class EditFieldInfo;
class SdDrawDocShell;
class SdView;
class SdPage;
class SdDrawDocument;
// ----------------------
// - SdOptionStreamMode -
// ----------------------

enum SdOptionStreamMode
{
	SD_OPTION_LOAD = 0,
	SD_OPTION_STORE = 1
};

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

class SdModule : public SdModuleDummy, public SfxListener
{
protected:

	SdOptions*				pImpressOptions;
	SdOptions*				pDrawOptions;
	SvStorageRef			xOptionStorage;
	BOOL					bAutoSave;
	SfxErrorHandler*		mpErrorHdl;
    OutputDevice*           mpVirtualRefDevice;

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

public:

							TYPEINFO();
							DECL_LINK( CalcFieldValueHdl, EditFieldInfo* );

							SdModule(SvFactory* pDrawObjFact, SvFactory* pGraphicObjFact);
	virtual					~SdModule();

	virtual SfxModule*		Load();
	virtual void			Free();


	SdOptions*				GetSdOptions(DocumentType eDocType);

    OutputDevice* GetVirtualRefDevice (void);

	OutputDevice* GetRefDevice (SdDrawDocShell& rDocShell);
    ::com::sun::star::text::WritingMode GetDefaultWritingMode() const;
};




#ifndef SD_MOD
#define SD_MOD() ( *(SdModule**) GetAppData(BF_SHL_DRAW) )
#endif

} //namespace binfilter
#endif                                 // _SDMOD_HXX

