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


#ifndef SCH_XMLEXPORT_HXX_
#define SCH_XMLEXPORT_HXX_

#ifndef _XMLOFF_SCH_XMLEXPORTHELPER_HXX_
#include <bf_xmloff/SchXMLExportHelper.hxx>
#endif
#ifndef _SCH_XMLAUTOSTYLEPOOLP_HXX_
#include "SchXMLAutoStylePoolP.hxx"
#endif
#ifndef _XMLOFF_XMLEXP_HXX
#include <bf_xmloff/xmlexp.hxx>
#endif
#ifndef _UNIVERSALL_REFERENCE_HXX
#include <bf_xmloff/uniref.hxx>
#endif
#ifndef _XMLOFF_PROPERTYSETMAPPER_HXX
#include <bf_xmloff/xmlprmap.hxx>
#endif
#ifndef _XMLOFF_PROPERTYHANDLERFACTORY_HXX
#include <bf_xmloff/prhdlfac.hxx>
#endif

namespace com { namespace sun { namespace star {
	namespace chart {
		class XDiagram;
		class XChartDocument;
		class XChartDataArray;
		struct ChartSeriesAddress;
	}
	namespace drawing {
		class XShape;
	}
	namespace task {
		class XStatusIndicator;
	}
}}}
namespace binfilter {

class SvXMLAutoStylePoolP;
class SvXMLUnitConverter;
class XMLChartExportPropertyMapper;

// ------------------------------------------
// export class for a complete chart document
// ------------------------------------------

class SchXMLExport : public SvXMLExport
{
private:
	::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator > mxStatusIndicator;
	SchXMLAutoStylePoolP maAutoStylePool;

	SchXMLExportHelper maExportHelper;

protected:
	virtual void _ExportStyles( sal_Bool bUsed );
	virtual void _ExportAutoStyles();
	virtual void _ExportMasterStyles();
	virtual void _ExportContent();

public:
	// #110680#
	SchXMLExport( 
		const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
		sal_uInt16 nExportFlags = EXPORT_ALL );
	virtual ~SchXMLExport();

	UniReference< XMLPropertySetMapper > GetPropertySetMapper() const { return maExportHelper.GetPropertySetMapper(); }

    // XServiceInfo ( : SvXMLExport )
    virtual ::rtl::OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException );
};

}//end of namespace binfilter
#endif	// SCH_XMLEXPORT_HXX_
