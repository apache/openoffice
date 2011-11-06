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



#ifndef _XIMPPAGE_HXX
#define _XIMPPAGE_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include "xmlictxt.hxx"
#endif

#ifndef _SDXMLIMP_IMPL_HXX
#include "sdxmlimp_impl.hxx"
#endif

#ifndef _XMLOFF_NMSPMAP_HXX
#include "nmspmap.hxx"
#endif

#ifndef _COM_SUN_STAR_DRAWING_XSHAPES_HPP_
#include <com/sun/star/drawing/XShapes.hpp>
#endif

#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif

#ifndef _XIMPSHAPE_HXX
#include "ximpshap.hxx"
#endif
namespace binfilter {

//////////////////////////////////////////////////////////////////////////////
// draw:g context (RECURSIVE)

class SdXMLGenericPageContext : public SvXMLImportContext
{
	// the shape group this group is working on
	::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes > mxShapes;

protected:
	::rtl::OUString				maPageLayoutName;

	void SetLocalShapesContext( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& rNew)
		{ mxShapes = rNew; }

	/** sets the presentation layout at this page. It is used for drawing pages and for the handout master */
	void SetLayout();

	/** deletes all shapes on this drawing page */
	void DeleteAllShapes();

	const SdXMLImport& GetSdImport() const { return (const SdXMLImport&)GetImport(); }
	SdXMLImport& GetSdImport() { return (SdXMLImport&)GetImport(); }

	/** sets the properties from a page master style with the given name on this contexts page */
	void SetPageMaster( ::rtl::OUString& rsPageMasterName );

public:
	TYPEINFO();

	SdXMLGenericPageContext( SvXMLImport& rImport, USHORT nPrfx, const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
		::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& rShapes);
	virtual ~SdXMLGenericPageContext();

	virtual void StartElement( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
	virtual SvXMLImportContext *CreateChildContext( 
		USHORT nPrefix, const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList );
	virtual void EndElement();

	const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& GetLocalShapesContext() const
		{ return mxShapes; }
	::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& GetLocalShapesContext()
		{ return mxShapes; }
};


}//end of namespace binfilter
#endif	//  _XIMPGROUP_HXX
