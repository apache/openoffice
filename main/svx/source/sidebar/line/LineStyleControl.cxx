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

#include "LineStyleControl.hxx"

#include <svx/dialogs.hrc>
#include <svx/dialmgr.hxx>
#include <LinePropertyPanel.hxx>
#include <LinePropertyPanel.hrc>
#include <svx/xlnstit.hxx>
#include <svx/xlineit0.hxx>
#include <svx/xlndsit.hxx>
#include <svx/xenum.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include "svx/sidebar/PopupContainer.hxx"


namespace svx { namespace sidebar {

LineStyleControl::LineStyleControl(Window* pParent, svx::sidebar::LinePropertyPanel& rPanel)
    : PopupControl(pParent,SVX_RES(RID_POPUPPANEL_LINEPAGE_STYLE)),
      mrLinePropertyPanel(rPanel),
      mpBindings(NULL),
      maVSStyle( this, SVX_RES(VS_STYLE)),
      maPBOptions	(this, SVX_RES(PB_OPTIONS)),
      mbVS(true)
{
	Initialize();
	FreeResource();
	mpBindings = mrLinePropertyPanel.GetBindings();
}




LineStyleControl::~LineStyleControl()
{
	delete[] maIMG;
	delete[] maIMGSel;
	delete[] maIMGH;
}




void LineStyleControl::Paint (const Rectangle& rect)
{
	svx::sidebar::PopupControl::Paint(rect);

	Color aOldLineColor = GetLineColor();
	Color aOldFillColor = GetFillColor();

	Point aPos = maPBOptions.GetPosPixel();	
	Size aSize = maPBOptions.GetSizePixel();
	Rectangle aRect( aPos, aSize );
	aRect.Left() -= 1;
	aRect.Top() -= 1;
	aRect.Right() += 1;
	aRect.Bottom() += 1;

	Color aLineColor(159,173,199);
	if(!GetSettings().GetStyleSettings().GetHighContrastMode())
		SetLineColor(aLineColor);
	else
		SetLineColor(GetSettings().GetStyleSettings().GetShadowColor());
	SetFillColor(COL_TRANSPARENT);
	DrawRect(aRect);

	SetLineColor(aOldLineColor);
	SetFillColor(aOldFillColor);
}




void LineStyleControl::Initialize()
{
	maIMG = new Image[11];
	maIMG[0] = Image(SVX_RES(IMG_LINE1));
	maIMG[1] = Image(SVX_RES(IMG_LINE2));
	maIMG[2] = Image(SVX_RES(IMG_LINE3));
	maIMG[3] = Image(SVX_RES(IMG_LINE4));
	maIMG[4] = Image(SVX_RES(IMG_LINE5));
	maIMG[5] = Image(SVX_RES(IMG_LINE6));
	maIMG[6] = Image(SVX_RES(IMG_LINE7));
	maIMG[7] = Image(SVX_RES(IMG_LINE8));
	maIMG[8] = Image(SVX_RES(IMG_LINE9));
	maIMG[9] = Image(SVX_RES(IMG_LINE10));
	maIMG[10] = Image(SVX_RES(IMG_LINE11));
	
	maIMGSel = new Image[11];
	maIMGSel[0] = Image(SVX_RES(IMG_LINE1_S));
	maIMGSel[1] = Image(SVX_RES(IMG_LINE2_S));
	maIMGSel[2] = Image(SVX_RES(IMG_LINE3_S));
	maIMGSel[3] = Image(SVX_RES(IMG_LINE4_S));
	maIMGSel[4] = Image(SVX_RES(IMG_LINE5_S));
	maIMGSel[5] = Image(SVX_RES(IMG_LINE6_S));
	maIMGSel[6] = Image(SVX_RES(IMG_LINE7_S));
	maIMGSel[7] = Image(SVX_RES(IMG_LINE8_S));
	maIMGSel[8] = Image(SVX_RES(IMG_LINE9_S));
	maIMGSel[9] = Image(SVX_RES(IMG_LINE10_S));
	maIMGSel[10] = Image(SVX_RES(IMG_LINE11_S));

	//for high contrast
	maIMGH = new Image[11];
	maIMGH[0] = Image(SVX_RES(IMG_LINE1_H));
	maIMGH[1] = Image(SVX_RES(IMG_LINE2_H));
	maIMGH[2] = Image(SVX_RES(IMG_LINE3_H));
	maIMGH[3] = Image(SVX_RES(IMG_LINE4_H));
	maIMGH[4] = Image(SVX_RES(IMG_LINE5_H));
	maIMGH[5] = Image(SVX_RES(IMG_LINE6_H));
	maIMGH[6] = Image(SVX_RES(IMG_LINE7_H));
	maIMGH[7] = Image(SVX_RES(IMG_LINE8_H));
	maIMGH[8] = Image(SVX_RES(IMG_LINE9_H));
	maIMGH[9] = Image(SVX_RES(IMG_LINE10_H));
	maIMGH[10] = Image(SVX_RES(IMG_LINE11_H));

// SymCustomButton::HoverButton not available in AOO, use PushButton instead
//	//high contrast
//	if(!GetSettings().GetStyleSettings().GetHighContrastMode())
//	{
//		maPBOptions.SetDefBkColor(GetSettings().GetStyleSettings().GetPopupPanelBackgroundColor());//Color(244,245,249)
//		maPBOptions.SetHoverBkColor( GetSettings().GetStyleSettings().GetCustButtonHoverColor() );//Color( 93, 120, 163 )
//		maPBOptions.SetHoverTxtColor( GetSettings().GetStyleSettings().GetCustButtonTextHoverColor() );//Color( 255, 255, 255 )
//	}
//	else
//	{
//		maPBOptions.SetDefBkColor(GetSettings().GetStyleSettings().GetMenuColor());//Color(244,245,249)
//		maPBOptions.SetHoverBkColor( GetSettings().GetStyleSettings().GetMenuColor() );//Color( 93, 120, 163 )
//		maPBOptions.SetHoverTxtColor( GetSettings().GetStyleSettings().GetCustButtonTextHoverColor() );//Color( 255, 255, 255 )
//	}
//
//	maPBOptions.SetIcoPosX( 2 ); //POPUPPANEL_MARGIN_LARGE  * 2

	Link aLink = LINK( this, LineStyleControl, PBStyleClickHdl ) ;
	maPBOptions.SetClickHdl(aLink);

	maVSStyle.SetStyle( maVSStyle.GetStyle()| WB_3DLOOK |  WB_NO_DIRECTSELECT  );// WB_NAMEFIELD | WB_ITEMBORDER |WB_DOUBLEBORDER | WB_NONEFIELD |
	//maVSStyle.SetControlBackground(COL_WHITE);
	maVSStyle.SetColor(GetSettings().GetStyleSettings().GetMenuColor());

	for(sal_uInt16 i = 1 ; i <= 12 ; i++)
		maVSStyle.InsertItem(i);

//	maVSStyle.SetItemHelpText(1, String( SVX_RES( STR_STYLE_NONE_TIP ) )); // not supported on AOO
	maVSStyle.SetItemText(1, String( SVX_RES( STR_STYLE_NONE  ) ));
	maVSStyle.SetItemText(2, String( SVX_RES( STR_STYLE_LINE1 ) ));
	maVSStyle.SetItemText(3, String( SVX_RES( STR_STYLE_LINE2 ) ));
	maVSStyle.SetItemText(4, String( SVX_RES( STR_STYLE_LINE3 ) ));
	maVSStyle.SetItemText(5, String( SVX_RES( STR_STYLE_LINE4 ) ));
	maVSStyle.SetItemText(6, String( SVX_RES( STR_STYLE_LINE5 ) ));
	maVSStyle.SetItemText(7, String( SVX_RES( STR_STYLE_LINE6 ) ));
	maVSStyle.SetItemText(8, String( SVX_RES( STR_STYLE_LINE7 ) ));
	maVSStyle.SetItemText(9, String( SVX_RES( STR_STYLE_LINE8 ) ));
	maVSStyle.SetItemText(10, String( SVX_RES( STR_STYLE_LINE9 ) ));
	maVSStyle.SetItemText(11, String( SVX_RES( STR_STYLE_LINE10 ) ));
	maVSStyle.SetItemText(12, String( SVX_RES( STR_STYLE_LINE11 ) ));
	SetAllNoSel();
	maVSStyle.SetFirstString(String( SVX_RES( STR_STYLE_NONE) ));
	
	aLink =  LINK( this, LineStyleControl, VSSelectHdl ) ;
    maVSStyle.SetSelectHdl(aLink);

	maVSStyle.StartSelection();
	maVSStyle.Show();
}




void LineStyleControl::GetFocus()
{
	if(mbVS)
		maVSStyle.GrabFocus();
	else
		maPBOptions.GrabFocus();
}




Control& LineStyleControl::GetPB()
{
	return maPBOptions;
}




ValueSet& LineStyleControl::GetValueSet()
{
	return maVSStyle;
}




void LineStyleControl::SetStyleSelect(XLineStyleItem* pStyle, XLineDashItem* pDash, bool bAvailable, bool bDashAvailable)
{
	mbVS = false;
	SetAllNoSel();
	if( !bAvailable)
	{
		maVSStyle.Format();
		maVSStyle.StartSelection();
		return;
	}

    const XLineStyle eXLS(pStyle ? (XLineStyle)pStyle->GetValue() : XLINE_NONE);

	switch(eXLS)
	{
	case XLINE_NONE:
		maVSStyle.SetFirstSelect(true);
		maVSStyle.SelectItem(1); //add 
		mbVS = true;
		break;
	case XLINE_SOLID:
		maVSStyle.SetItemImage(2, maIMGSel[0]);
		maVSStyle.SelectItem(2);		//add 
		mbVS = true;
		break;
	case XLINE_DASH:
		if(pDash && bDashAvailable)
		{
			String aDashName = pDash->GetName() ;
			for( sal_uInt16 n = 0; n < 10; n++ )
			{
				if( Dash_Name[n] == aDashName )
				{
					maVSStyle.SetItemImage( n + 3, maIMGSel[n+1]);
					maVSStyle.SelectItem( n + 3 );		//add 
					mbVS = true;
					break;
				}
			}
		}
		break;
	}
	maVSStyle.Format();
	maVSStyle.StartSelection();
//	GetFocus(mbVS);
}




void LineStyleControl::SetAllNoSel()
{
	maVSStyle.SelectItem(1); // ,false);		//modified // 'false' nut supported by AOO
	maVSStyle.SetNoSelection();
	maVSStyle.SetFirstSelect(false);
	for(sal_uInt16 i = 2; i <= 12; i++)
		maVSStyle.SetItemImage(i, GetDisplayBackground().GetColor().IsDark() ? maIMGH[i-2] : maIMG[i-2]);
}




IMPL_LINK(LineStyleControl, VSSelectHdl, void *, pControl)
{
	if(pControl == &maVSStyle)  
	{
		XLineStyleItem* pStyleItem = NULL;
		sal_uInt16 iPos = maVSStyle.GetSelectItemId();
		if(iPos == 1)
		{
			pStyleItem = new XLineStyleItem( XLINE_NONE);
		}
		else if(iPos == 2)
		{
			pStyleItem = new XLineStyleItem( XLINE_SOLID);
		}
		else
		{
			pStyleItem = new XLineStyleItem( XLINE_DASH );
			XLineDashItem aDashItem( Dash_Name[iPos - 3], Dash_Set[iPos - 3]);
			mpBindings->GetDispatcher()->Execute(SID_ATTR_LINE_DASH, SFX_CALLMODE_RECORD, &aDashItem, 0L);
			mrLinePropertyPanel.SetLineDashItem(&aDashItem);
		}
		mpBindings->GetDispatcher()->Execute(SID_ATTR_LINE_STYLE, SFX_CALLMODE_RECORD, pStyleItem, 0L);
		mrLinePropertyPanel.SetLineStyleItem(pStyleItem);
		delete pStyleItem;

		mrLinePropertyPanel.EndLineStylePopupMode();
	}
	return( 0L );
}




IMPL_LINK(LineStyleControl, PBStyleClickHdl, PushButton *, pPBtn)
{
	if(pPBtn == &maPBOptions)  
	{
		if (mpBindings)
		{
			SfxDispatcher* pDisp = mpBindings->GetDispatcher();
			pDisp->Execute( SID_ATTRIBUTES_LINE, SFX_CALLMODE_ASYNCHRON );
		}
		mrLinePropertyPanel.EndLineStylePopupMode();
	}
	return 0;
}


} } // end of namespace svx::sidebar

// eof
