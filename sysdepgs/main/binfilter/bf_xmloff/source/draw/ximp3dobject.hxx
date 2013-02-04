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



#ifndef _XIMP3DOBJECT_HXX
#define _XIMP3DOBJECT_HXX

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

#ifndef _COM_SUN_STAR_DRAWING_HOMOGENMATRIX_HPP_
#include <com/sun/star/drawing/HomogenMatrix.hpp>
#endif

#ifndef _SVX_VECTOR3D_HXX
#include <bf_goodies/vector3d.hxx>
#endif

#ifndef _COM_SUN_STAR_DRAWING_DIRECTION3D_HPP_
#include <com/sun/star/drawing/Direction3D.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_POSITION3D_HPP_
#include <com/sun/star/drawing/Position3D.hpp>
#endif

#ifndef _XIMPSHAPE_HXX
#include "ximpshap.hxx"
#endif
namespace binfilter {

//////////////////////////////////////////////////////////////////////////////
// common shape context

class SdXML3DObjectContext : public SdXMLShapeContext
{
protected:
	// the shape group this object should be created inside

	::com::sun::star::drawing::HomogenMatrix mxHomMat;
	BOOL						mbSetTransform;

/*
	void SetStyle();
	void AddShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape);
*/
public:
	TYPEINFO();

	SdXML3DObjectContext( SvXMLImport& rImport, 
		sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName, 
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
		::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& rShapes);
	virtual ~SdXML3DObjectContext();

	virtual void StartElement(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList);
	virtual void EndElement();
};

//////////////////////////////////////////////////////////////////////////////
// dr3d:3dcube context

class SdXML3DCubeObjectShapeContext : public SdXML3DObjectContext
{
	Vector3D				maMinEdge;
	Vector3D				maMaxEdge;
	BOOL					mbMinEdgeUsed;
	BOOL					mbMaxEdgeUsed;

public:
	TYPEINFO();

	SdXML3DCubeObjectShapeContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName, 
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
		::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& rShapes);
	virtual ~SdXML3DCubeObjectShapeContext();

	virtual void StartElement(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList);
	virtual void EndElement();
};

//////////////////////////////////////////////////////////////////////////////
// dr3d:3dsphere context

class SdXML3DSphereObjectShapeContext : public SdXML3DObjectContext
{
	Vector3D				maCenter;
	Vector3D				maSize;
	BOOL					mbCenterUsed;
	BOOL					mbSizeUsed;

public:
	TYPEINFO();

	SdXML3DSphereObjectShapeContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName, 
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
		::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& rShapes);
	virtual ~SdXML3DSphereObjectShapeContext();

	virtual void StartElement(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList);
	virtual void EndElement();
};

//////////////////////////////////////////////////////////////////////////////
// polygonbased context

class SdXML3DPolygonBasedShapeContext : public SdXML3DObjectContext
{
	::rtl::OUString				maPoints;
	::rtl::OUString				maViewBox;

public:
	TYPEINFO();

	SdXML3DPolygonBasedShapeContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName, 
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
		::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& rShapes);
	virtual ~SdXML3DPolygonBasedShapeContext();

	virtual void StartElement(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList);
	virtual void EndElement();
};

//////////////////////////////////////////////////////////////////////////////
// dr3d:3dlathe context

class SdXML3DLatheObjectShapeContext : public SdXML3DPolygonBasedShapeContext
{
public:
	TYPEINFO();

	SdXML3DLatheObjectShapeContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName, 
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
		::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& rShapes);
	virtual ~SdXML3DLatheObjectShapeContext();

	virtual void StartElement(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList);
	virtual void EndElement();
};

//////////////////////////////////////////////////////////////////////////////
// dr3d:3dextrude context

class SdXML3DExtrudeObjectShapeContext : public SdXML3DPolygonBasedShapeContext
{
public:
	TYPEINFO();

	SdXML3DExtrudeObjectShapeContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName, 
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
		::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >& rShapes);
	virtual ~SdXML3DExtrudeObjectShapeContext();

	virtual void StartElement(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList);
	virtual void EndElement();
};

}//end of namespace binfilter
#endif	//  _XIMP3DOBJECT_HXX
