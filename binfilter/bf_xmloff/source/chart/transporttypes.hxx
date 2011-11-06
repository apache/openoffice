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


#ifndef SCH_XML_TRANSPORTTYPES_HXX_
#define SCH_XML_TRANSPORTTYPES_HXX_

#ifndef __SGI_STL_VECTOR
#include <vector>
#endif
namespace binfilter {

enum SchXMLCellType
{
	SCH_CELL_TYPE_UNKNOWN,
	SCH_CELL_TYPE_FLOAT,
	SCH_CELL_TYPE_STRING
};

struct SchXMLCell
{
	::rtl::OUString aString;
	double fValue;
	SchXMLCellType eType;

	SchXMLCell() : fValue( 0.0 ), eType( SCH_CELL_TYPE_UNKNOWN ) {}
};

struct SchXMLTable
{
	std::vector< ::std::vector< SchXMLCell > > aData;		/// an array of rows containing the table contents

	sal_Int32 nRowIndex;								/// reflects the index of the row currently parsed
	sal_Int32 nColumnIndex;								/// reflects the index of the column currently parsed
	sal_Int32 nMaxColumnIndex;							/// the greatest number of columns detected

	sal_Int32 nNumberOfColsEstimate;					/// parsing column-elements may yield an estimate

	SchXMLTable() : nRowIndex( -1 ),
					nColumnIndex( -1 ),
					nMaxColumnIndex( -1 ),
					nNumberOfColsEstimate( 0 ) {}
};

// ----------------------------------------

struct SchNumericCellRangeAddress
{
	sal_Int32 nRow1, nRow2;
	sal_Int32 nCol1, nCol2;

	SchNumericCellRangeAddress() :
			nRow1( -1 ), nRow2( -1 ),
			nCol1( -1 ), nCol2( -1 )
		{}

	SchNumericCellRangeAddress( const SchNumericCellRangeAddress& aOther )
		{
			nRow1 = aOther.nRow1; nRow2 = aOther.nRow2;
			nCol1 = aOther.nCol1; nCol2 = aOther.nCol2;
		}
};

// ----------------------------------------

enum SchXMLAxisClass
{
	SCH_XML_AXIS_CATEGORY,
	SCH_XML_AXIS_DOMAIN,
	SCH_XML_AXIS_VALUE,
	SCH_XML_AXIS_SERIES,
	SCH_XML_AXIS_UNDEF
};

struct SchXMLAxis
{
	enum SchXMLAxisClass eClass;
	sal_Int8 nIndexInCategory;
	::rtl::OUString aName;
	::rtl::OUString aTitle;
	::com::sun::star::awt::Point aPosition;

	SchXMLAxis() : eClass( SCH_XML_AXIS_UNDEF ), nIndexInCategory( 0 ) {}
};

}//end of namespace binfilter
#endif	// SCH_XML_TRANSPORTTYPES_HXX_
