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



#ifndef _SVX_CELLCURSOR_HXX_
#define _SVX_CELLCURSOR_HXX_

#include <com/sun/star/table/XMergeableCellRange.hpp>
#include <com/sun/star/table/XCellCursor.hpp>
#include <cppuhelper/implbase2.hxx>
#include "cellrange.hxx"

// -----------------------------------------------------------------------------

namespace sdr { namespace table {

struct CellPos;

// -----------------------------------------------------------------------------
// CellCursor
// -----------------------------------------------------------------------------

typedef ::cppu::ImplInheritanceHelper2< CellRange, ::com::sun::star::table::XCellCursor, ::com::sun::star::table::XMergeableCellRange > CellCursorBase;

class CellCursor : public CellCursorBase
{
public:
	CellCursor( const TableModelRef& xTableModel, sal_Int32 nLeft, sal_Int32 nTop, sal_Int32 nRight, sal_Int32 nBottom );
	virtual ~CellCursor();

	// XCellRange
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCell > SAL_CALL getCellByPosition( sal_Int32 nColumn, sal_Int32 nRow );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange > SAL_CALL getCellRangeByPosition( sal_Int32 nLeft, sal_Int32 nTop, sal_Int32 nRight, sal_Int32 nBottom );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange > SAL_CALL getCellRangeByName( const ::rtl::OUString& aRange );

	// XCellCursor
    virtual void SAL_CALL gotoStart(  );
    virtual void SAL_CALL gotoEnd(  );
    virtual void SAL_CALL gotoNext(  );
    virtual void SAL_CALL gotoPrevious(  );
    virtual void SAL_CALL gotoOffset( ::sal_Int32 nColumnOffset, ::sal_Int32 nRowOffset );

    // XMergeableCellRange
    virtual void SAL_CALL merge(  );
    virtual void SAL_CALL split( ::sal_Int32 Columns, ::sal_Int32 Rows );
    virtual ::sal_Bool SAL_CALL isMergeable(  );
    virtual ::sal_Bool SAL_CALL isUnmergeable(  );

protected:
	bool GetMergedSelection( CellPos& rStart, CellPos& rEnd );

	void split_column( sal_Int32 nCol, sal_Int32 nColumns, std::vector< sal_Int32 >& rLeftOvers );
	void split_horizontal( sal_Int32 nColumns );
	void split_row( sal_Int32 nRow, sal_Int32 nRows, std::vector< sal_Int32 >& rLeftOvers );
	void split_vertical( sal_Int32 nRows );
};

} }

#endif
