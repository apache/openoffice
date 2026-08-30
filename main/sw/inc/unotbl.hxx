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


#ifndef _UNOTBL_HXX
#define _UNOTBL_HXX

#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/util/XSortable.hpp>
#include <com/sun/star/chart/XChartData.hpp>
#include <com/sun/star/chart/XChartDataArray.hpp>
#include <com/sun/star/chart2/data/XLabeledDataSequence.hpp>
#include <com/sun/star/text/XTextTableCursor.hpp>
#include <com/sun/star/text/XTextTable.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/sheet/XCellRangeData.hpp>
#include <com/sun/star/table/XAutoFormattable.hpp>

#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase4.hxx>
#include <cppuhelper/implbase5.hxx>
#include <cppuhelper/implbase7.hxx>
#include <cppuhelper/implbase10.hxx>

#include <comphelper/uno3.hxx>
#include <tools/string.hxx>

#include <calbck.hxx>
#include <TextCursorHelper.hxx>
#include <unoevtlstnr.hxx>
#include <unotext.hxx>


class SwUnoCrsr;
class SwTable;
class SwTableBox;
class SwTableLine;
class SwTableCursor;
class SwTableBoxFmt;
class SwChartDataProvider;
class SwFrmFmt;
class SwXCell;

void lcl_setString( SwXCell&, const rtl::OUString &rTxt, sal_Bool bKeepNumberFmt = sal_True );

// ---------------------------------------------------------------------------

class SwChartEventListenerContainer : public SwEventListenerContainer
{
	public:
        SwChartEventListenerContainer( ::com::sun::star::uno::XInterface* pxParentL) :
            SwEventListenerContainer(pxParentL){}
    	void ChartDataChanged();
};
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
typedef
cppu::WeakImplHelper4
<
	::com::sun::star::table::XCell,
	::com::sun::star::lang::XServiceInfo,
	::com::sun::star::beans::XPropertySet,
	::com::sun::star::container::XEnumerationAccess
>
SwXCellBaseClass;
class SwXCell : public SwXCellBaseClass,
	public SwXText,
	public SwClient
{
    friend void   lcl_setString( SwXCell &rCell, const rtl::OUString &rTxt,
                                 sal_Bool bKeepNumberFmt );
    friend double lcl_getValue( SwXCell &rCell );
    friend void   lcl_setValue( SwXCell &rCell, double nVal );


    const SfxItemPropertySet*   m_pPropSet;
    SwTableBox*                 pBox;       // only set in non-XML import
	const SwStartNode* 		pStartNode; // only set in XML import

    // table position where pBox was found last
    sal_uInt16              nFndPos;

protected:
	virtual const SwStartNode *GetStartNode() const;

    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::text::XTextCursor >
        CreateCursor();

    bool IsValid() const;

	virtual ~SwXCell();

	//SwClient
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);

public:
    SwXCell(SwFrmFmt* pTblFmt, SwTableBox* pBox, sal_uInt16 nPos=USHRT_MAX );
	SwXCell(SwFrmFmt* pTblFmt, const SwStartNode& rStartNode); // XML import interface


	TYPEINFO();

    static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();

	//XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier );

    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType );
    virtual void SAL_CALL acquire(  ) throw();
    virtual void SAL_CALL release(  ) throw();

	//XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

	//XCell
    virtual ::rtl::OUString SAL_CALL getFormula(  );
    virtual void SAL_CALL setFormula( const ::rtl::OUString& aFormula );
    virtual double SAL_CALL getValue(  );
    virtual void SAL_CALL setValue( double nValue );
    virtual ::com::sun::star::table::CellContentType SAL_CALL getType(  );
    virtual sal_Int32 SAL_CALL getError(  );

	//XText
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor >  SAL_CALL createTextCursor(void);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor >  SAL_CALL createTextCursorByRange(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & aTextPosition);
	virtual void SAL_CALL  setString(const rtl::OUString& aString);

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

	//XEnumerationAccess - frueher XParagraphEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration >  SAL_CALL createEnumeration(void);

	//XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
    virtual sal_Bool SAL_CALL hasElements(  );

    SwTableBox*   GetTblBox()const {return pBox;}
	static SwXCell*		CreateXCell(SwFrmFmt* pTblFmt, SwTableBox* pBox, SwTable *pTbl = 0 );
	SwTableBox* 	FindBox(SwTable* pTable, SwTableBox* pBox);

    SwFrmFmt* GetFrmFmt() const { return (SwFrmFmt*)GetRegisteredIn(); }
};
/* -----------------27.06.98 15:40-------------------
 *
 * --------------------------------------------------*/
