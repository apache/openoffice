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



#ifndef _XMLOFF_FORMS_CONTROLPROPERTYHDL_HXX_
#include "controlpropertyhdl.hxx"
#endif
#ifndef _COM_SUN_STAR_AWT_FONTEMPHASISMARK_HPP_
#include <com/sun/star/awt/FontEmphasisMark.hpp>
#endif
#ifndef _XMLOFF_FORMENUMS_HXX_
#include "formenums.hxx"
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _XMLOFF_XMLCONSTANTSPROPERTYHANDLER_HXX
#include "XMLConstantsPropertyHandler.hxx"
#endif
namespace binfilter {

//.........................................................................
namespace xmloff
{
//.........................................................................

	using namespace ::com::sun::star::uno;
	using namespace ::com::sun::star::awt;
	using namespace ::com::sun::star::beans;
	using namespace ::binfilter::xmloff::token;

	//=====================================================================
	//= OControlPropertyHandlerFactory
	//=====================================================================
	//---------------------------------------------------------------------
	OControlPropertyHandlerFactory::OControlPropertyHandlerFactory()
		:m_pTextAlignHandler(NULL)
		,m_pControlBorderHandler(NULL)
		,m_pRotationAngleHandler(NULL)
		,m_pFontWidthHandler(NULL)
		,m_pFontEmphasisHandler(NULL)
		,m_pFontReliefHandler(NULL)
	{
	}

	//---------------------------------------------------------------------
	OControlPropertyHandlerFactory::~OControlPropertyHandlerFactory()
	{
		delete m_pTextAlignHandler;
		delete m_pControlBorderHandler;
		delete m_pRotationAngleHandler;
		delete m_pFontWidthHandler;
		delete m_pFontEmphasisHandler;
		delete m_pFontReliefHandler;
	}

	//---------------------------------------------------------------------
	const XMLPropertyHandler* OControlPropertyHandlerFactory::GetPropertyHandler(sal_Int32 _nType) const
	{
		const XMLPropertyHandler* pHandler = NULL;

		switch (_nType)
		{
			case XML_TYPE_TEXT_ALIGN:
				if (!m_pTextAlignHandler)
					m_pTextAlignHandler = new XMLConstantsPropertyHandler(OEnumMapper::getEnumMap(OEnumMapper::epTextAlign), XML_TOKEN_INVALID );
				pHandler = m_pTextAlignHandler;
				break;

			case XML_TYPE_CONTROL_BORDER:
				if (!m_pControlBorderHandler)
					m_pControlBorderHandler = new OControlBorderHandler;
				pHandler = m_pControlBorderHandler;
				break;

			case XML_TYPE_ROTATION_ANGLE:
				if (!m_pRotationAngleHandler)
					m_pRotationAngleHandler = new ORotationAngleHandler;
				pHandler = m_pRotationAngleHandler;
				break;

			case XML_TYPE_FONT_WIDTH:
				if (!m_pFontWidthHandler)
					m_pFontWidthHandler = new OFontWidthHandler;
				pHandler = m_pFontWidthHandler;
				break;

			case XML_TYPE_CONTROL_TEXT_EMPHASIZE:
				if (!m_pFontEmphasisHandler)
					m_pFontEmphasisHandler = new XMLConstantsPropertyHandler( OEnumMapper::getEnumMap(OEnumMapper::epFontEmphasis), XML_NONE );
				pHandler = m_pFontEmphasisHandler;
				break;

			case XML_TYPE_TEXT_FONT_RELIEF:
				if (!m_pFontReliefHandler)
					m_pFontReliefHandler = new XMLConstantsPropertyHandler( OEnumMapper::getEnumMap(OEnumMapper::epFontRelief), XML_NONE );
				pHandler = m_pFontReliefHandler;
				break;
		}

		if (!pHandler)
			pHandler = XMLPropertyHandlerFactory::GetPropertyHandler(_nType);
		return pHandler;
	}

	//=====================================================================
	//= OControlTextEmphasisHandler
	//=====================================================================
	OControlTextEmphasisHandler::OControlTextEmphasisHandler()
	{
	}

