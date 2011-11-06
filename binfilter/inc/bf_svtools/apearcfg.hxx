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


#ifndef _SVT_APEARCFG_HXX
#define _SVT_APEARCFG_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#include <bf_svtools/bf_solar.h>
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif

class Application;

namespace binfilter {

/*--------------------------------------------------------------------
	 Beschreibung:
 --------------------------------------------------------------------*/
typedef enum {
	LookStardivision = 0,
	LookMotif,
	LookWindows,
	LookOSTwo,
	LookMacintosh
} SystemLook;

typedef enum {
    SnapToButton = 0,
    SnapToMiddle,
    NoSnap
} SnapType;

typedef enum { // MUST match the order chosen in ListBox LB_DRAG_MODE in optgdlg.src
	DragFullWindow,
	DragFrame,
	DragSystemDep
} DragMode;

}

#endif //  _OFA_APEARCFG_HXX
