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



#ifndef SC_CELLFORM_HXX
#define SC_CELLFORM_HXX

#include <bf_svtools/bf_solar.h>
class String;
class Color;
namespace binfilter {

class SvNumberFormatter;

class ScBaseCell;

enum ScForceTextFmt {
	ftDontForce,			// Zahlen als Zahlen
	ftForce,				// Zahlen als Text
	ftCheck					// ist das Zahlenformat ein Textformat?
};

//------------------------------------------------------------------------

class ScCellFormat
{
public:
	static void		GetString( ScBaseCell* pCell, ULONG nFormat, String& rString,
							   Color** ppColor, SvNumberFormatter& rFormatter,
							   BOOL bNullVals = TRUE,
							   BOOL bFormula  = FALSE,
							   ScForceTextFmt eForceTextFmt = ftDontForce );

	static void		GetInputString( ScBaseCell* pCell, ULONG nFormat, String& rString,
									  SvNumberFormatter& rFormatter );
};




} //namespace binfilter
#endif

