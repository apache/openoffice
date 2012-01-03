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


#include <bf_svtools/rectitem.hxx>

#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif

#ifndef _COM_SUN_STAR_AWT_RECTANGLE_HPP_
#include <com/sun/star/awt/Rectangle.hpp>
#endif

#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif

#include <bf_svtools/poolitem.hxx>
#include "memberid.hrc"

namespace binfilter
{

// STATIC DATA -----------------------------------------------------------

DBG_NAME(SfxRectangleItem)


// -----------------------------------------------------------------------

TYPEINIT1_AUTOFACTORY(SfxRectangleItem, SfxPoolItem);

// -----------------------------------------------------------------------

SfxRectangleItem::SfxRectangleItem()
{
	DBG_CTOR(SfxRectangleItem, 0);
}

// -----------------------------------------------------------------------

SfxRectangleItem::SfxRectangleItem( USHORT nW, const Rectangle& rVal ) :
	SfxPoolItem( nW ),
	aVal( rVal )
{
	DBG_CTOR(SfxRectangleItem, 0);
}

// -----------------------------------------------------------------------

SfxRectangleItem::SfxRectangleItem( const SfxRectangleItem& rItem ) :
	SfxPoolItem( rItem ),
	aVal( rItem.aVal )
{
	DBG_CTOR(SfxRectangleItem, 0);
}

// -----------------------------------------------------------------------

SfxItemPresentation SfxRectangleItem::GetPresentation
(
	SfxItemPresentation 	/*ePresentation*/,
	SfxMapUnit				/*eCoreMetric*/,
	SfxMapUnit				/*ePresentationMetric*/,
	XubString& 				rText,
    const ::IntlWrapper *
)	const
{
	DBG_CHKTHIS(SfxRectangleItem, 0);
	rText = UniString::CreateFromInt32(aVal.Top());
	rText.AppendAscii(RTL_CONSTASCII_STRINGPARAM(", "));
	rText += UniString::CreateFromInt32(aVal.Left());
	rText.AppendAscii(RTL_CONSTASCII_STRINGPARAM(", "));
	rText += UniString::CreateFromInt32(aVal.Bottom());
	rText.AppendAscii(RTL_CONSTASCII_STRINGPARAM(", "));
	rText += UniString::CreateFromInt32(aVal.Right());
	return SFX_ITEM_PRESENTATION_NAMELESS;
}

// -----------------------------------------------------------------------

int SfxRectangleItem::operator==( const SfxPoolItem& rItem ) const
{
	DBG_CHKTHIS(SfxRectangleItem, 0);
	DBG_ASSERT( SfxPoolItem::operator==( rItem ), "unequal type" );
	return ((SfxRectangleItem&)rItem).aVal == aVal;
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxRectangleItem::Clone(SfxItemPool *) const
{
	DBG_CHKTHIS(SfxRectangleItem, 0);
	return new SfxRectangleItem( *this );
}

// -----------------------------------------------------------------------

SfxPoolItem* SfxRectangleItem::Create(SvStream &rStream, USHORT ) const
{
	DBG_CHKTHIS(SfxRectangleItem, 0);
	Rectangle aStr;
	rStream >> aStr;
	return new SfxRectangleItem(Which(), aStr);
}

// -----------------------------------------------------------------------

SvStream& SfxRectangleItem::Store(SvStream &rStream, USHORT ) const
{
	DBG_CHKTHIS(SfxRectangleItem, 0);
	rStream << aVal;
	return rStream;
}


// -----------------------------------------------------------------------
BOOL SfxRectangleItem::QueryValue( ::com::sun::star::uno::Any& rVal,
								   BYTE nMemberId) const
{
	nMemberId &= ~CONVERT_TWIPS;
    switch ( nMemberId )
    {
        case 0:
        {
            rVal <<= com::sun::star::awt::Rectangle( aVal.getX(),
											 aVal.getY(),
											 aVal.getWidth(),
											 aVal.getHeight() );
            break;
        }
        case MID_RECT_LEFT:  rVal <<= aVal.getX(); break;
        case MID_RECT_RIGHT: rVal <<= aVal.getY(); break;
        case MID_WIDTH: rVal <<= aVal.getWidth(); break;
        case MID_HEIGHT: rVal <<= aVal.getHeight(); break;
        default: DBG_ERROR("Wrong MemberID!"); return FALSE;
    }

	return TRUE;
}

// -----------------------------------------------------------------------
BOOL SfxRectangleItem::PutValue( const com::sun::star::uno::Any& rVal,
								 BYTE nMemberId  )
{
	BOOL bRet = FALSE;
	nMemberId &= ~CONVERT_TWIPS;
    com::sun::star::awt::Rectangle aValue;
    sal_Int32 nVal = 0;
    if ( !nMemberId )
        bRet = (rVal >>= aValue);
    else
        bRet = (rVal >>= nVal);

    if ( bRet )
    {
        switch ( nMemberId )
        {
            case 0:
                aVal.setX( aValue.X );
                aVal.setY( aValue.Y );
                aVal.setWidth( aValue.Width );
                aVal.setHeight( aValue.Height );
                break;
            case MID_RECT_LEFT:  aVal.setX( nVal ); break;
            case MID_RECT_RIGHT: aVal.setY( nVal ); break;
            case MID_WIDTH: aVal.setWidth( nVal ); break;
            case MID_HEIGHT: aVal.setHeight( nVal ); break;
            default: DBG_ERROR("Wrong MemberID!"); return FALSE;
        }
    }

	return bRet;
}

}


