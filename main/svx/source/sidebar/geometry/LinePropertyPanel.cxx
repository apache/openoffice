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

using namespace css;
using namespace cssu;
using ::sfx2::sidebar::Theme;

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))
#define SIDEBAR_LINE_WIDTH_GLOBAL_VALUE String("PopupPanal_LineWidth", 20, RTL_TEXTENCODING_ASCII_US)

//////////////////////////////////////////////////////////////////////////////

const XDash Dash_Set[] = {
	//Dash, dots, dotlen, dash, dashlen, distance	
	XDash(XDASH_RECT, 1, 51, 1, 51, 51),									//0
	XDash(XDASH_RECTRELATIVE, 1, 197, 0,0, 127),							//6
	XDash(XDASH_RECTRELATIVE, 1, 197,0,0,197),								//5
	XDash(XDASH_RECT, 1,508, 1,508, 508),									//1
	XDash(XDASH_RECT, 1,  0, 0,  0, 457),									//3
	XDash(XDASH_RECTRELATIVE, 1,  0, 0, 0, 50),								//8
	XDash(XDASH_RECTRELATIVE, 3, 197, 3, 0, 100),							//7
	XDash(XDASH_RECT, 2, 0, 1, 203,203)	,									//9
	XDash(XDASH_RECT, 2, 51, 3,254, 127),									//2
	XDash(XDASH_RECT, 1,2007,10, 0, 152)									//4
};

const String Dash_Name[] = {
	String("Ultrafine dashed", 16,  RTL_TEXTENCODING_ASCII_US),				//0
	String("Dashed (variable)", 17,RTL_TEXTENCODING_ASCII_US ),				//6
	String("Fine dashed (variable)",22,  RTL_TEXTENCODING_ASCII_US),		//5	
	String("Fine dashed", 11, RTL_TEXTENCODING_ASCII_US),					//1
	String("Fine dotted", 11, RTL_TEXTENCODING_ASCII_US),					//3	
	String("Ultrafine dotted (variable)",27, RTL_TEXTENCODING_ASCII_US),	//8
	String("3 dashes 3 dots (variable)", 26, RTL_TEXTENCODING_ASCII_US),	//7
	String("2 dots 1 dash",13,RTL_TEXTENCODING_ASCII_US  )	,				//9
	String("Ultrafine 2 dots 3 dashes",  25, RTL_TEXTENCODING_ASCII_US),	//2
	String("Line with fine dots", 19, RTL_TEXTENCODING_ASCII_US)			//4
};

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

//////////////////////////////////////////////////////////////////////////////

class SvxLineColorPage : public svx::sidebar::PropertyPanelPopupPage
{
public:
    SvxLineColorPage(Window* pParent, svx::sidebar::LinePropertyPanel& rPanel);
    virtual ~SvxLineColorPage();
	void GetFocus();
	void SetCurColorSelect(Color aCol, bool bAvl);

private:
    svx::sidebar::LinePropertyPanel&    mrLinePropertyPanel;
	SfxBindings*		mpBindings;
	ValueSet			maVSColor;

	void Initialize();
	void FillColors();
	DECL_LINK(VSSelectHdl, void *);
};

SvxLineColorPage::SvxLineColorPage(Window* pParent, svx::sidebar::LinePropertyPanel& rPanel)
:   svx::sidebar::PropertyPanelPopupPage(pParent,SVX_RES(RID_POPUPPANEL_LINEPAGE_COLOR))
,	mrLinePropertyPanel(rPanel)
,	mpBindings(NULL)
,	maVSColor( this, SVX_RES(VS_COLOR))
{
	Initialize();
	FreeResource();
	mpBindings = mrLinePropertyPanel.GetBindings();
}

SvxLineColorPage::~SvxLineColorPage()
{
}

void SvxLineColorPage::Initialize()
{
	WinBits nBits = ( maVSColor.GetStyle() | WB_NONEFIELD | WB_ITEMBORDER | WB_NAMEFIELD | WB_3DLOOK | WB_NO_DIRECTSELECT); // | WB_SELECTIFOLLOWMOUSE); //Modified  for color picker migration 01/20/10
	maVSColor.SetText( String( SVX_RES( STR_AUTOMATICE ) ) );
	maVSColor.SetStyle( nBits );

	//Modified  for color picer migration. 01/22/10
	//maVSColor.SetColCount( 10 );
	//maVSColor.SetLineCount( 10 );
	maVSColor.SetColCount( 12 );
	maVSColor.SetLineCount( 12 );
	//End
	const Size aSize15x15 = Size( 15, 15 );
	maVSColor.CalcWindowSizePixel( aSize15x15 );
	maVSColor.Show();

	Link aLink =  LINK( this, SvxLineColorPage, VSSelectHdl ) ;
    maVSColor.SetSelectHdl(aLink);
}

void SvxLineColorPage::FillColors()
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
		//Modified   for color picker migration.01/20/10
		while ( i < 144 )
		{
			maVSColor.InsertItem( i + 1, aColWhite, aStrWhite );
			i++;
		}
		if ( nCount > 144 )
		{
			maVSColor.SetStyle( maVSColor.GetStyle() | WB_VSCROLL );
		}
		//End
	}

	if ( bOwn )
		delete pColorTable;

	maVSColor.Show();
}

void SvxLineColorPage::GetFocus()
{
	maVSColor.GrabFocus();
}

