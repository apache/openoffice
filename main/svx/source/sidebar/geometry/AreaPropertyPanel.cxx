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
#include "AreaPropertyPanel.hxx"
#include "AreaPropertyPanel.hrc"
#include <svx/dialogs.hrc>
#include <svx/dialmgr.hxx>
#include <sfx2/objsh.hxx>
#include <svx/xfltrit.hxx>
#include <svx/xflftrit.hxx>
#include <svx/xtable.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/bindings.hxx>
#include <helpid.hrc>
#include <svtools/valueset.hxx>
#include <unotools/pathoptions.hxx>
#include <svx/svxitems.hrc>
#include <vcl/toolbox.hxx>

using namespace css;
using namespace cssu;
using ::sfx2::sidebar::Theme;

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

//////////////////////////////////////////////////////////////////////////////
// positioning helpers

#define APOS1_1 Point(LogicToPixel(Point(POPUPPANEL_MARGIN_HORIZONTAL,POPUPPANEL_MARGIN_VERTICAL), MAP_APPFONT))
#define APOS2_1 Point(LogicToPixel(Point(POPUPPANEL_MARGIN_HORIZONTAL,POPUPPANEL_MARGIN_VERTICAL + FIXED_TEXT_HEIGHT + TEXT_CONTROL_SPACING_VERTICAL), MAP_APPFONT))
#define APOS1_2 Point(LogicToPixel(Point(POPUPPANEL_MARGIN_HORIZONTAL+CONTROL_WIDTH+CONTROL_SPACING_HORIZONTAL,POPUPPANEL_MARGIN_VERTICAL), MAP_APPFONT))
#define APOS2_2 Point(LogicToPixel(Point(POPUPPANEL_MARGIN_HORIZONTAL+CONTROL_WIDTH+CONTROL_SPACING_HORIZONTAL,POPUPPANEL_MARGIN_VERTICAL + FIXED_TEXT_HEIGHT + TEXT_CONTROL_SPACING_VERTICAL), MAP_APPFONT))
#define APOS1_3 Point(LogicToPixel(Point(POPUPPANEL_MARGIN_HORIZONTAL,POPUPPANEL_MARGIN_VERTICAL + FIXED_TEXT_HEIGHT + TEXT_CONTROL_SPACING_VERTICAL+MBOX_HEIGHT+CONTROL_SPACING_VERTICAL), MAP_APPFONT))
#define APOS1_4 Point(LogicToPixel(Point(POPUPPANEL_MARGIN_HORIZONTAL,POPUPPANEL_MARGIN_VERTICAL + 2*(FIXED_TEXT_HEIGHT + TEXT_CONTROL_SPACING_VERTICAL)+MBOX_HEIGHT+CONTROL_SPACING_VERTICAL), MAP_APPFONT))
#define APOS2_3 Point(LogicToPixel(Point(POPUPPANEL_MARGIN_HORIZONTAL+CONTROL_WIDTH+CONTROL_SPACING_HORIZONTAL,POPUPPANEL_MARGIN_VERTICAL + FIXED_TEXT_HEIGHT + TEXT_CONTROL_SPACING_VERTICAL+MBOX_HEIGHT+CONTROL_SPACING_VERTICAL), MAP_APPFONT))
#define APOS2_4 Point(LogicToPixel(Point(POPUPPANEL_MARGIN_HORIZONTAL+CONTROL_WIDTH+CONTROL_SPACING_HORIZONTAL,POPUPPANEL_MARGIN_VERTICAL + 2*(FIXED_TEXT_HEIGHT + TEXT_CONTROL_SPACING_VERTICAL)+MBOX_HEIGHT+CONTROL_SPACING_VERTICAL), MAP_APPFONT))
#define APOS1_5 Point(LogicToPixel(Point(POPUPPANEL_MARGIN_HORIZONTAL,POPUPPANEL_MARGIN_VERTICAL + 2*(FIXED_TEXT_HEIGHT + TEXT_CONTROL_SPACING_VERTICAL+MBOX_HEIGHT+CONTROL_SPACING_VERTICAL)), MAP_APPFONT))
#define APOS1_6 Point(LogicToPixel(Point(POPUPPANEL_MARGIN_HORIZONTAL,POPUPPANEL_MARGIN_VERTICAL + 3*(FIXED_TEXT_HEIGHT + TEXT_CONTROL_SPACING_VERTICAL)+2*(MBOX_HEIGHT+CONTROL_SPACING_VERTICAL)), MAP_APPFONT))
#define APOS2_5 Point(LogicToPixel(Point(POPUPPANEL_MARGIN_HORIZONTAL+CONTROL_WIDTH+CONTROL_SPACING_HORIZONTAL,POPUPPANEL_MARGIN_VERTICAL + 2*(FIXED_TEXT_HEIGHT + TEXT_CONTROL_SPACING_VERTICAL+MBOX_HEIGHT+CONTROL_SPACING_VERTICAL)), MAP_APPFONT))
#define APOS2_6 Point(LogicToPixel(Point(POPUPPANEL_MARGIN_HORIZONTAL+CONTROL_WIDTH+CONTROL_SPACING_HORIZONTAL,POPUPPANEL_MARGIN_VERTICAL + 3*(FIXED_TEXT_HEIGHT + TEXT_CONTROL_SPACING_VERTICAL)+2*(MBOX_HEIGHT+CONTROL_SPACING_VERTICAL)), MAP_APPFONT))
#define APOS1_7 Point(LogicToPixel(Point(POPUPPANEL_MARGIN_HORIZONTAL,POPUPPANEL_MARGIN_VERTICAL + 3*(FIXED_TEXT_HEIGHT + TEXT_CONTROL_SPACING_VERTICAL+MBOX_HEIGHT+CONTROL_SPACING_VERTICAL)), MAP_APPFONT))
#define APOS1_8 Point(LogicToPixel(Point(POPUPPANEL_MARGIN_HORIZONTAL,POPUPPANEL_MARGIN_VERTICAL + 4*(FIXED_TEXT_HEIGHT + TEXT_CONTROL_SPACING_VERTICAL)+3*(MBOX_HEIGHT+CONTROL_SPACING_VERTICAL)), MAP_APPFONT))
#define APOS_Left_Right_1 Point(LogicToPixel(Point(LEFT_RIGHT_X1,LEFT_RIGHT_Y1), MAP_APPFONT))
#define APOS_Left_Right_2 Point(LogicToPixel(Point(LEFT_RIGHT_X2,LEFT_RIGHT_Y1), MAP_APPFONT))
#define APOS_Left_Right_3 Point(LogicToPixel(Point(LEFT_RIGHT_X1,LEFT_RIGHT_Y2), MAP_APPFONT))
#define APOS_Left_Right_4 Point(LogicToPixel(Point(LEFT_RIGHT_X2,LEFT_RIGHT_Y2), MAP_APPFONT))
#define DEFAULT_CENTERX 50
#define DEFAULT_CENTERY 50
#define DEFAULT_ANGLE 0
#define DEFAULT_STARTVALUE 0
#define DEFAULT_ENDVALUE 16777215
#define DEFAULT_BORDER 0

//////////////////////////////////////////////////////////////////////////////

