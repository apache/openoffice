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



#ifdef PCH
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

#include <bf_svx/eeitem.hxx>
#define ITEMID_FIELD EE_FEATURE_FIELD

#include "scitems.hxx"		// fuer tbxctrls etc.
#include "scmod.hxx"
#include "scresid.hxx"
#include "bf_sc.hrc"
#include "cfgids.hxx"

//!	die Registrierung wird wegen CLOOKs in ein eigenes File wandern muessen...

// Interface-Registrierung
#include "docsh.hxx"
#include "docpool.hxx"
#include "appoptio.hxx"

#ifndef _SVX_ITEMDATA_HXX
#include <bf_svx/itemdata.hxx>
#endif


#ifndef _SVTOOLS_IMGDEF_HXX
#include <bf_svtools/imgdef.hxx>
#endif

#include <bf_svx/svdfield.hxx>		//	SdrRegisterFieldClasses
#include <rtl/logfile.hxx>

namespace binfilter {

//------------------------------------------------------------------

/*N*/ ScResId::ScResId( USHORT nId ) :
/*N*/ 	ResId( nId, *SC_MOD()->GetResMgr() )
/*N*/ {
/*N*/ }

//------------------------------------------------------------------

/*N*/ void ScDLL::Init()
/*N*/ {
/*N*/ 	RTL_LOGFILE_CONTEXT_AUTHOR ( aLog, "sc", "nn93723", "ScDLL::Init" );
/*N*/ 
/*N*/ 	// called directly after loading the DLL
/*N*/ 	// do whatever you want, you may use Sxx-DLL too
/*N*/ 
/*N*/ 	ScDocumentPool::InitVersionMaps();	// wird im ScModule ctor gebraucht
/*N*/ 
/*N*/ 	// the ScModule must be created
/*N*/ 	ScModuleDummy **ppShlPtr = (ScModuleDummy**) GetAppData(BF_SHL_CALC);
/*N*/ 	SvFactory *pFact = (SvFactory*)(*ppShlPtr)->pScDocShellFactory;
/*N*/ 	delete (*ppShlPtr);
/*N*/ 	ScModule* pMod = new ScModule((SfxObjectFactory*)pFact);
/*N*/ 	(*ppShlPtr) = pMod;
/*N*/ 	(*ppShlPtr)->pScDocShellFactory = pFact;
/*N*/ 
/*N*/ 	ScGlobal::Init();		// erst wenn der ResManager initialisiert ist
/*N*/ 							//	erst nach ScGlobal::Init duerfen die App-Optionen
/*N*/ 							//	initialisiert werden
/*N*/ 
/*N*/ 	//	Edit-Engine-Felder, soweit nicht schon in OfficeApplication::Init
/*N*/ 
/*N*/ 	SvClassManager& rClassManager = SvxFieldItem::GetClassManager();
/*N*/ //	rClassManager.SV_CLASS_REGISTER( SvxURLField );
/*N*/ //	rClassManager.SV_CLASS_REGISTER( SvxDateField );
/*N*/ //	rClassManager.SV_CLASS_REGISTER( SvxPageField );
/*N*/ 	rClassManager.SV_CLASS_REGISTER( SvxPagesField );
/*N*/ //	rClassManager.SV_CLASS_REGISTER( SvxTimeField );
/*N*/ 	rClassManager.SV_CLASS_REGISTER( SvxFileField );
/*N*/ //	rClassManager.SV_CLASS_REGISTER( SvxExtFileField );
/*N*/ 	rClassManager.SV_CLASS_REGISTER( SvxTableField );
/*N*/ 
/*N*/ 	SdrRegisterFieldClasses();		// SvDraw-Felder registrieren
/*N*/ 
/*N*/ 	pMod->PutItem( SfxUInt16Item( SID_ATTR_METRIC, pMod->GetAppOptions().GetAppMetric() ) );
/*N*/ 
/*N*/ 	//	StarOne Services are now handled in the registry
/*N*/ }

/*N*/ void ScDLL::Exit()
/*N*/ {
/*N*/ 	// called directly befor unloading the DLL
/*N*/ 	// do whatever you want, Sxx-DLL is accessible
/*N*/ 
/*N*/ 	// the SxxModule must be destroyed
/*N*/ 	ScModuleDummy **ppShlPtr = (ScModuleDummy**) GetAppData(BF_SHL_CALC);
/*N*/ 	delete (*ppShlPtr);
/*N*/ 	(*ppShlPtr) = NULL;
/*N*/ 
/*N*/ 	//	auf keinen Fall ein neues ScModuleDummy anlegen, weil dessen vtable sonst
/*N*/ 	//	in der DLL waere und das Loeschen im LibExit schiefgehen wuerde
/*N*/ 
/*N*/ 	//	ScGlobal::Clear ist schon im Module-dtor
/*N*/ }

//------------------------------------------------------------------
//	Statusbar
//------------------------------------------------------------------

#define TEXT_WIDTH(s)	rStatusBar.GetTextWidth((s))


#undef TEXT_WIDTH


}