void SvxLineColorPage::SetCurColorSelect(Color aCol, bool bAvailable)
{		
	FillColors();	
	short nCol = GetItemId_Impl_line( maVSColor, aCol );
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

IMPL_LINK(SvxLineColorPage, VSSelectHdl, void *, pControl)
{
	if(pControl == &maVSColor)
	{
		sal_uInt16 iPos = maVSColor.GetSelectItemId();
		Color aColor = maVSColor.GetItemColor( iPos );	  
		
		if(iPos == 0)
		{		//do enable disable
			aColor = COL_TRANSPARENT;
		}
		XLineColorItem aColorItem(maVSColor.GetItemText(iPos), aColor);
		mpBindings->GetDispatcher()->Execute(SID_ATTR_LINE_COLOR, SFX_CALLMODE_RECORD, &aColorItem, 0L);
		mrLinePropertyPanel.SetColor(aColor);

		if (mrLinePropertyPanel.GetColorFloatWin()->IsInPopupMode() )
        {
			mrLinePropertyPanel.GetColorFloatWin()->EndPopupMode();
        }
	}
	return( 0L );
}

//////////////////////////////////////////////////////////////////////////////

class SvxValueSetLineStyle : public ValueSet
{
	VirtualDevice* 	pVDev;
	bool			mbSelectFirst;
	XubString		strNone;
	public:
		SvxValueSetLineStyle( Window* pParent, const ResId& rResId);
		virtual ~SvxValueSetLineStyle();
		void SetFirstSelect(bool bSel);
		void SetFirstString(XubString str);

	virtual void	UserDraw( const UserDrawEvent& rUDEvt );	
	
};

SvxValueSetLineStyle::SvxValueSetLineStyle( Window* pParent, const ResId& rResId) :
	ValueSet( pParent, rResId )   
,	pVDev(NULL)
,	mbSelectFirst(true)
{
	SetColCount( 1 );
}

SvxValueSetLineStyle::~SvxValueSetLineStyle()
{	
	delete pVDev;
}

 void  SvxValueSetLineStyle::SetFirstString(XubString str)
{
	strNone = str;
}

void  SvxValueSetLineStyle::UserDraw( const UserDrawEvent& rUDEvt )
{
	Rectangle aRect = rUDEvt.GetRect();
	OutputDevice*  pDev = rUDEvt.GetDevice();
	sal_uInt16	nItemId = rUDEvt.GetItemId();
	
	long nRectHeight = aRect.GetHeight();
	long nRectWidth = aRect.GetWidth();	
	Point aBLPos = aRect.TopLeft();	
	if(nItemId == 1)
	{
		const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();
		Color aOldFillColor = pDev->GetFillColor();				
		const Color aTextColor = rStyleSettings.GetFieldTextColor();

		//draw back
		if( mbSelectFirst )
		{
			Color aBackColor(50,107,197);
			Rectangle aBackRect = aRect;
			aBackRect.Top() += 3;
			aBackRect.Bottom() -= 2;
			pDev->SetFillColor(aBackColor);
			pDev->DrawRect(aBackRect);
		}
		else
		{
			pDev->SetFillColor( COL_TRANSPARENT );
			pDev->DrawRect(aRect);
		}
		pDev->SetFillColor(aOldFillColor);

		//draw text
		Font aOldFont = pDev->GetFont();
		Color aOldColor = pDev->GetLineColor();
		Font aFont(OutputDevice::GetDefaultFont(
					DEFAULTFONT_UI_SANS, MsLangId::getSystemLanguage(), DEFAULTFONT_FLAGS_ONLYONE));
		Size aSize = aFont.GetSize();	
		aSize.Height() = nRectHeight*3/5;
		if( mbSelectFirst )
			aFont.SetColor(COL_WHITE);
		else
			//aFont.SetColor(COL_BLACK);
			aFont.SetColor(GetSettings().GetStyleSettings().GetFieldTextColor()); //high contrast 
		aFont.SetFillColor(COL_BLUE);
		aFont.SetSize( aSize );
		pDev->SetFont(aFont);			
//		String sText("None", 9, RTL_TEXTENCODING_ASCII_US);
		
	//	Point aStart(aBLPos.X() + 8 , aBLPos.Y() + nRectHeight/6);  
		Rectangle aStrRect = aRect;
		aStrRect.Top() += nRectHeight/6;
		aStrRect.Bottom() -= nRectHeight/6;
		aStrRect.Left() += 8;
		pDev->DrawText(aStrRect, strNone, TEXT_DRAW_ENDELLIPSIS);	
				
		pDev->SetFont(aOldFont);
		pDev->SetLineColor(aOldColor);


	}
	Invalidate( aRect );
}

void  SvxValueSetLineStyle::SetFirstSelect(bool bSel)
{
	mbSelectFirst = bSel;
}

//////////////////////////////////////////////////////////////////////////////

class SvxLineStylePage : public svx::sidebar::PropertyPanelPopupPage
{
public:
	SvxLineStylePage(Window* pParent, svx::sidebar::LinePropertyPanel& rPanel);
	virtual ~SvxLineStylePage();
	void GetFocus();
	void SetStyleSelect(XLineStyleItem* pStyle, XLineDashItem* pDash, bool bStyleAvailable, bool bDashAvailable);
	virtual void Paint(const Rectangle& rect);
	ValueSet& GetValueSet();
	Control& GetPB();

private:	
	svx::sidebar::LinePropertyPanel&    mrLinePropertyPanel;
	SfxBindings*		                mpBindings;
	SvxValueSetLineStyle	            maVSStyle;
//	SymCustomButton::HoverButton        maPBOptions; // not available in AOO, use PushButton instead
    PushButton                          maPBOptions;
	Image*	                            maIMG;
	Image*	                            maIMGSel;
	Image*	                            maIMGH;		// high contrast
	bool	                            mbVS;

	void Initialize();
	void SetAllNoSel();

	DECL_LINK( PBStyleClickHdl, PushButton *);
	DECL_LINK(VSSelectHdl, void *);
};

SvxLineStylePage::SvxLineStylePage(Window* pParent, svx::sidebar::LinePropertyPanel& rPanel)
:	svx::sidebar::PropertyPanelPopupPage(pParent,SVX_RES(RID_POPUPPANEL_LINEPAGE_STYLE))
,	mrLinePropertyPanel(rPanel)
,	mpBindings(NULL)
,	maVSStyle( this, SVX_RES(VS_STYLE))
//,	maPBOptions	(this, SVX_RES(PB_OPTIONS),Bitmap( SVX_RES( BMP_UNDERLINE_MORE )), Bitmap( SVX_RES( BMP_UNDERLINE_MORE_H )) )
,	maPBOptions	(this, SVX_RES(PB_OPTIONS))
,	mbVS(true)
{
	Initialize();
	FreeResource();
	mpBindings = mrLinePropertyPanel.GetBindings();
}

SvxLineStylePage::~SvxLineStylePage()
{
	delete[] maIMG;
	delete[] maIMGSel;
	delete[] maIMGH;
}

void SvxLineStylePage::Paint(const Rectangle& rect)
{
	svx::sidebar::PropertyPanelPopupPage::Paint(rect);
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

void SvxLineStylePage::Initialize()
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

	Link aLink = LINK( this, SvxLineStylePage, PBStyleClickHdl ) ;
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
	
	aLink =  LINK( this, SvxLineStylePage, VSSelectHdl ) ;
    maVSStyle.SetSelectHdl(aLink);

	maVSStyle.StartSelection();
	maVSStyle.Show();
}

void SvxLineStylePage::GetFocus()
{
	if(mbVS)
		maVSStyle.GrabFocus();
	else
		maPBOptions.GrabFocus();
}

Control& SvxLineStylePage::GetPB()
{
	return maPBOptions;
}

ValueSet& SvxLineStylePage::GetValueSet()
{
	return maVSStyle;
}

void SvxLineStylePage::SetStyleSelect(XLineStyleItem* pStyle, XLineDashItem* pDash, bool bAvailable, bool bDashAvailable)
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

void SvxLineStylePage::SetAllNoSel()
{
	maVSStyle.SelectItem(1); // ,false);		//modified // 'false' nut supported by AOO
	maVSStyle.SetNoSelection();
	maVSStyle.SetFirstSelect(false);
	for(sal_uInt16 i = 2; i <= 12; i++)
		maVSStyle.SetItemImage(i, GetDisplayBackground().GetColor().IsDark() ? maIMGH[i-2] : maIMG[i-2]);
}

IMPL_LINK(SvxLineStylePage, VSSelectHdl, void *, pControl)
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

		if (mrLinePropertyPanel.GetStyleFloatWin()->IsInPopupMode() )
			mrLinePropertyPanel.GetStyleFloatWin()->EndPopupMode();
	}
	return( 0L );
}

