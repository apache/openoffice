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


#ifndef SC_XMLCOLI_HXX
#define SC_XMLCOLI_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include <bf_xmloff/xmlictxt.hxx>
#endif

#ifndef _XMLOFF_XMLIMP_HXX
#include <bf_xmloff/xmlimp.hxx>
#endif
namespace binfilter {

class ScXMLImport;

class ScXMLTableColContext : public SvXMLImportContext
{
	sal_Int32				nColCount;
	::rtl::OUString			sStyleName;
	::rtl::OUString			sVisibility;
	::rtl::OUString			sCellStyleName;

	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

	ScXMLTableColContext( ScXMLImport& rImport, USHORT nPrfx,
					   const ::rtl::OUString& rLName,
					   const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual ~ScXMLTableColContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

class ScXMLTableColsContext : public SvXMLImportContext
{
	sal_Int32	nHeaderStartCol;
	sal_Int32	nHeaderEndCol;
	sal_Int32	nGroupStartCol;
	sal_Int32	nGroupEndCol;
	sal_Bool	bHeader : 1;
	sal_Bool	bGroup : 1;
	sal_Bool	bGroupDisplay : 1;

	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }

public:

	ScXMLTableColsContext( ScXMLImport& rImport, USHORT nPrfx,
					   const ::rtl::OUString& rLName,
					   const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList,
						const sal_Bool bHeader, const sal_Bool bGroup);

	virtual ~ScXMLTableColsContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

} //namespace binfilter
#endif
