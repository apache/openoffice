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



#ifndef _XMLOFF_XMLSECTIONFOOTNOTECONFIGEXPORT_HXX
#include "XMLSectionFootnoteConfigExport.hxx"
#endif

#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif


#ifndef _COM_SUN_STAR_STYLE_NUMBERINGTYPE_HPP_
#include <com/sun/star/style/NumberingType.hpp>
#endif


#ifndef _XMLOFF_TXTPRMAP_HXX
#include "txtprmap.hxx"
#endif


#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif

#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif



#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#include "rtl/ustring.hxx"


#include <vector>
namespace binfilter {


using namespace ::binfilter::xmloff::token;

using ::std::vector;
using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using ::com::sun::star::style::NumberingType::ARABIC;


void XMLSectionFootnoteConfigExport::exportXML(
	SvXMLExport& rExport,
	sal_Bool bEndnote,
	const vector<XMLPropertyState> *pProperties,
	sal_uInt32 nIdx,
	const UniReference<XMLPropertySetMapper> & rMapper)
{
	// store and initialize the values
	sal_Bool bNumOwn = sal_False;
	sal_Bool bNumRestart = sal_False;
	sal_Int16 nNumRestartAt = 0;
	sal_Int16 nNumberingType = ARABIC;
	OUString sNumPrefix;
	OUString sNumSuffix;
	sal_Bool bEnd = sal_False;

	// find entries in property states vector
	sal_uInt32 nCount = pProperties->size();
	for(sal_uInt32 i = 0; i < nCount; i++)
	{
		const XMLPropertyState& rState = (*pProperties)[i];

		sal_Int16 nContextId = rMapper->GetEntryContextId(rState.mnIndex);
		if (!bEndnote)
		{
			switch (nContextId)
			{
				case CTF_SECTION_FOOTNOTE_NUM_OWN:
					rState.maValue >>= bNumOwn;
					break;
				case CTF_SECTION_FOOTNOTE_NUM_RESTART:
					rState.maValue >>= bNumRestart;
					break;
				case CTF_SECTION_FOOTNOTE_NUM_RESTART_AT:
					rState.maValue >>= nNumRestartAt;
					break;
				case CTF_SECTION_FOOTNOTE_NUM_TYPE:
					rState.maValue >>= nNumberingType;
					break;
				case CTF_SECTION_FOOTNOTE_NUM_PREFIX:
					rState.maValue >>= sNumPrefix;
					break;
				case CTF_SECTION_FOOTNOTE_NUM_SUFFIX:
					rState.maValue >>= sNumSuffix;
					break;
				case CTF_SECTION_FOOTNOTE_END:
					DBG_ASSERT( i == nIdx, 
								"received wrong property state index" );
					rState.maValue >>= bEnd;
					break;
			}
		}
		else
		{
			switch (nContextId)
			{
				case CTF_SECTION_ENDNOTE_NUM_OWN:
					rState.maValue >>= bNumOwn;
					break;
				case CTF_SECTION_ENDNOTE_NUM_RESTART:
					rState.maValue >>= bNumRestart;
					break;
				case CTF_SECTION_ENDNOTE_NUM_RESTART_AT:
					rState.maValue >>= nNumRestartAt;
					break;
				case CTF_SECTION_ENDNOTE_NUM_TYPE:
					rState.maValue >>= nNumberingType;
					break;
				case CTF_SECTION_ENDNOTE_NUM_PREFIX:
					rState.maValue >>= sNumPrefix;
					break;
				case CTF_SECTION_ENDNOTE_NUM_SUFFIX:
					rState.maValue >>= sNumSuffix;
					break;
				case CTF_SECTION_ENDNOTE_END:
					DBG_ASSERT( i == nIdx, 
								"received wrong property state index" );
					rState.maValue >>= bEnd;
					break;
			}
		}
	}

	// we only make an element if we have an own footnote/endnote numbering
	if (bEnd)
	{
		// start numbering
		OUStringBuffer sBuf;
		if (bNumRestart)
		{
			// restart number is stored as 0.., but interpreted as 1..
			SvXMLUnitConverter::convertNumber(sBuf, 
											  (sal_Int32)(nNumRestartAt+1));
			rExport.AddAttribute(XML_NAMESPACE_TEXT, XML_START_VALUE, 
								 sBuf.makeStringAndClear());
		}

		if (bNumOwn)
		{
			// prefix and suffix
			if (sNumPrefix.getLength() > 0)
			{
					rExport.AddAttribute(XML_NAMESPACE_STYLE, XML_NUM_PREFIX,
										 sNumPrefix);
			}
			if (sNumSuffix.getLength() > 0)
			{
				rExport.AddAttribute(XML_NAMESPACE_STYLE, XML_NUM_SUFFIX,
									 sNumSuffix);
			}

			// number type: num format
			rExport.GetMM100UnitConverter().convertNumFormat( sBuf, 
															  nNumberingType );
			rExport.AddAttribute(XML_NAMESPACE_STYLE, XML_NUM_FORMAT, 
								 sBuf.makeStringAndClear());

			// and letter sync, if applicable
			rExport.GetMM100UnitConverter().convertNumLetterSync( 
				sBuf, nNumberingType );
			if (sBuf.getLength()) 
			{
				rExport.AddAttribute(XML_NAMESPACE_STYLE, 
									 XML_NUM_LETTER_SYNC,
									 sBuf.makeStringAndClear());
			}
		}

		// and finally, the element
		SvXMLElementExport rElem(rExport, XML_NAMESPACE_TEXT, 
								 (bEndnote ? XML_ENDNOTES_CONFIGURATION :
											 XML_FOOTNOTES_CONFIGURATION ), 
								 sal_True, sal_True);
	}
}
}//end of namespace binfilter
