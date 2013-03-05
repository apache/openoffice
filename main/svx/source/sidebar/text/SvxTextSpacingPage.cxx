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

namespace sidebar {

class SvxTextPropertyPage::SvxTextSpacingPage:public SfxPopupPage
{
public:
	SvxTextSpacingPage(Window* pParent, SvxTextPropertyPage* pPage);
	~SvxTextSpacingPage();
	void GetFocus();
	void SetControlState(bool bLBAvailable,bool bAvailable, long nKerning);
	virtual void Paint(const Rectangle& rect);
	//add 
	short GetLastCustomState();
	long  GetLastCustomValue();
	//add end

private:	
	SvxTextPropertyPage* mpPage;
	SfxBindings*		mpBindings;

	SvxValueSetCustom	maVSSpacing;

	FixedText			maLastCus;
//	Control				maBorder;

	FixedText			maFTSpacing;
	ListBox				maLBKerning;
	FixedText			maFTBy;
	MetricField			maEditKerning;

	Image*				mpImg;
	Image*				mpImgSel;
	XubString*			mpStr;
	XubString*			mpStrTip;

	Image				maImgCus;
	Image				maImgCusGrey;
	XubString			maStrCus;
	XubString			maStrCusE;		//add 
	XubString			maStrCusC;		//add 
	XubString			maStrCusN;		//add 
	XubString			maStrUnit;		//add 

	long				mnCustomKern;
	short				mnLastCus;		
	bool				mbCusEnable;
	bool				mbVS;

