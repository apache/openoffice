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


#ifndef _XMLOFF_SCH_XMLIMPORTHELPER_HXX_
#define _XMLOFF_SCH_XMLIMPORTHELPER_HXX_

#ifndef _UNIVERSALL_REFERENCE_HXX
#include <bf_xmloff/uniref.hxx>
#endif
#ifndef _XMLOFF_FAMILIES_HXX_
#include <bf_xmloff/families.hxx>
#endif
#ifndef _COM_SUN_STAR_UTIL_XSTRINGMAPPING_HPP_
#include <com/sun/star/util/XStringMapping.hpp>
#endif
#ifndef _COM_SUN_STAR_CHART_XCHARTDOCUMENT_HPP_
#include <com/sun/star/chart/XChartDocument.hpp>
#endif

namespace com { namespace sun { namespace star {
	namespace frame {
		class XModel;
	}
	namespace task {
		class XStatusIndicator;
	}
	namespace xml {
		namespace sax {
			class XAttributeList;
}}}}}
namespace binfilter {

class SvXMLUnitConverter;
class SvXMLStylesContext;
class XMLChartImportPropertyMapper;
class SvXMLTokenMap;
class SvXMLImportContext;
class SvXMLImport;

// ========================================

/** With this class you can import a <chart:chart> element containing
	its data as <table:table> element or without internal table. In
	the latter case you have to provide a table address mapper that
	converts table addresses in XML format to the appropriate application
	format.
 */
class SchXMLImportHelper : public UniRefBase
{
private:
	::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartDocument > mxChartDoc;
	::com::sun::star::uno::Reference< ::com::sun::star::util::XStringMapping > mxTableAddressMapper;
	SvXMLStylesContext* mpAutoStyles;

	SvXMLTokenMap* mpDocElemTokenMap;
	SvXMLTokenMap* mpTableElemTokenMap;
	SvXMLTokenMap* mpChartElemTokenMap;
	SvXMLTokenMap* mpPlotAreaElemTokenMap;
	SvXMLTokenMap* mpSeriesElemTokenMap;

	SvXMLTokenMap* mpChartAttrTokenMap;
	SvXMLTokenMap* mpPlotAreaAttrTokenMap;
	SvXMLTokenMap* mpAxisAttrTokenMap;
	SvXMLTokenMap* mpLegendAttrTokenMap;
	SvXMLTokenMap* mpAutoStyleAttrTokenMap;
	SvXMLTokenMap* mpCellAttrTokenMap;
	SvXMLTokenMap* mpSeriesAttrTokenMap;

public:

	SchXMLImportHelper();
	~SchXMLImportHelper();

	/** get the context for reading the <chart:chart> element with subelements.
		The result is stored in the XModel given if it also implements
		XChartDocument
	 */
	SvXMLImportContext* CreateChartContext(
		SvXMLImport& rImport,
		sal_uInt16 nPrefix, const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference<
			::com::sun::star::frame::XModel > xChartModel,
		const ::com::sun::star::uno::Reference<
			::com::sun::star::xml::sax::XAttributeList >& xAttrList );

	/** set the auto-style context that will be used to retrieve auto-styles
		used inside the following <chart:chart> element to parse
	 */
	void SetAutoStylesContext( SvXMLStylesContext* pAutoStyles ) { mpAutoStyles = pAutoStyles; }
	SvXMLStylesContext* GetAutoStylesContext() const { return mpAutoStyles; }

	/** set the string mapper that is used to convert cell addresses in
		XML format into the application format

		If the mapper is set it is automatically used for conversion
	  */
	void SetTableAddressMapper( ::com::sun::star::uno::Reference<
									::com::sun::star::util::XStringMapping >& xMapper )
		{ mxTableAddressMapper = xMapper; }

	const ::com::sun::star::uno::Reference<
		::com::sun::star::util::XStringMapping >& GetTableAddressMapper()
		{ return mxTableAddressMapper; }

	const ::com::sun::star::uno::Reference<
		::com::sun::star::chart::XChartDocument >& GetChartDocument()
		{ return mxChartDoc; }

	const SvXMLTokenMap& GetDocElemTokenMap();
	const SvXMLTokenMap& GetTableElemTokenMap();
	const SvXMLTokenMap& GetChartElemTokenMap();
	const SvXMLTokenMap& GetPlotAreaElemTokenMap();
	const SvXMLTokenMap& GetSeriesElemTokenMap();

	const SvXMLTokenMap& GetChartAttrTokenMap();
	const SvXMLTokenMap& GetPlotAreaAttrTokenMap();
	const SvXMLTokenMap& GetAxisAttrTokenMap();
	const SvXMLTokenMap& GetLegendAttrTokenMap();
	const SvXMLTokenMap& GetCellAttrTokenMap();
	const SvXMLTokenMap& GetSeriesAttrTokenMap();

	static sal_uInt16 GetChartFamilyID() { return XML_STYLE_FAMILY_SCH_CHART_ID; }

	void		ResizeChartData( sal_Int32 nSeries, sal_Int32 nDataPoints = -1 );
};

}//end of namespace binfilter
#endif	// _XMLOFF_SCH_XMLIMPORTHELPER_HXX_
