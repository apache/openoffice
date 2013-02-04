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



#ifndef _CHTSCENE_HXX
#define _CHTSCENE_HXX

#include <bf_svtools/bf_solar.h>

#include <bf_svx/polysc3d.hxx>
#include <chtmodel.hxx>
namespace binfilter {

/*************************************************************************
|*
|* Basisklasse fuer 3D-Szenen
|* Die Ableitung von SchObjGroup erfolgt damit das Chart frei plaziert
|* werden kann. Die Komponenten von 2d-Charts sind auch SchObjGroups
|*
\************************************************************************/

//DECLARE_LIST(DescrList, E3dLabelObj*)//STRIP008 //DECLARE_LIST(DescrList, E3dLabelObj*);

class ChartScene : public E3dPolyScene
{
  protected:
	ChartModel* pDoc;
	BOOL        bAskForLogicRect;
	Rectangle   Get3DDescrRect(E3dLabelObj *p3DObj,B3dCamera& rCamSet);

  public:

	TYPEINFO();
	ChartScene(ChartModel* pModel);
	virtual ~ChartScene();
    void Initialize();

	virtual Volume3D FitInSnapRect();

	BOOL GetAskForLogicRect() {return bAskForLogicRect;}
	void SetAskForLogicRect(BOOL value) {bAskForLogicRect = value;}

	virtual void WriteData(SvStream& rOut) const;

	void InsertAllTitleText (DescrList         &rList,
							 E3dObject         *pGroup,
							 long              nAxisId);

    // just call parent? overloading deprecated?
	//	The following three methods call their respective equivalents of
	//	E3dObject.  This is necessary because they are overloaded in E3dScene
	//	with methods that do not what we want :-)
//  	virtual	USHORT	GetHdlCount	(void)	const
//  		{	return E3dObject::GetHdlCount();	}
//  	virtual	void	AddToHdlList	(SdrHdlList& rHdlList)	const
//  		{	E3dObject::AddToHdlList (rHdlList);	}
//  	virtual FASTBOOL	HasSpecialDrag	(void)	const
//  		{	return E3dObject::HasSpecialDrag ();	}

    void ReduceDescrList(DescrList& aList);
};


} //namespace binfilter
#endif			// _E3D_SCENE3D_HXX
