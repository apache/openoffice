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


#ifndef _SCH_CELLRANGES_HXX_
#define _SCH_CELLRANGES_HXX_

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#include <vector>
namespace binfilter {

// Structures for CellRangeAddresses from container documents (Writer/Calc)
// ========================================================================

struct SchSingleCell
{
    sal_Int32 mnColumn, mnRow;
    sal_Bool  mbRelativeColumn : 1;     // if true the $-sign before the address is skipped in the XML format
    sal_Bool  mbRelativeRow    : 1;     // see above

    SchSingleCell() :
            mnColumn( -1 ),
            mnRow( -1 ),
            mbRelativeColumn( sal_False ),
            mbRelativeRow( sal_False )
        {}
};

struct SchCellAddress
{
    ::std::vector< SchSingleCell > maCells;      // to handle subtables (Writer)
};

struct SchCellRangeAddress
{
    SchCellAddress maUpperLeft;
    SchCellAddress maLowerRight;    // range is one cell if this vector is empty (or equal to aUpperLeft)
    ::rtl::OUString msTableName;
    sal_Int32 mnTableNumber;        // to reconstruct old address string in Calc

    SchCellRangeAddress() :
            mnTableNumber( -1 )
        {}
};

struct SchChartRange
{
    ::std::vector< SchCellRangeAddress > maRanges;    // a number of ranges describing the complete source data for the chart
    sal_Bool mbFirstColumnContainsLabels;
    sal_Bool mbFirstRowContainsLabels;
    sal_Bool mbKeepCopyOfData;                        // if this is true, the chart will export a table containing the current data

    SchChartRange() :
            mbFirstColumnContainsLabels( sal_False ),
            mbFirstRowContainsLabels( sal_False ),
            mbKeepCopyOfData( sal_True )
        {}
};

} //namespace binfilter
#endif	// _SCH_CELLRANGES_HXX_
