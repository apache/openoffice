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



#ifndef _SC_XMLEXPORTITERATOR_HXX
#define _SC_XMLEXPORTITERATOR_HXX

#ifndef __SGI_STL_VECTOR
#include <vector>
#endif
#ifndef __SGI_STL_LIST
#include <list>
#endif

#ifndef _COM_SUN_STAR_SHEET_XSPREADSHEET_HPP_
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#endif

#ifndef _COM_SUN_STAR_TABLE_CELLADDRESS_HPP_
#include <com/sun/star/table/CellAddress.hpp>
#endif
#ifndef _COM_SUN_STAR_TABLE_CELLRANGEADDRESS_HPP_
#include <com/sun/star/table/CellRangeAddress.hpp>
#endif
#ifndef _COM_SUN_STAR_TABLE_XCELL_HPP_
#include <com/sun/star/table/XCell.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXT_HPP_
#include <com/sun/star/text/XText.hpp>
#endif
#ifndef _COM_SUN_STAR_SHEET_XSHEETANNOTATION_HPP_
#include <com/sun/star/sheet/XSheetAnnotation.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPE_HPP_
#include <com/sun/star/drawing/XShape.hpp>
#endif

#ifndef SC_SCGLOB_HXX
#include "global.hxx"
#endif
#ifndef SC_DETFUNC_HXX
#include "detfunc.hxx"
#endif
#ifndef SC_DETDATA_HXX
#include "detdata.hxx"
#endif
namespace binfilter {

class	ScHorizontalCellIterator;
struct	ScMyCell;
class	ScXMLExport;
class	ScFormatRangeStyles;

//==============================================================================

class ScMyIteratorBase
{
protected:
	virtual sal_Bool			GetFirstAddress( ::com::sun::star::table::CellAddress& rCellAddress ) = 0;

public:
								ScMyIteratorBase();
	virtual						~ScMyIteratorBase();

	virtual void				SetCellData( ScMyCell& rMyCell ) = 0;
	virtual void				Sort() = 0;

	virtual void				UpdateAddress( ::com::sun::star::table::CellAddress& rCellAddress );
};

//==============================================================================

struct ScMyShape
{
	ScAddress	aAddress;
	ScAddress	aEndAddress;
	::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape> xShape;
	sal_Int16	nLayerID;

	sal_Bool operator<(const ScMyShape& aShape) const;
};

typedef std::list<ScMyShape>	ScMyShapeList;

class ScMyShapesContainer : ScMyIteratorBase
{
private:
	ScMyShapeList				aShapeList;
protected:
	virtual sal_Bool			GetFirstAddress( ::com::sun::star::table::CellAddress& rCellAddress );
public:
								ScMyShapesContainer();
	virtual						~ScMyShapesContainer();

								ScMyIteratorBase::UpdateAddress;
	void						AddNewShape(const ScMyShape& aShape);
	sal_Bool					HasShapes() { return !aShapeList.empty(); }
	const ScMyShapeList*		GetShapes() { return &aShapeList; }
	virtual void				SetCellData( ScMyCell& rMyCell );
	virtual void				Sort();
};

//==============================================================================

struct ScMyMergedRange
{
	::com::sun::star::table::CellRangeAddress aCellRange;
	sal_Int32					nRows;
	sal_Bool			   		bIsFirst : 1;
	sal_Bool					operator<(const ScMyMergedRange& aRange) const;
};

typedef std::list<ScMyMergedRange>	ScMyMergedRangeList;

class ScMyMergedRangesContainer : ScMyIteratorBase
{
private:
	ScMyMergedRangeList			aRangeList;
protected:
	virtual sal_Bool			GetFirstAddress( ::com::sun::star::table::CellAddress& rCellAddress );
public:
								ScMyMergedRangesContainer();
	virtual						~ScMyMergedRangesContainer();
	void						AddRange(const ::com::sun::star::table::CellRangeAddress aMergedRange);

								ScMyIteratorBase::UpdateAddress;
	virtual void				SetCellData( ScMyCell& rMyCell );
	virtual void				Sort();	// + remove doublets
};

//==============================================================================

struct ScMyAreaLink
{
	::rtl::OUString				sFilter;
	::rtl::OUString				sFilterOptions;
	::rtl::OUString				sURL;
	::rtl::OUString				sSourceStr;
	::com::sun::star::table::CellRangeAddress aDestRange;
	sal_Int32					nRefresh;

	inline						ScMyAreaLink() : nRefresh( 0 ) {}

	inline sal_Int32			GetColCount() const	{ return aDestRange.EndColumn - aDestRange.StartColumn + 1; }
	inline sal_Int32			GetRowCount() const	{ return aDestRange.EndRow - aDestRange.StartRow + 1; }

