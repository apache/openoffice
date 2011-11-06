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


#ifndef _XMLOFF_TEXTSTYLE_HXX_
#define _XMLOFF_TEXTSTYLE_HXX_

#ifndef _XMLOFF_STYLEEXP_HXX_
#include <bf_xmloff/styleexp.hxx>
#endif
#ifndef _UNIVERSALL_REFERENCE_HXX
#include <bf_xmloff/uniref.hxx>
#endif
namespace binfilter {

class XMLPropertySetMapper;

class XMLTextStyleExport : public XMLStyleExport
{
	UniReference < XMLPropertySetMapper > xParaPropMapper;

	void exportNumStyles( sal_Bool bUsed );

public:

	XMLTextStyleExport(
		SvXMLExport& rExp,
//		const ::com::sun::star::uno::Reference<
//				::com::sun::star::xml::sax::XDocumentHandler > & rHandler,
//		const ::com::sun::star::uno::Reference<
//				::com::sun::star::frame::XModel > & rModel,
		const ::rtl::OUString& rPoolStyleName,
//		const SvXMLNamespaceMap &rNamespaceMap,
//	 	MapUnit eDUnit,
		SvXMLAutoStylePoolP *pAutoStyleP=0 );
	virtual ~XMLTextStyleExport();

	void exportStyles( sal_Bool bUsed );

	UniReference < XMLPropertySetMapper >& getParaPropMapper();
};

}//end of namespace binfilter
#endif
