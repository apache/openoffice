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



#ifndef _SCH_APP_HXX
#define _SCH_APP_HXX

// Die folgenden includes gehoeren eigentlich in app.cxx

#include "schdll.hxx"

class SdClipboard;
class SdBasic;
class BasicIDE;

#ifndef _SCH_GLOB_HXX
#include <glob.hxx>
#endif

#ifndef _SFXNEW_HXX //autogen
#include <bf_sfx2/new.hxx>
#endif

#ifndef _SVXERR_HXX //autogen
#include <bf_svx/svxerr.hxx>
#endif

#ifndef _SFXAPP_HXX //autogen
#include <bf_sfx2/app.hxx>
#endif
namespace binfilter {

/*************************************************************************
|*
|* Klasse fuer Applikationsobjekt
|*
\************************************************************************/

class SchApp: public SfxApplication
{
protected :
	virtual void Init();
	virtual void Exit();
	virtual void OpenClients();

	SvxErrorHandler *pSvxErrorHandler;

public:
	SchApp();
	virtual ~SchApp();

	virtual void Main();
};



} //namespace binfilter
#endif		// _SCH_APP_HXX