	sal_Bool					Compare( const ScMyAreaLink& rAreaLink ) const;
	sal_Bool 					operator<(const ScMyAreaLink& rAreaLink ) const;
};

typedef ::std::list< ScMyAreaLink > ScMyAreaLinkList;

class ScMyAreaLinksContainer : ScMyIteratorBase
{
private:
	ScMyAreaLinkList			aAreaLinkList;
protected:
	virtual sal_Bool			GetFirstAddress( ::com::sun::star::table::CellAddress& rCellAddress );
public:
								ScMyAreaLinksContainer();
	virtual						~ScMyAreaLinksContainer();

	inline void					AddNewAreaLink( const ScMyAreaLink& rAreaLink )
									{ aAreaLinkList.push_back( rAreaLink ); }

								ScMyIteratorBase::UpdateAddress;
	virtual void				SetCellData( ScMyCell& rMyCell );
	virtual void				Sort();
};

//==============================================================================

struct ScMyCellRangeAddress : ::com::sun::star::table::CellRangeAddress
{
	ScMyCellRangeAddress(const ::com::sun::star::table::CellRangeAddress& rRange);
	sal_Bool 					operator<(const ScMyCellRangeAddress& rCellRangeAddress ) const;
};

typedef std::list<ScMyCellRangeAddress> ScMyEmptyDatabaseRangeList;

class ScMyEmptyDatabaseRangesContainer : ScMyIteratorBase
{
private:
	ScMyEmptyDatabaseRangeList	aDatabaseList;
protected:
	virtual sal_Bool			GetFirstAddress( ::com::sun::star::table::CellAddress& rCellAddress );
public:
								ScMyEmptyDatabaseRangesContainer();
	virtual						~ScMyEmptyDatabaseRangesContainer();
	void						AddNewEmptyDatabaseRange(const ::com::sun::star::table::CellRangeAddress& aCellRangeAddress);

								ScMyIteratorBase::UpdateAddress;
	virtual void				SetCellData( ScMyCell& rMyCell );
	virtual void				Sort();
};

//==============================================================================

struct ScMyDetectiveObj
{
	::com::sun::star::table::CellAddress		aPosition;
	::com::sun::star::table::CellRangeAddress	aSourceRange;
	ScDetectiveObjType							eObjType;
	sal_Bool									bHasError : 1;
	sal_Bool operator<(const ScMyDetectiveObj& rDetObj) const;
};

typedef ::std::list< ScMyDetectiveObj > ScMyDetectiveObjList;
typedef ::std::vector< ScMyDetectiveObj > ScMyDetectiveObjVec;

class ScMyDetectiveObjContainer : ScMyIteratorBase
{
private:
	ScMyDetectiveObjList			aDetectiveObjList;
protected:
	virtual sal_Bool			GetFirstAddress( ::com::sun::star::table::CellAddress& rCellAddress );
public:
								ScMyDetectiveObjContainer();
	virtual						~ScMyDetectiveObjContainer();

	void						AddObject(
									ScDetectiveObjType eObjType,
									const ScAddress& rPosition,
									const ScRange& rSourceRange,
									sal_Bool bHasError );

								ScMyIteratorBase::UpdateAddress;
	virtual void				SetCellData( ScMyCell& rMyCell );
	virtual void				Sort();
};

//==============================================================================

struct ScMyDetectiveOp
{
	::com::sun::star::table::CellAddress	aPosition;
	ScDetOpType								eOpType;
	sal_Int32								nIndex;
	sal_Bool operator<(const ScMyDetectiveOp& rDetOp) const;
};

typedef ::std::list< ScMyDetectiveOp > ScMyDetectiveOpList;
typedef ::std::vector< ScMyDetectiveOp > ScMyDetectiveOpVec;

class ScMyDetectiveOpContainer : ScMyIteratorBase
{
private:
	ScMyDetectiveOpList			aDetectiveOpList;
protected:
	virtual sal_Bool			GetFirstAddress( ::com::sun::star::table::CellAddress& rCellAddress );
public:
								ScMyDetectiveOpContainer();
	virtual						~ScMyDetectiveOpContainer();

	void						AddOperation( ScDetOpType eOpType, const ScAddress& rPosition, sal_uInt32 nIndex );

								ScMyIteratorBase::UpdateAddress;
	virtual void				SetCellData( ScMyCell& rMyCell );
	virtual void				Sort();
};

//==============================================================================

// contains data to export for the current cell position
struct ScMyCell
{
	::com::sun::star::uno::Reference< ::com::sun::star::table::XCell> xCell;
	::com::sun::star::uno::Reference< ::com::sun::star::text::XText> xText;
	::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetAnnotation> xAnnotation;
	::com::sun::star::table::CellAddress		aCellAddress;
	::com::sun::star::table::CellRangeAddress	aMergeRange;
	::com::sun::star::table::CellRangeAddress	aMatrixRange;

