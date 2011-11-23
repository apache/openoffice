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



#ifndef _XMLTEXTLISTBLOCKCONTEXT_HXX
#define _XMLTEXTLISTBLOCKCONTEXT_HXX

#ifndef _COM_SUN_STAR_CONTAINER_XINDEXREPLACE_HPP_
#include <com/sun/star/container/XIndexReplace.hpp>
#endif

#ifndef _XMLOFF_XMLICTXT_HXX 
#include "xmlictxt.hxx"
#endif
namespace binfilter {

class XMLTextImportHelper;

class XMLTextListBlockContext : public SvXMLImportContext
{
	XMLTextImportHelper&	rTxtImport;

	::com::sun::star::uno::Reference<
				::com::sun::star::container::XIndexReplace > xNumRules;

	const ::rtl::OUString	sNumberingRules;
	::rtl::OUString			sStyleName;
	SvXMLImportContextRef	xParentListBlock;
	sal_Int16				nLevel;
	sal_Int16				nLevels;
	sal_Bool				bOrdered : 1;
	sal_Bool				bRestartNumbering : 1;
	sal_Bool				bSetDefaults : 1;


public:

	TYPEINFO();

	XMLTextListBlockContext( SvXMLImport& rImport,
			XMLTextImportHelper& rTxtImp, sal_uInt16 nPrfx,
				const ::rtl::OUString& rLName,
			   	const ::com::sun::star::uno::Reference<
					::com::sun::star::xml::sax::XAttributeList > & xAttrList,
				sal_Bool bOrdered );
	virtual ~XMLTextListBlockContext();

	virtual void EndElement();

	SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
				const ::rtl::OUString& rLocalName,
			 	const ::com::sun::star::uno::Reference<
					::com::sun::star::xml::sax::XAttributeList > & xAttrList );

	const ::rtl::OUString& GetStyleName() const { return sStyleName; }
	sal_Int16 GetLevel() const { return nLevel; }
	sal_Bool IsRestartNumbering() const { return bRestartNumbering; }
	void ResetRestartNumbering() { bRestartNumbering = sal_False; }

//	sal_Bool HasGeneratedStyle() const { return xGenNumRule.is(); }
	const ::com::sun::star::uno::Reference <
		::com::sun::star::container::XIndexReplace >& GetNumRules() const
		{ return xNumRules; }
};


}//end of namespace binfilter
#endif
