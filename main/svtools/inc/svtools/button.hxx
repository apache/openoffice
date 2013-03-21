#include <vcl/button.hxx>
#include <vcl/svapp.hxx>
#include <vcl/bmpacc.hxx>
#include "svtools/svtdllapi.h"

namespace SidebarCustomButton
{
	typedef ::PushButton BitmapButtonBase;

	#define GetHighContrastBkColor	GetWindowColor
	#define GetHighContrastFgColor	GetWindowTextColor
	
	template<class TPDst>
	TPDst type_cast( ... )
	{
		return NULL;
	}

	template<class TPDst>
	TPDst type_cast( const TPDst pSrc )
	{
		return static_cast<TPDst>( pSrc );
	}

	inline const Color GetHighContrastColor( const Color &rBkColor, const Color &rFgColor, const Color & rBlackWhiteColor )
	{
		typedef long double LONGDOUBLE;
		return Color( 
			LONGDOUBLE(rBlackWhiteColor.GetRed())/255*rFgColor.GetRed() + (1-LONGDOUBLE(rBlackWhiteColor.GetRed())/255)*rBkColor.GetRed() + 0.5,
			LONGDOUBLE(rBlackWhiteColor.GetGreen())/255*rFgColor.GetGreen() + (1-LONGDOUBLE(rBlackWhiteColor.GetGreen())/255)*rBkColor.GetGreen() + 0.5,
			LONGDOUBLE(rBlackWhiteColor.GetBlue())/255*rFgColor.GetBlue() + (1-LONGDOUBLE(rBlackWhiteColor.GetBlue())/255)*rBkColor.GetBlue() + 0.5
			);
	}

	#define CHKPARAM(r)	if( const Bitmap *pBmp = type_cast<const Bitmap*>(&r) ){ if(m_anOrgBmp.IsEmpty()) m_anOrgBmp = *pBmp; } else if( const ResId *pResId = type_cast<const ResId*>(&r) ){ if(m_anOrgBmp.IsEmpty()) m_anOrgBmp = *pResId; }
	#define PASS_CONSTRUCT_PARAMETER(theClass, baseClass)	\
		theClass():m_crBk(0,0,0),m_crFg(255,255,255){}\
		template< class T1 >\
		theClass( T1 & rt1 ):baseClass( rt1 ),m_crBk(0,0,0),m_crFg(255,255,255){CHKPARAM(rt1)}\
		template< class T1 >\
		theClass( const T1 & rt1 ):baseClass( rt1 ),m_crBk(0,0,0),m_crFg(255,255,255){CHKPARAM(rt1)}\
		template< class T1, class T2 >\
		theClass( T1 & rt1, T2 & rt2 ):baseClass( rt1, rt2 ),m_crBk(0,0,0),m_crFg(255,255,255){CHKPARAM(rt1)CHKPARAM(rt2)}\
		template< class T1, class T2 >\
		theClass( const T1 & rt1,const T2 & rt2 ):baseClass( rt1, rt2 ),m_crBk(0,0,0),m_crFg(255,255,255){CHKPARAM(rt1)CHKPARAM(rt2)}\
		template< class T1, class T2, class T3 >\
		theClass( T1 & rt1, T2 &rt2, T3 & rt3 ):baseClass( rt1, rt2, rt3 ),m_crBk(0,0,0),m_crFg(255,255,255){CHKPARAM(rt1)CHKPARAM(rt2)CHKPARAM(rt3)}\
		template< class T1, class T2, class T3 >\
		theClass( const T1 & rt1, const T2 &rt2, const T3 & rt3 ):baseClass( rt1, rt2, rt3 ),m_crBk(0,0,0),m_crFg(255,255,255){CHKPARAM(rt1)CHKPARAM(rt2)CHKPARAM(rt3)}

	struct ImageH
	{
		Bitmap m_anOrgBmp;
		Image m_anImage;
		mutable Image m_anImageH;
		mutable Color m_crBk,m_crFg;
		PASS_CONSTRUCT_PARAMETER( ImageH, m_anImage )
		operator const Image &() const;
		operator const Image &();
	};

