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

#include <sfx2/sidebar/propertypanel.hrc>
#include <sfx2/sidebar/Theme.hxx>
#include <sfx2/sidebar/ControlFactory.hxx>
#include <LinePropertyPanel.hxx>
#include <LinePropertyPanel.hrc>
#include <svx/dialogs.hrc>
#include <svx/dialmgr.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <svx/xlnclit.hxx>
#include <svx/xtable.hxx>
#include <svx/xdash.hxx>
#include <svx/drawitem.hxx>
#include <svx/svxitems.hrc>
#include <svtools/valueset.hxx>
#include <unotools/pathoptions.hxx>
#include <unotools/viewoptions.hxx>
#include <comphelper/processfactory.hxx>
#include <i18npool/mslangid.hxx>
#include <svx/xlineit0.hxx>
#include <svx/xlndsit.hxx>
#include <vcl/svapp.hxx>
#include <svx/xlnwtit.hxx>
#include <vcl/lstbox.hxx>
#include <svx/tbxcolorupdate.hxx>
#include <vcl/toolbox.hxx>
#include <svx/xlntrit.hxx>
#include <svx/xlnstit.hxx>
#include <svx/xlnedit.hxx>
#include <svx/xlncapit.hxx>
#include <svx/xlinjoit.hxx>
#include "svx/sidebar/PopupContainer.hxx"
#include "svx/sidebar/PopupControl.hxx"
#include <svx/sidebar/ColorControl.hxx>
#include "LineStyleControl.hxx"
#include "LineWidthControl.hxx"
#include <boost/bind.hpp>

using namespace css;
using namespace cssu;
using ::sfx2::sidebar::Theme;

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

namespace {
    short GetItemId_Impl_line( ValueSet& rValueSet, const Color& rCol )
    {
        if(rCol == COL_AUTO)
            return 0;

        bool	bFound = false;
        sal_uInt16 nCount = rValueSet.GetItemCount();
        sal_uInt16	n	   = 1;

        while ( !bFound && n <= nCount )
        {
            Color aValCol = rValueSet.GetItemColor(n);

            bFound = (   aValCol.GetRed()   == rCol.GetRed()
                && aValCol.GetGreen() == rCol.GetGreen()
                && aValCol.GetBlue()  == rCol.GetBlue() );

            if ( !bFound )
                n++;
        }
        return bFound ? n : -1;
    }

    Color GetTransparentColor (void)
    {
        return COL_TRANSPARENT;
    }

} // end of anonymous namespace


class LineEndLB_LPP : public ListBox
{

public:
		 LineEndLB_LPP( Window* pParent, ResId Id ) : ListBox( pParent, Id ) {}
		 LineEndLB_LPP( Window* pParent, WinBits aWB ) : ListBox( pParent, aWB ) {}

	void Fill( const XLineEndList* pList, bool bStart = true );

	void	Append( XLineEndEntry* pEntry, Bitmap* pBmp = NULL,
					bool bStart = true );
	void	Modify( XLineEndEntry* pEntry, sal_uInt16 nPos, Bitmap* pBmp = NULL,
					bool bStart = true );
};

void LineEndLB_LPP::Fill( const XLineEndList* pList, bool bStart )
{
	long nCount = pList->Count();
	XLineEndEntry* pEntry;
	VirtualDevice aVD;
	SetUpdateMode( false );

	for( long i = 0; i < nCount; i++ )
	{
        pEntry = pList->GetLineEnd( i );
		Bitmap* pBitmap = const_cast<XLineEndList*>(pList)->CreateBitmapForUI( i );
		if( pBitmap )
		{
			Size aBmpSize( pBitmap->GetSizePixel() );
			aVD.SetOutputSizePixel( aBmpSize, false );
			aVD.DrawBitmap( Point(), *pBitmap );
			InsertEntry( pEntry->GetName(),
				aVD.GetBitmap( bStart ? Point() : Point( aBmpSize.Width() / 2, 0 ),
					Size( aBmpSize.Width() / 2, aBmpSize.Height() ) ) );

			delete pBitmap;
		}
		else
			InsertEntry( pEntry->GetName() );
	}
	SetUpdateMode( true );
}

void LineEndLB_LPP::Append( XLineEndEntry* pEntry, Bitmap* pBmp, bool bStart )
{
	if( pBmp )
	{
		VirtualDevice aVD;
		Size aBmpSize( pBmp->GetSizePixel() );

		aVD.SetOutputSizePixel( aBmpSize, false );
		aVD.DrawBitmap( Point(), *pBmp );
		InsertEntry( pEntry->GetName(),
			aVD.GetBitmap( bStart ? Point() : Point( aBmpSize.Width() / 2, 0 ),
				Size( aBmpSize.Width() / 2, aBmpSize.Height() ) ) );
	}
	else
		InsertEntry( pEntry->GetName() );
}

void LineEndLB_LPP::Modify( XLineEndEntry* pEntry, sal_uInt16 nPos, Bitmap* pBmp, bool bStart )
{
	RemoveEntry( nPos );

	if( pBmp )
	{
		VirtualDevice aVD;
		Size aBmpSize( pBmp->GetSizePixel() );

		aVD.SetOutputSizePixel( aBmpSize, false );
		aVD.DrawBitmap( Point(), *pBmp );
		InsertEntry( pEntry->GetName(),
			aVD.GetBitmap( bStart ? Point() : Point( aBmpSize.Width() / 2, 0 ),
				Size( aBmpSize.Width() / 2, aBmpSize.Height() ) ), nPos );
	}
	else
		InsertEntry( pEntry->GetName(), nPos );
}


