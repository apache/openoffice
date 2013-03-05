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
#include <TransformationPropertyPanel.hxx>
#include <TransformationPropertyPanel.hrc>
#include <svx/dialogs.hrc>
#include <svx/dialmgr.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/viewsh.hxx>
#include <sfx2/objsh.hxx>
#include <svx/dlgutil.hxx>
#include <unotools/viewoptions.hxx>
#include <vcl/virdev.hxx>
#include <vcl/svapp.hxx>
#include <vcl/field.hxx>
#include <vcl/fixed.hxx>
#include <vcl/toolbox.hxx>
#include <svx/svdview.hxx>
#include <svl/aeitem.hxx>

using namespace css;
using namespace cssu;
using ::sfx2::sidebar::Theme;

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))
#define USERITEM_NAME rtl::OUString::createFromAscii("FitItem")
#define NO_SELECT       (65535)



namespace
{
    const long DIAL_OUTER_WIDTH = 8;

    Image& getDialControlImage(bool bHighContrast)
    {
        static Image imgDiaCtrlBkNormal;
        static Image imgDiaCtrlBkH;

        if(bHighContrast)
        {
            if(!imgDiaCtrlBkH.GetSizePixel().Width())
            {
                imgDiaCtrlBkH = Image(SVX_RES(IMG_DIACONTROL_NORMAL));
            }

            return imgDiaCtrlBkH;
        }
        else
        {
            if(!imgDiaCtrlBkNormal.GetSizePixel().Width())
            {
                imgDiaCtrlBkNormal = Image(SVX_RES(IMG_DIACONTROL_H));
            }

            return imgDiaCtrlBkNormal;
        }
    }

    class DialControlBmp : public VirtualDevice
    {
    public:
        explicit            DialControlBmp( Window& rParent, bool bForSideBar = false );

        void                InitBitmap( const Size& rSize, const Font& rFont );
        void                CopyBackground( const DialControlBmp& rSrc );
        void                DrawBackground( const Size& rSize, bool bEnabled );
        void                DrawElements( const String& rText, sal_Int32 nAngle );

    private:
        const Color&        GetBackgroundColor() const;
        const Color&        GetTextColor() const;
        const Color&        GetScaleLineColor() const;
        const Color&        GetButtonLineColor() const;
        const Color&        GetButtonFillColor( bool bMain ) const;

        void                Init( const Size& rSize );
        void                DrawBackground();

        Window&             mrParent;
        Rectangle           maRect;
        long                mnCenterX;
        long                mnCenterY;
        bool                mbEnabled;
	    bool                mbForSideBar;
    };

    DialControlBmp::DialControlBmp( Window& rParent, bool bForSideBar ) :
        VirtualDevice( rParent, 0, 0 ),
        mrParent( rParent ),
        mbEnabled( true ),
	    mbForSideBar( bForSideBar )
    {
        EnableRTL( false );
    }

    void DialControlBmp::InitBitmap( const Size& rSize, const Font& rFont )
    {
        Init( rSize );
        SetFont( rFont );
    }

    void DialControlBmp::CopyBackground( const DialControlBmp& rSrc )
    {
        Init( rSrc.maRect.GetSize() );
        mbEnabled = rSrc.mbEnabled;
        Point aPos;
        DrawBitmapEx( aPos, rSrc.GetBitmapEx( aPos, maRect.GetSize() ) );
    }

    void DialControlBmp::DrawBackground( const Size& rSize, bool bEnabled )
    {
        Init( rSize );
        mbEnabled = bEnabled;
        DrawBackground();
    }

    void DialControlBmp::DrawElements( const String& rText, sal_Int32 nAngle )
    {
        // *** rotated text ***
        if (!mbForSideBar)
	    {
		    sal_Int32 nTmpAngle = nAngle;
		    if (Application::GetSettings().GetLayoutRTL())		
			    nAngle = 36000 - nAngle;

		    Font aFont( GetFont() );
		    aFont.SetColor( GetTextColor() );
		    aFont.SetOrientation( static_cast< short >( (nAngle + 5) / 10 ) );  // Font uses 1/10 degrees
		    aFont.SetWeight( WEIGHT_BOLD );
		    SetFont( aFont );

		    double fAngle = nAngle * F_PI180 / 100.0;
		    double fSin = sin( fAngle );
		    double fCos = cos( fAngle );
		    double fWidth = GetTextWidth( rText ) / 2.0;
		    double fHeight = GetTextHeight() / 2.0;
		    long nX = static_cast< long >( mnCenterX - fWidth * fCos - fHeight * fSin );
		    long nY = static_cast< long >( mnCenterY + fWidth * fSin - fHeight * fCos );
		    Rectangle aRect( nX, nY, 2 * mnCenterX - nX, 2 * mnCenterY - nY );
		    DrawText( aRect, rText, mbEnabled ? 0 : TEXT_DRAW_DISABLE );

		    // *** drag button ***
		    if (Application::GetSettings().GetLayoutRTL())
		    {
			    nAngle = nTmpAngle;
			    nAngle = 18000 - nAngle;
			    fAngle = nAngle * F_PI180 / 100.0;
			    fSin = sin( fAngle );
			    fCos = cos( fAngle );
		    }

		    bool bMain = (nAngle % 4500) != 0;
		    SetLineColor( GetButtonLineColor() );
		    SetFillColor( GetButtonFillColor( bMain ) );

		    nX = mnCenterX - static_cast< long >( (DIAL_OUTER_WIDTH / 2 - mnCenterX) * fCos );
		    nY = mnCenterY - static_cast< long >( (mnCenterY - DIAL_OUTER_WIDTH / 2) * fSin );
		    long nSize = bMain ? (DIAL_OUTER_WIDTH / 4) : (DIAL_OUTER_WIDTH / 2 - 1);
		    DrawEllipse( Rectangle( nX - nSize, nY - nSize, nX + nSize, nY + nSize ) );
	    }
        else
	    {
		    if (Application::GetSettings().GetLayoutRTL())		
			    nAngle = 18000 - nAngle;
		    double fAngle = nAngle * F_PI180 / 100.0;
		    double fSin = sin( fAngle );
		    double fCos = cos( fAngle );
		    DrawText( maRect, String(), mbEnabled ? 0 : TEXT_DRAW_DISABLE );
		    Point pt1( maRect.Center() );
		    Point pt2( pt1.X() + fCos * (maRect.GetWidth()-4)/2, pt1.Y() + -fSin * (maRect.GetHeight()-4) / 2 );
		    // for high contrast
		    if(!::Application::GetSettings().GetStyleSettings().GetHighContrastMode())
			    SetLineColor( Color( 60, 93, 138 ) );
		    else
			    SetLineColor(COL_BLACK);//Application::GetSettings().GetStyleSettings().GetFieldTextColor()
		    DrawLine( pt1, pt2 );
	    }
    }

    const Color& DialControlBmp::GetBackgroundColor() const
    {
        return GetSettings().GetStyleSettings().GetDialogColor();
    }

    const Color& DialControlBmp::GetTextColor() const
    {
        return GetSettings().GetStyleSettings().GetLabelTextColor();
    }

    const Color& DialControlBmp::GetScaleLineColor() const
    {
        const StyleSettings& rSett = GetSettings().GetStyleSettings();
        return mbEnabled ? rSett.GetButtonTextColor() : rSett.GetDisableColor();
    }

    const Color& DialControlBmp::GetButtonLineColor() const
    {
        const StyleSettings& rSett = GetSettings().GetStyleSettings();
        return mbEnabled ? rSett.GetButtonTextColor() : rSett.GetDisableColor();
    }

    const Color& DialControlBmp::GetButtonFillColor( bool bMain ) const
    {
        const StyleSettings& rSett = GetSettings().GetStyleSettings();
        return mbEnabled ? (bMain ? rSett.GetMenuColor() : rSett.GetHighlightColor()) : rSett.GetDisableColor();
    }

    void DialControlBmp::Init( const Size& rSize )
    {
        SetSettings( mrParent.GetSettings() );
        maRect.SetPos( Point( 0, 0 ) );
        maRect.SetSize( rSize );
        mnCenterX = rSize.Width() / 2;
        mnCenterY = rSize.Height() / 2;
        SetOutputSize( rSize );
        SetBackground();
    }

