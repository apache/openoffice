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



#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include <bf_xmloff/xmlnmspe.hxx>
#endif



#ifndef _SC_XMLTABLEMASTERPAGEEXPORT_HXX
#include "XMLTableMasterPageExport.hxx"
#endif

#include "unonames.hxx"
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::beans;
using namespace xmloff::token;

XMLTableMasterPageExport::XMLTableMasterPageExport( ScXMLExport& rExp ) :
		XMLTextMasterPageExport ( rExp )
{
}

XMLTableMasterPageExport::~XMLTableMasterPageExport()
{
}

void XMLTableMasterPageExport::exportHeaderFooterContent(
			const Reference< XText >& rText,
	        sal_Bool bAutoStyles, sal_Bool bProgress )
{
	DBG_ASSERT( rText.is(), "There is the text" );

	if( bAutoStyles )
		GetExport().GetTextParagraphExport()
				->collectTextAutoStyles( rText, bProgress, sal_False );
	else
	{
		GetExport().GetTextParagraphExport()->exportTextDeclarations( rText );
		GetExport().GetTextParagraphExport()->exportText( rText, bProgress, sal_False );
	}
}

void XMLTableMasterPageExport::exportHeaderFooter(const ::com::sun::star::uno::Reference < ::com::sun::star::sheet::XHeaderFooterContent >& xHeaderFooter,
													const XMLTokenEnum aName,
													const sal_Bool bDisplay)
{
	if( xHeaderFooter.is() )
	{
		Reference < XText > xCenter = xHeaderFooter->getCenterText();
		Reference < XText > xLeft = xHeaderFooter->getLeftText();
		Reference < XText > xRight = xHeaderFooter->getRightText();
		if (xCenter.is() && xLeft.is() && xRight.is())
		{
			::rtl::OUString sCenter (xCenter->getString());
			::rtl::OUString sLeft (xLeft->getString());
			::rtl::OUString sRight (xRight->getString());

			if( !bDisplay )
				GetExport().AddAttribute( XML_NAMESPACE_STYLE,
												XML_DISPLAY, XML_FALSE );
			SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_STYLE,
								  	aName, sal_True, sal_True );
			if (sCenter.getLength() && !sLeft.getLength() && !sRight.getLength())
				exportHeaderFooterContent( xCenter, sal_False, sal_False );
			else
			{
				if (sLeft.getLength())
				{
					SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_STYLE,
								  	  		XML_REGION_LEFT, sal_True, sal_True );
					exportHeaderFooterContent( xLeft, sal_False, sal_False );
				}
				if (sCenter.getLength())
				{
					SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_STYLE,
								  	  		XML_REGION_CENTER, sal_True, sal_True );
					exportHeaderFooterContent( xCenter, sal_False, sal_False );
				}
				if (sRight.getLength())
				{
					SvXMLElementExport aElem( GetExport(), XML_NAMESPACE_STYLE,
								  	  		XML_REGION_RIGHT, sal_True, sal_True );
					exportHeaderFooterContent( xRight, sal_False, sal_False );
				}
			}
		}
	}
}

void XMLTableMasterPageExport::exportMasterPageContent(
				const Reference < XPropertySet > & rPropSet,
				sal_Bool bAutoStyles )
{
	Any aAny;

	Reference < sheet::XHeaderFooterContent > xHeader;
	aAny = rPropSet->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( SC_UNO_PAGE_RIGHTHDRCON ) ) );
	aAny >>= xHeader;

	Reference < sheet::XHeaderFooterContent > xHeaderLeft;
	aAny = rPropSet->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( SC_UNO_PAGE_LEFTHDRCONT ) ) );
	aAny >>= xHeaderLeft;

	Reference < sheet::XHeaderFooterContent > xFooter;
	aAny = rPropSet->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( SC_UNO_PAGE_RIGHTFTRCON ) ) );
	aAny >>= xFooter;

	Reference < sheet::XHeaderFooterContent > xFooterLeft;
	aAny = rPropSet->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( SC_UNO_PAGE_LEFTFTRCONT ) ) );
	aAny >>= xFooterLeft;

	if( bAutoStyles )
	{
		if( xHeader.is() )
		{
			exportHeaderFooterContent( xHeader->getCenterText(), sal_True, sal_False );
			exportHeaderFooterContent( xHeader->getLeftText(), sal_True, sal_False );
			exportHeaderFooterContent( xHeader->getRightText(), sal_True, sal_False );
		}
		if( xHeaderLeft.is())
		{
			exportHeaderFooterContent( xHeaderLeft->getCenterText(), sal_True, sal_False );
			exportHeaderFooterContent( xHeaderLeft->getLeftText(), sal_True, sal_False );
			exportHeaderFooterContent( xHeaderLeft->getRightText(), sal_True, sal_False );
		}
		if( xFooter.is() )
		{
			exportHeaderFooterContent( xFooter->getCenterText(), sal_True, sal_False );
			exportHeaderFooterContent( xFooter->getLeftText(), sal_True, sal_False );
			exportHeaderFooterContent( xFooter->getRightText(), sal_True, sal_False );
		}
		if( xFooterLeft.is())
		{
			exportHeaderFooterContent( xFooterLeft->getCenterText(), sal_True, sal_False );
			exportHeaderFooterContent( xFooterLeft->getLeftText(), sal_True, sal_False );
			exportHeaderFooterContent( xFooterLeft->getRightText(), sal_True, sal_False );
		}
	}
	else
	{
		aAny = rPropSet->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( SC_UNO_PAGE_HDRON ) ) );
		sal_Bool bHeader = *(sal_Bool *)aAny.getValue();

		exportHeaderFooter(xHeader, XML_HEADER, bHeader );

		aAny = rPropSet->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( SC_UNO_PAGE_HDRSHARED ) ) );
		sal_Bool bLeftHeader = !(*(sal_Bool *)aAny.getValue()) && bHeader;

		exportHeaderFooter( xHeaderLeft, XML_HEADER_LEFT, bLeftHeader );

		aAny = rPropSet->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( SC_UNO_PAGE_FTRON ) ) );
		sal_Bool bFooter = *(sal_Bool *)aAny.getValue();

		exportHeaderFooter( xFooter, XML_FOOTER, bFooter );

		aAny = rPropSet->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( SC_UNO_PAGE_FTRSHARED ) ) );
		sal_Bool bLeftFooter = !(*(sal_Bool *)aAny.getValue()) && bFooter;

		exportHeaderFooter( xFooterLeft, XML_FOOTER_LEFT, bLeftFooter );
	}
}

}
