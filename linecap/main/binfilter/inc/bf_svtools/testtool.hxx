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


#ifndef SVTOOLS_TESTTOOL_HXX
#define SVTOOLS_TESTTOOL_HXX

#include <bf_svtools/bf_solar.h>
#ifndef _LINK_HXX //autogen
#include <tools/link.hxx>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

class Application;
class SvStream;

namespace binfilter
{

class StatementFlow;
class CommunicationManager;
class CommunicationLink;
#if OSL_DEBUG_LEVEL > 1
class EditWindow;
#endif
class ImplRC;

class RemoteControl
{
	friend class StatementFlow;

	BOOL         m_bIdleInserted;
#if OSL_DEBUG_LEVEL > 1
	EditWindow *m_pDbgWin;
#endif
	ImplRC* pImplRC;

public:
	RemoteControl();
	~RemoteControl();
	BOOL QueCommands( ULONG nServiceId, SvStream *pIn );
	SvStream* GetReturnStream();

	DECL_LINK( IdleHdl,   Application* );
	DECL_LINK( CommandHdl, Application* );

	DECL_LINK( QueCommandsEvent, CommunicationLink* );
	ULONG nStoredServiceId;
	SvStream *pStoredStream;

	void ExecuteURL( String &aURL );

protected:
	CommunicationManager *pServiceMgr;
	SvStream *pRetStream;
};

}

#endif // SVTOOLS_TESTTOOL_HXX
