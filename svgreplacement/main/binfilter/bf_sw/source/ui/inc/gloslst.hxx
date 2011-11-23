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



#ifndef _GLOSLST_HXX
#define _GLOSLST_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _DATETIME_HXX
#include <tools/datetime.hxx>
#endif
#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _TIMER_HXX //autogen
#include <vcl/timer.hxx>
#endif
#ifndef _SVARRAY_HXX //autogen
#include <bf_svtools/svarray.hxx>
#endif
class SvStringsISortDtor; 
namespace binfilter {

class SwGlossaries;


struct AutoTextGroup
{
	USHORT 		nCount;
	String 		sName;
	String  	sTitle;
	String		sLongNames;   // durch 0x0A getrennte Langnamen
	String 		sShortNames;  // durch 0x0A getrennte Kurznamen
	DateTime	aDateModified;
};


typedef AutoTextGroup* AutoTextGroupPtr;
SV_DECL_PTRARR(AutoTextGroups, AutoTextGroupPtr, 4, 4)//STRIP008 ;

class SwGlossaryList : public AutoTimer
{
	AutoTextGroups	aGroupArr;
	String			sPath;
	BOOL 			bFilled;
};

} //namespace binfilter
#endif






