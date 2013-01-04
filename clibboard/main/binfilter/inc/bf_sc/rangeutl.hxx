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



#ifndef SC_RANGEUTL_HXX
#define SC_RANGEUTL_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
class SvStream;
namespace binfilter {

//------------------------------------------------------------------------


class ScArea;
class ScDocument;
class ScRefTripel;
class ScTripel;
class ScRange;
class ScRangeName;
class ScDBCollection;

enum RutlNameScope { RUTL_NONE=0, RUTL_NAMES, RUTL_DBASE };

//------------------------------------------------------------------------

class ScRangeUtil
{
public:
				ScRangeUtil()  {}
				~ScRangeUtil() {}



	void	CutPosString		( const String&		theAreaStr,
								  String&			thePosStr ) const;


	BOOL	IsAbsArea			( const String& rAreaStr,
								  ScDocument*	pDoc,
								  USHORT		nTab,
								  String*		pCompleteStr = 0,
								  ScRefTripel*	pStartPos	 = 0,
								  ScRefTripel*	pEndPos		 = 0 ) const;


	BOOL	MakeRangeFromName	( const String&	rName,
									ScDocument*		pDoc,
									USHORT			nCurTab,
									ScRange&		rRange,
									RutlNameScope eScope=RUTL_NAMES
								  ) const;
};

//------------------------------------------------------------------------

class ScArea
{
public:
			ScArea( USHORT tab      = 0,
					USHORT colStart = 0,
					USHORT rowStart = 0,
					USHORT colEnd   = 0,
					USHORT rowEnd   = 0 );

			ScArea( const ScArea& r );

	ScArea&	operator=	( const ScArea& r );

public:
	USHORT nTab;
	USHORT nColStart;
	USHORT nRowStart;
	USHORT nColEnd;
	USHORT nRowEnd;
};

/*N#116571#*/ SvStream& operator<< ( SvStream& rStream, const ScArea& rArea );
/*N#116571#*/ SvStream& operator>> ( SvStream& rStream, ScArea& rArea );

//------------------------------------------------------------------------

//
//	gibt Bereiche mit Referenz und alle DB-Bereiche zurueck
//



} //namespace binfilter
#endif // SC_RANGEUTL_HXX

