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


//#if STLPORT_VERSION>=321
//#include <math.h>	// prevent conflict between exception and std::exception
//#endif

#ifndef _HINTIDS_HXX
#include <hintids.hxx>
#endif

#ifndef _SVDVIEW_HXX //autogen
#include <bf_svx/svdview.hxx>
#endif
#ifndef _IPENV_HXX //autogen
#include <bf_so3/ipenv.hxx>
#endif

#ifndef _WDOCSH_HXX
#include <wdocsh.hxx>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _WRTSH_HXX
#include <wrtsh.hxx>
#endif
#ifndef _CMDID_H
#include <cmdid.h>
#endif
#ifndef _PAGEDESC_HXX
#include <pagedesc.hxx>
#endif
#ifndef _VISCRS_HXX //autogen wg. SwSelPaintRects
#include <viscrs.hxx>
#endif
#ifndef _SWDTFLVR_HXX
#include <swdtflvr.hxx>
#endif
#ifndef _SWERROR_H
#include <swerror.h>
#endif
#ifndef _WRTSH_HRC
#include <wrtsh.hrc>
#endif
namespace binfilter {

/*N*/ void SwWrtShell::CalcAndSetScale( SvEmbeddedObjectRef xObj,
/*N*/ 							 const SwRect *pFlyPrtRect,
/*N*/ 							 const SwRect *pFlyFrmRect )
/*N*/ {
			DBG_ERROR("Split!");
/*N*/ }

	void SwWrtShell::ConnectObj( SvInPlaceObjectRef, const SwRect &, const SwRect & )
	{
	}

	SwWrtShell::~SwWrtShell()
	{
		SET_CURR_SHELL( this );
		while(IsModePushed())
		{
			DBG_BF_ASSERT(0, "STRIP");
		};
		while(PopCrsr(FALSE));
	}
}
