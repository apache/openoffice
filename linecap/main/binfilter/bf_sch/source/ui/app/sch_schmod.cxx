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



#define ITEMID_SEARCH           SID_SEARCH_ITEM

#ifdef _MSC_VER
#pragma hdrstop
#endif


#include <bf_svx/svxids.hrc>
#include <bf_sfx2/app.hxx>
#include <vcl/status.hxx>
#include <bf_svx/svdobj.hxx>

#define _SCH_DLL                 // fuer SD_MOD()
#include "schmod.hxx"
#include "schresid.hxx"

#include "app.hrc"
#include "glob.hrc"
#include "strings.hrc"
#include "res_bmp.hrc"

namespace binfilter {

/*N*/ TYPEINIT1( SchModuleDummy, SfxModule );
/*N*/ TYPEINIT1( SchModule, SchModuleDummy );

/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/

/*N*/ SchModule::SchModule(SvFactory* pObjFact) :
/*N*/ 	SchModuleDummy(SFX_APP()->CreateResManager("bf_sch"), FALSE, pObjFact),	//STRIP005
/*N*/ 	pXOutDevPool( NULL ),
/*N*/ 	pDragData( NULL ),
/*N*/ 	pClipboardData( NULL ),
/*N*/ 	mpTransferDragDrop( NULL ),
/*N*/ 	mpTransferClipboard( NULL ),
/*N*/ 	mpTransferSelectionClipbd( NULL ),
/*N*/ 	pChartOptions( NULL )
/*N*/ {
/*N*/ 	pSchObjFactory = new SchObjFactory();
/*N*/
/*N*/ 	if (!pSchObjFactory->GetInserted())
/*N*/ 	{
/*N*/ 		SdrObjFactory::InsertMakeObjectHdl(LINK(pSchObjFactory, SchObjFactory, MakeObject));
/*N*/ 		SdrObjFactory::InsertMakeUserDataHdl(LINK(pSchObjFactory, SchObjFactory, MakeUserData));
/*N*/
/*N*/ 		pSchObjFactory->SetInserted(TRUE);
/*N*/ 	}
/*N*/
/*N*/ 	// #62493# this factory exists already
/*N*/ 	//pE3dFactory = new E3dObjFactory();
/*N*/ 	//SdrObjFactory::InsertMakeObjectHdl(LINK(pE3dFactory, E3dObjFactory, MakeObject));
/*N*/
/*N*/ 	SetName( String( RTL_CONSTASCII_USTRINGPARAM( "StarChart" )) );
/*N*/
/*N*/ 	// establish listener, so that the module is informed
/*N*/ 	// right before the application is going to be shut down
/*N*/ 	StartListening( *SFX_APP() );
/*N*/ }

/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/

/*N*/ SchModule::~SchModule()
/*N*/ {
/*N*/ 	if (pSchObjFactory)
/*N*/ 	{
/*N*/ 		if (pSchObjFactory->GetInserted())
/*N*/ 		{
/*N*/ 			SdrObjFactory::RemoveMakeObjectHdl(LINK(pSchObjFactory, SchObjFactory, MakeObject));
/*N*/ 			SdrObjFactory::RemoveMakeUserDataHdl(LINK(pSchObjFactory, SchObjFactory, MakeUserData));
/*N*/ 		}
/*N*/
/*N*/ 		delete pSchObjFactory;
/*N*/ 	}
/*N*/ }


/*?*/ SfxModule *SchModuleDummy::Load()
/*?*/ {
/*?*/ 	return NULL;
/*?*/ }

/*N*/ SchOptions* SchModule::GetSchOptions()
/*N*/ {
/*N*/ 	if( ! pChartOptions )
/*N*/ 		pChartOptions = new SchOptions();
/*N*/
/*N*/ 	return pChartOptions;
/*N*/ }

/*************************************************************************
|*
|* get notifications
|*
\************************************************************************/

/*N*/ void SchModule::Notify( SfxBroadcaster& rBC, const SfxHint& rHint )
/*N*/ {
/*N*/ 	if( rHint.ISA( SfxSimpleHint ) &&
/*N*/ 		( (SfxSimpleHint&) rHint ).GetId() == SFX_HINT_DEINITIALIZING )
/*N*/ 	{
/*N*/ 		delete pChartOptions, pChartOptions = NULL;
/*N*/ 	}
/*N*/ }
}
