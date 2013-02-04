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


#ifndef _DVIEW_HXX
#define	_DVIEW_HXX


#ifndef _SVX_FMVIEW_HXX //autogen
#include <bf_svx/fmview.hxx>
#endif
class OutputDevice;
namespace binfilter {


class SwViewImp;

class SwDrawView : public FmFormView
{
	//Fuer den Anker
	Point			aAnchorPoint;		//Ankerposition
	SwViewImp	   &rImp;				//Die View gehoert immer zu einer Shell


protected:
	// add custom handles (used by other apps, e.g. AnchorPos)
    virtual void AddCustomHdl();

public:
	SwDrawView( SwViewImp &rI, SdrModel *pMd, OutputDevice* pOutDev=NULL );

	//aus der Basisklasse
	virtual void         MarkListHasChanged();

	// #i7672#
	// Overload to resue edit background color in active text edit view (OutlinerView)
	virtual void ModelHasChanged();


	const SwViewImp &Imp() const { return rImp; }
		  SwViewImp &Imp()		 { return rImp; }
};


} //namespace binfilter
#endif

