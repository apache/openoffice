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



#ifndef _XMLOFF_XMLSTARBASICEXPORTHANDLER_HXX
#include "XMLStarBasicExportHandler.hxx"
#endif



#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif


#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif
namespace binfilter {


using namespace ::com::sun::star::uno;
using namespace ::binfilter::xmloff::token;

using ::rtl::OUString;
using ::com::sun::star::beans::PropertyValue;


XMLStarBasicExportHandler::XMLStarBasicExportHandler() :
	sStarBasic(RTL_CONSTASCII_USTRINGPARAM("StarBasic")),
    sStarOffice(RTL_CONSTASCII_USTRINGPARAM("StarOffice")),
    sApplication(RTL_CONSTASCII_USTRINGPARAM("application")),
	sLibrary(RTL_CONSTASCII_USTRINGPARAM("Library")),
	sMacroName(RTL_CONSTASCII_USTRINGPARAM("MacroName"))
{
}

XMLStarBasicExportHandler::~XMLStarBasicExportHandler()
{
}

void XMLStarBasicExportHandler::Export(
	SvXMLExport& rExport,
	const OUString& rEventName,
	Sequence<PropertyValue> & rValues,
	sal_Bool bUseWhitespace)
{
	rExport.AddAttribute(XML_NAMESPACE_SCRIPT, XML_LANGUAGE, sStarBasic);
	rExport.AddAttribute(XML_NAMESPACE_SCRIPT, XML_EVENT_NAME, rEventName);

	sal_Int32 nCount = rValues.getLength();
	for(sal_Int32 i = 0; i < nCount; i++)
	{
		if (sLibrary.equals(rValues[i].Name))
		{
			OUString sTmp;
			rValues[i].Value >>= sTmp;
			rExport.AddAttribute(
                XML_NAMESPACE_SCRIPT, XML_LOCATION,
                (sTmp.equalsIgnoreAsciiCase(sApplication) || sTmp.equalsIgnoreAsciiCase(sStarOffice) ) ? XML_APPLICATION
                                                        : XML_DOCUMENT );
		}
		else if (sMacroName.equals(rValues[i].Name))
		{
			OUString sTmp;
			rValues[i].Value >>= sTmp;
			rExport.AddAttribute(XML_NAMESPACE_SCRIPT, XML_MACRO_NAME, sTmp);
		}
		// else: disregard
	}

	SvXMLElementExport aEventElemt(rExport, XML_NAMESPACE_SCRIPT, XML_EVENT,
								   bUseWhitespace, sal_False);
}
}//end of namespace binfilter
