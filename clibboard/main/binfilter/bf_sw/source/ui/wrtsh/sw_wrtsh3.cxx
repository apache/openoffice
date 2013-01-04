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

#ifndef _SVX_SVXIDS_HRC //autogen
#include <bf_svx/svxids.hrc>
#endif


#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#include "wrtsh.hxx"
#include "wrtsh.hrc"

namespace binfilter {

#define C2U(cChar) ::rtl::OUString::createFromAscii(cChar)

using namespace ::com::sun::star;
using namespace ::rtl;

extern sal_Bool bNoInterrupt;       // in mainwn.cxx



/*--------------------------------------------------------------------
    Beschreibung: FontWork-Slots invalidieren
 --------------------------------------------------------------------*/


/*N*/ void SwWrtShell::DrawSelChanged(SdrView* pView)
/*N*/ {
//STRIP001  static sal_uInt16 __READONLY_DATA aInval[] =
//STRIP001 /*N*/     {
//STRIP001 /*N*/         SID_ATTR_FILL_STYLE, SID_ATTR_FILL_COLOR, SID_ATTR_LINE_STYLE,
//STRIP001 /*N*/         SID_ATTR_LINE_WIDTH, SID_ATTR_LINE_COLOR, 0
//STRIP001 /*N*/     };
//STRIP001 /*N*/ 
//STRIP001 /*N*/     GetView().GetViewFrame()->GetBindings().Invalidate(aInval);
//STRIP001 /*N*/ 
//STRIP001 /*N*/     sal_Bool bOldVal = bNoInterrupt;
//STRIP001 /*N*/ /*N*/     bNoInterrupt = sal_True;    // Trick, um AttrChangedNotify ueber Timer auszufuehren
//STRIP001 /*N*/ /*N*/     GetView().AttrChangedNotify(this);
//STRIP001 /*N*/ /*N*/     bNoInterrupt = bOldVal;
/*N*/ }

}
