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



#ifndef _XMLOFF_XMLINDEXSPANENTRYCONTEXT_HXX_
#include "XMLIndexSpanEntryContext.hxx"
#endif

#include "rtl/ustring.hxx"

#ifndef _XMLOFF_XMLINDEXTEMPLATECONTEXT_HXX_
#include "XMLIndexTemplateContext.hxx"
#endif

namespace binfilter {


using ::rtl::OUString;
using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::Any;
using ::com::sun::star::beans::PropertyValue;


TYPEINIT1( XMLIndexSpanEntryContext, XMLIndexSimpleEntryContext);

XMLIndexSpanEntryContext::XMLIndexSpanEntryContext(
	SvXMLImport& rImport, 
	XMLIndexTemplateContext& rTemplate,
	sal_uInt16 nPrfx,
	const OUString& rLocalName ) :
		XMLIndexSimpleEntryContext(rImport, rTemplate.sTokenText, 
								   rTemplate, nPrfx, rLocalName)
{
	nValues++;	// one more for the text string
}

XMLIndexSpanEntryContext::~XMLIndexSpanEntryContext()
{
}

void XMLIndexSpanEntryContext::Characters(const OUString& sString)
{
	sContent.append(sString);
}

void XMLIndexSpanEntryContext::FillPropertyValues(
	Sequence<PropertyValue> & rValues)
{
	// call superclass for token type, stylename, 
	XMLIndexSimpleEntryContext::FillPropertyValues(rValues);

	// content
	Any aAny;
	aAny <<= sContent.makeStringAndClear();
	rValues[nValues-1].Name = rTemplateContext.sText;
	rValues[nValues-1].Value = aAny;
}

}//end of namespace binfilter