// namespace open

namespace svx { namespace sidebar {



LinePropertyPanel::LinePropertyPanel(
    Window* pParent,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings)
:   Control(
        pParent, 
        SVX_RES(RID_SIDEBAR_LINE_PANEL)),
    mpFTWidth(new FixedText(this, SVX_RES(FT_WIDTH))),
    mpTBWidthBackground(sfx2::sidebar::ControlFactory::CreateToolBoxBackground(this)),
    mpTBWidth(sfx2::sidebar::ControlFactory::CreateToolBox(mpTBWidthBackground.get(), SVX_RES(TB_WIDTH))),
    mpFTColor(new FixedText(this, SVX_RES(FT_COLOR))),
    mpTBColorBackground(sfx2::sidebar::ControlFactory::CreateToolBoxBackground(this)),
    mpTBColor(sfx2::sidebar::ControlFactory::CreateToolBox(mpTBColorBackground.get(), SVX_RES(TB_COLOR))),
    mpFTStyle(new FixedText(this, SVX_RES(FT_STYLE))),
    mpTBStyleBackground(sfx2::sidebar::ControlFactory::CreateToolBoxBackground(this)),
    mpTBStyle(sfx2::sidebar::ControlFactory::CreateToolBox(mpTBStyleBackground.get(), SVX_RES(TB_STYLE))),
    mpFTTrancparency(new FixedText(this, SVX_RES(FT_TRANSPARENT))),
    mpMFTransparent(new MetricField(this, SVX_RES(MF_TRANSPARENT))),
    mpFTArrow(new FixedText(this, SVX_RES(FT_ARROW))),
    mpLBStart(new LineEndLB_LPP(this, SVX_RES(LB_START))),
    mpLBEnd(new LineEndLB_LPP(this, SVX_RES(LB_END))),
    mpFTEdgeStyle(new FixedText(this, SVX_RES(FT_EDGESTYLE))),
    mpLBEdgeStyle(new ListBox(this, SVX_RES(LB_EDGESTYLE))),
    mpFTCapStyle(new FixedText(this, SVX_RES(FT_CAPSTYLE))),
    mpLBCapStyle(new ListBox(this, SVX_RES(LB_CAPSTYLE))),
    maStyleControl(SID_ATTR_LINE_STYLE, *pBindings, *this),             // ( SID_SVX_START + 169 )
    maDashControl (SID_ATTR_LINE_DASH, *pBindings, *this),              // ( SID_SVX_START + 170 )
    maWidthControl(SID_ATTR_LINE_WIDTH, *pBindings, *this),             // ( SID_SVX_START + 171 )
    maColorControl(SID_ATTR_LINE_COLOR, *pBindings, *this),             // ( SID_SVX_START + 172 )
    maStartControl(SID_ATTR_LINE_START, *pBindings, *this),             // ( SID_SVX_START + 173 )
    maEndControl(SID_ATTR_LINE_END, *pBindings, *this),                 // ( SID_SVX_START + 174 )
    maLineEndListControl(SID_LINEEND_LIST, *pBindings, *this),          // ( SID_SVX_START + 184 )
    maTransControl(SID_ATTR_LINE_TRANSPARENCE, *pBindings, *this),      // (SID_SVX_START+1107)
    maEdgeStyle(SID_ATTR_LINE_JOINT, *pBindings, *this),                // (SID_SVX_START+1110)
    maCapStyle(SID_ATTR_LINE_CAP, *pBindings, *this),                   // (SID_SVX_START+1111)
    maColor(COL_BLACK),
    mpColorUpdater(new ::svx::ToolboxButtonColorUpdater(SID_ATTR_LINE_COLOR, TBI_COLOR, mpTBColor.get(), TBX_UPDATER_MODE_CHAR_COLOR_NEW)),
    mpStyleItem(),
    mpDashItem(),
    mnTrans(0),
    meMapUnit(SFX_MAPUNIT_MM),
    mnWidthCoreValue(0),
    mpLineEndList(0),
    mpStartItem(0),
    mpEndItem(0),
    maColorPopup(this, ::boost::bind(&LinePropertyPanel::CreateColorPopupControl, this, _1)),
    maLineStylePopup(this, ::boost::bind(&LinePropertyPanel::CreateLineStylePopupControl, this, _1)),
    maLineWidthPopup(this, ::boost::bind(&LinePropertyPanel::CreateLineWidthPopupControl, this, _1)),
    maIMGColor(SVX_RES(IMG_COLOR)),
    maIMGNone(SVX_RES(IMG_NONE_ICON)),
    mpIMGStyleIcon(),
    mpIMGWidthIcon(),
    mpIMGStyleIconH(),
    mpIMGWidthIconH(),
    mxFrame(rxFrame),
    maContext(),
    mpBindings(pBindings),
    mbColorAvailable(true),
    mbStyleAvailable(false),
    mbDashAvailable(false),
    mbTransAvailable(true),
    mbWidthValuable(true),
    mbStartAvailable(true),
    mbEndAvailable(true)
{
    Initialize();
    FreeResource();
}



LinePropertyPanel::~LinePropertyPanel()
{
    // Destroy the toolboxes, then their background windows.
    mpTBWidth.reset();
    mpTBColor.reset();
    mpTBStyle.reset();
    mpTBWidthBackground.reset();
    mpTBColorBackground.reset();
    mpTBStyleBackground.reset();
}



void LinePropertyPanel::Initialize()
{
    mpIMGStyleIcon.reset(new Image[11]);
	mpIMGStyleIcon[0] = Image(SVX_RES(IMG_LINE1_ICON));
	mpIMGStyleIcon[1] = Image(SVX_RES(IMG_LINE2_ICON));
	mpIMGStyleIcon[2] = Image(SVX_RES(IMG_LINE3_ICON));
	mpIMGStyleIcon[3] = Image(SVX_RES(IMG_LINE4_ICON));
	mpIMGStyleIcon[4] = Image(SVX_RES(IMG_LINE5_ICON));
	mpIMGStyleIcon[5] = Image(SVX_RES(IMG_LINE6_ICON));
	mpIMGStyleIcon[6] = Image(SVX_RES(IMG_LINE7_ICON));
	mpIMGStyleIcon[7] = Image(SVX_RES(IMG_LINE8_ICON));
	mpIMGStyleIcon[8] = Image(SVX_RES(IMG_LINE9_ICON));
	mpIMGStyleIcon[9] = Image(SVX_RES(IMG_LINE10_ICON));
	mpIMGStyleIcon[10] = Image(SVX_RES(IMG_LINE11_ICON));

    mpIMGWidthIcon.reset(new Image[8]);
	mpIMGWidthIcon[0] = Image(SVX_RES(IMG_WIDTH1_ICON));
	mpIMGWidthIcon[1] = Image(SVX_RES(IMG_WIDTH2_ICON));
	mpIMGWidthIcon[2] = Image(SVX_RES(IMG_WIDTH3_ICON));
	mpIMGWidthIcon[3] = Image(SVX_RES(IMG_WIDTH4_ICON));
	mpIMGWidthIcon[4] = Image(SVX_RES(IMG_WIDTH5_ICON));
	mpIMGWidthIcon[5] = Image(SVX_RES(IMG_WIDTH6_ICON));
	mpIMGWidthIcon[6] = Image(SVX_RES(IMG_WIDTH7_ICON));
	mpIMGWidthIcon[7] = Image(SVX_RES(IMG_WIDTH8_ICON));

	//high contrast
    mpIMGStyleIconH.reset(new Image[11]);
	mpIMGStyleIconH[0] = Image(SVX_RES(IMG_LINE1_ICON_H));
	mpIMGStyleIconH[1] = Image(SVX_RES(IMG_LINE2_ICON_H));
	mpIMGStyleIconH[2] = Image(SVX_RES(IMG_LINE3_ICON_H));
	mpIMGStyleIconH[3] = Image(SVX_RES(IMG_LINE4_ICON_H));
	mpIMGStyleIconH[4] = Image(SVX_RES(IMG_LINE5_ICON_H));
	mpIMGStyleIconH[5] = Image(SVX_RES(IMG_LINE6_ICON_H));
	mpIMGStyleIconH[6] = Image(SVX_RES(IMG_LINE7_ICON_H));
	mpIMGStyleIconH[7] = Image(SVX_RES(IMG_LINE8_ICON_H));
	mpIMGStyleIconH[8] = Image(SVX_RES(IMG_LINE9_ICON_H));
	mpIMGStyleIconH[9] = Image(SVX_RES(IMG_LINE10_ICON_H));
	mpIMGStyleIconH[10] = Image(SVX_RES(IMG_LINE11_ICON_H));

    mpIMGWidthIconH.reset(new Image[8]);
	mpIMGWidthIconH[0] = Image(SVX_RES(IMG_WIDTH1_ICON_H));
	mpIMGWidthIconH[1] = Image(SVX_RES(IMG_WIDTH2_ICON_H));
	mpIMGWidthIconH[2] = Image(SVX_RES(IMG_WIDTH3_ICON_H));
	mpIMGWidthIconH[3] = Image(SVX_RES(IMG_WIDTH4_ICON_H));
	mpIMGWidthIconH[4] = Image(SVX_RES(IMG_WIDTH5_ICON_H));
	mpIMGWidthIconH[5] = Image(SVX_RES(IMG_WIDTH6_ICON_H));
	mpIMGWidthIconH[6] = Image(SVX_RES(IMG_WIDTH7_ICON_H));
	mpIMGWidthIconH[7] = Image(SVX_RES(IMG_WIDTH8_ICON_H));
	//end

	meMapUnit = maWidthControl.GetCoreMetric();

	mpTBColor->SetItemImage(TBI_COLOR, maIMGColor);
	Size aTbxSize( mpTBColor->CalcWindowSizePixel() );
	mpTBColor->SetOutputSizePixel( aTbxSize );
	mpTBColor->SetItemBits( TBI_COLOR, mpTBColor->GetItemBits( TBI_COLOR ) | TIB_DROPDOWNONLY );
	mpTBColor->SetQuickHelpText(TBI_COLOR,String(SVX_RES(STR_QH_TB_COLOR))); //Add
	mpTBColor->SetBackground(Wallpaper());
	mpTBColor->SetPaintTransparent(true);
	Link aLink = LINK(this, LinePropertyPanel, ToolboxColorSelectHdl);
	mpTBColor->SetDropdownClickHdl ( aLink );
	mpTBColor->SetSelectHdl ( aLink );

	mpTBStyle->SetItemImage(TBI_STYLE, mpIMGStyleIcon[0]);
	aTbxSize = mpTBStyle->CalcWindowSizePixel() ;
	mpTBStyle->SetOutputSizePixel( aTbxSize );
	mpTBStyle->SetItemBits( TBI_STYLE, mpTBStyle->GetItemBits( TBI_STYLE ) | TIB_DROPDOWNONLY );
	mpTBStyle->SetQuickHelpText(TBI_STYLE,String(SVX_RES(STR_QH_TB_STYLE))); //Add
	mpTBStyle->SetBackground(Wallpaper());
	mpTBStyle->SetPaintTransparent(true);
	aLink = LINK(this, LinePropertyPanel, ToolboxStyleSelectHdl);
	mpTBStyle->SetDropdownClickHdl ( aLink );
	mpTBStyle->SetSelectHdl ( aLink );

	mpTBWidth->SetItemImage(TBI_WIDTH, mpIMGWidthIcon[0]);
	aTbxSize = mpTBWidth->CalcWindowSizePixel() ;
	mpTBWidth->SetOutputSizePixel( aTbxSize );
	mpTBWidth->SetItemBits( TBI_WIDTH, mpTBWidth->GetItemBits( TBI_WIDTH ) | TIB_DROPDOWNONLY );
	mpTBWidth->SetQuickHelpText(TBI_WIDTH,String(SVX_RES(STR_QH_TB_WIDTH))); //Add
	mpTBWidth->SetBackground(Wallpaper());
	mpTBWidth->SetPaintTransparent(true);
	aLink = LINK(this, LinePropertyPanel, ToolboxWidthSelectHdl);
	mpTBWidth->SetDropdownClickHdl ( aLink );
	mpTBWidth->SetSelectHdl ( aLink );

	FillLineEndList();
	SelectEndStyle(true);
	SelectEndStyle(false);
	aLink = LINK( this, LinePropertyPanel, ChangeStartHdl );
	mpLBStart->SetSelectHdl( aLink );
	mpLBStart->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Beginning Style")));	//wj acc
    mpLBStart->SetDropDownLineCount(std::min(sal_uInt16(20), mpLBStart->GetEntryCount()));
	aLink = LINK( this, LinePropertyPanel, ChangeEndHdl );
	mpLBEnd->SetSelectHdl( aLink );
	mpLBEnd->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Ending Style")));	//wj acc
    mpLBEnd->SetDropDownLineCount(std::min(sal_uInt16(20), mpLBEnd->GetEntryCount()));

	aLink = LINK(this, LinePropertyPanel, ChangeTransparentHdl);
	mpMFTransparent->SetModifyHdl(aLink);
	mpMFTransparent->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Transparency")));	//wj acc
	
	mpTBWidth->SetAccessibleRelationLabeledBy(mpFTWidth.get());
	mpTBColor->SetAccessibleRelationLabeledBy(mpFTColor.get());
	mpTBStyle->SetAccessibleRelationLabeledBy(mpFTStyle.get());
	mpMFTransparent->SetAccessibleRelationLabeledBy(mpFTTrancparency.get());
	mpLBStart->SetAccessibleRelationLabeledBy(mpFTArrow.get());
	mpLBEnd->SetAccessibleRelationLabeledBy(mpLBEnd.get());

    aLink = LINK( this, LinePropertyPanel, ChangeEdgeStyleHdl );
    mpLBEdgeStyle->SetSelectHdl( aLink );
    mpLBEdgeStyle->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Corner Style")));

    aLink = LINK( this, LinePropertyPanel, ChangeCapStyleHdl );
    mpLBCapStyle->SetSelectHdl( aLink );
    mpLBCapStyle->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Cap Style")));
}



void LinePropertyPanel::SetupIcons(void)
{
    if(Theme::GetBoolean(Theme::Bool_UseSymphonyIcons))
    {
        // todo
    }
    else
    {
        // todo
    }
}



LinePropertyPanel* LinePropertyPanel::Create (
    Window* pParent,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings)
{
    if (pParent == NULL)
        throw lang::IllegalArgumentException(A2S("no parent Window given to LinePropertyPanel::Create"), NULL, 0);
    if ( ! rxFrame.is())
        throw lang::IllegalArgumentException(A2S("no XFrame given to LinePropertyPanel::Create"), NULL, 1);
    if (pBindings == NULL)
        throw lang::IllegalArgumentException(A2S("no SfxBindings given to LinePropertyPanel::Create"), NULL, 2);
    
    return new LinePropertyPanel(
        pParent,
        rxFrame,
        pBindings);
}



void LinePropertyPanel::DataChanged(
    const DataChangedEvent& rEvent)
{
    (void)rEvent;
    
    SetupIcons();
}



void LinePropertyPanel::HandleContextChange(
    const ::sfx2::sidebar::EnumContext aContext)
{
    if(maContext == aContext)
    {
        // Nothing to do.
        return;
    }

    maContext = aContext;



    // todo
}



void LinePropertyPanel::NotifyItemUpdate( 
    sal_uInt16 nSID, 
    SfxItemState eState, 
    const SfxPoolItem* pState)
{
	switch(nSID)
	{
    	case SID_ATTR_LINE_COLOR:
        {
		    if( eState == SFX_ITEM_DISABLED)
		    { 
			    mpFTColor->Disable();
			    mpTBColor->Disable();
			    mbColorAvailable = false;
			    mpColorUpdater->Update(COL_WHITE);
		    }
		    else
		    {
			    mpFTColor->Enable();
			    mpTBColor->Enable();
                const XLineColorItem* pItem = dynamic_cast< const XLineColorItem* >(pState);

			    if(eState >= SFX_ITEM_DEFAULT && pItem)
			    {
				    maColor = pItem->GetColorValue();
				    mbColorAvailable = true;
				    mpColorUpdater->Update(maColor);
			    }
			    else
			    {
				    mbColorAvailable = false;
				    mpColorUpdater->Update(COL_WHITE);
			    }	
		    }
		    break;
        }
	    case SID_ATTR_LINE_STYLE:
	    case SID_ATTR_LINE_DASH:
        {
		    if( eState == SFX_ITEM_DISABLED)
		    {
			    mpFTStyle->Disable();
			    mpTBStyle->Disable();
			    mpTBStyle->SetItemImage(TBI_STYLE,maIMGNone);
		    }
		    else
		    {
			    mpFTStyle->Enable();
			    mpTBStyle->Enable();
			    if( eState  >= SFX_ITEM_DEFAULT )
			    {
				    if(nSID == SID_ATTR_LINE_STYLE)
				    {
                        const XLineStyleItem* pItem = dynamic_cast< const XLineStyleItem* >(pState);

                        if(pItem)
                        {
					        mbStyleAvailable =true;
    					    mpStyleItem.reset(pState ? (XLineStyleItem*)pItem->Clone() : 0);
                        }
				    }
				    else if(nSID == SID_ATTR_LINE_DASH)
				    {
                        const XLineDashItem* pItem = dynamic_cast< const XLineDashItem* >(pState);

                        if(pItem)
                        {
    					    mbDashAvailable = true;
	    				    mpDashItem.reset(pState ? (XLineDashItem*)pItem->Clone() : 0);
                        }
				    }
			    }
			    else
			    {
				    if(nSID == SID_ATTR_LINE_STYLE)
					    mbStyleAvailable = false;
				    else 
					    mbDashAvailable = false;
			    }
			    SetStyleIcon();
		    }
		    break;
        }
    	case SID_ATTR_LINE_TRANSPARENCE:
        {
		    if( eState == SFX_ITEM_DISABLED )
		    {
			    mpFTTrancparency->Disable();
			    mpMFTransparent->Disable();
			    mpMFTransparent->SetValue(0);//add 
			    mpMFTransparent->SetText(String());
			    mbTransAvailable = false;
		    }
		    else
		    {
			    mpFTTrancparency->Enable();
			    mpMFTransparent->Enable();
			    mbTransAvailable = true;
                const XLineTransparenceItem* pItem = dynamic_cast< const XLineTransparenceItem* >(pState);

                if(eState != SFX_ITEM_DONTCARE && pItem)
			    {
				    mnTrans = pItem->GetValue();
				    mpMFTransparent->SetValue(mnTrans);
			    }
			    else
			    {
				    mpMFTransparent->SetValue(0);//add 
				    mpMFTransparent->SetText(String());
			    }
		    }
		    break;
        }
    	case SID_ATTR_LINE_WIDTH:
        {
		    if(eState == SFX_ITEM_DISABLED)
		    {
			    mpTBWidth->Disable();
			    mpFTWidth->Disable();
		    }
		    else
		    {
			    //enable
			    mpTBWidth->Enable();
			    mpFTWidth->Enable();
                const XLineWidthItem* pItem = dynamic_cast< const XLineWidthItem* >(pState);

			    if(eState >= SFX_ITEM_AVAILABLE && pItem)
			    {
				    mnWidthCoreValue = pItem->GetValue();
				    mbWidthValuable = true;
			    }
			    else
			    {
				    mbWidthValuable = false;
			    }
		    }
		    SetWidthIcon();
		    break;
        }
    	case SID_ATTR_LINE_START:
        {
            const XLineStartItem* pItem = dynamic_cast< const XLineStartItem* >(pState);

		    if(eState != SFX_ITEM_DONTCARE && pItem)
		    {
			    mbStartAvailable = true;	//add 
			    mpStartItem.reset(pItem ? (XLineStartItem*)pItem->Clone() : 0);
			    SelectEndStyle(true);
		    }
		    else
		    {
			    mpLBStart->SetNoSelection();
			    mbStartAvailable = false;	//add 
		    }
		    break;
        }
    	case SID_ATTR_LINE_END:
        {
		    mpFTArrow->Enable();
		    mpLBEnd->Enable();
            const XLineEndItem* pItem = dynamic_cast< const XLineEndItem* >(pState);

		    if(eState != SFX_ITEM_DONTCARE && pItem)
		    {
			    mbEndAvailable = true;		//add 
			    mpEndItem.reset(pItem ? (XLineEndItem*)pItem->Clone() : 0);
			    SelectEndStyle(false);		
		    }
		    else
		    {
			    mpLBEnd->SetNoSelection();
			    mbEndAvailable = false;		//add 
		    }	
		    break;
        }
    	case SID_LINEEND_LIST:
        {
		    FillLineEndList();
		    SelectEndStyle(true);
		    SelectEndStyle(false);
		    break;
        }
        case SID_ATTR_LINE_JOINT:
        {
            if(eState == SFX_ITEM_DISABLED)
            {
                mpLBEdgeStyle->Disable();
            }
            else
            {
                mpLBEdgeStyle->Enable();
                const XLineJointItem* pItem = dynamic_cast< const XLineJointItem* >(pState);
                sal_uInt16 nEntryPos(0);

                if(eState >= SFX_ITEM_AVAILABLE && pItem)
                {
                    switch(pItem->GetValue())
                    {
                        case com::sun::star::drawing::LineJoint_MIDDLE:
                        case com::sun::star::drawing::LineJoint_ROUND:
                        {
                            nEntryPos = 1;
                            break;
                        }
                        case com::sun::star::drawing::LineJoint_NONE:
                        {
                            nEntryPos = 2;
                            break;
                        }
                        case com::sun::star::drawing::LineJoint_MITER:
                        {
                            nEntryPos = 3;
                            break;
                        }
                        case com::sun::star::drawing::LineJoint_BEVEL:
                        {
                            nEntryPos = 4;
                            break;
                        }

                        default:
                            break;
                    }
                }

                if(nEntryPos)
                {
                    mpLBEdgeStyle->SelectEntryPos(nEntryPos - 1);
                }
                else
                {
                    mpLBEdgeStyle->SetNoSelection();
                }
            }
            break;
        }
        case SID_ATTR_LINE_CAP:
        {
            if(eState == SFX_ITEM_DISABLED)
            {
                mpLBCapStyle->Disable();
            }
            else
            {
                mpLBCapStyle->Enable();
                const XLineCapItem* pItem = dynamic_cast< const XLineCapItem* >(pState);
                sal_uInt16 nEntryPos(0);

                if(eState >= SFX_ITEM_AVAILABLE && pItem)
                {
                    switch(pItem->GetValue())
                    {
                        case com::sun::star::drawing::LineCap_BUTT:
                        {
                            nEntryPos = 1;
                            break;
                        }
                        case com::sun::star::drawing::LineCap_ROUND:
                        {
                            nEntryPos = 2;
                            break;
                        }
                        case com::sun::star::drawing::LineCap_SQUARE:
                        {
                            nEntryPos = 3;
                            break;
                        }

                        default:
                            break;
                    }
                }

                if(nEntryPos)
                {
                    mpLBCapStyle->SelectEntryPos(nEntryPos - 1);
                }
                else
                {
                    mpLBCapStyle->SetNoSelection();
                }
            }
            break;
        }
    }
}



SfxBindings* LinePropertyPanel::GetBindings() 
{ 
    return mpBindings; 
}



IMPL_LINK( LinePropertyPanel, ImplPopupModeEndHdl, FloatingWindow*, EMPTYARG )
{	
	return 0;
}




IMPL_LINK(LinePropertyPanel, ToolboxColorSelectHdl,ToolBox*, pToolBox)
{
	sal_uInt16 nId = pToolBox->GetCurItemId();
	if(nId == TBI_COLOR)
	{
        maColorPopup.Show(*pToolBox);
        maColorPopup.SetCurrentColor(maColor, mbColorAvailable);
	}
	return 0;
}




IMPL_LINK(LinePropertyPanel, ToolboxStyleSelectHdl,ToolBox*, pToolBox)
{
	if (pToolBox->GetCurItemId() == TBI_STYLE)
	{
        maLineStylePopup.SetStyleSelect(mpStyleItem.get(), mpDashItem.get(), mbStyleAvailable, mbDashAvailable);
        maLineStylePopup.Show(*pToolBox);
	}
	return 0;
}




IMPL_LINK(LinePropertyPanel, ChangeStartHdl, void*, EMPTYARG)
{
	sal_uInt16	nPos = mpLBStart->GetSelectEntryPos();
	if( nPos != LISTBOX_ENTRY_NOTFOUND && nPos != mpLBStart->GetSavedValue() )
	{
		XLineStartItem* pItem = NULL;
		if( nPos == 0 )
			pItem = new XLineStartItem();
		else if( mpLineEndList && mpLineEndList->Count() > (long) ( nPos - 1 ) )
			pItem = new XLineStartItem( mpLBStart->GetSelectEntry(),mpLineEndList->GetLineEnd( nPos - 1 )->GetLineEnd() );
		GetBindings()->GetDispatcher()->Execute(SID_ATTR_LINEEND_STYLE, SFX_CALLMODE_RECORD, pItem,  0L);
		delete pItem;
	}
	return 0;
}




IMPL_LINK(LinePropertyPanel, ChangeEndHdl, void*, EMPTYARG)
{
	sal_uInt16	nPos = mpLBEnd->GetSelectEntryPos();
	if( nPos != LISTBOX_ENTRY_NOTFOUND && nPos != mpLBEnd->GetSavedValue() )
	{
		XLineEndItem* pItem = NULL;
		if( nPos == 0 )
			pItem = new XLineEndItem();
		else if( mpLineEndList && mpLineEndList->Count() > (long) ( nPos - 1 ) )
			pItem = new XLineEndItem( mpLBEnd->GetSelectEntry(), mpLineEndList->GetLineEnd( nPos - 1 )->GetLineEnd() );
		GetBindings()->GetDispatcher()->Execute(SID_ATTR_LINEEND_STYLE, SFX_CALLMODE_RECORD, pItem,  0L);
		delete pItem;
	}
	return 0;
}




IMPL_LINK(LinePropertyPanel, ChangeEdgeStyleHdl, void*, EMPTYARG)
{
    const sal_uInt16 nPos(mpLBEdgeStyle->GetSelectEntryPos());

    if(LISTBOX_ENTRY_NOTFOUND != nPos && nPos != mpLBEdgeStyle->GetSavedValue())
    {
        XLineJointItem* pItem = 0;

        switch(nPos)
        {
            case 0: // rounded
            {
                pItem = new XLineJointItem(com::sun::star::drawing::LineJoint_ROUND);
                break;
            }
            case 1: // none
            {
                pItem = new XLineJointItem(com::sun::star::drawing::LineJoint_NONE);
                break;
            }
            case 2: // mitered
            {
                pItem = new XLineJointItem(com::sun::star::drawing::LineJoint_MITER);
                break;
            }
            case 3: // beveled
            {
                pItem = new XLineJointItem(com::sun::star::drawing::LineJoint_BEVEL);
                break;
            }
        }

        GetBindings()->GetDispatcher()->Execute(SID_ATTR_LINE_JOINT, SFX_CALLMODE_RECORD, pItem,  0L);
        delete pItem;
    }
    return 0;
}




IMPL_LINK(LinePropertyPanel, ChangeCapStyleHdl, void*, EMPTYARG)
{
    const sal_uInt16 nPos(mpLBCapStyle->GetSelectEntryPos());

    if(LISTBOX_ENTRY_NOTFOUND != nPos && nPos != mpLBCapStyle->GetSavedValue())
    {
        XLineCapItem* pItem = 0;

        switch(nPos)
        {
            case 0: // flat
            {
                pItem = new XLineCapItem(com::sun::star::drawing::LineCap_BUTT);
                break;
            }
            case 1: // round
            {
                pItem = new XLineCapItem(com::sun::star::drawing::LineCap_ROUND);
                break;
            }
            case 2: // square
            {
                pItem = new XLineCapItem(com::sun::star::drawing::LineCap_SQUARE);
                break;
            }
        }

        GetBindings()->GetDispatcher()->Execute(SID_ATTR_LINE_CAP, SFX_CALLMODE_RECORD, pItem,  0L);
        delete pItem;
    }
    return 0;
}




IMPL_LINK(LinePropertyPanel, ToolboxWidthSelectHdl,ToolBox*, pToolBox)
{
	if (pToolBox->GetCurItemId() == TBI_WIDTH)
	{
		maLineWidthPopup.SetWidthSelect(mnWidthCoreValue, mbWidthValuable, meMapUnit);
        maLineWidthPopup.Show(*pToolBox);
	}
	return 0;
}




IMPL_LINK( LinePropertyPanel, ChangeTransparentHdl, void *, EMPTYARG )
{
	sal_uInt16 nVal = (sal_uInt16)mpMFTransparent->GetValue();
	XLineTransparenceItem aItem( nVal );

	GetBindings()->GetDispatcher()->Execute(SID_ATTR_LINE_STYLE, SFX_CALLMODE_RECORD, &aItem, 0L);
	return( 0L );
}




PopupControl* LinePropertyPanel::CreateColorPopupControl (PopupContainer* pParent)
{
    const ResId aResId(SVX_RES(STR_AUTOMATICE));
    
    return new ColorControl(
        pParent,
        mpBindings,
        SVX_RES(RID_POPUPPANEL_LINEPAGE_COLOR),
        SVX_RES(VS_COLOR),
        ::boost::bind(GetTransparentColor),
        ::boost::bind(&LinePropertyPanel::SetColor, this, _1, _2),
        pParent,
        &aResId);
}




PopupControl* LinePropertyPanel::CreateLineStylePopupControl (PopupContainer* pParent)
{
    return new LineStyleControl (pParent, *this);
}




PopupControl* LinePropertyPanel::CreateLineWidthPopupControl (PopupContainer* pParent)
{
    return new LineWidthControl(pParent, *this);
}




void LinePropertyPanel::EndLineStylePopupMode (void)
{
    maLineStylePopup.Hide();
}




void LinePropertyPanel::EndLineWidthPopupMode (void)
{
    maLineWidthPopup.Hide();
}




void  LinePropertyPanel::SetStyleIcon()
{
	if(!mbStyleAvailable)
	{	//custome style that not listed in panel
		mpTBStyle->SetItemImage(TBI_STYLE,maIMGNone);
		return;
	}

    const XLineStyle eXLS(mpStyleItem ? (XLineStyle)mpStyleItem->GetValue() : XLINE_NONE);

	switch(eXLS)
	{
	case XLINE_NONE:
		mpTBStyle->SetItemImage(TBI_STYLE,maIMGNone);
		break;
	case XLINE_SOLID:
		mpTBStyle->SetItemImage(TBI_STYLE, GetDisplayBackground().GetColor().IsDark() ? mpIMGStyleIconH[0] : mpIMGStyleIcon[0]);
		break;
	case XLINE_DASH:
		if(mpDashItem && mbDashAvailable)
		{
			XDash aDash = mpDashItem->GetDashValue();
			sal_uInt16 n = 0;
			for( ; n < 10; n++ )
			{
				if( Dash_Set[n] == aDash )
				{
					mpTBStyle->SetItemImage(TBI_STYLE, GetDisplayBackground().GetColor().IsDark() ? mpIMGStyleIconH[n+1] :mpIMGStyleIcon[n+1]);
					break;
				}
			}
			if(n == 10)
				mpTBStyle->SetItemImage(TBI_STYLE,maIMGNone);
		}
		else
		{
			mpTBStyle->SetItemImage(TBI_STYLE,maIMGNone);
		}
		break;
	}
}



void LinePropertyPanel::SetWidthIcon(int n)
{
	if(n==0)
		mpTBWidth->SetItemImage( TBI_WIDTH, maIMGNone);
	else
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH[n-1] : mpIMGWidthIcon[n-1]);
}



void LinePropertyPanel::SetWidthIcon()
{
	if(!mbWidthValuable)
	{
		mpTBWidth->SetItemImage( TBI_WIDTH, maIMGNone);
		return;
	}

	long nVal = LogicToLogic(mnWidthCoreValue * 10,(MapUnit)meMapUnit , MAP_POINT);

	if(nVal <= 6)	
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH[0] : mpIMGWidthIcon[0]); 
	else if(nVal > 6 && nVal <= 9)	
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH[1] : mpIMGWidthIcon[1]);  
	else if(nVal > 9 && nVal <= 12)	
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH[2] : mpIMGWidthIcon[2]); 
	else if(nVal > 12 && nVal <= 19)	
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH[3] : mpIMGWidthIcon[3]); 
	else if(nVal > 19 && nVal <= 26)		
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH[4] : mpIMGWidthIcon[4]); 
	else if(nVal > 26 && nVal <= 37)	
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH[5] : mpIMGWidthIcon[5]); 
	else if(nVal > 37 && nVal <=52)		
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH[6] : mpIMGWidthIcon[6]); 
	else if(nVal > 52)		
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH[7] : mpIMGWidthIcon[7]); 

}



