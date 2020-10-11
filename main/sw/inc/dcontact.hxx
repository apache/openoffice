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


#ifndef _DCONTACT_HXX
#define	_DCONTACT_HXX

#include <svx/svdobj.hxx>
#include <svx/svdovirt.hxx>
#include <swtypes.hxx>
#include <fmtanchr.hxx>
#include <frmfmt.hxx>
#include <list>

#include "calbck.hxx"
#include <anchoreddrawobject.hxx>

class SfxPoolItem;
class SwFrmFmt;
class SwFlyFrmFmt;
class SwFlyFrm;
class SwFrm;
class SwPageFrm;
class SwVirtFlyDrawObj;
class SwFmtAnchor;
class SwFlyDrawObj;
class SwRect;
class SwDrawContact;
struct SwPosition;
class SwIndex;
class SdrTextObj;

//Der Umgekehrte Weg: Sucht das Format zum angegebenen Objekt.
//Wenn das Object ein SwVirtFlyDrawObj ist so wird das Format von
//selbigem besorgt.
//Anderfalls ist es eben ein einfaches Zeichenobjekt. Diese hat einen
//UserCall und der ist Client vom gesuchten Format.
//Implementierung in dcontact.cxx
SW_DLLPUBLIC SwFrmFmt *FindFrmFmt( SdrObject *pObj );
inline const SwFrmFmt *FindFrmFmt( const SdrObject *pObj )
{	return ::FindFrmFmt( (SdrObject*)pObj ); }
sal_Bool HasWrap( const SdrObject* pObj );

void setContextWritingMode( SdrObject* pObj, SwFrm* pAnchor );

//Bei Aenderungen das Objekt aus dem ContourCache entfernen.
//Implementierung in TxtFly.Cxx
void ClrContourCache( const SdrObject *pObj );

// liefert BoundRect inklusive Abstand
// --> OD 2006-08-15 #i68520# - change naming
SwRect GetBoundRectOfAnchoredObj( const SdrObject* pObj );
// <--

//Liefert den UserCall ggf. vom Gruppenobjekt
// OD 2004-03-31 #i26791# - change return type
SwContact* GetUserCall( const SdrObject* );

// liefert sal_True falls das SrdObject ein Marquee-Object (Lauftext) ist
sal_Bool IsMarqueeTextObj( const SdrObject& rObj );

//Basisklasse fuer die folgenden KontaktObjekte (Rahmen+Zeichenobjekte)
class SwContact : public SdrObjUserCall, public SwClient
{
    // OD 05.09.2003 #112039# - boolean, indicating destruction of contact object
    // important note: boolean has to be set at the beginning of each destructor
    //                 in the subclasses using method <SetInDTOR()>.
    bool mbInDTOR;

    /** method to move object to visible/invisible layer

        OD 21.08.2003 #i18447#
        Implementation for the public method <MoveObjToVisibleLayer(..)>
        and <MoveObjToInvisibleLayer(..)>
        If object is in invisible respectively visible layer, it's moved to
        the corresponding visible respectively invisible layers.
        For group object the members are individually moved to the corresponding
        layer, because <SdrObjGroup::GetLayer()> does return 0, if members
        aren't on the same layer as the group object, and
        <SdrObjGroup::SetLayer(..)|NbcSetLayer(..)> sets also the layer of
        the members.
        OD 2004-01-15 #110582# - moved from subclass <SwDrawContact>

        @author OD

        @param _bToVisible
        input parameter - boolean indicating, if object has to be moved to
        visible (== true) or invisible (== false) layer.

        @param _pDrawObj
        input parameter, which will be changed - drawing object, which will
        change its layer.
    */
    void _MoveObjToLayer( const bool _bToVisible,
                          SdrObject* _pDrawObj );

protected:
    // OD 05.09.2003 #112039# - accessor to set member <mbInDTOR>
    void SetInDTOR();

public:
	TYPEINFO();

	//Fuer den Reader, es wird nur die Verbindung hergestellt.
    SwContact( SwFrmFmt *pToRegisterIn );
    virtual ~SwContact();

    // OD 2004-03-29 #i26791#
    virtual const SwAnchoredObject* GetAnchoredObj( const SdrObject* _pSdrObj ) const = 0;
    virtual SwAnchoredObject* GetAnchoredObj( SdrObject* _pSdrObj ) = 0;

