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



#ifndef SC_FUDRAW_HXX
#define SC_FUDRAW_HXX

#ifndef _SC_FUPOOR_HXX
#include "fupoor.hxx"
#endif

#ifndef _SV_POINTR_HXX //autogen
#include <vcl/pointr.hxx>
#endif
namespace binfilter {

/*************************************************************************
|*
|* Basisklasse fuer alle Drawmodul-spezifischen Funktionen
|*
\************************************************************************/

class FuDraw : public FuPoor
{
 protected:
	Pointer aNewPointer;
	Pointer aOldPointer;

 public:
	FuDraw(ScTabViewShell* pViewSh, Window* pWin, SdrView* pView,
		   SdrModel* pDoc, SfxRequest& rReq);
	virtual ~FuDraw();






	// #97016# II

};



} //namespace binfilter
#endif		// _SD_FUDRAW_HXX
