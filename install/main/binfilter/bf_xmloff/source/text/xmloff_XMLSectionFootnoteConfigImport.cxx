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



#ifndef _XMLOFF_XMLSECTIONFOOTNOTECONFIGIMPORT_HXX
#include "XMLSectionFootnoteConfigImport.hxx"
#endif

#include "rtl/ustring.hxx"

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif


#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif


#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif


#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif

#ifndef _XMLOFF_NMSPMAP_HXX
#include "nmspmap.hxx"
#endif


#ifndef _XMLOFF_XMLNUMI_HXX
#include "xmlnumi.hxx"
#endif

#ifndef _XMLOFF_TEXTPRMAP_HXX_
#include "txtprmap.hxx"
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#include <vector>
namespace binfilter {


using namespace ::binfilter::xmloff::token;
using namespace ::com::sun::star::style;

using ::rtl::OUString;
using ::std::vector;
using ::com::sun::star::uno::Any;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::xml::sax::XAttributeList;


TYPEINIT1(XMLSectionFootnoteConfigImport, SvXMLImportContext);


XMLSectionFootnoteConfigImport::XMLSectionFootnoteConfigImport(
	SvXMLImport& rImport, 
	sal_uInt16 nPrefix, 
	const OUString& rLocalName, 
	vector<XMLPropertyState> & rProps,
	const UniReference<XMLPropertySetMapper> & rMapperRef,
	sal_Int32 nIndex) :
		SvXMLImportContext(rImport, nPrefix, rLocalName),
		rProperties(rProps),
		nPropIndex(nIndex),
		rMapper(rMapperRef)
{
}

XMLSectionFootnoteConfigImport::~XMLSectionFootnoteConfigImport()
{
}

void XMLSectionFootnoteConfigImport::StartElement( 
	const Reference<XAttributeList> & xAttrList)
{
	sal_Bool bEnd = sal_True;	// we're inside the element, so this is true
	sal_Bool bNumOwn = sal_False;
	sal_Bool bNumRestart = sal_False;
	sal_Int16 nNumRestartAt = 0;
	OUString sNumPrefix;
	OUString sNumSuffix;
	OUString sNumFormat;
	OUString sNumLetterSync;

	// iterate over xattribute list and fill values
	sal_Int16 nLength = xAttrList->getLength();
	for(sal_Int16 nAttr = 0; nAttr < nLength; nAttr++)
	{
		OUString sLocalName;
		sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
			GetKeyByAttrName( xAttrList->getNameByIndex(nAttr), 
							  &sLocalName );
		OUString sAttrValue = xAttrList->getValueByIndex(nAttr);

		if (XML_NAMESPACE_TEXT == nPrefix)
		{
			if (IsXMLToken(sLocalName, XML_START_VALUE))
			{
				sal_Int32 nTmp;
				if (SvXMLUnitConverter::convertNumber(nTmp, sAttrValue))
				{
					nNumRestartAt = nTmp - 1;
					bNumRestart = sal_True;
				}
			}
		}
		else if (XML_NAMESPACE_STYLE == nPrefix)
		{
			if (IsXMLToken(sLocalName, XML_NUM_PREFIX))
			{
				sNumPrefix = sAttrValue;
				bNumOwn = sal_True;
			}
			else if (IsXMLToken(sLocalName, XML_NUM_SUFFIX))
			{
				sNumSuffix = sAttrValue;
				bNumOwn = sal_True;
			}
			else if (IsXMLToken(sLocalName, XML_NUM_FORMAT))
			{
				sNumFormat = sAttrValue;
				bNumOwn = sal_True;
			}
			else if (IsXMLToken(sLocalName, XML_NUM_LETTER_SYNC))
			{
				sNumLetterSync = sAttrValue;
				bNumOwn = sal_True;
			}
		}
	}

	// OK, now we have all values and can fill the XMLPropertyState vector
	Any aAny;
	sal_Bool bEndnote = IsXMLToken(GetLocalName(), XML_ENDNOTES_CONFIGURATION);

	aAny.setValue( &bNumOwn, ::getBooleanCppuType() );
	sal_Int32 nIndex = rMapper->FindEntryIndex( bEndnote ? 
		CTF_SECTION_ENDNOTE_NUM_OWN : CTF_SECTION_FOOTNOTE_NUM_OWN );
	XMLPropertyState aNumOwn( nIndex, aAny );
	rProperties.push_back( aNumOwn );

	aAny.setValue( &bNumRestart, ::getBooleanCppuType() );
	nIndex = rMapper->FindEntryIndex( bEndnote ? 
		CTF_SECTION_ENDNOTE_NUM_RESTART : CTF_SECTION_FOOTNOTE_NUM_RESTART );
	XMLPropertyState aNumRestart( nIndex, aAny );
	rProperties.push_back( aNumRestart );

	aAny <<= nNumRestartAt;
	nIndex = rMapper->FindEntryIndex( bEndnote ? 
		CTF_SECTION_ENDNOTE_NUM_RESTART_AT : 
		CTF_SECTION_FOOTNOTE_NUM_RESTART_AT );
	XMLPropertyState aNumRestartAtState( nIndex, aAny );
	rProperties.push_back( aNumRestartAtState );

	sal_Int16 nNumType = NumberingType::ARABIC;
	GetImport().GetMM100UnitConverter().convertNumFormat( nNumType,
													sNumFormat,
													sNumLetterSync );
	aAny <<= nNumType;
	nIndex = rMapper->FindEntryIndex( bEndnote ? 
		CTF_SECTION_ENDNOTE_NUM_TYPE : CTF_SECTION_FOOTNOTE_NUM_TYPE );
	XMLPropertyState aNumFormatState( nIndex, aAny );
	rProperties.push_back( aNumFormatState );

	aAny <<= sNumPrefix;
	nIndex = rMapper->FindEntryIndex( bEndnote ? 
		CTF_SECTION_ENDNOTE_NUM_PREFIX : CTF_SECTION_FOOTNOTE_NUM_PREFIX );
	XMLPropertyState aPrefixState( nIndex, aAny );
	rProperties.push_back( aPrefixState );
		
	aAny <<= sNumSuffix;
	nIndex = rMapper->FindEntryIndex( bEndnote ? 
		CTF_SECTION_ENDNOTE_NUM_SUFFIX : CTF_SECTION_FOOTNOTE_NUM_SUFFIX );
	XMLPropertyState aSuffixState( nIndex, aAny );
	rProperties.push_back( aSuffixState );

	aAny.setValue( &bEnd, ::getBooleanCppuType() );
	nIndex = rMapper->FindEntryIndex( bEndnote ? 
		CTF_SECTION_ENDNOTE_END : CTF_SECTION_FOOTNOTE_END );
	DBG_ASSERT( nIndex == nPropIndex, "Wrong property mapper index received.");
	XMLPropertyState aEndState( nIndex, aAny );
	rProperties.push_back( aEndState );
}
}//end of namespace binfilter
