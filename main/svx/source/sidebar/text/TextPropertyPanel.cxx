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

#include "precompiled_svx.hxx"

#include "TextPropertyPanel.hrc"
#include "TextPropertyPanel.hxx"
#include "SvxSBFontNameBox.hxx"

#include "svx/dialmgr.hxx"

#include <editeng/brshitem.hxx>
#include <editeng/colritem.hxx>
#include <editeng/crsditem.hxx>
#include <editeng/escpitem.hxx>
#include <editeng/flstitem.hxx>
#include <editeng/fontitem.hxx>
#include <editeng/kernitem.hxx>
#include <editeng/postitem.hxx>
#include <editeng/shdditem.hxx>
#include <editeng/udlnitem.hxx>
#include <editeng/wghtitem.hxx>
#include <rtl/ref.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/viewsh.hxx>
#include <sfx2/sidebar/propertypanel.hrc>
#include <sfx2/sidebar/ControlFactory.hxx>
#include <sfx2/sidebar/Theme.hxx>
#include <svtools/ctrltool.hxx>
#include <svtools/unitconv.hxx>

#include <vcl/gradient.hxx>
#include <vcl/svapp.hxx>


using namespace css;
using namespace cssu;
using ::sfx2::sidebar::Theme;

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

namespace svx { namespace sidebar {

#undef HAS_IA2


#define FONT_CONTROL_WIDTH 		160
#define SIZE_CONTROL_WIDTH 		80
#define CONTROL_COMBOX_HEIGHT	20
#define CONTROL_HEIGHT_5X  		120
#define SIDEBAR_SPACE_NORMAL	0
#define SIDEBAR_SPACE_EXPAND	1
#define SIDEBAR_SPACE_CONDENSED	2
#define SIDEBAR_SPACING_GLOBAL_VALUE	String("PopupPanal_Spacing", 18, RTL_TEXTENCODING_ASCII_US)
#define TEXT_SECTIONPAGE_HEIGHT_S   SECTIONPAGE_MARGIN_VERTICAL_TOP + CBOX_HEIGHT  + ( TOOLBOX_ITEM_HEIGHT + 2 ) + CONTROL_SPACING_VERTICAL * 1 + SECTIONPAGE_MARGIN_VERTICAL_BOT
#define TEXT_SECTIONPAGE_HEIGHT		SECTIONPAGE_MARGIN_VERTICAL_TOP + CBOX_HEIGHT  + ( TOOLBOX_ITEM_HEIGHT + 2 ) * 2 + CONTROL_SPACING_VERTICAL * 2 + SECTIONPAGE_MARGIN_VERTICAL_BOT

//
#define SPACING_NOCUSTOM				0
#define SPACING_CLOSE_BY_CLICK_ICON		-1
#define SPACING_CLOSE_BY_CUS_EDIT		1
//end





Reference<ui::XUIElement> TextPropertyPanel::Create (
    const ::rtl::OUString& rsResourceURL,
    Window* pParent,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings)
{
    if (pParent == NULL)
        throw lang::IllegalArgumentException(A2S("no parent Window given to TextPropertyPanel::Create"), NULL, 0);
    if ( ! rxFrame.is())
        throw lang::IllegalArgumentException(A2S("no XFrame given to TextPropertyPanel::Create"), NULL, 1);
    if (pBindings == NULL)
        throw lang::IllegalArgumentException(A2S("no SfxBindings given to TextPropertyPanel::Create"), NULL, 2);
    
    ::rtl::Reference<TextPropertyPanel> pPanel (
        new TextPropertyPanel(
            rsResourceURL,
            pParent,
            rxFrame,
            pBindings));
    return Reference<ui::XUIElement>(static_cast<XWeak*>(pPanel.get()), UNO_QUERY);
}




TextPropertyPanel::TextPropertyPanel (
    const ::rtl::OUString& rsResourceURL,
    Window* pParent,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings)
    :	::sfx2::sidebar::SidebarPanelBase(rsResourceURL, pParent, rxFrame, SVX_RES(RID_SIDEBAR_TEXT_PANEL)),
        mpFontNameBox (new SvxSBFontNameBox(this, SVX_RES(CB_SBFONT_FONT))),
    	maFontSizeBox		(this, SVX_RES(MB_SBFONT_FONTSIZE)),
    	mpToolBoxIncDec(sfx2::sidebar::ControlFactory::CreateToolBox(this, SVX_RES(TB_INCREASE_DECREASE))),
        mpToolBoxFont(sfx2::sidebar::ControlFactory::CreateToolBox(this, SVX_RES(TB_FONT))),
        mpToolBoxFontColor(sfx2::sidebar::ControlFactory::CreateToolBox(this, SVX_RES(TB_FONTCOLOR))),		
        mpToolBoxScript(sfx2::sidebar::ControlFactory::CreateToolBox(this, SVX_RES(TB_SCRIPT))),
        mpToolBoxScriptSw(sfx2::sidebar::ControlFactory::CreateToolBox(this, SVX_RES(TB_SCRIPT_SW))),
        mpToolBoxSpacing(sfx2::sidebar::ControlFactory::CreateToolBox(this, SVX_RES(TB_SPACING))),
        mpToolBoxHighlight(sfx2::sidebar::ControlFactory::CreateToolBox(this, SVX_RES(TB_HIGHLIGHT))),
        mpFontColorUpdater(),
        mpHighlightUpdater(),

        maFontNameControl	(SID_ATTR_CHAR_FONT,		*pBindings, *this),
        maFontSizeControl	(SID_ATTR_CHAR_FONTHEIGHT,	*pBindings, *this),
        maWeightControl		(SID_ATTR_CHAR_WEIGHT,		*pBindings, *this),
        maItalicControl		(SID_ATTR_CHAR_POSTURE,		*pBindings, *this),
        maUnderlineControl	(SID_ATTR_CHAR_UNDERLINE,	*pBindings, *this),
        maStrikeControl		(SID_ATTR_CHAR_STRIKEOUT,	*pBindings, *this),
        maShadowControl		(SID_ATTR_CHAR_SHADOWED,	*pBindings, *this),
        maFontColorControl	(SID_ATTR_CHAR_COLOR,		*pBindings, *this),
        maScriptControlSw	(SID_ATTR_CHAR_ESCAPEMENT,	*pBindings, *this),  //for sw
        maSuperScriptControl (SID_SET_SUPER_SCRIPT,		*pBindings, *this),
        maSubScriptControl	(SID_SET_SUB_SCRIPT,		*pBindings, *this),
        maSpacingControl	(SID_ATTR_CHAR_KERNING,		*pBindings, *this),
        maHighlightControl	(SID_ATTR_BRUSH_CHAR,		*pBindings, *this),
        maSDFontGrow		(SID_GROW_FONT_SIZE,		*pBindings, *this),
        maSDFontShrink		(SID_SHRINK_FONT_SIZE,		*pBindings, *this),

        maImgIncrease		(SVX_RES( IMG_INCREASE)),
        maImgDecrease		(SVX_RES( IMG_DECREASE)),
        maImgBold			(SVX_RES( IMG_BOLD )),
        maImgItalic			(SVX_RES( IMG_ITALIC )),
        maImgUnderline		(SVX_RES( IMG_UNDERLINE )),
        maImgStrike			(SVX_RES( IMG_STRIKEOUT )),
        maImgShadow			(SVX_RES( IMG_SHADOWED )),
        maImgFontColor		(SVX_RES( IMG_FONTCOLOR)),
        maImgSupScript		(SVX_RES( IMG_SUPSCRIPT)),
        maImgSubScript		(SVX_RES( IMG_SUBSCRIPT)),
        maImgHighlight		(SVX_RES( IMG_HIGHLIGHT)),

        maImgNormalIcon		(SVX_RES(IMG_SPACING_D)),

        maImgIncreaseHigh	(SVX_RES( IMG_INCREASE_H )),
        maImgDecreaseHigh	(SVX_RES( IMG_DECREASE_H )),
        maImgBoldHigh		(SVX_RES( IMG_BOLD_H )),
        maImgItalicHigh		(SVX_RES( IMG_ITALIC_H )),
        maImgUnderlineHigh	(SVX_RES( IMG_UNDERLINE_H )),
        maImgStrikeHigh		(SVX_RES( IMG_STRIKEOUT_H )),
        maImgShadowHigh		(SVX_RES( IMG_SHADOWED_H )),
        maImgFontColorHigh	(SVX_RES( IMG_FONTCOLOR_H)),
        maImgSupScriptHigh	(SVX_RES( IMG_SUPSCRIPT_H)),
        maImgSubScriptHigh	(SVX_RES( IMG_SUBSCRIPT_H)),
        maImgHighlightHigh	(SVX_RES( IMG_HIGHLIGHT_H)),

