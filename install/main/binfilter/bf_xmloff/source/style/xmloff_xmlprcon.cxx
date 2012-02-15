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
#include "xmlimp.hxx"
#endif


#ifndef _XMLOFF_XMLPROPERTYSETCONTEXT_HXX
#include "xmlprcon.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::std;

SvXMLPropertySetContext::SvXMLPropertySetContext(
	SvXMLImport& rImp, USHORT nPrfx,
	const OUString& rLName,
    const uno::Reference< xml::sax::XAttributeList >& xAttrList,
	vector< XMLPropertyState > &rProps,
	const UniReference < SvXMLImportPropertyMapper >  &rMap,
	sal_Int32 nSIdx, sal_Int32 nEIdx ):
	SvXMLImportContext( rImp, nPrfx, rLName ),
	rProperties( rProps ),
	xMapper    ( rMap ),
	nStartIdx( nSIdx ),
	nEndIdx( nEIdx )
{
	xMapper->importXML( rProperties, xAttrList,
						GetImport().GetMM100UnitConverter(),
						GetImport().GetNamespaceMap(), nStartIdx, nEndIdx );
}

SvXMLPropertySetContext::~SvXMLPropertySetContext()
{
}

SvXMLImportContext *SvXMLPropertySetContext::CreateChildContext(
	USHORT nPrefix,
	const OUString& rLocalName,
	const uno::Reference< xml::sax::XAttributeList >& xAttrList )
{
	UniReference< XMLPropertySetMapper > aSetMapper(
			xMapper->getPropertySetMapper() );
	sal_Int32 nEntryIndex = aSetMapper->GetEntryIndex( nPrefix, rLocalName,
													   nStartIdx );

	if( ( nEntryIndex != -1 ) && (-1 == nEndIdx || nEntryIndex < nEndIdx ) &&
		( 0 != ( aSetMapper->GetEntryFlags( nEntryIndex )
				 		& MID_FLAG_ELEMENT_ITEM_IMPORT ) ) )
	{
		XMLPropertyState aProp( nEntryIndex );
		return CreateChildContext( nPrefix, rLocalName, xAttrList,
								   rProperties, aProp );
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
SvXMLImportContext *SvXMLPropertySetContext::CreateChildContext(
	USHORT nPrefix,
	const ::rtl::OUString& rLocalName,
	const uno::Reference< xml::sax::XAttributeList >& xAttrList,
	::std::vector< XMLPropertyState > &rProperties,
	const XMLPropertyState& rProp )
{
	return new SvXMLImportContext( GetImport(), nPrefix, rLocalName );
}


}//end of namespace binfilter
