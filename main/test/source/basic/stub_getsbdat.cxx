/**************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 *  "License"); you may not use this file except in compliance
 *  with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 *  specific language governing permissions and limitations
 *  under the License.
 *
 *************************************************************/

#include "precompiled_basic.hxx"
#include "sbintern.hxx"
#include <basic/sbdef.hxx>
#include <tools/shl.hxx>
#include <svl/svarray.hxx>

// Minimal stub for GetSbData() to avoid pulling in sbintern.cxx dependencies
SbiGlobals* GetSbData()
{
    static SbiGlobals* pInstance = NULL;
    if (!pInstance)
        pInstance = new SbiGlobals();
    return pInstance;
}

// Provide SbErrorStack implementation (normally in sbintern.cxx via SV_IMPL_PTRARR)
SV_IMPL_PTRARR(SbErrorStack, SbErrorStackEntry*)

// Minimal SbiGlobals constructor/destructor - only fields accessed by scanner.cxx/token.cxx are initialized
SbiGlobals::SbiGlobals()
    : pInst(NULL), pMod(NULL), pSbFac(NULL), pUnoFac(NULL),
      pTypeFac(NULL), pClassFac(NULL), pOLEFac(NULL), pFormFac(NULL),
      pCompMod(NULL), nInst(0), nCode(0), nLine(0), nCol1(0), nCol2(0),
      bCompiler(sal_False), bGlobalInitErr(sal_False), bRunInit(sal_False),
      eLanguageMode(SB_LANG_BASIC), pErrStack(NULL),
      pTransliterationWrapper(NULL), bBlockCompilerError(sal_False),
      pAppBasMgr(NULL), pMSOMacroRuntimLib(NULL)
{
}

// Don't delete members - they're NULL in our stub, and SbErrorStack DeleteAndDestroy
// is provided by the SV_IMPL_PTRARR above
SbiGlobals::~SbiGlobals()
{
}

// Provide SbTextPortions implementation (normally in sb.cxx via SV_IMPL_VARARR)
SV_IMPL_VARARR(SbTextPortions, SbTextPortion)

// Stub for StarBASIC::CError() to avoid linking against libsb.so
// This is only needed because scanner.cxx references it (pBasic=NULL path)
