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



#ifndef _EHDL_HXX
#define _EHDL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef __RSC

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _EINF_HXX
#include <tools/errinf.hxx>
#endif

class Window;
class ResMgr;

namespace binfilter
{

class  SfxErrorContext : private ErrorContext
{
public:
	SfxErrorContext(
			USHORT nCtxIdP, const String &aArg1, Window *pWin=0,
			USHORT nResIdP=USHRT_MAX, ResMgr *pMgrP=0);
	virtual BOOL GetString(ULONG nErrId, String &rStr);

private:
	USHORT nCtxId;
	USHORT nResId;
	ResMgr *pMgr;
	String aArg1;
};

class  SfxErrorHandler : private ErrorHandler
{
public:
	SfxErrorHandler(USHORT nId, ULONG lStart, ULONG lEnd, ResMgr *pMgr=0);
	~SfxErrorHandler();

protected:
	virtual BOOL     GetErrorString(ULONG lErrId, String &, USHORT&) const;
	virtual BOOL     GetMessageString(ULONG lErrId, String &, USHORT&) const;

private:

	ULONG            lStart;
	ULONG            lEnd;
	USHORT           nId;
	ResMgr          *pMgr;
    ResMgr          *pFreeMgr;

	BOOL             GetClassString(ULONG lErrId, String &) const;
	virtual BOOL     CreateString(
						 const ErrorInfo *, String &, USHORT &) const;
};


}

#endif

#endif

