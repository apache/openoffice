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



#ifndef _SC_XMLTABLEMASTERPAGEEXPORT_HXX
#define _SC_XMLTABLEMASTERPAGEEXPORT_HXX

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

#ifndef _XMLOFF_XMLTOKEN_HXX
#include <bf_xmloff/xmltoken.hxx>
#endif
#ifndef _XMLOFF_XMLTEXTMASTERPAGEEXPORT_HXX
#include <bf_xmloff/XMLTextMasterPageExport.hxx>
#endif

#ifndef _COM_SUN_STAR_SHEET_XHEADERFOOTERCONTENT_HPP_
#include <com/sun/star/sheet/XHeaderFooterContent.hpp>
#endif

#include "xmlexprt.hxx"
namespace binfilter {

namespace com { namespace sun { namespace star {
	namespace text { class XText; }
} } }

class XMLTableMasterPageExport : public XMLTextMasterPageExport
{
	void exportHeaderFooter(const ::com::sun::star::uno::Reference < ::com::sun::star::sheet::XHeaderFooterContent >& xHeaderFooter,
							const xmloff::token::XMLTokenEnum aName,
							const sal_Bool bDisplay);

protected:
	virtual void exportHeaderFooterContent(
			const ::com::sun::star::uno::Reference<
				::com::sun::star::text::XText >& rText,
	        sal_Bool bAutoStyles, sal_Bool bProgress );

	virtual void exportMasterPageContent(
				const ::com::sun::star::uno::Reference <
					::com::sun::star::beans::XPropertySet > & rPropSet,
				 sal_Bool bAutoStyles );

public:
	XMLTableMasterPageExport( ScXMLExport& rExp );
	~XMLTableMasterPageExport();
};


} //namespace binfilter
#endif	//  _XMLOFF_XMLTABLEMASTERPAGEEXPORT_HXX