class SwXTextTableRow : public cppu::WeakImplHelper2
<
	::com::sun::star::beans::XPropertySet,
    ::com::sun::star::lang::XServiceInfo
>,
	public SwClient
{
    const SfxItemPropertySet*   m_pPropSet;
	SwTableLine* 			pLine;

	SwFrmFmt* GetFrmFmt() const { return (SwFrmFmt*)GetRegisteredIn(); }
protected:
	virtual ~SwXTextTableRow();
	//SwClient
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);

public:
	SwXTextTableRow(SwFrmFmt* pFmt, SwTableLine* pLine);


	TYPEINFO();

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

	const SwTableLine* 	GetTblRow() const {return pLine;}
	static SwTableLine* FindLine(SwTable* pTable, SwTableLine* pLine);
};
/* -----------------20.07.98 13:03-------------------
 *
 * --------------------------------------------------*/
typedef cppu::WeakImplHelper3<
								::com::sun::star::text::XTextTableCursor,
								::com::sun::star::lang::XServiceInfo,
								::com::sun::star::beans::XPropertySet
							> SwXTextTableCursor_Base;
class SW_DLLPUBLIC SwXTextTableCursor : public SwXTextTableCursor_Base
	,public SwClient
	,public OTextCursorHelper
{
	SwDepend 				aCrsrDepend;
    const SfxItemPropertySet*   m_pPropSet;

	//	SwUnoCrsr* 		GetCrsr() const { return (SwUnoCrsr*)aCrsrDepend.GetRegisteredIn(); }

protected:
	virtual ~SwXTextTableCursor();
public:
	SwXTextTableCursor(SwFrmFmt* pFmt, SwTableBox* pBox);
	SwXTextTableCursor(SwFrmFmt& rTableFmt,
						const SwTableCursor* pTableSelection);


	DECLARE_XINTERFACE()

	//XTextTableCursor
    virtual ::rtl::OUString SAL_CALL getRangeName(  );
    virtual sal_Bool SAL_CALL gotoCellByName( const ::rtl::OUString& aCellName, sal_Bool bExpand );
    virtual sal_Bool SAL_CALL goLeft( sal_Int16 nCount, sal_Bool bExpand );
    virtual sal_Bool SAL_CALL goRight( sal_Int16 nCount, sal_Bool bExpand );
    virtual sal_Bool SAL_CALL goUp( sal_Int16 nCount, sal_Bool bExpand );
    virtual sal_Bool SAL_CALL goDown( sal_Int16 nCount, sal_Bool bExpand );
    virtual void SAL_CALL gotoStart( sal_Bool bExpand );
    virtual void SAL_CALL gotoEnd( sal_Bool bExpand );
    virtual sal_Bool SAL_CALL mergeRange(  );
    virtual sal_Bool SAL_CALL splitRange( sal_Int16 Count, sal_Bool Horizontal );

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

	//SwClient
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);

	// ITextCursorHelper
	virtual const SwPaM*		GetPaM() const;
	virtual	SwPaM*				GetPaM();
	virtual const SwDoc* 		GetDoc() const;
	virtual SwDoc* 				GetDoc();

	const SwUnoCrsr*			GetCrsr() const;
	SwUnoCrsr*					GetCrsr();
	SwFrmFmt* 		GetFrmFmt() const { return (SwFrmFmt*)GetRegisteredIn(); }
};

/*-----------------11.12.97 09:38-------------------

--------------------------------------------------*/
struct SwRangeDescriptor
{
    sal_Int32 nTop;
    sal_Int32 nLeft;
    sal_Int32 nBottom;
    sal_Int32 nRight;

    void Normalize();
};

class SwTableProperties_Impl;
class SwXTextTable : public cppu::WeakImplHelper10
<
	::com::sun::star::text::XTextTable,
	::com::sun::star::lang::XServiceInfo,
	::com::sun::star::table::XCellRange,
	::com::sun::star::chart::XChartDataArray,
	::com::sun::star::beans::XPropertySet,
	::com::sun::star::container::XNamed,
	::com::sun::star::table::XAutoFormattable,
	::com::sun::star::util::XSortable,
    ::com::sun::star::lang::XUnoTunnel,
    ::com::sun::star::sheet::XCellRangeData
