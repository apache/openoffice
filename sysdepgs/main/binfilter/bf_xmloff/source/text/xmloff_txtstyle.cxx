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
//#include <tools/debug.hxx>
#endif

#ifndef _COM_SUN_STAR_STYLE_PARAGRAPHSTYLECATEGORY_HPP_
#include <com/sun/star/style/ParagraphStyleCategory.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSTATE_HPP_
#include <com/sun/star/beans/XPropertyState.hpp>
#endif



#ifndef _XMLOFF_PROPERTYSETMAPPER_HXX
//#include "xmlprmap.hxx"
#endif
#ifndef _XMLOFF_XMLSMSPE_HXX
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_TXTPRMAP_HXX
//#include "txtprmap.hxx"
#endif
#ifndef _XMLOFF_XMLNUME_HXX
#include "xmlnume.hxx"
#endif
#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif
#ifndef _XMLOFF_XMLSECTIONEXPORT_HXX
#include "XMLSectionExport.hxx"
#endif
#ifndef _XMLOFF_XMLLINENUMBERINGEXPORT_HXX_
#include "XMLLineNumberingExport.hxx"
#endif
namespace binfilter {

#ifndef _XMLOFF_STYLEEXP_HXX
//#include "styleexp.hxx"
#endif

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::style;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::beans;
using namespace ::binfilter::xmloff::token;

void XMLTextParagraphExport::exportStyleAttributes(
		const ::com::sun::star::uno::Reference<
				::com::sun::star::style::XStyle > & rStyle )
{
	Any aAny;
	Reference< XPropertySet > xPropSet( rStyle, UNO_QUERY );
	Reference< XPropertySetInfo > xPropSetInfo =
			xPropSet->getPropertySetInfo();
	if( xPropSetInfo->hasPropertyByName( sCategory ) )
	{
		aAny = xPropSet->getPropertyValue( sCategory );
		sal_Int16 nCategory;
		aAny >>= nCategory;
		enum XMLTokenEnum eValue = XML_TOKEN_INVALID;
		if( -1 != nCategory )
		{
			switch( nCategory )
			{
			case ParagraphStyleCategory::TEXT:
				eValue = XML_TEXT;
				break;
			case ParagraphStyleCategory::CHAPTER:
				eValue = XML_CHAPTER;
				break;
			case ParagraphStyleCategory::LIST:
				eValue = XML_LIST;
				break;
			case ParagraphStyleCategory::INDEX:
				eValue = XML_INDEX;
				break;
			case ParagraphStyleCategory::EXTRA:
				eValue = XML_EXTRA;
				break;
			case ParagraphStyleCategory::HTML:
				eValue = XML_HTML;
				break;
			}
		}
		if( eValue != XML_TOKEN_INVALID )
			GetExport().AddAttribute( XML_NAMESPACE_STYLE, XML_CLASS, eValue);
	}
	if( xPropSetInfo->hasPropertyByName( sPageDescName ) )
	{
		Reference< XPropertyState > xPropState( xPropSet, uno::UNO_QUERY );
		if( PropertyState_DIRECT_VALUE ==
				xPropState->getPropertyState( sPageDescName  ) )
		{
			aAny = xPropSet->getPropertyValue( sPageDescName );
			OUString sName;
			aAny >>= sName;
			GetExport().AddAttribute( XML_NAMESPACE_STYLE,
									  XML_MASTER_PAGE_NAME,
									  sName );
		}
	}

	if( bProgress )
	{
		ProgressBarHelper *pProgress = GetExport().GetProgressBarHelper();
			pProgress->SetValue( pProgress->GetValue()+2 );
	}
}

void XMLTextParagraphExport::exportNumStyles( sal_Bool bUsed )
{
	SvxXMLNumRuleExport aNumRuleExport( GetExport() );
	aNumRuleExport.exportStyles( bUsed, pListAutoPool, !IsBlockMode() );
}

void XMLTextParagraphExport::exportTextStyles( sal_Bool bUsed, sal_Bool bProg )
{
	sal_Bool bOldProg = bProgress;
	bProgress = bProg;

    Reference < lang::XMultiServiceFactory > xFactory (GetExport().GetModel(), UNO_QUERY);
    if (xFactory.is())
    {
		OUString sTextDefaults ( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.text.Defaults" ) );
        Reference < XInterface > xInt = xFactory->createInstance ( sTextDefaults );
        if ( xInt.is() )
        {
            Reference < XPropertySet > xPropSet (xInt, UNO_QUERY);
            if (xPropSet.is())
                exportDefaultStyle( xPropSet, GetXMLToken(XML_PARAGRAPH), GetParaPropMapper());
        }
    }
	exportStyleFamily( "ParagraphStyles", GetXMLToken(XML_PARAGRAPH), GetParaPropMapper(),
					   bUsed, XML_STYLE_FAMILY_TEXT_PARAGRAPH, 0);
	exportStyleFamily( "CharacterStyles", GetXMLToken(XML_TEXT), GetTextPropMapper(),
					   bUsed, XML_STYLE_FAMILY_TEXT_TEXT );
	// get shape export to make sure the the frame family is added correctly.
	GetExport().GetShapeExport();
	exportStyleFamily( "FrameStyles", OUString(RTL_CONSTASCII_USTRINGPARAM(XML_STYLE_FAMILY_SD_GRAPHICS_NAME)), GetFramePropMapper(),
					   bUsed, XML_STYLE_FAMILY_TEXT_FRAME, 0);
	exportNumStyles( bUsed );
	if( !IsBlockMode() )
	{
		exportTextFootnoteConfiguration();
		XMLSectionExport::ExportBibliographyConfiguration(GetExport());
		XMLLineNumberingExport aLineNumberingExport(GetExport());
		aLineNumberingExport.Export();
	}

	bProgress = bOldProg;
}
}//end of namespace binfilter
