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


#ifndef _DFLYOBJ_HXX
#define	_DFLYOBJ_HXX

#ifndef _SVDOVIRT_HXX //autogen
#include <bf_svx/svdovirt.hxx>
#endif
#ifndef _SVDOBJ_HXX //autogen
#include <bf_svx/svdobj.hxx>
#endif
namespace binfilter {

class SwFlyFrm;
class SwFrmFmt;
class SdrObjMacroHitRec;

const UINT32 SWGInventor =	UINT32('S')*0x00000001+
							UINT32('W')*0x00000100+
							UINT32('G')*0x00010000;

const UINT16 SwFlyDrawObjIdentifier = 0x0001;
const UINT16 SwDrawFirst			= 0x0001;

//---------------------------------------
//SwFlyDrawObj, Die DrawObjekte fuer Flys.

class SwFlyDrawObj : public SdrObject
{
	SfxItemSet*					mpLocalItemSet;

public:
	TYPEINFO();

	SwFlyDrawObj();
	~SwFlyDrawObj();


	// ItemSet access

	//Damit eine Instanz dieser Klasse beim laden erzeugt werden kann
	//(per Factory).
	virtual UINT32 GetObjInventor()		const;
	virtual UINT16 GetObjIdentifier()	const;
};

//---------------------------------------
//SwVirtFlyDrawObj, die virtuellen Objekte fuer Flys.
//Flys werden immer mit virtuellen Objekten angezeigt. Nur so koennen sie
//ggf. mehrfach angezeigt werden (Kopf-/Fusszeilen).

class SwVirtFlyDrawObj : public SdrVirtObj
{
	SwFlyFrm *pFlyFrm;

public:
	TYPEINFO();

	SwVirtFlyDrawObj(SdrObject& rNew, SwFlyFrm* pFly);
	~SwVirtFlyDrawObj();

	//Ueberladene Methoden der Basisklasse SdrVirtObj

	//Wir nehemen die Groessenbehandlung vollstaendig selbst in die Hand.
	virtual const Rectangle& GetBoundRect() const;
	virtual const Rectangle& GetSnapRect()	const;

		  SwFrmFmt *GetFmt();

	// Get Methoden fuer die Fly Verpointerung
		  SwFlyFrm* GetFlyFrm()			{ return pFlyFrm; }
	const SwFlyFrm* GetFlyFrm() const	{ return pFlyFrm; }

	void SetRect() const;
	void _SetRectsDirty()				{ SetRectsDirty(); }

	// ist eine URL an einer Grafik gesetzt, dann ist das ein Makro-Object
};


} //namespace binfilter
#endif
