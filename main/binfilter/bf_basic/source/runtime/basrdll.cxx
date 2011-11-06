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



/*?*/ // #ifndef _SV_SVAPP_HXX //autogen
/*?*/ // #include <vcl/svapp.hxx>
/*?*/ // #endif
#include <bf_svtools/bf_solar.h>
/*?*/ // #ifndef _TOOLS_DEBUG_HXX //autogen
/*?*/ // #include <tools/debug.hxx>
/*?*/ // #endif
/*?*/ // #ifndef _SV_MSGBOX_HXX //autogen
/*?*/ // #include <vcl/msgbox.hxx>
/*?*/ // #endif

/*?*/ // #include "sbstar.hxx"
#include "basrdll.hxx"
/*?*/ // #include "basrid.hxx"
/*?*/ // #include "sb.hrc"

namespace binfilter {

/*?*/ // BasicResId::BasicResId( USHORT nId ):
/*?*/ // 	ResId( nId, (*(BasicDLL**)GetAppData(SHL_BASIC))->GetResMgr() )
/*?*/ // {
/*?*/ // }

BasicDLL::BasicDLL()
{
/*?*/ // 	*(BasicDLL**)GetAppData(SHL_BASIC) = this;
/*?*/ // 	::com::sun::star::lang::Locale aLocale = Application::GetSettings().GetUILocale();
/*?*/ // 	pResMgr = ResMgr::CreateResMgr(CREATEVERSIONRESMGR_NAME(ofa), aLocale );
	bDebugMode = FALSE;
	bBreakEnabled = TRUE;
}

BasicDLL::~BasicDLL()
{
/*?*/ //    delete pResMgr;
}

/*?*/ // void BasicDLL::EnableBreak( BOOL bEnable )
/*?*/ // {
/*?*/ // 	BasicDLL* pThis = *(BasicDLL**)GetAppData(SHL_BASIC);
/*?*/ // 	DBG_ASSERT( pThis, "BasicDLL::EnableBreak: Noch keine Instanz!" );
/*?*/ // 	if ( pThis )
/*?*/ // 		pThis->bBreakEnabled = bEnable;
/*?*/ // }
/*?*/ // 
/*?*/ // void BasicDLL::SetDebugMode( BOOL bDebugMode )
/*?*/ // {
/*?*/ // 	BasicDLL* pThis = *(BasicDLL**)GetAppData(SHL_BASIC);
/*?*/ // 	DBG_ASSERT( pThis, "BasicDLL::EnableBreak: Noch keine Instanz!" );
/*?*/ // 	if ( pThis )
/*?*/ // 		pThis->bDebugMode = bDebugMode;
/*?*/ // }
/*?*/ // 
/*?*/ // 
/*?*/ // void BasicDLL::BasicBreak()
/*?*/ // {
/*?*/ // 	//bJustStopping: Wenn jemand wie wild x-mal STOP drueckt, aber das Basic
/*?*/ // 	// nicht schnell genug anhaelt, kommt die Box ggf. oefters...
/*?*/ // 	static BOOL bJustStopping = FALSE;
/*?*/ // 
/*?*/ // 	BasicDLL* pThis = *(BasicDLL**)GetAppData(SHL_BASIC);
/*?*/ // 	DBG_ASSERT( pThis, "BasicDLL::EnableBreak: Noch keine Instanz!" );
/*?*/ // 	if ( pThis )
/*?*/ // 	{
/*?*/ // 		if ( StarBASIC::IsRunning() && !bJustStopping && ( pThis->bBreakEnabled || pThis->bDebugMode ) )
/*?*/ // 		{
/*?*/ // 			bJustStopping = TRUE;
/*?*/ // 			StarBASIC::Stop();
/*?*/ // 			String aMessageStr( BasicResId( IDS_SBERR_TERMINATED ) );
/*?*/ // 			InfoBox( 0, aMessageStr ).Execute();
/*?*/ // 			bJustStopping = FALSE;
/*?*/ // 		}
/*?*/ // 	}
/*?*/ // }

}