>,
	public SwClient
{
	SwEventListenerContainer		aLstnrCntnr;
	SwChartEventListenerContainer	aChartLstnrCntnr;
    const SfxItemPropertySet*       m_pPropSet;

	// Descriptor-interface
	SwTableProperties_Impl*		pTableProps;
	String 						m_sTableName;
	sal_Bool 						bIsDescriptor;
	unsigned short 				nRows;
	unsigned short 				nColumns;


	sal_Bool						bFirstRowAsLabel 	:1;
	sal_Bool						bFirstColumnAsLabel	:1;
protected:
	virtual ~SwXTextTable();
public:
	SwXTextTable();
	SwXTextTable(SwFrmFmt& rFrmFmt);


	static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();

	//XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier );

	TYPEINFO();

	//XTextTable
    virtual void SAL_CALL initialize( sal_Int32 nRows, sal_Int32 nColumns );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XTableRows > SAL_CALL getRows(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XTableColumns > SAL_CALL getColumns(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCell > SAL_CALL getCellByName( const ::rtl::OUString& aCellName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getCellNames(  );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextTableCursor > SAL_CALL createCursorByCellName( const ::rtl::OUString& aCellName );

	//XTextContent
	virtual void SAL_CALL attach(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & xTextRange);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL getAnchor(  );

	//XComponent
	virtual void SAL_CALL dispose(void);
	virtual void SAL_CALL addEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener);
	virtual void SAL_CALL removeEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener);

	//XCellRange
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCell > SAL_CALL getCellByPosition( sal_Int32 nColumn, sal_Int32 nRow );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange > SAL_CALL getCellRangeByPosition( sal_Int32 nLeft, sal_Int32 nTop, sal_Int32 nRight, sal_Int32 nBottom );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange > SAL_CALL getCellRangeByName( const ::rtl::OUString& aRange );

	//XChartDataArray
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< double > > SAL_CALL getData(  );
    virtual void SAL_CALL setData( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< double > >& aData );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getRowDescriptions(  );
    virtual void SAL_CALL setRowDescriptions( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aRowDescriptions );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getColumnDescriptions(  );
    virtual void SAL_CALL setColumnDescriptions( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aColumnDescriptions );

	//XChartData
	virtual void SAL_CALL addChartDataChangeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartDataChangeEventListener >& aListener );
    virtual void SAL_CALL removeChartDataChangeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartDataChangeEventListener >& aListener );
    virtual double SAL_CALL getNotANumber(  );
    virtual sal_Bool SAL_CALL isNotANumber( double nNumber );

	//XSortable
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL createSortDescriptor(void);
	virtual void SAL_CALL sort(const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& xDescriptor);

	//XAutoFormattable
	virtual void SAL_CALL autoFormat(const rtl::OUString& aName);

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

	//XNamed
	virtual rtl::OUString SAL_CALL getName(void);
	virtual void SAL_CALL setName(const rtl::OUString& Name_);

    //XCellRangeData
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > > SAL_CALL getDataArray(  );
    virtual void SAL_CALL setDataArray( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > >& aArray );

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

    void attachToRange(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & xTextRange);

	sal_uInt16 			getRowCount(void);
	sal_uInt16 			getColumnCount(void);
	::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange >  	GetRangeByName(SwFrmFmt* pFmt, SwTable* pTable,
						const String& sTLName, const String& sBRName,
						SwRangeDescriptor& rDesc);

	//SwClient
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);

	SwFrmFmt* GetFrmFmt() const { return (SwFrmFmt*)GetRegisteredIn(); }
};

/* -----------------27.04.98 16:41-------------------
 *
 * --------------------------------------------------*/
class SwXCellRange : public cppu::WeakImplHelper7
<
	::com::sun::star::table::XCellRange,
	::com::sun::star::lang::XServiceInfo,
	::com::sun::star::lang::XUnoTunnel,
	::com::sun::star::beans::XPropertySet,
	::com::sun::star::chart::XChartDataArray,
    ::com::sun::star::util::XSortable,
    ::com::sun::star::sheet::XCellRangeData
