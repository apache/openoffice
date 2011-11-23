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


#ifndef _FLYFRM_HXX
#define _FLYFRM_HXX

#include <bf_svtools/bf_solar.h>

#include "layfrm.hxx"
#include <orntenum.hxx>

class PolyPolygon;
namespace binfilter {
class SwPageFrm;
class SwFlyFrmFmt;
class SwFmtFrmSize;
struct SwCrsrMoveState;
class SwBorderAttrs;
class SwVirtFlyDrawObj;
class SwSpzFrmFmts;
class SwAttrSetChg;



//Sucht ausgehend von pOldAnch einen Anker fuer Absatzgebundene Rahmen.
//Wird beim Draggen von Absatzgebundenen Objekten zur Ankeranzeige sowie
//fuer Ankerwechsel benoetigt.
//implementiert in layout/flycnt.cxx
const SwCntntFrm *FindAnchor( const SwFrm *pOldAnch, const Point &rNew,
							  const BOOL bBody = FALSE );

// berechnet das Rechteck, in dem das Objekt bewegt bzw. resized werden darf
BOOL CalcClipRect( const SdrObject *pSdrObj, SwRect &rRect, BOOL bMove = TRUE );

//allg. Basisklasse fuer alle Freifliegenden Rahmen
class SwFlyFrm : public SwLayoutFrm
{
	//darf Locken. Definiert in frmtool.cxx
	friend void AppendObjs   ( const SwSpzFrmFmts *, ULONG, SwFrm *, SwPageFrm * );
	friend void AppendAllObjs( const SwSpzFrmFmts * );
	friend void Notify( SwFlyFrm *, SwPageFrm *pOld, const SwRect &rOld );
	//darf die Pos berechnen (lassen)
	friend void lcl_MakeFlyPosition( SwFlyFrm *pFly );

	void InitDrawObj( BOOL bNotify );	//Wird von den CToren gerufen.
	void FinitDrawObj();				//Wird vom CTor gerufen.

	void _UpdateAttr( SfxPoolItem*, SfxPoolItem*, BYTE &,
					  SwAttrSetChg *pa = 0, SwAttrSetChg *pb = 0 );

protected:

	SwVirtFlyDrawObj *pDrawObj;	// das Drawingobject zum Fly
	SwFrm *pAnchor;				// An diesem LayoutFrm haengt der Frm, kann 0 sein

	SwFlyFrm *pPrevLink,        // Vorgaenger/Nachfolger fuer Verkettung mit
			 *pNextLink;		// Textfluss

	Point aRelPos;	 //Die Relative Position zum Master

private:
	BOOL bLocked	:1;	//Cntnt-gebundene Flys muessen derart blockiert werden
						//koennen, dass sie nicht Formatiert werden; :MakeAll
						//returnt dann sofort. Dies ist bei Seitenwechseln
						//waehrend der Formatierung notwendig.
						//Auch wahrend des RootCTors ist dies notwendig da
						//sonst der Anker formatiert wird obwohl die Root noch
						//nicht korrekt an der Shell haengt und weil sonst
						//initial zuviel Formatiert wuerde.
	BOOL bNotifyBack:1;	//TRUE wenn am Ende eines MakeAll() der Background
						//vom NotifyDTor benachrichtigt werden muss.
protected:

	BOOL bInvalid :1; 	//Pos, PrtArea od. SSize wurden Invalidiert, sie werden
						//gleich wieder Validiert, denn sie muessen _immer_
						//gueltig sein. Damit in LayAction korrekt gearbeitet
						//werden kann muss hier festgehalten werden, dass sie
						//invalidiert wurden. Ausnahmen bestaetigen die Regelt!
	BOOL bMinHeight:1;	//TRUE wenn die vom Attribut vorgegebene Hoehe eine
						//eine Minimalhoehe ist (der Frm also bei Bedarf
						//darueberhinaus wachsen kann).
	BOOL bHeightClipped	:1; //TRUE wenn der Fly nicht die Pos/Size anhand der Attrs
	BOOL bWidthClipped	:1;	//formatieren konnte, weil z.B. nicht genug Raum vorh.
							//war.
	BOOL bFormatHeightOnly	:1; //Damit nach einer Anpassung der Breite
								//(CheckClip) nur das Format aufgerufen wird;
								//nicht aber die Breite anhand der Attribute
								//wieder bestimmt wird.
	BOOL bInCnt			:1; // FLY_IN_CNTNT, als Zeichen verankert
	BOOL bAtCnt			:1; // FLY_AT_CNTNT, am Absatz verankert
	BOOL bLayout   		:1; // FLY_PAGE, FLY_AT_FLY, an Seite oder Rahmen
	BOOL bAutoPosition	:1; // FLY_AUTO_CNTNT, im Text verankerter Rahmen
    BOOL bNoShrink      :1; // temporary forbud of shrinking to avoid loops

