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



#ifndef _SCH_FUSEL_HXX
#define _SCH_FUSEL_HXX


#ifndef _SCH_FUDRAW_HXX
#include "fudraw.hxx"
#endif
namespace binfilter {

class SdrDragMethod;
class SdrCircObj;

/*************************************************************************
|*
|* Basisklasse fuer alle Funktionen
|*
\************************************************************************/

class SchFuSelection : public SchFuDraw
{
protected:
	BOOL	bWasSelected;
	BOOL	bVCAction;
	SdrDragMethod* pDragMethod;


public:

	SchFuSelection(SchViewShell* pViewSh, SchWindow* pWin, SchView* pView,
				   ChartModel* pDoc, SfxRequest& rReq);

	virtual ~SchFuSelection();


	virtual void Activate();
	virtual void Deactivate();
};

} //namespace binfilter
#endif	// _SCH_FUSEL_HXX


