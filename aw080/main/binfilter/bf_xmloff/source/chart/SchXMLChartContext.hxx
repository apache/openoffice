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


#ifndef _SCH_XMLCHARTCONTEXT_HXX_
#define _SCH_XMLCHARTCONTEXT_HXX_

#ifndef _XMLOFF_XMLICTXT_HXX
#include "xmlictxt.hxx"
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPE_HPP_
#include <com/sun/star/drawing/XShape.hpp>
#endif

#include "transporttypes.hxx"
namespace com { namespace sun { namespace star {
	namespace chart {
		class XChartDocument;
		class XDiagram;
		struct ChartSeriesAddress;
	}
	namespace util {
		class XStringMapping;
	}
	namespace xml { namespace sax {
		class XAttributeList;
    }}
    namespace drawing {
        class XShapes;
    }
}}}
namespace binfilter {

class SchXMLImport;
class SchXMLImportHelper;



// ----------------------------------------

class SchXMLChartContext : public SvXMLImportContext
{
private:
	SchXMLTable maTable;
	SchXMLImportHelper& mrImportHelper;

	::rtl::OUString maMainTitle, maSubTitle;
	::com::sun::star::awt::Point maMainTitlePos, maSubTitlePos, maLegendPos;
	sal_Bool mbHasOwnTable;
	sal_Bool mbHasLegend;

	::com::sun::star::uno::Sequence< ::com::sun::star::chart::ChartSeriesAddress > maSeriesAddresses;
	::rtl::OUString msCategoriesAddress;
    ::rtl::OUString msChartAddress;
    ::rtl::OUString msTableNumberList;

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes > mxDrawPage;
    ::rtl::OUString msColTrans;
    ::rtl::OUString msRowTrans;

    ::com::sun::star::uno::Sequence< sal_Int32 > GetNumberSequenceFromString( const ::rtl::OUString& rStr );

public:
	SchXMLChartContext( SchXMLImportHelper& rImpHelper,
						SvXMLImport& rImport, const ::rtl::OUString& rLocalName );
	virtual ~SchXMLChartContext();

	virtual void StartElement( const ::com::sun::star::uno::Reference<
							   	  ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
	virtual void EndElement();
	virtual SvXMLImportContext *CreateChildContext(
		USHORT nPrefix,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference<
			::com::sun::star::xml::sax::XAttributeList >& xAttrList );
			
private:
	/**	@descr	This method bundles some settings to the chart model and executes them with
			a locked controller.  This includes setting the draw page size and setting
			the chart type.
		@param	aChartSize	The size the draw page will be set to.
		@param	bDomainForDefaultDataNeeded	This flag indicates wether the chart's data set
			has to contain a domain value.
		@param	aServiceName The name of the service the diagram is initialized with.
		@param	bSetWitchData	Indicates wether the data set takes it's data series from
			rows or from columns.
	*/
	void	InitChart	( ::com::sun::star::awt::Size aChartSize,
						sal_Bool bDomainForDefaultDataNeeded,
						::rtl::OUString aServiceName,
						sal_Bool bSetSwitchData);
};

// ----------------------------------------

class SchXMLTitleContext : public SvXMLImportContext
{
private:
	SchXMLImportHelper& mrImportHelper;
	::rtl::OUString& mrTitle;
	::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > mxTitleShape;
	::rtl::OUString msAutoStyleName;
	::com::sun::star::awt::Point& mrPosition;

public:
	SchXMLTitleContext( SchXMLImportHelper& rImpHelper,
						SvXMLImport& rImport, const ::rtl::OUString& rLocalName,
						::rtl::OUString& rTitle,
						::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xTitleShape,
						::com::sun::star::awt::Point& rPosition );
	virtual ~SchXMLTitleContext();

	virtual void StartElement( const ::com::sun::star::uno::Reference<
							   ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
	virtual SvXMLImportContext *CreateChildContext(
		USHORT nPrefix,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
};

// ----------------------------------------

class SchXMLLegendContext : public SvXMLImportContext
{
private:
	SchXMLImportHelper& mrImportHelper;
	::com::sun::star::awt::Point& mrPosition;

public:
	SchXMLLegendContext( SchXMLImportHelper& rImpHelper,
						 SvXMLImport& rImport, const ::rtl::OUString& rLocalName,
						 ::com::sun::star::awt::Point& rPosition );
	virtual ~SchXMLLegendContext();

	virtual void StartElement( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
};

}//end of namespace binfilter
#endif	// _SCH_XMLCHARTCONTEXT_HXX_
