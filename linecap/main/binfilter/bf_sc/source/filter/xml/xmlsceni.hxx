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


#ifndef SC_XMLSCENI_HXX
#define SC_XMLSCENI_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include <bf_xmloff/xmlictxt.hxx>
#endif

#ifndef _XMLOFF_XMLIMP_HXX
#include <bf_xmloff/xmlimp.hxx>
#endif

#ifndef _TOOLS_COLOR_HXX
#include <tools/color.hxx>
#endif

#ifndef SC_RANGELST_HXX
#include "rangelst.hxx"
#endif
namespace binfilter {

class ScXMLImport;

class ScXMLTableScenarioContext : public SvXMLImportContext
{
private:
	::rtl::OUString	sComment;
	Color			aBorderColor;
	ScRangeList		aScenarioRanges;
	sal_Bool		bDisplayBorder : 1;
	sal_Bool		bCopyBack : 1;
	sal_Bool		bCopyStyles : 1;
	sal_Bool		bCopyFormulas : 1;
	sal_Bool		bIsActive : 1;

	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

	ScXMLTableScenarioContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual ~ScXMLTableScenarioContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

} //namespace binfilter
#endif
