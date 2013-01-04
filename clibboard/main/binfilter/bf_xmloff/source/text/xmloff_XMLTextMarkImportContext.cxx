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




#include "XMLTextMarkImportContext.hxx"

#include "rtl/ustring.hxx"

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _XMLOFF_XMLUCONV_HXX
#include "xmluconv.hxx"
#endif


#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif

#ifndef _XMLOFF_NMSPMAP_HXX 
#include "nmspmap.hxx"
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif


#ifndef _COM_SUN_STAR_TEXT_XTEXTCONTENT_HPP_
#include <com/sun/star/text/XTextContent.hpp>
#endif



#ifndef _COM_SUN_STAR_CONTAINER_XNAMED_HPP_
#include <com/sun/star/container/XNamed.hpp>
#endif
namespace binfilter {


using namespace ::rtl;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::xml::sax;
using namespace ::binfilter::xmloff::token;
	
TYPEINIT1( XMLTextMarkImportContext, SvXMLImportContext);

XMLTextMarkImportContext::XMLTextMarkImportContext(
	SvXMLImport& rImport, 
	XMLTextImportHelper& rHlp,
	sal_uInt16 nPrefix,
	const OUString& rLocalName ) :
		SvXMLImportContext(rImport, nPrefix, rLocalName),
		rHelper(rHlp)
{
}

enum lcl_MarkType { TypeReference, TypeReferenceStart, TypeReferenceEnd,
					TypeBookmark, TypeBookmarkStart, TypeBookmarkEnd };

static SvXMLEnumMapEntry __READONLY_DATA lcl_aMarkTypeMap[] =
{
	{ XML_REFERENCE_MARK,			TypeReference },
	{ XML_REFERENCE_MARK_START,	    TypeReferenceStart },
	{ XML_REFERENCE_MARK_END,		TypeReferenceEnd },
	{ XML_BOOKMARK,				    TypeBookmark },
	{ XML_BOOKMARK_START,			TypeBookmarkStart },
	{ XML_BOOKMARK_END,			    TypeBookmarkEnd },
	{ XML_TOKEN_INVALID,    		0 },
};

void XMLTextMarkImportContext::StartElement(
	const Reference<XAttributeList> & xAttrList)
{
	const OUString sAPI_reference_mark(
		RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.ReferenceMark"));
	const OUString sAPI_bookmark(
		RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.Bookmark"));


	OUString sName;

	if (FindName(GetImport(), xAttrList, sName))
	{
		sal_uInt16 nTmp;
		if (SvXMLUnitConverter::convertEnum(nTmp, GetLocalName(), 
											lcl_aMarkTypeMap))
		{
			switch ((lcl_MarkType)nTmp)
			{
				case TypeReference:
					// export point reference mark
					CreateAndInsertMark(GetImport(),
										sAPI_reference_mark,
										sName,
									   rHelper.GetCursorAsRange()->getStart());
					break;

				case TypeBookmark:
					// export point bookmark
					CreateAndInsertMark(GetImport(),
										sAPI_bookmark,
										sName,
									   rHelper.GetCursorAsRange()->getStart());
					break;

				case TypeBookmarkStart:
					// save XTextRange for later construction of bookmark
					rHelper.InsertBookmarkStartRange(
						sName, rHelper.GetCursorAsRange()->getStart());
					break;

				case TypeBookmarkEnd:
				{
					// get old range, and construct
					Reference<XTextRange> xStartRange;
					if (rHelper.FindAndRemoveBookmarkStartRange(xStartRange, 
																sName))
					{
						Reference<XTextRange> xEndRange(
							rHelper.GetCursorAsRange()->getEnd());

						// check if beginning and end are in same XText
						if (xStartRange->getText() == xEndRange->getText())
						{
							// create range for insertion
							Reference<XTextCursor> xInsertionCursor =
								rHelper.GetText()->createTextCursorByRange(
									xEndRange);
							xInsertionCursor->gotoRange(xStartRange, sal_True);
							
							//DBG_ASSERT(! xInsertionCursor->isCollapsed(), 
							// 				"we want no point mark");
							// can't assert, because someone could
							// create a file with subsequence
							// start/end elements

							Reference<XTextRange> xInsertionRange(
								xInsertionCursor, UNO_QUERY);

							// insert reference
							CreateAndInsertMark(GetImport(),
												sAPI_bookmark,
												sName, 
												xInsertionRange);
						}
						// else: beginning/end in different XText -> ignore!
					}
					// else: no start found -> ignore!
					break;
				}

				case TypeReferenceStart:
				case TypeReferenceEnd:	
					DBG_ERROR("reference start/end are handled in txtparai !");
					break;

				default:
					DBG_ERROR("unknown mark type");
					break;
			}
		}
	}
}


void XMLTextMarkImportContext::CreateAndInsertMark(
	SvXMLImport& rImport,
	const OUString& sServiceName,
	const OUString& sMarkName,
	const Reference<XTextRange> & rRange)
{
	// create mark
	Reference<XMultiServiceFactory> xFactory(rImport.GetModel(),UNO_QUERY);
	if( xFactory.is() )	
	{
		Reference<XInterface> xIfc = xFactory->createInstance(sServiceName);
		
		// set name
		Reference<XNamed> xNamed(xIfc, UNO_QUERY);
		if (xNamed.is())
		{
			xNamed->setName(sMarkName);

			// cast to XTextContent and attach to document
			Reference<XTextContent> xTextContent(xIfc, UNO_QUERY);
			if (xTextContent.is())
			{
				// if inserting marks, bAbsorb==sal_False will cause
				// collapsing of the given XTextRange.
				rImport.GetTextImport()->GetText()->insertTextContent(rRange, 
													 xTextContent, sal_True);
			}
		}
	}
}

sal_Bool XMLTextMarkImportContext::FindName(
	SvXMLImport& rImport,
	const Reference<XAttributeList> & xAttrList,
	OUString& sName)
{
	sal_Bool bNameOK(sal_False);

	// find name attribute first
	sal_Int16 nLength = xAttrList->getLength();
	for(sal_Int16 nAttr = 0; nAttr < nLength; nAttr++)
	{
		OUString sLocalName;
		sal_uInt16 nPrefix = rImport.GetNamespaceMap().
			GetKeyByAttrName( xAttrList->getNameByIndex(nAttr), 
							  &sLocalName );

		if ( (XML_NAMESPACE_TEXT == nPrefix) &&
			 IsXMLToken(sLocalName, XML_NAME)   )
		{
			sName = xAttrList->getValueByIndex(nAttr);
			bNameOK = sal_True;
		}
	}

	return bNameOK;
}
}//end of namespace binfilter