	//---------------------------------------------------------------------
	sal_Bool OControlTextEmphasisHandler::exportXML( ::rtl::OUString& _rStrExpValue, const Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const
	{
		::rtl::OUStringBuffer aReturn;
		sal_Bool bSuccess = sal_False;
		sal_Int16 nFontEmphasis;
		if (_rValue >>= nFontEmphasis)
		{
			// the type
			sal_Int16 nType = nFontEmphasis & ~(FontEmphasisMark::ABOVE | FontEmphasisMark::BELOW);
			// the position of the mark
			sal_Bool bBelow = 0 != (nFontEmphasis & FontEmphasisMark::BELOW);

			// convert
			if (bSuccess = _rUnitConverter.convertEnum(aReturn, nType, OEnumMapper::getEnumMap(OEnumMapper::epFontEmphasis), XML_NONE))
			{
				aReturn.append( (sal_Unicode)' ' );
				aReturn.append( GetXMLToken(bBelow ? XML_BELOW : XML_ABOVE) );

				_rStrExpValue = aReturn.makeStringAndClear();
			}
		}

		return bSuccess;
	}

	//---------------------------------------------------------------------
	sal_Bool OControlTextEmphasisHandler::importXML( const ::rtl::OUString& _rStrImpValue, Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const
	{
		sal_Bool bSuccess = sal_True;
		sal_uInt16 nEmphasis = FontEmphasisMark::NONE;

		sal_Bool bBelow = sal_False;
		sal_Bool bHasPos = sal_False, bHasType = sal_False;

		::rtl::OUString sToken;
		SvXMLTokenEnumerator aTokenEnum(_rStrImpValue);
		while (aTokenEnum.getNextToken(sToken))
		{
			if (!bHasPos)
			{
				if (IsXMLToken(sToken, XML_ABOVE))
				{
					bBelow = sal_False;
					bHasPos = sal_True;
				}
				else if (IsXMLToken(sToken, XML_BELOW))
				{
					bBelow = sal_True;
					bHasPos = sal_True;
				}
			}
			if (!bHasType)
			{
				if (_rUnitConverter.convertEnum(nEmphasis, sToken, OEnumMapper::getEnumMap(OEnumMapper::epFontEmphasis)))
				{
					bHasType = sal_True;
				}
				else
				{
					bSuccess = sal_False;
					break;
				}
			}
		}

		if (bSuccess)
		{
			nEmphasis |= bBelow ? FontEmphasisMark::BELOW : FontEmphasisMark::ABOVE;
			_rValue <<= (sal_Int16)nEmphasis;
		}

		return bSuccess;
	}

	//=====================================================================
	//= OControlBorderHandler
	//=====================================================================
	//---------------------------------------------------------------------
	OControlBorderHandler::OControlBorderHandler()
	{
	}

	//---------------------------------------------------------------------
	sal_Bool OControlBorderHandler::importXML( const ::rtl::OUString& _rStrImpValue, Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const
	{
		::rtl::OUString sToken;
		SvXMLTokenEnumerator aTokens(_rStrImpValue);

		sal_uInt16 nConvertedStyle = (sal_uInt16)-1;

		sal_Bool bConversionSuccess = sal_False;
		while	(	!bConversionSuccess				// could not interpret the previous token (ignored it)
				&&	aTokens.getNextToken(sToken)	// have a new token
				&&	(0 != sToken.getLength())		// really have a new token
				)
		{
			bConversionSuccess = _rUnitConverter.convertEnum(nConvertedStyle, sToken, OEnumMapper::getEnumMap(OEnumMapper::epBorderWidth));
		}

		if (!bConversionSuccess)
			return sal_False;

		// if we're here, the string could have had more or less than the requested 3 tokens, but we ignore this.
		// At least we have a valid style, which is everything we're interested in.
		_rValue <<= (sal_Int16)nConvertedStyle;
		return sal_True;
	}

	//---------------------------------------------------------------------
	sal_Bool OControlBorderHandler::exportXML( ::rtl::OUString& _rStrExpValue, const Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const
	{
		sal_Bool bSuccess = sal_False;
		sal_Int16 nBorder = 0;

		::rtl::OUStringBuffer aOut;
		bSuccess =	(_rValue >>= nBorder)
				&&	_rUnitConverter.convertEnum(aOut, nBorder, OEnumMapper::getEnumMap(OEnumMapper::epBorderWidth));

		_rStrExpValue = aOut.makeStringAndClear();
		return bSuccess;
	}

	//=====================================================================
	//= OFontWidthHandler
	//=====================================================================
	//---------------------------------------------------------------------
	OFontWidthHandler::OFontWidthHandler()
	{
	}

	//---------------------------------------------------------------------
	sal_Bool OFontWidthHandler::importXML( const ::rtl::OUString& _rStrImpValue, Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const
	{
		sal_Bool bSuccess = sal_False;

		sal_Int32 nWidth = 0;
		if (bSuccess = _rUnitConverter.convertMeasure(nWidth, _rStrImpValue, MAP_POINT))
			_rValue <<= (sal_Int16)nWidth;

		return bSuccess;
	}

	//---------------------------------------------------------------------
	sal_Bool OFontWidthHandler::exportXML( ::rtl::OUString& _rStrExpValue, const Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const
	{
		sal_Int16 nWidth = 0;
		::rtl::OUStringBuffer aResult;
		if (_rValue >>= nWidth)
			_rUnitConverter.convertMeasure(aResult, nWidth, MAP_POINT, MAP_POINT);
		_rStrExpValue = aResult.makeStringAndClear();

		return _rStrExpValue.getLength() != 0;
	}

	//=====================================================================
	//= ORotationAngleHandler
	//=====================================================================
	//---------------------------------------------------------------------
	ORotationAngleHandler::ORotationAngleHandler()
	{
	}

	//---------------------------------------------------------------------
	sal_Bool ORotationAngleHandler::importXML( const ::rtl::OUString& _rStrImpValue, Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const
	{
		sal_Bool bSucces = sal_False;

		double fValue;
		if (bSucces = _rUnitConverter.convertDouble(fValue, _rStrImpValue))
		{
			fValue *= 10;
			_rValue <<= (float)fValue;
		}

		return bSucces;
	}

	//---------------------------------------------------------------------
	sal_Bool ORotationAngleHandler::exportXML( ::rtl::OUString& _rStrExpValue, const Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const
	{
		float fAngle;
		sal_Bool bSuccess = sal_False;

		if (bSuccess = (_rValue >>= fAngle))
		{
			::rtl::OUStringBuffer sValue;
			_rUnitConverter.convertDouble(sValue, ((double)fAngle) / 10);
			_rStrExpValue = sValue.makeStringAndClear();
		}

		return bSuccess;
	}

//.........................................................................
}	// namespace xmloff
//.........................................................................

}//end of namespace binfilter
