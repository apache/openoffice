#include <svtools/button.hxx>
#include <limits>

namespace SidebarCustomButton
{
	void GetHighContrastBmp( const Color &rBk, const Color &rFg, const Bitmap &rBmpSrc, Bitmap &rBmpDst )
	{
		//can not use rBmpDst=rBmpSrc
		if ( rBmpDst.GetSizePixel() != rBmpSrc.GetSizePixel() )
		{
			//rBmpDst.SetEmpty();
			rBmpDst.~Bitmap();			
			new( &rBmpDst ) Bitmap( rBmpSrc.GetSizePixel(), /*24*/rBmpSrc.GetBitCount() );
		}

		if( BitmapReadAccess*      pReadAcc = const_cast<Bitmap*>(&rBmpSrc)->AcquireReadAccess() )
		{
			if( BitmapWriteAccess*     pWriteAcc = rBmpDst.AcquireWriteAccess() )
			{
				const long	nWidth = pReadAcc->Width();
				const long	nHeight = pReadAcc->Height();
				sal_Bool bDstHasPalette = pWriteAcc->HasPalette();

				for ( long i=0L; i<nWidth; i++ )
				{
					for ( long j=0L; j<nHeight; j++ )
					{
						//if( bDstHasPalette )
						//{
						//	pWriteAcc->SetPixel( j, i, pReadAcc->GetPixel( j, i ) );
						//	pWriteAcc->SetPaletteColor( pReadAcc->GetPixel( j, i ).GetIndex(), GetHighContrastColor( rBk, rFg, pReadAcc->GetColor(j,i) ) );
						//}
						//else
						pWriteAcc->SetPixel( j, i, GetHighContrastColor( rBk, rFg, pReadAcc->GetColor(j,i) ) );
					}
				}

				rBmpDst.ReleaseAccess( pWriteAcc );
			}

			const_cast<Bitmap*>(&rBmpSrc)->ReleaseAccess( pReadAcc );
		}
	}

	ImageList::ImageList( const Bitmap& rBitmap, const Color& rColor, sal_uInt16 nInitSize )
	{
		Size sz = rBitmap.GetSizePixel();

		for ( sal_Int16 i=0; i<nInitSize; i++ )
		{
			Size szTmp( sz.Width()/nInitSize, sz.Height() );
			Bitmap aBmp( szTmp, rBitmap.GetBitCount() );
			aBmp.CopyPixel( Rectangle( Point(0,0 ), szTmp ), Rectangle( Point(szTmp.Width()*i,0 ), szTmp ), &rBitmap );
			this->AddImage( i+1, Image( aBmp, rColor ) );
		}
	}

	Image ImageListH::GetImage( sal_uInt16 nId )
	{
		const StyleSettings & rSettings = Application::GetSettings().GetStyleSettings();
		const Color & rcrBk = rSettings.GetHighContrastBkColor();
		const Color & rcrFg = rSettings.GetHighContrastFgColor();

		if( rcrBk!=m_crBk || rcrFg!=m_crFg )
		{
			m_crBk = rcrBk;
			m_crFg = rcrFg;

			if( !m_anOrgBmp.IsEmpty() )
			{
				Bitmap aBmpTmp;
				GetHighContrastBmp( m_crBk, m_crFg, m_anOrgBmp, aBmpTmp );
				Bitmap aSavedBmp = m_anOrgBmp;
				sal_uInt16 nCount = GetImageCount();
				this->~ImageListH();
				new( const_cast<ImageListH*>(this) ) ImageListH( aBmpTmp, GetHighContrastColor( rcrBk, rcrFg, Color(255,0,255 ) ), nCount );
				m_crBk = rcrBk;
				m_crFg = rcrFg;
				m_anOrgBmp = aSavedBmp;
			}
		}

		return ImageList::GetImage( nId );
	}
	

	BitmapH::operator const Bitmap &() const
	{
		if ( m_aBmpH.IsEmpty() )
		{
			m_aBmpH = m_anOrgBmp;
			m_crBk = Color(0,0,0);
			m_crFg = Color(255,255,255);
		}

		const StyleSettings & rSettings = Application::GetSettings().GetStyleSettings();
		const Color & rcrBk = rSettings.GetHighContrastBkColor();
		const Color & rcrFg = rSettings.GetHighContrastFgColor();

		if( rcrBk!=m_crBk || rcrFg!=m_crFg )
		{
			m_crBk = rcrBk;
			m_crFg = rcrFg;
			GetHighContrastBmp( m_crBk, m_crFg, m_anOrgBmp, m_aBmpH );
		}

		return m_aBmpH;
	}

	BitmapH::operator const Bitmap&()
	{
		return *static_cast<const BitmapH*>(this);
	}

	BitmapH::operator Bitmap&()
	{
		return m_anOrgBmp;
	}

	ImageH::operator const Image&() const
	{
		if ( m_anImageH.GetBitmapEx().IsEmpty() )
		{
			m_anImageH = m_anImage;
			m_crBk = Color(0,0,0);
			m_crFg = Color(255,255,255);
		}

		const StyleSettings & rSettings = Application::GetSettings().GetStyleSettings();
		const Color & rcrBk = rSettings.GetHighContrastBkColor();
		const Color & rcrFg = rSettings.GetHighContrastFgColor();

		if( rcrBk!=m_crBk || rcrFg!=m_crFg )
		{
			m_crBk = rcrBk;
			m_crFg = rcrFg;
			Bitmap aBmpTmp;
			//not use GetHighContrastBmp( m_crBk, m_crFg, m_anImage.GetBitmap(), aBmpTmp );, will cause pWriteAcc->HasPalette() problem
			GetHighContrastBmp( m_crBk, m_crFg, m_anOrgBmp, aBmpTmp );
			m_anImageH = Image( aBmpTmp, GetHighContrastColor( m_crBk, m_crFg, Color(255,0,255) ) );
		}


		return m_anImageH;
	}