    void DialControlBmp::DrawBackground()
    {
        // *** background with 3D effect ***

        SetLineColor();
        SetFillColor();
        Erase();

        if (!mbForSideBar)
	    {
		    EnableRTL( true ); // #107807# draw 3D effect in correct direction

		    sal_uInt8 nDiff = mbEnabled ? 0x18 : 0x10;
		    Color aColor;

		    aColor = GetBackgroundColor();
		    SetFillColor( aColor );
		    DrawPie( maRect, maRect.TopRight(), maRect.TopCenter() );
		    DrawPie( maRect, maRect.BottomLeft(), maRect.BottomCenter() );

		    aColor.DecreaseLuminance( nDiff );
		    SetFillColor( aColor );
		    DrawPie( maRect, maRect.BottomCenter(), maRect.TopRight() );

		    aColor.DecreaseLuminance( nDiff );
		    SetFillColor( aColor );
		    DrawPie( maRect, maRect.BottomRight(), maRect.RightCenter() );

		    aColor = GetBackgroundColor();
		    aColor.IncreaseLuminance( nDiff );
		    SetFillColor( aColor );
		    DrawPie( maRect, maRect.TopCenter(), maRect.BottomLeft() );

		    aColor.IncreaseLuminance( nDiff );
		    SetFillColor( aColor );
		    DrawPie( maRect, maRect.TopLeft(), maRect.LeftCenter() );

		    EnableRTL( false );

		    // *** calibration ***

		    Point aStartPos( mnCenterX, mnCenterY );
		    Color aFullColor( GetScaleLineColor() );
		    Color aLightColor( GetBackgroundColor() );
		    aLightColor.Merge( aFullColor, 128 );

		    for( int nAngle = 0; nAngle < 360; nAngle += 15 )
		    {
			    SetLineColor( (nAngle % 45) ? aLightColor : aFullColor );
			    double fAngle = nAngle * F_PI180;
			    long nX = static_cast< long >( -mnCenterX * cos( fAngle ) );
			    long nY = static_cast< long >( mnCenterY * sin( fAngle ) );
			    DrawLine( aStartPos, Point( mnCenterX - nX, mnCenterY - nY ) );
		    }

		    // *** clear inner area ***

		    SetLineColor();
		    SetFillColor( GetBackgroundColor() );
		    DrawEllipse( Rectangle( maRect.Left() + DIAL_OUTER_WIDTH, maRect.Top() + DIAL_OUTER_WIDTH,
			    maRect.Right() - DIAL_OUTER_WIDTH, maRect.Bottom() - DIAL_OUTER_WIDTH ) );
	    }
        else
	    {
            const Image& rImage = getDialControlImage(::Application::GetSettings().GetStyleSettings().GetHighContrastMode());

            DrawImage(maRect.TopLeft(), maRect.GetSize(), rImage);
	    }
    }
} // end of anonymous namespace



namespace
{
    struct DialControl_Impl
    {
        DialControlBmp      maBmpEnabled;
        DialControlBmp      maBmpDisabled;
        DialControlBmp      maBmpBuffered;
        Link                maModifyHdl;
        NumericField*       mpLinkField;
        Size                maWinSize;
        Font                maWinFont;
        sal_Int32           mnAngle;
        sal_Int32           mnOldAngle;
        long                mnCenterX;
        long                mnCenterY;
        bool                mbNoRot;
	    bool                mbForSideBar;

        explicit            DialControl_Impl( Window& rParent, bool bForSideBar = false );
        void                Init( const Size& rWinSize, const Font& rWinFont );
    };

    DialControl_Impl::DialControl_Impl( Window& rParent, bool bForSideBar ) :
        maBmpEnabled( rParent, bForSideBar ),
        maBmpDisabled( rParent, bForSideBar ),
        maBmpBuffered( rParent, bForSideBar ),
        mpLinkField( 0 ),
        mnAngle( 0 ),
        mbNoRot( false ),
	    mbForSideBar( bForSideBar )
    {
    }

    void DialControl_Impl::Init( const Size& rWinSize, const Font& rWinFont )
    {
        // "(x - 1) | 1" creates odd value <= x, to have a well-defined center pixel position
        maWinSize = Size( (rWinSize.Width() - 1) | 1, (rWinSize.Height() - 1) | 1 );
        maWinFont = rWinFont;

        mnCenterX = maWinSize.Width() / 2;
        mnCenterY = maWinSize.Height() / 2;
        maWinFont.SetTransparent( true );

        maBmpEnabled.DrawBackground( maWinSize, true );
        maBmpDisabled.DrawBackground( maWinSize, false );
        maBmpBuffered.InitBitmap( maWinSize, maWinFont );
    }
} // end of anonymous namespace



/** This control allows to input a rotation angle, visualized by a dial.

    Usage: A single click sets a rotation angle rounded to steps of 15 degrees.
    Dragging with the left mouse button sets an exact rotation angle. Pressing
    the ESCAPE key during mouse drag cancels the operation and restores the old
    state of the control.

    It is possible to link a numeric field to this control using the function
    SetLinkedField(). The DialControl will take full control of this numeric
    field:
    -   Sets the rotation angle to the numeric field in mouse operations.
    -   Shows the value entered/modified in the numeric field.
    -   Enables/disables/shows/hides the field according to own state changes.
    */
class DialControl : public Control
{
public:
    DialControl( Window* pParent, const ResId& rResId, bool bForSideBar = false );
    virtual             ~DialControl();

    virtual void        Paint( const Rectangle& rRect );

    virtual void        StateChanged( StateChangedType nStateChange );
    virtual void        DataChanged( const DataChangedEvent& rDCEvt );

    virtual void        MouseButtonDown( const MouseEvent& rMEvt );
    virtual void        MouseMove( const MouseEvent& rMEvt );
    virtual void        MouseButtonUp( const MouseEvent& rMEvt );
    virtual void        KeyInput( const KeyEvent& rKEvt );
    virtual void        LoseFocus();
	/**For side bar**/
	bool                mbForSideBar;

    /** Returns true, if the control is not in "don't care" state. */
    bool                HasRotation() const;
    /** Sets the control to "don't care" state. */
    void                SetNoRotation();

    /** Returns the current rotation angle in 1/100 degrees. */
    sal_Int32           GetRotation() const;
    /** Sets the rotation to the passed value (in 1/100 degrees). */
    void                SetRotation( sal_Int32 nAngle );

    /** Links the passed numeric edit field to the control (bi-directional). */
    void                SetLinkedField( NumericField* pField );
    /** Returns the linked numeric edit field, or 0. */
    NumericField*       GetLinkedField() const;

    /** The passed handler is called whenever the totation value changes. */
    void                SetModifyHdl( const Link& rLink );
    /** Returns the current modify handler. */
    const Link&         GetModifyHdl() const;

private:
    void                Init( const Size& rWinSize, const Font& rWinFont );
    void                Init( const Size& rWinSize );
    void                InvalidateControl();

    void                ImplSetRotation( sal_Int32 nAngle, bool bBroadcast );
    void                ImplSetFieldLink( const Link& rLink );

    void                HandleMouseEvent( const Point& rPos, bool bInitial );
    void                HandleEscapeEvent();

    DECL_LINK( LinkedFieldModifyHdl, NumericField* );

    std::auto_ptr< DialControl_Impl > mpImpl;
};

DialControl::DialControl( Window* pParent, const ResId& rResId, bool bForSideBar ) :
    Control( pParent, rResId ),
	mbForSideBar(bForSideBar),
    mpImpl( new DialControl_Impl( *this, bForSideBar ) )
{
    Init( GetOutputSizePixel() );
}

DialControl::~DialControl()
{
}

void DialControl::Paint( const Rectangle&  )
{
    Point aPos;
    DrawBitmapEx( aPos, mpImpl->maBmpBuffered.GetBitmapEx( aPos, mpImpl->maWinSize ) );
}

void DialControl::StateChanged( StateChangedType nStateChange )
{
    if( nStateChange == STATE_CHANGE_ENABLE )
        InvalidateControl();

    // update the linked edit field
    if( mpImpl->mpLinkField )
    {
        NumericField& rField = *mpImpl->mpLinkField;
        switch( nStateChange )
        {
            case STATE_CHANGE_VISIBLE:  rField.Show( IsVisible() );     break;
            case STATE_CHANGE_ENABLE:   rField.Enable( IsEnabled() );   break;
        }
    }

    Control::StateChanged( nStateChange );
}

