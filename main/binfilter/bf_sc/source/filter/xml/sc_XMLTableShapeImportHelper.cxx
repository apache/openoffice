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



#ifndef _SC_XMLTABLESHAPEIMPORTHELPER_HXX
#include "XMLTableShapeImportHelper.hxx"
#endif

#ifndef SC_XMLIMPRT_HXX
#include "xmlimprt.hxx"
#endif
#ifndef _SC_XMLCONVERTER_HXX
#include "XMLConverter.hxx"
#endif
#ifndef SC_DRWLAYER_HXX
#include "drwlayer.hxx"
#endif

#ifndef _XMLOFF_NMSPMAP_HXX
#include <bf_xmloff/nmspmap.hxx>
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX
#include <bf_xmloff/xmlnmspe.hxx>
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include <bf_xmloff/xmluconv.hxx>
#endif

#ifndef _SVX_UNOSHAPE_HXX
#include <bf_svx/unoshape.hxx>
#endif

namespace binfilter {

#define SC_LAYERID "LayerID"

using namespace ::com::sun::star;
using namespace xmloff::token;

XMLTableShapeImportHelper::XMLTableShapeImportHelper(
		ScXMLImport& rImp, SvXMLImportPropertyMapper *pImpMapper ) :
	XMLShapeImportHelper(rImp, rImp.GetModel(), pImpMapper )
{
}

XMLTableShapeImportHelper::~XMLTableShapeImportHelper()
{
}

void XMLTableShapeImportHelper::SetLayer(uno::Reference<drawing::XShape>& rShape, sal_Int16 nLayerID, const ::rtl::OUString& sType) const
{
    if (sType.equals(::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.drawing.ControlShape"))))
		nLayerID = SC_LAYER_CONTROLS;
    if (nLayerID != -1)
    {
		uno::Reference< beans::XPropertySet > xShapeProp( rShape, uno::UNO_QUERY );
		if( xShapeProp.is() )
			xShapeProp->setPropertyValue(OUString( RTL_CONSTASCII_USTRINGPARAM( SC_LAYERID ) ), uno::makeAny(nLayerID) );
    }
}

void XMLTableShapeImportHelper::finishShape(
	uno::Reference< drawing::XShape >& rShape,
	const uno::Reference< xml::sax::XAttributeList >& xAttrList,
	uno::Reference< drawing::XShapes >& rShapes )
{
	XMLShapeImportHelper::finishShape( rShape, xAttrList, rShapes );
	static_cast<ScXMLImport&>(mrImporter).LockSolarMutex();
	if (rShapes == static_cast<ScXMLImport&>(mrImporter).GetTables().GetCurrentXShapes())
	{
		Rectangle* pRect = NULL;
		sal_Int32 nEndX(-1);
		sal_Int32 nEndY(-1);
		sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
		table::CellAddress aEndCell;
		::rtl::OUString* pRangeList = NULL;
        sal_Int16 nLayerID(-1);
		for( sal_Int16 i=0; i < nAttrCount; i++ )
		{
			const ::rtl::OUString& rAttrName = xAttrList->getNameByIndex( i );
			const ::rtl::OUString& rValue = xAttrList->getValueByIndex( i );

			::rtl::OUString aLocalName;
			sal_uInt16 nPrefix =
				static_cast<ScXMLImport&>(mrImporter).GetNamespaceMap().GetKeyByAttrName( rAttrName,
																&aLocalName );
			if(nPrefix == XML_NAMESPACE_TABLE)
			{
				if (IsXMLToken(aLocalName, XML_END_CELL_ADDRESS))
				{
					sal_Int32 nOffset(0);
					ScXMLConverter::GetAddressFromString(aEndCell, rValue, static_cast<ScXMLImport&>(mrImporter).GetDocument(), nOffset);
				}
				else if (IsXMLToken(aLocalName, XML_END_X))
					static_cast<ScXMLImport&>(mrImporter).GetMM100UnitConverter().convertMeasure(nEndX, rValue);
				else if (IsXMLToken(aLocalName, XML_END_Y))
					static_cast<ScXMLImport&>(mrImporter).GetMM100UnitConverter().convertMeasure(nEndY, rValue);
				else if (IsXMLToken(aLocalName, XML_TABLE_BACKGROUND))
					if (IsXMLToken(rValue, XML_TRUE))
						nLayerID = SC_LAYER_BACK;
			}
			else if(nPrefix == XML_NAMESPACE_DRAW)
			{
				if (IsXMLToken(aLocalName, XML_NOTIFY_ON_UPDATE_OF_RANGES))
					pRangeList = new ::rtl::OUString(rValue);
			}
		}
        SetLayer(rShape, nLayerID, rShape->getShapeType());

		if (!bOnTable)
		{
			static_cast<ScXMLImport&>(mrImporter).GetTables().AddShape(rShape,
				pRangeList, aStartCell, aEndCell, nEndX, nEndY);
			SvxShape* pShapeImp = SvxShape::getImplementation(rShape);
			if (pShapeImp)
			{
				SdrObject *pSdrObj = pShapeImp->GetSdrObject();
				if (pSdrObj)
					ScDrawLayer::SetAnchor(pSdrObj, SCA_CELL);
			}
		}
		else
		{
			SvxShape* pShapeImp = SvxShape::getImplementation(rShape);
			if (pShapeImp)
			{
				SdrObject *pSdrObj = pShapeImp->GetSdrObject();
				if (pSdrObj)
					ScDrawLayer::SetAnchor(pSdrObj, SCA_PAGE);
			}
		}
	}
    else //#99532# this are grouped shapes which should also get the layerid
    {
		sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
        sal_Int16 nLayerID(-1);
		for( sal_Int16 i=0; i < nAttrCount; i++ )
		{
			const ::rtl::OUString& rAttrName = xAttrList->getNameByIndex( i );
			const ::rtl::OUString& rValue = xAttrList->getValueByIndex( i );

			::rtl::OUString aLocalName;
			sal_uInt16 nPrefix =
				static_cast<ScXMLImport&>(mrImporter).GetNamespaceMap().GetKeyByAttrName( rAttrName,
																&aLocalName );
			if(nPrefix == XML_NAMESPACE_TABLE)
			{
				if (IsXMLToken(aLocalName, XML_TABLE_BACKGROUND))
					if (IsXMLToken(rValue, XML_TRUE))
						nLayerID = SC_LAYER_BACK;
			}
		}
        SetLayer(rShape, nLayerID, rShape->getShapeType());
    }
	static_cast<ScXMLImport&>(mrImporter).UnlockSolarMutex();
}
}
