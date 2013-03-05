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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sw.hxx"


#ifndef _SVX_SVXIDS_HRC //autogen
#include <svx/svxids.hrc>
#endif
#include <sfx2/app.hxx>
#include <sfx2/childwin.hxx>
#include <sfx2/bindings.hxx>
#include <svx/svdmark.hxx>
#include <svx/svdview.hxx>
#include <svx/fmglob.hxx>
#include <svx/svdouno.hxx>
#include <com/sun/star/form/FormButtonType.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <svx/htmlmode.hxx>
#include <tools/urlobj.hxx>
#include "wrtsh.hxx"
#include "view.hxx"
#include "IMark.hxx"
#include "doc.hxx"
#include "wrtsh.hrc"

#include <unomid.h>


using namespace ::com::sun::star;
using ::rtl::OUString;

extern sal_Bool bNoInterrupt;       // in mainwn.cxx

sal_Bool SwWrtShell::MoveBookMark( BookMarkMove eFuncId, const ::sw::mark::IMark* const pMark)
{
//JP 08.03.96: die Wizards brauchen die Selektion !!
//  EndSelect();
    (this->*fnKillSel)( 0, sal_False );

    sal_Bool bRet = sal_True;
    switch(eFuncId)
    {
        case BOOKMARK_INDEX:bRet = SwCrsrShell::GotoMark( pMark );break;
        case BOOKMARK_NEXT: bRet = SwCrsrShell::GoNextBookmark();break;
        case BOOKMARK_PREV: bRet = SwCrsrShell::GoPrevBookmark();break;
        default:;//prevent warning
    }

    if( bRet && IsSelFrmMode() )
    {
        UnSelectFrm();
        LeaveSelFrmMode();
    }
    if( IsSelection() )
    {
        fnKillSel = &SwWrtShell::ResetSelect;
        fnSetCrsr = &SwWrtShell::SetCrsrKillSel;
    }
    return bRet;
}

sal_Bool SwWrtShell::GotoField( const SwFmtFld& rFld )
{
    (this->*fnKillSel)( 0, sal_False );

    sal_Bool bRet = SwCrsrShell::GotoFld( rFld );
    if( bRet && IsSelFrmMode() )
    {
        UnSelectFrm();
        LeaveSelFrmMode();
    }

	if( IsSelection() )
    {
        fnKillSel = &SwWrtShell::ResetSelect;
        fnSetCrsr = &SwWrtShell::SetCrsrKillSel;
    }

	return bRet;
}

bool SwWrtShell::GotoFieldmark(::sw::mark::IFieldmark const * const pMark)
{
    (this->*fnKillSel)( 0, sal_False );
    bool bRet = SwCrsrShell::GotoFieldmark(pMark);
    if( bRet && IsSelFrmMode() )
    {
        UnSelectFrm();
        LeaveSelFrmMode();
    }
    if( IsSelection() )
    {
        fnKillSel = &SwWrtShell::ResetSelect;
        fnSetCrsr = &SwWrtShell::SetCrsrKillSel;
    }
    return bRet;
}

/*--------------------------------------------------------------------
    Beschreibung: FontWork-Slots invalidieren
 --------------------------------------------------------------------*/


void SwWrtShell::DrawSelChanged( )
{
    static sal_uInt16 __READONLY_DATA aInval[] =
    {
        SID_ATTR_TRANSFORM_POS_X,           // ( SID_SVX_START + 88 )  // for TransformationPropertyPanel
        SID_ATTR_TRANSFORM_POS_Y,           // ( SID_SVX_START + 89 )  // for TransformationPropertyPanel
        SID_ATTR_TRANSFORM_WIDTH,           // ( SID_SVX_START + 90 )  // for TransformationPropertyPanel
        SID_ATTR_TRANSFORM_HEIGHT,          // ( SID_SVX_START + 91 )  // for TransformationPropertyPanel
        SID_ATTR_TRANSFORM_ROT_X,           // ( SID_SVX_START + 93 )  // for TransformationPropertyPanel
        SID_ATTR_TRANSFORM_ROT_Y,           // ( SID_SVX_START + 94 )  // for TransformationPropertyPanel
        SID_ATTR_TRANSFORM_ANGLE,           // ( SID_SVX_START + 95 )  // for TransformationPropertyPanel
        SID_ATTR_FILL_STYLE,                // ( SID_SVX_START + 164 )
        SID_ATTR_FILL_COLOR,                // ( SID_SVX_START + 165 )
        SID_ATTR_FILL_GRADIENT,             // ( SID_SVX_START + 166 ) // for AreaPropertyPanel
        SID_ATTR_FILL_HATCH,                // ( SID_SVX_START + 167 ) // for AreaPropertyPanel
        SID_ATTR_FILL_BITMAP,               // ( SID_SVX_START + 168 ) // for AreaPropertyPanel
        SID_ATTR_LINE_STYLE,                // ( SID_SVX_START + 169 )
        SID_ATTR_LINE_DASH,                 // ( SID_SVX_START + 170 ) // for LinePropertyPanel
        SID_ATTR_LINE_WIDTH,                // ( SID_SVX_START + 171 )
        SID_ATTR_LINE_COLOR,                // ( SID_SVX_START + 172 )
        SID_ATTR_LINE_START,                // ( SID_SVX_START + 173 ) // for LinePropertyPanel
        SID_ATTR_LINE_END,                  // ( SID_SVX_START + 174 ) // for LinePropertyPanel
        SID_COLOR_TABLE,                    // ( SID_SVX_START + 179 ) // for AreaPropertyPanel
        SID_GRADIENT_LIST,                  // ( SID_SVX_START + 180 ) // for AreaPropertyPanel
        SID_HATCH_LIST,                     // ( SID_SVX_START + 181 ) // for AreaPropertyPanel
        SID_BITMAP_LIST,                    // ( SID_SVX_START + 182 ) // for AreaPropertyPanel
        SID_LINEEND_LIST,                   // ( SID_SVX_START + 184 ) // for LinePropertyPanel
        SID_ATTR_TRANSFORM_PROTECT_POS,     // ( SID_SVX_START + 236 ) // for TransformationPropertyPanel
        SID_ATTR_TRANSFORM_PROTECT_SIZE,    // ( SID_SVX_START + 237 ) // for TransformationPropertyPanel
        SID_ATTR_TRANSFORM_AUTOWIDTH,       // ( SID_SVX_START + 310 ) // for TransformationPropertyPanel
        SID_ATTR_TRANSFORM_AUTOHEIGHT,      // ( SID_SVX_START + 311 ) // for TransformationPropertyPanel
        SID_ATTR_TRANSFORM_ANCHOR,          // ( SID_SVX_START + 318 ) // for TransformationPropertyPanel
        SID_ATTR_FILL_TRANSPARENCE,         // (SID_SVX_START+1105)
        SID_ATTR_FILL_FLOATTRANSPARENCE,    // (SID_SVX_START+1106)
        SID_ATTR_LINE_TRANSPARENCE,         // (SID_SVX_START+1107)
        SID_ATTR_LINE_JOINT,                // (SID_SVX_START+1110)
        SID_ATTR_LINE_CAP,                  // (SID_SVX_START+1111)
        0
    };

    GetView().GetViewFrame()->GetBindings().Invalidate(aInval);

    sal_Bool bOldVal = bNoInterrupt;
    bNoInterrupt = sal_True;    // Trick, um AttrChangedNotify ueber Timer auszufuehren
    GetView().AttrChangedNotify(this);
    bNoInterrupt = bOldVal;
}