	struct BitmapH
	{
		Bitmap m_anOrgBmp;
		mutable Bitmap m_aBmpH;
		mutable Color m_crBk,m_crFg;
		PASS_CONSTRUCT_PARAMETER( BitmapH, m_anOrgBmp )
		operator const Bitmap&() const;
		operator const Bitmap&();
		operator Bitmap&();
		sal_Bool IsEmpty(){ return m_anOrgBmp.IsEmpty();	}
		void Mirror( sal_uInt64 nFlags ){ m_anOrgBmp.Mirror( nFlags );	}
	};
		
	struct ImageList: ::ImageList
	{
		ImageList( const Bitmap& rBitmap, const Color& rColor,
			sal_uInt16 nInitSize );
		ImageList(){}
	};

	struct ImageListH:public ImageList
	{
		Bitmap m_anOrgBmp;
		mutable Color m_crBk,m_crFg;
		PASS_CONSTRUCT_PARAMETER( ImageListH, ImageList )
		Image GetImage( sal_uInt16 nId );
	};		
		
	

	class SVT_DLLPUBLIC BitmapButton:public BitmapButtonBase
	{
	private:
		ImageList m_anImgList;
		ImageListH m_anImgList_h;
		Point	m_pt4Img;
		Size m_szProperSizePixel;
		Color	m_crBkGround;
		// Copy assignment is forbidden and not implemented.
		SVT_DLLPRIVATE          BitmapButton( const BitmapButton & );
		SVT_DLLPRIVATE          BitmapButton & operator= ( const BitmapButton & );
	protected:
		void	SetPt4Img( const Point &rNewPt ){	m_pt4Img = rNewPt; Rectangle rc = GetFocusRect();	rc.SetPos( rNewPt ); SetFocusRect( rc );	}
		const Point &GetPt4Img(){	return m_pt4Img; }
		const Color & GetBkColor(){	return m_crBkGround;	}
		void	SetBkColor( const Color & rColor ){ m_crBkGround = rColor;	}
	public:
		BitmapButton( Window *pParent, const ResId& rResId, const Bitmap &rBmp, const Bitmap &rBmp_h );
		//void	Paint( const Rectangle &rRect );
		virtual sal_Bool	CustomDraw( sal_uInt16 nButtonStyle, ControlState nState );
		virtual Size	GetProperSizePixel(){	return m_szProperSizePixel; }

		sal_Bool	IsMouseIn()
		{
			return IsMouseOver();//Rectangle( GetPosPixel(), GetOutputSizePixel() ).IsInside( GetPointerPosPixel() );
		}
	};

	class SVT_DLLPUBLIC HoverButton:public BitmapButton
	{
		Color	m_defBkGround;
		Color	m_HoverBkGround;
		Color	m_HoverTxtColor;
//		Image	m_defImg;
//		Image	m_hImg;
//		Point	m_pt4Img;
//		Size	m_ImgSizePixel;

		// Copy assignment is forbidden and not implemented.
		SVT_DLLPRIVATE          HoverButton( const HoverButton & );
		SVT_DLLPRIVATE          HoverButton & operator= ( const HoverButton & );

	public:	
		HoverButton( Window *pParent, const ResId& rResId, const Bitmap &rBmp, const Bitmap &rBmp_h );
		virtual Size	GetProperSizePixel(){	return  GetOutputSizePixel();}
		virtual sal_Bool	CustomDraw( sal_uInt16 nButtonStyle, ControlState nState );
		void	SetIcoPosX( long nX);
		void	SetDefBkColor ( const Color & rColor ){ m_defBkGround = rColor;	} //User must set this color
		void	SetHoverBkColor ( const Color & rColor ){ m_HoverBkGround = rColor;	}//User must set this color
		void	SetHoverTxtColor ( const Color & rColor ){ m_HoverTxtColor = rColor;	}//User must set this color
	};

}

