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

#include <limits.h>




#ifndef _CMDID_H
#include <cmdid.h>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _SWDTFLVR_HXX
#include <swdtflvr.hxx>
#endif

#if OSL_DEBUG_LEVEL > 1
#endif
namespace binfilter {

namespace com { namespace sun { namespace star { namespace util {
	struct SearchOptions;
} } } }

using namespace ::com::sun::star::util;


static long nStartDragX = 0, nStartDragY = 0;
static BOOL  bStartDrag = FALSE;

/*------------------------------------------------------------------------
 Beschreibung:	Rahmengebundenes Macro ausfuehren
------------------------------------------------------------------------*/

/*N*/ IMPL_LINK( SwWrtShell, ExecFlyMac, void *, pFlyFmt )
/*N*/ {
DBG_BF_ASSERT(0, "STRIP"); //STRIP001 	const SwFrmFmt *pFmt = pFlyFmt ? (SwFrmFmt*)pFlyFmt : GetFlyFrmFmt();
/*N*/ 	return 0;
/*N*/ }

}
