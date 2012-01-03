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



#ifndef _E3D_LABEL3D_HXX
#define _E3D_LABEL3D_HXX

#ifndef _E3D_PNTOBJ3D_HXX
#include <bf_svx/pntobj3d.hxx>
#endif
namespace binfilter {

class Viewport3D;
class E3dScene;
class E3dLabelObj;

/************************************************************************/

class E3dLabelObj;
DECLARE_LIST(E3dLabelList, E3dLabelObj*)//STRIP008 DECLARE_LIST(E3dLabelList, E3dLabelObj*);

/*************************************************************************
|*
|* Objekt zur Verknuepfung eines 2D-Labels mit einer 3D-Position.
|* Bei einer Transformation wird die neue Position des 2D-Objekts
|* mit SetAnchorPos gesetzt. Wenn das Objekt zu dieser Position
|* ausgerichtet werden soll (z.B. rechtsbuendig), kann das mittels
|* SetRelativePos erreicht werden.
|*
\************************************************************************/

class E3dLabelObj : public E3dPointObj
{
 // damit nicht jeder daran rumpfuschen kann...
 friend class E3dScene;
	SdrObject* GetMy2DLabelObj() { return p2DLabelObj; }

 protected:
	SdrObject* p2DLabelObj;

 public:
	TYPEINFO();
	// Eigentumsuebergang bei pLabelObj!
	E3dLabelObj(const Vector3D& aPos,
				SdrObject*      pLabelObj) :
		E3dPointObj(aPos),
		p2DLabelObj(pLabelObj)
	{
	}

	E3dLabelObj() :
		E3dPointObj(),
		p2DLabelObj(NULL)
	{
	}

	virtual ~E3dLabelObj();

	virtual void SetPage(SdrPage* pNewPage);
	virtual void SetModel(SdrModel* pNewModel);

	virtual UINT16 GetObjIdentifier() const;


	const SdrObject* Get2DLabelObj() const { return p2DLabelObj; }

	virtual void WriteData(SvStream& rOut) const;
	virtual void ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);

	// TakeObjName...() ist fuer die Anzeige in der UI, z.B. "3 Rahmen selektiert".

};

}//end of namespace binfilter
#endif			// _E3D_LABEL3D_HXX
