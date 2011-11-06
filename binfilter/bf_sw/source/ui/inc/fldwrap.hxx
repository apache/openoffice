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


#ifndef _FLDWRAP_HXX
#define _FLDWRAP_HXX

#include <bf_svtools/bf_solar.h>

#include "chldwrap.hxx"
namespace binfilter {

class SwFldDlgWrapper : public SwChildWinWrapper
{
public:
	SwFldDlgWrapper( Window* pParent, USHORT nId,
						SfxBindings* pBindings, SfxChildWinInfo* pInfo ): SwChildWinWrapper( pParent, nId ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 SfxBindings* pBindings, SfxChildWinInfo* pInfo );

	SFX_DECL_CHILDWINDOW(SwFldDlgWrapper);

};
/* -----------------04.02.2003 14:14-----------------
 * field dialog only showing database page to support
 * mail merge 
 * --------------------------------------------------*/
class SwFldDataOnlyDlgWrapper : public SwChildWinWrapper
{
public:
    SwFldDataOnlyDlgWrapper( Window* pParent, USHORT nId,
                        SfxBindings* pBindings, SfxChildWinInfo* pInfo );

    SFX_DECL_CHILDWINDOW(SwFldDataOnlyDlgWrapper);

    virtual BOOL    ReInitDlg(SwDocShell *pDocSh);
};


} //namespace binfilter
#endif