void LinePropertyPanel::SetLineStyleItem(XLineStyleItem* pStyle)
{
    mpStyleItem.reset(pStyle ? (XLineStyleItem*)pStyle->Clone() : 0);
}



void LinePropertyPanel::SetLineDashItem(XLineDashItem* pDash)
{
    mpDashItem.reset(pDash ? (XLineDashItem*)pDash->Clone() : 0);
}



void LinePropertyPanel::SetColor (
    const String& rsColorName,
    const Color aColor)
{
    XLineColorItem aColorItem(rsColorName, aColor);
    mpBindings->GetDispatcher()->Execute(SID_ATTR_LINE_COLOR, SFX_CALLMODE_RECORD, &aColorItem, 0L);
    maColor = aColor;
}



void LinePropertyPanel::SetWidth(long nWidth)
{
    mnWidthCoreValue = nWidth;
    mbWidthValuable = true;
}



void  LinePropertyPanel::FillLineEndList()
{
	SfxObjectShell* pSh = SfxObjectShell::Current();
	if ( pSh && pSh->GetItem( SID_LINEEND_LIST ) )
	{
		mpLBStart->Enable();
		SvxLineEndListItem aItem( *(const SvxLineEndListItem*)(pSh->GetItem( SID_LINEEND_LIST ) ) );		
		mpLineEndList = aItem.GetLineEndList();
		String sNone( SVX_RES( RID_SVXSTR_NONE ) );
		//
		mpLBStart->Clear();
		mpLBEnd->Clear();
		
        if(mpLineEndList)
		{
			mpLBStart->InsertEntry( sNone );
			mpLBStart->Fill( mpLineEndList );
			mpLBStart->SelectEntryPos(0);

			mpLBEnd->InsertEntry( sNone );
			mpLBEnd->Fill( mpLineEndList, false);
			mpLBEnd->SelectEntryPos(0);
		}
	}
	else
	{
		mpLBStart->Disable();
		mpLBEnd->Disable();
	}
}