    // OD 13.05.2003 #108784# - made methods virtual and not inline
    // OD 2004-04-01 #i26791# - made methods pure virtual
    virtual const SdrObject *GetMaster() const = 0;
    virtual SdrObject *GetMaster() = 0;
    virtual void SetMaster( SdrObject* _pNewMaster ) = 0;

		  SwFrmFmt	*GetFmt(){ return (SwFrmFmt*)GetRegisteredIn(); }
	const SwFrmFmt	*GetFmt() const
		{ return (const SwFrmFmt*)GetRegisteredIn(); }

    // OD 05.09.2003 #112039# - accessor for member <mbInDTOR>
    bool IsInDTOR() const;

    /** method to move drawing object to corresponding visible layer

        OD 21.08.2003 #i18447#
        uses method <_MoveObjToLayer(..)>
        OD 2004-01-15 #110582# - moved from subclass <SwDrawContact> and made virtual

        @author OD

        @param _pDrawObj
        drawing object, which will be moved to the visible layer
    */
    virtual void MoveObjToVisibleLayer( SdrObject* _pDrawObj );

    /** method to move drawing object to corresponding invisible layer

        OD 21.08.2003 #i18447#
        uses method <_MoveObjToLayer(..)>
        OD 2004-01-15 #110582# - moved from subclass <SwDrawContact> and made virtual.

        @author OD

        @param _pDrawObj
        drawing object, which will be moved to the visible layer
    */
    virtual void MoveObjToInvisibleLayer( SdrObject* _pDrawObj );

    // -------------------------------------------------------------------------
    // OD 2004-01-16 #110582# - some virtual helper methods for information
    // about the object (Writer fly frame resp. drawing object)
    const SwFmtAnchor& GetAnchorFmt() const
    {
        ASSERT( GetFmt(),
                "<SwContact::GetAnchorFmt()> - no frame format -> crash" );

        return GetFmt()->GetAnchor();
    }

    RndStdIds GetAnchorId() const { return GetAnchorFmt().GetAnchorId(); }
    bool      ObjAnchoredAtPage() const { return GetAnchorId() == FLY_AT_PAGE; }
    bool      ObjAnchoredAtFly()  const { return GetAnchorId() == FLY_AT_FLY; }
    bool      ObjAnchoredAtPara() const { return GetAnchorId() == FLY_AT_PARA; }
    bool      ObjAnchoredAtChar() const { return GetAnchorId() == FLY_AT_CHAR; }
    bool      ObjAnchoredAsChar() const { return GetAnchorId() == FLY_AS_CHAR; }

    const SwPosition&  GetCntntAnchor() const
    {
        ASSERT( GetAnchorFmt().GetCntntAnchor(),
                "<SwContact::GetCntntAnchor()> - no content anchor -> crash" );

        return *(GetAnchorFmt().GetCntntAnchor());
    }

    const SwIndex&     GetCntntAnchorIndex() const;

    // -------------------------------------------------------------------------

    /** get data collection of anchored objects, handled by with contact

        OD 2004-08-23 #110810#

        @author
    */
    virtual void GetAnchoredObjs( std::list<SwAnchoredObject*>& _roAnchoredObjs ) const = 0;

    /** get minimum order number of anchored objects handled by with contact

        OD 2004-08-24 #110810#

        @author
    */
    sal_uInt32 GetMinOrdNum() const;

    /** get maximum order number of anchored objects handled by with contact

        OD 2004-08-24 #110810#

        @author
    */
    sal_uInt32 GetMaxOrdNum() const;
};

//KontactObjekt fuer die Verbindung zwischen Rahmen bzw. deren Formaten
//im StarWriter (SwClient) und den Zeichenobjekten des Drawing (SdrObjUserCall)

class SW_DLLPUBLIC SwFlyDrawContact : public SwContact
{
private:
    // OD 2004-04-01 #i26791#
    SwFlyDrawObj* mpMasterObj;

protected:
	 // virtuelle Methoden von SwClient
    virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew );

public:
	TYPEINFO();

	//Legt das DrawObjekt an und meldet es beim Model an.
    SwFlyDrawContact( SwFlyFrmFmt* pToRegisterIn, SdrModel* pMod );
    virtual ~SwFlyDrawContact();

    // OD 2004-03-29 #i26791#
    virtual const SwAnchoredObject* GetAnchoredObj( const SdrObject* _pSdrObj ) const;
    virtual SwAnchoredObject* GetAnchoredObj( SdrObject* _pSdrObj );

    // OD 2004-04-01 #i26791#
    virtual const SdrObject* GetMaster() const;
    virtual SdrObject* GetMaster();
    virtual void SetMaster( SdrObject* _pNewMaster );

    // OD 2004-01-16 #110582# - override methods to control Writer fly frames,
    // which are linked, and to assure that all objects anchored at/inside the
    // Writer fly frame are also made visible/invisible.
    virtual void MoveObjToVisibleLayer( SdrObject* _pDrawObj );
    virtual void MoveObjToInvisibleLayer( SdrObject* _pDrawObj );

    /** get data collection of anchored objects handled by with contact

        OD 2004-08-23 #110810#

        @author
    */
    virtual void GetAnchoredObjs( std::list<SwAnchoredObject*>& _roAnchoredObjs ) const;
};

// OD 16.05.2003 #108784# - new class for re-direct methods calls at a 'virtual'
//      drawing object to its referenced object.
class SwDrawVirtObj : public SdrVirtObj
{
    private:
        // data for connection to writer layout
        // OD 2004-03-25 #i26791# - anchored drawing object instance for the
        // 'virtual' drawing object
        SwAnchoredDrawObject maAnchoredDrawObj;

        // writer-drawing contact object the 'virtual' drawing object is controlled by.
        // This object is also the <UserCall> of the drawing object, if it's
        // inserted into the drawing layer.
        SwDrawContact&  mrDrawContact;

        using SdrVirtObj::GetPlusHdl;
    
   protected:
        // AW: Need own sdr::contact::ViewContact since AnchorPos from parent is
        // not used but something own (top left of new SnapRect minus top left
        // of original SnapRect)
        virtual sdr::contact::ViewContact* CreateObjectSpecificViewContact();

   public:
        TYPEINFO();

        SwDrawVirtObj( SdrObject&       _rNewObj,
                       SwDrawContact&   _rDrawContact );
        virtual ~SwDrawVirtObj();

		// access to offset
        // OD 30.06.2003 #108784# - virtual!!!
        virtual const Point GetOffset() const;

        virtual SdrObject* Clone() const;
        virtual void operator=( const SdrObject& rObj );

        // connection to writer layout
        // OD 2004-03-29 #i26791#
        const SwAnchoredObject* GetAnchoredObj() const;
        SwAnchoredObject* AnchoredObj();
        const SwFrm* GetAnchorFrm() const;
        SwFrm* AnchorFrm();
        void RemoveFromWriterLayout();

        // connection to drawing layer
        void AddToDrawingPage();
        void RemoveFromDrawingPage();

        // is 'virtual' drawing object connected to writer layout and
        // to drawing layer.
        bool IsConnected() const;

        virtual void NbcSetAnchorPos(const Point& rPnt);

        // #108784#
		// All overloaded methods which need to use the offset
		virtual void RecalcBoundRect();
		virtual ::basegfx::B2DPolyPolygon TakeXorPoly() const;
        virtual ::basegfx::B2DPolyPolygon TakeContour() const;
		virtual SdrHdl* GetHdl(sal_uInt32 nHdlNum) const;
		virtual SdrHdl* GetPlusHdl(const SdrHdl& rHdl, sal_uInt16 nPlNum) const;
        virtual void NbcMove(const Size& rSiz);
		virtual void NbcResize(const Point& rRef, const Fraction& xFact, const Fraction& yFact);
		virtual void NbcRotate(const Point& rRef, long nWink, double sn, double cs);
		virtual void NbcMirror(const Point& rRef1, const Point& rRef2);
		virtual void NbcShear(const Point& rRef, long nWink, double tn, FASTBOOL bVShear);
        virtual void Move(const Size& rSiz);
        virtual void Resize(const Point& rRef, const Fraction& xFact, const Fraction& yFact);
		virtual void Rotate(const Point& rRef, long nWink, double sn, double cs);
		virtual void Mirror(const Point& rRef1, const Point& rRef2);
		virtual void Shear(const Point& rRef, long nWink, double tn, FASTBOOL bVShear);
		virtual void RecalcSnapRect();
		virtual const Rectangle& GetSnapRect() const;
		virtual void SetSnapRect(const Rectangle& rRect);
        virtual void NbcSetSnapRect(const Rectangle& rRect);
		virtual const Rectangle& GetLogicRect() const;
		virtual void SetLogicRect(const Rectangle& rRect);
        virtual void NbcSetLogicRect(const Rectangle& rRect);
		virtual Point GetSnapPoint(sal_uInt32 i) const;
		virtual Point GetPoint(sal_uInt32 i) const;
        virtual void NbcSetPoint(const Point& rPnt, sal_uInt32 i);

