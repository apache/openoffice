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

#include "precompiled_basic.hxx"
#include "sbintern.hxx"
#include "image.hxx"
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

// Don't delete members - they're NULL in our stub
SbiGlobals::~SbiGlobals()
{
}

// Provide SbTextPortions implementation (normally in sb.cxx via SV_IMPL_VARARR)
SV_IMPL_VARARR(SbTextPortions, SbTextPortion)

// Stub for StarBASIC::CError() - scanner.cxx calls it with pBasic=NULL path
// This is a no-op stub

// Stubs for symbols from sb.cxx and image.cxx that are hidden in libsb.so
// These are only referenced by codegen.cxx and dim.cxx, not needed for tokenizer tests

// SbiImage stub (normally implemented in basic/source/classes/image.cxx)
// Using the same initialization list as the real constructor
// SbiImage constructor/destructor and methods
extern "C" {
// These will be provided by the linker if needed
}

// Provide all the symbols from sb.cxx and image.cxx that are needed by codegen.cxx and dim.cxx
// These are hidden in libsb.so so we must provide them

// SbClassFactory methods (normally in basic/source/classes/sb.cxx)
SbClassFactory::SbClassFactory( void ) {}
SbClassFactory::~SbClassFactory() {}
void SbClassFactory::AddClassModule( SbModule* ) {}
void SbClassFactory::RemoveClassModule( SbModule* ) {}
SbxBase* SbClassFactory::Create( sal_uInt16, sal_uInt32 ) { return NULL; }
SbxObject* SbClassFactory::CreateObject( const String& ) { return NULL; }
SbModule* SbClassFactory::FindClass( const String& ) { return NULL; }

// SbClassData methods (normally in basic/source/classes/sb.cxx)
SbClassData::SbClassData() {}
void SbClassData::clear() {}

// runsInSetup (normally in basic/source/classes/sb.cxx)
sal_Bool runsInSetup( void ) { return sal_False; }

// cloneTypeObjectImpl (normally in basic/source/classes/sb.cxx)
SbxObject* cloneTypeObjectImpl( const SbxObject& ) { return NULL; }

// SbiImage methods (normally in basic/source/classes/image.cxx)
// These are only referenced by codegen.cxx, not needed for tokenizer tests
SbiImage::SbiImage() {}
SbiImage::~SbiImage() {}
void SbiImage::Clear() {}
void SbiImage::MakeStrings( short ) {}
void SbiImage::AddString( const String& ) {}
void SbiImage::AddCode( char*, sal_uInt32 ) {}
void SbiImage::AddType(SbxObject*) {}
void SbiImage::AddEnum(SbxObject*) {}
String SbiImage::GetString( short ) const { return String(); }
const SbxObject* SbiImage::FindType( String ) const { return NULL; }
