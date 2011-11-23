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

#ifndef _DOCSTAT_HXX
#include <docstat.hxx>
#endif
namespace binfilter {


/************************************************************************
 *						   SwDocStat::SwDocStat()
 ************************************************************************/

/*N*/ SwDocStat::SwDocStat() :
/*N*/ 	nTbl(0),
/*N*/ 	nGrf(0),
/*N*/ 	nOLE(0),
/*N*/ 	nPage(1),
/*N*/ 	nPara(1),
/*N*/ 	nWord(0),
/*N*/ 	nChar(0),
/*N*/ 	bModified(TRUE),
/*N*/ 	pInternStat(0)
/*N*/ {}

/************************************************************************
 *						   void SwDocStat::Reset()
 ************************************************************************/

/*N*/ void SwDocStat::Reset()
/*N*/ {
/*N*/ 	nTbl 	= 0;
/*N*/ 	nGrf 	= 0;
/*N*/ 	nOLE 	= 0;
/*N*/ 	nPage   = 1;
/*N*/ 	nPara   = 1;
/*N*/ 	nWord 	= 0;
/*N*/ 	nChar	= 0;
/*N*/ 	bModified = TRUE;
/*N*/ 	pInternStat = 0;
/*N*/ }

}
