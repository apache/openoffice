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

#ifndef _EEITEM_HXX
#include "eeitem.hxx"
#endif
#ifndef _EEITEMID_HXX
#include "eeitemid.hxx"
#endif





#ifndef _SV_WINDOW_HXX
#include <vcl/window.hxx>
#endif

#include <editdoc.hxx>
namespace binfilter {

#if defined( DBG_UTIL ) || ( OSL_DEBUG_LEVEL > 1 )









/*N*/ BOOL CheckOrderedList( CharAttribArray& rAttribs, BOOL bStart )
/*N*/ {
/*N*/ 	USHORT nPrev = 0;
/*N*/ 	for ( USHORT nAttr = 0; nAttr < rAttribs.Count(); nAttr++ )
/*N*/ 	{
/*N*/ 		EditCharAttrib* pAttr = rAttribs[nAttr];
/*N*/ 		USHORT nCur = bStart ? pAttr->GetStart() : pAttr->GetEnd();
/*N*/ 		if ( nCur < nPrev )
/*N*/ 			return FALSE;
/*N*/ 
/*N*/ 		nPrev = nCur;
/*N*/ 	}
/*N*/ 	return TRUE;
/*N*/ }

#endif

}
