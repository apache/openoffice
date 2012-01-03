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




#ifdef _MSC_VER
#pragma hdrstop
#endif

#include <hintids.hxx>
#ifndef _ERRHDL_HXX //autogen
#include <errhdl.hxx>
#endif
#ifndef _FMTHBSH_HXX //autogen
#include <fmthbsh.hxx>
#endif
namespace binfilter {


/*************************************************************************
|*
|*    class SwFmtHardBlank
|*
|*    Beschreibung      Dokument 1.20
|*    Ersterstellung    JP 23.11.90
|*    Letzte Aenderung  JP 20.02.91
|*
*************************************************************************/

/*N*/ SwFmtHardBlank::SwFmtHardBlank( sal_Unicode cCh, BOOL bCheck )
/*N*/ 	: SfxPoolItem( RES_TXTATR_HARDBLANK ),
/*N*/ 	cChar( cCh )
/*N*/ {
/*N*/ 	ASSERT( !bCheck || (' ' != cCh && '-' != cCh),
/*N*/ 			"Invalid character for the HardBlank attribute - "
/*N*/ 			"must be a normal unicode character" );
/*N*/ }



/*************************************************************************
|*
|*    class SwFmtSoftHyph
|*
|*    Beschreibung      Dokument 1.20
|*    Ersterstellung    JP 23.11.90
|*    Letzte Aenderung  JP 20.02.91
|*
*************************************************************************/

/*N*/ SwFmtSoftHyph::SwFmtSoftHyph()
/*N*/ 	: SfxPoolItem( RES_TXTATR_SOFTHYPH )
/*N*/ {
/*N*/ }




}
