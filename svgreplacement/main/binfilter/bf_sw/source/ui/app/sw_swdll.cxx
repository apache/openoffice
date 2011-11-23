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




#ifndef _SVDOBJ_HXX //autogen
#include <bf_svx/svdobj.hxx>
#endif
#ifndef _RTL_LOGFILE_HXX_
#include <rtl/logfile.hxx>
#endif

#ifndef _GLOBDOC_HRC
#include "globdoc.hrc"
#endif

#ifndef _SWWDOCSH_HXX
#include <wdocsh.hxx>
#endif
#ifndef _SWGLOBDOCSH_HXX
#include <globdoc.hxx>
#endif
#ifndef _SWMODULE_HXX
#include <swmodule.hxx>
#endif
#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _INIT_HXX
#include <init.hxx>
#endif
#ifndef _DOBJFAC_HXX
#include <dobjfac.hxx>
#endif
#ifndef _CFGID_H
#include <cfgid.h>
#endif
namespace binfilter {

/*************************************************************************
|*
|* Init
|*
\************************************************************************/

/*N*/ void SwDLL::Init()
/*N*/ {
/*N*/ 	RTL_LOGFILE_CONTEXT_AUTHOR( aLog, "SW", "JP93722",  "SwDLL" );
/*N*/
/*N*/ 	// called directly after loading the DLL
/*N*/ 	// do whatever you want, you may use Sw-DLL too
/*N*/
/*N*/ 	// the SdModule must be created
/*N*/ 	SwModuleDummy** ppShlPtr = (SwModuleDummy**) GetAppData(BF_SHL_WRITER);
/*N*/
/*N*/ 	SvFactory* pDocFact     = (SvFactory*)(*ppShlPtr)->pSwDocShellFactory;
/*N*/ 	SvFactory* pWDocFact    = (SvFactory*)(*ppShlPtr)->pSwWebDocShellFactory;
/*N*/ 	SvFactory* pGlobDocFact = (SvFactory*)(*ppShlPtr)->pSwGlobalDocShellFactory;
/*N*/ 	delete (*ppShlPtr);
/*N*/     SwModule* pModule = new SwModule( pWDocFact, pDocFact, pGlobDocFact );
/*N*/ 	(*ppShlPtr) = pModule;
/*N*/ 	(*ppShlPtr)->pSwDocShellFactory    		= pDocFact    ;
/*N*/ 	(*ppShlPtr)->pSwWebDocShellFactory     	= pWDocFact   ;
/*N*/ 	(*ppShlPtr)->pSwGlobalDocShellFactory 	= pGlobDocFact;
/*N*/
/*N*/ 	SdrObjFactory::InsertMakeObjectHdl( LINK( &aSwObjectFactory, SwObjectFactory, MakeObject ) );
/*N*/
/*N*/ 	RTL_LOGFILE_CONTEXT_TRACE( aLog, "Init Core/UI/Filter" );
/*N*/
/*N*/ 	//Initialisierung der Statics
/*N*/ 	::binfilter::_InitCore();
/*N*/ 	::binfilter::_InitFilter();
///*N*/   ::binfilter::_InitUI();
/*N*/
/*N*/ 	pModule->InitAttrPool();
/*N*/ 	//jetzt darf das SwModule seinen Pool anlegen
/*N*/ }

/*************************************************************************
|*
|* Exit
|*
\************************************************************************/

/*N*/ void SwDLL::Exit()
/*N*/ {
/*N*/ 	// called directly befor unloading the DLL
/*N*/ 	// do whatever you want, Sw-DLL is accessible
/*N*/
/*N*/ 	// der Pool muss vor den statics geloescht werden
/*N*/ 	SW_MOD()->RemoveAttrPool();
/*N*/
///*N*/   ::binfilter::_FinitUI();
/*N*/ 	::binfilter::_FinitFilter();
/*N*/ 	::binfilter::_FinitCore();
/*N*/ 	// Objekt-Factory austragen
/*N*/ 	SdrObjFactory::RemoveMakeObjectHdl(LINK(&aSwObjectFactory, SwObjectFactory, MakeObject ));
/*N*/    // the SwModule must be destroyed
/*N*/ 	SwModuleDummy** ppShlPtr = (SwModuleDummy**) GetAppData(BF_SHL_WRITER);
/*N*/ 	delete (*ppShlPtr);
/*N*/ 	(*ppShlPtr) = NULL;
/*N*/ }

}