		// #108784#
		virtual FASTBOOL HasTextEdit() const;

        // OD 17.06.2003 #108784# - overload 'layer' methods
        virtual SdrLayerID GetLayer() const;
        virtual void NbcSetLayer(SdrLayerID nLayer);
        virtual void SetLayer(SdrLayerID nLayer);

        // FullDrag support
	    virtual bool supportsFullDrag() const;
	    virtual SdrObject* getFullDragClone() const;

        // #i97197#
	    virtual void SetBoundRectDirty();
    	virtual const Rectangle& GetCurrentBoundRect() const;
    	virtual const Rectangle& GetLastBoundRect() const;
};

// OD 26.06.2003 #108784#
bool CheckControlLayer( const SdrObject *pObj );

//KontactObjekt fuer die Verbindung von Formaten als Repraesentanten der
//Zeichenobjekte im StarWriter (SwClient) und den Objekten selbst im Drawing
//(SdrObjUserCall).

// --> OD 2006-01-18 #129959#
class NestedUserCallHdl;
// <--

class SwDrawContact : public SwContact
{
    private:
        // OD 2004-03-25 #i26791# - anchored drawing object instance for the
        // 'master' drawing object
        SwAnchoredDrawObject maAnchoredDrawObj;

        // OD 16.05.2003 #108784# - data structure for collecting 'virtual'
        // drawing object supporting drawing objects in headers/footers.
        std::list<SwDrawVirtObj*> maDrawVirtObjs;

        // OD 2004-04-01 #i26791# - boolean indicating set 'master' drawing
        // object has been cleared.
        bool mbMasterObjCleared : 1;

        // OD 10.10.2003 #112299# - internal flag to indicate that disconnect
        // from layout is in progress
        bool mbDisconnectInProgress : 1;

        // --> OD 2006-01-18 #129959#
        // Needed data for handling of nested <SdrObjUserCall> events in
        // method <_Changed(..)>
        bool mbUserCallActive : 1;
        // event type, which is handled for <mpSdrObjHandledByCurrentUserCall>.
        // Note: value only valid, if <mbUserCallActive> is sal_True.
        SdrUserCallType meEventTypeOfCurrentUserCall;

        friend class NestedUserCallHdl;
        // <--

        // unary function used by <list> iterator to find a disconnected 'virtual'
        // drawing object
        struct UsedOrUnusedVirtObjPred
        {
            bool mbUsedPred;
            UsedOrUnusedVirtObjPred( bool _bUsed ) : mbUsedPred( _bUsed ) {};
            bool operator() ( const SwDrawVirtObj* _pDrawVirtObj )
            {
                if ( mbUsedPred )
                {
                    return _pDrawVirtObj->IsConnected();
                }
                else
                {
                    return !_pDrawVirtObj->IsConnected();
                }
            }
        };

        // unary function used by <list> iterator to find a 'virtual' drawing
        // object anchored at a given frame
        struct VirtObjAnchoredAtFrmPred
        {
            const SwFrm* mpAnchorFrm;
            VirtObjAnchoredAtFrmPred( const SwFrm& _rAnchorFrm );
            bool operator() ( const SwDrawVirtObj* _pDrawVirtObj );
        };

        // OD 16.05.2003 #108784# - method for adding/removing 'virtual' drawing object.
        SwDrawVirtObj* CreateVirtObj();
        void DestroyVirtObj( SwDrawVirtObj* pVirtObj );
        void RemoveAllVirtObjs();

        // OD 2004-03-31 #i26791#
        void _InvalidateObjs( const bool _bUpdateSortedObjsList = false );

        // --> OD 2006-01-23 #124157#
        // no copy-constructor and no assignment operator
        SwDrawContact( const SwDrawContact& );
        SwDrawContact& operator=( const SwDrawContact& );
        // <--