        mpFontList			(NULL),
        mbMustDelete		(false),
        mbFocusOnFontSizeCtrl(false),
        /*AF
        mpFloatWinUnderline(NULL),
        mpPageUnderline(NULL),
        mpFloatWinFontColor(NULL),
        mpPageFontColor(NULL),
        mpFloatWinSpacing(NULL),
        mpPageSpacing(NULL)
        */
        maContext(),
        mpBindings(pBindings)
{
	Initialize();
	FreeResource();
}




TextPropertyPanel::~TextPropertyPanel (void)
{
    if(mbMustDelete)
        delete mpFontList;

    /*AF
	delete mpPageUnderline;
	delete mpFloatWinUnderline;	

	delete mpPageFontColor;
	delete mpFloatWinFontColor;

	delete mpPageSpacing;
	delete mpFloatWinSpacing;
    */
}




void TextPropertyPanel::HandleContextChange (
    const ::sfx2::sidebar::EnumContext aContext)
{
    if (maContext == aContext)
    {
        // Nothing to do.
        return;
    }

    maContext = aContext;
    switch (maContext.GetCombinedContext())
    {
        case CombinedEnumContext(Application_Calc, Context_Cell):
        case CombinedEnumContext(Application_Calc, Context_Pivot):
        {
            mpToolBoxScript->Hide();
		    mpToolBoxScriptSw->Hide();
            mpToolBoxSpacing->Hide();
            mpToolBoxHighlight->Hide();
			
            Size aSize(PROPERTYPAGE_WIDTH,TEXT_SECTIONPAGE_HEIGHT_S);
            aSize = LogicToPixel( aSize, MapMode(MAP_APPFONT) ); 
            aSize.setWidth(GetOutputSizePixel().Width());
            SetSizePixel(aSize);
            break;
        }

        case CombinedEnumContext(Application_Writer, Context_Text):
        case CombinedEnumContext(Application_Writer, Context_Table):
        {
            mpToolBoxScriptSw->Show();
            mpToolBoxScript->Hide();
            mpToolBoxHighlight->Show();
            mpToolBoxSpacing->Show();

            Size aSize(PROPERTYPAGE_WIDTH, TEXT_SECTIONPAGE_HEIGHT);
            aSize = LogicToPixel( aSize, MapMode(MAP_APPFONT) ); 
            aSize.setWidth(GetOutputSizePixel().Width());
            SetSizePixel(aSize);
            break;
        }

        case CombinedEnumContext(Application_Writer, Context_DrawText):
        case CombinedEnumContext(Application_Writer, Context_Annotation):
        {
            mpToolBoxScriptSw->Show();
            mpToolBoxScript->Hide();
            mpToolBoxSpacing->Show();
            mpToolBoxHighlight->Hide();

            Size aSize(PROPERTYPAGE_WIDTH,TEXT_SECTIONPAGE_HEIGHT);
            aSize = LogicToPixel( aSize, MapMode(MAP_APPFONT) ); 
            aSize.setWidth(GetOutputSizePixel().Width());
            SetSizePixel(aSize);
            break;
        }

        case CombinedEnumContext(Application_Calc, Context_EditCell):
        case CombinedEnumContext(Application_Calc, Context_DrawText):
        case CombinedEnumContext(Application_Draw, Context_DrawText):
        case CombinedEnumContext(Application_Draw, Context_Text):
        case CombinedEnumContext(Application_Draw, Context_Table):
        case CombinedEnumContext(Application_Draw, Context_OutlineText):
        case CombinedEnumContext(Application_Draw, Context_Draw):
        case CombinedEnumContext(Application_Draw, Context_TextObject):
        case CombinedEnumContext(Application_Draw, Context_Graphic):
        case CombinedEnumContext(Application_Impress, Context_DrawText):
        case CombinedEnumContext(Application_Impress, Context_Text):
        case CombinedEnumContext(Application_Impress, Context_Table):
        case CombinedEnumContext(Application_Impress, Context_OutlineText):
        case CombinedEnumContext(Application_Impress, Context_Draw):
        case CombinedEnumContext(Application_Impress, Context_TextObject):
        case CombinedEnumContext(Application_Impress, Context_Graphic):
        {
            mpToolBoxScriptSw->Hide();
            mpToolBoxScript->Show();
            mpToolBoxSpacing->Show();
            mpToolBoxHighlight->Hide();

            Size aSize(PROPERTYPAGE_WIDTH,TEXT_SECTIONPAGE_HEIGHT);
            aSize = LogicToPixel( aSize,MapMode(MAP_APPFONT) ); 
            aSize.setWidth(GetOutputSizePixel().Width());
            SetSizePixel(aSize);
            break;
        }

        default:
            break;
    }
}




void TextPropertyPanel::PaintRect(const ToolBox &rTB)
{
	Point aPos = rTB.GetPosPixel();	
	Size aSize = rTB.GetSizePixel();
	Rectangle aRect( aPos, aSize );
	aRect.Left() -= 2;
	aRect.Top() -= 1;
	aRect.Right() += 2;
	aRect.Bottom() += 1;
	Color aOldLineColor = GetLineColor();
	Color aOldFillColor = GetFillColor();
	Color aLineColor(200,209,225);
//	Color aLineColor = GetSettings().GetStyleSettings().GetPropertySectionTBxBorderColor();
	if(!GetSettings().GetStyleSettings().GetHighContrastMode())	
		SetLineColor(aLineColor);
	else
		SetLineColor(GetSettings().GetStyleSettings().GetShadowColor());
	SetFillColor(COL_TRANSPARENT);
	DrawRect(aRect);

	aRect.Left() += 1 ;
	aRect.Top() += 1;
	aRect.Right() -= 1;
	aRect.Bottom() -= 1;
	Color aStartColor(220,228,238);
	Color aEndColor(245,245,247);
	Gradient aBKGrad(GRADIENT_LINEAR, aStartColor, aEndColor);
	if(!GetSettings().GetStyleSettings().GetHighContrastMode())	
//	DrawGradient(aRect, GetSettings().GetStyleSettings().GetPropertySectionTBxBKGGradient());
		DrawGradient(aRect, aBKGrad);
	//else
	//{
	//	SetFillColor(GetSettings().GetStyleSettings().GetMenuColor());
	//	DrawRect(aRect);
	//}

	SetLineColor(aOldLineColor);
	SetFillColor(aOldFillColor);
}




void TextPropertyPanel::Paint(const Rectangle &rRect)
{	
	SidebarPanelBase::Paint(rRect);

	if(mpFontColorUpdater == NULL && mpHighlightUpdater == NULL)
		return;
	
	//size increase & decrease
    //AF	PaintRect(*mpToolBoxIncDec);

	//Font Toolbox
	PaintRect(*mpToolBoxFont);

	//font color toolbox
	PaintRect(*mpToolBoxFontColor);

	//script for sw
    switch (maContext.GetCombinedContext())
    {
        case CombinedEnumContext(Application_Writer, Context_Text):
        case CombinedEnumContext(Application_Writer, Context_Table):
            PaintRect(*mpToolBoxHighlight);
            // fall through
        case CombinedEnumContext(Application_Writer, Context_DrawText):
        case CombinedEnumContext(Application_Writer, Context_Annotation):
            PaintRect(*mpToolBoxScriptSw);
            break;

        case CombinedEnumContext(Application_Calc, Context_EditCell):
        case CombinedEnumContext(Application_Calc, Context_DrawText):
        case CombinedEnumContext(Application_Draw, Context_DrawText):
        case CombinedEnumContext(Application_Draw, Context_Text):
        case CombinedEnumContext(Application_Draw, Context_Table):
        case CombinedEnumContext(Application_Draw, Context_OutlineText):
        case CombinedEnumContext(Application_Draw, Context_Draw):
        case CombinedEnumContext(Application_Draw, Context_TextObject):
        case CombinedEnumContext(Application_Draw, Context_Graphic):
        case CombinedEnumContext(Application_Impress, Context_DrawText):
        case CombinedEnumContext(Application_Impress, Context_Text):
        case CombinedEnumContext(Application_Impress, Context_Table):
        case CombinedEnumContext(Application_Impress, Context_OutlineText):
        case CombinedEnumContext(Application_Impress, Context_Draw):
        case CombinedEnumContext(Application_Impress, Context_TextObject):
        case CombinedEnumContext(Application_Impress, Context_Graphic):
            PaintRect(*mpToolBoxScript);
            break;
    }

    if (maContext.GetCombinedContext() != CombinedEnumContext(Application_Calc, Context_Cell))
	{
		PaintRect(*mpToolBoxSpacing);
	}
}




void TextPropertyPanel::DataChanged (const DataChangedEvent& rEvent)
{
    SetupIcons();
}



void TextPropertyPanel::Initialize (void)
{
	//<<modify fill font list
	SfxObjectShell* pDocSh = SfxObjectShell::Current();
    const SfxPoolItem* pItem = NULL;
    
    if (pDocSh != NULL)
        pItem = pDocSh->GetItem( SID_ATTR_CHAR_FONTLIST );
    if (pItem != NULL)
        mpFontList = ( (SvxFontListItem*)pItem )->GetFontList();
    else
    {
        mpFontList = new FontList( Application::GetDefaultDevice() );
		mbMustDelete = 1;
    }

    mpFontNameBox->SetAccessibleName(mpFontNameBox->GetQuickHelpText());
    maFontSizeBox.Fill( &mpFontList->Get( String::CreateFromAscii( "" ), String::CreateFromAscii( "" )),mpFontList );
	maFontSizeBox.SetAccessibleName(maFontSizeBox.GetQuickHelpText());

	//toolbox
    SetupIcons();
	InitToolBoxIncDec();
	InitToolBoxFont();
	InitToolBoxFontColor();
	InitToolBoxScript();
	InitToolBoxSpacing();
	InitToolBoxHighlight();

#ifdef HAS_IA2
	mpFontNameBox->SetAccRelationLabeledBy(&maFontNameBox);
    mpFontNameBox->SetMpSubEditAccLableBy(&maFontNameBox);
    maFontSizeBox.SetAccRelationLabeledBy(&maFontSizeBox);
	maFontSizeBox.SetMpSubEditAccLableBy(&maFontSizeBox);
	maToolBoxFont.SetAccRelationLabeledBy(&maToolBoxFont);
	maToolBoxIncDec.SetAccRelationLabeledBy(&maToolBoxIncDec);
	maToolBoxFontColor.SetAccRelationLabeledBy(&maToolBoxFontColor);
	maToolBoxScript.SetAccRelationLabeledBy(&maToolBoxScript);
	maToolBoxScriptSw.SetAccRelationLabeledBy(&maToolBoxScriptSw);
	maToolBoxSpacing.SetAccRelationLabeledBy(&maToolBoxSpacing);
	maToolBoxHighlight.SetAccRelationLabeledBy(&maToolBoxHighlight);
#endif

	//init state
	mpHeightItem = NULL;
	meWeight = WEIGHT_NORMAL;
	meItalic = ITALIC_NONE;
	mbShadow = false;
	meStrike = STRIKEOUT_NONE;
	mbPostureAvailable = true;
	mbWeightAvailable = true;
	meUnderline = UNDERLINE_NONE;
	meUnderlineColor = COL_AUTO;   //
	maColor = COL_BLACK;
	mbColorAvailable = true;
	maBackColor = COL_AUTO;
	mbBackColorAvailable = true;
	meColorType = FONT_COLOR;
	meEscape = SVX_ESCAPEMENT_OFF;
	mbSuper = false;
	mbSub = false;
	mbKernAvailable = true;
	mbKernLBAvailable = true;
	mlKerning = 0;
	mpFontColorUpdater.reset(new ToolboxButtonColorUpdater(
            SID_ATTR_CHAR_COLOR,
            TBI_FONTCOLOR,
            mpToolBoxFontColor.get(),
            TBX_UPDATER_MODE_CHAR_COLOR_NEW));
    mpHighlightUpdater.reset(new ToolboxButtonColorUpdater(
            SID_ATTR_BRUSH_CHAR,
            TBI_HIGHLIGHT,
            mpToolBoxHighlight.get(),
            TBX_UPDATER_MODE_CHAR_COLOR_NEW));

	//set handler
	mpFontNameBox->SetBindings(mpBindings);
	//add 
	Link aLink = LINK(this, TextPropertyPanel, FontSelHdl);
	mpFontNameBox->SetSelectHdl(aLink);
	//add end
	
	aLink = LINK(this, TextPropertyPanel, FontSizeModifyHdl);
	maFontSizeBox.SetModifyHdl(aLink);
	//add 
	aLink = LINK(this, TextPropertyPanel, FontSizeSelHdl);
	maFontSizeBox.SetSelectHdl(aLink);
	//add end
	aLink = LINK(this, TextPropertyPanel, FontSizeLoseFocus);
	maFontSizeBox.SetLoseFocusHdl(aLink);

// add
	long aSizeBoxHeight = maFontSizeBox.GetSizePixel().getHeight();;
	Point aPosFontSize = maFontSizeBox.GetPosPixel();
	long aPosY = aPosFontSize.getY();
	Point pTBIncDec = mpToolBoxIncDec->GetPosPixel();
	long aIncDecHeight = mpToolBoxIncDec->GetSizePixel().getHeight();
	pTBIncDec.setY(aPosY+aSizeBoxHeight/2-aIncDecHeight/2);
	mpToolBoxIncDec->SetPosPixel(pTBIncDec);
	//end
}




void TextPropertyPanel::InitToolBoxFont()
{
	mpToolBoxFont->SetQuickHelpText(TBI_BOLD,String(SVX_RES(STR_QH_BOLD))); //Add
	mpToolBoxFont->SetQuickHelpText(TBI_ITALIC,String(SVX_RES(STR_QH_ITALIC))); //Add
	mpToolBoxFont->SetQuickHelpText(TBI_UNDERLINE,String(SVX_RES(STR_QH_UNDERLINE))); //Add
	mpToolBoxFont->SetBackground(Wallpaper());
	mpToolBoxFont->SetPaintTransparent(true);

	Size aTbxSize( mpToolBoxFont->CalcWindowSizePixel() );
	mpToolBoxFont->SetOutputSizePixel( aTbxSize );

	Link aLink  = LINK(this, TextPropertyPanel, ToolboxFontSelectHandler);
	mpToolBoxFont->SetSelectHdl ( aLink );
	aLink = LINK(this, TextPropertyPanel, ToolBoxUnderlineClickHdl);
	mpToolBoxFont->SetDropdownClickHdl(aLink);
}




void TextPropertyPanel::InitToolBoxIncDec()
{
	Size aTbxSize( mpToolBoxIncDec->CalcWindowSizePixel() );
	mpToolBoxIncDec->SetOutputSizePixel( aTbxSize );

	Link aLink = LINK(this, TextPropertyPanel, ToolboxIncDecSelectHdl);
    mpToolBoxIncDec->SetSelectHdl ( aLink );
}




void TextPropertyPanel::InitToolBoxFontColor()
{
	Size aTbxSize( mpToolBoxFontColor->CalcWindowSizePixel() );
	mpToolBoxFontColor->SetOutputSizePixel( aTbxSize );
	mpToolBoxFontColor->SetItemBits( TBI_FONTCOLOR, mpToolBoxFontColor->GetItemBits( TBI_FONTCOLOR ) | TIB_DROPDOWNONLY );
	mpToolBoxFontColor->SetBackground(Wallpaper());
	mpToolBoxFontColor->SetPaintTransparent(true);
	
	Link aLink = LINK(this, TextPropertyPanel, ToolBoxFontColorDropHdl);
    mpToolBoxFontColor->SetDropdownClickHdl ( aLink );
	mpToolBoxFontColor->SetSelectHdl ( aLink );

}
void TextPropertyPanel::InitToolBoxScript()
{
	Size aTbxSize( mpToolBoxScriptSw->CalcWindowSizePixel() );
	mpToolBoxScriptSw->SetOutputSizePixel( aTbxSize );
	mpToolBoxScriptSw->SetBackground(Wallpaper());
	mpToolBoxScriptSw->SetPaintTransparent(true);
	
	Link aLink = LINK(this, TextPropertyPanel, ToolBoxSwScriptSelectHdl);
    mpToolBoxScriptSw->SetSelectHdl ( aLink );

	aTbxSize = mpToolBoxScript->CalcWindowSizePixel() ;
	mpToolBoxScript->SetOutputSizePixel( aTbxSize );
	mpToolBoxScript->SetBackground(Wallpaper());
	mpToolBoxScript->SetPaintTransparent(true);

	aLink = LINK(this, TextPropertyPanel, ToolBoxScriptSelectHdl);
    mpToolBoxScript->SetSelectHdl ( aLink );
}
void TextPropertyPanel::InitToolBoxSpacing()
{
	Size aTbxSize( mpToolBoxSpacing->CalcWindowSizePixel() );
	mpToolBoxSpacing->SetOutputSizePixel( aTbxSize );
	mpToolBoxSpacing->SetItemBits( TBI_SPACING, mpToolBoxSpacing->GetItemBits( TBI_SPACING ) | TIB_DROPDOWNONLY );
	mpToolBoxSpacing->SetBackground(Wallpaper());
	mpToolBoxSpacing->SetPaintTransparent(true);

	Link aLink = LINK(this, TextPropertyPanel, SpacingClickHdl);
    mpToolBoxSpacing->SetDropdownClickHdl ( aLink );
	mpToolBoxSpacing->SetSelectHdl( aLink );
}
void TextPropertyPanel::InitToolBoxHighlight()
{
	Size aTbxSize( mpToolBoxHighlight->CalcWindowSizePixel() );
	mpToolBoxHighlight->SetOutputSizePixel( aTbxSize );
	mpToolBoxHighlight->SetItemBits( TBI_HIGHLIGHT, mpToolBoxHighlight->GetItemBits( TBI_HIGHLIGHT ) | TIB_DROPDOWNONLY );
	mpToolBoxHighlight->SetBackground(Wallpaper());
	mpToolBoxHighlight->SetPaintTransparent(true);

	Link aLink = LINK(this, TextPropertyPanel, ToolBoxHighlightDropHdl);
    mpToolBoxHighlight->SetDropdownClickHdl ( aLink );
	mpToolBoxHighlight->SetSelectHdl( aLink );
}




void TextPropertyPanel::SetupIcons (void)
{
    if (Theme::GetBoolean(Theme::Bool_UseSymphonyIcons))
    {
    	mpToolBoxIncDec->SetItemImage(TBI_INCREASE, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? maImgIncreaseHigh : maImgIncrease);
        mpToolBoxIncDec->SetItemImage(TBI_DECREASE, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? maImgDecreaseHigh : maImgDecrease);
        mpToolBoxFont->SetItemImage(TBI_BOLD, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? maImgBoldHigh : maImgBold);
        mpToolBoxFont->SetItemImage(TBI_ITALIC, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? maImgItalicHigh : maImgItalic);
        mpToolBoxFont->SetItemImage(TBI_UNDERLINE, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? maImgUnderlineHigh : maImgUnderline);
        mpToolBoxFont->SetItemImage(TBI_STRIKEOUT, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? maImgStrikeHigh : maImgStrike);
        mpToolBoxFont->SetItemImage(TBI_SHADOWED, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? maImgShadowHigh : maImgShadow);

    	mpToolBoxFontColor->SetItemImage(TBI_FONTCOLOR, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? maImgFontColorHigh : maImgFontColor);
        //for sw
        mpToolBoxScriptSw->SetItemImage(TBI_SUPER_SW, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? maImgSupScriptHigh : maImgSupScript);
        mpToolBoxScriptSw->SetItemImage(TBI_SUB_SW, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? maImgSubScriptHigh : maImgSubScript);
        //for sc and sd
        mpToolBoxScript->SetItemImage(TBI_SUPER, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? maImgSupScriptHigh : maImgSupScript);
        mpToolBoxScript->SetItemImage(TBI_SUB, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? maImgSubScriptHigh : maImgSubScript);
        mpToolBoxSpacing->SetItemImage(TBI_SPACING,  maImgNormalIcon);
        mpToolBoxHighlight->SetItemImage(TBI_HIGHLIGHT, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? maImgHighlightHigh : maImgHighlight);
    }
    else
    {
        mpToolBoxIncDec->SetItemImage(TBI_INCREASE, GetIcon(A2S(".uno:Grow")));
        mpToolBoxIncDec->SetItemImage(TBI_DECREASE, GetIcon(A2S(".uno:Shrink")));
        mpToolBoxFont->SetItemImage(TBI_BOLD, GetIcon(A2S(".uno:Bold")));
        mpToolBoxFont->SetItemImage(TBI_ITALIC, GetIcon(A2S(".uno:Italic")));
        mpToolBoxFont->SetItemImage(TBI_UNDERLINE, GetIcon(A2S(".uno:Underline")));
        mpToolBoxFont->SetItemImage(TBI_STRIKEOUT, GetIcon(A2S(".uno:Strikeout")));
        mpToolBoxFont->SetItemImage(TBI_SHADOWED, GetIcon(A2S(".uno:Shadowed")));

    	mpToolBoxFontColor->SetItemImage(TBI_FONTCOLOR, GetIcon(A2S(".uno:FontColor")));
        //for sw
        mpToolBoxScriptSw->SetItemImage(TBI_SUPER_SW, GetIcon(A2S(".uno:SuperScript")));
        mpToolBoxScriptSw->SetItemImage(TBI_SUB_SW, GetIcon(A2S(".uno:SubScript")));
        //for sc and sd
        mpToolBoxScript->SetItemImage(TBI_SUPER, GetIcon(A2S(".uno:SuperScript")));
        mpToolBoxScript->SetItemImage(TBI_SUB, GetIcon(A2S(".uno:SubScript")));
        mpToolBoxSpacing->SetItemImage(TBI_SPACING,  GetIcon(A2S(".uno:FontworkCharacterSpacingFloater")));
        mpToolBoxHighlight->SetItemImage(TBI_HIGHLIGHT, GetIcon(A2S(".uno:BackColor")));
    }
}




IMPL_LINK( TextPropertyPanel, FontSelHdl, FontNameBox*, pBox )
{
	if ( !pBox->IsTravelSelect() )
	{
		if( SfxViewShell::Current() )
		{
			Window* pShellWnd = SfxViewShell::Current()->GetWindow();

			if ( pShellWnd )
				pShellWnd->GrabFocus();
		}
	}
	return 0;
}
//add end
IMPL_LINK( TextPropertyPanel, FontSizeModifyHdl, FontSizeBox*, pSizeBox )
{
	if (pSizeBox == &maFontSizeBox)
	{	
		long nSize = pSizeBox->GetValue();
		mbFocusOnFontSizeCtrl = true;

		float fSize = (float)nSize / 10;
		SfxMapUnit eUnit = maFontSizeControl.GetCoreMetric();
		SvxFontHeightItem aItem( CalcToUnit( fSize, eUnit ), 100, SID_ATTR_CHAR_FONTHEIGHT ) ;

		mpBindings->GetDispatcher()->Execute( SID_ATTR_CHAR_FONTHEIGHT, SFX_CALLMODE_RECORD, &aItem, 0L );
		mpBindings->Invalidate(SID_ATTR_CHAR_FONTHEIGHT,true,false);
	}
	return 0;
}
//add 
IMPL_LINK( TextPropertyPanel, FontSizeSelHdl, FontSizeBox*, pSizeBox )
{
	if ( !pSizeBox->IsTravelSelect() )
	{
		if( SfxViewShell::Current() )
		{
			Window* pShellWnd = SfxViewShell::Current()->GetWindow();

			if ( pShellWnd )
				pShellWnd->GrabFocus();
		}
	}
	
	return 0;
}
//add end
IMPL_LINK(TextPropertyPanel, FontSizeLoseFocus, FontSizeBox*, pSizeBox)
{
	if(pSizeBox == &maFontSizeBox)
	{
		mbFocusOnFontSizeCtrl = false;
	}
	return 0;
}

IMPL_LINK(TextPropertyPanel, ToolboxFontSelectHandler, ToolBox*, pToolBox)
{
	const sal_uInt16 nId = pToolBox->GetCurItemId();

	//Bold
	if(nId == TBI_BOLD)
	{
		EndTracking();
		if(meWeight != WEIGHT_BOLD)
			meWeight = WEIGHT_BOLD;
		else
			meWeight = WEIGHT_NORMAL;
		SvxWeightItem aWeightItem(meWeight, SID_ATTR_CHAR_WEIGHT);
		mpBindings->GetDispatcher()->Execute(SID_ATTR_CHAR_WEIGHT, SFX_CALLMODE_RECORD, &aWeightItem, 0L);
		UpdateFontBold();
	}
	//Italic
	else if(nId == TBI_ITALIC)
	{
		EndTracking();
		if(meItalic != ITALIC_NORMAL)
			meItalic = ITALIC_NORMAL;
		else
			meItalic = ITALIC_NONE;
		SvxPostureItem aPostureItem(meItalic, SID_ATTR_CHAR_POSTURE);
		mpBindings->GetDispatcher()->Execute(SID_ATTR_CHAR_POSTURE, SFX_CALLMODE_RECORD, &aPostureItem, 0L);
		UpdateFontItalic();
	}
	//underline
	else if(nId == TBI_UNDERLINE)
	{
		EndTracking();
		//add , keep underline's color
		if(meUnderline == UNDERLINE_NONE)
		{
			meUnderline = GetDefaultUnderline();
			//<<modify 
			//SvxTextLineItem aLineItem(meUnderline, SID_ATTR_CHAR_UNDERLINE);
			SvxUnderlineItem aLineItem(meUnderline, SID_ATTR_CHAR_UNDERLINE);
			//modify end>>
			aLineItem.SetColor(meUnderlineColor);
			mpBindings->GetDispatcher()->Execute(SID_ATTR_CHAR_UNDERLINE, SFX_CALLMODE_RECORD, &aLineItem, 0L);
		}
		else
		{
			meUnderline = UNDERLINE_NONE;
			//<<modify 
			//SvxTextLineItem aLineItem(meUnderline, SID_ATTR_CHAR_UNDERLINE);
			SvxUnderlineItem aLineItem(meUnderline, SID_ATTR_CHAR_UNDERLINE);
			//modify end>>
			mpBindings->GetDispatcher()->Execute(SID_ATTR_CHAR_UNDERLINE, SFX_CALLMODE_RECORD, &aLineItem, 0L);
		}
		UpdateFontUnderline();
		//add end
	}
	//strike out
	else if(nId == TBI_STRIKEOUT)
	{
		EndTracking();
		if(meStrike !=  STRIKEOUT_NONE && meStrike != STRIKEOUT_DONTKNOW)
			meStrike = STRIKEOUT_NONE;
		else
			meStrike = STRIKEOUT_SINGLE;
		SvxCrossedOutItem aStrikeItem(meStrike,SID_ATTR_CHAR_STRIKEOUT);
		mpBindings->GetDispatcher()->Execute(SID_ATTR_CHAR_STRIKEOUT, SFX_CALLMODE_RECORD, &aStrikeItem, 0L);
		UpdateFontStrikeOut();
	}
	//shadowed
	else if(nId == TBI_SHADOWED)
	{
		EndTracking();
		mbShadow = !mbShadow;
		SvxShadowedItem aShadowItem(mbShadow, SID_ATTR_CHAR_SHADOWED);
		mpBindings->GetDispatcher()->Execute(SID_ATTR_CHAR_SHADOWED, SFX_CALLMODE_RECORD, &aShadowItem, 0L);
		UpdateFontShadowed();
	}
	return 0;
}




IMPL_LINK(TextPropertyPanel, ToolboxIncDecSelectHdl, ToolBox*, pToolBox)
{
	const sal_uInt16 nId = pToolBox->GetCurItemId();

	// font size +/- enhancement in sd
    switch (maContext.GetCombinedContext())
    {
        case CombinedEnumContext(Application_Draw, Context_DrawText):
        case CombinedEnumContext(Application_Draw, Context_Text):
        case CombinedEnumContext(Application_Draw, Context_Table):
        case CombinedEnumContext(Application_Draw, Context_OutlineText):
        case CombinedEnumContext(Application_Draw, Context_Draw):
        case CombinedEnumContext(Application_Draw, Context_TextObject):
        case CombinedEnumContext(Application_Draw, Context_Graphic):
        case CombinedEnumContext(Application_Impress, Context_DrawText):
        case CombinedEnumContext(Application_Impress, Context_Text):
        case CombinedEnumContext(Application_Impress, Context_Table):
        case CombinedEnumContext(Application_Impress, Context_OutlineText):
        case CombinedEnumContext(Application_Impress, Context_Draw):
        case CombinedEnumContext(Application_Impress, Context_TextObject):
        case CombinedEnumContext(Application_Impress, Context_Graphic):
            if(nId == TBI_INCREASE)
            {
                EndTracking();
                SfxVoidItem aItem(SID_GROW_FONT_SIZE);
                mpBindings->GetDispatcher()->Execute( SID_GROW_FONT_SIZE, SFX_CALLMODE_RECORD, &aItem, 0L );
            }
            else if(nId == TBI_DECREASE)
            {
                EndTracking();
                SfxVoidItem aItem(SID_SHRINK_FONT_SIZE);
                mpBindings->GetDispatcher()->Execute( SID_SHRINK_FONT_SIZE, SFX_CALLMODE_RECORD, &aItem, 0L );
            }
            break;

        default:
            if(nId == TBI_INCREASE)
            {
                EndTracking();
                mbFocusOnFontSizeCtrl = false;
                sal_Int64 iValue = maFontSizeBox.GetValue();
                int iPos = maFontSizeBox.GetValuePos(iValue, FUNIT_NONE);
                long nSize = iValue;
                if(iPos != LISTBOX_ENTRY_NOTFOUND)
                    nSize = maFontSizeBox.GetValue(iPos+1 , FUNIT_NONE);
                else if(iValue >= 100 && iValue < 105)
                    nSize = 105;
                else if(iValue >= 105 && iValue < 110)
                    nSize = 110;
                else if(iValue < 960)
                {	
                    nSize = (nSize / 10) * 10 + 10;
                    while(maFontSizeBox.GetValuePos(nSize, FUNIT_NONE) == LISTBOX_ENTRY_NOTFOUND)
                        nSize += 10;
                }
                else
                {
                    nSize = iValue;
                }

                float fSize = (float)nSize / 10;

                SfxMapUnit eUnit = maFontSizeControl.GetCoreMetric();
                SvxFontHeightItem aItem( CalcToUnit( fSize, eUnit ), 100, SID_ATTR_CHAR_FONTHEIGHT ) ;

                mpBindings->GetDispatcher()->Execute( SID_ATTR_CHAR_FONTHEIGHT, SFX_CALLMODE_RECORD, &aItem, 0L );
                mpBindings->Invalidate(SID_ATTR_CHAR_FONTHEIGHT,true,false);
                //add  , update ASAP
                maFontSizeBox.SetValue( nSize );
                if(nSize >= 960)
                {
                    mpToolBoxIncDec->EnableItem(TBI_INCREASE,false);
                    mpToolBoxIncDec->EnableItem(TBI_DECREASE,true);
                }
                else if(nSize <= 60)
                {
                    mpToolBoxIncDec->EnableItem(TBI_INCREASE,true);
                    mpToolBoxIncDec->EnableItem(TBI_DECREASE,false);
                }
                else
                {
                    mpToolBoxIncDec->EnableItem(TBI_INCREASE,true);
                    mpToolBoxIncDec->EnableItem(TBI_DECREASE,true);
                }
                //add end
            }
            else if(nId == TBI_DECREASE)
            {
                EndTracking();
                mbFocusOnFontSizeCtrl = false;
                sal_Int64 iValue = maFontSizeBox.GetValue();
                int iPos = maFontSizeBox.GetValuePos(iValue, FUNIT_NONE);
                long nSize = iValue;
                if(iPos != LISTBOX_ENTRY_NOTFOUND)
                    nSize = maFontSizeBox.GetValue(iPos-1 , FUNIT_NONE);
                else if(iValue > 100 && iValue <= 105)
                    nSize = 100;
                else if(iValue > 105 && iValue <= 110)
                    nSize = 105;
                else if(iValue > 960)
                {
                    nSize = 960;
                }
                else if(iValue > 60)
                {	
                    nSize = (nSize / 10) * 10 ;
                    while(maFontSizeBox.GetValuePos(nSize, FUNIT_NONE) == LISTBOX_ENTRY_NOTFOUND)
                        nSize -= 10;
                }
                else
                {
                    nSize = iValue;
                }

                float fSize = (float)nSize / 10;

                SfxMapUnit eUnit = maFontSizeControl.GetCoreMetric();
                SvxFontHeightItem aItem( CalcToUnit( fSize, eUnit ), 100, SID_ATTR_CHAR_FONTHEIGHT ) ;

                mpBindings->GetDispatcher()->Execute( SID_ATTR_CHAR_FONTHEIGHT, SFX_CALLMODE_RECORD, &aItem, 0L );
                mpBindings->Invalidate(SID_ATTR_CHAR_FONTHEIGHT,true,false);
                //add 
                maFontSizeBox.SetValue( nSize );
                if(nSize >= 960)
                {
                    mpToolBoxIncDec->EnableItem(TBI_INCREASE,false);
                    mpToolBoxIncDec->EnableItem(TBI_DECREASE,true);
                }
                else if(nSize <= 60)
                {
                    mpToolBoxIncDec->EnableItem(TBI_INCREASE,true);
                    mpToolBoxIncDec->EnableItem(TBI_DECREASE,false);
                }
                else
                {
                    mpToolBoxIncDec->EnableItem(TBI_INCREASE,true);
                    mpToolBoxIncDec->EnableItem(TBI_DECREASE,true);
                }
                //add end
            }
	}
	return 0;
}




IMPL_LINK(TextPropertyPanel, ToolBoxUnderlineClickHdl, ToolBox*, pToolBox)
{
	const sal_uInt16 nId = pToolBox->GetCurItemId();
	if(nId == TBI_UNDERLINE)
	{
		pToolBox->SetItemDown( nId, true );

        /**AF
		SvxTextUnderlinePage* pUnderlinePage = GetUnderlinePage();
		Size aFloatSz = pUnderlinePage->GetOutputSizePixel();
		GetUnderlineFloatWin()->SetSizePixel( aFloatSz );

		Point aPos = mpToolBoxFont->GetPosPixel();
		aPos = OutputToScreenPixel( aPos );
		Size aSize = mpToolBoxFont->GetSizePixel();
		Rectangle aRect( aPos, aSize );

		GetUnderlineFloatWin()->StartPopupMode( aRect, FLOATWIN_POPUPMODE_NOFOCUSCLOSE|FLOATWIN_POPUPMODE_DOWN );
		GetUnderlineFloatWin()->SetPopupModeFlags(GetUnderlineFloatWin()->GetPopupModeFlags() | FLOATWIN_POPUPMODE_NOAPPFOCUSCLOSE );
		pUnderlinePage->SetUnderlineSelect(meUnderline);
        */
	}
	return 0;
}




IMPL_LINK(TextPropertyPanel, ToolBoxFontColorDropHdl,ToolBox*, pToolBox)
{
	const sal_uInt16 nId = pToolBox->GetCurItemId();
	if(nId == TBI_FONTCOLOR)
	{
		meColorType = FONT_COLOR;

		pToolBox->SetItemDown( nId, true );

        /*AF
		SvxTextFontColorPage* pFontColorPage = GetFontColorPage();
	
		Size aFloatSz = pFontColorPage->GetOutputSizePixel();
		GetFontColorFloatWin()->SetSizePixel( aFloatSz );

		Point aPos = mpToolBoxFontColor->GetPosPixel();
		aPos = OutputToScreenPixel( aPos );
		Size aSize = mpToolBoxFontColor->GetSizePixel();
		Rectangle aRect( aPos, aSize );

		GetFontColorFloatWin()->StartPopupMode( aRect, FLOATWIN_POPUPMODE_NOFOCUSCLOSE|FLOATWIN_POPUPMODE_DOWN );
		GetFontColorFloatWin()->SetPopupModeFlags(GetFontColorFloatWin()->GetPopupModeFlags() | FLOATWIN_POPUPMODE_NOAPPFOCUSCLOSE );

		pFontColorPage->GetFocus();
		pFontColorPage->SetCurColorSelect(maColor, mbColorAvailable);
        */
	}
	return 0;
}




IMPL_LINK(TextPropertyPanel, ToolBoxSwScriptSelectHdl, ToolBox*, pToolBox)
{
	const sal_uInt16 nId = pToolBox->GetCurItemId();
	if( nId == TBI_SUPER_SW )
	{
		if(meEscape != SVX_ESCAPEMENT_SUPERSCRIPT)
		{
			meEscape = SVX_ESCAPEMENT_SUPERSCRIPT;
			SvxEscapementItem aSupItem(DFLT_ESC_SUPER, DFLT_ESC_PROP, SID_ATTR_CHAR_ESCAPEMENT);
			mpBindings->GetDispatcher()->Execute( SID_ATTR_CHAR_ESCAPEMENT, SFX_CALLMODE_RECORD, &aSupItem, 0L );
		}
		else
		{			
			meEscape = SVX_ESCAPEMENT_OFF;
			SvxEscapementItem aNoneItem(0, 100, SID_ATTR_CHAR_ESCAPEMENT);
			mpBindings->GetDispatcher()->Execute( SID_ATTR_CHAR_ESCAPEMENT, SFX_CALLMODE_RECORD, &aNoneItem, 0L );
		}
	}
	else if(TBI_SUB_SW == nId)
	{
		if(meEscape != SVX_ESCAPEMENT_SUBSCRIPT)
		{
			meEscape = (SvxEscapement)SVX_ESCAPEMENT_SUBSCRIPT;
			SvxEscapementItem aSubItem(DFLT_ESC_SUB, DFLT_ESC_PROP, SID_ATTR_CHAR_ESCAPEMENT);
			mpBindings->GetDispatcher()->Execute( SID_ATTR_CHAR_ESCAPEMENT, SFX_CALLMODE_RECORD, &aSubItem, 0L );
		}
		else
		{
			meEscape = SVX_ESCAPEMENT_OFF;
			SvxEscapementItem aNoneItem(0, 100, SID_ATTR_CHAR_ESCAPEMENT);
			mpBindings->GetDispatcher()->Execute( SID_ATTR_CHAR_ESCAPEMENT, SFX_CALLMODE_RECORD, &aNoneItem, 0L );
		}
	}
	UpdateFontScript();

	return 0;
}




IMPL_LINK(TextPropertyPanel, ToolBoxScriptSelectHdl, ToolBox*, pToolBox)
{
	const sal_uInt16 nId = pToolBox->GetCurItemId();
	if( nId == TBI_SUPER )
	{
		mbSuper = !mbSuper;
		SfxBoolItem aSupItem(SID_SET_SUPER_SCRIPT, mbSuper);
		mpBindings->GetDispatcher()->Execute( SID_SET_SUPER_SCRIPT, SFX_CALLMODE_RECORD, &aSupItem, 0L );
	}
	else if(TBI_SUB == nId)
	{

		mbSub = !mbSub;
		SfxBoolItem aSubItem(SID_SET_SUB_SCRIPT, mbSub );
		mpBindings->GetDispatcher()->Execute( SID_SET_SUB_SCRIPT, SFX_CALLMODE_RECORD, &aSubItem, 0L );
	}
	UpdateFontScript();
	return 0;
}




IMPL_LINK(TextPropertyPanel, ToolBoxHighlightDropHdl, ToolBox*, pToolBox)
{
	const sal_uInt16 nId = pToolBox->GetCurItemId();
	if(nId == TBI_HIGHLIGHT)
	{
		meColorType = BACK_COLOR;

		pToolBox->SetItemDown( nId, true );

        /*AF
		SvxTextFontColorPage* pFontColorPage = GetFontColorPage();

		Size aFloatSz = pFontColorPage->GetOutputSizePixel();
		GetFontColorFloatWin()->SetSizePixel( aFloatSz );

		Point aPos = mpToolBoxHighlight->GetPosPixel();
		aPos = OutputToScreenPixel( aPos );
		Size aSize = mpToolBoxHighlight->GetSizePixel();
		Rectangle aRect( aPos, aSize );

		GetFontColorFloatWin()->StartPopupMode( aRect, FLOATWIN_POPUPMODE_NOFOCUSCLOSE|FLOATWIN_POPUPMODE_DOWN );
		GetFontColorFloatWin()->SetPopupModeFlags(GetFontColorFloatWin()->GetPopupModeFlags() | FLOATWIN_POPUPMODE_NOAPPFOCUSCLOSE );

		pFontColorPage->GetFocus();
		pFontColorPage->SetCurColorSelect(maBackColor,
		mbBackColorAvailable);
        */
	}
	return 0;
}




IMPL_LINK(TextPropertyPanel, SpacingClickHdl, ToolBox*, pToolBox)
{
	const sal_uInt16 nId = pToolBox->GetCurItemId();
	if(nId == TBI_SPACING)
	{
		pToolBox->SetItemDown( nId, true );

        /*AF
		SvxTextSpacingPage* pSpacingPage = GetSpacingPage();
		pSpacingPage->SetControlState(mbKernLBAvailable,mbKernAvailable,mlKerning);

		Size aFloatSz = pSpacingPage->GetOutputSizePixel();
		GetSpacingFloatWin()->SetSizePixel( aFloatSz );

		Point aPos = mpToolBoxSpacing->GetPosPixel();
		aPos = OutputToScreenPixel( aPos );
		Size aSize = mpToolBoxSpacing->GetSizePixel();
		Rectangle aRect( aPos, aSize );

		GetSpacingFloatWin()->StartPopupMode( aRect, FLOATWIN_POPUPMODE_NOFOCUSCLOSE|FLOATWIN_POPUPMODE_DOWN );
		GetSpacingFloatWin()->SetPopupModeFlags(GetSpacingFloatWin()->GetPopupModeFlags() | FLOATWIN_POPUPMODE_NOAPPFOCUSCLOSE );
		pSpacingPage->GetFocus();
        */
	}
	return 0;
}




IMPL_LINK( TextPropertyPanel, ImplPopupModeEndHdl, FloatingWindow*, EMPTYARG )
{	
	return 0;
}




IMPL_LINK( TextPropertyPanel, ImplSpacingPopupModeEndHdl, FloatingWindow*, EMPTYARG )
{
    /*AF
	if(mpPageSpacing)
	{
		if( mpPageSpacing->GetLastCustomState() == SPACING_CLOSE_BY_CUS_EDIT)
		{
			SvtViewOptions aWinOpt( E_WINDOW, SIDEBAR_SPACING_GLOBAL_VALUE );
			::com::sun::star::uno::Sequence < ::com::sun::star::beans::NamedValue > aSeq(1);
			aSeq[0].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Spacing") ); 
			aSeq[0].Value <<= ::rtl::OUString( String::CreateFromInt32( mpPageSpacing->GetLastCustomValue() ));
			aWinOpt.SetUserData( aSeq );

		}
	}
    */
	return 0;
}




void TextPropertyPanel::NotifyItemUpdate (
    const sal_uInt16 nSID,
    const SfxItemState eState,
    const SfxPoolItem* pState)
{
	switch(nSID)
	{
        case SID_ATTR_CHAR_FONT:
            if (  eState >= SFX_ITEM_DEFAULT && pState->ISA(SvxFontItem) )
            {
                mpFontNameBox->Enable();
                const SvxFontItem* pFontItem = (const SvxFontItem*)pState;
                mpFontNameBox->SetText( pFontItem->GetFamilyName() );
            }
            else
            {
                mpFontNameBox->SetText( String() );
                if (SFX_ITEM_DISABLED == eState)
                {
                    mpFontNameBox->Disable();
                }
            }
            break;
        case SID_ATTR_CHAR_FONTHEIGHT:
            if (  eState >= SFX_ITEM_DEFAULT && pState->ISA(SvxFontHeightItem) )
            {
                mpHeightItem = (SvxFontHeightItem*)pState;//const SvxFontHeightItem* 
                SfxMapUnit eUnit = maFontSizeControl.GetCoreMetric();
                long iValue = (long)CalcToPoint( mpHeightItem->GetHeight(), eUnit, 10 );	
                mpToolBoxIncDec->Enable();

                // font size +/- enhancement in sd
                switch(maContext.GetCombinedContext())
                {
                    case CombinedEnumContext(Application_Draw, Context_DrawText):
                    case CombinedEnumContext(Application_Draw, Context_Text):
                    case CombinedEnumContext(Application_Draw, Context_Table):
                    case CombinedEnumContext(Application_Draw, Context_OutlineText):
                    case CombinedEnumContext(Application_Draw, Context_Draw):
                    case CombinedEnumContext(Application_Draw, Context_TextObject):
                    case CombinedEnumContext(Application_Draw, Context_Graphic):
                    case CombinedEnumContext(Application_Impress, Context_DrawText):
                    case CombinedEnumContext(Application_Impress, Context_Text):
                    case CombinedEnumContext(Application_Impress, Context_Table):
                    case CombinedEnumContext(Application_Impress, Context_OutlineText):
                    case CombinedEnumContext(Application_Impress, Context_Draw):
                    case CombinedEnumContext(Application_Impress, Context_TextObject):
                    case CombinedEnumContext(Application_Impress, Context_Graphic):
                        break;

                    default:
                        if(iValue > 60 && iValue < 960 )
                        {
                            mpToolBoxIncDec->EnableItem(TBI_INCREASE,true);
                            mpToolBoxIncDec->EnableItem(TBI_DECREASE,true);
                        }
                        else if (iValue <= 60)
                        {
                            mpToolBoxIncDec->EnableItem(TBI_INCREASE,true);
                            mpToolBoxIncDec->EnableItem(TBI_DECREASE,false);
                        }
                        else if (iValue >= 960)
                        {
                            mpToolBoxIncDec->EnableItem(TBI_INCREASE,false);
                            mpToolBoxIncDec->EnableItem(TBI_DECREASE,true);
                        }
                }
                mpToolBoxIncDec->SetItemState(TBI_INCREASE, STATE_NOCHECK);
                mpToolBoxIncDec->SetItemState(TBI_DECREASE, STATE_NOCHECK);

                if( mbFocusOnFontSizeCtrl )
                    return;

                maFontSizeBox.Enable( );
                maFontSizeBox.SetValue( iValue );	
                maFontSizeBox.LoseFocus();
            }
            else
            {			
                mpHeightItem = NULL;
                maFontSizeBox.SetText( String() );
                //increase decrease diabled when multi-seletion have different font size
			
                // font size +/- enhancement in sd
                switch(maContext.GetCombinedContext())
                {
                    case CombinedEnumContext(Application_Draw, Context_DrawText):
                    case CombinedEnumContext(Application_Draw, Context_Text):
                    case CombinedEnumContext(Application_Draw, Context_Table):
                    case CombinedEnumContext(Application_Draw, Context_OutlineText):
                    case CombinedEnumContext(Application_Draw, Context_Draw):
                    case CombinedEnumContext(Application_Draw, Context_TextObject):
                    case CombinedEnumContext(Application_Draw, Context_Graphic):
                    case CombinedEnumContext(Application_Impress, Context_DrawText):
                    case CombinedEnumContext(Application_Impress, Context_Text):
                    case CombinedEnumContext(Application_Impress, Context_Table):
                    case CombinedEnumContext(Application_Impress, Context_OutlineText):
                    case CombinedEnumContext(Application_Impress, Context_Draw):
                    case CombinedEnumContext(Application_Impress, Context_TextObject):
                    case CombinedEnumContext(Application_Impress, Context_Graphic):
                        break;

                    default:
                        mpToolBoxIncDec->Disable();
                }
                if ( eState <= SFX_ITEM_READONLY )
                {
                    maFontSizeBox.Disable( );
                }
            }
            break;	
        case SID_ATTR_CHAR_WEIGHT:
            mbWeightAvailable = (eState >= SFX_ITEM_DONTCARE);
            if( eState >= SFX_ITEM_DEFAULT && pState->ISA(SvxWeightItem))
            {
                const SvxWeightItem* pItem = (const SvxWeightItem*)pState;
                meWeight = (FontWeight)pItem->GetValue();
                TextStyleChanged();
            }
            else
            {
                meWeight = WEIGHT_NORMAL;
                TextStyleChanged();
            }
            break;
        case SID_ATTR_CHAR_POSTURE:
            mbPostureAvailable = (eState >= SFX_ITEM_DONTCARE);
            if( eState >= SFX_ITEM_DEFAULT && pState->ISA(SvxPostureItem))
            {
                const SvxPostureItem* pItem = (const SvxPostureItem*)pState;
                meItalic = (FontItalic)pItem->GetValue();
                TextStyleChanged();
            }
            else
            {
                meItalic = ITALIC_NONE;
                TextStyleChanged();
            }
            break;
        case SID_ATTR_CHAR_UNDERLINE:
            if( eState >= SFX_ITEM_DEFAULT) //SvxUnderlineItem
            {
                //<<delete 
                //if(pState->ISA(SvxTextLineItem))
                //{
                //	const SvxTextLineItem* pItem = (const SvxTextLineItem*)pState;
                //	meUnderline = (FontUnderline)pItem->GetValue();
                //	 //add , need to record the underline's color, if not the color will turn to auto
                //	meUnderlineColor = pItem->GetColor(); 
                //	//add end
                //}
                //else
                //delete end>>
                if(pState->ISA(SvxUnderlineItem))
                {
                    const SvxUnderlineItem* pItem = (const SvxUnderlineItem*)pState;
                    meUnderline = (FontUnderline)pItem->GetValue();
                    //add 
                    meUnderlineColor = pItem->GetColor(); 
                    //add end
                }
                TextStyleChanged();
            }
            else
            {
                meUnderline = UNDERLINE_NONE;
                TextStyleChanged();
            }
            break;
        case SID_ATTR_CHAR_SHADOWED:
            if( eState >= SFX_ITEM_DEFAULT && pState->ISA(SvxShadowedItem))
            {
                const SvxShadowedItem* pItem = (const SvxShadowedItem*)pState;
                mbShadow = pItem->GetValue();
                TextStyleChanged();
            }
            else
            {
                mbShadow = false;
                TextStyleChanged();
            }
            break;
        case SID_ATTR_CHAR_STRIKEOUT:
            if( eState >= SFX_ITEM_DEFAULT && pState->ISA(SvxCrossedOutItem))
            {
                const SvxCrossedOutItem* pItem = (const SvxCrossedOutItem*)pState;
                meStrike = (FontStrikeout)pItem->GetValue();

                TextStyleChanged();
            }
            else
            {
                meStrike = STRIKEOUT_NONE;
                TextStyleChanged();
            }
            break;
        case SID_ATTR_CHAR_COLOR:
            if( eState >= SFX_ITEM_DEFAULT && pState->ISA(SvxColorItem))
            {
                const SvxBrushItem* pItem =  (const SvxBrushItem*)pState;
                maColor = pItem->GetColor();
                mbColorAvailable = true;
                mpFontColorUpdater->Update(maColor);
            }
            else
            {
                mbColorAvailable = false;
                maColor.SetColor(COL_AUTO);
                mpFontColorUpdater->Update(maColor);
            }
            break;
        case SID_ATTR_BRUSH_CHAR:
            if( eState >= SFX_ITEM_DEFAULT && pState->ISA(SvxBrushItem))
            {
                const SvxBrushItem* pItem =  (const SvxBrushItem*)pState;
                maBackColor = pItem->GetColor();
                mbBackColorAvailable = true;
                mpHighlightUpdater->Update(maBackColor);
            }
            else
            {
                mbBackColorAvailable = false;
                maBackColor.SetColor(COL_AUTO);
                mpHighlightUpdater->Update(maBackColor);
            }
            break;
        case SID_ATTR_CHAR_ESCAPEMENT:
            if( eState == SFX_ITEM_AVAILABLE)
            {
                if( pState->ISA(SvxEscapementItem))
                {
                    const SvxEscapementItem* pItem = (const SvxEscapementItem *)pState;
                    short nEsc = pItem->GetEsc();
                    if(nEsc == 0)
                        meEscape = SVX_ESCAPEMENT_OFF;
                    else if(nEsc > 0)
                        meEscape = SVX_ESCAPEMENT_SUPERSCRIPT;
                    else
                        meEscape = SVX_ESCAPEMENT_SUBSCRIPT;
                }
                else
                {
                    meEscape = SVX_ESCAPEMENT_OFF;
                }
                TextStyleChanged();
            }
            else if(eState == SFX_ITEM_DISABLED)
            {
                mpToolBoxScriptSw->EnableItem(TBI_SUPER,false);
                mpToolBoxScriptSw->EnableItem(TBI_SUB,false);
            }
            else
            {
                meEscape = SVX_ESCAPEMENT_OFF;
                TextStyleChanged();
            }	
            break;
        case SID_SET_SUB_SCRIPT:
            if( eState >= SFX_ITEM_DEFAULT && pState->ISA(SfxBoolItem))
            {
                const SfxBoolItem* pItem = (const SfxBoolItem*)pState;
                mbSub = pItem->GetValue();
                TextStyleChanged();
            }
            else
            {
                mbSub = false;
                TextStyleChanged();
            }
            break;
        case SID_SET_SUPER_SCRIPT:
            if( eState >= SFX_ITEM_DEFAULT && pState->ISA(SfxBoolItem))
            {
                const SfxBoolItem* pItem = (const SfxBoolItem*)pState;
                mbSuper = pItem->GetValue();
                TextStyleChanged();
            }
            else
            {
                mbSuper = false;
                TextStyleChanged();
            }
            break;
        case SID_ATTR_CHAR_KERNING:     
            if ( SFX_ITEM_AVAILABLE == eState )
            {
                mbKernLBAvailable = true;
            
                if(pState->ISA(SvxKerningItem))
                {
                    const SvxKerningItem* pKerningItem  = (const SvxKerningItem*)pState;      
                    mlKerning = (long)pKerningItem->GetValue();			
                    mbKernAvailable = true;
                }
                else
                {
                    mlKerning = 0;
                    mbKernAvailable =false;
                }
            }
            else if (SFX_ITEM_DISABLED == eState)
            {
                mbKernLBAvailable = false;
                mbKernAvailable = false;
                mlKerning = 0;
            }
            else
            {
                mbKernLBAvailable = true;
                mbKernAvailable = false;	
                mlKerning = 0;
            }
            break;

            // font size +/- enhancement in sd
        case SID_SHRINK_FONT_SIZE:
        case SID_GROW_FONT_SIZE:
                switch(maContext.GetCombinedContext())
                {
                    case CombinedEnumContext(Application_Draw, Context_DrawText):
                    case CombinedEnumContext(Application_Draw, Context_Text):
                    case CombinedEnumContext(Application_Draw, Context_Table):
                    case CombinedEnumContext(Application_Draw, Context_OutlineText):
                    case CombinedEnumContext(Application_Draw, Context_Draw):
                    case CombinedEnumContext(Application_Draw, Context_TextObject):
                    case CombinedEnumContext(Application_Draw, Context_Graphic):
                    case CombinedEnumContext(Application_Impress, Context_DrawText):
                    case CombinedEnumContext(Application_Impress, Context_Text):
                    case CombinedEnumContext(Application_Impress, Context_Table):
                    case CombinedEnumContext(Application_Impress, Context_OutlineText):
                    case CombinedEnumContext(Application_Impress, Context_Draw):
                    case CombinedEnumContext(Application_Impress, Context_TextObject):
                    case CombinedEnumContext(Application_Impress, Context_Graphic):
                        if(eState == SFX_ITEM_DISABLED)
                        {
                            mpToolBoxIncDec->Disable();
                        }
                        else
                        {
                            mpToolBoxIncDec->Enable();
                        }
                    break;
                }
                break;
    }
}




void TextPropertyPanel::TextStyleChanged()
{
	if( !mbWeightAvailable )
		mpToolBoxFont->EnableItem(TBI_BOLD,false);
	else
		mpToolBoxFont->EnableItem(TBI_BOLD,true);

	if(!mbPostureAvailable )
		mpToolBoxFont->EnableItem(TBI_ITALIC,false);
	else
		mpToolBoxFont->EnableItem(TBI_ITALIC,true);

	UpdateFontBold();
	UpdateFontItalic();
	UpdateFontUnderline();
	UpdateFontStrikeOut();
	UpdateFontShadowed();
	UpdateFontScript();
}




void TextPropertyPanel::UpdateFontBold()
{
	if( meWeight == WEIGHT_BOLD )
	{	
		mpToolBoxFont->SetItemState(TBI_BOLD,	STATE_CHECK);
	}
	else
	{
		mpToolBoxFont->SetItemState(TBI_BOLD,	STATE_NOCHECK);
	}
}
void TextPropertyPanel::UpdateFontItalic()
{
	if(meItalic == ITALIC_NORMAL)
	{
		mpToolBoxFont->SetItemState(TBI_ITALIC,	STATE_CHECK);
	}
	else
	{
		mpToolBoxFont->SetItemState(TBI_ITALIC,	STATE_NOCHECK);
	}
}
void TextPropertyPanel::UpdateFontUnderline()
{
	if(meUnderline == UNDERLINE_NONE)
	{
		mpToolBoxFont->SetItemState(TBI_UNDERLINE,	STATE_NOCHECK);
	}
	else
	{
		mpToolBoxFont->SetItemState(TBI_UNDERLINE,	STATE_CHECK);
	}
}
void TextPropertyPanel::UpdateFontStrikeOut()
{
	if(meStrike !=  STRIKEOUT_NONE && meStrike != STRIKEOUT_DONTKNOW)
	{
		mpToolBoxFont->SetItemState(TBI_STRIKEOUT,	STATE_CHECK);
	}
	else
	{
		mpToolBoxFont->SetItemState(TBI_STRIKEOUT,	STATE_NOCHECK);
	}
}
void TextPropertyPanel::UpdateFontShadowed()
{
	if(mbShadow)
	{
		mpToolBoxFont->SetItemState(TBI_SHADOWED,	STATE_CHECK);
	}
	else
	{
		mpToolBoxFont->SetItemState(TBI_SHADOWED,	STATE_NOCHECK);
	}
}
void  TextPropertyPanel::UpdateFontScript()
{
	//script for sw
	mpToolBoxScriptSw->EnableItem(TBI_SUPER,true);
	mpToolBoxScriptSw->EnableItem(TBI_SUB,true);
	if(meEscape == SVX_ESCAPEMENT_SUPERSCRIPT)
	{
		mpToolBoxScriptSw->SetItemState(TBI_SUPER_SW,	STATE_CHECK);
		mpToolBoxScriptSw->SetItemState(TBI_SUB_SW,	STATE_NOCHECK);
	}
	else if(meEscape == SVX_ESCAPEMENT_SUBSCRIPT)
	{
		mpToolBoxScriptSw->SetItemState(TBI_SUPER_SW,	STATE_NOCHECK);
		mpToolBoxScriptSw->SetItemState(TBI_SUB_SW,	STATE_CHECK);
	}
	else
	{
		mpToolBoxScriptSw->SetItemState(TBI_SUPER_SW,	STATE_NOCHECK);
		mpToolBoxScriptSw->SetItemState(TBI_SUB_SW,	STATE_NOCHECK);
	}

	//script for sc sd
	mpToolBoxScript->EnableItem(TBI_SUPER,true);
	mpToolBoxScript->EnableItem(TBI_SUB,true);
	if(mbSuper)
	{
		mpToolBoxScript->SetItemState(TBI_SUPER,	STATE_CHECK);
		mpToolBoxScript->SetItemState(TBI_SUB,	STATE_NOCHECK);
	}
	else if(mbSub)
	{
		mpToolBoxScript->SetItemState(TBI_SUPER,	STATE_NOCHECK);
		mpToolBoxScript->SetItemState(TBI_SUB,	STATE_CHECK);
	}
	else
	{
		mpToolBoxScript->SetItemState(TBI_SUPER,	STATE_NOCHECK);
		mpToolBoxScript->SetItemState(TBI_SUB,	STATE_NOCHECK);
	}
}

/*
USHORT TextPropertyPanel::GetCurrColorType()
{
	return meColorType;
}
long TextPropertyPanel::GetSelFontSize()
{
    long nH = 240;
    SfxMapUnit eUnit = maSpacingControl.GetCoreMetric();
    if (mpHeightItem)
        nH = LogicToLogic(  mpHeightItem->GetHeight(), (MapUnit)eUnit, MAP_TWIP );
    return nH;
} 
SfxPropertyPageController TextPropertyPanel::GetSpaceController()
{
	return maSpacingControl;
}

//add 
Color& TextPropertyPanel::GetUnderlineColor() 
{
	return meUnderlineColor;
}
//add end
void TextPropertyPanel::SetBackColor(Color aCol)
{
	maBackColor = aCol;
}
void TextPropertyPanel::SetColor(Color aCol)
{
	maColor = aCol;
}
void TextPropertyPanel::SetUnderline(FontUnderline	eUnderline)
{
	meUnderline = eUnderline;
}
void TextPropertyPanel::SetSpacing(long nKern)
{
	mlKerning = nKern;
}
*/
} } // end of namespace svx::sidebar