void LinePropertyPanel::SelectEndStyle(bool bStart)
{
	sal_Bool bSelected(false);

	if(bStart)
	{
		//<<add 
		if( !mbStartAvailable )
		{
			mpLBStart->SetNoSelection();
			return;
		}
		//add end>>
		if(mpStartItem && mpLineEndList)
		{
			const basegfx::B2DPolyPolygon& rItemPolygon = mpStartItem->GetLineStartValue();
			for(sal_Int32 a(0);!bSelected &&  a < mpLineEndList->Count(); a++)
			{
				XLineEndEntry* pEntry = mpLineEndList->GetLineEnd(a);
				const basegfx::B2DPolyPolygon& rEntryPolygon = pEntry->GetLineEnd();
				if(rItemPolygon == rEntryPolygon)
				{
					mpLBStart->SelectEntryPos((sal_uInt16)a + 1);
					bSelected = true;
				}
			}
		}
		if(!bSelected)
			mpLBStart->SelectEntryPos( 0 );
	}
	else
	{
		//<<add 
		if( !mbEndAvailable )
		{
			mpLBEnd->SetNoSelection();
			return;
		}
		//add end>>
		if(mpEndItem && mpLineEndList)
		{
			const basegfx::B2DPolyPolygon& rItemPolygon = mpEndItem->GetLineEndValue();
			for(sal_Int32 a(0);!bSelected &&  a < mpLineEndList->Count(); a++)
			{
				XLineEndEntry* pEntry = mpLineEndList->GetLineEnd(a);
				const basegfx::B2DPolyPolygon& rEntryPolygon = pEntry->GetLineEnd();
				if(rItemPolygon == rEntryPolygon)
				{
					mpLBEnd->SelectEntryPos((sal_uInt16)a + 1);
					bSelected = true;
				}
			}
		}
		if(!bSelected)
			mpLBEnd->SelectEntryPos( 0 );
	}
}


} } // end of namespace svx::sidebar

// eof
