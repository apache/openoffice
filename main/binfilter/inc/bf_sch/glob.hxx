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



#ifndef _SCH_GLOB_HXX
#define _SCH_GLOB_HXX

#include <bf_svtools/bf_solar.h>
namespace binfilter {

#define DIA_SLOW	0
#define DIA_MEDIUM	1
#define DIA_FAST	2


#define SCH_IF_APP              SFX_INTERFACE_LIB + 2
#define SCH_IF_CHARTVIEWSHELL   SFX_INTERFACE_LIB + 3
#define SCH_IF_DOCSHELL         SFX_INTERFACE_LIB + 4
#define SCH_IF_STANDARDOBJBAR   SFX_INTERFACE_LIB + 5

#define SCH_IF_SCHAPP SFX_INTERFACE_SCH_START + 0

// Inventor-Ids fuer StarChart

const UINT32 SchInventor = UINT32('S')*0x00000001+
						   UINT32('C')*0x00000100+
						   UINT32('H')*0x00010000+
						   UINT32('U')*0x01000000;

// Object-Ids fuer StarChart

const UINT16 SCH_OBJGROUP_ID		= 1;
const UINT16 SCH_OBJECTID_ID		= 2;
const UINT16 SCH_OBJECTADJUST_ID	= 3;
const UINT16 SCH_DATAROW_ID             = 4;
const UINT16 SCH_DATAPOINT_ID		= 5;
const UINT16 SCH_LIGHTFACTOR_ID		= 6;
const UINT16 SCH_AXIS_ID                = 7;

} //namespace binfilter
#endif // _SCH_GLOB_HXX


