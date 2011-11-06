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

#include <bf_sfx2/printer.hxx>
#include <vcl/virdev.hxx>

#include "sizedev.hxx"
#include "docsh.hxx"
#include "scmod.hxx"
#include "inputopt.hxx"
namespace binfilter {

//------------------------------------------------------------------

/*N*/ ScSizeDeviceProvider::ScSizeDeviceProvider( ScDocShell* pDocSh )
/*N*/ {
/*N*/ 	BOOL bTextWysiwyg = SC_MOD()->GetInputOptions().GetTextWysiwyg();
/*N*/ 	if ( bTextWysiwyg )
/*N*/ 	{
/*?*/ 		pDevice = pDocSh->GetPrinter();
/*?*/ 		bOwner = FALSE;
/*?*/ 
/*?*/ 		aOldMapMode = pDevice->GetMapMode();
/*?*/ 		pDevice->SetMapMode( MAP_PIXEL );		// GetNeededSize needs pixel MapMode
/*?*/ 		// printer has right DigitLanguage already
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		pDevice = new VirtualDevice;
/*N*/ 		pDevice->SetDigitLanguage( SC_MOD()->GetOptDigitLanguage() );
/*N*/ 		bOwner = TRUE;
/*N*/ 	}
/*N*/ 
/*N*/ 	Point aLogic = pDevice->LogicToPixel( Point(1000,1000), MAP_TWIP );
/*N*/ 	nPPTX = aLogic.X() / 1000.0;
/*N*/ 	nPPTY = aLogic.Y() / 1000.0;
/*N*/ 
/*N*/ 	if ( !bTextWysiwyg )
/*N*/ 		nPPTX /= pDocSh->GetOutputFactor();
/*N*/ }

/*N*/ ScSizeDeviceProvider::~ScSizeDeviceProvider()
/*N*/ {
/*N*/ 	if (bOwner)
/*N*/ 		delete pDevice;
/*N*/ 	else
/*?*/ 		pDevice->SetMapMode( aOldMapMode );
/*N*/ }

}