short GetItemId_Imp( ValueSet& rValueSet, const Color& rCol )
{
	if(rCol == COL_AUTO)
		return 0;

	bool	bFound = false;
	sal_uInt16	nCount = rValueSet.GetItemCount();
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

//////////////////////////////////////////////////////////////////////////////

class SvxColorPage : public svx::sidebar::PropertyPanelPopupPage
{
public:
	SvxColorPage(Window* pParent, svx::sidebar::AreaPropertyPanel& rPanel);
    virtual ~SvxColorPage();
	void GetFocus();
	void SetCurColorSelect(Color aCol, bool bAvl);

private:	
	svx::sidebar::AreaPropertyPanel&    mrAreaPropertyPanel;
	SfxBindings*		                mpBindings;
	ValueSet			                maVSColor;

	void Initialize();
	void FillColors();
	DECL_LINK(VSSelectHdl, void *);
};

SvxColorPage::SvxColorPage(Window* pParent, svx::sidebar::AreaPropertyPanel& rPanel)
:	svx::sidebar::PropertyPanelPopupPage( pParent,SVX_RES(RID_POPUPPANEL_AERAPAGE_COLOR))
,	mrAreaPropertyPanel(rPanel)
,	mpBindings(NULL)
,	maVSColor( this, SVX_RES(VS_COLOR))
{
	Initialize();
	FreeResource();
    mpBindings = mrAreaPropertyPanel.GetBindings();
}

SvxColorPage::~SvxColorPage()
{
}

void SvxColorPage::Initialize()
{
	WinBits nBits = ( maVSColor.GetStyle() | WB_ITEMBORDER | WB_NAMEFIELD | WB_NO_DIRECTSELECT); // | WB_SELECTIFOLLOWMOUSE );
//	maVSColor.SetText( String( SVX_RES( STR_AUTOMATICE ) ) );
	maVSColor.SetStyle( nBits );

	maVSColor.SetColCount( 12 );
	maVSColor.SetLineCount( 12 );
	const Size aSize15x15 = Size( 15, 15 );
	maVSColor.CalcWindowSizePixel( aSize15x15 );
	maVSColor.Show();

	Link aLink =  LINK( this, SvxColorPage, VSSelectHdl ) ;
    maVSColor.SetSelectHdl(aLink);
}

void SvxColorPage::FillColors()
{
	SfxObjectShell* pDocSh = SfxObjectShell::Current();
	const SfxPoolItem* pItem = NULL;
	XColorTable* pColorTable = NULL;
	bool bOwn = false;

	DBG_ASSERT( pDocSh, "DocShell not found!" );

	if ( pDocSh && ( 0 != ( pItem = pDocSh->GetItem( SID_COLOR_TABLE ) ) ) )
		pColorTable = ( (SvxColorTableItem*)pItem )->GetColorTable();

	if ( !pColorTable )
	{
		bOwn = true;
		pColorTable = new XColorTable( SvtPathOptions().GetPalettePath() );
	}

	if ( pColorTable )
	{
		short i	= 0;
		long nCount	= pColorTable->Count();
		XColorEntry* pEntry	= NULL;
		Color aColWhite( COL_WHITE );
		String aStrWhite( SVX_RES( RID_SVXITEMS_COLOR_WHITE ) );

		maVSColor.Clear();
		maVSColor.SetStyle( maVSColor.GetStyle() & ~WB_VSCROLL );
		for ( i = 0; i < nCount; i++ )
		{
            pEntry = pColorTable->GetColor(i);
			maVSColor.InsertItem(	i + 1, pEntry->GetColor(), pEntry->GetName() );
		}
		while ( i < 144 )
		{
			maVSColor.InsertItem( i + 1, aColWhite, aStrWhite );
			i++;
		}
		if ( nCount > 144 )
		{
			maVSColor.SetStyle( maVSColor.GetStyle() | WB_VSCROLL );
		}
	}

	if ( bOwn )
		delete pColorTable;

	maVSColor.Show();
}

void SvxColorPage::GetFocus()
{
	maVSColor.GrabFocus();
}

void SvxColorPage::SetCurColorSelect(Color aCol, bool bAvailable)
{
//	sal_uInt16 nType = mrAreaPropertyPanel.GetCurrColorType();
		
	FillColors();	
	short nCol = GetItemId_Imp( maVSColor, aCol );
	if(! bAvailable)
	{
		maVSColor.SetNoSelection();
		return;
	}

	//if not found
	if( nCol == -1)
	{
		maVSColor.SetNoSelection();
	}
	else
	{
		maVSColor.SelectItem( nCol );
	}
}

IMPL_LINK(SvxColorPage, VSSelectHdl, void *, pControl)
{
    if(pControl == &maVSColor)
    {
        sal_uInt16 iPos = maVSColor.GetSelectItemId();
        Color aColor = maVSColor.GetItemColor( iPos );	
        String aTmpStr = maVSColor.GetItemText( iPos );
        if(aColor.GetColor() == 0 && aTmpStr.Equals(String::CreateFromAscii("")))
            aColor = mrAreaPropertyPanel.maLastColor;
        XFillColorItem aXFillColorItem( aTmpStr, aColor );
        mpBindings->GetDispatcher()->Execute(SID_ATTR_FILL_COLOR, SFX_CALLMODE_RECORD, &aXFillColorItem, 0L);
            mrAreaPropertyPanel.maLastColor = aColor;

        if (mrAreaPropertyPanel.GetColorFloatWin()->IsInPopupMode() )
            mrAreaPropertyPanel.GetColorFloatWin()->EndPopupMode();
    }

    return( 0L );
}

//////////////////////////////////////////////////////////////////////////////

class SvxAreaTrGrPage : public svx::sidebar::PropertyPanelPopupPage
{
public:
	SvxAreaTrGrPage(Window* pParent, svx::sidebar::AreaPropertyPanel& rPanel);
    virtual ~SvxAreaTrGrPage();

	void ToGetFocus();
	void Rearrange(XFillFloatTransparenceItem* pGradientItem);
	void InitStatus(XFillFloatTransparenceItem* pGradientItem);
	void ExecuteValueModify( sal_uInt8 nStartCol, sal_uInt8 nEndCol );
	void SetControlState_Impl(XGradientStyle eXGS);
private:	
	FixedText           maFtTrgrCenterX;
	MetricField         maMtrTrgrCenterX;
	FixedText           maFtTrgrCenterY;
	MetricField         maMtrTrgrCenterY;
	FixedText           maFtTrgrAngle;
	MetricField         maMtrTrgrAngle;	
	ToolBox		 	    maBtnLeft45;
	ToolBox		 	    maBtnRight45;
	FixedText           maFtTrgrStartValue;
	MetricField         maMtrTrgrStartValue;
	FixedText           maFtTrgrEndValue;
	MetricField         maMtrTrgrEndValue;
	FixedText           maFtTrgrBorder;
	MetricField         maMtrTrgrBorder;
	Image				maRotLeft;
	Image				maRotRight;

	svx::sidebar::AreaPropertyPanel& mrAreaPropertyPanel;
	SfxBindings*		mpBindings;
	DECL_LINK( ModifiedTrgrHdl_Impl, void* );
	DECL_LINK( Left_Click45_Impl, void* );
	DECL_LINK( Right_Click45_Impl, void* );
};

SvxAreaTrGrPage::SvxAreaTrGrPage(Window* pParent, svx::sidebar::AreaPropertyPanel& rPanel)
:	svx::sidebar::PropertyPanelPopupPage( pParent,SVX_RES(RID_POPUPPANEL_AREAPAGE_TRGR)),
	maFtTrgrCenterX(this, SVX_RES(FT_TRGR_CENTER_X)),
	maMtrTrgrCenterX(this, SVX_RES(MTR_TRGR_CENTER_X)),
	maFtTrgrCenterY(this, SVX_RES(FT_TRGR_CENTER_Y)),
	maMtrTrgrCenterY(this, SVX_RES(MTR_TRGR_CENTER_Y)),
	maFtTrgrAngle(this, SVX_RES(FT_TRGR_ANGLE)),
	maMtrTrgrAngle(this, SVX_RES(MTR_TRGR_ANGLE)),	
	maFtTrgrStartValue(this, SVX_RES(FT_TRGR_START_VALUE)),
	maMtrTrgrStartValue(this, SVX_RES(MTR_TRGR_START_VALUE)),
	maFtTrgrEndValue(this, SVX_RES(FT_TRGR_END_VALUE)),
	maMtrTrgrEndValue(this, SVX_RES(MTR_TRGR_END_VALUE)),
	maFtTrgrBorder(this, SVX_RES(FT_TRGR_BORDER)),
	maMtrTrgrBorder(this, SVX_RES(MTR_TRGR_BORDER)),
	maBtnLeft45(this, SVX_RES(BTN_LEFT_SECOND)),
	maBtnRight45(this, SVX_RES(BTN_RIGHT_FIRST)),
	maRotLeft( SVX_RES(IMG_ROT_LEFT)),
	maRotRight( SVX_RES(IMG_ROT_RIGHT)),
	mrAreaPropertyPanel(rPanel),
	mpBindings(NULL)
{
	Link aLink = LINK( this, SvxAreaTrGrPage, ModifiedTrgrHdl_Impl);
	maMtrTrgrCenterX.SetModifyHdl( aLink );
	maMtrTrgrCenterY.SetModifyHdl( aLink );
	maMtrTrgrAngle.SetModifyHdl( aLink );
	maMtrTrgrBorder.SetModifyHdl( aLink );
	maMtrTrgrStartValue.SetModifyHdl( aLink );
	maMtrTrgrEndValue.SetModifyHdl( aLink );
	aLink = LINK( this, SvxAreaTrGrPage, Left_Click45_Impl);
	maBtnLeft45.SetSelectHdl( aLink );
	aLink = LINK( this, SvxAreaTrGrPage, Right_Click45_Impl);
	maBtnRight45.SetSelectHdl( aLink );
	maBtnLeft45.SetItemImage(1,maRotLeft);
	Size aTbxSize = maBtnLeft45.CalcWindowSizePixel();
	maBtnLeft45.SetOutputSizePixel( aTbxSize );
	maBtnLeft45.SetQuickHelpText(1, String(SVX_RES(STR_HELP_LEFT)));	//acc wj

	maBtnRight45.SetItemImage(1,maRotRight);
	aTbxSize = maBtnRight45.CalcWindowSizePixel();
	maBtnRight45.SetOutputSizePixel( aTbxSize );
	maBtnRight45.SetQuickHelpText(1, String(SVX_RES(STR_HELP_RIGHT)));	//acc wj

	maBtnLeft45.SetBackground(Wallpaper());
	maBtnLeft45.SetPaintTransparent(true);
	maBtnRight45.SetBackground(Wallpaper());
	maBtnRight45.SetPaintTransparent(true);

	FreeResource();
    mpBindings = mrAreaPropertyPanel.GetBindings();
}

SvxAreaTrGrPage::~SvxAreaTrGrPage()
{
}

void SvxAreaTrGrPage::ToGetFocus()
{
	if(maMtrTrgrCenterX.IsVisible())
		maMtrTrgrCenterX.GrabFocus();
	else
		maMtrTrgrAngle.GrabFocus();
}

void SvxAreaTrGrPage::Rearrange(XFillFloatTransparenceItem* pGradientItem)
{
	InitStatus(pGradientItem);
	const XGradient& rGradient = pGradientItem->GetGradientValue();	
	XGradientStyle eXGS(rGradient.GetGradientStyle());
	Size aSize(POP_WIDTH,POP_HEIGHT);
	aSize = LogicToPixel( aSize, MapMode(MAP_APPFONT) );
	Size aSize2(POP_WIDTH,POP_HEIGHT2);
	aSize2 = LogicToPixel( aSize2, MapMode(MAP_APPFONT) );
	long aPosY = 0;
	Point aPointAngle;
	Size aSizeAngle = maMtrTrgrAngle.GetSizePixel();
	Size aTbxSize = maBtnLeft45.CalcWindowSizePixel();

	switch(eXGS)
	{
	case XGRAD_LINEAR:
	case XGRAD_AXIAL:
		maFtTrgrCenterX.Hide();
		maMtrTrgrCenterX.Hide();
		maFtTrgrCenterY.Hide();
		maMtrTrgrCenterY.Hide();
		maFtTrgrAngle.Show();
		maFtTrgrAngle.SetPosPixel(APOS1_1);
		maMtrTrgrAngle.Show();
		maMtrTrgrAngle.SetPosPixel(APOS2_1);
		maFtTrgrStartValue.SetPosPixel(APOS1_3);
		maMtrTrgrStartValue.SetPosPixel(APOS1_4);
		maFtTrgrEndValue.SetPosPixel(APOS2_3);
		maMtrTrgrEndValue.SetPosPixel(APOS2_4);
		maFtTrgrBorder.SetPosPixel(APOS1_5);
		maMtrTrgrBorder.SetPosPixel(APOS1_6);

		maBtnLeft45.Show();
		maBtnRight45.Show();

		aPointAngle = maMtrTrgrAngle.GetPosPixel();
		aPosY = aPointAngle.getY() + aSizeAngle.getHeight() - aTbxSize.getHeight();

		maBtnLeft45.SetPosPixel(Point(APOS_Left_Right_1.getX(), aPosY));
		maBtnRight45.SetPosPixel(Point(APOS_Left_Right_2.getX(), aPosY));

		SetSizePixel(aSize2);
		break;

	case XGRAD_RADIAL:
		maFtTrgrCenterX.Show();
		maFtTrgrCenterX.SetPosPixel(APOS1_1);
		maMtrTrgrCenterX.Show();
		maMtrTrgrCenterX.SetPosPixel(APOS2_1);
		maFtTrgrCenterY.Show();
		maFtTrgrCenterY.SetPosPixel(APOS1_2);
		maMtrTrgrCenterY.Show();
		maMtrTrgrCenterY.SetPosPixel(APOS2_2);
		maFtTrgrAngle.Hide();
		maMtrTrgrAngle.Hide();
		maFtTrgrStartValue.SetPosPixel(APOS1_3);
		maMtrTrgrStartValue.SetPosPixel(APOS1_4);
		maFtTrgrEndValue.SetPosPixel(APOS2_3);
		maMtrTrgrEndValue.SetPosPixel(APOS2_4);
		maFtTrgrBorder.SetPosPixel(APOS1_5);
		maMtrTrgrBorder.SetPosPixel(APOS1_6);

		maBtnLeft45.Hide();
		maBtnRight45.Hide();

		SetSizePixel(aSize2);

		break;

	case XGRAD_ELLIPTICAL:
	case XGRAD_SQUARE:
	case XGRAD_RECT:
		maFtTrgrCenterX.Show();
		maFtTrgrCenterX.SetPosPixel(APOS1_1);
		maMtrTrgrCenterX.Show();
		maMtrTrgrCenterX.SetPosPixel(APOS2_1);
		maFtTrgrCenterY.Show();
		maFtTrgrCenterY.SetPosPixel(APOS1_2);
		maMtrTrgrCenterY.Show();
		maMtrTrgrCenterY.SetPosPixel(APOS2_2);
		maFtTrgrAngle.Show();
		maFtTrgrAngle.SetPosPixel(APOS1_3);
		maMtrTrgrAngle.Show();
		maMtrTrgrAngle.SetPosPixel(APOS1_4);

		maFtTrgrStartValue.SetPosPixel(APOS1_5);
		maMtrTrgrStartValue.SetPosPixel(APOS1_6);
		maFtTrgrEndValue.SetPosPixel(APOS2_5);
		maMtrTrgrEndValue.SetPosPixel(APOS2_6);
		maFtTrgrBorder.SetPosPixel(APOS1_7);
		maMtrTrgrBorder.SetPosPixel(APOS1_8);

		maBtnLeft45.Show();
		maBtnRight45.Show();

		aPointAngle = maMtrTrgrAngle.GetPosPixel();
		aPosY = aPointAngle.getY() + aSizeAngle.getHeight() - aTbxSize.getHeight();

		maBtnLeft45.SetPosPixel(Point(APOS_Left_Right_3.getX(), aPosY));
		maBtnRight45.SetPosPixel(Point(APOS_Left_Right_4.getX(), aPosY));

		SetSizePixel(aSize);

		break;
	}
}

void SvxAreaTrGrPage::InitStatus(XFillFloatTransparenceItem* pGradientItem)
{
	const XGradient& aGradient = pGradientItem->GetGradientValue();

	XGradient rGradient;

	if (aGradient.GetXOffset() == DEFAULT_CENTERX &&
		aGradient.GetYOffset() == DEFAULT_CENTERY &&
		(aGradient.GetAngle() / 10) == DEFAULT_ANGLE &&
		((sal_uInt16)((((sal_uInt16)aGradient.GetStartColor().GetRed() + 1) * 100) / 255)) == DEFAULT_STARTVALUE &&
		((sal_uInt16)((((sal_uInt16)aGradient.GetEndColor().GetRed() + 1) * 100) / 255)) == DEFAULT_ENDVALUE &&
		aGradient.GetBorder() == DEFAULT_BORDER)
	{
		switch(aGradient.GetGradientStyle())
		{
		case XGRAD_LINEAR:
			{
				rGradient = mrAreaPropertyPanel.maGradientLinear;
			}
			break;
		case XGRAD_AXIAL:
			{
				rGradient = mrAreaPropertyPanel.maGradientAxial;
			}
			break;
		case XGRAD_RADIAL:
			{
				rGradient = mrAreaPropertyPanel.maGradientRadial;
			}
			break;
		case XGRAD_ELLIPTICAL:
			{
				rGradient = mrAreaPropertyPanel.maGradientElliptical;
			}
			break;
		case XGRAD_SQUARE:
			{
				rGradient = mrAreaPropertyPanel.maGradientSquare;
			}
			break;
		case XGRAD_RECT:
			{
				rGradient = mrAreaPropertyPanel.maGradientRect;
			}
			break;
		}
	}
	else
		rGradient = aGradient;
	maMtrTrgrCenterX.SetValue(rGradient.GetXOffset());
	maMtrTrgrCenterY.SetValue(rGradient.GetYOffset());
	maMtrTrgrAngle.SetValue(rGradient.GetAngle() / 10);
	maMtrTrgrStartValue.SetValue((sal_uInt16)((((sal_uInt16)rGradient.GetStartColor().GetRed() + 1) * 100) / 255));
	maMtrTrgrEndValue.SetValue((sal_uInt16)((((sal_uInt16)rGradient.GetEndColor().GetRed() + 1) * 100) / 255));
	maMtrTrgrBorder.SetValue(rGradient.GetBorder());	
}

void SvxAreaTrGrPage::ExecuteValueModify( sal_uInt8 nStartCol, sal_uInt8 nEndCol )
{
	//Added 
	sal_Int16 aMtrValue = (sal_Int16)maMtrTrgrAngle.GetValue();
	while(aMtrValue<0)
		aMtrValue += 360;
	sal_uInt16 nVal = aMtrValue/360;
	nVal = aMtrValue - nVal*360;
	maMtrTrgrAngle.SetValue(nVal);
	//End of new code
	XGradient aTmpGradient(
		Color(nStartCol, nStartCol, nStartCol),
		Color(nEndCol, nEndCol, nEndCol),
		(XGradientStyle)(mrAreaPropertyPanel.mpLBTransType->GetSelectEntryPos()-2),
		(sal_uInt16)maMtrTrgrAngle.GetValue() * 10,
		(sal_uInt16)maMtrTrgrCenterX.GetValue(),
		(sal_uInt16)maMtrTrgrCenterY.GetValue(),
		(sal_uInt16)maMtrTrgrBorder.GetValue(),
		100, 100);

	switch(aTmpGradient.GetGradientStyle())
	{
	case XGRAD_LINEAR:
		{
			mrAreaPropertyPanel.maGradientLinear = aTmpGradient;
		}
		break;
	case XGRAD_AXIAL:
		{
			mrAreaPropertyPanel.maGradientAxial = aTmpGradient;
		}
		break;
	case XGRAD_RADIAL:
		{
			mrAreaPropertyPanel.maGradientRadial = aTmpGradient;
		}
		break;
	case XGRAD_ELLIPTICAL:
		{
			mrAreaPropertyPanel.maGradientElliptical = aTmpGradient;
		}
		break;
	case XGRAD_SQUARE:
		{
			mrAreaPropertyPanel.maGradientSquare = aTmpGradient;
		}
		break;
	case XGRAD_RECT:
		{
			mrAreaPropertyPanel.maGradientRect = aTmpGradient;
		}
		break;
	}

	SfxItemPool* pPool = NULL;
	bool bEnable = true;		
	XFillFloatTransparenceItem aGradientItem(pPool,aTmpGradient, bEnable );

	mpBindings->GetDispatcher()->Execute( SID_SVX_AREA_TRANSP_GRADIENT, SFX_CALLMODE_RECORD, &aGradientItem, 0L );
}

IMPL_LINK(SvxAreaTrGrPage, ModifiedTrgrHdl_Impl, void *, pControl)
{
	sal_uInt8 nStartCol = (sal_uInt8)(((sal_uInt16)maMtrTrgrStartValue.GetValue() * 255) / 100);
	sal_uInt8 nEndCol = (sal_uInt8)(((sal_uInt16)maMtrTrgrEndValue.GetValue() * 255) / 100);
	ExecuteValueModify( nStartCol, nEndCol );
	return( 0L );
}

IMPL_LINK(SvxAreaTrGrPage, Left_Click45_Impl, void *, pControl)
{
	sal_uInt8 nStartCol = (sal_uInt8)(((sal_uInt16)maMtrTrgrStartValue.GetValue() * 255) / 100);
	sal_uInt8 nEndCol = (sal_uInt8)(((sal_uInt16)maMtrTrgrEndValue.GetValue() * 255) / 100);
	sal_uInt16 aTemp = (sal_uInt16)maMtrTrgrAngle.GetValue();
	if(aTemp>=315)
		aTemp -= 360;
	aTemp += 45;
	maMtrTrgrAngle.SetValue(aTemp);
	ExecuteValueModify( nStartCol, nEndCol );
	return( 0L );
}

IMPL_LINK(SvxAreaTrGrPage, Right_Click45_Impl, void *, pControl)
{
	sal_uInt8 nStartCol = (sal_uInt8)(((sal_uInt16)maMtrTrgrStartValue.GetValue() * 255) / 100);
	sal_uInt8 nEndCol = (sal_uInt8)(((sal_uInt16)maMtrTrgrEndValue.GetValue() * 255) / 100);
	sal_uInt16 aTemp = (sal_uInt16)maMtrTrgrAngle.GetValue();
	if(aTemp<45)
		aTemp += 360;
	aTemp -= 45;
	maMtrTrgrAngle.SetValue(aTemp);
	ExecuteValueModify( nStartCol, nEndCol );
	return( 0L );
}

//////////////////////////////////////////////////////////////////////////////
// namespace open

namespace svx { namespace sidebar {

//////////////////////////////////////////////////////////////////////////////

AreaPropertyPanel::AreaPropertyPanel(
    Window* pParent,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings)
:   Control(
        pParent, 
        SVX_RES(RID_SIDEBAR_AREA_PANEL)),
    meLastXFS(-1),
    maLastColor(Color(COL_DEFAULT_SHAPE_FILLING)),
    mnLastPosGradient(0),
    mnLastPosHatch(0),
    mnLastPosBitmap(0),
    mnLastTransSolid(50),
    maGradientLinear(),
    maGradientAxial(),
    maGradientRadial(),
    maGradientElliptical(),
    maGradientSquare(),
    maGradientRect(),
    mpColorTextFT(new FixedText(this, SVX_RES(FT_COLOR_LIST))),
    mpLbFillType(new SvxFillTypeBox(this)),
    mpLbFillAttr(new SvxFillAttrBox(this)),
    mpToolBoxColor(sfx2::sidebar::ControlFactory::CreateToolBox(this, SVX_RES(TB_COLOR))),
    mpTrspTextFT(new FixedText(this, SVX_RES(FL_TRSP_TEXT))),
    mpLBTransType(new ListBox(this, SVX_RES(LB_TRGR_TYPES))),
    mpMTRTransparent(new MetricField(this, SVX_RES(MTR_TRANSPARENT))),
    mpBTNGradient(sfx2::sidebar::ControlFactory::CreateToolBox(this,SVX_RES(BTN_GRADIENT))),
    mpColorUpdater(new ::svx::ToolboxButtonColorUpdater(SID_ATTR_FILL_COLOR, TBI_COLOR, mpToolBoxColor.get(), TBX_UPDATER_MODE_CHAR_COLOR_NEW)),
    mpStyleItem(),
    mpColorItem(),
    mpFillGradientItem(),
    mpHatchItem(),
    mpBitmapItem(),
    mpColorTableItem(),
    mpGradientListItem(),
    mpHatchListItem(),
    mpBitmapListItem(),
    maStyleControl(SID_ATTR_FILL_STYLE, *pBindings, *this),
    maColorControl(SID_ATTR_FILL_COLOR, *pBindings, *this),
    maGradientControl(SID_ATTR_FILL_GRADIENT, *pBindings, *this),
    maHatchControl(SID_ATTR_FILL_HATCH, *pBindings, *this),
    maBitmapControl(SID_ATTR_FILL_BITMAP, *pBindings, *this),
    maColorTableControl(SID_COLOR_TABLE, *pBindings, *this),
    maGradientListControl(SID_GRADIENT_LIST, *pBindings, *this),
    maHatchListControl(SID_HATCH_LIST, *pBindings, *this),
    maBitmapListControl(SID_BITMAP_LIST, *pBindings, *this),
    maTransTypeController(SID_SVX_AREA_TRANS_TYPE, *pBindings, *this),
    maTransController(SID_SVX_AREA_TRANSPARENCY, *pBindings, *this),
    maTransGradientController(SID_SVX_AREA_TRANSP_GRADIENT, *pBindings, *this),
    maImgAxial(SVX_RES(IMG_AXIAL)),
    maImgElli(SVX_RES(IMG_ELLI)),
    maImgQuad(SVX_RES(IMG_QUAD)),
    maImgRadial(SVX_RES(IMG_RADIAL)),
    maImgSquare(SVX_RES(IMG_SQUARE)),
    maImgLinear(SVX_RES(IMG_LINEAR)),
    maImgColor(SVX_RES(IMG_COLOR)),
    maImgAxialH(SVX_RES(IMG_AXIAL_H)),
    maImgElliH(SVX_RES(IMG_ELLI_H)),
    maImgQuadH(SVX_RES(IMG_QUAD_H)),
    maImgRadialH(SVX_RES(IMG_RADIAL_H)),
    maImgSquareH(SVX_RES(IMG_SQUARE_H)),
    maImgLinearH(SVX_RES(IMG_LINEAR_H)),
    maImgColorH(SVX_RES(IMG_COLOR_H)),
    msHelpFillType(SVX_RES(STR_HELP_TYPE)),
    msHelpFillAttr(SVX_RES(STR_HELP_ATTR)),
    mpTrGrFloatWin(),
    mpTrGrPage(),
    mpFloatWinColor(),
    mpPageColor(),
    mpGradientItem(),
    mpTransTypeItem(),
    mxFrame(rxFrame),
    maContext(),
    mpBindings(pBindings),
    mbTBShow(true),
    mbColorAvail(true)
{
    Initialize();
    FreeResource();
}

//////////////////////////////////////////////////////////////////////////////

AreaPropertyPanel::~AreaPropertyPanel()
{
}

//////////////////////////////////////////////////////////////////////////////

void AreaPropertyPanel::ShowMenu (void)
{
    if (mpBindings != NULL)
    {
        SfxDispatcher* pDispatcher = mpBindings->GetDispatcher();
        if (pDispatcher != NULL)
            pDispatcher->Execute(SID_ATTRIBUTES_AREA, SFX_CALLMODE_ASYNCHRON);
    }
}

//////////////////////////////////////////////////////////////////////////////

void AreaPropertyPanel::Initialize()
{
    maGradientLinear.SetXOffset(DEFAULT_CENTERX);
    maGradientLinear.SetYOffset(DEFAULT_CENTERY);
    maGradientLinear.SetAngle(DEFAULT_ANGLE);
    maGradientLinear.SetStartColor(Color(DEFAULT_STARTVALUE));
    maGradientLinear.SetEndColor(Color(DEFAULT_ENDVALUE));
    maGradientLinear.SetBorder(DEFAULT_BORDER);
    maGradientLinear.SetGradientStyle(XGRAD_LINEAR);

    maGradientAxial = maGradientLinear;
    maGradientAxial.SetGradientStyle(XGRAD_AXIAL);

    maGradientRadial = maGradientLinear;
    maGradientRadial.SetGradientStyle(XGRAD_RADIAL);

    maGradientElliptical = maGradientLinear;
    maGradientElliptical.SetGradientStyle(XGRAD_ELLIPTICAL);

    maGradientSquare = maGradientLinear;
    maGradientSquare.SetGradientStyle(XGRAD_SQUARE);

    maGradientRect = maGradientLinear;
    maGradientRect.SetGradientStyle(XGRAD_RECT);

    //Added for windows classic theme
    mpBTNGradient->SetBackground(Wallpaper());
    mpBTNGradient->SetPaintTransparent(true);

    Size aLogicalFillSize(MBOX_WIDTH,LISTBOX_HEIGHT);
    Size aLogicalAttrSize(MBOX_WIDTH + 1,LISTBOX_HEIGHT);

    Point aPoint(SECTIONPAGE_MARGIN_HORIZONTAL,SECTIONPAGE_MARGIN_VERTICAL_TOP + FIXED_TEXT_HEIGHT + TEXT_CONTROL_SPACING_VERTICAL);
    Point aPoint_Picker(SECTIONPAGE_MARGIN_HORIZONTAL + MBOX_WIDTH + CONTROL_SPACING_HORIZONTAL,SECTIONPAGE_MARGIN_VERTICAL_TOP + FIXED_TEXT_HEIGHT + TEXT_CONTROL_SPACING_VERTICAL);

    Size aTypeSize(LogicToPixel(aLogicalFillSize, MAP_APPFONT));
    Size aAttrSize(LogicToPixel(aLogicalAttrSize, MAP_APPFONT));

    Point aTypePoint(LogicToPixel(aPoint, MAP_APPFONT));
    Point aAttrPoint(LogicToPixel(aPoint_Picker, MAP_APPFONT));

    mpLbFillType->SetPosSizePixel(aTypePoint,aTypeSize);
    mpLbFillAttr->SetPosSizePixel(aAttrPoint,aAttrSize);

    mpLbFillType->SetHelpId(HID_PPROPERTYPANEL_AREA_LB_FILL_TYPES);
    mpLbFillAttr->SetHelpId(HID_PPROPERTYPANEL_AREA_LB_FILL_ATTR);

    mpLbFillType->SetQuickHelpText(msHelpFillType);
    mpLbFillAttr->SetQuickHelpText(msHelpFillAttr);

    mpLbFillType->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Fill")));    //wj acc
    mpLbFillAttr->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Fill")));    //wj acc

    Link aLink = LINK( this, AreaPropertyPanel, SelectFillTypeHdl );
    mpLbFillType->SetSelectHdl( aLink );

    aLink = LINK( this, AreaPropertyPanel, SelectFillAttrHdl );
    mpLbFillAttr->SetSelectHdl( aLink );

    //add  for new color picker
    mpLbFillAttr->Hide();
    mpToolBoxColor->SetItemImage(TBI_COLOR, GetDisplayBackground().GetColor().IsDark()? maImgColorH : maImgColor);
    Size aTbxSize( mpToolBoxColor->CalcWindowSizePixel() ); 
    mpToolBoxColor->SetOutputSizePixel( aTbxSize );
    mpToolBoxColor->SetItemBits( TBI_COLOR, mpToolBoxColor->GetItemBits( TBI_COLOR ) | TIB_DROPDOWNONLY );
    mpToolBoxColor->SetBackground(Wallpaper());
    mpToolBoxColor->SetPaintTransparent(true);
    mpToolBoxColor->SetQuickHelpText(TBI_COLOR, String(SVX_RES(STR_HELP_COLOR)));    //wj acc
    //mpToolBoxColor->SetItemText(TBI_COLOR, msHelpFillAttr);

    long aHeightLBStyle = mpLbFillType->GetSizePixel().getHeight();
    long aLBPosY = mpLbFillType->GetPosPixel().getY();
    long aHeightTBAttr = mpToolBoxColor->GetSizePixel().getHeight();
    Point aPointTBAttr = mpToolBoxColor->GetPosPixel();
    aPointTBAttr.setY( aLBPosY + aHeightLBStyle / 2 - aHeightTBAttr / 2);
    mpToolBoxColor->SetPosPixel(aPointTBAttr);
 
    aLink = LINK(this, AreaPropertyPanel, ToolBoxColorDropHdl);
    mpToolBoxColor->SetDropdownClickHdl ( aLink );
    mpToolBoxColor->SetSelectHdl ( aLink );

    //add end
    mpLBTransType->SetSelectHdl(LINK(this, AreaPropertyPanel, ChangeTrgrTypeHdl_Impl));
    mpLBTransType->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Transparency")));    //wj acc

    mpMTRTransparent->SetValue( 50 );
    mpMTRTransparent->SetModifyHdl(LINK(this, AreaPropertyPanel, ModifyTransparentHdl_Impl));
    mpMTRTransparent->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Transparency")));    //wj acc

    mpBTNGradient->SetItemBits( TBI_BTX_GRADIENT, mpBTNGradient->GetItemBits( TBI_BTX_GRADIENT ) | TIB_DROPDOWNONLY );
    aLink = LINK( this, AreaPropertyPanel, ClickTrGrHdl_Impl );
    mpBTNGradient->SetDropdownClickHdl( aLink );
    mpBTNGradient->SetSelectHdl( aLink );
    aTbxSize = mpBTNGradient->CalcWindowSizePixel();
    mpBTNGradient->SetOutputSizePixel( aTbxSize );
    mpBTNGradient->SetItemImage(TBI_BTX_GRADIENT,maImgLinear);
    mpBTNGradient->SetQuickHelpText(TBI_BTX_GRADIENT, String(SVX_RES(STR_HELP_GRADIENT)));    //wj acc
    mpBTNGradient->Hide();

    long aHeightLBTrans = mpLBTransType->GetSizePixel().getHeight();
    Point aPointLB = mpLBTransType->GetPosPixel();
    long aPosY = aPointLB.getY();

    Point aPointMetric = mpMTRTransparent->GetPosPixel();
    Point aPointTB = mpMTRTransparent->GetPosPixel();
    long aHeightMetric = mpMTRTransparent->GetSizePixel().getHeight();
    long aHeightTool = mpBTNGradient->GetSizePixel().getHeight();
    aPointMetric.setY(aPosY+aHeightLBTrans/2-aHeightMetric/2);
    aPointTB.setY(aPosY+aHeightLBTrans/2-aHeightTool/2);
    aPointTB.setX(aPointTB.getX()+3);
    mpMTRTransparent->SetPosPixel(aPointMetric);
    mpBTNGradient->SetPosPixel(aPointTB);

    mpLbFillType->SetAccessibleRelationLabeledBy(mpColorTextFT.get());
    mpLbFillAttr->SetAccessibleRelationLabeledBy(mpLbFillAttr.get());
    mpToolBoxColor->SetAccessibleRelationLabeledBy(mpToolBoxColor.get());
    mpLBTransType->SetAccessibleRelationLabeledBy(mpTrspTextFT.get());
    mpMTRTransparent->SetAccessibleRelationLabeledBy(mpMTRTransparent.get());
    mpBTNGradient->SetAccessibleRelationLabeledBy(mpBTNGradient.get());

    SetupIcons();
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( AreaPropertyPanel, SelectFillTypeHdl, ListBox *, pBox )
{
	XFillStyle  eXFS = (XFillStyle)mpLbFillType->GetSelectEntryPos();

	if( (XFillStyle) meLastXFS != eXFS )
	{
			mpLbFillAttr->Clear();
			SfxObjectShell* pSh = SfxObjectShell::Current();
			XFillStyleItem aXFillStyleItem( eXFS );
			GetBindings()->GetDispatcher()->Execute(SID_ATTR_FILL_STYLE, SFX_CALLMODE_RECORD, &aXFillStyleItem, 0L); 

			switch( eXFS )
			{
			case XFILL_NONE:
				{
					mpLbFillAttr->Show();	
					mpToolBoxColor->Hide();	
					mbTBShow = false;
					mpLbFillType->Selected();
					mpLbFillAttr->Disable();
				}
				break;

			case XFILL_SOLID:
				{
					mpLbFillAttr->Hide();	
					mpToolBoxColor->Show();	
					mbTBShow = true;
					String aTmpStr;
					Color aColor = maLastColor;
					XFillColorItem aXFillColorItem( aTmpStr, aColor );
					GetBindings()->GetDispatcher()->Execute(SID_ATTR_FILL_COLOR, SFX_CALLMODE_RECORD, &aXFillColorItem, 0L);
				}
				break;

			case XFILL_GRADIENT:
				{
					mpLbFillAttr->Show();	
					mpToolBoxColor->Hide();	
					mbTBShow = false;
					if ( pSh && pSh->GetItem( SID_GRADIENT_LIST ) )
					{
						// 
						if(mpLbFillAttr->GetEntryCount() == 0)
						{
							SvxGradientListItem aItem( *(const SvxGradientListItem*)(
							pSh->GetItem( SID_GRADIENT_LIST ) ) );
							mpLbFillAttr->Enable();
							mpLbFillAttr->Clear();
							mpLbFillAttr->Fill( aItem.GetGradientList() );	
						}

						if ( mnLastPosGradient != LISTBOX_ENTRY_NOTFOUND)
						{
							SvxGradientListItem aItem( *(const SvxGradientListItem*)( pSh->GetItem( SID_GRADIENT_LIST ) ) );
							if ( mnLastPosGradient < aItem.GetGradientList()->Count() )  
							{
								XGradient aGradient = aItem.GetGradientList()->GetGradient( mnLastPosGradient )->GetGradient();
								XFillGradientItem aXFillGradientItem( mpLbFillAttr->GetEntry(mnLastPosGradient), aGradient );
								GetBindings()->GetDispatcher()->Execute(SID_ATTR_FILL_GRADIENT, SFX_CALLMODE_RECORD, &aXFillGradientItem, 0L);
								mpLbFillAttr->SelectEntryPos(mnLastPosGradient);	//add 
							}
						}
					}
					else
						mpLbFillAttr->Disable();
				}
				break;

			case XFILL_HATCH:
				{
					mpLbFillAttr->Show();	
					mpToolBoxColor->Hide();	
					mbTBShow = false;
					if ( pSh && pSh->GetItem( SID_HATCH_LIST ) )
					{
						// 
						if(mpLbFillAttr->GetEntryCount() == 0)
						{
							SvxHatchListItem aItem( *(const SvxHatchListItem*)(
								pSh->GetItem( SID_HATCH_LIST ) ) );
							mpLbFillAttr->Enable();
							mpLbFillAttr->Clear();
							mpLbFillAttr->Fill( aItem.GetHatchList() );
						}

						if ( mnLastPosHatch != LISTBOX_ENTRY_NOTFOUND )
						{
							SvxHatchListItem aItem( *(const SvxHatchListItem*)( pSh->GetItem( SID_HATCH_LIST ) ) );
							if ( mnLastPosHatch < aItem.GetHatchList()->Count() )  
							{
								XHatch aHatch = aItem.GetHatchList()->GetHatch( mnLastPosHatch )->GetHatch();
								XFillHatchItem aXFillHatchItem( mpLbFillAttr->GetSelectEntry(), aHatch );
								GetBindings()->GetDispatcher()->Execute(SID_ATTR_FILL_HATCH, SFX_CALLMODE_RECORD, &aXFillHatchItem, 0L);
								mpLbFillAttr->SelectEntryPos(mnLastPosHatch); //add 
							}
						}
					}
					else
						mpLbFillAttr->Disable();
				}
				break;

			case XFILL_BITMAP:
				{
					mpLbFillAttr->Show();	
					mpToolBoxColor->Hide();	
					mbTBShow = false;
					if ( pSh && pSh->GetItem( SID_BITMAP_LIST ) )
					{
						// 
						if(mpLbFillAttr->GetEntryCount() == 0)
						{
							SvxBitmapListItem aItem( *(const SvxBitmapListItem*)(
								pSh->GetItem( SID_BITMAP_LIST ) ) );
							mpLbFillAttr->Enable();
							mpLbFillAttr->Clear();
							mpLbFillAttr->Fill( aItem.GetBitmapList() );
						}

						if ( mnLastPosBitmap != LISTBOX_ENTRY_NOTFOUND )
						{
							SvxBitmapListItem aItem( *(const SvxBitmapListItem*)( pSh->GetItem( SID_BITMAP_LIST ) ) );
							if ( mnLastPosBitmap < aItem.GetBitmapList()->Count() ) 
							{
                                const XBitmapEntry* pXBitmapEntry = aItem.GetBitmapList()->GetBitmap(mnLastPosBitmap);
								XFillBitmapItem aXFillBitmapItem( mpLbFillAttr->GetSelectEntry(), pXBitmapEntry->GetGraphicObject() );
								GetBindings()->GetDispatcher()->Execute(SID_ATTR_FILL_BITMAP, SFX_CALLMODE_RECORD, &aXFillBitmapItem, 0L);
								mpLbFillAttr->SelectEntryPos(mnLastPosBitmap); //add 
							}
						}
					}
					else
						mpLbFillAttr->Disable();
				}
				break;
			}
			meLastXFS = (sal_uInt16)eXFS;
			if( eXFS != XFILL_NONE )
			{
				if ( pBox )
					mpLbFillType->Selected();
			}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( AreaPropertyPanel, SelectFillAttrHdl, ListBox*, pBox )
{
	XFillStyle eXFS = (XFillStyle)mpLbFillType->GetSelectEntryPos();
	XFillStyleItem aXFillStyleItem( eXFS );

	SfxObjectShell* pSh = SfxObjectShell::Current();
	if(pBox)
	{
		if( (XFillStyle) meLastXFS != eXFS )
        {
			GetBindings()->GetDispatcher()->Execute(SID_ATTR_FILL_STYLE, SFX_CALLMODE_RECORD, &aXFillStyleItem, 0L); //Added  20090909
        }

		switch( eXFS )
		{
		case XFILL_SOLID:
			//{
			//	//String aTmpStr = mpLbFillAttr->GetSelectEntry();
			//	//Color aColor = mpLbFillAttr->GetSelectEntryColor();
			//	//if(aColor.GetColor() == 0 && aTmpStr.Equals(String::CreateFromAscii("")))
			//	String aTmpStr;
			//	Color aColor = maLastColor;
			//	XFillColorItem aXFillColorItem( aTmpStr, aColor );
			//	GetBindings()->GetDispatcher()->Execute(SID_ATTR_FILL_COLOR, SFX_CALLMODE_RECORD, &aXFillColorItem, 0L);
			//	maLastColor = aColor;
			//}
			break;

		case XFILL_GRADIENT:
			{
				sal_uInt16 nPos = mpLbFillAttr->GetSelectEntryPos();
				if( nPos == LISTBOX_ENTRY_NOTFOUND )
					nPos = mnLastPosGradient;

				if ( nPos != LISTBOX_ENTRY_NOTFOUND && pSh && pSh->GetItem( SID_GRADIENT_LIST ) )
				{
					SvxGradientListItem aItem( *(const SvxGradientListItem*)( pSh->GetItem( SID_GRADIENT_LIST ) ) );
					if ( nPos < aItem.GetGradientList()->Count() )  
					{
						XGradient aGradient = aItem.GetGradientList()->GetGradient( nPos )->GetGradient();
						XFillGradientItem aXFillGradientItem( mpLbFillAttr->GetSelectEntry(), aGradient );
						GetBindings()->GetDispatcher()->Execute(SID_ATTR_FILL_GRADIENT, SFX_CALLMODE_RECORD, &aXFillGradientItem, 0L);
					}
				}
				if(nPos != LISTBOX_ENTRY_NOTFOUND)
					mnLastPosGradient = nPos;
			}
			break;

		case XFILL_HATCH:
			{
				sal_uInt16 nPos = mpLbFillAttr->GetSelectEntryPos();
				if( nPos == LISTBOX_ENTRY_NOTFOUND )
					nPos = mnLastPosHatch;
				if ( nPos != LISTBOX_ENTRY_NOTFOUND && pSh && pSh->GetItem( SID_HATCH_LIST ) )
				{
					SvxHatchListItem aItem( *(const SvxHatchListItem*)( pSh->GetItem( SID_HATCH_LIST ) ) );
					if ( nPos < aItem.GetHatchList()->Count() )  
					{
						XHatch aHatch = aItem.GetHatchList()->GetHatch( nPos )->GetHatch();
						XFillHatchItem aXFillHatchItem( mpLbFillAttr->GetSelectEntry(), aHatch );
						GetBindings()->GetDispatcher()->Execute(SID_ATTR_FILL_HATCH, SFX_CALLMODE_RECORD, &aXFillHatchItem, 0L);
					}
				}
				if(nPos != LISTBOX_ENTRY_NOTFOUND)
					mnLastPosHatch = nPos;
			}
			break;

		case XFILL_BITMAP:
			{
				sal_uInt16 nPos = mpLbFillAttr->GetSelectEntryPos();
				if( nPos == LISTBOX_ENTRY_NOTFOUND )
					nPos = mnLastPosBitmap;
				if ( nPos != LISTBOX_ENTRY_NOTFOUND && pSh && pSh->GetItem( SID_BITMAP_LIST ) )
				{
					SvxBitmapListItem aItem( *(const SvxBitmapListItem*)( pSh->GetItem( SID_BITMAP_LIST ) ) );
					if ( nPos < aItem.GetBitmapList()->Count() ) 
					{
                        const XBitmapEntry* pXBitmapEntry = aItem.GetBitmapList()->GetBitmap(nPos);
						XFillBitmapItem aXFillBitmapItem( mpLbFillAttr->GetSelectEntry(), pXBitmapEntry->GetGraphicObject() );
						GetBindings()->GetDispatcher()->Execute(SID_ATTR_FILL_BITMAP, SFX_CALLMODE_RECORD, &aXFillBitmapItem, 0L);
					}
				}
				if(nPos != LISTBOX_ENTRY_NOTFOUND)
					mnLastPosBitmap = nPos;
			}
			break;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//add  for color picker

IMPL_LINK(AreaPropertyPanel, ToolBoxColorDropHdl,ToolBox*, pToolBox)
{
	sal_uInt16 nId = pToolBox->GetCurItemId();
	if(nId == TBI_COLOR)
	{
		pToolBox->SetItemDown( nId, true );

		SvxColorPage* pColorPage = GetColorPage();
		Size aFloatSz = pColorPage->GetOutputSizePixel();
//		GetColorFloatWin()->SetSizePixel( aFloatSz );

		Point aPos = mpToolBoxColor->GetPosPixel();
		aPos = OutputToScreenPixel( aPos );
		Size aSize = mpToolBoxColor->GetSizePixel();
		Rectangle aRect( aPos, aSize );

//		GetColorFloatWin()->StartPopupMode( aRect, FLOATWIN_POPUPMODE_NOFOCUSCLOSE|FLOATWIN_POPUPMODE_DOWN );
//		GetColorFloatWin()->SetPopupModeFlags(GetColorFloatWin()->GetPopupModeFlags() | FLOATWIN_POPUPMODE_NOAPPFOCUSCLOSE );
		
		pColorPage->GetFocus();
		//modified );
		if(mpColorItem)
			pColorPage->SetCurColorSelect(mpColorItem->GetColorValue(), mbColorAvail);		
		else
			pColorPage->SetCurColorSelect(COL_WHITE, false);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

void AreaPropertyPanel::ImpEnsureFloatWinColorAndPageColor()
{
    if(!mpFloatWinColor)
    {
        mpFloatWinColor.reset(new PropertyPanelPopuplWindow(this));
        mpFloatWinColor->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Color")));	//wj acc
        mpFloatWinColor->SetPopupModeEndHdl( LINK( this, AreaPropertyPanel, ImplPopupModeEndHdl ) );
        mpPageColor.reset(new SvxColorPage(mpFloatWinColor.get(), *this));
        mpFloatWinColor->SetBorderStyle(mpFloatWinColor->GetBorderStyle() | WINDOW_BORDER_MENU);
    }
}

//////////////////////////////////////////////////////////////////////////////
//color picker popup window 

SvxColorPage* AreaPropertyPanel::GetColorPage()
{
    ImpEnsureFloatWinColorAndPageColor();

    return mpPageColor.get();
}

//////////////////////////////////////////////////////////////////////////////

PropertyPanelPopuplWindow* AreaPropertyPanel::GetColorFloatWin()
{
    ImpEnsureFloatWinColorAndPageColor();

    return mpFloatWinColor.get();
}

//////////////////////////////////////////////////////////////////////////////

void AreaPropertyPanel::SetupIcons(void)
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

//////////////////////////////////////////////////////////////////////////////

AreaPropertyPanel* AreaPropertyPanel::Create (
    Window* pParent,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings)
{
    if (pParent == NULL)
        throw lang::IllegalArgumentException(A2S("no parent Window given to AreaPropertyPanel::Create"), NULL, 0);
    if ( ! rxFrame.is())
        throw lang::IllegalArgumentException(A2S("no XFrame given to AreaPropertyPanel::Create"), NULL, 1);
    if (pBindings == NULL)
        throw lang::IllegalArgumentException(A2S("no SfxBindings given to AreaPropertyPanel::Create"), NULL, 2);
    
    return new AreaPropertyPanel(
        pParent,
        rxFrame,
        pBindings);
}

//////////////////////////////////////////////////////////////////////////////

void AreaPropertyPanel::DataChanged(
    const DataChangedEvent& rEvent)
{
    (void)rEvent;
    
    SetupIcons();
}

//////////////////////////////////////////////////////////////////////////////

void AreaPropertyPanel::HandleContextChange(
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

//////////////////////////////////////////////////////////////////////////////

void AreaPropertyPanel::NotifyItemUpdate( 
    sal_uInt16 nSID, 
    SfxItemState eState, 
    const SfxPoolItem* pState)
{	
	XFillStyle eXFS;
	SfxObjectShell* pSh = SfxObjectShell::Current(); 
	//add ,,, 20090915
	if (nSID == SID_SVX_AREA_TRANS_TYPE)
	{
		if( eState >= SFX_ITEM_AVAILABLE)
		{
			mpTransTypeItem.reset(pState ? (SfxUInt16Item*)pState->Clone() : 0);
			sal_uInt16 m_nValue =  mpTransTypeItem->GetValue();
			if(m_nValue>0 && m_nValue <= 100)
			{
				mpLBTransType->Enable();
				mpLBTransType->SelectEntryPos(1);
				mpBTNGradient->Hide();
				mpMTRTransparent->Show();
				mpMTRTransparent->Enable();
				mpMTRTransparent->SetValue(m_nValue);
				//Add 
				if(mpTrGrFloatWin)
					mpTrGrFloatWin->EndPopupMode();
			}
			else
			{
				switch ( m_nValue )
				{
				case 0:
					mpLBTransType->Enable();
					mpLBTransType->SelectEntryPos(0);
					mpBTNGradient->Hide();
					mpMTRTransparent->Enable();
					mpMTRTransparent->Show();
					mpMTRTransparent->SetValue(0);
					//Add 
					if(mpTrGrFloatWin)
						mpTrGrFloatWin->EndPopupMode();
					break;
				case 102:
				case 103:
				case 104:
				case 105:
				case 106:
				case 107:
					mpLBTransType->Enable();
					mpLBTransType->SelectEntryPos( m_nValue - 100 );
					mpMTRTransparent->Hide();
					mpBTNGradient->Enable();
					mpBTNGradient->Show();
					//for beta1
					switch ( m_nValue )
					{
					case 102:
						mpBTNGradient->SetItemImage(TBI_BTX_GRADIENT, GetSettings().GetStyleSettings().GetHighContrastMode()? maImgLinearH : maImgLinear);	// high contrast
						break;
					case 103:
						mpBTNGradient->SetItemImage(TBI_BTX_GRADIENT,GetSettings().GetStyleSettings().GetHighContrastMode()? maImgAxialH : maImgAxial);
						break;
					case 104:
						mpBTNGradient->SetItemImage(TBI_BTX_GRADIENT,GetSettings().GetStyleSettings().GetHighContrastMode()? maImgRadialH : maImgRadial);
						break;
					case 105:
						mpBTNGradient->SetItemImage(TBI_BTX_GRADIENT,GetSettings().GetStyleSettings().GetHighContrastMode()? maImgElliH : maImgElli);
						break;
					case 106:
						mpBTNGradient->SetItemImage(TBI_BTX_GRADIENT,GetSettings().GetStyleSettings().GetHighContrastMode()? maImgQuadH : maImgQuad);
						break;
					case 107:
						mpBTNGradient->SetItemImage(TBI_BTX_GRADIENT,GetSettings().GetStyleSettings().GetHighContrastMode()? maImgSquareH : maImgSquare);
						break;
					}
					//end of new code
					break;
				}
			}
		}
		else
		{
			mpLBTransType->SetNoSelection();
			mpMTRTransparent->Disable();	
			mpMTRTransparent->Show();	
			mpBTNGradient->Disable();
			mpBTNGradient->Hide();
		}
	}
	//End of new code
	else if (nSID == SID_SVX_AREA_TRANSPARENCY)
	{		
		if( eState == SFX_ITEM_AVAILABLE )
		{
			mpMTRTransparent->Enable();

		}
		else if(eState == SFX_ITEM_DISABLED)
			mpLBTransType->Disable();   // 20090923
	}
	else if (nSID == SID_SVX_AREA_TRANSP_GRADIENT)
	{
		if( eState == SFX_ITEM_AVAILABLE )
		{
			mpBTNGradient->Enable();	

			const XFillFloatTransparenceItem* pGradientItem = PTR_CAST(XFillFloatTransparenceItem, pState);
			if (pGradientItem)
            {
				mpGradientItem.reset((XFillFloatTransparenceItem*)pGradientItem->Clone());
			}
		}
		else if(eState == SFX_ITEM_DISABLED)
			mpLBTransType->Disable();   // 20090923

	}
	else if	(nSID == SID_ATTR_FILL_STYLE )
	{
		if( eState == SFX_ITEM_DISABLED )
		{
			mpLbFillType->Disable(); 
			mpLbFillType->SetNoSelection();
			mpLbFillAttr->Show();
			mpLbFillAttr->Disable();
			mpLbFillAttr->SetNoSelection();
			mpToolBoxColor->Hide();
			mbTBShow = false;
			meLastXFS = -1; 
            mpStyleItem.reset();
		}
		else if( SFX_ITEM_AVAILABLE == eState )
		{
			mpStyleItem.reset(pState ? (XFillStyleItem*)pState->Clone() : 0);
			mpLbFillType->Enable();

			eXFS = (XFillStyle)mpStyleItem->GetValue();
			meLastXFS = eXFS;  
			mpLbFillType->SelectEntryPos(
				sal::static_int_cast< sal_uInt16 >( eXFS ) );
			//Added for select invisable
			if(eXFS == XFILL_NONE)
			{
				mpLbFillAttr->SetNoSelection();
				mpLbFillAttr->Disable();
			}
			//else
			//	mpLbFillAttr->Enable();
			Update();
			//SelectFillTypeHdl( NULL );
		}
		else
		{
			mpLbFillType->SetNoSelection();
			mpLbFillAttr->Show();
			mpLbFillAttr->Disable();
			mpLbFillAttr->SetNoSelection();
			mpToolBoxColor->Hide();
			mbTBShow = false;
			meLastXFS = -1;  //Added 
			mpStyleItem.reset();
		}
	}
	else if(nSID == SID_ATTR_FILL_COLOR)
	{
		if( SFX_ITEM_AVAILABLE == eState)
		{	
			mpColorItem.reset(pState ? (XFillColorItem*)pState->Clone() : 0);
		}
		if( mpStyleItem && (XFillStyle)mpStyleItem->GetValue() == XFILL_SOLID)
		{
			mpLbFillAttr->Hide();
			mpToolBoxColor->Show();
			mbTBShow = true;
			if( SFX_ITEM_AVAILABLE == eState)
			{	
				mpToolBoxColor->Enable();
				mbColorAvail = true;	//
			    // maLastColor = mpColorItem->GetColorValue();
				Update();
			}
			else if(SFX_ITEM_DISABLED == eState )
			{
				mpToolBoxColor->Disable();
				mbColorAvail = false;	//
				mpColorUpdater->Update(COL_WHITE);
			}
			else
			{
				mbColorAvail = false;	//
				mpColorUpdater->Update(COL_WHITE);
			}
		}
	}
	else if(nSID == SID_ATTR_FILL_GRADIENT)
	{
		if( SFX_ITEM_AVAILABLE == eState)
		{
			mpFillGradientItem.reset(pState ? (XFillGradientItem*)pState->Clone() : 0);
		}
		if( mpStyleItem && (XFillStyle)mpStyleItem->GetValue() == XFILL_GRADIENT )
		{
			mpLbFillAttr->Show();
			mpToolBoxColor->Hide();
			mbTBShow = false;
			if( SFX_ITEM_AVAILABLE == eState)
			{
				mpLbFillAttr->Enable();
				Update();
			}
			
			else if(SFX_ITEM_DISABLED == eState )
			{
				mpLbFillAttr->Disable();
				mpLbFillAttr->SetNoSelection();
			}
			else
				mpLbFillAttr->SetNoSelection();
		}
	}
	else if(nSID == SID_ATTR_FILL_HATCH)
	{
		if( SFX_ITEM_AVAILABLE == eState)
		{	
			mpHatchItem.reset(pState ? (XFillHatchItem*)pState->Clone() : 0);
		}
		if( mpStyleItem && (XFillStyle)mpStyleItem->GetValue() == XFILL_HATCH )
		{
			mpLbFillAttr->Show();
			mpToolBoxColor->Hide();
			mbTBShow = false;
			if( SFX_ITEM_AVAILABLE == eState)
			{	
				mpLbFillAttr->Enable();
				Update();
			}
			else if(SFX_ITEM_DISABLED == eState )
			{
				mpLbFillAttr->Disable();
				mpLbFillAttr->SetNoSelection();
			}
			else
				mpLbFillAttr->SetNoSelection();
		}
	}
	else if(nSID == SID_ATTR_FILL_BITMAP)
	{
		if( SFX_ITEM_AVAILABLE == eState)
		{
			mpBitmapItem.reset(pState ? (XFillBitmapItem*)pState->Clone() : 0);
		}
		if( mpStyleItem && (XFillStyle)mpStyleItem->GetValue() == XFILL_BITMAP )
		{
			mpLbFillAttr->Show();
			mpToolBoxColor->Hide();
			mbTBShow = false;
			if( SFX_ITEM_AVAILABLE == eState)
			{
				mpLbFillAttr->Enable();
				Update();
			}
			else if(SFX_ITEM_DISABLED == eState )
			{
				mpLbFillAttr->Disable();
				mpLbFillAttr->SetNoSelection();
			}
			else
				mpLbFillAttr->SetNoSelection();
		}
	}
	else if(nSID == SID_COLOR_TABLE)
	{
		if( SFX_ITEM_AVAILABLE == eState)
		{
			mpColorTableItem.reset(pState ? (SvxColorTableItem*)pState->Clone() : 0);

			if( mpStyleItem && (XFillStyle)mpStyleItem->GetValue()== XFILL_SOLID)
			{
				if ( mpColorItem )
				{
					String aString( mpColorItem->GetName() );
					Color aColor = mpColorItem->GetColorValue();
					mpLbFillAttr->Clear();
					SvxColorTableItem aItem( *(const SvxColorTableItem*)(
						pSh->GetItem( SID_COLOR_TABLE ) ) );
					mpLbFillAttr->Enable();
					mpLbFillAttr->Fill( aItem.GetColorTable() );
					mpLbFillAttr->SelectEntry( aColor );
				}
				else 
					mpLbFillAttr->SetNoSelection();
			}
		}
	}
	else if(nSID == SID_GRADIENT_LIST)
	{
		if( SFX_ITEM_AVAILABLE == eState)
		{
			mpGradientListItem.reset(pState ? (SvxGradientListItem*)pState->Clone() : 0);

			if( mpStyleItem && (XFillStyle)mpStyleItem->GetValue() == XFILL_GRADIENT)
			{
				if ( mpFillGradientItem )
				{
					String aString( mpFillGradientItem->GetName() );
					mpLbFillAttr->Clear();
					SvxGradientListItem aItem( *(const SvxGradientListItem*)(
						pSh->GetItem( SID_GRADIENT_LIST ) ) );
					mpLbFillAttr->Enable();
					mpLbFillAttr->Fill( aItem.GetGradientList() );
					mpLbFillAttr->SelectEntry( aString );
				}
				else
					mpLbFillAttr->SetNoSelection();
			}
		}
	}
	else if(nSID == SID_HATCH_LIST)
	{
		if( SFX_ITEM_AVAILABLE == eState)
		{
			mpHatchListItem.reset(pState ? (SvxHatchListItem*)pState->Clone() : 0);

			if( mpStyleItem && (XFillStyle)mpStyleItem->GetValue() == XFILL_HATCH )
			{
				if ( mpHatchItem)
				{
					String aString( mpHatchItem->GetName() );
					mpLbFillAttr->Clear();
					SvxHatchListItem aItem( *(const SvxHatchListItem*)(
						pSh->GetItem( SID_HATCH_LIST ) ) );
					mpLbFillAttr->Enable();
					mpLbFillAttr->Fill( aItem.GetHatchList() );
					mpLbFillAttr->SelectEntry( aString );
				}
				else
					mpLbFillAttr->SetNoSelection();
			}
		}
	}
	else if(nSID == SID_BITMAP_LIST)
	{
		if( SFX_ITEM_AVAILABLE == eState)
		{
			mpBitmapListItem.reset(pState ? (SvxBitmapListItem*)pState->Clone() : 0);

			if( mpStyleItem && (XFillStyle)mpStyleItem->GetValue() == XFILL_BITMAP )
			{
				if ( mpBitmapItem )
				{
					String aString( mpBitmapItem->GetName() );
					mpLbFillAttr->Clear();
					SvxBitmapListItem aItem( *(const SvxBitmapListItem*)(
						pSh->GetItem( SID_BITMAP_LIST ) ) );
					mpLbFillAttr->Enable();
					mpLbFillAttr->Fill( aItem.GetBitmapList() );
					mpLbFillAttr->SelectEntry( aString );
				}
				else
					mpLbFillAttr->SetNoSelection();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

SfxBindings* AreaPropertyPanel::GetBindings() 
{ 
    return mpBindings; 
}

//////////////////////////////////////////////////////////////////////////////

void AreaPropertyPanel::Update()
{
	if ( mpStyleItem )
	{
		XFillStyle eXFS = (XFillStyle)mpStyleItem->GetValue();
		SfxObjectShell* pSh = SfxObjectShell::Current();

		switch( eXFS )
		{
			case XFILL_NONE:
			{
				mpLbFillAttr->Show();	//wj for new color picker
				mpToolBoxColor->Hide();	//wj for new color picker
				mbTBShow = false;
			}
			break;

			case XFILL_SOLID:
			{
				if ( mpColorItem )
				{
					mpLbFillAttr->Hide();	//wj for new color picker
					mpToolBoxColor->Show();	//wj for new color picker
					mbTBShow = true;					
					mpColorUpdater->Update(mpColorItem->GetColorValue());
				}
				else
					mpColorUpdater->Update(COL_WHITE);
			}
			break;

			case XFILL_GRADIENT:
			{
				mpLbFillAttr->Show();	//wj for new color picker
				mpToolBoxColor->Hide();	//wj for new color picker
				mbTBShow = false;
				if ( pSh && pSh->GetItem( SID_GRADIENT_LIST ) )
				{
					SvxGradientListItem aItem( *(const SvxGradientListItem*)(
						pSh->GetItem( SID_GRADIENT_LIST ) ) );
					mpLbFillAttr->Enable();
					mpLbFillAttr->Clear();
					mpLbFillAttr->Fill( aItem.GetGradientList() );
					if ( mpFillGradientItem )
					{
						String aString( mpFillGradientItem->GetName() );
						mpLbFillAttr->SelectEntry( aString );
					}
					else
						mpLbFillAttr->SetNoSelection();
				}
				else
					mpLbFillAttr->SetNoSelection();
			}
			break;

			case XFILL_HATCH:
			{
				mpLbFillAttr->Show();	//wj for new color picker
				mpToolBoxColor->Hide();	//wj for new color picker
				mbTBShow = false;
				if ( pSh && pSh->GetItem( SID_HATCH_LIST ) )
				{
					SvxHatchListItem aItem( *(const SvxHatchListItem*)(
						pSh->GetItem( SID_HATCH_LIST ) ) );
					mpLbFillAttr->Enable();
					mpLbFillAttr->Clear();
					mpLbFillAttr->Fill( aItem.GetHatchList() );
					if ( mpHatchItem )
					{
						String aString( mpHatchItem->GetName() );
						mpLbFillAttr->SelectEntry( aString );
					}
					else
						mpLbFillAttr->SetNoSelection();
				}
				else
					mpLbFillAttr->SetNoSelection();
			}
			break;

			case XFILL_BITMAP:
			{
				mpLbFillAttr->Show();	//wj for new color picker
				mpToolBoxColor->Hide();	//wj for new color picker
				mbTBShow = false;
				//mpLbFillAttr->Fill( mpBitmapListItem->GetBitmapList() );
				if ( pSh && pSh->GetItem( SID_BITMAP_LIST ) )
				{
					SvxBitmapListItem aItem( *(const SvxBitmapListItem*)(
						pSh->GetItem( SID_BITMAP_LIST ) ) );
					mpLbFillAttr->Enable();
					mpLbFillAttr->Clear();
					mpLbFillAttr->Fill( aItem.GetBitmapList() );
					if ( mpBitmapItem )
					{
						String aString( mpBitmapItem->GetName() );
						mpLbFillAttr->SelectEntry( aString );
					}
					else
						mpLbFillAttr->SetNoSelection();
				}
				else
					mpLbFillAttr->SetNoSelection();
			}
			break;

			default:
				DBG_ERROR( "Nicht unterstuetzter Flaechentyp" );
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

void AreaPropertyPanel::ImpEnsureTrGrFloatWinAndTrGrPage()
{
    if(!mpTrGrFloatWin)
    {
        mpTrGrFloatWin.reset(new PropertyPanelPopuplWindow(this));
        mpTrGrFloatWin->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Transparency and Gradient")));	//wj acc
        mpTrGrFloatWin->SetPopupModeEndHdl( LINK( this, AreaPropertyPanel, ImplPopupModeEndHdl ) );
        mpTrGrPage.reset(new SvxAreaTrGrPage(mpTrGrFloatWin.get(), *this));
    }
}

SvxAreaTrGrPage* AreaPropertyPanel::GetTrGrPage()
{
    ImpEnsureTrGrFloatWinAndTrGrPage();

    return mpTrGrPage.get();
}

//////////////////////////////////////////////////////////////////////////////

PropertyPanelPopuplWindow* AreaPropertyPanel::GetTrGrFloatWin()
{
    ImpEnsureTrGrFloatWinAndTrGrPage();

    return mpTrGrFloatWin.get();
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( AreaPropertyPanel, ImplPopupModeEndHdl, FloatingWindow*, EMPTYARG )
{	
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( AreaPropertyPanel, ClickTrGrHdl_Impl, ToolBox*, pBox )
{
	SvxAreaTrGrPage* pTrGrPage = GetTrGrPage();
	pTrGrPage->Rearrange(mpGradientItem.get());
	pBox->SetItemDown( TBI_BTX_GRADIENT, true );

	const XGradient& rGradient = mpGradientItem->GetGradientValue();	
	XGradientStyle eXGS(rGradient.GetGradientStyle());

	Size aFloatSz = pTrGrPage->GetOutputSizePixel();
    GetTrGrFloatWin()->SetSizePixel( aFloatSz );

	Point aPos=mpBTNGradient->GetPosPixel();
	aPos = OutputToScreenPixel( aPos );
	Size aSize = mpBTNGradient->GetSizePixel();
	Rectangle aRect( aPos, aSize );

	//Todo
	//if( ImplIsAntiparallel() )
    //    ImplReMirror( aRect );

	GetTrGrFloatWin()->StartPopupMode( aRect, FLOATWIN_POPUPMODE_DOWN | FLOATWIN_POPUPMODE_NOFOCUSCLOSE);
	GetTrGrFloatWin()->SetPopupModeFlags(GetTrGrFloatWin()->GetPopupModeFlags() | FLOATWIN_POPUPMODE_NOAPPFOCUSCLOSE );

	pTrGrPage->ToGetFocus();

	return (0L);
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK(AreaPropertyPanel, ChangeTrgrTypeHdl_Impl, void *, EMPTYARG)
{
	sal_uInt16 nSelectType = mpLBTransType->GetSelectEntryPos();
	bool bGradient = false;
	sal_uInt16 nTrans = 0;

	if (nSelectType == 0)
	{
		mpBTNGradient->Hide();
		mpMTRTransparent->Show();
		mpMTRTransparent->Enable();
		mpMTRTransparent->SetValue(0);
	}
	else if (nSelectType == 1)
	{
		mpBTNGradient->Hide();
		mpMTRTransparent->Show();
		nTrans = mnLastTransSolid;
		mpMTRTransparent->SetValue(nTrans);
		mpLBTransType->SelectEntryPos(1);// for multi-selected, choose solid no selection
		mpMTRTransparent->Enable();
	}
	else
	{
		mpBTNGradient->Show();
		//for beta1
		switch ( nSelectType )
		{
		case 2:
			mpBTNGradient->SetItemImage(TBI_BTX_GRADIENT,GetSettings().GetStyleSettings().GetHighContrastMode()? maImgLinearH : maImgLinear); // high contrast
			break;
		case 3:
			mpBTNGradient->SetItemImage(TBI_BTX_GRADIENT,GetSettings().GetStyleSettings().GetHighContrastMode()? maImgAxialH : maImgAxial);
			break;
		case 4:
			mpBTNGradient->SetItemImage(TBI_BTX_GRADIENT,GetSettings().GetStyleSettings().GetHighContrastMode()? maImgRadialH : maImgRadial);
			break;
		case 5:
			mpBTNGradient->SetItemImage(TBI_BTX_GRADIENT,GetSettings().GetStyleSettings().GetHighContrastMode()? maImgElliH : maImgElli );
			break;
		case 6:
			mpBTNGradient->SetItemImage(TBI_BTX_GRADIENT,GetSettings().GetStyleSettings().GetHighContrastMode()? maImgQuadH : maImgQuad );
			break;
		case 7:
			mpBTNGradient->SetItemImage(TBI_BTX_GRADIENT,GetSettings().GetStyleSettings().GetHighContrastMode()? maImgSquareH : maImgSquare);
			break;
		}
		//end of new code
		mpMTRTransparent->Hide();
		mpBTNGradient->Enable();
		bGradient = true;
	}

	XFillTransparenceItem aLinearItem(nTrans);
	GetBindings()->GetDispatcher()->Execute( SID_SVX_AREA_TRANSPARENCY, SFX_CALLMODE_RECORD, &aLinearItem, 0L );

	if (nSelectType > 1) nSelectType = nSelectType-2;

	XGradient aTmpGradient;
		
	switch(nSelectType)
	{
	case XGRAD_LINEAR:
		aTmpGradient = maGradientLinear;
		break;
	case XGRAD_AXIAL:
		aTmpGradient = maGradientAxial;
		break;
	case XGRAD_RADIAL:
		aTmpGradient = maGradientRadial;
		break;
	case XGRAD_ELLIPTICAL:
		aTmpGradient = maGradientElliptical;
		break;
	case XGRAD_SQUARE:
		aTmpGradient = maGradientSquare;
		break;
	case XGRAD_RECT:
		aTmpGradient = maGradientRect;
		break;
	}
	SfxItemPool* pPool = NULL;
	bool bEnable = false;
	if (bGradient) bEnable = true;		
	XFillFloatTransparenceItem aGradientItem(pPool,aTmpGradient, bEnable );

	GetBindings()->GetDispatcher()->Execute( SID_SVX_AREA_TRANSP_GRADIENT, SFX_CALLMODE_RECORD, &aGradientItem, 0L );

	return( 0L );
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK(AreaPropertyPanel, ModifyTransparentHdl_Impl, void*, EMPTYARG)
{
	sal_uInt16 nTrans = (sal_uInt16)mpMTRTransparent->GetValue();
	mnLastTransSolid = nTrans;
	sal_uInt16 nSelectType = mpLBTransType->GetSelectEntryPos();
	if (nTrans != 0 && nSelectType == 0)
		mpLBTransType->SelectEntryPos(1);
	XFillTransparenceItem aLinearItem(nTrans);
	GetBindings()->GetDispatcher()->Execute( SID_SVX_AREA_TRANSPARENCY, SFX_CALLMODE_RECORD, &aLinearItem, 0L );

	return 0L;
}

//////////////////////////////////////////////////////////////////////////////
// namespace close

}} // end of namespace ::svx::sidebar

//////////////////////////////////////////////////////////////////////////////
// eof
