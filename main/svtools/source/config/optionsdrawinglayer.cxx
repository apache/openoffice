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


// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svtools.hxx"

#ifdef _MSC_VER
#pragma hdrstop
#endif

//_________________________________________________________________________________________________________________
//	includes
//_________________________________________________________________________________________________________________

#include <svtools/optionsdrawinglayer.hxx>
#include <unotools/configmgr.hxx>
#include <unotools/configitem.hxx>
#include <tools/debug.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <vcl/svapp.hxx>
#include <vcl/outdev.hxx>

//_________________________________________________________________________________________________________________
//	namespaces
//_________________________________________________________________________________________________________________

using namespace ::utl					;
using namespace ::rtl					;
using namespace ::osl					;
using namespace ::com::sun::star::uno	;

//_________________________________________________________________________________________________________________
//	const
//_________________________________________________________________________________________________________________

#define	ROOTNODE_START			   		OUString(RTL_CONSTASCII_USTRINGPARAM("Office.Common/Drawinglayer"	))
#define	DEFAULT_OVERLAYBUFFER			sal_True
#define	DEFAULT_PAINTBUFFER				sal_True
#define	DEFAULT_STRIPE_COLOR_A			0
#define	DEFAULT_STRIPE_COLOR_B			16581375
#define	DEFAULT_STRIPE_LENGTH			4

// #i73602#
// #i74769#, #i75172# : Change default for Calc and Writer to True
#define	DEFAULT_OVERLAYBUFFER_CALC			sal_True
#define	DEFAULT_OVERLAYBUFFER_WRITER		sal_True
#define	DEFAULT_OVERLAYBUFFER_DRAWIMPRESS	sal_True

// #i74769#, #i75172#
#define	DEFAULT_PAINTBUFFER_CALC			sal_True
#define	DEFAULT_PAINTBUFFER_WRITER			sal_True
#define	DEFAULT_PAINTBUFFER_DRAWIMPRESS		sal_True

// #i4219#
#define	DEFAULT_MAXIMUMPAPERWIDTH			300
#define	DEFAULT_MAXIMUMPAPERHEIGHT			300
#define	DEFAULT_MAXIMUMPAPERLEFTMARGIN		9999
#define	DEFAULT_MAXIMUMPAPERRIGHTMARGIN		9999
#define	DEFAULT_MAXIMUMPAPERTOPMARGIN		9999
#define	DEFAULT_MAXIMUMPAPERBOTTOMMARGIN	9999

// primitives
#define	DEFAULT_ANTIALIASING                        sal_True
#define	DEFAULT_SNAPHORVERLINESTODISCRETE			sal_True
#define	DEFAULT_SOLIDDRAGCREATE                     sal_True
#define	DEFAULT_RENDERDECORATEDTEXTDIRECT           sal_True
#define	DEFAULT_RENDERSIMPLETEXTDIRECT              sal_True
#define	DEFAULT_QUADRATIC3DRENDERLIMIT              1000000
#define	DEFAULT_QUADRATICFORMCONTROLRENDERLIMIT     45000

// #i97672# selection settings
#define	DEFAULT_TRANSPARENTSELECTION                sal_True
#define DEFAULT_TRANSPARENTSELECTIONPERCENT			75
#define DEFAULT_SELECTIONMAXIMUMLUMINANCEPERCENT    70

#define	PROPERTYNAME_OVERLAYBUFFER		OUString(RTL_CONSTASCII_USTRINGPARAM("OverlayBuffer"	))
#define	PROPERTYNAME_PAINTBUFFER		OUString(RTL_CONSTASCII_USTRINGPARAM("PaintBuffer"		))
#define	PROPERTYNAME_STRIPE_COLOR_A		OUString(RTL_CONSTASCII_USTRINGPARAM("StripeColorA"		))
#define	PROPERTYNAME_STRIPE_COLOR_B		OUString(RTL_CONSTASCII_USTRINGPARAM("StripeColorB"		))
#define	PROPERTYNAME_STRIPE_LENGTH		OUString(RTL_CONSTASCII_USTRINGPARAM("StripeLength"		))

// #i73602#
#define	PROPERTYNAME_OVERLAYBUFFER_CALC			OUString(RTL_CONSTASCII_USTRINGPARAM("OverlayBuffer_Calc"))
#define	PROPERTYNAME_OVERLAYBUFFER_WRITER		OUString(RTL_CONSTASCII_USTRINGPARAM("OverlayBuffer_Writer"))
#define	PROPERTYNAME_OVERLAYBUFFER_DRAWIMPRESS	OUString(RTL_CONSTASCII_USTRINGPARAM("OverlayBuffer_DrawImpress"))

// #i74769#, #i75172#
#define	PROPERTYNAME_PAINTBUFFER_CALC			OUString(RTL_CONSTASCII_USTRINGPARAM("PaintBuffer_Calc"))
#define	PROPERTYNAME_PAINTBUFFER_WRITER			OUString(RTL_CONSTASCII_USTRINGPARAM("PaintBuffer_Writer"))
#define	PROPERTYNAME_PAINTBUFFER_DRAWIMPRESS	OUString(RTL_CONSTASCII_USTRINGPARAM("PaintBuffer_DrawImpress"))

// #i4219#
#define	PROPERTYNAME_MAXIMUMPAPERWIDTH OUString(RTL_CONSTASCII_USTRINGPARAM("MaximumPaperWidth"))
#define	PROPERTYNAME_MAXIMUMPAPERHEIGHT OUString(RTL_CONSTASCII_USTRINGPARAM("MaximumPaperHeight"))
#define	PROPERTYNAME_MAXIMUMPAPERLEFTMARGIN OUString(RTL_CONSTASCII_USTRINGPARAM("MaximumPaperLeftMargin"))
#define	PROPERTYNAME_MAXIMUMPAPERRIGHTMARGIN OUString(RTL_CONSTASCII_USTRINGPARAM("MaximumPaperRightMargin"))
#define	PROPERTYNAME_MAXIMUMPAPERTOPMARGIN OUString(RTL_CONSTASCII_USTRINGPARAM("MaximumPaperTopMargin"))
#define	PROPERTYNAME_MAXIMUMPAPERBOTTOMMARGIN OUString(RTL_CONSTASCII_USTRINGPARAM("MaximumPaperBottomMargin"))

// primitives
#define	PROPERTYNAME_ANTIALIASING OUString(RTL_CONSTASCII_USTRINGPARAM("AntiAliasing"))
#define	PROPERTYNAME_SNAPHORVERLINESTODISCRETE OUString(RTL_CONSTASCII_USTRINGPARAM("SnapHorVerLinesToDiscrete"))
#define	PROPERTYNAME_SOLIDDRAGCREATE OUString(RTL_CONSTASCII_USTRINGPARAM("SolidDragCreate"))
#define	PROPERTYNAME_RENDERDECORATEDTEXTDIRECT OUString(RTL_CONSTASCII_USTRINGPARAM("RenderDecoratedTextDirect"))
#define	PROPERTYNAME_RENDERSIMPLETEXTDIRECT OUString(RTL_CONSTASCII_USTRINGPARAM("RenderSimpleTextDirect"))
#define	PROPERTYNAME_QUADRATIC3DRENDERLIMIT OUString(RTL_CONSTASCII_USTRINGPARAM("Quadratic3DRenderLimit"))
#define	PROPERTYNAME_QUADRATICFORMCONTROLRENDERLIMIT OUString(RTL_CONSTASCII_USTRINGPARAM("QuadraticFormControlRenderLimit"))

