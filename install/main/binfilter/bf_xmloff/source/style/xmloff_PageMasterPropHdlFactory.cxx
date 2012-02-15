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



#ifndef _XMLOFF_PAGEMASTERPROPHDLFACTORY_HXX
#include "PageMasterPropHdlFactory.hxx"
#endif

#ifndef _XMLOFF_PROPERTYHANDLER_BASICTYPES_HXX
#include "xmlbahdl.hxx"
#endif
#ifndef _XMLOFF_NAMEDBOOLPROPERTYHANDLER_HXX
#include "NamedBoolPropertyHdl.hxx"
#endif
#ifndef _XMLOFF_XMLTEXTCOLUMNSPPROPERTYHANDLER_HXX
#include "XMLTextColumnsPropertyHandler.hxx"
#endif
#ifndef _XMLOFF_XMLCONSTANTSPROPERTYHANDLER_HXX
#include "XMLConstantsPropertyHandler.hxx"
#endif

#ifndef _XMLOFF_PAGEMASTERPROPHDL_HXX_
#include "PageMasterPropHdl.hxx"
#endif
#ifndef _XMLOFF_PAGEMASTERSTYLEMAP_HXX
#include "PageMasterStyleMap.hxx"
#endif

#ifndef _COM_SUN_STAR_TEXT_TEXTGRIDMODE_HPP_
#include <com/sun/star/text/TextGridMode.hpp>
#endif
namespace binfilter {


using namespace ::rtl;
using namespace ::binfilter::xmloff::token;
using namespace ::com::sun::star;


SvXMLEnumMapEntry aXML_TextGridMode_ConstantMap[] =
{
	{ XML_NONE,         text::TextGridMode::NONE },
	{ XML_LINE,	        text::TextGridMode::LINES },
	{ XML_BOTH,         text::TextGridMode::LINES_AND_CHARS },
	{ XML_TOKEN_INVALID, 0 }
};

//______________________________________________________________________________

XMLPageMasterPropHdlFactory::XMLPageMasterPropHdlFactory() :
	XMLPropertyHandlerFactory()
{
}

XMLPageMasterPropHdlFactory::~XMLPageMasterPropHdlFactory()
{
}

const XMLPropertyHandler* XMLPageMasterPropHdlFactory::GetPropertyHandler( sal_Int32 nType ) const
{
	nType &= MID_FLAG_MASK;

	XMLPropertyHandler* pHdl = (XMLPropertyHandler*) XMLPropertyHandlerFactory::GetPropertyHandler( nType );
	if( !pHdl )
	{
		switch( nType )
		{
			case XML_PM_TYPE_PAGESTYLELAYOUT:
				pHdl = new XMLPMPropHdl_PageStyleLayout();
			break;
			case XML_PM_TYPE_NUMFORMAT:
				pHdl = new XMLPMPropHdl_NumFormat();
			break;
			case XML_PM_TYPE_NUMLETTERSYNC:
				pHdl = new XMLPMPropHdl_NumLetterSync();
			break;
			case XML_PM_TYPE_PAPERTRAYNUMBER:
				pHdl = new XMLPMPropHdl_PaperTrayNumber();
			break;
			case XML_PM_TYPE_PRINTORIENTATION:
				pHdl = new XMLNamedBoolPropertyHdl(
                    GetXMLToken( XML_LANDSCAPE ),
					GetXMLToken( XML_PORTRAIT ) );
			break;
			case XML_PM_TYPE_PRINTANNOTATIONS:
				pHdl = new XMLPMPropHdl_Print( XML_ANNOTATIONS );
			break;
			case XML_PM_TYPE_PRINTCHARTS:
				pHdl = new XMLPMPropHdl_Print( XML_CHARTS );
			break;
			case XML_PM_TYPE_PRINTDRAWING:
				pHdl = new XMLPMPropHdl_Print( XML_DRAWINGS );
			break;
			case XML_PM_TYPE_PRINTFORMULAS:
				pHdl = new XMLPMPropHdl_Print( XML_FORMULAS );
			break;
			case XML_PM_TYPE_PRINTGRID:
				pHdl = new XMLPMPropHdl_Print( XML_GRID );
			break;
			case XML_PM_TYPE_PRINTHEADERS:
				pHdl = new XMLPMPropHdl_Print( XML_HEADERS );
			break;
			case XML_PM_TYPE_PRINTOBJECTS:
				pHdl = new XMLPMPropHdl_Print( XML_OBJECTS );
			break;
			case XML_PM_TYPE_PRINTZEROVALUES:
				pHdl = new XMLPMPropHdl_Print( XML_ZERO_VALUES );
			break;
			case XML_PM_TYPE_PRINTPAGEORDER:
				pHdl = new XMLNamedBoolPropertyHdl( GetXMLToken( XML_TTB ),
                                                    GetXMLToken( XML_LTR ) );
			break;
			case XML_PM_TYPE_FIRSTPAGENUMBER:
				pHdl = new XMLNumberNonePropHdl( XML_CONTINUE, 2 );
			break;
			case XML_PM_TYPE_CENTER_HORIZONTAL:
				pHdl = new XMLPMPropHdl_CenterHorizontal();
			break;
			case XML_PM_TYPE_CENTER_VERTICAL:
				pHdl = new XMLPMPropHdl_CenterVertical();
			break;
			case XML_TYPE_TEXT_COLUMNS:
				pHdl = new XMLTextColumnsPropertyHandler;
			break;
            case XML_TYPE_LAYOUT_GRID_MODE:
                pHdl = new XMLConstantsPropertyHandler(
                    aXML_TextGridMode_ConstantMap, XML_NONE );
            break;
		}

		if( pHdl )
			PutHdlCache( nType, pHdl );
	}
	return pHdl;
}


}//end of namespace binfilter
