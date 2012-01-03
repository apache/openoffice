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

#ifndef _XMLOFF_XMLTEXTPROPERTYSETCONTEXT_HXX
#include "XMLTextPropertySetContext.hxx"
#endif
#ifndef _XMLTEXTCOLUMNSCONTEXT_HXX
#include "XMLTextColumnsContext.hxx"
#endif
#ifndef _XMLBACKGROUNDIMAGECONTEXT_HXX
#include "XMLBackgroundImageContext.hxx"
#endif
#ifndef _XMLOFF_XMLSECTIONFOOTNOTECONFIGIMPORT_HXX
#include "XMLSectionFootnoteConfigImport.hxx"
#endif

#ifndef _XMLOFF_TXTPRMAP_HXX
#include "txtprmap.hxx"
#endif
#ifndef _XMLOFF_XMLTABI_HXX
#include "xmltabi.hxx"
#endif
#ifndef _XMLOFF_TXTDROPI_HXX
#include "txtdropi.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star;

XMLTextPropertySetContext::XMLTextPropertySetContext(
				 SvXMLImport& rImport, sal_uInt16 nPrfx,
				 const OUString& rLName,
				 const Reference< xml::sax::XAttributeList > & xAttrList,
				 ::std::vector< XMLPropertyState > &rProps,
				 const UniReference < SvXMLImportPropertyMapper > &rMap,
				 OUString& rDCTextStyleName ) :
	SvXMLPropertySetContext( rImport, nPrfx, rLName, xAttrList, rProps, rMap ),
	rDropCapTextStyleName( rDCTextStyleName )
{
}

XMLTextPropertySetContext::~XMLTextPropertySetContext()
{
}
	
SvXMLImportContext *XMLTextPropertySetContext::CreateChildContext(
				   sal_uInt16 nPrefix,
				   const OUString& rLocalName,
				   const Reference< xml::sax::XAttributeList > & xAttrList,
				   ::std::vector< XMLPropertyState > &rProperties,
				   const XMLPropertyState& rProp )
{
	SvXMLImportContext *pContext = 0;

	switch( xMapper->getPropertySetMapper()
					->GetEntryContextId( rProp.mnIndex ) )
	{
	case CTF_TABSTOP:
		pContext = new SvxXMLTabStopImportContext( GetImport(), nPrefix,
												   rLocalName, rProp,
												   rProperties );
		break;
	case CTF_TEXTCOLUMNS:
#ifndef SVX_LIGHT
		pContext = new XMLTextColumnsContext( GetImport(), nPrefix,
												   rLocalName, xAttrList, rProp,
												   rProperties );
#else
		// create default context to skip content
		pContext = new SvXMLImportContext( GetImport(), nPrefix, rLocalName );
#endif // #ifndef SVX_LIGHT
		break;

	case CTF_DROPCAPFORMAT:
		{
			DBG_ASSERT( rProp.mnIndex >= 2 &&
						CTF_DROPCAPWHOLEWORD  == xMapper->getPropertySetMapper()
							->GetEntryContextId( rProp.mnIndex-2 ),
						"invalid property map!");
			XMLTextDropCapImportContext *pDCContext =
				new XMLTextDropCapImportContext( GetImport(), nPrefix,
														rLocalName, xAttrList,
														rProp,
														rProp.mnIndex-2,
														rProperties );
			rDropCapTextStyleName = pDCContext->GetStyleName();
		pContext = pDCContext;
		}
		break;

	case CTF_BACKGROUND_URL:
    {
		DBG_ASSERT( rProp.mnIndex >= 2 &&
					CTF_BACKGROUND_POS  == xMapper->getPropertySetMapper()
						->GetEntryContextId( rProp.mnIndex-2 ) &&
					CTF_BACKGROUND_FILTER  == xMapper->getPropertySetMapper()
						->GetEntryContextId( rProp.mnIndex-1 ),
					"invalid property map!");

        // #99657# Transparency might be there as well... but doesn't have
        // to. Thus, this is checked with an if, rather than with an assertion.
        sal_Int32 nTranspIndex = -1;
        if( (rProp.mnIndex >= 3) &&
            ( CTF_BACKGROUND_TRANSPARENCY == 
              xMapper->getPropertySetMapper()->GetEntryContextId( 
                  rProp.mnIndex-3 ) ) )
            nTranspIndex = rProp.mnIndex-3;

		pContext = 
			new XMLBackgroundImageContext( GetImport(), nPrefix,
										   rLocalName, xAttrList,
										   rProp,
										   rProp.mnIndex-2,
										   rProp.mnIndex-1,
										   nTranspIndex,
										   rProperties );
    }
    break;
#ifndef SVX_LIGHT
	case CTF_SECTION_FOOTNOTE_END:
		pContext = new XMLSectionFootnoteConfigImport( 
			GetImport(), nPrefix, rLocalName, rProperties, 
			xMapper->getPropertySetMapper(), rProp.mnIndex);
		break;
	case CTF_SECTION_ENDNOTE_END:
		pContext = new XMLSectionFootnoteConfigImport( 
			GetImport(), nPrefix, rLocalName, rProperties, 
			xMapper->getPropertySetMapper(), rProp.mnIndex);
		break;
#endif // #ifndef SVX_LIGHT
	}
	
	if( !pContext )
		pContext = SvXMLPropertySetContext::CreateChildContext( nPrefix, rLocalName,
															xAttrList, 
															rProperties, rProp );
	
	return pContext;
}


}//end of namespace binfilter
