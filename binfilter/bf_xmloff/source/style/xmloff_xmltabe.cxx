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



#include <tools/debug.hxx>

#ifndef _COM_SUN_STAR_STYLE_TABSTOP_HPP_
#include <com/sun/star/style/TabStop.hpp>
#endif


#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif
#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif

#ifndef _XMLOFF_XMLTABE_HXX
#include "xmltabe.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::binfilter::xmloff::token;

SvXMLEnumMapEntry pXML_tabstop_style[] =
{
	{ XML_LEFT,     style::TabAlign_LEFT	},
	{ XML_CENTER,   style::TabAlign_CENTER	},
	{ XML_RIGHT,    style::TabAlign_RIGHT	},
	{ XML_CHAR,     style::TabAlign_DECIMAL	},
	{ XML_DEFAULT,  style::TabAlign_DEFAULT  },	// ?????????????????????????????????????
	{ XML_TOKEN_INVALID,        0 }
};

void SvxXMLTabStopExport::exportTabStop( const ::com::sun::star::style::TabStop* pTabStop )
{
    SvXMLUnitConverter& rUnitConv = rExport.GetMM100UnitConverter();

	// text:level
	OUStringBuffer sBuffer;

	// position attribute
	rUnitConv.convertMeasure( sBuffer, pTabStop->Position );
	rExport.AddAttribute( XML_NAMESPACE_STYLE, XML_POSITION,
                           sBuffer.makeStringAndClear() );

	// type attribute
	if( style::TabAlign_LEFT != pTabStop->Alignment )
	{
		rUnitConv.convertEnum( sBuffer, pTabStop->Alignment,
								   pXML_tabstop_style );
		rExport.AddAttribute( XML_NAMESPACE_STYLE, XML_TYPE,
                               sBuffer.makeStringAndClear() );
	}

	// char
	if( style::TabAlign_DECIMAL == pTabStop->Alignment &&
		pTabStop->DecimalChar != 0 )
	{
		sBuffer.append( pTabStop->DecimalChar );
		rExport.AddAttribute( XML_NAMESPACE_STYLE, XML_CHAR,
                               sBuffer.makeStringAndClear() );
	}

	// leader-char
	if( ' ' != pTabStop->FillChar && 0 != pTabStop->FillChar )
	{
		sBuffer.append( pTabStop->FillChar );
		rExport.AddAttribute( XML_NAMESPACE_STYLE, XML_LEADER_CHAR,
                               sBuffer.makeStringAndClear() );
	}

    SvXMLElementExport rElem( rExport, XML_NAMESPACE_STYLE, XML_TAB_STOP, 
                              sal_True, sal_True );
}


SvxXMLTabStopExport::SvxXMLTabStopExport( 
    SvXMLExport& rExp)
    : rExport( rExp )
{
}

SvxXMLTabStopExport::~SvxXMLTabStopExport()
{
}

void SvxXMLTabStopExport::Export( const uno::Any& rAny )
{
	uno::Sequence< ::com::sun::star::style::TabStop> aSeq;
	if(!(rAny >>= aSeq))
	{
		DBG_ERROR( "SvxXMLTabStopExport needs a Sequence ::com::sun::star::style::TabStop>" );
	}
	else
	{
		const ::com::sun::star::style::TabStop* pTabs = aSeq.getConstArray();
		const sal_Int32 nTabs   = aSeq.getLength();
		
		// ignore default tab stop here
		//if( 1 == nTabs && style::TabAlign_DEFAULT == pTabs[0].Alignment )
		//	return;

        SvXMLElementExport rElem( rExport, XML_NAMESPACE_STYLE, XML_TAB_STOPS,
                                  sal_True, sal_True );

		for( sal_Int32 nIndex = 0; nIndex < nTabs; nIndex++ )
		{
			if( style::TabAlign_DEFAULT != pTabs[nIndex].Alignment )
				exportTabStop( &(pTabs[nIndex]) );			
		}
	}
}


}//end of namespace binfilter
