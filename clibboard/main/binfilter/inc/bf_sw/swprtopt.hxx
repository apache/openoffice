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



#ifndef _SWPRTOPT_HXX
#define _SWPRTOPT_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SV_MULTISEL_HXX //autogen
#include <tools/multisel.hxx>
#endif
#ifndef _SW_PRINTDATA_HXX
#include <printdata.hxx>
#endif
namespace binfilter {

#define POSTITS_NONE	0
#define POSTITS_ONLY	1
#define POSTITS_ENDDOC	2
#define POSTITS_ENDPAGE 3

class SwPrtOptions : public SwPrintData
{
	USHORT nJobNo;
	String sJobName;

public:
	SwPrtOptions( const String& rJobName ) : aOffset( Point(0,0) ),
		nMergeCnt( 0 ), nMergeAct( 0 ), sJobName( rJobName ),
        nJobNo( 1 ),  bCollate(FALSE), bPrintSelection (FALSE),
        bJobStartet(FALSE)
	{}

	const String& GetJobName() const 	{ return sJobName; }

#if defined(TCPP)
	// seit neuestem (SV 223) kann der keinen mehr generieren
	inline	SwPrtOptions(const SwPrtOptions& rNew) {*this = rNew;}
#endif

	MultiSelection	aMulti;
	Point  aOffset;
	ULONG  nMergeCnt;			// Anzahl der Serienbriefe
	ULONG  nMergeAct;			// Aktueller Serienbriefnr.
	USHORT nCopyCount;

    BOOL   bCollate,
           bPrintSelection,     // Markierung drucken
		   bJobStartet;

        SwPrtOptions& operator=(const SwPrintData& rData)
            {
				SwPrintData::operator=(rData);
                return *this;
            }
};


} //namespace binfilter
#endif //_SWPRTOPT_HXX















