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



#include "vbatextbox.hxx"
#include "vbanewfont.hxx"
#include <com/sun/star/text/XTextRange.hpp>
#include <ooo/vba/msforms/fmBorderStyle.hpp>
#include <ooo/vba/msforms/fmSpecialEffect.hpp>

using namespace com::sun::star;
using namespace ooo::vba;

ScVbaTextBox::ScVbaTextBox( const uno::Reference< ov::XHelperInterface >& xParent, const uno::Reference< uno::XComponentContext >& xContext, const uno::Reference< uno::XInterface >& xControl, const uno::Reference< frame::XModel >& xModel, AbstractGeometryAttributes* pGeomHelper, bool bDialog ) : TextBoxImpl_BASE( xParent, xContext, xControl, xModel, pGeomHelper ), mbDialog( bDialog )
{
}

// Attributes
uno::Any SAL_CALL
ScVbaTextBox::getValue()
{
	return uno::makeAny( getText() );
}

void SAL_CALL
ScVbaTextBox::setValue( const uno::Any& _value )
{
    // booleans are converted to uppercase strings
	rtl::OUString sVal = extractStringFromAny( _value, true );
	setText( sVal );
}

//getString() will cause some imfo lose.
rtl::OUString SAL_CALL
ScVbaTextBox::getText()
{
    uno::Any aValue;
    aValue = m_xProps->getPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Text" ) ) );
    rtl::OUString sString;
    aValue >>= sString;
    return sString;
}

void SAL_CALL
ScVbaTextBox::setText( const rtl::OUString& _text )
{
    if ( !mbDialog )
    {
    uno::Reference< text::XTextRange > xTextRange( m_xProps, uno::UNO_QUERY_THROW );
    xTextRange->setString( _text );
}
    else
        m_xProps->setPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Text") ), uno::makeAny( _text ) );
}

sal_Int32 SAL_CALL
ScVbaTextBox::getMaxLength()
{
    uno::Any aValue;
    aValue = m_xProps->getPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MaxTextLen" ) ) );
    sal_Int32 nMaxLength = 0;
    aValue >>= nMaxLength;
    return nMaxLength;
}

void SAL_CALL
ScVbaTextBox::setMaxLength( sal_Int32 _maxlength )
{
    uno::Any aValue( _maxlength );
    m_xProps->setPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MaxTextLen" ) ), aValue);
}

sal_Bool SAL_CALL
ScVbaTextBox::getMultiline()
{
    uno::Any aValue;
    aValue = m_xProps->getPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MultiLine" ) ) );
    sal_Bool bRet = false;
    aValue >>= bRet;
    return bRet;
}

void SAL_CALL
ScVbaTextBox::setMultiline( sal_Bool _multiline )
{
    uno::Any aValue( _multiline );
    m_xProps->setPropertyValue
            (rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "MultiLine" ) ), aValue);
}

sal_Int32 SAL_CALL ScVbaTextBox::getSpecialEffect()
{
    return msforms::fmSpecialEffect::fmSpecialEffectSunken;
}

void SAL_CALL ScVbaTextBox::setSpecialEffect( sal_Int32 /*nSpecialEffect*/ )
{
}

sal_Int32 SAL_CALL ScVbaTextBox::getBorderStyle()
{
    return msforms::fmBorderStyle::fmBorderStyleNone;
}

void SAL_CALL ScVbaTextBox::setBorderStyle( sal_Int32 /*nBorderStyle*/ )
{
}

sal_Int32 SAL_CALL ScVbaTextBox::getTextLength()
{
    return getText().getLength();
}

uno::Reference< msforms::XNewFont > SAL_CALL ScVbaTextBox::getFont()
{
    return new VbaNewFont( this, mxContext, m_xProps );
}

rtl::OUString&
ScVbaTextBox::getServiceImplName()
{
	static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaTextBox") );
	return sImplName;
}

uno::Sequence< rtl::OUString >
ScVbaTextBox::getServiceNames()
{
	static uno::Sequence< rtl::OUString > aServiceNames;
	if ( aServiceNames.getLength() == 0 )
	{
		aServiceNames.realloc( 1 );
		aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.msforms.TextBox" ) );
	}
	return aServiceNames;
}
