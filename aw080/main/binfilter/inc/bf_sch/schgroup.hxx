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



#ifndef _SCH_SCHGROUP_HXX
#define _SCH_SCHGROUP_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVDOGRP_HXX //autogen
#include <bf_svx/svdogrp.hxx>
#endif

#include "chtmodel.hxx"
namespace binfilter {

/************************************************************************/

class SchObjGroup : public SdrObjGroup
{
 public:
	enum ChartGroupTypeNames {LEGEND, DIAGRAM, NOTHING};

 private:
	SdrObjTransformInfoRec aInfo;
	ChartGroupTypeNames eChartGroupType;  // FG: 11.3.97 Wird einfach auf TRUE gesetzt falls es
								   //     das Chart selbst ist. Das muss beim Resize anders
								   //     behandelt werden
	ChartModel *pChartmodel;       // Auch das ist nur fuer das Resize wichtig, um dort
								   // CreateChart() aufzurufen;

	BOOL bAskForLogicRect;         // Da die Groesse eines Gruppenobjektes durch die
								   // Mitlgieder berechnet wird, hilft ein Resize der Gruppe nichts.
								   // Bei BuildChart muss eine Neuberechnung bisweilen verhindert
								   // werden. (z.B falls der Benutzer die Gruppe selbst resized hat)
    bool mbUseChartInventor;

public:

	TYPEINFO();

	SchObjGroup(ChartModel *pChmodel = NULL);
	virtual ~SchObjGroup();

	virtual UINT32 GetObjInventor() const;
	virtual UINT16 GetObjIdentifier() const;



	// FG: Damit soll ermöglicht werden dass man Objektgruppen im Chart resizen kann
	void	SetObjInfo(SdrObjTransformInfoRec aMyInfo);

	// FG: Um das Verhalten bei einem Resize zu aendern muessen beide Routinen
	//     überladen werden.

	// FG: 9.3.1997 Methode von Joe, die Überladen wird um zu Kennzeichnen
	//              ob die Gruppe jemals verschoben worden ist.
	virtual void Move  (const Size& rSiz);

	void SetGroupType (ChartGroupTypeNames value) {eChartGroupType = value;}
	ChartGroupTypeNames GetGroupType() {return eChartGroupType;}
	void SetGroupIsChart() {eChartGroupType = DIAGRAM;}
	BOOL GetGroupIsChart() {return (eChartGroupType == DIAGRAM);}


	void SetModel (ChartModel *pChModel) {pChartmodel = pChModel; SdrObjGroup::SetModel( (SdrModel*) pChModel ); }

	void SetAskForLogicRect(BOOL value) {bAskForLogicRect = value;}
	BOOL GetAskForLogicRect() {return bAskForLogicRect;}

    void SetUseChartInventor( bool bUseChartInventor );
};

} //namespace binfilter
#endif	// _SCH_SCHGROUP_HXX


