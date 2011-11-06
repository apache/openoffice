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



#ifndef _XMLOFF_XMLSTARBASICCONTEXTFACTORY_HXX
#include "XMLStarBasicContextFactory.hxx"
#endif

#ifndef _XMLOFF_XMLEVENTSIMPORTCONTEXT_HXX
#include "XMLEventsImportContext.hxx"
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

namespace binfilter {


using namespace ::binfilter::xmloff::token;

using ::rtl::OUString;
using ::com::sun::star::xml::sax::XAttributeList;
using ::com::sun::star::beans::PropertyValue;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Any;


XMLStarBasicContextFactory::XMLStarBasicContextFactory() :
	sEventType(RTL_CONSTASCII_USTRINGPARAM("EventType")),
	sLibrary(RTL_CONSTASCII_USTRINGPARAM("Library")),
	sMacroName(RTL_CONSTASCII_USTRINGPARAM("MacroName")),
	sStarBasic(RTL_CONSTASCII_USTRINGPARAM("StarBasic"))
{
}

XMLStarBasicContextFactory::~XMLStarBasicContextFactory()
{
}

SvXMLImportContext* XMLStarBasicContextFactory::CreateContext(
	SvXMLImport& rImport,
	sal_uInt16 nPrefix,
	const OUString& rLocalName,
	const Reference<XAttributeList> & xAttrList,
	XMLEventsImportContext* rEvents,
	const OUString& rApiEventName,
	const OUString& rLanguage)
{
	OUString sLibraryVal;
	OUString sMacroNameVal;

	sal_Int16 nCount = xAttrList->getLength();
	for(sal_Int16 nAttr = 0; nAttr < nCount; nAttr++)
	{
		OUString sLocalName;
		sal_uInt16 nPrefix = rImport.GetNamespaceMap().
			GetKeyByAttrName( xAttrList->getNameByIndex(nAttr), &sLocalName );

		if (XML_NAMESPACE_SCRIPT == nPrefix)
		{
			if (IsXMLToken(sLocalName, XML_LIBRARY))
			{
				sLibraryVal = xAttrList->getValueByIndex(nAttr);
			}
			if (IsXMLToken(sLocalName, XML_LOCATION))
			{
				sLibraryVal = xAttrList->getValueByIndex(nAttr);
                if ( IsXMLToken( sLibraryVal, XML_APPLICATION ) )
                    sLibraryVal = 
                        OUString(RTL_CONSTASCII_USTRINGPARAM("StarOffice"));
			}
			else if (IsXMLToken(sLocalName, XML_MACRO_NAME))
			{
				sMacroNameVal = xAttrList->getValueByIndex(nAttr);
			}
			// else: ingore
		}
		// else: ignore
	}

	Sequence<PropertyValue> aValues(3);

	// EventType
	aValues[0].Name = sEventType;
	aValues[0].Value <<= sStarBasic;

	// library name
	aValues[1].Name = sLibrary;
	aValues[1].Value <<= sLibraryVal;

	// macro name
	aValues[2].Name = sMacroName;
	aValues[2].Value <<= sMacroNameVal;

	// add values for event now
	rEvents->AddEventValues(rApiEventName, aValues);

	// return dummy context
	return new SvXMLImportContext(rImport, nPrefix, rLocalName);
}
}//end of namespace binfilter