	ImageH::operator const Image &()
	{
		return *static_cast<const ImageH*>(this);
	}

	sal_Bool BitmapButton::CustomDraw( sal_uInt16 nButtonStyle, ControlState nState )
	{
		
		//	PushButton::Paint( rRect );
		Size sz = m_szProperSizePixel;
		sal_uInt16 nId = 0U;

		if( ~nState & CTRL_STATE_ENABLED )
			nId = 4U;
		else if ( nState & CTRL_STATE_ROLLOVER )
			if ( nState & CTRL_STATE_PRESSED )
				nId = 3U;
			else
				nId = 2u;
		else
			nId = 1u;

		//SCTRACEA( __FUNCTION__"nId=%d", static_cast<int>(nId) );

		SetFillColor( GetBkColor() );
		SetLineColor( GetBkColor() );
		DrawRect( Rectangle( m_pt4Img, sz ) );
		DrawImage( m_pt4Img, sz, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? m_anImgList_h.GetImage( nId ) : m_anImgList.GetImage( nId ) );

		if( nState & CTRL_STATE_FOCUSED )
		{
			Rectangle rcFocus( m_pt4Img, sz );
			rcFocus.Left()++, rcFocus.Top()++, rcFocus.Right()--, rcFocus.Bottom()--;
			SetFocusRect( rcFocus );
			ShowFocus( GetFocusRect() );
		}

		return sal_True;
	}

	BitmapButton::BitmapButton( Window *pParent, const ResId& rResId, const Bitmap &rBmp, const Bitmap & rBmp_h )
		:BitmapButtonBase( pParent, rResId ), m_pt4Img( 0, 0 ), m_anImgList( rBmp, Color( 255, 0, 255 ), 4 ), m_anImgList_h( rBmp_h, Color( 255, 0, 255 ), 4 ),
		m_crBkGround( COL_TRANSPARENT )
	{
		m_szProperSizePixel = rBmp.GetSizePixel();
		m_szProperSizePixel.Width() /= 4;

		SetFocusRect( Rectangle(Point(0,0), GetProperSizePixel() ) );
	}

	//=============================  Hover Button  ===============================
//	
	HoverButton::HoverButton( Window *pParent, const ResId& rResId, const Bitmap &rBmp, const Bitmap &rBmp_h ):
			BitmapButton( pParent,rResId, rBmp, rBmp_h)
	{		
	}

	sal_Bool HoverButton::CustomDraw( sal_uInt16 nButtonStyle, ControlState nState )
	{
		Size sz = GetSizePixel();
		Rectangle rc( Point(0,0), sz );
		Color crFillOld = GetFillColor();
		Color crLineOld = GetLineColor();
		sal_Bool bMouseIn = IsMouseOver();
		Color aBkColor = bMouseIn ? m_HoverBkGround : m_defBkGround;		
		SetFillColor( aBkColor );
		SetLineColor( m_defBkGround );
		DrawRect( rc );
				
		/*
		SetLineColor( aBkColor );
		DrawRect( Rectangle( m_pt4Img, m_ImgSizePixel ) );
		DrawImage( m_pt4Img, m_ImgSizePixel, Application::GetSettings().GetStyleSettings().GetHighContrastMode()? m_hImg : m_defImg );*/
		BitmapButton::CustomDraw( nButtonStyle, nState );

		sz = BitmapButton::GetProperSizePixel();
		const Point & rPt4Img = GetPt4Img();
		long nTxtHeight = GetTextHeight();
		Point aTxtPt( rPt4Img.X() + sz.Width() + 3, rPt4Img.Y() + sz.Height()/2 - nTxtHeight/2 - 1 );

		Color crOldText = GetTextColor();
		if( bMouseIn )	SetTextColor( m_HoverTxtColor );
		DrawText( Rectangle(aTxtPt, Point( rPt4Img.X() + rc.getWidth() -3, aTxtPt.Y() + nTxtHeight ) ), GetText(), 
			TEXT_DRAW_ENDELLIPSIS|TEXT_DRAW_MNEMONIC|(IsEnabled()?0:TEXT_DRAW_DISABLE) );	//sym2_3092
		

		if( nState & CTRL_STATE_FOCUSED )
		{
			Size aFocusSize = CalcMinimumSize();
			aFocusSize.Width() += 3;
			Rectangle rcFocus( rPt4Img, aFocusSize );
			rcFocus.Left()++, rcFocus.Top()++, rcFocus.Right()--, rcFocus.Bottom()--;
			SetFocusRect( rcFocus );
			ShowFocus( GetFocusRect() );
		}
		
		SetTextColor( crOldText );
		SetFillColor( crFillOld );
		SetLineColor( crLineOld );
		return sal_True;
	}

	void HoverButton::SetIcoPosX( long nX)
	{
		//long nY = (GetOutputSizePixel().Height() - m_ImgSizePixel.Height()) / 2;		

		long nY = (GetProperSizePixel().Height() - BitmapButton::GetProperSizePixel().Height()) / 2;
		SetPt4Img( Point( nX, nY ) );
	}

}
