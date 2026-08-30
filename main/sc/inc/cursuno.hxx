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



#ifndef SC_CURSUNO_HXX
#define SC_CURSUNO_HXX

#include "cellsuno.hxx"
#include <com/sun/star/table/XCellCursor.hpp>
#include <com/sun/star/sheet/XSheetCellCursor.hpp>
#include <com/sun/star/sheet/XUsedAreaCursor.hpp>

class ScCellCursorObj : public ScCellRangeObj,
						public com::sun::star::sheet::XSheetCellCursor,
						public com::sun::star::sheet::XUsedAreaCursor,
						public com::sun::star::table::XCellCursor
{
public:
							ScCellCursorObj(ScDocShell* pDocSh, const ScRange& rR);
	virtual					~ScCellCursorObj();

	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
								const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL	acquire() throw();
	virtual void SAL_CALL	release() throw();

							// XSheetCellCursor
	virtual void SAL_CALL	collapseToCurrentRegion();
	virtual void SAL_CALL	collapseToCurrentArray();
	virtual void SAL_CALL	collapseToMergedArea();
	virtual void SAL_CALL	expandToEntireColumns();
	virtual void SAL_CALL	expandToEntireRows();
	virtual void SAL_CALL	collapseToSize( sal_Int32 nColumns, sal_Int32 nRows );

							// XUsedAreaCursor
	virtual void SAL_CALL	gotoStartOfUsedArea( sal_Bool bExpand );
	virtual void SAL_CALL	gotoEndOfUsedArea( sal_Bool bExpand );

							// XCellCursor
	virtual void SAL_CALL	gotoStart();
	virtual void SAL_CALL	gotoEnd();
	virtual void SAL_CALL	gotoNext();
	virtual void SAL_CALL	gotoPrevious();
	virtual void SAL_CALL	gotoOffset( sal_Int32 nColumnOffset, sal_Int32 nRowOffset );

							// XSheetCellRange
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet > SAL_CALL
							getSpreadsheet();

							// XCellRange
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCell > SAL_CALL
							getCellByPosition( sal_Int32 nColumn, sal_Int32 nRow );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange > SAL_CALL
							getCellRangeByPosition( sal_Int32 nLeft, sal_Int32 nTop,
								sal_Int32 nRight, sal_Int32 nBottom );
    using ScCellRangeObj::getCellRangeByName;
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange > SAL_CALL
							getCellRangeByName( const ::rtl::OUString& aRange );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

							// XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();
	virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();
};


#endif
