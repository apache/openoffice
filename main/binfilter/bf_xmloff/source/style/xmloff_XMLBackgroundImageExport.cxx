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



#ifndef _COM_SUN_STAR_STYLE_GRAPHICLOCATION_HPP_ 
#include <com/sun/star/style/GraphicLocation.hpp>
#endif

#include <xmlnmspe.hxx>


#ifndef _XMLOFF_XMLEXP_HXX 
#include "xmlexp.hxx"
#endif
#ifndef _XMLBACKGROUNDIMAGEEXPORT_HXX
#include "XMLBackgroundImageExport.hxx"
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::style;
using namespace ::binfilter::xmloff::token;

XMLBackgroundImageExport::XMLBackgroundImageExport( SvXMLExport& rExp ) :
	rExport( rExp )
{
}

XMLBackgroundImageExport::~XMLBackgroundImageExport()
{
}

void XMLBackgroundImageExport::exportXML( const Any& rURL,
			const Any *pPos,
			const Any *pFilter,
            const Any *pTransparency,
			sal_uInt16 nPrefix,
			const ::rtl::OUString& rLocalName )
{
	GraphicLocation ePos;
	if( !(pPos && ((*pPos) >>= ePos)) )
		ePos = GraphicLocation_AREA;

	OUString sURL;
	rURL >>= sURL;
	if( sURL.getLength() && GraphicLocation_NONE != ePos )
	{
		OUString sTempURL( GetExport().AddEmbeddedGraphicObject( sURL ) );
		if( sTempURL.getLength() )
		{
			GetExport().AddAttribute( XML_NAMESPACE_XLINK, XML_HREF, sTempURL );
			GetExport().AddAttribute( XML_NAMESPACE_XLINK, XML_TYPE, 
                                      XML_SIMPLE );
			GetExport().AddAttribute( XML_NAMESPACE_XLINK, XML_ACTUATE,
                                      XML_ONLOAD );
		}
		
		OUStringBuffer aOut;
		switch( ePos )
		{
		case GraphicLocation_LEFT_TOP:
		case GraphicLocation_MIDDLE_TOP:
		case GraphicLocation_RIGHT_TOP:
			aOut.append( GetXMLToken(XML_TOP) );
			break;
		case GraphicLocation_LEFT_MIDDLE:
		case GraphicLocation_MIDDLE_MIDDLE:
		case GraphicLocation_RIGHT_MIDDLE:
			aOut.append( GetXMLToken(XML_CENTER) );
			break;
		case GraphicLocation_LEFT_BOTTOM:
		case GraphicLocation_MIDDLE_BOTTOM:
		case GraphicLocation_RIGHT_BOTTOM:
			aOut.append( GetXMLToken(XML_BOTTOM) );
			break;
		}

		if( aOut.getLength() )
		{
			aOut.append( sal_Unicode( ' ' ) );

			switch( ePos )
			{
			case GraphicLocation_LEFT_TOP:
			case GraphicLocation_LEFT_BOTTOM:
			case GraphicLocation_LEFT_MIDDLE:
				aOut.append( GetXMLToken(XML_LEFT) );
				break;
			case GraphicLocation_MIDDLE_TOP:
			case GraphicLocation_MIDDLE_MIDDLE:
			case GraphicLocation_MIDDLE_BOTTOM:
				aOut.append( GetXMLToken(XML_CENTER) );
				break;
			case GraphicLocation_RIGHT_MIDDLE:
			case GraphicLocation_RIGHT_TOP:
			case GraphicLocation_RIGHT_BOTTOM:
				aOut.append( GetXMLToken(XML_RIGHT) );
				break;
			}
		}
		if( aOut.getLength() )
			GetExport().AddAttribute( XML_NAMESPACE_STYLE,
								  XML_POSITION, aOut.makeStringAndClear() );

		if( GraphicLocation_AREA == ePos )
		{
			aOut.append( GetXMLToken(XML_BACKGROUND_STRETCH) );
		}
		else if( GraphicLocation_NONE != ePos && GraphicLocation_TILED != ePos  )
		{
			aOut.append( GetXMLToken(XML_BACKGROUND_NO_REPEAT) );
		}
		if( aOut.getLength() )
			GetExport().AddAttribute( XML_NAMESPACE_STYLE, XML_REPEAT,
						  aOut.makeStringAndClear() );

		if( pFilter )
		{
			OUString sFilter;
			(*pFilter) >>= sFilter;
			if( sFilter.getLength() )
				GetExport().AddAttribute( XML_NAMESPACE_STYLE, XML_FILTER_NAME,
										  sFilter );
		}

        if( pTransparency )
        {
            sal_Int8 nTransparency;
            if( (*pTransparency) >>= nTransparency )
            {
                OUStringBuffer aOut;
                SvXMLUnitConverter::convertPercent( aOut, nTransparency );
                GetExport().AddAttribute( XML_NAMESPACE_DRAW, XML_TRANSPARENCY,
                                          aOut.makeStringAndClear() );
            }
        }
	}

	{
		SvXMLElementExport aElem( GetExport(), nPrefix, rLocalName, sal_True, sal_True );
		if( sURL.getLength() && GraphicLocation_NONE != ePos )
		{
			// optional office:binary-data
			GetExport().AddEmbeddedGraphicObjectAsBase64( sURL );
		}
	}
}
}//end of namespace binfilter
