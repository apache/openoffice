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

class SvxTextPropertyPage::SvxTextFontColorPage:public SfxPopupPage
{
public:
	SvxTextFontColorPage(Window* pParent, SvxTextPropertyPage* pPage);
	void GetFocus();
	void SetCurColorSelect(Color aCol, BOOL bAvl);
private:	
	SvxTextPropertyPage* mpPage;
	SfxBindings*		mpBindings;
	ValueSet			maVSColor;

	void initial();
	void FillColors();
	DECL_LINK(VSSelectHdl, void *);
};
SvxTextPropertyPage::SvxTextFontColorPage::SvxTextFontColorPage(Window* pParent, SvxTextPropertyPage* pPage)
:	SfxPopupPage( pParent,SVX_RES(RID_POPUPPANEL_TEXTPAGE_FONT_COLOR))
,	mpPage(pPage)
,	mpBindings(NULL)
,	maVSColor( this, SVX_RES(VS_FONT_COLOR))
{
	initial();
	FreeResource();
	if (mpPage)
		mpBindings = mpPage->GetBindings();
}

void SvxTextPropertyPage::SvxTextFontColorPage::initial()
{
	WinBits nBits = ( maVSColor.GetStyle() | WB_NONEFIELD | WB_ITEMBORDER | WB_NAMEFIELD | WB_3DLOOK | WB_NO_DIRECTSELECT | WB_SELECTIFOLLOWMOUSE); //Modified  for color picker migration 01/20/10
	maVSColor.SetText( String( SVX_RES( STR_AUTOMATICE ) ) );
	maVSColor.SetStyle( nBits );
	//Modified  for color picker migration 01/20/10
	//maVSColor.SetColCount( 10 );
	//maVSColor.SetLineCount( 10 );
	maVSColor.SetColCount( 12 );
	maVSColor.SetLineCount( 12 );
	//End
	const Size aSize15x15 = Size( 15, 15 );
	maVSColor.CalcWindowSizePixel( aSize15x15 );
	maVSColor.Show();

	Link aLink =  LINK( this, SvxTextFontColorPage, VSSelectHdl ) ;
    maVSColor.SetSelectHdl(aLink);
}
void SvxTextPropertyPage::SvxTextFontColorPage::FillColors()
{
	SfxObjectShell* pDocSh = SfxObjectShell::Current();
	const SfxPoolItem* pItem = NULL;
	XColorTable* pColorTable = NULL;
	FASTBOOL bOwn = FALSE;

	DBG_ASSERT( pDocSh, "DocShell not found!" );

	if ( pDocSh && ( 0 != ( pItem = pDocSh->GetItem( SID_COLOR_TABLE ) ) ) )
		pColorTable = ( (SvxColorTableItem*)pItem )->GetColorTable();

	if ( !pColorTable )
	{
		bOwn = TRUE;
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
void SvxTextPropertyPage::SvxTextFontColorPage::GetFocus()
{
	maVSColor.GrabFocus();
}
void SvxTextPropertyPage::SvxTextFontColorPage::SetCurColorSelect(Color aCol, BOOL bAvailable)
{
//	USHORT nType = mpPage->GetCurrColorType();
		
	FillColors();	
	short nCol = GetItemId_Impl( maVSColor, aCol );
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
IMPL_LINK(SvxTextPropertyPage::SvxTextFontColorPage, VSSelectHdl, void *, pControl)
{
	if(pControl = &maVSColor)
	{
		USHORT nType = mpPage->GetCurrColorType();

		USHORT iPos = maVSColor.GetSelectItemId();
		Color aColor = maVSColor.GetItemColor( iPos );	  
		if(nType == FONT_COLOR)
		{
			if(iPos == 0)
				aColor = COL_AUTO;
			SvxColorItem aColorItem(aColor, SID_ATTR_CHAR_COLOR);
			mpBindings->GetDispatcher()->Execute(SID_ATTR_CHAR_COLOR, SFX_CALLMODE_RECORD, &aColorItem, 0L);
			mpPage->SetColor(aColor);
		}
		else if(nType == BACK_COLOR)
		{
			if(iPos == 0)
				aColor = COL_TRANSPARENT;
			SvxBrushItem aBrushItem(aColor, SID_ATTR_BRUSH_CHAR);
			mpBindings->GetDispatcher()->Execute(SID_ATTR_BRUSH_CHAR, SFX_CALLMODE_RECORD, &aBrushItem, 0L);
			mpPage->SetBackColor(aColor);
		}

		if (mpPage->GetFontColorFloatWin()->IsInPopupMode() )
			mpPage->GetFontColorFloatWin()->EndPopupMode();
	}
	return( 0L );
}

} // end of namespace sidebar
