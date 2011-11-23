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


#ifndef _LINETYPE_HXX
#define _LINETYPE_HXX

#include <bf_svtools/bf_solar.h>
namespace binfilter {

/*--------------------------------------------------------------------
	 Beschreibung: Ein Linientyp wird durch die 3 Werte festgelegt
 --------------------------------------------------------------------*/

enum SwLineWidth
{
	LIN_0	= 0,
	LIN_1 	= 1,
	LIN_2	= 20,
	LIN_3	= 50,
	LIN_4	= 80,
	LIN_5	= 100
};

struct SwLineType
{
	USHORT nIn;
	USHORT nOut;
	USHORT nDist;
};

USHORT			   GetLineTypeCount();
const SwLineType&  GetLineType(USHORT nIdx);

} //namespace binfilter
#endif // _LINETYPE_HXX