// #i97672# selection settings
#define	PROPERTYNAME_TRANSPARENTSELECTION OUString(RTL_CONSTASCII_USTRINGPARAM("TransparentSelection"))
#define	PROPERTYNAME_TRANSPARENTSELECTIONPERCENT OUString(RTL_CONSTASCII_USTRINGPARAM("TransparentSelectionPercent"))
#define	PROPERTYNAME_SELECTIONMAXIMUMLUMINANCEPERCENT OUString(RTL_CONSTASCII_USTRINGPARAM("SelectionMaximumLuminancePercent"))

#define	PROPERTYHANDLE_OVERLAYBUFFER				0
#define	PROPERTYHANDLE_PAINTBUFFER					1
#define	PROPERTYHANDLE_STRIPE_COLOR_A				2
#define	PROPERTYHANDLE_STRIPE_COLOR_B				3
#define	PROPERTYHANDLE_STRIPE_LENGTH				4

// #i73602#
#define	PROPERTYHANDLE_OVERLAYBUFFER_CALC			5
#define	PROPERTYHANDLE_OVERLAYBUFFER_WRITER			6
#define	PROPERTYHANDLE_OVERLAYBUFFER_DRAWIMPRESS	7

// #i74769#, #i75172#
#define	PROPERTYHANDLE_PAINTBUFFER_CALC				8
#define	PROPERTYHANDLE_PAINTBUFFER_WRITER			9
#define	PROPERTYHANDLE_PAINTBUFFER_DRAWIMPRESS		10

// #i4219#
#define	PROPERTYHANDLE_MAXIMUMPAPERWIDTH			11
#define	PROPERTYHANDLE_MAXIMUMPAPERHEIGHT			12
#define	PROPERTYHANDLE_MAXIMUMPAPERLEFTMARGIN		13
#define	PROPERTYHANDLE_MAXIMUMPAPERRIGHTMARGIN		14
#define	PROPERTYHANDLE_MAXIMUMPAPERTOPMARGIN		15
#define	PROPERTYHANDLE_MAXIMUMPAPERBOTTOMMARGIN		16

// primitives
#define	PROPERTYHANDLE_ANTIALIASING                     17
#define	PROPERTYHANDLE_SNAPHORVERLINESTODISCRETE        18
#define	PROPERTYHANDLE_SOLIDDRAGCREATE					19
#define	PROPERTYHANDLE_RENDERDECORATEDTEXTDIRECT        20
#define	PROPERTYHANDLE_RENDERSIMPLETEXTDIRECT           21
#define	PROPERTYHANDLE_QUADRATIC3DRENDERLIMIT           22
#define	PROPERTYHANDLE_QUADRATICFORMCONTROLRENDERLIMIT  23

// #i97672# selection settings
#define	PROPERTYHANDLE_TRANSPARENTSELECTION             24
#define	PROPERTYHANDLE_TRANSPARENTSELECTIONPERCENT      25
#define	PROPERTYHANDLE_SELECTIONMAXIMUMLUMINANCEPERCENT 26

#define	PROPERTYCOUNT								27

class SvtOptionsDrawinglayer_Impl : public ConfigItem
{
public:

//---------------------------------------------------------------------------------------------------------
//	constructor / destructor
//---------------------------------------------------------------------------------------------------------

	 SvtOptionsDrawinglayer_Impl();
	~SvtOptionsDrawinglayer_Impl();

//---------------------------------------------------------------------------------------------------------
//	overloaded methods of baseclass
//---------------------------------------------------------------------------------------------------------

	virtual void Commit();
    virtual void Notify( const com::sun::star::uno::Sequence<rtl::OUString>& aPropertyNames);

//---------------------------------------------------------------------------------------------------------
//	public interface
//---------------------------------------------------------------------------------------------------------

	sal_Bool	IsOverlayBuffer() const;
	sal_Bool	IsPaintBuffer() const;
	Color		GetStripeColorA() const;
	Color		GetStripeColorB() const;
	sal_uInt16	GetStripeLength() const;

	// #i73602#
	sal_Bool	IsOverlayBuffer_Calc() const;
	sal_Bool	IsOverlayBuffer_Writer() const;
	sal_Bool	IsOverlayBuffer_DrawImpress() const;

	// #i74769#, #i75172#
	sal_Bool	IsPaintBuffer_Calc() const;
	sal_Bool	IsPaintBuffer_Writer() const;
	sal_Bool	IsPaintBuffer_DrawImpress() const;

	void		SetOverlayBuffer( sal_Bool bState );
	void		SetPaintBuffer( sal_Bool bState );
	void		SetStripeColorA( Color aColor );
	void		SetStripeColorB( Color aColor );
	void		SetStripeLength( sal_uInt16 nLength );

	// #i73602#
	void		SetOverlayBuffer_Calc( sal_Bool bState );
	void		SetOverlayBuffer_Writer( sal_Bool bState );
	void		SetOverlayBuffer_DrawImpress( sal_Bool bState );

	// #i74769#, #i75172#
	void		SetPaintBuffer_Calc( sal_Bool bState );
	void		SetPaintBuffer_Writer( sal_Bool bState );
	void		SetPaintBuffer_DrawImpress( sal_Bool bState );

	// #i4219#
	sal_uInt32 GetMaximumPaperWidth() const;
	sal_uInt32 GetMaximumPaperHeight() const;
	sal_uInt32 GetMaximumPaperLeftMargin() const;
	sal_uInt32 GetMaximumPaperRightMargin() const;
	sal_uInt32 GetMaximumPaperTopMargin() const;
	sal_uInt32 GetMaximumPaperBottomMargin() const;

	void SetMaximumPaperWidth(sal_uInt32 nNew);
	void SetMaximumPaperHeight(sal_uInt32 nNew);
	void SetMaximumPaperLeftMargin(sal_uInt32 nNew);
	void SetMaximumPaperRightMargin(sal_uInt32 nNew);
	void SetMaximumPaperTopMargin(sal_uInt32 nNew);
	void SetMaximumPaperBottomMargin(sal_uInt32 nNew);

    // helper
    sal_Bool IsAAPossibleOnThisSystem() const;

    // primitives
	sal_Bool	IsAntiAliasing() const;
	sal_Bool	IsSnapHorVerLinesToDiscrete() const;
	sal_Bool	IsSolidDragCreate() const;
	sal_Bool	IsRenderDecoratedTextDirect() const;
	sal_Bool	IsRenderSimpleTextDirect() const;
	sal_uInt32  GetQuadratic3DRenderLimit() const;
	sal_uInt32  GetQuadraticFormControlRenderLimit() const;

    void		SetAntiAliasing( sal_Bool bState );
    void		SetSnapHorVerLinesToDiscrete( sal_Bool bState );
	void		SetSolidDragCreate( sal_Bool bState );
	void		SetRenderDecoratedTextDirect( sal_Bool bState );
	void		SetRenderSimpleTextDirect( sal_Bool bState );
	void        SetQuadratic3DRenderLimit(sal_uInt32 nNew);
	void        SetQuadraticFormControlRenderLimit(sal_uInt32 nNew);

