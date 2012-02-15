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



#ifndef _SVDOOLE2_HXX
#define _SVDOOLE2_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif

#ifndef _SVDORECT_HXX
#include <bf_svx/svdorect.hxx>
#endif
#ifndef _GRAPH_HXX //autogen
#include <vcl/graph.hxx>
#endif
#ifndef _GDIMTF_HXX //autogen
#include <vcl/gdimtf.hxx>
#endif

#include <bf_so3/svstor.hxx>

namespace binfilter {

class SvPersist;
class SvInPlaceObjectRef;
class SvEmbeddedInfoObject;

//************************************************************
//   SdrOle2Obj
//************************************************************

class SvxUnoShapeModifyListener;

class SdrOle2ObjImpl;

class SdrOle2Obj :  public SdrRectObj
{
protected:
	SvInPlaceObjectRef*			ppObjRef;    // Referenz auf Objekt

	String						aName;       // Name des Objektes

	Graphic*					pGraphic;
	String						aProgName;

	// wg. Kompatibilitaet erstmal am SdrTextObj
	BOOL						bFrame : 1;
	BOOL						bInDestruction : 1;

	SdrOle2ObjImpl*				mpImpl;

	SvxUnoShapeModifyListener*	pModifyListener;

protected:

/*NBFF*/	void ImpSetVisAreaSize();

	void Init();

public:
	TYPEINFO();

	SdrOle2Obj(FASTBOOL bFrame_=FALSE);
	SdrOle2Obj(const SvInPlaceObjectRef& rNewObjRef, const String& rNewObjName, const Rectangle& rNewRect, FASTBOOL bFrame_=FALSE);
	virtual ~SdrOle2Obj();

	// Ein OLE-Zeichenobjekt kann eine StarView-Grafik beinhalten.
	// Diese wird angezeigt, wenn das OLE-Objekt leer ist.
	void        SetGraphic(const Graphic* pGrf);
	Graphic*    GetGraphic() const { return pGraphic; }

    // OLE object has got a separate PersistName member now;
    // !!! use ::SetPersistName( ... ) only, if you know what you do !!!
    String      GetPersistName() const;
    void        SetPersistName( const String& rPersistName );


	virtual String  GetName() const;

	// Einem SdrOle2Obj kann man ein Applikationsnamen verpassen, den man
	// spaeter wieder abfragen kann (SD braucht das fuer Praesentationsobjekte).
	void SetProgName(const String& rNam) { aProgName=rNam; }
	const String& GetProgName() const { return aProgName; }
	FASTBOOL IsEmpty() const;
	void SetObjRef(const SvInPlaceObjectRef& rNewObjRef);
	const SvInPlaceObjectRef& GetObjRef() const;

	virtual void SetPage(SdrPage* pNewPage);
	virtual void SetModel(SdrModel* pModel);

    /** Change the IsClosedObj attribute

    	@param bIsClosed
        Whether the OLE object is closed, i.e. has opaque background
     */
    void SetClosedObj( bool bIsClosed );

	virtual UINT16 GetObjIdentifier() const;
	virtual SdrObject* CheckHit(const Point& rPnt, USHORT nTol, const SetOfByte* pVisiLayer) const;




	virtual void NbcMove(const Size& rSize);
	virtual void NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact);
	virtual void NbcSetSnapRect(const Rectangle& rRect);
	virtual void NbcSetLogicRect(const Rectangle& rRect);

	// fuer Import:
	FASTBOOL HasGDIMetaFile() const;
	const GDIMetaFile* GetGDIMetaFile() const;

	virtual void WriteData(SvStream& rOut) const;
	virtual void ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);

	BOOL Unload();

	void Connect();
	void Disconnect();

	::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > getXModel() const;

	/// #110015# sets the visible area at the SvInPlaceObject and SvEmbeddedInfoObject
	void SetVisibleArea( const Rectangle& rArea );
};

}//end of namespace binfilter
#endif //_SVDOOLE2_HXX

