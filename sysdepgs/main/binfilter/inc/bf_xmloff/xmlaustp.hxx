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



#ifndef _XMLOFF_XMLASTPLP_HXX
#define _XMLOFF_XMLASTPLP_HXX

#ifndef _COM_SUN_STAR_XML_SAX_XDOCUMENTHANDLER_HPP_
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#endif

#ifndef _XMLOFF_PROPERTYSETMAPPER_HXX
#include <bf_xmloff/xmlprmap.hxx>
#endif

#ifndef _UNIVERSALL_REFERENCE_HXX
#include <bf_xmloff/uniref.hxx>
#endif
namespace binfilter {

class SvXMLExportPropertyMapper;
class SvXMLNamespaceMap;
class SvXMLAttributeList;
class SvXMLAutoStylePoolP_Impl;
class SvXMLExport;

class SvXMLAutoStylePoolP : public UniRefBase
{
	friend class SvXMLAutoStylePoolP_Impl;

	SvXMLAutoStylePoolP_Impl	*pImpl;

protected:

	virtual void exportStyleAttributes(
			SvXMLAttributeList& rAttrList,
			sal_Int32 nFamily,
			const ::std::vector< XMLPropertyState >& rProperties,
			const SvXMLExportPropertyMapper& rPropExp
			, const SvXMLUnitConverter& rUnitConverter,
			const SvXMLNamespaceMap& rNamespaceMap
            ) const;

// this methode is executed after Properties Export, so you can e.g. export a map or so on
	virtual void exportStyleContent(
			const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > & rHandler,
			sal_Int32 nFamily,
			const ::std::vector< XMLPropertyState >& rProperties,
			const SvXMLExportPropertyMapper& rPropExp
			, const SvXMLUnitConverter& rUnitConverter,
			const SvXMLNamespaceMap& rNamespaceMap
            ) const;

public:

	SvXMLAutoStylePoolP( SvXMLExport& rExport);

	virtual ~SvXMLAutoStylePoolP();

    SvXMLExport& GetExport() const;

	/** register a new family with its appropriate instance of a derivation of XMLPropertySetMapper
		for family numbers see families.hxx
		if bAsFamily is sal_False, the family name is used as element name
	 */
	// TODO: Remove this ugly method as soon as possible
	void AddFamily( sal_Int32 nFamily, const ::rtl::OUString& rStrName, SvXMLExportPropertyMapper* pMapper, ::rtl::OUString aStrPrefix, sal_Bool bAsFamily = sal_True );
	void AddFamily( sal_Int32 nFamily, const ::rtl::OUString& rStrName,
					const UniReference< SvXMLExportPropertyMapper >& rMapper,
					const ::rtl::OUString& rStrPrefix, sal_Bool bAsFamily = sal_True );

	/// Register a name that must not be used as a generated name.
	void RegisterName( sal_Int32 nFamily, const ::rtl::OUString& rName );

	/// Add an item set to the pool and return its generated name.
	::rtl::OUString Add( sal_Int32 nFamily, const ::std::vector< XMLPropertyState >& rProperties );
	::rtl::OUString Add( sal_Int32 nFamily, const ::rtl::OUString& rParent, const ::std::vector< XMLPropertyState >& rProperties );
	sal_Bool Add(::rtl::OUString& rName, sal_Int32 nFamily, const ::rtl::OUString& rParent, const ::std::vector< XMLPropertyState >& rProperties );


	/// Find an item set's name.
	::rtl::OUString Find( sal_Int32 nFamily, const ::rtl::OUString& rParent, const ::std::vector< XMLPropertyState >& rProperties ) const;

	/** Export all item sets ofs a certain class in the order in that they have been added. */
	void exportXML( sal_Int32 nFamily
		, const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > & rHandler,
		const SvXMLUnitConverter& rUnitConverter,
		const SvXMLNamespaceMap& rNamespaceMap
        ) const;

	void ClearEntries();
};

}//end of namespace binfilter
#endif	//  _XMLOFF_XMLASTPLP_HXX