>,
	public SwClient
{
	SwDepend						aCursorDepend; //the cursor is removed after the doc has been removed
	SwChartEventListenerContainer	aChartLstnrCntnr;

	SwRangeDescriptor 			aRgDesc;
    const SfxItemPropertySet*   m_pPropSet;

	SwUnoCrsr* 					pTblCrsr;

	sal_Bool						bFirstRowAsLabel 	:1;
	sal_Bool						bFirstColumnAsLabel	:1;

public:
	SwXCellRange(SwUnoCrsr* pCrsr, SwFrmFmt& rFrmFmt, SwRangeDescriptor& rDesc);
	~SwXCellRange();

	TYPEINFO();

	static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();

	//XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier );


	//XCellRange
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCell > SAL_CALL getCellByPosition( sal_Int32 nColumn, sal_Int32 nRow );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange > SAL_CALL getCellRangeByPosition( sal_Int32 nLeft, sal_Int32 nTop, sal_Int32 nRight, sal_Int32 nBottom );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange > SAL_CALL getCellRangeByName( const ::rtl::OUString& aRange );

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

	//XChartData
    virtual void SAL_CALL addChartDataChangeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartDataChangeEventListener >& aListener );
    virtual void SAL_CALL removeChartDataChangeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartDataChangeEventListener >& aListener );
    virtual double SAL_CALL getNotANumber(  );
    virtual sal_Bool SAL_CALL isNotANumber( double nNumber );

	//XChartDataArray
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< double > > SAL_CALL getData(  );
    virtual void SAL_CALL setData( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< double > >& aData );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getRowDescriptions(  );
    virtual void SAL_CALL setRowDescriptions( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aRowDescriptions );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getColumnDescriptions(  );
    virtual void SAL_CALL setColumnDescriptions( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aColumnDescriptions );

	//XSortable
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL createSortDescriptor(void);
	virtual void SAL_CALL sort(const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& xDescriptor);

    //XCellRangeData
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > > SAL_CALL getDataArray(  );
    virtual void SAL_CALL setDataArray( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > >& aArray );

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

	//SwClient
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);

	SwFrmFmt* 	GetFrmFmt() const { return (SwFrmFmt*)GetRegisteredIn(); }
	sal_uInt16 		getRowCount(void);
	sal_uInt16 		getColumnCount(void);

    const SwUnoCrsr* GetTblCrsr() const;

    // for SwChartDataSequence
    void GetDataSequence(
            ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > *pAnySeq,
            ::com::sun::star::uno::Sequence< ::rtl::OUString > *pTxtSeq,
            ::com::sun::star::uno::Sequence< double > *pDblSeq,
            sal_Bool bForceNumberResults = sal_False );

};
/* -----------------03.02.99 07:31-------------------
 *
 * --------------------------------------------------*/
class SwXTableRows : public cppu::WeakImplHelper2
<
	::com::sun::star::table::XTableRows,
	::com::sun::star::lang::XServiceInfo
>,
	public SwClient

{
	SwFrmFmt* GetFrmFmt() const { return (SwFrmFmt*)GetRegisteredIn(); }
protected:
	virtual ~SwXTableRows();
public:
	SwXTableRows(SwFrmFmt& rFrmFmt);


	TYPEINFO();

// automatisch auskommentiert - [getIdlClass or queryInterface] - Bitte XTypeProvider benutzen!
//	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::reflection::XIdlClass >  > getIdlClasses(void) throw( ::com::sun::star::uno::RuntimeException );

// automatisch auskommentiert - [getIdlClass or queryInterface] - Bitte XTypeProvider benutzen!
//	virtual sal_Bool		queryInterface( ::com::sun::star::uno::Uik aUik, ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >  & rOut );

//	SMART_UNO_DECLARATION( SwXTableRows, UsrObject );

	//XIndexAccess
	virtual sal_Int32 SAL_CALL getCount(void);
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex(sal_Int32 nIndex);

	//XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
    virtual sal_Bool SAL_CALL hasElements(  );

	//XTableRows
	virtual void SAL_CALL insertByIndex(sal_Int32 nIndex, sal_Int32 nCount);
	virtual void SAL_CALL removeByIndex(sal_Int32 nIndex, sal_Int32 nCount);

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

	//SwClient
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);
};

/* -----------------03.02.99 07:31-------------------
 *
 * --------------------------------------------------*/
class SwXTableColumns : public cppu::WeakImplHelper2
<
	::com::sun::star::table::XTableColumns,
	::com::sun::star::lang::XServiceInfo
>,
	public SwClient

{
	SwFrmFmt* GetFrmFmt() const { return (SwFrmFmt*)GetRegisteredIn(); }
protected:
	virtual ~SwXTableColumns();
public:
	SwXTableColumns(SwFrmFmt& rFrmFmt);


	TYPEINFO();
// automatisch auskommentiert - [getIdlClass or queryInterface] - Bitte XTypeProvider benutzen!
//	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::reflection::XIdlClass >  > getIdlClasses(void) throw( ::com::sun::star::uno::RuntimeException );

// automatisch auskommentiert - [getIdlClass or queryInterface] - Bitte XTypeProvider benutzen!
//	virtual sal_Bool		queryInterface( ::com::sun::star::uno::Uik aUik, ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >  & rOut );

//	SMART_UNO_DECLARATION( SwXTableColumns, UsrObject );

	//XIndexAccess
	virtual sal_Int32 SAL_CALL getCount(void);
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex(sal_Int32 nIndex);

	//XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
    virtual sal_Bool SAL_CALL hasElements(  );

	//XTableColumns
	virtual void SAL_CALL insertByIndex(sal_Int32 nIndex, sal_Int32 nCount);
	virtual void SAL_CALL removeByIndex(sal_Int32 nIndex, sal_Int32 nCount);

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

	//SwClient
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);
};

#endif
