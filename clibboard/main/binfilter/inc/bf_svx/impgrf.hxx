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


#ifndef _SVX_IMPGRF_HXX
#define _SVX_IMPGRF_HXX

// include ---------------------------------------------------------------

#ifndef _FILTER_HXX //autogen
#include <bf_svtools/filter.hxx>
#endif

namespace binfilter {

// Funktionen ------------------------------------------------------------

// returnt einen static Graphic-Filter, wird einmalig angelegt,
// steht immer zur Verfuegung, DARF NIE geloescht werden!!!!
GraphicFilter* GetGrfFilter();
USHORT	FillFilter( GraphicFilter& rFilter );

// class SvxImportGraphicDialog ------------------------------------------
#ifndef SV_NODIALOG

#define ENABLE_STANDARD			((USHORT)0x0001)	// Standard-Button
#define ENABLE_LINK				((USHORT)0x0002)	// Verkn"upfungs-Box
#define ENABLE_STD_AND_LINK		(ENABLE_STANDARD | ENABLE_LINK)
#define ENABLE_PROPERTY			((USHORT)0x0004)	// Eigenschaften-Button
#define ENABLE_ALL				((USHORT)0x0007)	// alle
#define ENABLE_PROP_WITHOUTLINK ((USHORT)0x0008)	// Eigenschaften ohne Link
#define ENABLE_EMPTY_FILENAMES	((USHORT)0x0010)	// Leere Dateinamen zulassen

#endif	// SV_NODIALOG

}//end of namespace binfilter
#endif