void DialControl::DataChanged( const DataChangedEvent& rDCEvt )
{
    if( (rDCEvt.GetType() == DATACHANGED_SETTINGS) && (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        Init( mpImpl->maWinSize, mpImpl->maWinFont );
        InvalidateControl();
    }
    Control::DataChanged( rDCEvt );
}

void DialControl::MouseButtonDown( const MouseEvent& rMEvt )
{
    if( rMEvt.IsLeft() )
    {
        GrabFocus();
        CaptureMouse();
        mpImpl->mnOldAngle = mpImpl->mnAngle;
        HandleMouseEvent( rMEvt.GetPosPixel(), true );
    }
	if (!mbForSideBar)
		Control::MouseButtonDown( rMEvt );
}

void DialControl::MouseMove( const MouseEvent& rMEvt )
{
    if( IsMouseCaptured() && rMEvt.IsLeft() )
        HandleMouseEvent( rMEvt.GetPosPixel(), false );
    Control::MouseMove(rMEvt );
}

void DialControl::MouseButtonUp( const MouseEvent& rMEvt )
{
    if( IsMouseCaptured() )
    {
        ReleaseMouse();
        if( mpImpl->mpLinkField )
            mpImpl->mpLinkField->GrabFocus();
    }
    Control::MouseButtonUp( rMEvt );
}

void DialControl::KeyInput( const KeyEvent& rKEvt )
{
    const KeyCode& rKCode = rKEvt.GetKeyCode();
    if( !rKCode.GetModifier() && (rKCode.GetCode() == KEY_ESCAPE) )
        HandleEscapeEvent();
    else
        Control::KeyInput( rKEvt );
}

void DialControl::LoseFocus()
{
    // release captured mouse
    HandleEscapeEvent();
    Control::LoseFocus();
}

bool DialControl::HasRotation() const
{
    return !mpImpl->mbNoRot;
}

void DialControl::SetNoRotation()
{
    if( !mpImpl->mbNoRot )
    {
        mpImpl->mbNoRot = true;
        InvalidateControl();
        if( mpImpl->mpLinkField )
            mpImpl->mpLinkField->SetText( String() );
    }
}

sal_Int32 DialControl::GetRotation() const
{
    return mpImpl->mnAngle;
}

void DialControl::SetRotation( sal_Int32 nAngle )
{
    ImplSetRotation( nAngle, false );
}

void DialControl::SetLinkedField( NumericField* pField )
{
    // remove modify handler from old linked field
    ImplSetFieldLink( Link() );
    // remember the new linked field
    mpImpl->mpLinkField = pField;
    // set modify handler at new linked field
    ImplSetFieldLink( LINK( this, DialControl, LinkedFieldModifyHdl ) );
}

NumericField* DialControl::GetLinkedField() const
{
    return mpImpl->mpLinkField;
}

void DialControl::SetModifyHdl( const Link& rLink )
{
    mpImpl->maModifyHdl = rLink;
}

const Link& DialControl::GetModifyHdl() const
{
    return mpImpl->maModifyHdl;
}

void DialControl::Init( const Size& rWinSize, const Font& rWinFont )
{
    mpImpl->Init( rWinSize, rWinFont );
    EnableRTL( false ); // #107807# don't mirror mouse handling
    SetOutputSizePixel( mpImpl->maWinSize );
    SetBackground();
}

void DialControl::Init( const Size& rWinSize )
{
    Font aFont( OutputDevice::GetDefaultFont(
        DEFAULTFONT_UI_SANS, Application::GetSettings().GetUILanguage(), DEFAULTFONT_FLAGS_ONLYONE ) );
    Init( rWinSize, aFont );
}

void DialControl::InvalidateControl()
{
    mpImpl->maBmpBuffered.CopyBackground( IsEnabled() ? mpImpl->maBmpEnabled : mpImpl->maBmpDisabled );
    if( !mpImpl->mbNoRot )
        mpImpl->maBmpBuffered.DrawElements( GetText(), mpImpl->mnAngle );
    Invalidate();
}

void DialControl::ImplSetRotation( sal_Int32 nAngle, bool bBroadcast )
{
    bool bOldSel = mpImpl->mbNoRot;
    mpImpl->mbNoRot = false;

    while( nAngle < 0 ) nAngle += 36000;
    nAngle = (((nAngle + 50) / 100) * 100) % 36000;
    if( !bOldSel || (mpImpl->mnAngle != nAngle) )
    {
        mpImpl->mnAngle = nAngle;
        InvalidateControl();
        if( mpImpl->mpLinkField )
            mpImpl->mpLinkField->SetValue( static_cast< long >( GetRotation() / 100 ) );
        if( bBroadcast )
            mpImpl->maModifyHdl.Call( this );
    }
}

void DialControl::ImplSetFieldLink( const Link& rLink )
{
    if( mpImpl->mpLinkField )
    {
        NumericField& rField = *mpImpl->mpLinkField;
        rField.SetModifyHdl( rLink );
        rField.SetUpHdl( rLink );
        rField.SetDownHdl( rLink );
        rField.SetFirstHdl( rLink );
        rField.SetLastHdl( rLink );
        rField.SetLoseFocusHdl( rLink );
    }
}

void DialControl::HandleMouseEvent( const Point& rPos, bool bInitial )
{
    long nX = rPos.X() - mpImpl->mnCenterX;
    long nY = mpImpl->mnCenterY - rPos.Y();
    double fH = sqrt( static_cast< double >( nX ) * nX + static_cast< double >( nY ) * nY );
    if( fH != 0.0 )
    {
        double fAngle = acos( nX / fH );
        sal_Int32 nAngle = static_cast< sal_Int32 >( fAngle / F_PI180 * 100.0 );
        if( nY < 0 )
            nAngle = 36000 - nAngle;
        if( bInitial )  // round to entire 15 degrees
            nAngle = ((nAngle + 750) / 1500) * 1500;

		if (Application::GetSettings().GetLayoutRTL())
			nAngle = 18000 - nAngle;
        ImplSetRotation( nAngle, true );
    }
}

void DialControl::HandleEscapeEvent()
{
    if( IsMouseCaptured() )
    {
        ReleaseMouse();
        ImplSetRotation( mpImpl->mnOldAngle, true );
        if( mpImpl->mpLinkField )
            mpImpl->mpLinkField->GrabFocus();
    }
}

IMPL_LINK( DialControl, LinkedFieldModifyHdl, NumericField*, pField )
{
    if( pField )
        ImplSetRotation( static_cast< sal_Int32 >( pField->GetValue() * 100 ), false );
    return 0;
}


// namespace open

namespace svx { namespace sidebar {



TransformationPropertyPanel::TransformationPropertyPanel(
    Window* pParent,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings)
:   Control(
        pParent, 
        SVX_RES(RID_SIDEBAR_TRANSFORMATION_PANEL)),
    mpFtPosX(new FixedText(this, SVX_RES(FT_SBSHAPE_HORIZONTAL))),
    mpMtrPosX(new MetricField(this, SVX_RES(MF_SBSHAPE_HORIZONTAL))),
    mpFtPosY(new FixedText(this, SVX_RES(FT_SBSHAPE_VERTICAL))),
    mpMtrPosY(new MetricField(this, SVX_RES(MF_SBSHAPE_VERTICAL))),
    mpFtWidth(new FixedText(this, SVX_RES(FT_WIDTH))),
    mpMtrWidth(new MetricField(this, SVX_RES(MTR_FLD_WIDTH))),
    mpFtHeight(new FixedText(this, SVX_RES(FT_HEIGHT))),
    mpMtrHeight(new MetricField(this, SVX_RES(MTR_FLD_HEIGHT))),
    mpCbxScale(new CheckBox(this, SVX_RES(CBX_SCALE))),
    mpFtAngle(new FixedText(this, SVX_RES(FT_ANGLE))),
    mpMtrAngle(new MetricBox(this, SVX_RES(MTR_FLD_ANGLE))),
    mpDial(new DialControl(this, SVX_RES(DIAL_CONTROL), true)),
    mpFtFlip(new FixedText(this, SVX_RES(FT_FLIP))),
    mpFlipTbxBackground(sfx2::sidebar::ControlFactory::CreateToolBoxBackground(this)),
    mpFlipTbx(sfx2::sidebar::ControlFactory::CreateToolBox(mpFlipTbxBackground.get(), SVX_RES(TBX_FLIP))),
    maRect(),
    mpView(0),
    mlOldWidth(1),
    mlOldHeight(1),
    meRP(RP_LT),
    maAnchorPos(),
    mlRotX(0),
    mlRotY(0),
    maUIScale(),
    mePoolUnit(),
    meDlgUnit(),
    maTransfPosXControl(SID_ATTR_TRANSFORM_POS_X, *pBindings, *this),
    maTransfPosYControl(SID_ATTR_TRANSFORM_POS_Y, *pBindings, *this),
    maTransfWidthControl(SID_ATTR_TRANSFORM_WIDTH, *pBindings, *this),
    maTransfHeightControl(SID_ATTR_TRANSFORM_HEIGHT, *pBindings, *this),
    maTransAffineMatrix2DControl(SID_ATTR_TRANSFORM_MATRIX, *pBindings, *this),
    maSvxAngleControl( SID_ATTR_TRANSFORM_ANGLE, *pBindings, *this),
    maRotXControl(SID_ATTR_TRANSFORM_ROT_X, *pBindings, *this),
    maRotYControl(SID_ATTR_TRANSFORM_ROT_Y, *pBindings, *this),
    maProPosControl(SID_ATTR_TRANSFORM_PROTECT_POS, *pBindings, *this),
    maProSizeControl(SID_ATTR_TRANSFORM_PROTECT_SIZE, *pBindings, *this),
    maAutoWidthControl(SID_ATTR_TRANSFORM_AUTOWIDTH, *pBindings, *this),
    maAutoHeightControl(SID_ATTR_TRANSFORM_AUTOHEIGHT, *pBindings, *this),
    m_aMetricCtl(SID_ATTR_METRIC, *pBindings, *this),
    maImgFlipHori(SVX_RES(IMG_HORI_FLIP)),
    maImgFlipVert(SVX_RES(IMG_VERT_FLIP)),
    mxFrame(rxFrame),
    maContext(),
    mpBindings(pBindings),
    mbMtrPosXMirror(false),
    mbSizeProtected(false),
    mbPositionProtected(false),
    mbAutoWidth(false),
    mbAutoHeight(false),
    mbAdjustEnabled(false),
    mbIsFlip(false),
    mbInDestructor(false)
{
	Initialize();
	FreeResource();	
}



TransformationPropertyPanel::~TransformationPropertyPanel()
{
    mbInDestructor = true;

    // Destroy the background windows of the toolboxes.
    mpFlipTbx.reset();
    mpFlipTbxBackground.reset();
}



void TransformationPropertyPanel::ShowMenu (void)
{
    if (mpBindings != NULL)
    {
        SfxDispatcher* pDispatcher = mpBindings->GetDispatcher();
        if (pDispatcher != NULL)
            pDispatcher->Execute(SID_ATTR_TRANSFORM, SFX_CALLMODE_ASYNCHRON);
    }
}



void TransformationPropertyPanel::Initialize()
{
	//Position : Horizontal / Vertical
	mpMtrPosX->SetModifyHdl( LINK( this, TransformationPropertyPanel, ChangePosXHdl ) );
	mpMtrPosY->SetModifyHdl( LINK( this, TransformationPropertyPanel, ChangePosYHdl ) );
	mpMtrPosX->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Horizontal")));	//wj acc
	mpMtrPosY->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Vertical")));		//wj acc
	
    //Size : Width / Height
	mpMtrWidth->SetModifyHdl( LINK( this, TransformationPropertyPanel, ChangeWidthHdl ) );
	mpMtrHeight->SetModifyHdl( LINK( this, TransformationPropertyPanel, ChangeHeightHdl ) );
	mpMtrWidth->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Width")));	//wj acc
	mpMtrHeight->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Height")));	//wj acc
	
    //Size : Keep ratio
	mpCbxScale->SetClickHdl( LINK( this, TransformationPropertyPanel, ClickAutoHdl ) );
	
    //rotation:
	mpMtrAngle->SetModifyHdl(LINK( this, TransformationPropertyPanel, AngleModifiedHdl));
	mpMtrAngle->EnableAutocomplete( false );
	mpMtrAngle->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Rotation")));	//wj acc
	
    //rotation control
	mpDial->SetModifyHdl(LINK( this, TransformationPropertyPanel, RotationHdl));
	
    //flip:
	mpFlipTbx->SetSelectHdl( LINK( this, TransformationPropertyPanel, FlipHdl) );
	mpFlipTbx->SetItemImage(FLIP_HORIZONTAL,maImgFlipHori);
    mpFlipTbx->SetItemImage(FLIP_VERTICAL,maImgFlipVert);
	mpFlipTbx->SetQuickHelpText(FLIP_HORIZONTAL,String(SVX_RES(STR_QH_HORI_FLIP))); //Add
	mpFlipTbx->SetQuickHelpText(FLIP_VERTICAL,String(SVX_RES(STR_QH_VERT_FLIP))); //Add
	
	mpMtrPosX->SetAccessibleRelationLabeledBy(mpFtPosX.get());	
	mpMtrPosY->SetAccessibleRelationLabeledBy(mpFtPosY.get());	
	mpMtrWidth->SetAccessibleRelationLabeledBy(mpFtWidth.get());	
	mpMtrHeight->SetAccessibleRelationLabeledBy(mpFtHeight.get());	
	mpMtrAngle->SetAccessibleRelationLabeledBy(mpFtAngle.get());
	//mpMtrAngle->SetMpSubEditAccLableBy(mpFtAngle.get());
	mpFlipTbx->SetAccessibleRelationLabeledBy(mpFtFlip.get());

    mpMtrAngle->InsertValue(0, FUNIT_CUSTOM);
	mpMtrAngle->InsertValue(4500, FUNIT_CUSTOM);
	mpMtrAngle->InsertValue(9000, FUNIT_CUSTOM);
	mpMtrAngle->InsertValue(13500, FUNIT_CUSTOM);
	mpMtrAngle->InsertValue(18000, FUNIT_CUSTOM);
	mpMtrAngle->InsertValue(22500, FUNIT_CUSTOM);
	mpMtrAngle->InsertValue(27000, FUNIT_CUSTOM);
	mpMtrAngle->InsertValue(31500, FUNIT_CUSTOM);

    SfxViewShell* pCurSh = SfxViewShell::Current();
	if ( pCurSh )
		mpView = pCurSh->GetDrawView();
	else
		mpView = NULL;

	if ( mpView != NULL )
	{
		maUIScale = mpView->GetModel()->GetUIScale();

		const SdrMarkList& rMarkList = mpView->GetMarkedObjectList();
		if(1 == rMarkList.GetMarkCount())
		{
			const SdrObject* pObj = rMarkList.GetMark(0)->GetMarkedSdrObj();
			const SdrObjKind eKind((SdrObjKind)pObj->GetObjIdentifier());

			if((pObj->GetObjInventor() == SdrInventor) && (OBJ_TEXT == eKind || OBJ_TITLETEXT == eKind || OBJ_OUTLINETEXT == eKind) && ((SdrTextObj*)pObj)->HasText())
			{
				mbAdjustEnabled = true;
			}
		}
	}
	
    mePoolUnit = maTransfWidthControl.GetCoreMetric();
	meDlgUnit = GetModuleFieldUnit();
	SetFieldUnit( *mpMtrPosX, meDlgUnit, true );
	SetFieldUnit( *mpMtrPosY, meDlgUnit, true );
	SetFieldUnit( *mpMtrWidth, meDlgUnit, true );
	SetFieldUnit( *mpMtrHeight, meDlgUnit, true );
}



void TransformationPropertyPanel::SetupIcons(void)
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



TransformationPropertyPanel* TransformationPropertyPanel::Create (
    Window* pParent,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings)
{
    if (pParent == NULL)
        throw lang::IllegalArgumentException(A2S("no parent Window given to TransformationPropertyPanel::Create"), NULL, 0);
    if ( ! rxFrame.is())
        throw lang::IllegalArgumentException(A2S("no XFrame given to TransformationPropertyPanel::Create"), NULL, 1);
    if (pBindings == NULL)
        throw lang::IllegalArgumentException(A2S("no SfxBindings given to TransformationPropertyPanel::Create"), NULL, 2);
    
    return new TransformationPropertyPanel(
        pParent,
        rxFrame,
        pBindings);
}



void TransformationPropertyPanel::DataChanged(
    const DataChangedEvent& rEvent)
{
    (void)rEvent;
    
    SetupIcons();
}



void TransformationPropertyPanel::HandleContextChange(
    const ::sfx2::sidebar::EnumContext aContext)
{
    if(maContext == aContext)
    {
        // Nothing to do.
        return;
    }

    maContext = aContext;

    switch (maContext.GetCombinedContext())
    {
        case CombinedEnumContext(Application_Writer, Context_Draw): //case PROPERTY_CONTEXT_SW_DRAW:		
		{
			mpMtrWidth->SetMin( 2 );
			mpMtrHeight->SetMin( 2 );
			mpFtPosX->Hide();
			mpMtrPosX->Hide();
			mpFtPosY->Hide();
			mpMtrPosY->Hide();
			
            //rotation
			mpFtAngle->Show();
			mpMtrAngle->Show();
			mpDial->Show();
			
            //flip
			mpFtFlip->Show();
			mpFlipTbx->Show();
			Size aTbxSize = mpFlipTbx->CalcWindowSizePixel();
			mpFlipTbx->SetOutputSizePixel( aTbxSize );
			mbIsFlip = true;

			mpFtWidth->SetPosPixel(Point(LogicToPixel(Point(FT_POSITION_X_X,FT_POSITION_X_Y), MAP_APPFONT)));
			mpMtrWidth->SetPosPixel(Point(LogicToPixel(Point(MF_POSITION_X_X,MF_POSITION_X_Y), MAP_APPFONT)));
			mpFtHeight->SetPosPixel(Point(LogicToPixel(Point(FT_POSITION_Y_X,FT_POSITION_Y_Y), MAP_APPFONT)));
			mpMtrHeight->SetPosPixel(Point(LogicToPixel(Point(MF_POSITION_Y_X,MF_POSITION_Y_Y), MAP_APPFONT)));
			mpCbxScale->SetPosPixel(Point(LogicToPixel(Point(FT_WIDTH_X,FT_WIDTH_Y), MAP_APPFONT)));

			mpFtAngle->SetPosPixel(Point(LogicToPixel(Point(FT_ANGLE_X,FT_ANGLE_Y), MAP_APPFONT)));
			mpMtrAngle->SetPosPixel(Point(LogicToPixel(Point(MF_ANGLE_X2,MF_ANGLE_Y2), MAP_APPFONT)));
			mpFlipTbx->SetPosPixel(Point(LogicToPixel(Point(FLIP_HORI_X2,FLIP_HORI_Y2), MAP_APPFONT)));
			mpDial->SetPosPixel(Point(LogicToPixel(Point(ROTATE_CONTROL_X2,ROTATE_CONTROL_Y2), MAP_APPFONT)));
			mpFtFlip->SetPosPixel(Point(LogicToPixel(Point(FT_FLIP_X2,FT_FLIP_Y2), MAP_APPFONT)));

			Size aSize(GetOutputSizePixel().Width(),PS_SECTIONPAGE_HEIGHT2);
			aSize = LogicToPixel( aSize, MapMode(MAP_APPFONT) ); 
			SetSizePixel(aSize);
		}
		break;

        case CombinedEnumContext(Application_Writer, Context_Graphic): //case PROPERTY_CONTEXT_SW_GRAPHIC:	
		case CombinedEnumContext(Application_Writer, Context_Media): //case PROPERTY_CONTEXT_SW_MEDIA:		
		case CombinedEnumContext(Application_Writer, Context_Frame): //case PROPERTY_CONTEXT_SW_FRAME:		
		case CombinedEnumContext(Application_Writer, Context_OLE): //case PROPERTY_CONTEXT_SW_OLE:			
		case CombinedEnumContext(Application_Writer, Context_Form): //case PROPERTY_CONTEXT_SW_FORM:		
		{
			mpMtrWidth->SetMin( 2 );
			mpMtrHeight->SetMin( 2 );
			mpFtPosX->Hide();
			mpMtrPosX->Hide();
			mpFtPosY->Hide();
			mpMtrPosY->Hide();
        
			//rotation
			mpFtAngle->Hide();
			mpMtrAngle->Hide();
			mpDial->Hide();
        
			//flip
			mpFlipTbx->Hide();
			mpFtFlip->Hide();
			mbIsFlip = false;
        
			mpFtWidth->SetPosPixel(Point(LogicToPixel(Point(FT_POSITION_X_X,FT_POSITION_X_Y), MAP_APPFONT)));
			mpMtrWidth->SetPosPixel(Point(LogicToPixel(Point(MF_POSITION_X_X,MF_POSITION_X_Y), MAP_APPFONT)));
			mpFtHeight->SetPosPixel(Point(LogicToPixel(Point(FT_POSITION_Y_X,FT_POSITION_Y_Y), MAP_APPFONT)));
			mpMtrHeight->SetPosPixel(Point(LogicToPixel(Point(MF_POSITION_Y_X,MF_POSITION_Y_Y), MAP_APPFONT)));
			mpCbxScale->SetPosPixel(Point(LogicToPixel(Point(FT_WIDTH_X,FT_WIDTH_Y), MAP_APPFONT)));
        
			Size aSize(GetOutputSizePixel().Width(),PS_SECTIONPAGE_HEIGHT3);
			aSize = LogicToPixel( aSize, MapMode(MAP_APPFONT) ); 
			SetSizePixel(aSize);
		}
		break;
        case CombinedEnumContext(Application_Calc, Context_Draw): //case PROPERTY_CONTEXT_SC_DRAW:
        case CombinedEnumContext(Application_Draw, Context_Draw): //case PROPERTY_CONTEXT_SD_DRAW:
		case CombinedEnumContext(Application_Calc, Context_Graphic): //case PROPERTY_CONTEXT_SC_GRAPHIC:
        case CombinedEnumContext(Application_Draw, Context_TextObject): //case PROPERTY_CONTEXT_SD_TEXTOBJ:
        case CombinedEnumContext(Application_Draw, Context_Graphic): //case PROPERTY_CONTEXT_SD_GRAPHIC:
		{
			mpMtrWidth->SetMin( 1 );
			mpMtrHeight->SetMin( 1 );
			mpFtPosX->Show();
			mpMtrPosX->Show();
			mpFtPosY->Show();
			mpMtrPosY->Show();
        
			//rotation
			mpFtAngle->Show();
			mpMtrAngle->Show();
			mpDial->Show();
        
            //flip
			mpFlipTbx->Show();
			mpFtFlip->Show();
			Size aTbxSize = mpFlipTbx->CalcWindowSizePixel();
			mpFlipTbx->SetOutputSizePixel( aTbxSize );
			mbIsFlip = true;
        
			Size aSize(GetOutputSizePixel().Width(),PS_SECTIONPAGE_HEIGHT);
			aSize = LogicToPixel( aSize, MapMode(MAP_APPFONT) ); 
			SetSizePixel(aSize);
		}
		break;
		case CombinedEnumContext(Application_Calc, Context_Chart): //case PROPERTY_CONTEXT_SC_CHART:
		case CombinedEnumContext(Application_Calc, Context_Form): //case PROPERTY_CONTEXT_SC_FORM:			
		case CombinedEnumContext(Application_Calc, Context_Media): //case PROPERTY_CONTEXT_SC_MEDIA:		
		case CombinedEnumContext(Application_Calc, Context_OLE): //case PROPERTY_CONTEXT_SC_OLE:
		case CombinedEnumContext(Application_Calc, Context_Multiobj): //case PROPERTY_CONTEXT_SC_MULTIOBJ:	
		case CombinedEnumContext(Application_Draw, Context_Media): //case PROPERTY_CONTEXT_SD_MEDIA:		
		case CombinedEnumContext(Application_Draw, Context_Form): //case PROPERTY_CONTEXT_SD_FORM:			
		case CombinedEnumContext(Application_Draw, Context_OLE): //case PROPERTY_CONTEXT_SD_OLE:
		case CombinedEnumContext(Application_Draw, Context_3DObj): //case PROPERTY_CONTEXT_SD_3DOBJ:
		case CombinedEnumContext(Application_Draw, Context_Multiobj): //case PROPERTY_CONTEXT_SD_MULTIOBJ:
		{
			mpMtrWidth->SetMin( 1 );
			mpMtrHeight->SetMin( 1 );
			mpFtPosX->Show();
			mpMtrPosX->Show();
			mpFtPosY->Show();
			mpMtrPosY->Show();
        
			//rotation
			mpFtAngle->Hide();
			mpMtrAngle->Hide();
			mpDial->Hide();
        
			//flip
			mpFlipTbx->Hide();
			mpFtFlip->Hide();
			mbIsFlip = false;
        
			Size aSize(GetOutputSizePixel().Width(),PS_SECTIONPAGE_HEIGHT4);
			aSize = LogicToPixel( aSize, MapMode(MAP_APPFONT) ); 
			SetSizePixel(aSize);
		}
		break;
	}

    //Added for windows classic theme
	mpFlipTbx->SetBackground(Wallpaper());
	mpFlipTbx->SetPaintTransparent(true);
}



IMPL_LINK( TransformationPropertyPanel, ChangeWidthHdl, void*, pBox )
{
	if( mpCbxScale->IsChecked() &&
		mpCbxScale->IsEnabled() )
	{
		long nHeight = (long) ( ((double) mlOldHeight * (double) mpMtrWidth->GetValue()) / (double) mlOldWidth );
		if( nHeight <= mpMtrHeight->GetMax( FUNIT_NONE ) )
		{
			mpMtrHeight->SetUserValue( nHeight, FUNIT_NONE );
		}
		else
		{
			nHeight = (long)mpMtrHeight->GetMax( FUNIT_NONE );
			mpMtrHeight->SetUserValue( nHeight );
			const long nWidth = (long) ( ((double) mlOldWidth * (double) nHeight) / (double) mlOldHeight );
			mpMtrWidth->SetUserValue( nWidth, FUNIT_NONE );
		}
	}
	executeSize();
	return 0;
}



IMPL_LINK( TransformationPropertyPanel, ChangeHeightHdl, void *, EMPTYARG )
{
	if( mpCbxScale->IsChecked() &&
		mpCbxScale->IsEnabled() )
	{
		long nWidth = (long) ( ((double)mlOldWidth * (double)mpMtrHeight->GetValue()) / (double)mlOldHeight );
		if( nWidth <= mpMtrWidth->GetMax( FUNIT_NONE ) )
		{
			mpMtrWidth->SetUserValue( nWidth, FUNIT_NONE );
		}
		else
		{
			nWidth = (long)mpMtrWidth->GetMax( FUNIT_NONE );
			mpMtrWidth->SetUserValue( nWidth );
			const long nHeight = (long) ( ((double)mlOldHeight * (double)nWidth) / (double)mlOldWidth );
			mpMtrHeight->SetUserValue( nHeight, FUNIT_NONE );
		}
	}
	executeSize();
	return 0;
}



IMPL_LINK( TransformationPropertyPanel, ChangePosXHdl, void *, EMPTYARG )
{
	executePosX();
	return 0;
}



IMPL_LINK( TransformationPropertyPanel, ChangePosYHdl, void *, EMPTYARG )
{
	executePosY();
	return 0;
}



IMPL_LINK( TransformationPropertyPanel, ClickAutoHdl, void *, EMPTYARG )
{
	if ( mpCbxScale->IsChecked() )
	{
		mlOldWidth  = Max( GetCoreValue( *mpMtrWidth,  mePoolUnit ), 1L );
		mlOldHeight = Max( GetCoreValue( *mpMtrHeight, mePoolUnit ), 1L );
	}

	// mpCbxScale must synchronized with that on Position and Size tabpage on Shape Properties dialog
	SvtViewOptions	aPageOpt( E_TABPAGE, String::CreateFromInt32( RID_SVXPAGE_POSITION_SIZE ) );
	aPageOpt.SetUserItem( USERITEM_NAME, ::com::sun::star::uno::makeAny( ::rtl::OUString( String::CreateFromInt32( mpCbxScale->IsChecked() ) ) ) );

	return 0;
}



IMPL_LINK( TransformationPropertyPanel, AngleModifiedHdl, void *, EMPTYARG )
{
	String sTmp = mpMtrAngle->GetText();
	bool    bNegative = 0;
	sal_Unicode nChar = sTmp.GetChar( 0 );

	if( nChar == '-' )
	{
		bNegative = 1;
		nChar = sTmp.GetChar( 1 );
	}

	if( (nChar < '0') || (nChar > '9') )
		return 0;
	double dTmp = sTmp.ToDouble();
	if(bNegative)
	{
		while(dTmp<0)
			dTmp += 360;
	}
	sal_Int64 nTmp = dTmp*100;

	SfxInt32Item aAngleItem( SID_ATTR_TRANSFORM_ANGLE,(sal_uInt32) nTmp);
	SfxInt32Item aRotXItem( SID_ATTR_TRANSFORM_ROT_X,(sal_uInt32) mlRotX);
	SfxInt32Item aRotYItem( SID_ATTR_TRANSFORM_ROT_Y,(sal_uInt32) mlRotY);
	
	GetBindings()->GetDispatcher()->Execute(
		SID_ATTR_TRANSFORM, SFX_CALLMODE_RECORD, &aAngleItem, &aRotXItem, &aRotYItem, 0L );

	return 0;
}



IMPL_LINK( TransformationPropertyPanel, RotationHdl, void *, EMPTYARG )
{
	sal_Int32 nTmp = mpDial->GetRotation();

	SfxInt32Item aAngleItem( SID_ATTR_TRANSFORM_ANGLE,(sal_uInt32) nTmp);
	SfxInt32Item aRotXItem( SID_ATTR_TRANSFORM_ROT_X,(sal_uInt32) mlRotX);
	SfxInt32Item aRotYItem( SID_ATTR_TRANSFORM_ROT_Y,(sal_uInt32) mlRotY);
	
	GetBindings()->GetDispatcher()->Execute(
		SID_ATTR_TRANSFORM, SFX_CALLMODE_RECORD, &aAngleItem, &aRotXItem, &aRotYItem, 0L );

	return 0;
}



IMPL_LINK( TransformationPropertyPanel, FlipHdl, ToolBox*, pBox )
{
	switch (pBox->GetCurItemId())
	{
		case FLIP_HORIZONTAL:
		{
			SfxBoolItem aHoriItem( SID_ATTR_TRANSFORM_MIRROR_HORIZONTAL );
			GetBindings()->GetDispatcher()->Execute(
				SID_ATTR_TRANSFORM_MIRROR_HORIZONTAL, SFX_CALLMODE_RECORD, &aHoriItem, 0L );
		}
		break;
		case FLIP_VERTICAL:
		{
			SfxBoolItem aVertItem( SID_ATTR_TRANSFORM_MIRROR_VERTICAL );
			GetBindings()->GetDispatcher()->Execute(
				SID_ATTR_TRANSFORM_MIRROR_VERTICAL, SFX_CALLMODE_RECORD, &aVertItem, 0L );
		}
		break;
	}
	return 0;
}



void TransformationPropertyPanel::NotifyItemUpdate( 
    sal_uInt16 nSID, 
    SfxItemState eState, 
    const SfxPoolItem* pState)
{  
	mpFtAngle->Enable();
	mpMtrAngle->Enable();
	mpDial->Enable();
	mpFtFlip->Enable();
	mpFlipTbx->Enable();

    const SfxUInt32Item*	pWidthItem;
	const SfxUInt32Item*	pHeightItem;		
	
	SfxViewShell* pCurSh = SfxViewShell::Current();
	if ( pCurSh )
		mpView = pCurSh->GetDrawView();
	else
		mpView = NULL;

	if ( mpView == NULL )
		return;

	const SdrMarkList& rMarkList = mpView->GetMarkedObjectList();

	if(1 == rMarkList.GetMarkCount())
	{
		const SdrObject* pObj = rMarkList.GetMark(0)->GetMarkedSdrObj();
		const SdrObjKind eKind((SdrObjKind)pObj->GetObjIdentifier());

		if((pObj->GetObjInventor() == SdrInventor) && (OBJ_TEXT == eKind || OBJ_TITLETEXT == eKind || OBJ_OUTLINETEXT == eKind) && ((SdrTextObj*)pObj)->HasText())
			mbAdjustEnabled = true;
		else
			mbAdjustEnabled = false;
	}
	else
		mbAdjustEnabled = false;

    if (nSID == SID_ATTR_TRANSFORM_MATRIX)
    {
        // check if slot for AffineMatrixItem does work. If yes,
        // most direct values can be migrated to this full object transformation,
        // except the common slots like rotation center, move/size protect, etc.
        bool bBla = true;
    }
	else if (nSID == SID_ATTR_TRANSFORM_WIDTH)
	{
		if ( SFX_ITEM_AVAILABLE == eState )
		{
            pWidthItem = dynamic_cast< const SfxUInt32Item* >(pState);

			if (pWidthItem)
			{				
				long mlOldWidth1 = pWidthItem->GetValue();

				mlOldWidth1 = Fraction( mlOldWidth1 ) / maUIScale;
				SetMetricValue( *mpMtrWidth, mlOldWidth1, mePoolUnit );
				mlOldWidth = mlOldWidth1;
			}
		}
		else
		{
			mpMtrWidth->SetText( String());
		}
	}
	else if (nSID == SID_ATTR_TRANSFORM_HEIGHT)
	{
		if ( SFX_ITEM_AVAILABLE == eState )
		{
            pHeightItem = dynamic_cast< const SfxUInt32Item* >(pState);

            if(pHeightItem)
			{
				long mlOldHeight1 = pHeightItem->GetValue();

				mlOldHeight1 = Fraction( mlOldHeight1 ) / maUIScale;
				SetMetricValue( *mpMtrHeight, mlOldHeight1, mePoolUnit );
				mlOldHeight = mlOldHeight1;
			}
		}
		else
		{
			mpMtrHeight->SetText( String());
		}
	}
	else if ( nSID == SID_ATTR_TRANSFORM_POS_X)
	{
        const SfxInt32Item* pItem = dynamic_cast< const SfxInt32Item* >(pState);

		if (SFX_ITEM_AVAILABLE == eState && pItem)
		{
			long nTmp = pItem->GetValue(); 
			nTmp = Fraction( nTmp ) / maUIScale;
			SetMetricValue( *mpMtrPosX, nTmp, mePoolUnit );
		}
		else
		{
			mpMtrPosX->SetText( String());
		}
	}
	else if ( nSID == SID_ATTR_TRANSFORM_POS_Y)
	{
        const SfxInt32Item* pItem = dynamic_cast< const SfxInt32Item* >(pState);

		if (SFX_ITEM_AVAILABLE == eState && pItem)
		{
			long nTmp = pItem->GetValue(); 
			nTmp = Fraction( nTmp ) / maUIScale;
			SetMetricValue( *mpMtrPosY, nTmp, mePoolUnit );
		}
		else
		{
			mpMtrPosY->SetText( String());
		}
	}
	else if ( nSID == SID_ATTR_TRANSFORM_ROT_X)
	{
		if (SFX_ITEM_AVAILABLE == eState)
		{
			mlRotX = ((const SfxInt32Item*)pState)->GetValue(); 
			mlRotX = Fraction( mlRotX ) / maUIScale;
		}
	}
	else if ( nSID == SID_ATTR_TRANSFORM_ROT_Y)
	{
		if (SFX_ITEM_AVAILABLE == eState)
		{
			mlRotY = ((const SfxInt32Item*)pState)->GetValue(); 
			mlRotY = Fraction( mlRotY ) / maUIScale;
		}
	}
	else if (nSID == SID_ATTR_TRANSFORM_PROTECT_POS)
	{
        const SfxBoolItem* pItem = dynamic_cast< const SfxBoolItem* >(pState);

        if (SFX_ITEM_AVAILABLE == eState && pItem)
		{
			// record the state of position protect
			mbPositionProtected = pItem->GetValue();
		}
		else
		{
			mbPositionProtected = false;
		}
	}
	else if (nSID == SID_ATTR_TRANSFORM_PROTECT_SIZE)
	{
        const SfxBoolItem* pItem = dynamic_cast< const SfxBoolItem* >(pState);

		if (SFX_ITEM_AVAILABLE == eState && pItem)
		{
			// record the state of size protect
			mbSizeProtected = pItem->GetValue();
		}
		else
		{
			mbSizeProtected = false;
		}
	}	
	else if (nSID == SID_ATTR_TRANSFORM_AUTOWIDTH)
	{
        const SfxBoolItem* pItem = dynamic_cast< const SfxBoolItem* >(pState);

        if (SFX_ITEM_AVAILABLE == eState && pItem)
        {
			mbAutoWidth = pItem->GetValue();
        }
	}
	else if (nSID == SID_ATTR_TRANSFORM_AUTOHEIGHT)
	{
        const SfxBoolItem* pItem = dynamic_cast< const SfxBoolItem* >(pState);

		if (SFX_ITEM_AVAILABLE == eState && pItem)
        {
			mbAutoHeight = pItem->GetValue();
        }
	}
	else if( nSID == SID_ATTR_TRANSFORM_ANGLE )
	{
		if (eState >= SFX_ITEM_AVAILABLE)
		{
			long nTmp = ((const SfxInt32Item*)pState)->GetValue(); 
			mpMtrAngle->SetValue( nTmp );
			mpDial->SetRotation( nTmp );
			switch(nTmp)
			{
				case 0:
					mpMtrAngle->SelectEntryPos(0);
				break;
				case 4500:
					mpMtrAngle->SelectEntryPos(1);
				break;
				case 9000:
					mpMtrAngle->SelectEntryPos(2);
				break;
				case 13500:
					mpMtrAngle->SelectEntryPos(3);
				break;
				case 18000:
					mpMtrAngle->SelectEntryPos(4);
				break;
				case 22500:
					mpMtrAngle->SelectEntryPos(5);
				break;
				case 27000:
					mpMtrAngle->SelectEntryPos(6);
				break;
				case 315000:
					mpMtrAngle->SelectEntryPos(7);
			}
		}
		else
		{
			mpMtrAngle->SetText( String() );
			mpDial->SetRotation( 0 );
		}
	}
	else if( nSID == SID_ATTR_METRIC )
    {
		MetricState( eState, pState );	
    }

    const sal_Int32 nCombinedContext(maContext.GetCombinedContext());

	if(1 == rMarkList.GetMarkCount())
	{
		const SdrObject* pObj = rMarkList.GetMark(0)->GetMarkedSdrObj();
		const SdrObjKind eKind((SdrObjKind)pObj->GetObjIdentifier());

        if(((nCombinedContext == CombinedEnumContext(Application_Draw, Context_Draw) // mnContextId == PROPERTY_CONTEXT_SD_DRAW
            || nCombinedContext == CombinedEnumContext(Application_Draw, Context_TextObject) // mnContextId == PROPERTY_CONTEXT_SD_TEXTOBJ
            ) && OBJ_EDGE == eKind)
            || OBJ_CAPTION == eKind)
		// if((((mnContextId == PROPERTY_CONTEXT_SD_DRAW) || (mnContextId == PROPERTY_CONTEXT_SD_TEXTOBJ)) && OBJ_EDGE == eKind) || OBJ_CAPTION == eKind)
		{
			mpFtAngle->Disable();
			mpMtrAngle->Disable();
			mpDial->Disable();
			mpFlipTbx->Disable();
			mpFtFlip->Disable();
		}
	}
	else
	{
		sal_uInt16 nMarkObj = 0;
		bool isNoEdge = true;
		while(rMarkList.GetMark(nMarkObj))
		{
			const SdrObject* pObj = rMarkList.GetMark(nMarkObj)->GetMarkedSdrObj();
			const SdrObjKind eKind((SdrObjKind)pObj->GetObjIdentifier());

            if(((nCombinedContext == CombinedEnumContext(Application_Draw, Context_Draw) // mnContextId == PROPERTY_CONTEXT_SD_DRAW
                || nCombinedContext == CombinedEnumContext(Application_Draw, Context_TextObject) // mnContextId == PROPERTY_CONTEXT_SD_TEXTOBJ
                ) && OBJ_EDGE == eKind) 
                || OBJ_CAPTION == eKind)
            // if((((mnContextId == PROPERTY_CONTEXT_SD_DRAW) || (mnContextId == PROPERTY_CONTEXT_SD_TEXTOBJ)) && OBJ_EDGE == eKind) ||OBJ_CAPTION == eKind)
			{
				isNoEdge = false;
				break;
			}
			nMarkObj++;
		}
		if(!isNoEdge)
		{
			mpFtAngle->Disable();
			mpMtrAngle->Disable();
			mpDial->Disable();
			mpFlipTbx->Disable();
			mpFtFlip->Disable();
		}
	}

    if(nCombinedContext == CombinedEnumContext(Application_Draw, Context_TextObject)) // mnContextId == PROPERTY_CONTEXT_SD_TEXTOBJ
    //if(mnContextId == PROPERTY_CONTEXT_SD_TEXTOBJ)
	{
		mpFlipTbx->Disable();
		mpFtFlip->Disable();
	}

    DisableControls();

	// mpCbxScale must synchronized with that on Position and Size tabpage on Shape Properties dialog
	SvtViewOptions	aPageOpt( E_TABPAGE, String::CreateFromInt32( RID_SVXPAGE_POSITION_SIZE ) );
	String	sUserData;
	::com::sun::star::uno::Any	aUserItem = aPageOpt.GetUserItem( USERITEM_NAME );
	::rtl::OUString	aTemp;
	if ( aUserItem >>= aTemp )
		sUserData = String( aTemp );
	mpCbxScale->Check( (bool)sUserData.ToInt32() );
}



SfxBindings* TransformationPropertyPanel::GetBindings() 
{ 
    return mpBindings; 
}



void TransformationPropertyPanel::executeSize()
{
	if ( mpMtrWidth->IsValueModified() || mpMtrHeight->IsValueModified())
	{
		Fraction aUIScale = mpView->GetModel()->GetUIScale();

		// get Width
		double nWidth = (double)mpMtrWidth->GetValue( meDlgUnit );
		nWidth = MetricField::ConvertDoubleValue( nWidth, mpMtrWidth->GetBaseValue(), mpMtrWidth->GetDecimalDigits(), meDlgUnit, FUNIT_100TH_MM );
		long lWidth = (long)(nWidth * (double)aUIScale);
		lWidth = OutputDevice::LogicToLogic( lWidth, MAP_100TH_MM, (MapUnit)mePoolUnit );
		lWidth = (long)mpMtrWidth->Denormalize( lWidth );

		// get Height
		double nHeight = (double)mpMtrHeight->GetValue( meDlgUnit );
		nHeight = MetricField::ConvertDoubleValue( nHeight, mpMtrHeight->GetBaseValue(), mpMtrHeight->GetDecimalDigits(), meDlgUnit, FUNIT_100TH_MM );
		long lHeight = (long)(nHeight * (double)aUIScale);
		lHeight = OutputDevice::LogicToLogic( lHeight, MAP_100TH_MM, (MapUnit)mePoolUnit );
		lHeight = (long)mpMtrWidth->Denormalize( lHeight );

		// put Width & Height to itemset
		SfxUInt32Item aWidthItem( SID_ATTR_TRANSFORM_WIDTH, (sal_uInt32) lWidth);
		SfxUInt32Item aHeightItem( SID_ATTR_TRANSFORM_HEIGHT, (sal_uInt32) lHeight);
		SfxAllEnumItem aPointItem (SID_ATTR_TRANSFORM_SIZE_POINT, (sal_uInt16)meRP);
        const sal_Int32 nCombinedContext(maContext.GetCombinedContext());

        if( nCombinedContext == CombinedEnumContext(Application_Writer, Context_Graphic) // mnContextId == PROPERTY_CONTEXT_SW_GRAPHIC 
            || nCombinedContext == CombinedEnumContext(Application_Writer, Context_OLE) //mnContextId == PROPERTY_CONTEXT_SW_OLE
            )
        // if( mnContextId == PROPERTY_CONTEXT_SW_GRAPHIC || mnContextId == PROPERTY_CONTEXT_SW_OLE )
        {
			GetBindings()->GetDispatcher()->Execute(SID_ATTR_TRANSFORM, SFX_CALLMODE_RECORD, &aWidthItem, &aHeightItem, &aPointItem, 0L );
        }
		else
		{
			if ( (mpMtrWidth->IsValueModified()) && (mpMtrHeight->IsValueModified()))
				GetBindings()->GetDispatcher()->Execute(SID_ATTR_TRANSFORM, SFX_CALLMODE_RECORD, &aWidthItem, &aHeightItem, &aPointItem, 0L );
			else if( mpMtrWidth->IsValueModified())
				GetBindings()->GetDispatcher()->Execute(SID_ATTR_TRANSFORM, SFX_CALLMODE_RECORD, &aWidthItem, &aPointItem, 0L );
			else if ( mpMtrHeight->IsValueModified())
				GetBindings()->GetDispatcher()->Execute(SID_ATTR_TRANSFORM, SFX_CALLMODE_RECORD, &aHeightItem, &aPointItem, 0L );
		}		
	}
}



void TransformationPropertyPanel::executePosX()
{
	if ( mpMtrPosX->IsValueModified())
	{
		long lX = GetCoreValue( *mpMtrPosX, mePoolUnit );
		if( mbMtrPosXMirror )
			lX = -lX;
		long lY = GetCoreValue( *mpMtrPosY, mePoolUnit );

		Size aPageSize;
		Rectangle aRect;
		maRect = mpView->GetAllMarkedRect();	
		aRect = mpView->GetAllMarkedRect();

		Fraction aUIScale = mpView->GetModel()->GetUIScale();
		lX += maAnchorPos.X();
		lX = Fraction( lX ) * aUIScale;
		lY += maAnchorPos.Y();
		lY = Fraction( lY ) * aUIScale;
		
		SfxInt32Item aPosXItem( SID_ATTR_TRANSFORM_POS_X,(sal_uInt32) lX);
		SfxInt32Item aPosYItem( SID_ATTR_TRANSFORM_POS_Y,(sal_uInt32) lY);
	
		GetBindings()->GetDispatcher()->Execute(
			SID_ATTR_TRANSFORM, SFX_CALLMODE_RECORD, &aPosXItem, 0L );	
	}
}



void TransformationPropertyPanel::executePosY()
{
	if ( mpMtrPosY->IsValueModified() )
	{
		long lX = GetCoreValue( *mpMtrPosX, mePoolUnit );
		long lY = GetCoreValue( *mpMtrPosY, mePoolUnit );

		Size aPageSize;
		Rectangle aRect;
		maRect = mpView->GetAllMarkedRect();	
		aRect = mpView->GetAllMarkedRect();

		Fraction aUIScale = mpView->GetModel()->GetUIScale();
		lX += maAnchorPos.X();
		lX = Fraction( lX ) * aUIScale;
		lY += maAnchorPos.Y();
		lY = Fraction( lY ) * aUIScale;
		
		SfxInt32Item aPosXItem( SID_ATTR_TRANSFORM_POS_X,(sal_uInt32) lX);
		SfxInt32Item aPosYItem( SID_ATTR_TRANSFORM_POS_Y,(sal_uInt32) lY);
	
		GetBindings()->GetDispatcher()->Execute(
			SID_ATTR_TRANSFORM, SFX_CALLMODE_RECORD, &aPosYItem, 0L );	
	}
}



void TransformationPropertyPanel::MetricState( SfxItemState eState, const SfxPoolItem* pState )
{
	bool bPosXBlank = false;
	bool bPosYBlank = false;
	bool bWidthBlank = false;
	bool bHeightBlank = false;
	String sNull = String::CreateFromAscii("");
	meDlgUnit = GetCurrentUnit(eState,pState);

	if( mpMtrPosX->GetText() == sNull )
		bPosXBlank = true;
	SetFieldUnit( *mpMtrPosX, meDlgUnit, true );
	if(bPosXBlank)
		mpMtrPosX->SetText(String());

	if( mpMtrPosY->GetText() == sNull )
		bPosYBlank = true;
	SetFieldUnit( *mpMtrPosY, meDlgUnit, true );
	if(bPosYBlank)
		mpMtrPosY->SetText(String());

	if( mpMtrWidth->GetText() == sNull )
		bWidthBlank = true;
	SetFieldUnit( *mpMtrWidth, meDlgUnit, true );
	if(bWidthBlank)
		mpMtrWidth->SetText(String());

	if( mpMtrHeight->GetText() == sNull )
		bHeightBlank = true;
	SetFieldUnit( *mpMtrHeight, meDlgUnit, true );
	if(bHeightBlank)
		mpMtrHeight->SetText(String());
}



FieldUnit TransformationPropertyPanel::GetCurrentUnit( SfxItemState eState, const SfxPoolItem* pState )
{
	FieldUnit eUnit = FUNIT_NONE;

	if ( pState && eState >= SFX_ITEM_DEFAULT )
		eUnit = (FieldUnit)( (const SfxUInt16Item*)pState )->GetValue();
	else
	{
		SfxViewFrame* pFrame = SfxViewFrame::Current();
		SfxObjectShell* pSh = NULL;
		if ( pFrame )
			pSh = pFrame->GetObjectShell();
		if ( pSh )
		{
			SfxModule* pModule = pSh->GetModule();
			if ( pModule )
			{
				const SfxPoolItem* pItem = pModule->GetItem( SID_ATTR_METRIC );
				if ( pItem )
					eUnit = (FieldUnit)( (SfxUInt16Item*)pItem )->GetValue();
			}
			else
			{
				DBG_ERRORFILE( "GetModuleFieldUnit(): no module found" );
			}
		}
	}
	
	return eUnit;
}



void TransformationPropertyPanel::DisableControls()
{
    if( mbPositionProtected )
	{	
		// the position is protected("Position protect" option in modal dialog is checked),
		// disable all the Position controls in sidebar
		mpFtPosX->Disable();
		mpMtrPosX->Disable();
		mpFtPosY->Disable();
		mpMtrPosY->Disable();
		mpFtAngle->Disable();
		mpMtrAngle->Disable();
		mpDial->Disable();
		mpFtFlip->Disable();
		mpFlipTbx->Disable();

		mpFtWidth->Disable();
		mpMtrWidth->Disable();
		mpFtHeight->Disable();
		mpMtrHeight->Disable();
        mpCbxScale->Disable();
	}
	else
	{
		mpFtPosX->Enable();
		mpMtrPosX->Enable();
		mpFtPosY->Enable();
		mpMtrPosY->Enable();

        //mpFtAngle->Enable();
		//mpMtrAngle->Enable();
		//mpDial->Enable();
		//mpFtFlip->Enable();
		//mpFlipTbx->Enable();

		if( mbSizeProtected )
		{
			mpFtWidth->Disable();
			mpMtrWidth->Disable();
			mpFtHeight->Disable();
			mpMtrHeight->Disable();
			mpCbxScale->Disable();
		}
		else
		{
			if(	mbAdjustEnabled )
			{
				if( mbAutoWidth )
				{
					mpFtWidth->Disable();
					mpMtrWidth->Disable();
					mpCbxScale->Disable();
				}
				else
				{
					mpFtWidth->Enable();
					mpMtrWidth->Enable();
				}
				if( mbAutoHeight )
				{
					mpFtHeight->Disable();
					mpMtrHeight->Disable();
					mpCbxScale->Disable();
				}
				else
				{
					mpFtHeight->Enable();
					mpMtrHeight->Enable();
				}
				if( !mbAutoWidth && !mbAutoHeight )
					mpCbxScale->Enable();
			}
			else
			{
				mpFtWidth->Enable();
				mpMtrWidth->Enable();
				mpFtHeight->Enable();
				mpMtrHeight->Enable();
				mpCbxScale->Enable();
			}
		}
	}
}


} } // end of namespace svx::sidebar