	friend class SwNoTxtFrm; // Darf NotifyBackground rufen
	virtual void NotifyBackground( SwPageFrm *pPage,
								   const SwRect& rRect, PrepareHint eHint) = 0;

	//Wird nur von SwXFrm::MakeAll() gerufen; wer es anders macht wird
	//in einen Sack gesteckt und muss zwei Tage drin hocken bleiben.
	virtual void MakeFlyPos();
	virtual void Format( const SwBorderAttrs *pAttrs = 0 );
	void MakePrtArea( const SwBorderAttrs &rAttrs );

	void Lock()			{ bLocked = TRUE; }
	void Unlock()		{ bLocked = FALSE; }

	void SetMinHeight()	 { bMinHeight = TRUE; }
	void ResetMinHeight(){ bMinHeight = FALSE; }

	Size CalcRel( const SwFmtFrmSize &rSz ) const;

	SwFlyFrm( SwFlyFrmFmt*, SwFrm *pAnchor );
public:

	virtual ~SwFlyFrm();
	virtual	void Modify( SfxPoolItem*, SfxPoolItem* );
		// erfrage vom Client Informationen
	virtual BOOL GetInfo( SfxPoolItem& ) const;
	virtual void ChgSize( const Size& aNewSize );

	virtual void Cut();
#ifdef DBG_UTIL
#endif

    SwTwips _Shrink( SwTwips, SZPTR BOOL bTst );
    SwTwips _Grow  ( SwTwips, SZPTR BOOL bTst );
	void 	_Invalidate( SwPageFrm *pPage = 0 );

	BOOL FrmSizeChg( const SwFmtFrmSize & );

	const SwFrm	*GetAnchor() const { return pAnchor; }
		  SwFrm	*GetAnchor() { return pAnchor; }
		  void	 ChgAnchor( SwFrm *pNew ) { pAnchor = pNew; }

	SwFlyFrm *GetPrevLink() { return pPrevLink; }
	SwFlyFrm *GetNextLink() { return pNextLink; }



	const SwVirtFlyDrawObj *GetVirtDrawObj() const { return pDrawObj; }
		  SwVirtFlyDrawObj *GetVirtDrawObj() 	   { return pDrawObj; }
	void NotifyDrawObj();

	const Point& GetCurRelPos() const { return aRelPos; }
	BOOL IsInvalid() const { return bInvalid; }
	void Invalidate() const { ((SwFlyFrm*)this)->bInvalid = TRUE; }
	void Validate() const { ((SwFlyFrm*)this)->bInvalid = FALSE; }

	BOOL IsMinHeight()	const { return bMinHeight; }
	BOOL IsLocked()		const { return bLocked; }
	BOOL IsAutoPos()	const { return bAutoPosition; }
	BOOL IsFlyInCntFrm() const { return bInCnt; }
	BOOL IsFlyFreeFrm() const { return bAtCnt || bLayout; }
	BOOL IsFlyLayFrm() const { return bLayout; }
	BOOL IsFlyAtCntFrm() const { return bAtCnt; }

	BOOL IsNotifyBack()	const { return bNotifyBack; }
	void SetNotifyBack()	  { bNotifyBack = TRUE; }
	void ResetNotifyBack()	  { bNotifyBack = FALSE; }
    BOOL IsNoShrink()   const { return bNoShrink; }
    void SetNoShrink( BOOL bNew ) { bNoShrink = bNew; }

	BOOL IsClipped()		const	{ return bHeightClipped || bWidthClipped; }
	BOOL IsHeightClipped()	const	{ return bHeightClipped; }
	BOOL IsWidthClipped()	const	{ return bWidthClipped;  }

	BOOL IsLowerOf( const SwLayoutFrm *pUpper ) const;
	inline BOOL IsUpperOf( const SwFlyFrm *pLower ) const;


	SwRect AddSpacesToFrm() const;

    // OD 16.04.2003 #i13147# - add parameter <_bForPaint> to avoid load of
    // the graphic during paint. Default value: sal_False
    BOOL GetContour( PolyPolygon&   rContour,
            const sal_Bool _bForPaint = sal_False ) const;

    BOOL ConvertHoriTo40( SwHoriOrient &rHori, SwRelationOrient &rRel, SwTwips &rPos ) const;

	//Auf dieser Shell painten (PreView, Print-Flag usw. rekursiv beachten)?.

    /** SwFlyFrm::IsBackgroundTransparent - for feature #99657#

        OD 12.08.2002
        determines, if background of fly frame has to be drawn transparent
        definition found in /core/layout/paintfrm.cxx

        @author OD

        @return true, if background color is transparent or a existing background
        graphic is transparent.
    */
    sal_Bool IsBackgroundTransparent() const;
};

inline BOOL SwFlyFrm::IsUpperOf( const SwFlyFrm *pLower ) const
{
	return pLower->IsLowerOf( this );
}

} //namespace binfilter
#endif