	::rtl::OUString				sStringValue;
	::rtl::OUString				sAnnotationText;

	ScMyAreaLink				aAreaLink;
	ScMyShapeList				aShapeList;
	ScMyDetectiveObjVec			aDetectiveObjVec;
	ScMyDetectiveOpVec			aDetectiveOpVec;

	double						fValue;
	sal_Int32					nValidationIndex;
	sal_Int32					nStyleIndex;
	sal_Int32					nNumberFormat;
	::com::sun::star::table::CellContentType	nType;

	sal_Bool					bIsAutoStyle : 1;

	sal_Bool					bHasShape : 1;
	sal_Bool					bIsMergedBase : 1;
	sal_Bool					bIsCovered : 1;
	sal_Bool					bHasAreaLink : 1;
	sal_Bool					bHasEmptyDatabase : 1;
	sal_Bool					bHasDetectiveObj : 1;
	sal_Bool					bHasDetectiveOp : 1;

	sal_Bool					bIsEditCell : 1;
	sal_Bool					bKnowWhetherIsEditCell : 1;
	sal_Bool					bHasStringValue : 1;
	sal_Bool					bHasDoubleValue : 1;
	sal_Bool					bHasXText : 1;

	sal_Bool					bIsMatrixBase : 1;
	sal_Bool					bIsMatrixCovered : 1;
	sal_Bool					bHasAnnotation : 1;

								ScMyCell();
								~ScMyCell();
};

//==============================================================================

struct ScMyExportAnnotation
{
	::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetAnnotation> xAnnotation;
	::com::sun::star::table::CellAddress		aCellAddress;
	sal_Bool operator<(const ScMyExportAnnotation& rAnno) const;
};

typedef ::std::list< ScMyExportAnnotation > ScMyExportAnnotationList;

class ScMyNotEmptyCellsIterator
{
	::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet> xTable;
	::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange> xCellRange;
	::com::sun::star::table::CellAddress	aLastAddress;
	ScMyExportAnnotationList			aAnnotations;

	ScMyShapesContainer*				pShapes;
	ScMyEmptyDatabaseRangesContainer*	pEmptyDatabaseRanges;
	ScMyMergedRangesContainer*			pMergedRanges;
	ScMyAreaLinksContainer*				pAreaLinks;
	ScMyDetectiveObjContainer*			pDetectiveObj;
	ScMyDetectiveOpContainer*			pDetectiveOp;

	ScXMLExport&				rExport;
	ScHorizontalCellIterator*	pCellItr;

	sal_uInt16					nCellCol;
	sal_uInt16					nCellRow;
	sal_Int16					nCurrentTable;

	void						UpdateAddress( ::com::sun::star::table::CellAddress& rAddress );
	void						SetCellData( ScMyCell& rMyCell, ::com::sun::star::table::CellAddress& rAddress );

	void						SetMatrixCellData( ScMyCell& rMyCell );
	void						HasAnnotation( ScMyCell& aCell );
public:
								ScMyNotEmptyCellsIterator(ScXMLExport& rExport);
								~ScMyNotEmptyCellsIterator();

	void						Clear();

	inline void					SetShapes(ScMyShapesContainer* pNewShapes)
									{ pShapes = pNewShapes; }
	inline void					SetEmptyDatabaseRanges(ScMyEmptyDatabaseRangesContainer* pNewEmptyDatabaseRanges)
									{ pEmptyDatabaseRanges = pNewEmptyDatabaseRanges; }
	inline void					SetMergedRanges(ScMyMergedRangesContainer* pNewMergedRanges)
									{ pMergedRanges = pNewMergedRanges; }
	inline void					SetAreaLinks(ScMyAreaLinksContainer* pNewAreaLinks)
									{ pAreaLinks = pNewAreaLinks; }
	inline void					SetDetectiveObj(ScMyDetectiveObjContainer* pNewDetectiveObj)
									{ pDetectiveObj = pNewDetectiveObj; }
	inline void					SetDetectiveOp(ScMyDetectiveOpContainer* pNewDetectiveOp)
									{ pDetectiveOp = pNewDetectiveOp; }

	void						SetCurrentTable(const sal_Int32 nTable,
									::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet>& rxTable);

	sal_Bool					GetNext(ScMyCell& aCell, ScFormatRangeStyles* pCellStyles);
};

} //namespace binfilter
#endif

