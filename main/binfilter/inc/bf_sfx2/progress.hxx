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


#ifndef _SFX_PROGRESS_HXX
#define _SFX_PROGRESS_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _LINK_HXX
#include <tools/link.hxx>
#endif
class String;
namespace binfilter {

class SfxObjectShell;
class SfxStatusBarManager;
class INetHint;
class SvDispatch;
struct SfxProgress_Impl;
struct PlugInLoadStatus;
struct SvProgressArg;

//=======================================================================

class SfxProgress
{
	SfxProgress_Impl*		pImp;
	ULONG					nVal;
	BOOL					bSuspended;

public:
							SfxProgress( SfxObjectShell* pObjSh,
										 const String& rText,
										 ULONG nRange, BOOL bAllDocs = FALSE,
										 BOOL bWait = TRUE );
	virtual 				~SfxProgress();

	virtual void			SetText( const String& rText );
	virtual BOOL			SetState( ULONG nVal, ULONG nNewRange = 0 );
	ULONG					GetState() const { return nVal; }

	void					Resume();
	void					Suspend();
	BOOL					IsSuspended() const { return bSuspended; }

	void					Reschedule();

	void					Stop();


	static SfxProgress* 	GetActiveProgress( SfxObjectShell *pDocSh = 0 );

#if _SOLAR__PRIVATE
	DECL_STATIC_LINK( SfxProgress, SetStateHdl, PlugInLoadStatus* );
	DECL_STATIC_LINK( SfxProgress, DefaultBindingProgress, SvProgressArg* );
	FASTBOOL				StatusBarManagerGone_Impl(SfxStatusBarManager*pStb);
#endif
};

}//end of namespace binfilter
#endif

