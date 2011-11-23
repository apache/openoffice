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


#ifndef _SCH_XML_CONTEXTS_HXX_
#define _SCH_XML_CONTEXTS_HXX_

#include "SchXMLImport.hxx"
#include "SchXMLTableContext.hxx"

#ifndef _XMLOFF_XMLICTXT_HXX
#include "xmlictxt.hxx"
#endif
#ifndef _XMLOFF_XMLTKMAP_HXX
#include "xmltkmap.hxx"
#endif
namespace com { namespace sun { namespace star { namespace xml { namespace sax {
		class XAttributeList;
}}}}}
namespace binfilter {


/* ========================================

   These contexts are only nedded by
   SchXMLImport not by the SchXMLImportHelper
   that is also used by other applications

   ======================================== */

class SchXMLDocContext : public SvXMLImportContext
{
private:
	SchXMLImportHelper& mrImportHelper;

public:
	SchXMLDocContext(
		SchXMLImportHelper& rImpHelper,
		SvXMLImport& rImport,
		sal_uInt16 nPrefix,
		const ::rtl::OUString& rLName );
	virtual ~SchXMLDocContext();

	virtual SvXMLImportContext* CreateChildContext(
		sal_uInt16 nPrefix,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
};

// ========================================

class SchXMLBodyContext : public SvXMLImportContext
{
private:
	SchXMLImportHelper& mrImportHelper;

public:
	SchXMLBodyContext(
		SchXMLImportHelper& rImpHelper,
		SvXMLImport& rImport,
		sal_uInt16 nPrefix,
		const ::rtl::OUString& rLName );
	virtual ~SchXMLBodyContext();

	virtual void EndElement();
	virtual SvXMLImportContext* CreateChildContext(
		sal_uInt16 nPrefix,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
};

// ========================================

}//end of namespace binfilter
#endif	// _SCH_XML_CONTEXTS_HXX_