sal_Bool SwWrtShell::GotoMark( const ::rtl::OUString& rName )
{
    IDocumentMarkAccess::const_iterator_t ppMark = getIDocumentMarkAccess()->findMark( rName );
    if(ppMark == getIDocumentMarkAccess()->getMarksEnd()) return false;
    return MoveBookMark( BOOKMARK_INDEX, ppMark->get() );
}


sal_Bool SwWrtShell::GotoMark( const ::sw::mark::IMark* const pMark )
{
    return MoveBookMark( BOOKMARK_INDEX, pMark );
}


sal_Bool SwWrtShell::GoNextBookmark()
{
    return MoveBookMark( BOOKMARK_NEXT );
}


sal_Bool SwWrtShell::GoPrevBookmark()
{
    return MoveBookMark( BOOKMARK_PREV );
}


void SwWrtShell::ExecMacro( const SvxMacro& rMacro, String* pRet, SbxArray* pArgs )
{
    // OD 11.02.2003 #100556# - execute macro, if it is allowed.
    if ( IsMacroExecAllowed() )
    {
        GetDoc()->ExecMacro( rMacro, pRet, pArgs );
    }
}


sal_uInt16 SwWrtShell::CallEvent( sal_uInt16 nEvent, const SwCallMouseEvent& rCallEvent,
                                sal_Bool bChkPtr, SbxArray* pArgs,
                                const Link* pCallBack )
{
    return GetDoc()->CallEvent( nEvent, rCallEvent, bChkPtr, pArgs, pCallBack );
}


    // fall ein util::URL-Button selektiert ist, dessen util::URL returnen, ansonsten
    // einen LeerString
sal_Bool SwWrtShell::GetURLFromButton( String& rURL, String& rDescr ) const
{
    sal_Bool bRet = sal_False;
    const SdrView *pDView = GetDrawView();
    if( pDView )
    {
        // Ein Fly ist genau dann erreichbar, wenn er selektiert ist.
        const SdrMarkList &rMarkList = pDView->GetMarkedObjectList();

        if (rMarkList.GetMark(0))
        {
            SdrUnoObj* pUnoCtrl = PTR_CAST(SdrUnoObj, rMarkList.GetMark(0)->GetMarkedSdrObj());
            if (pUnoCtrl && FmFormInventor == pUnoCtrl->GetObjInventor())
            {
                uno::Reference< awt::XControlModel >  xControlModel = pUnoCtrl->GetUnoControlModel();

                ASSERT( xControlModel.is(), "UNO-Control ohne Model" );
                if( !xControlModel.is() )
                    return bRet;

                uno::Reference< beans::XPropertySet >  xPropSet(xControlModel, uno::UNO_QUERY);

                uno::Any aTmp;

                form::FormButtonType eButtonType = form::FormButtonType_URL;
                uno::Reference< beans::XPropertySetInfo >   xInfo = xPropSet->getPropertySetInfo();
                if(xInfo->hasPropertyByName( C2U("ButtonType") ))
                {
                    aTmp = xPropSet->getPropertyValue( C2U("ButtonType") );
                    form::FormButtonType eTmpButtonType;
                    aTmp >>= eTmpButtonType;
                    if( eButtonType == eTmpButtonType)
                    {
                        // Label
                        aTmp = xPropSet->getPropertyValue( C2U("Label") );
                        OUString uTmp;
                        if( (aTmp >>= uTmp) && uTmp.getLength())
                        {
                            rDescr = String(uTmp);
                        }

                        // util::URL
                        aTmp = xPropSet->getPropertyValue( C2U("TargetURL") );
                        if( (aTmp >>= uTmp) && uTmp.getLength())
                        {
                            rURL = String(uTmp);
                        }
                        bRet = sal_True;
                    }
                }
            }
        }
    }

    return bRet;
}
