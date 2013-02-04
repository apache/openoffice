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


#ifndef _SCH_XMLTABLECONTEXT_HXX_
#define _SCH_XMLTABLECONTEXT_HXX_

#ifndef _XMLOFF_XMLICTXT_HXX
#include "xmlictxt.hxx"
#endif
#ifndef SCH_XMLIMPORT_HXX_
#include "SchXMLImport.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif

#include "transporttypes.hxx"

namespace com { namespace sun { namespace star {
	namespace frame {
		class XModel;
	}
	namespace xml { namespace sax {
		class XAttributeList;
	}}
	namespace chart {
		class XChartDocument;
		struct ChartSeriesAddress;
}}}}
namespace binfilter {


// ========================================

class SchXMLTableContext : public SvXMLImportContext
{
private:
	SchXMLImportHelper& mrImportHelper;
	SchXMLTable& mrTable;

public:
	SchXMLTableContext( SchXMLImportHelper& rImpHelper,
						SvXMLImport& rImport,
						const ::rtl::OUString& rLocalName,
						SchXMLTable& aTable );
	virtual ~SchXMLTableContext();

	virtual SvXMLImportContext* CreateChildContext(
		USHORT nPrefix,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
};

// ----------------------------------------

class SchXMLTableHelper
{
public:
	/// The data for the ChartDocument is applied linearly
	static void applyTableSimple(
		const SchXMLTable& rTable,
		::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartDocument > xChartDoc );
};

// ========================================

// ----------------------------------------
// classes for columns
// ----------------------------------------

/** With this context all column elements are parsed to
	determine the index of the column containing
	the row descriptions and probably get an estimate
	for the altogether number of columns
 */
class SchXMLTableColumnsContext : public SvXMLImportContext
{
private:
	SchXMLImportHelper& mrImportHelper;
	SchXMLTable& mrTable;

public:
	SchXMLTableColumnsContext( SchXMLImportHelper& rImpHelper,
							   SvXMLImport& rImport,
							   const ::rtl::OUString& rLocalName,
							   SchXMLTable& aTable );
	virtual ~SchXMLTableColumnsContext();

	virtual SvXMLImportContext* CreateChildContext(
		USHORT nPrefix,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
};

// ----------------------------------------

class SchXMLTableColumnContext : public SvXMLImportContext
{
private:
	SchXMLImportHelper& mrImportHelper;
	SchXMLTable& mrTable;

public:
	SchXMLTableColumnContext( SchXMLImportHelper& rImpHelper,
							  SvXMLImport& rImport,
							  const ::rtl::OUString& rLocalName,
							  SchXMLTable& aTable );
	virtual ~SchXMLTableColumnContext();
	virtual void StartElement( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
};

// ----------------------------------------
// classes for rows
// ----------------------------------------

class SchXMLTableRowsContext : public SvXMLImportContext
{
private:
	SchXMLImportHelper& mrImportHelper;
	SchXMLTable& mrTable;

public:
	SchXMLTableRowsContext( SchXMLImportHelper& rImpHelper,
							SvXMLImport& rImport,
							const ::rtl::OUString& rLocalName,
							SchXMLTable& aTable );
	virtual ~SchXMLTableRowsContext();

	virtual SvXMLImportContext* CreateChildContext(
		USHORT nPrefix,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
};

// ----------------------------------------

class SchXMLTableRowContext : public SvXMLImportContext
{
private:
	SchXMLImportHelper& mrImportHelper;
	SchXMLTable& mrTable;

public:
	SchXMLTableRowContext( SchXMLImportHelper& rImpHelper,
						   SvXMLImport& rImport,
						   const ::rtl::OUString& rLocalName,
						   SchXMLTable& aTable );
	virtual ~SchXMLTableRowContext();

	virtual SvXMLImportContext* CreateChildContext(
		USHORT nPrefix,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
};

// ----------------------------------------
// classes for cells and their content
// ----------------------------------------

class SchXMLTableCellContext : public SvXMLImportContext
{
private:
	SchXMLImportHelper& mrImportHelper;
	SchXMLTable& mrTable;
	::rtl::OUString maCellContent;
	sal_Bool mbReadPara;

public:
	SchXMLTableCellContext( SchXMLImportHelper& rImpHelper,
							SvXMLImport& rImport,
							const ::rtl::OUString& rLocalName,
							SchXMLTable& aTable );
	virtual ~SchXMLTableCellContext();

	virtual SvXMLImportContext* CreateChildContext(
		USHORT nPrefix,
		const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
	virtual void StartElement( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );
	virtual void EndElement();
};

}//end of namespace binfilter
#endif	// _SCH_XMLTABLECONTEXT_HXX_