	// #i97672# selection settings
	sal_Bool	IsTransparentSelection() const;
	sal_uInt16	GetTransparentSelectionPercent() const;
	sal_uInt16	GetSelectionMaximumLuminancePercent() const;

	void		SetTransparentSelection( sal_Bool bState );
	void		SetTransparentSelectionPercent( sal_uInt16 nPercent );
	void		SetSelectionMaximumLuminancePercent( sal_uInt16 nPercent );

//-------------------------------------------------------------------------------------------------------------
//	private methods
//-------------------------------------------------------------------------------------------------------------

private:

	static Sequence< OUString > impl_GetPropertyNames();

//-------------------------------------------------------------------------------------------------------------
//	private member
//-------------------------------------------------------------------------------------------------------------

private:

		sal_Bool	m_bOverlayBuffer;
		sal_Bool	m_bPaintBuffer;
		Color		m_bStripeColorA;
		Color		m_bStripeColorB;
		sal_uInt16	m_nStripeLength;

		// #i73602#
		sal_Bool	m_bOverlayBuffer_Calc;
		sal_Bool	m_bOverlayBuffer_Writer;
		sal_Bool	m_bOverlayBuffer_DrawImpress;

		// #i74769#, #i75172#
		sal_Bool	m_bPaintBuffer_Calc;
		sal_Bool	m_bPaintBuffer_Writer;
		sal_Bool	m_bPaintBuffer_DrawImpress;

		// #i4219#
		sal_uInt32	m_nMaximumPaperWidth;
		sal_uInt32	m_nMaximumPaperHeight;
		sal_uInt32	m_nMaximumPaperLeftMargin;
		sal_uInt32	m_nMaximumPaperRightMargin;
		sal_uInt32	m_nMaximumPaperTopMargin;
		sal_uInt32	m_nMaximumPaperBottomMargin;

        // primitives
		sal_Bool	m_bAntiAliasing;
		sal_Bool	m_bSnapHorVerLinesToDiscrete;
		sal_Bool	m_bSolidDragCreate;
		sal_Bool	m_bRenderDecoratedTextDirect;
		sal_Bool	m_bRenderSimpleTextDirect;
		sal_uInt32	m_nQuadratic3DRenderLimit;
		sal_uInt32	m_nQuadraticFormControlRenderLimit;

		// #i97672# selection settings
		sal_uInt16	m_nTransparentSelectionPercent;
		sal_uInt16	m_nSelectionMaximumLuminancePercent;
		sal_Bool	m_bTransparentSelection;

        // local values
        bool        m_bAllowAA : 1;
        bool        m_bAllowAAChecked : 1;
};

