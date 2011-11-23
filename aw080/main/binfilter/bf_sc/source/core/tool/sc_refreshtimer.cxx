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

#include "refreshtimer.hxx"
#include <tools/debug.hxx>//STRIP001 
namespace binfilter {

/*N*/ ScRefreshTimerProtector::ScRefreshTimerProtector( ScRefreshTimerControl * const * pp )
/*N*/ 		:
/*N*/ 		ppControl( pp )
/*N*/ {
/*N*/ 	if ( ppControl && *ppControl )
/*N*/ 	{
/*N*/ 		(*ppControl)->SetAllowRefresh( FALSE );
/*N*/ 		// wait for any running refresh in another thread to finnish
/*N*/ 		::vos::OGuard aGuard( (*ppControl)->GetMutex() );
/*N*/ 	}
/*N*/ }


/*N*/ ScRefreshTimer::~ScRefreshTimer()
/*N*/ {
/*N*/ 	if ( IsActive() )
/*?*/ 		Stop();
/*N*/ 	RemoveFromControl();
/*N*/ }


/*N*/ void ScRefreshTimer::SetRefreshDelay( ULONG nSeconds )
/*N*/ {
/*N*/ 	DBG_BF_ASSERT(0, "STRIP"); //STRIP001 BOOL bActive = IsActive();
/*N*/ }



}
