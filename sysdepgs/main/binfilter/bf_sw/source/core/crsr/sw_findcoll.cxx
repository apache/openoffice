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

#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

#ifndef _SWCRSR_HXX
#include <swcrsr.hxx>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

namespace binfilter {


//------------------ Methoden der CrsrShell ---------------------------

// Parameter fuer das Suchen vom FormatCollections






// Suchen nach Format-Collections


/*M*/ ULONG SwCursor::Find( const SwTxtFmtColl& rFmtColl,
/*M*/                     SwDocPositions nStart, SwDocPositions nEnde, BOOL& bCancel,
/*M*/ 					FindRanges eFndRngs, const SwTxtFmtColl* pReplFmtColl )
/*M*/ {DBG_BF_ASSERT(0, "STRIP");return 0;//STRIP001 
/*M*/ 	// OLE-Benachrichtigung abschalten !!
/*M*/ }



}
