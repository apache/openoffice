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

#define ITEMID_BOXINFO      SID_ATTR_BORDER_INNER



#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _ZFORMAT_HXX //autogen
#include <bf_svtools/zformat.hxx>
#endif
#ifndef _SVX_DIALOGS_HRC
#include <bf_svx/dialogs.hrc>
#endif

#define READ_OLDVERS		// erstmal noch alte Versionen lesen

#ifndef _DOC_HXX
#include <doc.hxx>
#endif
#ifndef _TBLAFMT_HXX
#include <tblafmt.hxx>
#endif
#ifndef _CELLATR_HXX
#include <cellatr.hxx>
#endif
#ifndef _ERRHDL_HXX //autogen
#include <errhdl.hxx>
#endif
#include "bf_so3/staticbaseurl.hxx"
namespace binfilter {

// bis SO5PF
const USHORT AUTOFORMAT_ID_X		= 9501;
const USHORT AUTOFORMAT_ID_358		= 9601;
const USHORT AUTOFORMAT_DATA_ID_X	= 9502;

// ab SO5
//! in nachfolgenden Versionen muss der Betrag dieser IDs groesser sein
const USHORT AUTOFORMAT_ID_504		= 9801;
const USHORT AUTOFORMAT_DATA_ID_504	= 9802;

const USHORT AUTOFORMAT_ID_552		= 9901;
const USHORT AUTOFORMAT_DATA_ID_552	= 9902;

// --- from 641 on: CJK and CTL font settings
const USHORT AUTOFORMAT_ID_641      = 10001;
const USHORT AUTOFORMAT_DATA_ID_641 = 10002;

// current version
const USHORT AUTOFORMAT_ID          = AUTOFORMAT_ID_641;
const USHORT AUTOFORMAT_DATA_ID     = AUTOFORMAT_DATA_ID_641;


#ifdef READ_OLDVERS
const USHORT AUTOFORMAT_OLD_ID		= 8201;
const USHORT AUTOFORMAT_OLD_ID1		= 8301;
const USHORT AUTOFORMAT_OLD_DATA_ID	= 8202;
#endif

}
