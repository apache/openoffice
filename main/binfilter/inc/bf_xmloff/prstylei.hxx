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


#ifndef _XMLOFF_PRSTYLEI_HXX_
#define _XMLOFF_PRSTYLEI_HXX_

#ifndef _COM_SUN_STAR_STYLE_XSTYLE_HPP_
#include <com/sun/star/style/XStyle.hpp>
#endif
#ifndef __SGI_STL_VECTOR
#include <vector>
#endif
#ifndef _XMLOFF_XMLSTYLE_HXX_
#include <bf_xmloff/xmlstyle.hxx>
#endif
namespace com { namespace sun { namespace star {
	namespace beans { class XPropertySet; }
} } }
namespace binfilter {

struct XMLPropertyState;
class SvXMLStylesContext;



class XMLPropStyleContext : public SvXMLStyleContext
{
	const ::rtl::OUString sIsPhysical;
	const ::rtl::OUString sFollowStyle;
	::std::vector< XMLPropertyState > aProperties;
	::com::sun::star::uno::Reference < ::com::sun::star::style::XStyle > xStyle;
	SvXMLImportContextRef				xStyles;

protected:

	virtual void SetAttribute( sal_uInt16 nPrefixKey,
							   const ::rtl::OUString& rLocalName,
							   const ::rtl::OUString& rValue );
	SvXMLStylesContext *GetStyles() { return (SvXMLStylesContext *)&xStyles; }
	::std::vector< XMLPropertyState > & GetProperties() { return aProperties; }

	// This methos my be overloaded to create a new style. Its called by
	// CreateInsert to create a style if a style with the requested family and
	// name couldn't be found. The st
	virtual ::com::sun::star::uno::Reference <
		::com::sun::star::style::XStyle > Create();

public:

	TYPEINFO();

	XMLPropStyleContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
			const ::rtl::OUString& rLName,
			const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttrList,
			SvXMLStylesContext& rStyles, sal_uInt16 nFamily = 0,
		    sal_Bool bDefaultStyle=sal_False );
	virtual ~XMLPropStyleContext();

	virtual SvXMLImportContext *CreateChildContext(
			sal_uInt16 nPrefix,
			const ::rtl::OUString& rLocalName,
			const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttrList );

	virtual void FillPropertySet(
			const ::com::sun::star::uno::Reference<
				::com::sun::star::beans::XPropertySet > & rPropSet );

	const SvXMLStylesContext *GetStyles() const { return (const SvXMLStylesContext *)&xStyles; }
	const ::std::vector< XMLPropertyState > & GetProperties() const { return aProperties; }

	const ::com::sun::star::uno::Reference <
				::com::sun::star::style::XStyle >&
	   		GetStyle() const { return xStyle; }
	void SetStyle(
			const ::com::sun::star::uno::Reference<
				::com::sun::star::style::XStyle >& xStl) { xStyle = xStl; }

	virtual void SetDefaults();

	virtual void CreateAndInsert( sal_Bool bOverwrite );
	virtual void Finish( sal_Bool bOverwrite );
};

}//end of namespace binfilter
#endif
