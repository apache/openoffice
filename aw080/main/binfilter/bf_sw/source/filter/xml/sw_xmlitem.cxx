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




#ifndef _XMLOFF_XMLIMP_HXX
#include <bf_xmloff/xmlimp.hxx>
#endif

#ifndef _XMLIMPIT_HXX
#include "xmlimpit.hxx"
#endif

#ifndef _XMLITEM_HXX
#include "xmlitem.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;

SvXMLItemSetContext::SvXMLItemSetContext( SvXMLImport& rImp, USHORT nPrfx,
										  const OUString& rLName,
										  const uno::Reference< xml::sax::XAttributeList >& xAttrList,
										  SfxItemSet& rISet,
										  const SvXMLImportItemMapper& rIMap,
										  const SvXMLUnitConverter& rUnitConverter ):
	SvXMLImportContext( rImp, nPrfx, rLName ),
	rItemSet( rISet ),
	rIMapper( rIMap ),
	rUnitConv( rUnitConverter )

{
	rIMapper.importXML( rItemSet, xAttrList, rUnitConv,
			   			GetImport().GetNamespaceMap() );
}

SvXMLItemSetContext::~SvXMLItemSetContext()
{
}

SvXMLImportContext *SvXMLItemSetContext::CreateChildContext( USHORT nPrefix,
											const OUString& rLocalName,
											const uno::Reference< xml::sax::XAttributeList >& xAttrList )
{
	SvXMLItemMapEntriesRef xMapEntries = rIMapper.getMapEntries();
	SvXMLItemMapEntry* pEntry = xMapEntries->getByName( nPrefix, rLocalName );

	if( pEntry && 0 != (pEntry->nMemberId & MID_FLAG_ELEMENT_ITEM_IMPORT) )
	{
		return CreateChildContext( nPrefix, rLocalName, xAttrList,
								   rItemSet, *pEntry, rUnitConv );
	}
	else
	{
		return new SvXMLImportContext( GetImport(), nPrefix, rLocalName );
	}
}

/** This method is called from this instance implementation of
	CreateChildContext if the element matches an entry in the
	SvXMLImportItemMapper with the mid flag MID_FLAG_ELEMENT
*/
SvXMLImportContext *SvXMLItemSetContext::CreateChildContext( USHORT nPrefix,
								   const ::rtl::OUString& rLocalName,
								   const uno::Reference< xml::sax::XAttributeList >& xAttrList,
						 		   SfxItemSet&  rItemSet,
								   const SvXMLItemMapEntry& rEntry,
								   const SvXMLUnitConverter& rUnitConv )
{
	return new SvXMLImportContext( GetImport(), nPrefix, rLocalName );
}


}
