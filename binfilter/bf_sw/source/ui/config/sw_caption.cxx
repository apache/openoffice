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


#include "numrule.hxx"
#include "caption.hxx"
namespace binfilter {

#define VERSION_01 		1
#define CAPTION_VERSION VERSION_01

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

/*N*/ InsCaptionOpt::InsCaptionOpt(const SwCapObjType eType, const SvGlobalName* pOleId) :
/*N*/ 	bUseCaption(FALSE),
/*N*/ 	eObjType(eType),
/*N*/ 	nNumType(SVX_NUM_ARABIC),
/*N*/ 	nPos(1),
/*N*/ 	nLevel(0),
/*N*/ 	cSeparator('.'),
/*N*/ 	bIgnoreSeqOpts(FALSE),
/*N*/ 	bCopyAttributes(FALSE)
/*N*/ {
/*N*/ 	if (pOleId)
/*N*/ 		aOleId = *pOleId;
/*N*/ }

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/

/*N*/ InsCaptionOpt::~InsCaptionOpt()
/*N*/ {
/*N*/ }

/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


/*--------------------------------------------------------------------
	Beschreibung:
 --------------------------------------------------------------------*/


/*************************************************************************
|*
|*    InsCaptionOpt::operator>>()
|*
|*    Beschreibung      Stream-Leseoperator
|*
*************************************************************************/


/*************************************************************************
|*
|*    InsCaptionOpt::operator<<()
|*
|*    Beschreibung      Stream-Schreiboperator
|*
*************************************************************************/


}
