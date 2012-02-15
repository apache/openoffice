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

#ifndef _SWTYPES_HXX
#include <swtypes.hxx>
#endif
#ifndef _ENVIMG_HXX
#include <envimg.hxx>
#endif

#ifndef _CMDID_H
#include <cmdid.h>
#endif
namespace binfilter {

#ifdef WIN
#define NEXTLINE  UniString::CreateFromAscii("\r\n")
#else
#define NEXTLINE  '\n'
#endif

using namespace utl;
using namespace rtl;
using namespace ::com::sun::star::uno;

#define C2U(cChar) OUString::createFromAscii(cChar)

/*N*/ TYPEINIT1_AUTOFACTORY( SwEnvItem, SfxPoolItem );

/*N*/ SwEnvItem::SwEnvItem() :
/*N*/ 	SfxPoolItem(FN_ENVELOP)
/*N*/ {
/*N*/ DBG_BF_ASSERT(0, "STRIP"); //STRIP001 	aAddrText       = aEmptyStr;
/*N*/ }
// --------------------------------------------------------------------------
/*N*/ SwEnvItem::SwEnvItem(const SwEnvItem& rItem) :
/*N*/ 	SfxPoolItem(FN_ENVELOP),
/*N*/ 	aAddrText      (rItem.aAddrText),
/*N*/ 	bSend          (rItem.bSend),
/*N*/ 	aSendText      (rItem.aSendText),
/*N*/ 	lSendFromLeft  (rItem.lSendFromLeft),
/*N*/ 	lSendFromTop   (rItem.lSendFromTop),
/*N*/ 	lAddrFromLeft  (rItem.lAddrFromLeft),
/*N*/ 	lAddrFromTop   (rItem.lAddrFromTop),
/*N*/ 	lWidth         (rItem.lWidth),
/*N*/ 	lHeight        (rItem.lHeight),
/*N*/ 	eAlign         (rItem.eAlign),
/*N*/ 	bPrintFromAbove(rItem.bPrintFromAbove),
/*N*/ 	lShiftRight    (rItem.lShiftRight),
/*N*/ 	lShiftDown     (rItem.lShiftDown)
/*N*/ {DBG_BF_ASSERT(0, "STRIP"); //STRIP001
/*N*/ }

// --------------------------------------------------------------------------
/*N*/ int SwEnvItem::operator ==(const SfxPoolItem& rItem) const
/*N*/ {
DBG_BF_ASSERT(0, "STRIP"); return 0;//STRIP001 	const SwEnvItem& rEnv = (const SwEnvItem&) rItem;
/*N*/ }

// --------------------------------------------------------------------------
/*N*/ SfxPoolItem* SwEnvItem::Clone(SfxItemPool*) const
/*N*/ {
/*N*/ 	return new SwEnvItem(*this);
/*N*/ }
// --------------------------------------------------------------------------
}