    protected:        
        // virtuelle Methoden von SwClient
        virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew );

    public:
        TYPEINFO();

        SwDrawContact( SwFrmFmt *pToRegisterIn, SdrObject *pObj );
        virtual ~SwDrawContact();

        // OD 2004-03-29 #i26791#
        // --> OD 2005-01-06 #i30669# - no default value for parameter <_pSdrObj>
        virtual const SwAnchoredObject* GetAnchoredObj( const SdrObject* _pSdrObj ) const;
        virtual SwAnchoredObject* GetAnchoredObj( SdrObject* _pSdrObj );
        // <--

        // OD 2004-04-01 #i26791#
        virtual const SdrObject* GetMaster() const;
        virtual SdrObject* GetMaster();
        virtual void SetMaster( SdrObject* _pNewMaster );

        // OD 2004-03-29 #i26791#
        const SwFrm* GetAnchorFrm( const SdrObject* _pDrawObj = 0L ) const;
        SwFrm* GetAnchorFrm( SdrObject* _pDrawObj = 0L );

        // --> OD 2004-06-30 #i28701# - page frame is now stored at member <maAnchoredDrawObj>
        inline const SwPageFrm* GetPageFrm() const
        {
            return maAnchoredDrawObj.GetPageFrm();
        }
        inline SwPageFrm* GetPageFrm()
        {
            return maAnchoredDrawObj.GetPageFrm();
        }
        void SetPageFrm( SwPageFrm* _pNewPageFrm )
        {
            return maAnchoredDrawObj.SetPageFrm( _pNewPageFrm );
        }
        // <--
        void ChkPage();
        SwPageFrm* FindPage( const SwRect &rRect );

        //Fuegt das SdrObject in die Arrays (SwPageFrm und SwFrm) des Layouts ein.
        //Der Anker wird Anhand des Attributes SwFmtAnchor bestimmt.
        //Das Objekt wird ggf. beim alten Anker abgemeldet.
        void ConnectToLayout( const SwFmtAnchor *pAnch = 0 );
        // OD 27.06.2003 #108784# - method to insert 'master' drawing object
        // into drawing page
        void InsertMasterIntoDrawPage();

        void DisconnectFromLayout( bool _bMoveMasterToInvisibleLayer = true );
        // OD 19.06.2003 #108784# - disconnect for a dedicated drawing object -
        // could be 'master' or 'virtual'.
        void DisconnectObjFromLayout( SdrObject* _pDrawObj );
        // OD 26.06.2003 #108784# - method to remove 'master' drawing object
        // from drawing page.
        // To be used by the undo for delete of object. Call it after method
        // <DisconnectFromLayout( bool = true )> is already performed.
        // Note: <DisconnectFromLayout( bool )> no longer removes the 'master'
        // drawing object from drawing page.
        void RemoveMasterFromDrawPage();

        // OD 19.06.2003 #108784# - get drawing object ('master' or 'virtual')
        // by frame.
        SdrObject* GetDrawObjectByAnchorFrm( const SwFrm& _rAnchorFrm );

        // virtuelle Methoden von SdrObjUserCall
        virtual void Changed(const SdrObject& rObj, SdrUserCallType eType, const Rectangle& rOldBoundRect);

        // wird von Changed() und auch vom UndoDraw benutzt, uebernimmt
        // das Notifien von Absaetzen, die ausweichen muessen
        void _Changed(const SdrObject& rObj, SdrUserCallType eType, const Rectangle* pOldBoundRect);

        //Moved alle SW-Verbindungen zu dem neuen Master.
        void ChangeMasterObject( SdrObject *pNewMaster );

        // OD 19.06.2003 #108784#
        SwDrawVirtObj* AddVirtObj();

        // OD 20.06.2003 #108784#
        void NotifyBackgrdOfAllVirtObjs( const Rectangle* pOldBoundRect );

        /** get data collection of anchored objects, handled by with contact

            OD 2004-08-23 #110810#

            @author
        */

        static void GetTextObjectsFromFmt( std::list<SdrTextObj*>&, SwDoc* );
        virtual void GetAnchoredObjs( std::list<SwAnchoredObject*>& _roAnchoredObjs ) const;
};

#endif