//_________________________________________________________________________________________________________________
//	definitions
//_________________________________________________________________________________________________________________

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl() :
	ConfigItem( ROOTNODE_START	),
	m_bOverlayBuffer( DEFAULT_OVERLAYBUFFER ),
	m_bPaintBuffer( DEFAULT_PAINTBUFFER ),
	m_bStripeColorA(Color(DEFAULT_STRIPE_COLOR_A)),
	m_bStripeColorB(Color(DEFAULT_STRIPE_COLOR_B)),
	m_nStripeLength(DEFAULT_STRIPE_LENGTH),

	// #i73602#
	m_bOverlayBuffer_Calc( DEFAULT_OVERLAYBUFFER_CALC ),
	m_bOverlayBuffer_Writer( DEFAULT_OVERLAYBUFFER_WRITER ),
	m_bOverlayBuffer_DrawImpress( DEFAULT_OVERLAYBUFFER_DRAWIMPRESS ),

	// #i74769#, #i75172#
	m_bPaintBuffer_Calc( DEFAULT_PAINTBUFFER_CALC ),
	m_bPaintBuffer_Writer( DEFAULT_PAINTBUFFER_WRITER ),
	m_bPaintBuffer_DrawImpress( DEFAULT_PAINTBUFFER_DRAWIMPRESS ),

	// #i4219#
	m_nMaximumPaperWidth(DEFAULT_MAXIMUMPAPERWIDTH),
	m_nMaximumPaperHeight(DEFAULT_MAXIMUMPAPERHEIGHT),
	m_nMaximumPaperLeftMargin(DEFAULT_MAXIMUMPAPERLEFTMARGIN),
	m_nMaximumPaperRightMargin(DEFAULT_MAXIMUMPAPERRIGHTMARGIN),
	m_nMaximumPaperTopMargin(DEFAULT_MAXIMUMPAPERTOPMARGIN),
	m_nMaximumPaperBottomMargin(DEFAULT_MAXIMUMPAPERBOTTOMMARGIN),

    // primitives
	m_bAntiAliasing(DEFAULT_ANTIALIASING),
	m_bSnapHorVerLinesToDiscrete(DEFAULT_SNAPHORVERLINESTODISCRETE),
	m_bSolidDragCreate(DEFAULT_SOLIDDRAGCREATE),
	m_bRenderDecoratedTextDirect(DEFAULT_RENDERDECORATEDTEXTDIRECT),
	m_bRenderSimpleTextDirect(DEFAULT_RENDERSIMPLETEXTDIRECT),
	m_nQuadratic3DRenderLimit(DEFAULT_QUADRATIC3DRENDERLIMIT),
	m_nQuadraticFormControlRenderLimit(DEFAULT_QUADRATICFORMCONTROLRENDERLIMIT),

	// #i97672# selection settings
	m_nTransparentSelectionPercent(DEFAULT_TRANSPARENTSELECTIONPERCENT),
	m_nSelectionMaximumLuminancePercent(DEFAULT_SELECTIONMAXIMUMLUMINANCEPERCENT),
	m_bTransparentSelection(DEFAULT_TRANSPARENTSELECTION),

    // local values
    m_bAllowAA(true),
    m_bAllowAAChecked(false)
{
	Sequence< OUString >	seqNames( impl_GetPropertyNames() );
	Sequence< Any >			seqValues	= GetProperties( seqNames )	;

	DBG_ASSERT( !(seqNames.getLength()!=seqValues.getLength()), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nI miss some values of configuration keys!\n" );

	// Copy values from list in right order to our internal member.
	sal_Int32 nPropertyCount = seqValues.getLength();
	sal_Int32 nProperty	= 0;

	for( nProperty=0; nProperty<nPropertyCount; ++nProperty )
	{
		DBG_ASSERT( !(seqValues[nProperty].hasValue()==sal_False), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nInvalid property value for property detected!\n" );

        switch( nProperty )
        {
            case PROPERTYHANDLE_OVERLAYBUFFER:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\OverlayBuffer\"?" );
				seqValues[nProperty] >>= m_bOverlayBuffer;
			}
			break;

            case PROPERTYHANDLE_PAINTBUFFER:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\PaintBuffer\"?" );
				seqValues[nProperty] >>= m_bPaintBuffer;
			}
			break;

			case PROPERTYHANDLE_STRIPE_COLOR_A:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\StripeColorA\"?" );
				sal_Int32 nValue = 0;
				seqValues[nProperty] >>= nValue;
				m_bStripeColorA = nValue;
			}
			break;

			case PROPERTYHANDLE_STRIPE_COLOR_B:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\StripeColorB\"?" );
				sal_Int32 nValue = 0;
				seqValues[nProperty] >>= nValue;
				m_bStripeColorB = nValue;
			}
			break;

			case PROPERTYHANDLE_STRIPE_LENGTH:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_SHORT), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\StripeLength\"?" );
				seqValues[nProperty] >>= m_nStripeLength;
			}
			break;

			// #i73602#
            case PROPERTYHANDLE_OVERLAYBUFFER_CALC:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\OverlayBuffer_Calc\"?" );
				seqValues[nProperty] >>= m_bOverlayBuffer_Calc;
			}
			break;

			case PROPERTYHANDLE_OVERLAYBUFFER_WRITER:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\OverlayBuffer_Writer\"?" );
				seqValues[nProperty] >>= m_bOverlayBuffer_Writer;
			}
			break;

			case PROPERTYHANDLE_OVERLAYBUFFER_DRAWIMPRESS:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\OverlayBuffer_DrawImpress\"?" );
				seqValues[nProperty] >>= m_bOverlayBuffer_DrawImpress;
			}
			break;

			// #i74769#, #i75172#
            case PROPERTYHANDLE_PAINTBUFFER_CALC:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\PaintBuffer_Calc\"?" );
				seqValues[nProperty] >>= m_bPaintBuffer_Calc;
			}
			break;

			case PROPERTYHANDLE_PAINTBUFFER_WRITER:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\PaintBuffer_Writer\"?" );
				seqValues[nProperty] >>= m_bPaintBuffer_Writer;
			}
			break;

			case PROPERTYHANDLE_PAINTBUFFER_DRAWIMPRESS:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\PaintBuffer_DrawImpress\"?" );
				seqValues[nProperty] >>= m_bPaintBuffer_DrawImpress;
			}
			break;

			// #i4219#
			case PROPERTYHANDLE_MAXIMUMPAPERWIDTH:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\MaximumPaperWidth\"?" );
				seqValues[nProperty] >>= m_nMaximumPaperWidth;
			}
			break;

			case PROPERTYHANDLE_MAXIMUMPAPERHEIGHT:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\MaximumPaperHeight\"?" );
				seqValues[nProperty] >>= m_nMaximumPaperHeight;
			}
			break;

			case PROPERTYHANDLE_MAXIMUMPAPERLEFTMARGIN:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\MaximumPaperLeftMargin\"?" );
				seqValues[nProperty] >>= m_nMaximumPaperLeftMargin;
			}
			break;

			case PROPERTYHANDLE_MAXIMUMPAPERRIGHTMARGIN:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\MaximumPaperRightMargin\"?" );
				seqValues[nProperty] >>= m_nMaximumPaperRightMargin;
			}
			break;

			case PROPERTYHANDLE_MAXIMUMPAPERTOPMARGIN:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\MaximumPaperTopMargin\"?" );
				seqValues[nProperty] >>= m_nMaximumPaperTopMargin;
			}
			break;

			case PROPERTYHANDLE_MAXIMUMPAPERBOTTOMMARGIN:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\MaximumPaperBottomMargin\"?" );
				seqValues[nProperty] >>= m_nMaximumPaperBottomMargin;
			}
			break;

            // primitives
            case PROPERTYHANDLE_ANTIALIASING:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\AntiAliasing\"?" );
				seqValues[nProperty] >>= m_bAntiAliasing;
			}
			break;

            // primitives
            case PROPERTYHANDLE_SNAPHORVERLINESTODISCRETE:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\SnapHorVerLinesToDiscrete\"?" );
				seqValues[nProperty] >>= m_bSnapHorVerLinesToDiscrete;
			}
			break;

            case PROPERTYHANDLE_SOLIDDRAGCREATE:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\SolidDragCreate\"?" );
				seqValues[nProperty] >>= m_bSolidDragCreate;
			}
			break;

            case PROPERTYHANDLE_RENDERDECORATEDTEXTDIRECT:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\RenderDecoratedTextDirect\"?" );
				seqValues[nProperty] >>= m_bRenderDecoratedTextDirect;
			}
			break;

            case PROPERTYHANDLE_RENDERSIMPLETEXTDIRECT:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\RenderSimpleTextDirect\"?" );
				seqValues[nProperty] >>= m_bRenderSimpleTextDirect;
			}
			break;

            case PROPERTYHANDLE_QUADRATIC3DRENDERLIMIT:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\Quadratic3DRenderLimit\"?" );
				seqValues[nProperty] >>= m_nQuadratic3DRenderLimit;
			}
			break;

            case PROPERTYHANDLE_QUADRATICFORMCONTROLRENDERLIMIT:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_LONG), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\QuadraticFormControlRenderLimit\"?" );
				seqValues[nProperty] >>= m_nQuadraticFormControlRenderLimit;
			}
			break;

			// #i97672# selection settings
            case PROPERTYHANDLE_TRANSPARENTSELECTION:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_BOOLEAN), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\TransparentSelection\"?" );
				seqValues[nProperty] >>= m_bTransparentSelection;
			}
			break;
			
			case PROPERTYHANDLE_TRANSPARENTSELECTIONPERCENT:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_SHORT), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\TransparentSelectionPercent\"?" );
				seqValues[nProperty] >>= m_nTransparentSelectionPercent;
			}
			
			case PROPERTYHANDLE_SELECTIONMAXIMUMLUMINANCEPERCENT:
			{
				DBG_ASSERT(!(seqValues[nProperty].getValueTypeClass()!=TypeClass_SHORT), "SvtOptionsDrawinglayer_Impl::SvtOptionsDrawinglayer_Impl()\nWho has changed the value type of \"Office.Common\\Drawinglayer\\SelectionMaximumLuminancePercent\"?" );
				seqValues[nProperty] >>= m_nSelectionMaximumLuminancePercent;
			}
			break;
        }
	}
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
SvtOptionsDrawinglayer_Impl::~SvtOptionsDrawinglayer_Impl()
{
	if( IsModified() )
		Commit();
}

