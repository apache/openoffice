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



#include "sbx.hxx"
//#include "sbcomp.hxx"
#include "image.hxx"


// For debugging only
// #define DBG_SAVE_DISASSEMBLY

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess3.hpp>
#include <com/sun/star/io/XTextOutputStream.hpp>
#include <com/sun/star/io/XActiveDataSource.hpp>

namespace binfilter {

/*?*/ // #ifdef DBG_SAVE_DISASSEMBLY
/*?*/ // static bool dbg_bDisassemble = true;
/*?*/ // 
/*?*/ // using namespace comphelper;
/*?*/ // using namespace rtl;
/*?*/ // using namespace com::sun::star::uno;
/*?*/ // using namespace com::sun::star::lang;
/*?*/ // using namespace com::sun::star::ucb;
/*?*/ // using namespace com::sun::star::io;
/*?*/ // 
/*?*/ // void dbg_SaveDisassembly( SbModule* pModule )
/*?*/ // {
/*?*/ //     bool bDisassemble = dbg_bDisassemble;
/*?*/ //     if( bDisassemble )
/*?*/ // 	{
/*?*/ // 		Reference< XSimpleFileAccess3 > xSFI;
/*?*/ // 		Reference< XTextOutputStream > xTextOut;
/*?*/ // 		Reference< XOutputStream > xOut;
/*?*/ // 		Reference< XMultiServiceFactory > xSMgr = getProcessServiceFactory();
/*?*/ // 		if( xSMgr.is() )
/*?*/ // 		{
/*?*/ // 			Reference< XSimpleFileAccess3 > xSFI = Reference< XSimpleFileAccess3 >( xSMgr->createInstance
/*?*/ // 				( OUString::createFromAscii( "com.sun.star.ucb.SimpleFileAccess" ) ), UNO_QUERY );
/*?*/ // 			if( xSFI.is() )
/*?*/ // 			{
/*?*/ // 				String aFile( RTL_CONSTASCII_USTRINGPARAM("file:///d:/BasicAsm_") );
/*?*/ // 				StarBASIC* pBasic = (StarBASIC*)pModule->GetParent();
/*?*/ // 				if( pBasic )
/*?*/ // 				{
/*?*/ // 					aFile += pBasic->GetName();
/*?*/ // 					aFile.AppendAscii( "_" );
/*?*/ // 				}
/*?*/ // 				aFile += pModule->GetName();
/*?*/ // 				aFile.AppendAscii( ".txt" );
/*?*/ // 
/*?*/ // 				// String aFile( RTL_CONSTASCII_USTRINGPARAM("file:///d:/BasicAsm.txt") );
/*?*/ // 				if( xSFI->exists( aFile ) )
/*?*/ // 					xSFI->kill( aFile );
/*?*/ // 				xOut = xSFI->openFileWrite( aFile );
/*?*/ // 				Reference< XInterface > x = xSMgr->createInstance( OUString::createFromAscii( "com.sun.star.io.TextOutputStream" ) );
/*?*/ // 				Reference< XActiveDataSource > xADS( x, UNO_QUERY );
/*?*/ // 				xADS->setOutputStream( xOut );
/*?*/ // 				xTextOut = Reference< XTextOutputStream >( x, UNO_QUERY );
/*?*/ // 			}
/*?*/ // 		}
/*?*/ // 
/*?*/ // 		if( xTextOut.is() )
/*?*/ // 		{
/*?*/ // 			String aDisassemblyStr;
/*?*/ // 			pModule->Disassemble( aDisassemblyStr );
/*?*/ // 			xTextOut->writeString( aDisassemblyStr );
/*?*/ // 		}
/*?*/ // 		xOut->closeOutput();
/*?*/ // 	}
/*?*/ // }
/*?*/ // #endif

// Diese Routine ist hier definiert, damit der Compiler als eigenes Segment
// geladen werden kann.

BOOL SbModule::Compile()
{
    DBG_ERROR( "SbModule::Compile: dead code!" );
    return FALSE;
}

}
