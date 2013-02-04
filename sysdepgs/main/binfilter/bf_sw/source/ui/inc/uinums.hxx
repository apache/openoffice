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


#ifndef _UINUMS_HXX
#define _UINUMS_HXX

#ifndef _SVARRAY_HXX //autogen
#include <bf_svtools/svarray.hxx>
#endif

#ifndef _NUMRULE_HXX
#include <numrule.hxx>
#endif
namespace binfilter {






//------------------------------------------------------------------------
/********************************************************************

********************************************************************/

/********************************************************************

********************************************************************/
class SwChapterNumRules //STRIP001 : public SwBaseNumRules
{

public:
	SwChapterNumRules(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 SwChapterNumRules();
	virtual ~SwChapterNumRules(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual ~SwChapterNumRules();

};

// INLINE METHODE --------------------------------------------------------

} //namespace binfilter
#endif