//*****************************************************************************************************************
//	Commit
//*****************************************************************************************************************
void SvtOptionsDrawinglayer_Impl::Commit()
{
	Sequence< OUString >	aSeqNames( impl_GetPropertyNames() );
	Sequence< Any >			aSeqValues( aSeqNames.getLength() );

	for( sal_Int32 nProperty = 0, nCount = aSeqNames.getLength(); nProperty < nCount; ++nProperty )
	{
        switch( nProperty )
        {
            case PROPERTYHANDLE_OVERLAYBUFFER:
				aSeqValues[nProperty] <<= m_bOverlayBuffer;
			break;

            case PROPERTYHANDLE_PAINTBUFFER:
				aSeqValues[nProperty] <<= m_bPaintBuffer;
			break;

			case PROPERTYHANDLE_STRIPE_COLOR_A:
				aSeqValues[nProperty] <<= m_bStripeColorA.GetColor();
			break;

			case PROPERTYHANDLE_STRIPE_COLOR_B:
				aSeqValues[nProperty] <<= m_bStripeColorB.GetColor();
			break;

			case PROPERTYHANDLE_STRIPE_LENGTH:
				aSeqValues[nProperty] <<= m_nStripeLength;
			break;

			// #i73602#
            case PROPERTYHANDLE_OVERLAYBUFFER_CALC:
				aSeqValues[nProperty] <<= m_bOverlayBuffer_Calc;
			break;

			case PROPERTYHANDLE_OVERLAYBUFFER_WRITER:
				aSeqValues[nProperty] <<= m_bOverlayBuffer_Writer;
			break;

			case PROPERTYHANDLE_OVERLAYBUFFER_DRAWIMPRESS:
				aSeqValues[nProperty] <<= m_bOverlayBuffer_DrawImpress;
			break;

			// #i74769#, #i75172#
            case PROPERTYHANDLE_PAINTBUFFER_CALC:
				aSeqValues[nProperty] <<= m_bPaintBuffer_Calc;
			break;

			case PROPERTYHANDLE_PAINTBUFFER_WRITER:
				aSeqValues[nProperty] <<= m_bPaintBuffer_Writer;
			break;

			case PROPERTYHANDLE_PAINTBUFFER_DRAWIMPRESS:
				aSeqValues[nProperty] <<= m_bPaintBuffer_DrawImpress;
			break;

			// #i4219#
			case PROPERTYHANDLE_MAXIMUMPAPERWIDTH:
				aSeqValues[nProperty] <<= m_nMaximumPaperWidth;
			break;

			case PROPERTYHANDLE_MAXIMUMPAPERHEIGHT:
				aSeqValues[nProperty] <<= m_nMaximumPaperHeight;
			break;

			case PROPERTYHANDLE_MAXIMUMPAPERLEFTMARGIN:
				aSeqValues[nProperty] <<= m_nMaximumPaperLeftMargin;
			break;

			case PROPERTYHANDLE_MAXIMUMPAPERRIGHTMARGIN:
				aSeqValues[nProperty] <<= m_nMaximumPaperRightMargin;
			break;

			case PROPERTYHANDLE_MAXIMUMPAPERTOPMARGIN:
				aSeqValues[nProperty] <<= m_nMaximumPaperTopMargin;
			break;

			case PROPERTYHANDLE_MAXIMUMPAPERBOTTOMMARGIN:
				aSeqValues[nProperty] <<= m_nMaximumPaperBottomMargin;
			break;

            // primitives
            case PROPERTYHANDLE_ANTIALIASING:
				aSeqValues[nProperty] <<= m_bAntiAliasing;
			break;

            case PROPERTYHANDLE_SNAPHORVERLINESTODISCRETE:
				aSeqValues[nProperty] <<= m_bSnapHorVerLinesToDiscrete;
			break;

            case PROPERTYHANDLE_SOLIDDRAGCREATE:
				aSeqValues[nProperty] <<= m_bSolidDragCreate;
			break;

            case PROPERTYHANDLE_RENDERDECORATEDTEXTDIRECT:
				aSeqValues[nProperty] <<= m_bRenderDecoratedTextDirect;
			break;

            case PROPERTYHANDLE_RENDERSIMPLETEXTDIRECT:
				aSeqValues[nProperty] <<= m_bRenderSimpleTextDirect;
			break;

            case PROPERTYHANDLE_QUADRATIC3DRENDERLIMIT:
				aSeqValues[nProperty] <<= m_nQuadratic3DRenderLimit;
			break;

            case PROPERTYHANDLE_QUADRATICFORMCONTROLRENDERLIMIT:
				aSeqValues[nProperty] <<= m_nQuadraticFormControlRenderLimit;
			break;

			// #i97672# selection settings
            case PROPERTYHANDLE_TRANSPARENTSELECTION:
				aSeqValues[nProperty] <<= m_bTransparentSelection;
			break;

			case PROPERTYHANDLE_TRANSPARENTSELECTIONPERCENT:
				aSeqValues[nProperty] <<= m_nTransparentSelectionPercent;
			break;

			case PROPERTYHANDLE_SELECTIONMAXIMUMLUMINANCEPERCENT:
				aSeqValues[nProperty] <<= m_nSelectionMaximumLuminancePercent;
			break;
        }
	}

	PutProperties( aSeqNames, aSeqValues );
}

