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



#include "svdhlpln.hxx"
#include "svdio.hxx"



#ifndef _SV_WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif

namespace binfilter {

/*?*/ SvStream& operator<<(SvStream& rOut, const SdrHelpLine& rHL)
/*?*/ {
/*?*/ 	SdrIOHeader aHead(rOut,STREAM_WRITE,SdrIOHlpLID);
/*?*/ 	rOut<<UINT16(rHL.eKind);
/*?*/ 	rOut<<rHL.aPos;
/*?*/ 	return rOut;
/*?*/ }

/*N*/ SvStream& operator>>(SvStream& rIn, SdrHelpLine& rHL)
/*N*/ {
/*N*/ 	SdrIOHeader aHead(rIn,STREAM_READ);
/*N*/ 	UINT16 nDum;
/*N*/ 	rIn>>nDum;
/*N*/ 	rHL.eKind=(SdrHelpLineKind)nDum;
/*N*/ 	rIn>>rHL.aPos;
/*N*/ 	return rIn;
/*N*/ }

/*N*/ void SdrHelpLineList::Clear()
/*N*/ {
/*N*/ 	USHORT nAnz=GetCount();
/*N*/ 	for (USHORT i=0; i<nAnz; i++) {
/*N*/ 		delete GetObject(i);
/*N*/ 	}
/*N*/ 	aList.Clear();
/*N*/ }

/*N*/ void SdrHelpLineList::operator=(const SdrHelpLineList& rSrcList)
/*N*/ {
/*N*/ 	Clear();
/*N*/ 	USHORT nAnz=rSrcList.GetCount();
/*N*/ 	for (USHORT i=0; i<nAnz; i++) {
/*N*/ 		Insert(rSrcList[i]);
/*N*/ 	}
/*N*/ }

/*N*/ SvStream& operator<<(SvStream& rOut, const SdrHelpLineList& rHLL)
/*N*/ {
/*N*/ 	SdrIOHeader aHead(rOut,STREAM_WRITE,SdrIOHLstID);
/*N*/ 	USHORT nAnz=rHLL.GetCount();
/*N*/ 	rOut<<nAnz;
/*N*/ 	for (USHORT i=0; i<nAnz; i++) {
/*N*/ 		rOut<<rHLL[i];
/*N*/ 	}
/*N*/ 	return rOut;
/*N*/ }

/*N*/ SvStream& operator>>(SvStream& rIn, SdrHelpLineList& rHLL)
/*N*/ {
/*N*/ 	SdrIOHeader aHead(rIn,STREAM_READ);
/*N*/ 	rHLL.Clear();
/*N*/ 	USHORT nAnz;
/*N*/ 	rIn>>nAnz;
/*N*/ 	for (USHORT i=0; i<nAnz; i++) {
/*N*/ 		SdrHelpLine* pHL=new SdrHelpLine;
/*N*/ 		rIn>>*pHL;
/*N*/ 		rHLL.aList.Insert(pHL,CONTAINER_APPEND);
/*N*/ 	}
/*N*/ 	return rIn;
/*N*/ }

}
