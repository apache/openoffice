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




#define _SWLIB_CXX

#ifndef _SFX_FCONTNR_HXX //autogen
#include <bf_sfx2/fcontnr.hxx>
#endif
#ifndef _SFXDOCFILE_HXX //autogen
#include <bf_sfx2/docfile.hxx>
#endif

#include <swdll0.hxx> // wird vom makefile generiert
#ifndef _SWMODULE_HXX //autogen
#include <swmodule.hxx>
#endif
#ifndef _SWWDOCSH_HXX //autogen
#include <wdocsh.hxx>
#endif
#ifndef _SWGLOBDOCSH_HXX //autogen
#include <globdoc.hxx>
#endif
#ifndef _SHELLIO_HXX //autogen
#include <shellio.hxx>
#endif
#ifndef _SO_CLSIDS_HXX
#include <comphelper/classids.hxx>
#endif

#include <bf_svtools/moduleoptions.hxx>

#include <app.hrc>
#include <web.hrc>
#include <globdoc.hrc>

#include <iodetect.hxx>

namespace binfilter {

#define C2S(cChar) String::CreateFromAscii(RTL_CONSTASCII_STRINGPARAM(cChar))
//os: the Icon Id is unused since start of VCL
#define ICON_ID_INVALID 0

inline BOOL IsDocShellRegistered()
{
    return SvtModuleOptions().IsWriter();
}

USHORT AutoDetec( const String& FileName, USHORT & rVersion );

IO_DETECT_IMPL1
IO_DETECT_IMPL2
IO_DETECT_IMPL3
IO_DETECT_IMPL4

//-------------------------------------------------------------------------
extern "C" { static void SAL_CALL thisModule() {} }

SFX_IMPL_MODULE_LIB(Sw, C2S(DLL_NAME))

TYPEINIT1( SwModuleDummy, SfxModule );


//-------------------------------------------------------------------------
SFX_IMPL_OBJECTFACTORY_MOD(SwDocShell, 						\
		SFXOBJECTSHELL_STD_NORMAL|SFXOBJECTSHELL_HASMENU,   \
		swriter, 											\
		SvGlobalName(BF_SO3_SW_CLASSID),						\
		Sw,													\
		DLL_NAME)
{
	SfxObjectFactory& rFactory = (SfxObjectFactory&)Factory();
	rFactory.SetDocumentServiceName(C2S("com.sun.star.text.TextDocument"));
	rFactory.GetFilterContainer()->SetDetectFilter( &SwDLL::DetectFilter );
}

//-------------------------------------------------------------------------
SFX_IMPL_OBJECTFACTORY_MOD(SwWebDocShell, 								\
		SFXOBJECTSHELL_STD_NORMAL|SFXOBJECTSHELL_HASMENU, swriter/web, 	\
		SvGlobalName(BF_SO3_SWWEB_CLASSID),								\
		Sw,																\
		DLL_NAME)
{
	SfxObjectFactory& rFactory = (SfxObjectFactory&)Factory();
    rFactory.SetDocumentServiceName(C2S("com.sun.star.text.WebDocument"));
	rFactory.GetFilterContainer()->SetDetectFilter( &SwDLL::DetectFilter );
}

//-------------------------------------------------------------------------
SFX_IMPL_OBJECTFACTORY_MOD(SwGlobalDocShell, 										  \
	 	SFXOBJECTSHELL_STD_NORMAL|SFXOBJECTSHELL_HASMENU, swriter/GlobalDocument, \
		SvGlobalName(BF_SO3_SWGLOB_CLASSID),								\
		Sw,																			  \
		DLL_NAME)
{
	SfxObjectFactory& rFactory = (SfxObjectFactory&)Factory();
    rFactory.SetDocumentServiceName(C2S("com.sun.star.text.GlobalDocument"));
	rFactory.GetFilterContainer()->SetDetectFilter( &SwDLL::GlobDetectFilter );
}

//-------------------------------------------------------------------------

void 	SwDLL::LibInit()
{
	// in most cases you must initialize your document-factory here
    SvtModuleOptions aOpt;
    SfxObjectFactory* pWriterFactory = NULL;
    SfxObjectFactory* pGlobalFactory = NULL;
    if ( aOpt.IsWriter() )
    {
        SwDocShell::RegisterFactory(    SDT_SW_DOCFACTPRIO      );
        SwGlobalDocShell::RegisterFactory(SDT_SW_DOCFACTPRIO + 2);
        pWriterFactory = &SwDocShell::Factory();
        pGlobalFactory = &SwGlobalDocShell::Factory();
    }

    // WebWriter alway needed because it is used for the help viewer
	SwWebDocShell::RegisterFactory(	SDT_SW_DOCFACTPRIO + 1	);

	// create a dummy-module for Object-Factory-Pointer
    SW_MOD() = (SwModule*)new SwModuleDummy( 0, TRUE, &SwWebDocShell::Factory(), pWriterFactory, pGlobalFactory );
    SwModule* pMod = SW_MOD();
}

//-------------------------------------------------------------------------
void 	SwDLL::LibExit()
{
		// Innerhalb von FreeLibSw wird Exit() gerufen
	FreeLibSw();

	// destroy the dummy-module with Object-Factory-Pointer
	DELETEZ( *(SwModuleDummy**) GetAppData(BF_SHL_WRITER) );
}

//-------------------------------------------------------------------------

SfxModule* SwModuleDummy::Load()
{
	return (LoadLibSw() ? SW_MOD() : NULL);
}
}