	void initial();
	DECL_LINK(VSSelHdl, void*);
	DECL_LINK(KerningSelectHdl, ListBox*);
	DECL_LINK(KerningModifyHdl,  MetricField*);
};
SvxTextPropertyPage::SvxTextSpacingPage::SvxTextSpacingPage(Window* pParent, SvxTextPropertyPage* pPage)
:	SfxPopupPage( pParent,SVX_RES(RID_POPUPPANEL_TEXTPAGE_SPACING))
,	mpPage(pPage)
,	mpBindings(NULL)
,	maVSSpacing		(this, SVX_RES(VS_SPACING))
,	maLastCus		(this, SVX_RES(FT_LASTCUSTOM))
//,	maBorder		(this, SVX_RES(CT_BORDER))
,	maFTSpacing		(this, SVX_RES(FT_SPACING))
,	maLBKerning		(this, SVX_RES(LB_KERNING))
,	maFTBy			(this, SVX_RES(FT_BY))
,	maEditKerning	(this, SVX_RES(ED_KERNING))

,	mpImg			(NULL)
,	mpImgSel		(NULL)
,	mpStr			(NULL)
,	mpStrTip		(NULL)
		
,	maImgCus		(SVX_RES(IMG_CUSTOM))
,	maImgCusGrey	(SVX_RES(IMG_CUSTOM_GRAY))
,	maStrCus		(SVX_RES(STR_CUSTOM))
,	maStrCusE		(SVX_RES(STR_CUSTOM_E_TIP))	//add 
,	maStrCusC		(SVX_RES(STR_CUSTOM_C_TIP))	//add 
,	maStrCusN		(SVX_RES(STR_NORMAL_TIP))	//add 
,	maStrUnit		(SVX_RES(STR_PT))			//add 

,	mnCustomKern(0)
,	mnLastCus ( SPACING_NOCUSTOM )
,	mbCusEnable(false)
,	mbVS(true)
{
	initial();
	FreeResource();
	if (mpPage)
		mpBindings = mpPage->GetBindings();
	Link aLink = LINK(this, SvxTextSpacingPage, KerningSelectHdl);
	maLBKerning.SetSelectHdl(aLink);
	aLink =LINK(this, SvxTextSpacingPage, KerningModifyHdl);
	maEditKerning.SetModifyHdl(aLink);

}
SvxTextPropertyPage::SvxTextSpacingPage::~SvxTextSpacingPage()
{
	delete[] mpImg;
	delete[] mpImgSel;
	delete[] mpStr;
	delete[] mpStrTip;
}
void SvxTextPropertyPage::SvxTextSpacingPage::Paint(const Rectangle& rect)
{
	SfxPopupPage::Paint(rect);
	Color aOldLineColor = GetLineColor();
	Color aOldFillColor = GetFillColor();

	//Point aPos = maBorder.GetPosPixel();	
	//Size aSize = maBorder.GetSizePixel();
	Point aPos( LogicToPixel( Point( CUSTOM_X, CUSTOM_Y), MAP_APPFONT ));
	Size aSize( LogicToPixel( Size(  CUSTOM_WIDTH, CUSTOM_HEIGHT ), MAP_APPFONT ));
	Rectangle aRect( aPos, aSize );
	aRect.Left() -= 1;
	aRect.Top() -= 1;
	aRect.Right() += 1;
	aRect.Bottom() += 1;


	Color aLineColor(189,201,219);
	if(!GetSettings().GetStyleSettings().GetHighContrastMode())
		SetLineColor(aLineColor);
	else
		SetLineColor(GetSettings().GetStyleSettings().GetShadowColor());
	SetFillColor(COL_TRANSPARENT);
	//add  for high contract
	//if(GetSettings().GetStyleSettings().GetHighContrastMode())
	//{
	//	maBorder.SetBackground(GetSettings().GetStyleSettings().GetMenuColor());
	//	maFTSpacing.SetBackground(GetSettings().GetStyleSettings().GetMenuColor());
	//	maFTBy.SetBackground(GetSettings().GetStyleSettings().GetMenuColor());
	//}
	DrawRect(aRect);


	SetLineColor(aOldLineColor);
	SetFillColor(aOldFillColor);
}
void SvxTextPropertyPage::SvxTextSpacingPage::initial()
{
	maVSSpacing.SetStyle( maVSSpacing.GetStyle()| WB_3DLOOK |  WB_NO_DIRECTSELECT  );
	//for high contract
	if(GetSettings().GetStyleSettings().GetHighContrastMode())
	{
		maVSSpacing.SetControlBackground(GetSettings().GetStyleSettings().GetMenuColor());
		maVSSpacing.SetColor(GetSettings().GetStyleSettings().GetMenuColor());
		maVSSpacing.SetBackground(GetSettings().GetStyleSettings().GetMenuColor());
		maFTSpacing.SetBackground(GetSettings().GetStyleSettings().GetMenuColor());
		maFTBy.SetBackground(GetSettings().GetStyleSettings().GetMenuColor());
	}
	mpImg = new Image[5];
	mpImg[0] = Image(SVX_RES(IMG_VERY_TIGHT));
	mpImg[1] = Image(SVX_RES(IMG_TIGHT));
	mpImg[2] = Image(SVX_RES(IMG_NORMAL));
	mpImg[3] = Image(SVX_RES(IMG_LOOSE));
	mpImg[4] = Image(SVX_RES(IMG_VERY_LOOSE));

	mpImgSel = new Image[5];
	mpImgSel[0] = Image(SVX_RES(IMG_VERY_TIGHT_S));
	mpImgSel[1] = Image(SVX_RES(IMG_TIGHT_S));
	mpImgSel[2] = Image(SVX_RES(IMG_NORMAL_S));
	mpImgSel[3] = Image(SVX_RES(IMG_LOOSE_S));
	mpImgSel[4] = Image(SVX_RES(IMG_VERY_LOOSE_S));

	mpStr = new XubString[5];
	mpStr[0] = XubString(SVX_RES(STR_VERY_TIGHT));
	mpStr[1] = XubString(SVX_RES(STR_TIGHT));
	mpStr[2] = XubString(SVX_RES(STR_NORMAL));
	mpStr[3] = XubString(SVX_RES(STR_LOOSE));
	mpStr[4] = XubString(SVX_RES(STR_VERY_LOOSE));
	maVSSpacing.InsertItemSet(5, mpImg, mpImgSel,mpStr);

	mpStrTip = new XubString[5];
	mpStrTip[0] = XubString(SVX_RES(STR_VERY_TIGHT_TIP));
	mpStrTip[1] = XubString(SVX_RES(STR_TIGHT_TIP));
	mpStrTip[2] = XubString(SVX_RES(STR_NORMAL_TIP));
	mpStrTip[3] = XubString(SVX_RES(STR_LOOSE_TIP));
	mpStrTip[4] = XubString(SVX_RES(STR_VERY_LOOSE_TIP));
	maVSSpacing.SetDefaultTip(mpStr);	//modify
	maVSSpacing.SetDefaultTip(mpStrTip, TRUE); //Add

	maVSSpacing.InsertCustom(maImgCus, maImgCusGrey, maStrCus);
	maVSSpacing.SetCustomTip(maStrCus); //Add

	maVSSpacing.SetSelItem(0);			
	Link aLink = LINK(this, SvxTextSpacingPage,VSSelHdl );
	maVSSpacing.SetSelectHdl(aLink);
	maVSSpacing.StartSelection();
	maVSSpacing.Show();
}
void SvxTextPropertyPage::SvxTextSpacingPage::GetFocus()
{
	if(!mbVS)
		maLBKerning.GrabFocus();
	else
		maVSSpacing.GrabFocus();
}
void SvxTextPropertyPage::SvxTextSpacingPage::SetControlState(bool bLBAvailable,bool bAvailable, long nKerning)
{
	mbVS = true;
	maVSSpacing.SetSelItem(0);
	SvtViewOptions aWinOpt( E_WINDOW, SIDEBAR_SPACING_GLOBAL_VALUE );
    if ( aWinOpt.Exists() )
	{
		::com::sun::star::uno::Sequence < ::com::sun::star::beans::NamedValue > aSeq = aWinOpt.GetUserData();
		::rtl::OUString aTmp;
		if ( aSeq.getLength())
			aSeq[0].Value >>= aTmp;

		String aWinData( aTmp );
		mnCustomKern = aWinData.ToInt32();
		mnLastCus = SPACING_CLOSE_BY_CUS_EDIT;
		mbCusEnable = true;
	}
	else
	{
		mnLastCus = SPACING_NOCUSTOM;
		mbCusEnable = false;
	}

	if( !mnLastCus ) 
	{
		maVSSpacing.SetCusEnable(false);
		maVSSpacing.SetCustomTip(maStrCus,TRUE);  //LAST CUSTOM no tip defect //add 
	}
	else
	{
		//set custom tips
		maVSSpacing.SetCusEnable(true);
		if(mnCustomKern > 0)
		{	
			String aStrTip( maStrCusE);   //LAST CUSTOM no tip defect //add 
			aStrTip.Append( String::CreateFromDouble( (double)mnCustomKern / 10));
			//aStrTip.Append(String("pt", 2, RTL_TEXTENCODING_ASCII_US));
			aStrTip.Append(maStrUnit);		// modify 
			maVSSpacing.SetCustomTip(aStrTip,TRUE);
		}
		else if(mnCustomKern < 0)
		{	
			String aStrTip(maStrCusC) ;		//LAST CUSTOM no tip defect //add 
			aStrTip.Append( String::CreateFromDouble( (double)-mnCustomKern / 10));
			//aStrTip.Append(String("pt", 2, RTL_TEXTENCODING_ASCII_US));
			aStrTip.Append(maStrUnit);		// modify 
			maVSSpacing.SetCustomTip(aStrTip,TRUE);
		}	
		else
		{	
			String aStrTip(maStrCusN) ;		//LAST CUSTOM no tip defect //add 
			maVSSpacing.SetCustomTip(aStrTip,TRUE);
		}
		
	}

	if(bLBAvailable && bAvailable)
	{
		maLBKerning.Enable();
		maFTSpacing.Enable();

		if(mpPage == NULL)
			return;
		SfxMapUnit eUnit = mpPage->GetSpaceController().GetCoreMetric();
        MapUnit eOrgUnit = (MapUnit)eUnit;
        MapUnit ePntUnit( MAP_POINT );			
        long nBig = maEditKerning.Normalize(nKerning);
        nKerning = LogicToLogic( nBig, eOrgUnit, ePntUnit );

		if ( nKerning > 0 )
        {
			maFTBy.Enable();
            maEditKerning.Enable();
            maEditKerning.SetMax( 9999 );
            maEditKerning.SetLast( 9999 );
            maEditKerning.SetValue( nKerning );
            maLBKerning.SelectEntryPos( SIDEBAR_SPACE_EXPAND );
			if(nKerning == 30)
			{
				maVSSpacing.SetSelItem(4);
			}
			else if(nKerning == 60)
			{
				maVSSpacing.SetSelItem(5);
			}
			else
			{
				maVSSpacing.SetSelItem(0);
				mbVS = false;
			}
        }
        else if ( nKerning < 0 )
        {
			maFTBy.Enable();
            maEditKerning.Enable();
            maEditKerning.SetValue( -nKerning );
            maLBKerning.SelectEntryPos( SIDEBAR_SPACE_CONDENSED );
            long nMax = mpPage->GetSelFontSize()/6;
            maEditKerning.SetMax( maEditKerning.Normalize( nMax ), FUNIT_POINT );
            maEditKerning.SetLast( maEditKerning.GetMax( maEditKerning.GetUnit() ) );
			if( nKerning == -30 )
			{
				maVSSpacing.SetSelItem(1);
			}
			else if( nKerning == -15 )
			{
				maVSSpacing.SetSelItem(2);
			}
			else
			{
				maVSSpacing.SetSelItem(0);
				mbVS = false;
			}
        }
        else
        {
			maVSSpacing.SetSelItem(3);
			maLBKerning.SelectEntryPos( SIDEBAR_SPACE_NORMAL );
			maFTBy.Disable();
            maEditKerning.Disable();
            maEditKerning.SetValue( 0 );
            maEditKerning.SetMax( 9999 );
            maEditKerning.SetLast( 9999 );
        }
	}
	else if(bLBAvailable && !bAvailable)
	{	
		//modified 
		maVSSpacing.SetSelItem(0);
		mbVS = false;
		maLBKerning.Enable();
		maFTSpacing.Enable();
		maLBKerning.SetNoSelection();
		maEditKerning.SetText(String());
		maEditKerning.Disable();
		maFTBy.Disable();
	}
	else
	{
		maVSSpacing.SetSelItem(0);
		mbVS = false;
		maEditKerning.SetText(String());
        maLBKerning.SetNoSelection();
		maLBKerning.Disable();
		maFTSpacing.Disable();
        maEditKerning.Disable();
		maFTBy.Disable();
	}
	GetFocus();
	maVSSpacing.Format();
	maVSSpacing.StartSelection();
}
IMPL_LINK(SvxTextPropertyPage::SvxTextSpacingPage, VSSelHdl, void *, pControl)
{
//	mnLastCus = SPACING_CLOSE_BY_CLICK_ICON;

	if(pControl == &maVSSpacing)  
	{		
		USHORT iPos = maVSSpacing.GetSelectItemId();
		short nKern = 0;
		SfxMapUnit eUnit = mpPage->GetSpaceController().GetCoreMetric();
		long nVal = 0;
		if(iPos == 1)
		{
			nVal = LogicToLogic(30, MAP_POINT, (MapUnit)eUnit);
			nKern = (short)maEditKerning.Denormalize(nVal);
			SvxKerningItem aKernItem(-nKern, SID_ATTR_CHAR_KERNING);
			mpBindings->GetDispatcher()->Execute(SID_ATTR_CHAR_KERNING, SFX_CALLMODE_RECORD, &aKernItem, 0L);
			mpPage->SetSpacing(-nKern);
			mnLastCus = SPACING_CLOSE_BY_CLICK_ICON;
		}
		else if(iPos == 2)
		{
			nVal = LogicToLogic(15, MAP_POINT, (MapUnit)eUnit);
			nKern = (short)maEditKerning.Denormalize(nVal);
			SvxKerningItem aKernItem(-nKern, SID_ATTR_CHAR_KERNING);
			mpBindings->GetDispatcher()->Execute(SID_ATTR_CHAR_KERNING, SFX_CALLMODE_RECORD, &aKernItem, 0L);
			mpPage->SetSpacing(-nKern);
			mnLastCus = SPACING_CLOSE_BY_CLICK_ICON;
		}
		else if(iPos == 3)
		{
			SvxKerningItem aKernItem(0, SID_ATTR_CHAR_KERNING);
			mpBindings->GetDispatcher()->Execute(SID_ATTR_CHAR_KERNING, SFX_CALLMODE_RECORD, &aKernItem, 0L);
			mpPage->SetSpacing(0);
			mnLastCus = SPACING_CLOSE_BY_CLICK_ICON;
		}
		else if(iPos == 4)
		{
			nVal = LogicToLogic(30, MAP_POINT, (MapUnit)eUnit);
			nKern = (short)maEditKerning.Denormalize(nVal);
			SvxKerningItem aKernItem(nKern, SID_ATTR_CHAR_KERNING);
			mpBindings->GetDispatcher()->Execute(SID_ATTR_CHAR_KERNING, SFX_CALLMODE_RECORD, &aKernItem, 0L);
			mpPage->SetSpacing(nKern);
			mnLastCus = SPACING_CLOSE_BY_CLICK_ICON;
		}
		else if(iPos == 5)
		{
			nVal = LogicToLogic(60, MAP_POINT, (MapUnit)eUnit);
			nKern = (short)maEditKerning.Denormalize(nVal);
			SvxKerningItem aKernItem(nKern, SID_ATTR_CHAR_KERNING);
			mpBindings->GetDispatcher()->Execute(SID_ATTR_CHAR_KERNING, SFX_CALLMODE_RECORD, &aKernItem, 0L);
			mpPage->SetSpacing(nKern);
			mnLastCus = SPACING_CLOSE_BY_CLICK_ICON;
		}
		else if(iPos == 6)
		{
			//modified 
			if(mbCusEnable)
			{
				nVal = LogicToLogic(mnCustomKern, MAP_POINT, (MapUnit)eUnit);
				nKern = (short)maEditKerning.Denormalize(nVal);
				SvxKerningItem aKernItem(nKern , SID_ATTR_CHAR_KERNING);
				mpBindings->GetDispatcher()->Execute(SID_ATTR_CHAR_KERNING, SFX_CALLMODE_RECORD, &aKernItem, 0L);
				mpPage->SetSpacing(nKern);
				mnLastCus = SPACING_CLOSE_BY_CLICK_ICON;
			}
			else
			{
				maVSSpacing.SetNoSelection();		//add , set no selection and keep the last select item 
				maVSSpacing.Format();
				Invalidate();
				maVSSpacing.StartSelection();
			}
			//modify end
		}
		
		if(iPos < 6 || (iPos == 6 && mbCusEnable)) //add 
			if (mpPage->GetSpacingFloatWin()->IsInPopupMode() )
				mpPage->GetSpacingFloatWin()->EndPopupMode();
	}


		
	return 0;
}

IMPL_LINK(SvxTextPropertyPage::SvxTextSpacingPage, KerningSelectHdl, ListBox*, EMPTYARG)
{
	if ( maLBKerning.GetSelectEntryPos() > 0 )
	{
		maFTBy.Enable();
		maEditKerning.Enable();
	}
	else
	{
		maEditKerning.SetValue( 0 );
		maFTBy.Disable();
		maEditKerning.Disable();
	}

	if(maVSSpacing.GetSelItem())
	{
		maVSSpacing.SetSelItem(0);	//modify 
		maVSSpacing.Format();
		Invalidate();
		maVSSpacing.StartSelection();
	}
	KerningModifyHdl( NULL );
	return 0;
}
IMPL_LINK(SvxTextPropertyPage::SvxTextSpacingPage, KerningModifyHdl, MetricField*, EMPTYARG)
{
	if(maVSSpacing.GetSelItem())
	{
		maVSSpacing.SetSelItem(0);	//modify 
		maVSSpacing.Format();
		Invalidate();
		maVSSpacing.StartSelection();
	}
	USHORT nPos = maLBKerning.GetSelectEntryPos();
    short nKern = 0;
    SfxMapUnit eUnit = mpPage->GetSpaceController().GetCoreMetric();
	mnLastCus = SPACING_CLOSE_BY_CUS_EDIT; 
	if ( nPos == SIDEBAR_SPACE_EXPAND || nPos == SIDEBAR_SPACE_CONDENSED )
    {
        long nTmp = static_cast<long>(maEditKerning.GetValue());
        if ( nPos == SIDEBAR_SPACE_CONDENSED )
		{
			long nMax =  mpPage->GetSelFontSize()/6;
			maEditKerning.SetMax( maEditKerning.Normalize( nMax ), FUNIT_TWIP );
			maEditKerning.SetLast( maEditKerning.GetMax( maEditKerning.GetUnit() ) );
			if(nTmp > maEditKerning.GetMax())
				nTmp = maEditKerning.GetMax();
			mnCustomKern = -nTmp;
			long nVal = LogicToLogic( nTmp, MAP_POINT, (MapUnit)eUnit );
			nKern = (short)maEditKerning.Denormalize( nVal );
            nKern *= - 1;		
		}
		else
		{
			maEditKerning.SetMax( 9999 );
			maEditKerning.SetLast( 9999 );
			if(nTmp > maEditKerning.GetMax(FUNIT_TWIP))
				nTmp = maEditKerning.GetMax(FUNIT_TWIP);
			mnCustomKern = nTmp;
			long nVal = LogicToLogic( nTmp, MAP_POINT, (MapUnit)eUnit );
			nKern = (short)maEditKerning.Denormalize( nVal );	
		}
    }
	else
	{
		mnCustomKern = 0;
	}
	SvxKerningItem aKernItem(nKern, SID_ATTR_CHAR_KERNING);
	mpBindings->GetDispatcher()->Execute(SID_ATTR_CHAR_KERNING, SFX_CALLMODE_RECORD, &aKernItem, 0L);
	mpPage->SetSpacing(nKern);
	return 0;
}
short  SvxTextPropertyPage::SvxTextSpacingPage::GetLastCustomState()
{
	return mnLastCus;
}
long  SvxTextPropertyPage::SvxTextSpacingPage::GetLastCustomValue()
{
	return mnCustomKern;
}

} // end of namespace sidebar