IMPL_LINK(SvxLineStylePage, PBStyleClickHdl, PushButton *, pPBtn)
{
	if(pPBtn == &maPBOptions)  
	{
		if (mpBindings)
		{
			SfxDispatcher* pDisp = mpBindings->GetDispatcher();
			pDisp->Execute( SID_ATTRIBUTES_LINE, SFX_CALLMODE_ASYNCHRON );
		}
		if (mrLinePropertyPanel.GetStyleFloatWin()->IsInPopupMode() )
			mrLinePropertyPanel.GetStyleFloatWin()->EndPopupMode();
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

class SvxValueSetLineWidth : public ValueSet
{
protected:
	VirtualDevice* 	pVDev;
	sal_uInt16			nSelItem;
	XubString*		strUnit;
	Image			imgCus;
	bool			bCusEnable;
public:
		SvxValueSetLineWidth( Window* pParent, const ResId& rResId);
		virtual ~SvxValueSetLineWidth();
		void SetUnit(XubString* str);
		void SetSelItem(sal_uInt16 nSel);
		sal_uInt16 GetSelItem();
		void SetImage(Image img);
		void SetCusEnable(bool bEnable);

	virtual void	UserDraw( const UserDrawEvent& rUDEvt );	
	
};

SvxValueSetLineWidth::SvxValueSetLineWidth( Window* pParent, const ResId& rResId) :
	ValueSet( pParent, rResId )   
,	pVDev(NULL)
,	nSelItem(0)
,	bCusEnable(false)
{
	strUnit = new XubString[9];
	SetColCount( 1 );
    SetLineCount( 9);
}

SvxValueSetLineWidth::~SvxValueSetLineWidth()
{	
	delete pVDev;
	delete[] strUnit;
}

void SvxValueSetLineWidth::SetUnit(XubString* str)
{
	for(int i = 0; i < 9; i++)
	{
		strUnit[i] = str[i];
	}
}

void SvxValueSetLineWidth::SetSelItem(sal_uInt16 nSel)
{
	nSelItem = nSel;
	if(nSel == 0)
	{
		SelectItem(1); // ,false); // 'false' nut supported by AOO
		SetNoSelection();
	}
	else
	{
		SelectItem(nSelItem);	
		GrabFocus();
	}
}

sal_uInt16 SvxValueSetLineWidth::GetSelItem()
{
	return nSelItem;
}

void SvxValueSetLineWidth::SetImage(Image img)
{
	imgCus = img;
}

void SvxValueSetLineWidth::SetCusEnable(bool bEnable)
{
	bCusEnable = bEnable;
}

void  SvxValueSetLineWidth::UserDraw( const UserDrawEvent& rUDEvt )
{
	Rectangle aRect = rUDEvt.GetRect();
	OutputDevice*  pDev = rUDEvt.GetDevice();
	sal_uInt16	nItemId = rUDEvt.GetItemId();
	
	long nRectHeight = aRect.GetHeight();
	long nRectWidth = aRect.GetWidth();	
	Point aBLPos = aRect.TopLeft();	

	//const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();	
	//Color aBackColor(0,0,200);
	//const Color aTextColor = rStyleSettings.GetFieldTextColor();
	Font aOldFont = pDev->GetFont();
	Color aOldColor = pDev->GetLineColor();
	Color aOldFillColor = pDev->GetFillColor();	

	Font aFont(OutputDevice::GetDefaultFont(DEFAULTFONT_UI_SANS, MsLangId::getSystemLanguage(), DEFAULTFONT_FLAGS_ONLYONE));
	Size aSize = aFont.GetSize();	
	aSize.Height() = nRectHeight*3/5;
	aFont.SetSize( aSize );

	Point aLineStart(aBLPos.X() + 5,			aBLPos.Y() + ( nRectHeight - nItemId )/2);
	Point aLineEnd(aBLPos.X() + nRectWidth * 7 / 9 - 10, aBLPos.Y() + ( nRectHeight - nItemId )/2);
	if(nItemId == 9)
	{
		Point aImgStart(aBLPos.X() + 5,			aBLPos.Y() + ( nRectHeight - 23 ) / 2);
		pDev->DrawImage(aImgStart, imgCus);
	//	Point aStart(aImgStart.X() + 14 + 20 , aBLPos.Y() + nRectHeight/6);
		Rectangle aStrRect = aRect;
		aStrRect.Top() += nRectHeight/6;
		aStrRect.Bottom() -= nRectHeight/6;
		aStrRect.Left() += imgCus.GetSizePixel().Width() + 20;
		if(bCusEnable)
			aFont.SetColor(GetSettings().GetStyleSettings().GetFieldTextColor());
		else
			aFont.SetColor(GetSettings().GetStyleSettings().GetDisableColor());

		pDev->SetFont(aFont);			    
		pDev->DrawText(aStrRect, strUnit[ nItemId - 1 ], TEXT_DRAW_ENDELLIPSIS);
	}
	else
	{
		if( nSelItem ==  nItemId )
		{
			Color aBackColor(50,107,197);
			Rectangle aBackRect = aRect;
			aBackRect.Top() += 3;
			aBackRect.Bottom() -= 2;
			pDev->SetFillColor(aBackColor);
			pDev->DrawRect(aBackRect);
		}
		else
		{
			pDev->SetFillColor( COL_TRANSPARENT );
			pDev->DrawRect(aRect);
		}

		//draw text	
		if(nSelItem ==  nItemId )
			aFont.SetColor(COL_WHITE);
		else 
			aFont.SetColor(GetSettings().GetStyleSettings().GetFieldTextColor());
		pDev->SetFont(aFont);		
		Point aStart(aBLPos.X() + nRectWidth * 7 / 9 , aBLPos.Y() + nRectHeight/6);
		pDev->DrawText(aStart, strUnit[ nItemId - 1 ]);	 //can't set TEXT_DRAW_ENDELLIPSIS here ,or the text will disappear	

		//draw line
		if( nSelItem ==  nItemId )
			pDev->SetLineColor(COL_WHITE);
		else  
			pDev->SetLineColor(GetSettings().GetStyleSettings().GetFieldTextColor());

		for(sal_uInt16 i = 1; i <= nItemId; i++)
		{	
			pDev->DrawLine(aLineStart,aLineEnd );	
			aLineStart.setY(aLineStart.getY() + 1);
			aLineEnd.setY  (aLineEnd.getY() + 1);
		}
	}

	Invalidate( aRect );
	pDev->SetLineColor(aOldColor);
	pDev->SetFillColor(aOldFillColor);
	pDev->SetFont(aOldFont);
}

//////////////////////////////////////////////////////////////////////////////

class SvxLineWidthPage : public svx::sidebar::PropertyPanelPopupPage
{
public:
	SvxLineWidthPage(Window* pParent, svx::sidebar::LinePropertyPanel& rPanel);
	~SvxLineWidthPage();
	void GetFocus();
	void SetWidthSelect( long lValue, bool bValuable, SfxMapUnit eMapUnit);
	virtual void Paint(const Rectangle& rect);
	ValueSet& GetValueSet();
	bool IsCloseByEdit();
	long GetTmpCustomWidth();

private:	
	svx::sidebar::LinePropertyPanel&    mrLinePropertyPanel;
	SfxBindings*		                mpBindings;
	SvxValueSetLineWidth			    maVSWidth;
	FixedText			                maFTCus;
//	Control				                maBorder;
	FixedText			                maFTWidth;
	MetricField			                maMFWidth;
	SfxMapUnit			                meMapUnit;
	XubString*			                rStr;
	XubString			                mstrPT;	//
	long				                mnCustomWidth;
	bool				                mbCustom;
	bool				                mbColseByEdit;
	long				                mnTmpCusomWidth;
	bool				                mbVSFocus;

	Image				                maIMGCus;
	Image				                maIMGCusGray;

    void Initialize();
	DECL_LINK(VSSelectHdl, void *);
	DECL_LINK(MFModifyHdl, void *);
};

SvxLineWidthPage::SvxLineWidthPage(Window* pParent, svx::sidebar::LinePropertyPanel& rPanel)
:	svx::sidebar::PropertyPanelPopupPage( pParent,SVX_RES(RID_POPUPPANEL_LINEPAGE_WIDTH))
,	mrLinePropertyPanel(rPanel)
,	mpBindings(NULL)
,	maVSWidth( this, SVX_RES(VS_WIDTH))
,	maFTCus( this, SVX_RES(FT_CUSTOME))
//,	maBorder(this, SVX_RES(CT_BORDER))
,	maFTWidth( this, SVX_RES(FT_LINE_WIDTH))//&maBorder
,	maMFWidth( this, SVX_RES(MF_WIDTH))//&maBorder
,	meMapUnit(SFX_MAPUNIT_TWIP)
,	rStr(NULL)
,	mstrPT(SVX_RES(STR_PT))	//
,	mnCustomWidth(0)
,	mbCustom(false)
,	mbColseByEdit(false)
,	mnTmpCusomWidth(0)
,	mbVSFocus(true)
,	maIMGCus(SVX_RES(IMG_WIDTH_CUSTOM))
,	maIMGCusGray(SVX_RES(IMG_WIDTH_CUSTOM_GRAY))
{
	Initialize();
	FreeResource();
	mpBindings = mrLinePropertyPanel.GetBindings();
}

SvxLineWidthPage::~SvxLineWidthPage()
{
	delete[] rStr;
}

void SvxLineWidthPage::Paint(const Rectangle& rect)
{
	svx::sidebar::PropertyPanelPopupPage::Paint(rect);
	Color aOldLineColor = GetLineColor();
	Color aOldFillColor = GetFillColor();

	//Point aPos = maBorder.GetPosPixel();	
	//Size aSize = maBorder.GetSizePixel();
	Point aPos( LogicToPixel( Point( CUSTOM_X, CUSTOM_Y), MAP_APPFONT ));
	Size aSize( LogicToPixel( Size(  CUSTOM_W, CUSTOM_H ), MAP_APPFONT ));
	Rectangle aRect( aPos, aSize );
	aRect.Left() -= 1;
	aRect.Top() -= 1;
	aRect.Right() += 1;
	aRect.Bottom() += 1;

	Color aLineColor(189,201,219);//(0,0,0);
	if(!GetSettings().GetStyleSettings().GetHighContrastMode())
		SetLineColor(aLineColor);
	else
		SetLineColor(GetSettings().GetStyleSettings().GetShadowColor());
	SetFillColor(COL_TRANSPARENT);
	DrawRect(aRect);

	SetLineColor(aOldLineColor);
	SetFillColor(aOldFillColor);
}

void SvxLineWidthPage::Initialize()
{
	maVSWidth.SetStyle( maVSWidth.GetStyle()| WB_3DLOOK |  WB_NO_DIRECTSELECT  );// WB_NAMEFIELD | WB_ITEMBORDER |WB_DOUBLEBORDER | WB_NONEFIELD |
	//for high contract wj
	if(GetSettings().GetStyleSettings().GetHighContrastMode())
	{
		maVSWidth.SetColor(GetSettings().GetStyleSettings().GetMenuColor());
	//	maBorder.SetBackground(GetSettings().GetStyleSettings().GetMenuColor());
		maFTWidth.SetBackground(GetSettings().GetStyleSettings().GetMenuColor());
	}
	else
	{
		maVSWidth.SetColor(COL_WHITE);
	//	maBorder.SetBackground(Wallpaper(COL_WHITE));
		maFTWidth.SetBackground(Wallpaper(COL_WHITE));
	}

	sal_Int64 nFirst= maMFWidth.Denormalize( maMFWidth.GetFirst( FUNIT_TWIP ) );
	sal_Int64 nLast = maMFWidth.Denormalize( maMFWidth.GetLast( FUNIT_TWIP ) );
	sal_Int64 nMin = maMFWidth.Denormalize( maMFWidth.GetMin( FUNIT_TWIP ) );
	sal_Int64 nMax = maMFWidth.Denormalize( maMFWidth.GetMax( FUNIT_TWIP ) );
	maMFWidth.SetSpinSize( 10 );
	maMFWidth.SetUnit( FUNIT_POINT );
    if( maMFWidth.GetDecimalDigits() > 1 )
		 maMFWidth.SetDecimalDigits( 1 );
	maMFWidth.SetFirst( maMFWidth.Normalize( nFirst ), FUNIT_TWIP );
	maMFWidth.SetLast( maMFWidth.Normalize( nLast ), FUNIT_TWIP );
	maMFWidth.SetMin( maMFWidth.Normalize( nMin ), FUNIT_TWIP );
	maMFWidth.SetMax( maMFWidth.Normalize( nMax ), FUNIT_TWIP );	

	rStr = new XubString[9];
	//modify,
	rStr[0] = String("05", 2, RTL_TEXTENCODING_ASCII_US);
	rStr[1] = String("08", 2, RTL_TEXTENCODING_ASCII_US);
	rStr[2] = String("10", 2, RTL_TEXTENCODING_ASCII_US);
	rStr[3] = String("15", 2, RTL_TEXTENCODING_ASCII_US);
	rStr[4] = String("23", 2, RTL_TEXTENCODING_ASCII_US);
	rStr[5] = String("30", 2, RTL_TEXTENCODING_ASCII_US);
	rStr[6] = String("45", 2, RTL_TEXTENCODING_ASCII_US);
	rStr[7] = String("60", 2, RTL_TEXTENCODING_ASCII_US);	
	rStr[8] = String( SVX_RES(STR_WIDTH_LAST_CUSTOM) );

	LocaleDataWrapper aLocaleWrapper( ::comphelper::getProcessServiceFactory(), Application::GetSettings().GetLocale() );
	const sal_Unicode cSep = aLocaleWrapper.getNumDecimalSep().GetChar(0);

	for(int i = 0; i <= 7 ; i++)
	{
		rStr[i] = rStr[i].Insert(cSep, 1);//Modify
		rStr[i].Append(mstrPT);
	}
	//end

	for(sal_uInt16 i = 1 ; i <= 9 ; i++)
	{
		maVSWidth.InsertItem(i);
		maVSWidth.SetItemText(i, rStr[i-1]);
	}
	maVSWidth.SetUnit(rStr);
	maVSWidth.SetItemData(1,(void*)5);
	maVSWidth.SetItemData(2,(void*)8);
	maVSWidth.SetItemData(3,(void*)10);
	maVSWidth.SetItemData(4,(void*)15);
	maVSWidth.SetItemData(5,(void*)23);
	maVSWidth.SetItemData(6,(void*)30);
	maVSWidth.SetItemData(7,(void*)45);
	maVSWidth.SetItemData(8,(void*)60);
	maVSWidth.SetImage(maIMGCusGray);

	maVSWidth.SetSelItem(0);
	Link aLink =  LINK( this, SvxLineWidthPage, VSSelectHdl ) ;
    maVSWidth.SetSelectHdl(aLink);
	aLink = LINK(this, SvxLineWidthPage, MFModifyHdl);
	maMFWidth.SetModifyHdl(aLink);

	maVSWidth.StartSelection();
	maVSWidth.Show();
}

void SvxLineWidthPage::GetFocus()
{
	if(!mbVSFocus)
		maMFWidth.GrabFocus();
	else
		maVSWidth.GrabFocus();
}

ValueSet& SvxLineWidthPage::GetValueSet()
{
	return maVSWidth;
}

void SvxLineWidthPage::SetWidthSelect( long lValue, bool bValuable, SfxMapUnit eMapUnit)
{
	mbVSFocus = true;
	maVSWidth.SetSelItem(0);
	mbColseByEdit = false;
	meMapUnit = eMapUnit;
	SvtViewOptions aWinOpt( E_WINDOW, SIDEBAR_LINE_WIDTH_GLOBAL_VALUE );
    if ( aWinOpt.Exists() )
	{
		::com::sun::star::uno::Sequence < ::com::sun::star::beans::NamedValue > aSeq = aWinOpt.GetUserData();
		::rtl::OUString aTmp;
		if ( aSeq.getLength())
			aSeq[0].Value >>= aTmp;

		String aWinData( aTmp );
		mnCustomWidth = aWinData.ToInt32();
		mbCustom = true;
		maVSWidth.SetImage(maIMGCus);
		maVSWidth.SetCusEnable(true);

		String aStrTip( String::CreateFromDouble( (double)mnCustomWidth / 10));
		aStrTip.Append(mstrPT);		//		
		maVSWidth.SetItemText(9, aStrTip);
	}
	else
	{
		mbCustom = false;
		maVSWidth.SetImage(maIMGCusGray);
		maVSWidth.SetCusEnable(false);
		//modify 
		//String aStrTip(String(SVX_RES(STR_WIDTH_LAST_CUSTOM)));	
		//maVSWidth.SetItemText(9, aStrTip);
		maVSWidth.SetItemText(9, rStr[8]);
	}
	
	if(bValuable)
    {
        sal_Int64 nVal = OutputDevice::LogicToLogic( lValue, (MapUnit)eMapUnit, MAP_100TH_MM );
        nVal = maMFWidth.Normalize( nVal );
        maMFWidth.SetValue( nVal, FUNIT_100TH_MM );
    }
	else 
    {
		maMFWidth.SetText( String() );
    }

	MapUnit eOrgUnit = (MapUnit)eMapUnit;
	MapUnit ePntUnit( MAP_TWIP );		
	lValue = LogicToLogic( lValue , eOrgUnit, ePntUnit );

	XubString strCurrValue = maMFWidth.GetText();
	sal_uInt16 i = 0;
	for(; i < 8; i++)
		if(strCurrValue == rStr[i])
		{
			maVSWidth.SetSelItem(i+1);
			break;
		}
	if (i>=8)
	{
		mbVSFocus = false;
		maVSWidth.SetSelItem(0);
	}
	maVSWidth.Format();
	maVSWidth.StartSelection();
}

IMPL_LINK(SvxLineWidthPage, VSSelectHdl, void *, pControl)
{
	if(pControl == &maVSWidth)  
	{		
		sal_uInt16 iPos = maVSWidth.GetSelectItemId();
		if(iPos >= 1 && iPos <= 8)
		{
			long nVal = LogicToLogic((long)(unsigned long)maVSWidth.GetItemData( iPos ) , MAP_POINT, (MapUnit)meMapUnit);
			nVal = maMFWidth.Denormalize(nVal);
			XLineWidthItem aWidthItem( nVal );
			mpBindings->GetDispatcher()->Execute(SID_ATTR_LINE_WIDTH, SFX_CALLMODE_RECORD, &aWidthItem, 0L);
			mrLinePropertyPanel.SetWidthIcon(iPos);
			mrLinePropertyPanel.SetWidth(nVal);
			mbColseByEdit = false;
			mnTmpCusomWidth = 0;
		}
		else if(iPos == 9)
		{//last custom
			//modified 
			if(mbCustom)
			{
				long nVal = LogicToLogic(mnCustomWidth , MAP_POINT, (MapUnit)meMapUnit);
				nVal = maMFWidth.Denormalize(nVal);
				XLineWidthItem aWidthItem( nVal );
				mpBindings->GetDispatcher()->Execute(SID_ATTR_LINE_WIDTH, SFX_CALLMODE_RECORD, &aWidthItem, 0L);
				mrLinePropertyPanel.SetWidth(nVal);
				mbColseByEdit = false;
				mnTmpCusomWidth = 0;
			}
			else
			{
				maVSWidth.SetNoSelection();		//add , set no selection and keep the last select item 
				maVSWidth.Format();
				Invalidate();
				maVSWidth.StartSelection();
			}
			//modify end
		}
		if((iPos >= 1 && iPos <= 8) || (iPos == 9 && mbCustom)) //add 
			if (mrLinePropertyPanel.GetWidthFloatWin()->IsInPopupMode() )
				mrLinePropertyPanel.GetWidthFloatWin()->EndPopupMode();
	}
	return( 0L );
}

IMPL_LINK(SvxLineWidthPage, MFModifyHdl, void *, pControl)
{
	if(pControl == &maMFWidth)  
	{
		if(maVSWidth.GetSelItem())
		{
			maVSWidth.SetSelItem(0);
			maVSWidth.Format();
			Invalidate();
			maVSWidth.StartSelection();
		}
		long nTmp = static_cast<long>(maMFWidth.GetValue());
		long nVal = LogicToLogic( nTmp, MAP_POINT, (MapUnit)meMapUnit );
		sal_Int32 nNewWidth = (short)maMFWidth.Denormalize( nVal );
		XLineWidthItem aWidthItem(nNewWidth);
		mpBindings->GetDispatcher()->Execute(SID_ATTR_LINE_WIDTH, SFX_CALLMODE_RECORD, &aWidthItem, 0L);
		
		mbColseByEdit = true;
		mnTmpCusomWidth = nTmp;
		/*for(sal_uInt16 i = 0; i < 8; i++)
		{
			if(nTmp == (sal_Int32)maVSWidth.GetItemData(i))
			{
				mbColseByEdit = false;
				break;
			}
		}*/

	}
	return( 0L );
}

bool SvxLineWidthPage::IsCloseByEdit()
{
	return mbColseByEdit;
}

long SvxLineWidthPage::GetTmpCustomWidth()
{
	return mnTmpCusomWidth;
}

//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////
// namespace open

namespace svx { namespace sidebar {

//////////////////////////////////////////////////////////////////////////////

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
    maColorControl(SID_ATTR_LINE_COLOR, *pBindings, *this),
    maStyleControl(SID_ATTR_LINE_STYLE, *pBindings, *this),
    maDashControl (SID_ATTR_LINE_DASH, *pBindings, *this),
    maWidthControl(SID_ATTR_LINE_WIDTH, *pBindings, *this),
    maTransControl(SID_ATTR_LINE_TRANSPARENCE, *pBindings, *this),
    maStartControl(SID_ATTR_LINE_START, *pBindings, *this),
    maEndControl(SID_ATTR_LINE_END, *pBindings, *this),
    maLineEndListControl(SID_LINEEND_LIST, *pBindings, *this),
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
    mpFloatWinColor(0),
    mpPageColor(0),
    mpFloatWinStyle(0),
    mpPageStyle(0),
    mpFloatWinWidth(0),
    mpPageWidth(0),
    maIMGColor(SVX_RES(IMG_COLOR)),
    maIMGNone(SVX_RES(IMG_NONE_ICON)),
    mpIMGStyleIcon(new Image[11]),
    mpIMGWidthIcon(new Image[8]),
    mpIMGStyleIconH(new Image[11]),
    mpIMGWidthIconH(new Image[8]),
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

//////////////////////////////////////////////////////////////////////////////

LinePropertyPanel::~LinePropertyPanel()
{
    // Destroy the background windows of the toolboxes.
    mpTBWidthBackground.reset();
    mpTBColorBackground.reset();
    mpTBStyleBackground.reset();
}

//////////////////////////////////////////////////////////////////////////////

void LinePropertyPanel::ShowMenu (void)
{
    if (mpBindings != NULL)
    {
        SfxDispatcher* pDispatcher = mpBindings->GetDispatcher();
        if (pDispatcher != NULL)
            pDispatcher->Execute(SID_ATTRIBUTES_LINE, SFX_CALLMODE_ASYNCHRON);
    }
}

//////////////////////////////////////////////////////////////////////////////

void LinePropertyPanel::Initialize()
{
	mpIMGStyleIcon.get()[0] = Image(SVX_RES(IMG_LINE1_ICON));
	mpIMGStyleIcon.get()[1] = Image(SVX_RES(IMG_LINE2_ICON));
	mpIMGStyleIcon.get()[2] = Image(SVX_RES(IMG_LINE3_ICON));
	mpIMGStyleIcon.get()[3] = Image(SVX_RES(IMG_LINE4_ICON));
	mpIMGStyleIcon.get()[4] = Image(SVX_RES(IMG_LINE5_ICON));
	mpIMGStyleIcon.get()[5] = Image(SVX_RES(IMG_LINE6_ICON));
	mpIMGStyleIcon.get()[6] = Image(SVX_RES(IMG_LINE7_ICON));
	mpIMGStyleIcon.get()[7] = Image(SVX_RES(IMG_LINE8_ICON));
	mpIMGStyleIcon.get()[8] = Image(SVX_RES(IMG_LINE9_ICON));
	mpIMGStyleIcon.get()[9] = Image(SVX_RES(IMG_LINE10_ICON));
	mpIMGStyleIcon.get()[10] = Image(SVX_RES(IMG_LINE11_ICON));

	mpIMGWidthIcon.get()[0] = Image(SVX_RES(IMG_WIDTH1_ICON));
	mpIMGWidthIcon.get()[1] = Image(SVX_RES(IMG_WIDTH2_ICON));
	mpIMGWidthIcon.get()[2] = Image(SVX_RES(IMG_WIDTH3_ICON));
	mpIMGWidthIcon.get()[3] = Image(SVX_RES(IMG_WIDTH4_ICON));
	mpIMGWidthIcon.get()[4] = Image(SVX_RES(IMG_WIDTH5_ICON));
	mpIMGWidthIcon.get()[5] = Image(SVX_RES(IMG_WIDTH6_ICON));
	mpIMGWidthIcon.get()[6] = Image(SVX_RES(IMG_WIDTH7_ICON));
	mpIMGWidthIcon.get()[7] = Image(SVX_RES(IMG_WIDTH8_ICON));

	//high contrast
	mpIMGStyleIconH.get()[0] = Image(SVX_RES(IMG_LINE1_ICON_H));
	mpIMGStyleIconH.get()[1] = Image(SVX_RES(IMG_LINE2_ICON_H));
	mpIMGStyleIconH.get()[2] = Image(SVX_RES(IMG_LINE3_ICON_H));
	mpIMGStyleIconH.get()[3] = Image(SVX_RES(IMG_LINE4_ICON_H));
	mpIMGStyleIconH.get()[4] = Image(SVX_RES(IMG_LINE5_ICON_H));
	mpIMGStyleIconH.get()[5] = Image(SVX_RES(IMG_LINE6_ICON_H));
	mpIMGStyleIconH.get()[6] = Image(SVX_RES(IMG_LINE7_ICON_H));
	mpIMGStyleIconH.get()[7] = Image(SVX_RES(IMG_LINE8_ICON_H));
	mpIMGStyleIconH.get()[8] = Image(SVX_RES(IMG_LINE9_ICON_H));
	mpIMGStyleIconH.get()[9] = Image(SVX_RES(IMG_LINE10_ICON_H));
	mpIMGStyleIconH.get()[10] = Image(SVX_RES(IMG_LINE11_ICON_H));

	mpIMGWidthIconH.get()[0] = Image(SVX_RES(IMG_WIDTH1_ICON_H));
	mpIMGWidthIconH.get()[1] = Image(SVX_RES(IMG_WIDTH2_ICON_H));
	mpIMGWidthIconH.get()[2] = Image(SVX_RES(IMG_WIDTH3_ICON_H));
	mpIMGWidthIconH.get()[3] = Image(SVX_RES(IMG_WIDTH4_ICON_H));
	mpIMGWidthIconH.get()[4] = Image(SVX_RES(IMG_WIDTH5_ICON_H));
	mpIMGWidthIconH.get()[5] = Image(SVX_RES(IMG_WIDTH6_ICON_H));
	mpIMGWidthIconH.get()[6] = Image(SVX_RES(IMG_WIDTH7_ICON_H));
	mpIMGWidthIconH.get()[7] = Image(SVX_RES(IMG_WIDTH8_ICON_H));
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

	mpTBStyle->SetItemImage(TBI_STYLE, mpIMGStyleIcon.get()[0]);
	aTbxSize = mpTBStyle->CalcWindowSizePixel() ;
	mpTBStyle->SetOutputSizePixel( aTbxSize );
	mpTBStyle->SetItemBits( TBI_STYLE, mpTBStyle->GetItemBits( TBI_STYLE ) | TIB_DROPDOWNONLY );
	mpTBStyle->SetQuickHelpText(TBI_STYLE,String(SVX_RES(STR_QH_TB_STYLE))); //Add
	mpTBStyle->SetBackground(Wallpaper());
	mpTBStyle->SetPaintTransparent(true);
	aLink = LINK(this, LinePropertyPanel, ToolboxStyleSelectHdl);
	mpTBStyle->SetDropdownClickHdl ( aLink );
	mpTBStyle->SetSelectHdl ( aLink );

	mpTBWidth->SetItemImage(TBI_WIDTH, mpIMGWidthIcon.get()[0]);
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
	aLink = LINK( this, LinePropertyPanel, ChangeEndHdl );
	mpLBEnd->SetSelectHdl( aLink );
	mpLBEnd->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Ending Style")));	//wj acc

	aLink = LINK(this, LinePropertyPanel, ChangeTransparentHdl);
	mpMFTransparent->SetModifyHdl(aLink);
	mpMFTransparent->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Transparency")));	//wj acc
	
	mpTBWidth->SetAccessibleRelationLabeledBy(mpFTWidth.get());
	mpTBColor->SetAccessibleRelationLabeledBy(mpFTColor.get());
	mpTBStyle->SetAccessibleRelationLabeledBy(mpFTStyle.get());
	mpMFTransparent->SetAccessibleRelationLabeledBy(mpFTTrancparency.get());
	mpLBStart->SetAccessibleRelationLabeledBy(mpFTArrow.get());
	mpLBEnd->SetAccessibleRelationLabeledBy(mpLBEnd.get());
}

//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////

void LinePropertyPanel::DataChanged(
    const DataChangedEvent& rEvent)
{
    (void)rEvent;
    
    SetupIcons();
}

//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////

void LinePropertyPanel::NotifyItemUpdate( 
    sal_uInt16 nSID, 
    SfxItemState eState, 
    const SfxPoolItem* pState)
{
	switch(nSID)
	{
	case SID_ATTR_LINE_COLOR:
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
			if (  eState >= SFX_ITEM_DEFAULT && pState && pState->ISA(XLineColorItem) )
			{
				maColor = ((const XLineColorItem*)pState)->GetColorValue();
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
	case SID_ATTR_LINE_STYLE:
	case SID_ATTR_LINE_DASH:
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
				if(nSID == SID_ATTR_LINE_STYLE && pState && pState->ISA(XLineStyleItem))
				{
					mbStyleAvailable =true;
					mpStyleItem.reset(pState ? (XLineStyleItem*)pState->Clone() : 0);
				}
				else if(nSID == SID_ATTR_LINE_DASH && pState && pState->ISA(XLineDashItem))
				{
					mbDashAvailable = true;
					mpDashItem.reset(pState ? (XLineDashItem*)pState->Clone() : 0);
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
	case SID_ATTR_LINE_TRANSPARENCE:
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
			if(eState != SFX_ITEM_DONTCARE && pState && pState->ISA(XLineTransparenceItem) )
			{
				const XLineTransparenceItem* pItem = (const XLineTransparenceItem*)pState;
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
	case SID_ATTR_LINE_WIDTH:
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
			if(eState >= SFX_ITEM_AVAILABLE && pState->ISA(XLineWidthItem) )
			{
				const XLineWidthItem* pItem = (const XLineWidthItem*)pState;
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
	case SID_ATTR_LINE_START:
		if(eState != SFX_ITEM_DONTCARE && pState && pState->ISA(XLineStartItem) )
		{
			mbStartAvailable = true;	//add 
			mpStartItem.reset(pState ? (XLineStartItem*)pState->Clone() : 0);
			SelectEndStyle(true);
		}
		else
		{
			mpLBStart->SetNoSelection();
			mbStartAvailable = false;	//add 
		}
		break;
	case SID_ATTR_LINE_END:
		mpFTArrow->Enable();
		mpLBEnd->Enable();
		if(eState != SFX_ITEM_DONTCARE && pState && pState->ISA(XLineEndItem))
		{
			mbEndAvailable = true;		//add 
			mpEndItem.reset(pState ? (XLineEndItem*)pState->Clone() : 0);
			SelectEndStyle(false);		
		}
		else
		{
			mpLBEnd->SetNoSelection();
			mbEndAvailable = false;		//add 
		}	
		break;
	case SID_LINEEND_LIST:
		FillLineEndList();
		SelectEndStyle(true);
		SelectEndStyle(false);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////

SfxBindings* LinePropertyPanel::GetBindings() 
{ 
    return mpBindings; 
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( LinePropertyPanel, ImplPopupModeEndHdl, FloatingWindow*, EMPTYARG )
{	
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( LinePropertyPanel, ImplWidthPopupModeEndHdl, FloatingWindow*, EMPTYARG )
{
	if(mpPageWidth)
	{
		if( mpPageWidth->IsCloseByEdit() )
		{
			SvtViewOptions aWinOpt( E_WINDOW, SIDEBAR_LINE_WIDTH_GLOBAL_VALUE );
			::com::sun::star::uno::Sequence < ::com::sun::star::beans::NamedValue > aSeq(1);
			aSeq[0].Name = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("LineWidth") ); 
			aSeq[0].Value <<= ::rtl::OUString( String::CreateFromInt32( mpPageWidth->GetTmpCustomWidth() ));
			aWinOpt.SetUserData( aSeq );
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK(LinePropertyPanel, ToolboxColorSelectHdl,ToolBox*, pToolBox)
{
	sal_uInt16 nId = pToolBox->GetCurItemId();
	if(nId == TBI_COLOR)
	{
		pToolBox->SetItemDown( nId, true );

		SvxLineColorPage* pColorPage = GetColorPage();
		Size aFloatSz = pColorPage->GetOutputSizePixel();
		GetColorFloatWin()->SetSizePixel( aFloatSz );

		Point aPos = mpTBColor->GetPosPixel();
		aPos = OutputToScreenPixel( aPos );
		Size aSize = mpTBColor->GetSizePixel();
		Rectangle aRect( aPos, aSize );

		GetColorFloatWin()->StartPopupMode( aRect, FLOATWIN_POPUPMODE_NOFOCUSCLOSE|FLOATWIN_POPUPMODE_DOWN );
		GetColorFloatWin()->SetPopupModeFlags(GetColorFloatWin()->GetPopupModeFlags() | FLOATWIN_POPUPMODE_NOAPPFOCUSCLOSE );

		pColorPage->GetFocus();
		pColorPage->SetCurColorSelect(maColor, mbColorAvailable);	
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK(LinePropertyPanel, ToolboxStyleSelectHdl,ToolBox*, pToolBox)
{
	sal_uInt16 nId = pToolBox->GetCurItemId();
	if(nId == TBI_STYLE)
	{
		pToolBox->SetItemDown( nId, true );

		SvxLineStylePage* pStylePage = GetStylePage();
		pStylePage->SetStyleSelect(mpStyleItem.get(), mpDashItem.get(), mbStyleAvailable, mbDashAvailable);	

		Size aFloatSz = pStylePage->GetOutputSizePixel();
		GetStyleFloatWin()->SetSizePixel( aFloatSz );

		Point aPos = mpTBStyle->GetPosPixel();
		aPos = OutputToScreenPixel( aPos );
		Size aSize = mpTBStyle->GetSizePixel();
		Rectangle aRect( aPos, aSize );

		GetStyleFloatWin()->StartPopupMode( aRect, FLOATWIN_POPUPMODE_NOFOCUSCLOSE|FLOATWIN_POPUPMODE_DOWN );
		GetStyleFloatWin()->SetPopupModeFlags(GetStyleFloatWin()->GetPopupModeFlags() | FLOATWIN_POPUPMODE_NOAPPFOCUSCLOSE );

		pStylePage->GetFocus();
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK(LinePropertyPanel, ToolboxWidthSelectHdl,ToolBox*, pToolBox)
{
	sal_uInt16 nId = pToolBox->GetCurItemId();
	if(nId == TBI_WIDTH)
	{
		pToolBox->SetItemDown( nId, true );

		SvxLineWidthPage* pWidthPage = GetWidthPage();
		pWidthPage->SetWidthSelect( mnWidthCoreValue, mbWidthValuable, meMapUnit);

		Size aFloatSz = pWidthPage->GetOutputSizePixel();
		GetWidthFloatWin()->SetSizePixel( aFloatSz );

		Point aPos = mpTBWidth->GetPosPixel();
		aPos = OutputToScreenPixel( aPos );
		Size aSize = mpTBWidth->GetSizePixel();
		Rectangle aRect( aPos, aSize );

		if(pWidthPage)
		{
			GetWidthFloatWin()->StartPopupMode( aRect, FLOATWIN_POPUPMODE_NOFOCUSCLOSE|FLOATWIN_POPUPMODE_DOWN );
			GetWidthFloatWin()->SetPopupModeFlags(GetWidthFloatWin()->GetPopupModeFlags() | FLOATWIN_POPUPMODE_NOAPPFOCUSCLOSE );	
			pWidthPage->GetFocus();
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( LinePropertyPanel, ChangeTransparentHdl, void *, EMPTYARG )
{
	sal_uInt16 nVal = (sal_uInt16)mpMFTransparent->GetValue();
	XLineTransparenceItem aItem( nVal );

	GetBindings()->GetDispatcher()->Execute(SID_ATTR_LINE_STYLE, SFX_CALLMODE_RECORD, &aItem, 0L);
	return( 0L );
}

//////////////////////////////////////////////////////////////////////////////
//color popup window

void LinePropertyPanel::ImpEnsureFloatWinColorAndPageColor()
{
    if(!mpFloatWinColor)
    {
        mpFloatWinColor.reset(new PropertyPanelPopuplWindow(this));
        mpFloatWinColor->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Color")));
        mpFloatWinColor->SetPopupModeEndHdl(LINK( this, LinePropertyPanel, ImplPopupModeEndHdl));
        mpPageColor.reset(new SvxLineColorPage(mpFloatWinColor.get(), *this));
        mpFloatWinColor->SetBorderStyle(mpFloatWinColor->GetBorderStyle() | WINDOW_BORDER_MENU);
    }
}

SvxLineColorPage* LinePropertyPanel::GetColorPage()
{
    ImpEnsureFloatWinColorAndPageColor();

    return mpPageColor.get();
}

PropertyPanelPopuplWindow* LinePropertyPanel::GetColorFloatWin()
{
    ImpEnsureFloatWinColorAndPageColor();

    return mpFloatWinColor.get();
}

//////////////////////////////////////////////////////////////////////////////
//style popup window

void LinePropertyPanel::ImpEnsureFloatWinStyleAndPageStyle()
{
    if(!mpFloatWinStyle)
    {
        mpFloatWinStyle.reset(new PropertyPanelPopuplWindow(this));
        mpFloatWinStyle->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Style")));
        mpFloatWinStyle->SetPopupModeEndHdl( LINK( this, LinePropertyPanel, ImplPopupModeEndHdl ) );
        mpPageStyle.reset(new SvxLineStylePage(mpFloatWinStyle.get(), *this));
        mpFloatWinStyle->SetBorderStyle( mpFloatWinStyle->GetBorderStyle() | WINDOW_BORDER_MENU );
    }
}

SvxLineStylePage* LinePropertyPanel::GetStylePage()
{
    if(!mpFloatWinStyle)
    {
        ImpEnsureFloatWinStyleAndPageStyle();
        Size aSize = mpPageStyle->GetSizePixel();
        Size aSizeVS(182, 23*12);

        mpPageStyle->GetValueSet().SetSizePixel(aSizeVS);
        aSize.setWidth (aSize.Width() + aSizeVS.Width());
        aSize.setHeight (aSize.Height() + aSizeVS.Height());
        mpPageStyle->SetSizePixel(aSize);
        mpPageStyle->GetPB().SetSizePixel( Size(aSizeVS.Width() - 15 , mpPageStyle->GetPB().GetSizePixel().Height()) );
        mpPageStyle->GetPB().SetPosPixel(Point(mpPageStyle->GetPB().GetPosPixel().X(), mpPageStyle->GetPB().GetPosPixel().Y() + aSizeVS.Height()));
    }

    return mpPageStyle.get();
}

PropertyPanelPopuplWindow* LinePropertyPanel::GetStyleFloatWin()
{
    ImpEnsureFloatWinStyleAndPageStyle();

    return mpFloatWinStyle.get();
}

//////////////////////////////////////////////////////////////////////////////
// width popup window

void LinePropertyPanel::ImpEnsureFloatWinWidthAndPageWidth()
{
    if(!mpFloatWinWidth)
    {
        mpFloatWinWidth.reset(new PropertyPanelPopuplWindow(this));
        mpFloatWinWidth->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Width")));
        mpFloatWinWidth->SetPopupModeEndHdl( LINK( this, LinePropertyPanel, ImplWidthPopupModeEndHdl ) );
        mpPageWidth.reset(new SvxLineWidthPage(mpFloatWinWidth.get(), *this));
        mpFloatWinWidth->SetBorderStyle( mpFloatWinWidth->GetBorderStyle() | WINDOW_BORDER_MENU );
    }
}

SvxLineWidthPage* LinePropertyPanel::GetWidthPage()
{
    ImpEnsureFloatWinWidthAndPageWidth();

    return mpPageWidth.get();
}

PropertyPanelPopuplWindow* LinePropertyPanel::GetWidthFloatWin()
{
    ImpEnsureFloatWinWidthAndPageWidth();

    return mpFloatWinWidth.get();
}

//////////////////////////////////////////////////////////////////////////////

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
		mpTBStyle->SetItemImage(TBI_STYLE, GetDisplayBackground().GetColor().IsDark() ? mpIMGStyleIconH.get()[0] : mpIMGStyleIcon.get()[0]);
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
					mpTBStyle->SetItemImage(TBI_STYLE, GetDisplayBackground().GetColor().IsDark() ? mpIMGStyleIconH.get()[n+1] :mpIMGStyleIcon.get()[n+1]);
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

//////////////////////////////////////////////////////////////////////////////

void LinePropertyPanel::SetWidthIcon(int n)
{
	if(n==0)
		mpTBWidth->SetItemImage( TBI_WIDTH, maIMGNone);
	else
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH.get()[n-1] : mpIMGWidthIcon.get()[n-1]);
}

//////////////////////////////////////////////////////////////////////////////

void LinePropertyPanel::SetWidthIcon()
{
	if(!mbWidthValuable)
	{
		mpTBWidth->SetItemImage( TBI_WIDTH, maIMGNone);
		return;
	}

	long nVal = LogicToLogic(mnWidthCoreValue * 10,(MapUnit)meMapUnit , MAP_POINT);

	if(nVal <= 6)	
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH.get()[0] : mpIMGWidthIcon.get()[0]); 
	else if(nVal > 6 && nVal <= 9)	
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH.get()[1] : mpIMGWidthIcon.get()[1]);  
	else if(nVal > 9 && nVal <= 12)	
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH.get()[2] : mpIMGWidthIcon.get()[2]); 
	else if(nVal > 12 && nVal <= 19)	
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH.get()[3] : mpIMGWidthIcon.get()[3]); 
	else if(nVal > 19 && nVal <= 26)		
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH.get()[4] : mpIMGWidthIcon.get()[4]); 
	else if(nVal > 26 && nVal <= 37)	
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH.get()[5] : mpIMGWidthIcon.get()[5]); 
	else if(nVal > 37 && nVal <=52)		
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH.get()[6] : mpIMGWidthIcon.get()[6]); 
	else if(nVal > 52)		
		mpTBWidth->SetItemImage( TBI_WIDTH, GetDisplayBackground().GetColor().IsDark() ? mpIMGWidthIconH.get()[7] : mpIMGWidthIcon.get()[7]); 

}

//////////////////////////////////////////////////////////////////////////////

void LinePropertyPanel::SetLineStyleItem(XLineStyleItem* pStyle)
{
    mpStyleItem.reset(pStyle ? (XLineStyleItem*)pStyle->Clone() : 0);
}

//////////////////////////////////////////////////////////////////////////////

void LinePropertyPanel::SetLineDashItem(XLineDashItem* pDash)
{
    mpDashItem.reset(pDash ? (XLineDashItem*)pDash->Clone() : 0);
}

//////////////////////////////////////////////////////////////////////////////

void LinePropertyPanel::SetColor(Color aColor)
{
    maColor = aColor;
}

//////////////////////////////////////////////////////////////////////////////

void LinePropertyPanel::SetWidth(long nWidth)
{
    mnWidthCoreValue = nWidth;
    mbWidthValuable = true;
}

//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////
// namespace close

}} // end of namespace ::svx::sidebar

//////////////////////////////////////////////////////////////////////////////
// eof
