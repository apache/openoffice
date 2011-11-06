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



#ifndef _XMLOFF_XMLTEXTCHARSTYLENAMESELEMENTEXPORT_HXX
#include "XMLTextCharStyleNamesElementExport.hxx"
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include "nmspmap.hxx"
#endif
#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif

namespace com { namespace sun { namespace star {
	namespace beans { class XPropertySet; }
} } }
namespace binfilter {

using namespace ::com::sun::star::uno;
using ::com::sun::star::beans::XPropertySet;
using ::rtl::OUString;
using namespace ::binfilter::xmloff::token;

XMLTextCharStyleNamesElementExport::XMLTextCharStyleNamesElementExport(
	SvXMLExport& rExp,
	sal_Bool bDoSth,
	const Reference < XPropertySet > & rPropSet,
	const OUString& rPropName ) :
	rExport( rExp ),
	nCount( 0 )
{
	if( bDoSth )
	{
		Any aAny = rPropSet->getPropertyValue( rPropName );
		Sequence < OUString > aNames;
		if( aAny >>= aNames )
		{
			nCount = aNames.getLength();
			OSL_ENSURE( nCount > 0, "no char style found" );
			if( nCount > 1 )
			{
    			aName = rExport.GetNamespaceMap().GetQNameByKey(
								XML_NAMESPACE_TEXT, GetXMLToken(XML_SPAN) );
				sal_Int32 i = nCount;
				const OUString *pName = aNames.getConstArray();
				while( --i )
				{
					rExport.AddAttribute( XML_NAMESPACE_TEXT, XML_STYLE_NAME,
										  *pName );
					rExport.StartElement( aName, sal_False );
					++pName;
				}
			}
		}
	}
}

XMLTextCharStyleNamesElementExport::~XMLTextCharStyleNamesElementExport()
{
	if( nCount > 1 )
	{
		sal_Int32 i = nCount;
		while( --i )
			rExport.EndElement( aName, sal_False );
	}
}

}//end of namespace binfilter
