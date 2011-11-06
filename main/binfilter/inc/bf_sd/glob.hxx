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



#ifndef _SD_GLOB_HXX
#define _SD_GLOB_HXX

#include <bf_svtools/bf_solar.h>

namespace binfilter {

const UINT32 SdUDInventor=UINT32('S')*0x00000001+
						  UINT32('D')*0x00000100+
						  UINT32('U')*0x00010000+
						  UINT32('D')*0x01000000;

#define SD_ANIMATIONINFO_ID 1
#define SD_IMAPINFO_ID		2

#define SD_LT_FAMILY (SfxStyleFamily)0xaffe

#define SD_LT_SEPARATOR "~LT~"

}
#endif