void SvtOptionsDrawinglayer_Impl::Notify( const com::sun::star::uno::Sequence<rtl::OUString>& )
{
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtOptionsDrawinglayer_Impl::IsOverlayBuffer() const
{
	return m_bOverlayBuffer;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtOptionsDrawinglayer_Impl::IsPaintBuffer() const
{
	return m_bPaintBuffer;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
Color SvtOptionsDrawinglayer_Impl::GetStripeColorA() const
{
	return m_bStripeColorA;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
Color SvtOptionsDrawinglayer_Impl::GetStripeColorB() const
{
	return m_bStripeColorB;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_uInt16 SvtOptionsDrawinglayer_Impl::GetStripeLength() const
{
	return m_nStripeLength;
}

// #i73602#
sal_Bool SvtOptionsDrawinglayer_Impl::IsOverlayBuffer_Calc() const
{
	return m_bOverlayBuffer_Calc;
}

sal_Bool SvtOptionsDrawinglayer_Impl::IsOverlayBuffer_Writer() const
{
	return m_bOverlayBuffer_Writer;
}

sal_Bool SvtOptionsDrawinglayer_Impl::IsOverlayBuffer_DrawImpress() const
{
	return m_bOverlayBuffer_DrawImpress;
}

// #i74769#, #i75172#
sal_Bool SvtOptionsDrawinglayer_Impl::IsPaintBuffer_Calc() const
{
	return m_bPaintBuffer_Calc;
}

sal_Bool SvtOptionsDrawinglayer_Impl::IsPaintBuffer_Writer() const
{
	return m_bPaintBuffer_Writer;
}

sal_Bool SvtOptionsDrawinglayer_Impl::IsPaintBuffer_DrawImpress() const
{
	return m_bPaintBuffer_DrawImpress;
}

// #i4219#
sal_uInt32 SvtOptionsDrawinglayer_Impl::GetMaximumPaperWidth() const
{
	return m_nMaximumPaperWidth;
}

sal_uInt32 SvtOptionsDrawinglayer_Impl::GetMaximumPaperHeight() const
{
	return m_nMaximumPaperHeight;
}

sal_uInt32 SvtOptionsDrawinglayer_Impl::GetMaximumPaperLeftMargin() const
{
	return m_nMaximumPaperLeftMargin;
}

sal_uInt32 SvtOptionsDrawinglayer_Impl::GetMaximumPaperRightMargin() const
{
	return m_nMaximumPaperRightMargin;
}

sal_uInt32 SvtOptionsDrawinglayer_Impl::GetMaximumPaperTopMargin() const
{
	return m_nMaximumPaperTopMargin;
}

sal_uInt32 SvtOptionsDrawinglayer_Impl::GetMaximumPaperBottomMargin() const
{
	return m_nMaximumPaperBottomMargin;
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer_Impl::SetOverlayBuffer( sal_Bool bState )
{
	if(m_bOverlayBuffer != bState)
	{
		m_bOverlayBuffer = bState;
		SetModified();
	}
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer_Impl::SetPaintBuffer( sal_Bool bState )
{
	if(m_bPaintBuffer != bState)
	{
		m_bPaintBuffer = bState;
		SetModified();
	}
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer_Impl::SetStripeColorA( Color aColor )
{
	if(m_bStripeColorA != aColor)
	{
		m_bStripeColorA = aColor;
		SetModified();
	}
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer_Impl::SetStripeColorB( Color aColor )
{
	if(m_bStripeColorB != aColor)
	{
		m_bStripeColorB = aColor;
		SetModified();
	}
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer_Impl::SetStripeLength( sal_uInt16 nLength )
{
	if(m_nStripeLength != nLength)
	{
		m_nStripeLength = nLength;
		SetModified();
	}
}

// #i73602#
void SvtOptionsDrawinglayer_Impl::SetOverlayBuffer_Calc( sal_Bool bState )
{
	if(m_bOverlayBuffer_Calc != bState)
	{
		m_bOverlayBuffer_Calc = bState;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetOverlayBuffer_Writer( sal_Bool bState )
{
	if(m_bOverlayBuffer_Writer != bState)
	{
		m_bOverlayBuffer_Writer = bState;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetOverlayBuffer_DrawImpress( sal_Bool bState )
{
	if(m_bOverlayBuffer_DrawImpress != bState)
	{
		m_bOverlayBuffer_DrawImpress = bState;
		SetModified();
	}
}

// #i74769#, #i75172#
void SvtOptionsDrawinglayer_Impl::SetPaintBuffer_Calc( sal_Bool bState )
{
	if(m_bPaintBuffer_Calc != bState)
	{
		m_bPaintBuffer_Calc = bState;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetPaintBuffer_Writer( sal_Bool bState )
{
	if(m_bPaintBuffer_Writer != bState)
	{
		m_bPaintBuffer_Writer = bState;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetPaintBuffer_DrawImpress( sal_Bool bState )
{
	if(m_bPaintBuffer_DrawImpress != bState)
	{
		m_bPaintBuffer_DrawImpress = bState;
		SetModified();
	}
}

// #i4219#
void SvtOptionsDrawinglayer_Impl::SetMaximumPaperWidth( sal_uInt32 nNew )
{
	if(m_nMaximumPaperWidth != nNew)
	{
		m_nMaximumPaperWidth = nNew;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetMaximumPaperHeight( sal_uInt32 nNew )
{
	if(m_nMaximumPaperHeight != nNew)
	{
		m_nMaximumPaperHeight = nNew;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetMaximumPaperLeftMargin( sal_uInt32 nNew )
{
	if(m_nMaximumPaperLeftMargin != nNew)
	{
		m_nMaximumPaperLeftMargin = nNew;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetMaximumPaperRightMargin( sal_uInt32 nNew )
{
	if(m_nMaximumPaperRightMargin != nNew)
	{
		m_nMaximumPaperRightMargin = nNew;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetMaximumPaperTopMargin( sal_uInt32 nNew )
{
	if(m_nMaximumPaperTopMargin != nNew)
	{
		m_nMaximumPaperTopMargin = nNew;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetMaximumPaperBottomMargin( sal_uInt32 nNew )
{
	if(m_nMaximumPaperBottomMargin != nNew)
	{
		m_nMaximumPaperBottomMargin = nNew;
		SetModified();
	}
}

// helper
sal_Bool SvtOptionsDrawinglayer_Impl::IsAAPossibleOnThisSystem() const
{
    if(!m_bAllowAAChecked)
    {
        SvtOptionsDrawinglayer_Impl* pThat = const_cast< SvtOptionsDrawinglayer_Impl* >(this);
        pThat->m_bAllowAAChecked = true;

#ifdef WIN32
        // WIN32 uses GDIPlus with VCL forthe first incarnation; this will be enhanced
        // in the future to use canvases and the canvas renderer, thus a AA-abled
        // canvas needs to be checked here in the future.
        // Currently, just allow AA for WIN32
#endif

	    // check XRenderExtension
	    if(m_bAllowAA && !Application::GetDefaultDevice()->supportsOperation( OutDevSupport_TransparentRect ))
	    {
		    pThat->m_bAllowAA = false;
	    }
    }

    return m_bAllowAA;
}

// primitives
sal_Bool SvtOptionsDrawinglayer_Impl::IsAntiAliasing() const
{
	return m_bAntiAliasing;
}

sal_Bool SvtOptionsDrawinglayer_Impl::IsSnapHorVerLinesToDiscrete() const
{
	return m_bSnapHorVerLinesToDiscrete;
}

sal_Bool SvtOptionsDrawinglayer_Impl::IsSolidDragCreate() const
{
	return m_bSolidDragCreate;
}

sal_Bool SvtOptionsDrawinglayer_Impl::IsRenderDecoratedTextDirect() const
{
	return m_bRenderDecoratedTextDirect;
}

sal_Bool SvtOptionsDrawinglayer_Impl::IsRenderSimpleTextDirect() const
{
	return m_bRenderSimpleTextDirect;
}

sal_uInt32 SvtOptionsDrawinglayer_Impl::GetQuadratic3DRenderLimit() const
{
	return m_nQuadratic3DRenderLimit;
}

sal_uInt32 SvtOptionsDrawinglayer_Impl::GetQuadraticFormControlRenderLimit() const
{
	return m_nQuadraticFormControlRenderLimit;
}

void SvtOptionsDrawinglayer_Impl::SetAntiAliasing( sal_Bool bState )
{
	if(m_bAntiAliasing != bState)
	{
		m_bAntiAliasing = bState;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetSnapHorVerLinesToDiscrete( sal_Bool bState )
{
	if(m_bSnapHorVerLinesToDiscrete != bState)
	{
		m_bSnapHorVerLinesToDiscrete = bState;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetSolidDragCreate( sal_Bool bState )
{
	if(m_bSolidDragCreate != bState)
	{
		m_bSolidDragCreate = bState;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetRenderDecoratedTextDirect( sal_Bool bState )
{
	if(m_bRenderDecoratedTextDirect != bState)
	{
		m_bRenderDecoratedTextDirect = bState;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetRenderSimpleTextDirect( sal_Bool bState )
{
	if(m_bRenderSimpleTextDirect != bState)
	{
		m_bRenderSimpleTextDirect = bState;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetQuadratic3DRenderLimit(sal_uInt32 nNew)
{
	if(m_nQuadratic3DRenderLimit != nNew)
	{
		m_nQuadratic3DRenderLimit = nNew;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetQuadraticFormControlRenderLimit(sal_uInt32 nNew)
{
	if(m_nQuadraticFormControlRenderLimit != nNew)
	{
		m_nQuadraticFormControlRenderLimit = nNew;
		SetModified();
	}
}

// #i97672# selection settings
sal_Bool SvtOptionsDrawinglayer_Impl::IsTransparentSelection() const
{
	return m_bTransparentSelection;
}

void SvtOptionsDrawinglayer_Impl::SetTransparentSelection( sal_Bool bState )
{
	if(m_bTransparentSelection != bState)
	{
		m_bTransparentSelection = bState;
		SetModified();
	}
}

void SvtOptionsDrawinglayer_Impl::SetTransparentSelectionPercent( sal_uInt16 nPercent )
{
	if(m_nTransparentSelectionPercent != nPercent)
	{
		m_nTransparentSelectionPercent = nPercent;
		SetModified();
	}
}

sal_uInt16 SvtOptionsDrawinglayer_Impl::GetTransparentSelectionPercent() const
{
	return m_nTransparentSelectionPercent;
}

void SvtOptionsDrawinglayer_Impl::SetSelectionMaximumLuminancePercent( sal_uInt16 nPercent )
{
	if(m_nSelectionMaximumLuminancePercent != nPercent)
	{
		m_nSelectionMaximumLuminancePercent = nPercent;
		SetModified();
	}
}

sal_uInt16 SvtOptionsDrawinglayer_Impl::GetSelectionMaximumLuminancePercent() const
{
	return m_nSelectionMaximumLuminancePercent;
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Sequence< OUString > SvtOptionsDrawinglayer_Impl::impl_GetPropertyNames()
{
	// Build static list of configuration key names.
	static const OUString pProperties[] =
	{
		PROPERTYNAME_OVERLAYBUFFER		,
		PROPERTYNAME_PAINTBUFFER		,
		PROPERTYNAME_STRIPE_COLOR_A		,
		PROPERTYNAME_STRIPE_COLOR_B		,
		PROPERTYNAME_STRIPE_LENGTH		,

		// #i73602#
		PROPERTYNAME_OVERLAYBUFFER_CALC,
		PROPERTYNAME_OVERLAYBUFFER_WRITER,
		PROPERTYNAME_OVERLAYBUFFER_DRAWIMPRESS,

		// #i74769#, #i75172#
		PROPERTYNAME_PAINTBUFFER_CALC,
		PROPERTYNAME_PAINTBUFFER_WRITER,
		PROPERTYNAME_PAINTBUFFER_DRAWIMPRESS,

		// #i4219#
		PROPERTYNAME_MAXIMUMPAPERWIDTH,
		PROPERTYNAME_MAXIMUMPAPERHEIGHT,
		PROPERTYNAME_MAXIMUMPAPERLEFTMARGIN,
		PROPERTYNAME_MAXIMUMPAPERRIGHTMARGIN,
		PROPERTYNAME_MAXIMUMPAPERTOPMARGIN,
		PROPERTYNAME_MAXIMUMPAPERBOTTOMMARGIN,

        // primitives
        PROPERTYNAME_ANTIALIASING,
		PROPERTYNAME_SNAPHORVERLINESTODISCRETE,
        PROPERTYNAME_SOLIDDRAGCREATE,
        PROPERTYNAME_RENDERDECORATEDTEXTDIRECT,
        PROPERTYNAME_RENDERSIMPLETEXTDIRECT,
        PROPERTYNAME_QUADRATIC3DRENDERLIMIT,
        PROPERTYNAME_QUADRATICFORMCONTROLRENDERLIMIT,
		
		// #i97672# selection settings
		PROPERTYNAME_TRANSPARENTSELECTION,
		PROPERTYNAME_TRANSPARENTSELECTIONPERCENT,
		PROPERTYNAME_SELECTIONMAXIMUMLUMINANCEPERCENT
    };

	// Initialize return sequence with these list ...
	static const Sequence< OUString > seqPropertyNames( pProperties, PROPERTYCOUNT );
	// ... and return it.
	return seqPropertyNames;
}

//*****************************************************************************************************************
//	initialize static member
//	DON'T DO IT IN YOUR HEADER!
//	see definition for further informations
//*****************************************************************************************************************
SvtOptionsDrawinglayer_Impl* SvtOptionsDrawinglayer::m_pDataContainer = NULL;
sal_Int32 SvtOptionsDrawinglayer::m_nRefCount = 0;

//*****************************************************************************************************************
//	constructor
//*****************************************************************************************************************
SvtOptionsDrawinglayer::SvtOptionsDrawinglayer()
{
    // Global access, must be guarded (multithreading!).
    MutexGuard aGuard( GetOwnStaticMutex() );
	// Increase our refcount ...
	++m_nRefCount;
	// ... and initialize our data container only if it not already!
    if( m_pDataContainer == NULL )
	{
        m_pDataContainer = new SvtOptionsDrawinglayer_Impl();
	}
}

//*****************************************************************************************************************
//	destructor
//*****************************************************************************************************************
SvtOptionsDrawinglayer::~SvtOptionsDrawinglayer()
{
    // Global access, must be guarded (multithreading!)
    MutexGuard aGuard( GetOwnStaticMutex() );
	// Decrease our refcount.
	--m_nRefCount;
	// If last instance was deleted ...
	// we must destroy our static data container!
    if( m_nRefCount <= 0 )
	{
		delete m_pDataContainer;
		m_pDataContainer = NULL;
	}
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtOptionsDrawinglayer::IsOverlayBuffer() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsOverlayBuffer();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_Bool SvtOptionsDrawinglayer::IsPaintBuffer() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsPaintBuffer();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
Color SvtOptionsDrawinglayer::GetStripeColorA() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetStripeColorA();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
Color SvtOptionsDrawinglayer::GetStripeColorB() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetStripeColorB();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
sal_uInt16 SvtOptionsDrawinglayer::GetStripeLength() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetStripeLength();
}

// #i73602#
sal_Bool SvtOptionsDrawinglayer::IsOverlayBuffer_Calc() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsOverlayBuffer_Calc();
}

sal_Bool SvtOptionsDrawinglayer::IsOverlayBuffer_Writer() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsOverlayBuffer_Writer();
}

sal_Bool SvtOptionsDrawinglayer::IsOverlayBuffer_DrawImpress() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsOverlayBuffer_DrawImpress();
}

// #i74769#, #i75172#
sal_Bool SvtOptionsDrawinglayer::IsPaintBuffer_Calc() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsPaintBuffer_Calc();
}

sal_Bool SvtOptionsDrawinglayer::IsPaintBuffer_Writer() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsPaintBuffer_Writer();
}

sal_Bool SvtOptionsDrawinglayer::IsPaintBuffer_DrawImpress() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsPaintBuffer_DrawImpress();
}

// #i4219#
sal_uInt32 SvtOptionsDrawinglayer::GetMaximumPaperWidth() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetMaximumPaperWidth();
}

sal_uInt32 SvtOptionsDrawinglayer::GetMaximumPaperHeight() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetMaximumPaperHeight();
}

sal_uInt32 SvtOptionsDrawinglayer::GetMaximumPaperLeftMargin() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetMaximumPaperLeftMargin();
}

sal_uInt32 SvtOptionsDrawinglayer::GetMaximumPaperRightMargin() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetMaximumPaperRightMargin();
}

sal_uInt32 SvtOptionsDrawinglayer::GetMaximumPaperTopMargin() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetMaximumPaperTopMargin();
}

sal_uInt32 SvtOptionsDrawinglayer::GetMaximumPaperBottomMargin() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetMaximumPaperBottomMargin();
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer::SetOverlayBuffer( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetOverlayBuffer( bState );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer::SetPaintBuffer( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetPaintBuffer( bState );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer::SetStripeColorA( Color aColor )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetStripeColorA( aColor );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer::SetStripeColorB( Color aColor )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetStripeColorB( aColor );
}

//*****************************************************************************************************************
//	public method
//*****************************************************************************************************************
void SvtOptionsDrawinglayer::SetStripeLength( sal_uInt16 nLength )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetStripeLength( nLength );
}

// #i73602#
void SvtOptionsDrawinglayer::SetOverlayBuffer_Calc( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetOverlayBuffer_Calc( bState );
}

void SvtOptionsDrawinglayer::SetOverlayBuffer_Writer( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetOverlayBuffer_Writer( bState );
}

void SvtOptionsDrawinglayer::SetOverlayBuffer_DrawImpress( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetOverlayBuffer_DrawImpress( bState );
}

// #i74769#, #i75172#
void SvtOptionsDrawinglayer::SetPaintBuffer_Calc( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetPaintBuffer_Calc( bState );
}

void SvtOptionsDrawinglayer::SetPaintBuffer_Writer( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetPaintBuffer_Writer( bState );
}

void SvtOptionsDrawinglayer::SetPaintBuffer_DrawImpress( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetPaintBuffer_DrawImpress( bState );
}

// #i4219#
void SvtOptionsDrawinglayer::SetMaximumPaperWidth( sal_uInt32 nNew )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetMaximumPaperWidth( nNew );
}

void SvtOptionsDrawinglayer::SetMaximumPaperHeight( sal_uInt32 nNew )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetMaximumPaperHeight( nNew );
}

void SvtOptionsDrawinglayer::SetMaximumPaperLeftMargin( sal_uInt32 nNew )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetMaximumPaperLeftMargin( nNew );
}

void SvtOptionsDrawinglayer::SetMaximumPaperRightMargin( sal_uInt32 nNew )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetMaximumPaperRightMargin( nNew );
}

void SvtOptionsDrawinglayer::SetMaximumPaperTopMargin( sal_uInt32 nNew )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetMaximumPaperTopMargin( nNew );
}

void SvtOptionsDrawinglayer::SetMaximumPaperBottomMargin( sal_uInt32 nNew )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetMaximumPaperBottomMargin( nNew );
}

// helper
sal_Bool SvtOptionsDrawinglayer::IsAAPossibleOnThisSystem() const
{
	return m_pDataContainer->IsAAPossibleOnThisSystem();
}

// primitives
sal_Bool SvtOptionsDrawinglayer::IsAntiAliasing() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsAntiAliasing() && IsAAPossibleOnThisSystem();
}

sal_Bool SvtOptionsDrawinglayer::IsSnapHorVerLinesToDiscrete() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsAntiAliasing() && m_pDataContainer->IsSnapHorVerLinesToDiscrete();
}

sal_Bool SvtOptionsDrawinglayer::IsSolidDragCreate() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsSolidDragCreate();
}

sal_Bool SvtOptionsDrawinglayer::IsRenderDecoratedTextDirect() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsRenderDecoratedTextDirect();
}

sal_Bool SvtOptionsDrawinglayer::IsRenderSimpleTextDirect() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsRenderSimpleTextDirect();
}

sal_uInt32 SvtOptionsDrawinglayer::GetQuadratic3DRenderLimit() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetQuadratic3DRenderLimit();
}

sal_uInt32 SvtOptionsDrawinglayer::GetQuadraticFormControlRenderLimit() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->GetQuadraticFormControlRenderLimit();
}

void SvtOptionsDrawinglayer::SetAntiAliasing( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetAntiAliasing( bState );
}

void SvtOptionsDrawinglayer::SetSnapHorVerLinesToDiscrete( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetSnapHorVerLinesToDiscrete( bState );
}

void SvtOptionsDrawinglayer::SetSolidDragCreate( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetSolidDragCreate( bState );
}

void SvtOptionsDrawinglayer::SetRenderDecoratedTextDirect( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetRenderDecoratedTextDirect( bState );
}

void SvtOptionsDrawinglayer::SetRenderSimpleTextDirect( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetRenderSimpleTextDirect( bState );
}

void SvtOptionsDrawinglayer::SetQuadratic3DRenderLimit(sal_uInt32 nNew)
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetQuadratic3DRenderLimit( nNew );
}

void SvtOptionsDrawinglayer::SetQuadraticFormControlRenderLimit(sal_uInt32 nNew)
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetQuadraticFormControlRenderLimit( nNew );
}

// #i97672# selection settings
sal_Bool SvtOptionsDrawinglayer::IsTransparentSelection() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	return m_pDataContainer->IsTransparentSelection();
}

void SvtOptionsDrawinglayer::SetTransparentSelection( sal_Bool bState )
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	m_pDataContainer->SetTransparentSelection( bState );
}

sal_uInt16 SvtOptionsDrawinglayer::GetTransparentSelectionPercent() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	sal_uInt16 aRetval(m_pDataContainer->GetTransparentSelectionPercent());

	// crop to range [10% .. 90%]
	if(aRetval < 10)
	{
		aRetval = 10;
	}

	if(aRetval > 90)
	{
		aRetval = 90;
	}

	return aRetval;
}

void SvtOptionsDrawinglayer::SetTransparentSelectionPercent( sal_uInt16 nPercent )
{
    MutexGuard aGuard( GetOwnStaticMutex() );

    // crop to range [10% .. 90%]
	if(nPercent < 10)
	{
		nPercent = 10;
	}

	if(nPercent > 90)
	{
		nPercent = 90;
	}

    m_pDataContainer->SetTransparentSelectionPercent( nPercent );
}

sal_uInt16 SvtOptionsDrawinglayer::GetSelectionMaximumLuminancePercent() const
{
    MutexGuard aGuard( GetOwnStaticMutex() );
	sal_uInt16 aRetval(m_pDataContainer->GetSelectionMaximumLuminancePercent());

	// crop to range [0% .. 100%]
	if(aRetval > 90)
	{
		aRetval = 90;
	}

	return aRetval;
}

Color SvtOptionsDrawinglayer::getHilightColor() const
{
    Color aRetval(Application::GetSettings().GetStyleSettings().GetHighlightColor());
    const basegfx::BColor aSelection(aRetval.getBColor());
    const double fLuminance(aSelection.luminance());
    const double fMaxLum(GetSelectionMaximumLuminancePercent() / 100.0);

    if(fLuminance > fMaxLum)
    {
        const double fFactor(fMaxLum / fLuminance);
        const basegfx::BColor aNewSelection(
            aSelection.getRed() * fFactor,
            aSelection.getGreen() * fFactor,
            aSelection.getBlue() * fFactor);

        aRetval = Color(aNewSelection);
    }

    return aRetval;
}

void SvtOptionsDrawinglayer::SetSelectionMaximumLuminancePercent( sal_uInt16 nPercent )
{
    MutexGuard aGuard( GetOwnStaticMutex() );

    // crop to range [0% .. 100%]
	if(nPercent > 90)
	{
		nPercent = 90;
	}

    m_pDataContainer->SetSelectionMaximumLuminancePercent( nPercent );
}

//*****************************************************************************************************************
//	private method
//*****************************************************************************************************************
Mutex& SvtOptionsDrawinglayer::GetOwnStaticMutex()
{
	// Initialize static mutex only for one time!
    static Mutex* pMutex = NULL;
	// If these method first called (Mutex not already exist!) ...
    if( pMutex == NULL )
    {
		// ... we must create a new one. Protect follow code with the global mutex -
		// It must be - we create a static variable!
        MutexGuard aGuard( Mutex::getGlobalMutex() );
		// We must check our pointer again - because it can be that another instance of our class will be faster than these!
        if( pMutex == NULL )
        {
			// Create the new mutex and set it for return on static variable.
            static Mutex aMutex;
            pMutex = &aMutex;
        }
    }
	// Return new created or already existing mutex object.
    return *pMutex;
}

// eof

