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



#ifndef _XMLOFF_XMLCHANGEDREGIONIMPORTCONTEXT_HXX
#include "XMLChangedRegionImportContext.hxx"
#endif

#ifndef _XMLOFF_XMLCHANGEELEMENTIMPORTCONTEXT_HXX
#include "XMLChangeElementImportContext.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _COM_SUN_STAR_UTIL_DATETIME_HPP_
#include <com/sun/star/util/DateTime.hpp>
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


using namespace ::binfilter::xmloff::token;

using ::rtl::OUString;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::text::XTextCursor;
using ::com::sun::star::util::DateTime;
using ::com::sun::star::xml::sax::XAttributeList;


	
TYPEINIT1(XMLChangedRegionImportContext, SvXMLImportContext);

XMLChangedRegionImportContext::XMLChangedRegionImportContext(
	SvXMLImport& rImport,
	sal_uInt16 nPrefix,
	const OUString& rLocalName) :
        SvXMLImportContext(rImport, nPrefix, rLocalName),
        bMergeLastPara(sal_True)
{
}

XMLChangedRegionImportContext::~XMLChangedRegionImportContext()
{
}

void XMLChangedRegionImportContext::StartElement(
	const Reference<XAttributeList> & xAttrList)
{
	// process attributes: id
	sal_Int16 nLength = xAttrList->getLength();
	for(sal_Int16 nAttr = 0; nAttr < nLength; nAttr++)
	{
		OUString sLocalName;
		sal_uInt16 nPrefix = GetImport().GetNamespaceMap().
			GetKeyByAttrName( xAttrList->getNameByIndex(nAttr), 
							  &sLocalName );

        const OUString sValue = xAttrList->getValueByIndex(nAttr);
		if ( XML_NAMESPACE_TEXT == nPrefix ) 
        {
            if( IsXMLToken( sLocalName, XML_ID ) )
            {
                sID = sValue;
            }
            else if( IsXMLToken( sLocalName, XML_MERGE_LAST_PARAGRAPH ) )
            {
                sal_Bool bTmp;
                if( SvXMLUnitConverter::convertBool(bTmp, sValue) )
                {
                    bMergeLastPara = bTmp;
                }
            }
        }
	}
}

SvXMLImportContext* XMLChangedRegionImportContext::CreateChildContext(
	sal_uInt16 nPrefix,
	const OUString& rLocalName,
	const Reference<XAttributeList> & xAttrList)
{
	SvXMLImportContext* pContext = NULL;

	if (XML_NAMESPACE_TEXT == nPrefix)
	{
		if ( IsXMLToken( rLocalName, XML_INSERTION ) || 
             IsXMLToken( rLocalName, XML_DELETION ) || 
             IsXMLToken( rLocalName, XML_FORMAT_CHANGE ) )
		{
			// create XMLChangeElementImportContext for all kinds of changes
			pContext = new XMLChangeElementImportContext(
			   GetImport(), nPrefix, rLocalName, 
			   IsXMLToken( rLocalName, XML_DELETION ),
			   *this);
		}
		// else: it may be a text element, see below
	}

	if (NULL == pContext)
	{
		pContext = SvXMLImportContext::CreateChildContext(nPrefix, rLocalName, 
														  xAttrList);

		// was it a text element? If not, use default!
		if (NULL == pContext)
		{
			pContext = SvXMLImportContext::CreateChildContext(
				nPrefix, rLocalName, xAttrList);
		}
	}

	return pContext;
}

void XMLChangedRegionImportContext::EndElement()
{
	// restore old XCursor (if necessary)
	if (xOldCursor.is())
	{
		// delete last paragraph 
		// (one extra paragraph was inserted in the beginning)
		UniReference<XMLTextImportHelper> rHelper = 
			GetImport().GetTextImport();
		rHelper->DeleteParagraph();

		GetImport().GetTextImport()->SetCursor(xOldCursor);
		xOldCursor = NULL;
	}
}

void XMLChangedRegionImportContext::SetChangeInfo(
	const OUString& rType,
	const OUString& rAuthor,
	const OUString& rComment,
	const OUString& rDate)
{
	DateTime aDateTime;
	if (SvXMLUnitConverter::convertDateTime(aDateTime, rDate))
	{
		GetImport().GetTextImport()->RedlineAdd(
			rType, sID, rAuthor, rComment, aDateTime, bMergeLastPara);
	}
}

void XMLChangedRegionImportContext::UseRedlineText()
{
	// if we haven't already installed the redline cursor, do it now
	if (! xOldCursor.is())
	{
		// get TextImportHelper and old Cursor
		UniReference<XMLTextImportHelper> rHelper(GetImport().GetTextImport());
		Reference<XTextCursor> xCursor( rHelper->GetCursor() );

		// create Redline and new Cursor 
		Reference<XTextCursor> xNewCursor = 
			rHelper->RedlineCreateText(xCursor, sID);

		if (xNewCursor.is())
		{
			// save old cursor and install new one
			xOldCursor = xCursor;
			rHelper->SetCursor( xNewCursor );
		}
		// else: leave as is
	}
}
}//end of namespace binfilter
