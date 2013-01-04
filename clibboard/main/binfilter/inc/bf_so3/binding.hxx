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


#ifndef _BINDING_HXX
#define _BINDING_HXX

#ifndef __RSC

#include <bf_svtools/bf_solar.h>

namespace binfilter {
typedef ULONG SvBindStatus;

typedef ULONG SvStatusCallbackType;
#define SVBSCF_FIRSTDATANOTIFICATION          0x01
#define SVBSCF_LASTDATANOTIFICATION           0x02
#define SVBSCF_INTERMEDIATEDATANOTIFICATION   0x03
#define SVBSCF_RELOADAVAILABLENOTIFICATION    0x04
#define SVBSCF_NEWPARTAVAILABLENOTIFICATION   0x08

typedef ULONG SvBindMode;
#define SVBIND_ASYNC             0x01
#define SVBIND_ASYNCSTORAGE      0x02
#define SVBIND_NEWESTVERSION     0x04
#define SVBIND_SUCCESSONERRORDOC 0x08

enum SvBindAction
{
	BINDACTION_GET    = 0,
	BINDACTION_POST   = 0x1,
	BINDACTION_PUT    = 0x2,
	BINDACTION_CUSTOM = 0x3
};

}

#endif /* __RSC */

#endif /* _BINDING_HXX */
