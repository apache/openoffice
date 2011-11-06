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



#ifndef _SVDOGRAF_HXX
#define _SVDOGRAF_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SV_GRAPH_HXX //autogen
#include <vcl/graph.hxx>
#endif
#ifndef _SVDORECT_HXX
#include <bf_svx/svdorect.hxx>
#endif
#ifndef _BF_GOODIES_GRAPHICOBJECT_HXX
#include <bf_goodies/graphicobject.hxx>
#endif
namespace binfilter {
class BfGraphicObject;

// -----------
// - Defines -
// -----------

#define SDRGRAFOBJ_TRANSFORMATTR_NONE		0x00000000UL
#define SDRGRAFOBJ_TRANSFORMATTR_COLOR		0x00000001UL
#define SDRGRAFOBJ_TRANSFORMATTR_MIRROR		0x00000002UL
#define SDRGRAFOBJ_TRANSFORMATTR_ROTATE		0x00000004UL
#define SDRGRAFOBJ_TRANSFORMATTR_ALL		0xffffffffUL

// ---------------------
// - SdrGrafObjGeoData -
// ---------------------

class Animation;

class SdrGrafObjGeoData : public SdrRectObjGeoData
{
public:

	FASTBOOL	bMirrored;

				SdrGrafObjGeoData() :
					bMirrored( FALSE ) {}
};

// --------------
// - SdrGrafObj -
// --------------

class SdrGrafSetItem;
class SdrGraphicLink;

class SdrGrafObj: public SdrRectObj
{

	friend class SdrGraphicLink;

protected:

	void					ImpSetAttrToGrafInfo(); // Werte vom Pool kopieren
	void					ImpSetGrafInfoToAttr(); // Werte in den Pool kopieren
	BfGraphicAttr			aGrafInfo;
	FASTBOOL				bCopyToPoolOnAfterRead;

	String					aName;

	Rectangle				aCropRect;			// Wenn aCropRect nicht Empty ist, dann enthaelt es den sichtbaren
												// Ausschnitt der Grafik in logischen Eingeiten der Grafik! Also Bitmap->=Pixel
	String					aFileName;			// Wenn es sich um einen Link handelt, steht hier der Dateiname drin.
	String					aFilterName;
	BfGraphicObject*			pGraphic;			// Zur Beschleunigung von Bitmapausgaben, besonders von gedrehten.
	void*					pReserve1;
	void*					pReserve2;
	void*					pReserve3;
	void*					pReserve4;
	void*					pReserve5;
	SdrGraphicLink*			pGraphicLink;		// Und hier noch ein Pointer fuer gelinkte Grafiken
	FASTBOOL				bMirrored;			// True bedeutet, die Grafik ist horizontal, d.h. ueber die Y-Achse gespiegelt auszugeben.
	ULONG					nGrafStreamPos;
	FASTBOOL				bDummy1:1;
	FASTBOOL				bDummy2:1;

#if _SOLAR__PRIVATE

	void					ImpLinkAnmeldung();
	void					ImpLinkAbmeldung();
	void					ImpPaintReplacement(OutputDevice* pOutDev, const XubString& rText, const Bitmap* pBmp, FASTBOOL bFill) const;

	sal_Bool				ImpUpdateGraphicLink() const;

							DECL_LINK( ImpSwapHdl, BfGraphicObject* );

#endif // __PRIVATE

public:

							TYPEINFO();

							SdrGrafObj();
							SdrGrafObj(const Graphic& rGrf);
	virtual					~SdrGrafObj();

	void					SetGraphicObject( const BfGraphicObject& rGrfObj );
	const BfGraphicObject&	GetGraphicObject() const;

	void					SetGraphic(const Graphic& rGrf);
	const Graphic&			GetGraphic() const;

	GraphicType				GetGraphicType() const;

	void					SetGrafStreamURL( const String& rGraphicStreamURL );
	String					GetGrafStreamURL() const;

	void					ForceSwapIn() const;
	void					ForceSwapOut() const;

	void					SetGraphicLink(const String& rFileName, const String& rFilterName);
	void					ReleaseGraphicLink();
	FASTBOOL				IsLinkedGraphic() const { return (BOOL)aFileName.Len(); }

	const String&			GetFileName() const { return aFileName; }
	const String&			GetFilterName() const { return aFilterName; }



	virtual void			SetName(const String& rStr);
	virtual String			GetName() const;

	virtual UINT16			GetObjIdentifier() const;
	virtual FASTBOOL		Paint(ExtOutputDevice& rOut, const SdrPaintInfoRec& rInfoRec) const;
	virtual SdrObject*		CheckHit(const Point& rPnt, USHORT nTol, const SetOfByte* pVisiLayer) const;


	virtual void			operator=(const SdrObject& rObj);

	virtual void			NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact);
	virtual void			NbcRotate(const Point& rRef, long nWink, double sn, double cs);
	virtual void			NbcSetSnapRect(const Rectangle& rRect);
	virtual void			NbcSetLogicRect(const Rectangle& rRect);
	virtual SdrObjGeoData*	NewGeoData() const;
	virtual void			SaveGeoData(SdrObjGeoData& rGeo) const;
	virtual void			RestartAnimation(SdrPageView* pPageView) const;

	virtual void			SetPage(SdrPage* pNewPage);
	virtual void			SetModel(SdrModel* pNewModel);
	virtual void			WriteData(SvStream& rOut) const;
	virtual void			ReadData(const SdrObjIOHeader& rHead, SvStream& rIn);
	void					ReadDataTilV10(const SdrObjIOHeader& rHead, SvStream& rIn);


	void 					AdjustToMaxRect( const Rectangle& rMaxRect, BOOL bShrinkOnly );

	virtual void			SFX_NOTIFY( SfxBroadcaster& rBC, const TypeId& rBCType,
										const SfxHint& rHint, const TypeId& rHintType );
	virtual void			ForceDefaultAttr();
	virtual void			NbcSetStyleSheet( SfxStyleSheet* pNewStyleSheet, FASTBOOL bDontRemoveHardAttr );

	// ItemSet access
	virtual SfxItemSet* CreateNewItemSet(SfxItemPool& rPool);

	// private support routines for ItemSet access. NULL pointer means clear item.
	virtual void ItemSetChanged(const SfxItemSet& rSet);

	// pre- and postprocessing for objects for saving
	virtual void PreSave();
	virtual void PostSave();

	virtual void			AfterRead();

	BOOL					IsMirrored() { return bMirrored; }
	void					SetMirrored( sal_Bool _bMirrored ) { bMirrored = _bMirrored; }
};

}//end of namespace binfilter
#endif //_SVDOGRAF_HXX

