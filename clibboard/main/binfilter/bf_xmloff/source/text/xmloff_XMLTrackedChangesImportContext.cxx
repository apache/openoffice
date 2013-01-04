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



#ifndef _XMLOFF_XMLTRACKEDCHANGESIMPORTCONTEXT_HXX
#include "XMLTrackedChangesImportContext.hxx"
#endif

#ifndef _XMLOFF_XMLCHANGEDREGIONIMPORTCONTEXT_HXX
#include "XMLChangedRegionImportContext.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif

#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif

#ifndef _XMLOFF_NMSPMAP_HXX
#include "nmspmap.hxx"
#endif

#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif

namespace binfilter {


using ::rtl::OUString;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::xml::sax::XAttributeList;
using namespace ::binfilter::xmloff::token;



TYPEINIT1( XMLTrackedChangesImportContext, SvXMLImportContext );

XMLTrackedChangesImportContext::XMLTrackedChangesImportContext(
	SvXMLImport& rImport,
	sal_uInt16 nPrefix,
	const OUString& rLocalName) :
		SvXMLImportContext(rImport, nPrefix, rLocalName)
{
}

XMLTrackedChangesImportContext::~XMLTrackedChangesImportContext()
{
}

void XMLTrackedChangesImportContext::StartElement( 
	const Reference<XAttributeList> & xAttrList )
{
	sal_Bool bTrackChanges = sal_True;
	Sequence<sal_Int8> aProtectionKey;

	// scan for text:track-changes and text:protection-key attributes
	sal_Int16 nLength = xAttrList->getLength();
	for( sal_Int16 i = 0; i < nLength; i++ )
	{
		OUString sLocalName;
		sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
			GetKeyByAttrName( xAttrList->getNameByIndex(i), &sLocalName );

		if ( XML_NAMESPACE_TEXT == nPrefix )
		{
			if ( IsXMLToken( sLocalName, XML_TRACK_CHANGES ) )
			{
				sal_Bool bTmp;
				if( SvXMLUnitConverter::convertBool( 
					bTmp, xAttrList->getValueByIndex(i)) )
				{
					bTrackChanges = bTmp;
				}
			}
			else if ( IsXMLToken( sLocalName, XML_PROTECTION_KEY ) )			{
				Sequence<sal_Int8> aSequence;
				SvXMLUnitConverter::decodeBase64( 
					aSequence, xAttrList->getValueByIndex(i));
				if ( aSequence.getLength() > 0 )
				{
					aProtectionKey = aSequence;
				}
			}
		}
	}

	// set tracked changes
	GetImport().GetTextImport()->SetRecordChanges( bTrackChanges );
	GetImport().GetTextImport()->SetChangesProtectionKey( aProtectionKey );
}  


SvXMLImportContext* XMLTrackedChangesImportContext::CreateChildContext(
	sal_uInt16 nPrefix,
	const OUString& rLocalName,
	const Reference<XAttributeList> & xAttrList)
{
	SvXMLImportContext* pContext = NULL;

	if ( (XML_NAMESPACE_TEXT == nPrefix) &&
		 IsXMLToken( rLocalName, XML_CHANGED_REGION ) )
	{
		pContext = new XMLChangedRegionImportContext(GetImport(), 
													 nPrefix, rLocalName);
	}

	if (NULL == pContext)
	{
		pContext = SvXMLImportContext::CreateChildContext(nPrefix, rLocalName, 
														  xAttrList);
	}

	return pContext;
}
}//end of namespace binfilter
