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



#ifndef SC_CELLSUNO_HXX
#define SC_CELLSUNO_HXX

#include "global.hxx"			// ScRange, ScAddress
#include "rangelst.hxx"			// ScRangeList

#include "formula/grammar.hxx"
#include <svl/lstner.hxx>
#include <svl/listener.hxx>
#include <svl/itemprop.hxx>
#include <com/sun/star/table/XTableChartsSupplier.hpp>
#include <com/sun/star/chart/XChartDataArray.hpp>
#include <com/sun/star/text/XTextFieldsSupplier.hpp>
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <com/sun/star/sheet/XSheetCellRange.hpp>
#include <com/sun/star/sheet/XFormulaQuery.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#include <com/sun/star/sheet/XSheetOperation.hpp>
#include <com/sun/star/sheet/XSheetCellRangeContainer.hpp>
#include <com/sun/star/sheet/XCellFormatRangesSupplier.hpp>
#include <com/sun/star/sheet/XUniqueCellFormatRangesSupplier.hpp>
#include <com/sun/star/sheet/XCellRangesQuery.hpp>
#include <com/sun/star/sheet/XSheetFilterableEx.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XSheetPageBreak.hpp>
#include <com/sun/star/sheet/XCellRangeMovement.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/sheet/XPrintAreas.hpp>
#include <com/sun/star/sheet/XSheetLinkable.hpp>
#include <com/sun/star/sheet/XSubTotalCalculatable.hpp>
#include <com/sun/star/sheet/XArrayFormulaRange.hpp>
#include <com/sun/star/sheet/XCellRangeData.hpp>
#include <com/sun/star/sheet/XCellRangeFormula.hpp>
#include <com/sun/star/sheet/XCellSeries.hpp>
#include <com/sun/star/sheet/XMultipleOperation.hpp>
#include <com/sun/star/sheet/XFormulaTokens.hpp>
#include <com/sun/star/sheet/XArrayFormulaTokens.hpp>
#include <com/sun/star/sheet/XCellAddressable.hpp>
#include <com/sun/star/util/XReplaceable.hpp>
#include <com/sun/star/util/XIndent.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/util/XMergeable.hpp>
#include <com/sun/star/table/XAutoFormattable.hpp>
#include <com/sun/star/util/XSortable.hpp>
#include <com/sun/star/util/XImportable.hpp>
#include <com/sun/star/table/XColumnRowRange.hpp>
#include <com/sun/star/table/BorderLine.hpp>
#include <com/sun/star/table/TableBorder.hpp>
#include <com/sun/star/sheet/XDataPilotTablesSupplier.hpp>
#include <com/sun/star/sheet/XSheetAnnotationAnchor.hpp>
#include <com/sun/star/sheet/XScenariosSupplier.hpp>
#include <com/sun/star/sheet/XSheetAnnotationsSupplier.hpp>
#include <com/sun/star/sheet/XSheetAuditing.hpp>
#include <com/sun/star/sheet/XSheetOutline.hpp>
#include <com/sun/star/util/XProtectable.hpp>
#include <com/sun/star/sheet/XScenario.hpp>
#include <com/sun/star/sheet/XScenarioEnhanced.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/document/XActionLockable.hpp>
#include <com/sun/star/beans/XTolerantMultiPropertySet.hpp>
#include <com/sun/star/sheet/XExternalSheetName.hpp>
#include <com/sun/star/document/XEventsSupplier.hpp>
#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implbase3.hxx>

#ifndef __SGI_STL_VECTOR
#include <vector>
#endif

class ScDocShell;
class ScMarkData;
class ScMemChart;
class ScPrintRangeSaver;
class ScAttrRectIterator;
class ScCellRangeObj;
class SvxUnoText;
class ScLinkListener;
class ScPatternAttr;
class SvxBorderLine;
class SvxBoxItem;
class SvxBoxInfoItem;
class SvxItemPropertySet;

class ScLinkListener : public SvtListener
{
	Link	aLink;
public:
					ScLinkListener(const Link& rL) : aLink(rL) {}
	virtual			~ScLinkListener();
	virtual void	Notify( SvtBroadcaster& rBC, const SfxHint& rHint );
};

typedef ::com::sun::star::uno::Reference<
			::com::sun::star::util::XModifyListener >* XModifyListenerPtr;
SV_DECL_PTRARR_DEL( XModifyListenerArr_Impl, XModifyListenerPtr, 4, 4 )

class ScNamedEntry;
typedef ScNamedEntry* ScNamedEntryPtr;
SV_DECL_PTRARR_DEL( ScNamedEntryArr_Impl, ScNamedEntryPtr, 4, 4 )


//	ScCellRangesBase - Basisklasse fuer ScCellRangesObj (mit Index-Access)
//									und ScCellRangeObj (ohne Index-Access)

//	XServiceInfo ist in den Ableitungen implementiert

class ScHelperFunctions
{
public:
    static const SvxBorderLine* GetBorderLine( SvxBorderLine& rLine, const com::sun::star::table::BorderLine& rStruct );
    static void FillBoxItems( SvxBoxItem& rOuter, SvxBoxInfoItem& rInner, const com::sun::star::table::TableBorder& rBorder );
    static void FillBorderLine( com::sun::star::table::BorderLine& rStruct, const SvxBorderLine* pLine );
    static void FillTableBorder( com::sun::star::table::TableBorder& rBorder,
							const SvxBoxItem& rOuter, const SvxBoxInfoItem& rInner );
    static void ApplyBorder( ScDocShell* pDocShell, const ScRangeList& rRanges,
						const SvxBoxItem& rOuter, const SvxBoxInfoItem& rInner );
};

namespace ooo
{
    namespace vba {
        namespace excel {
	class ScVbaCellRangeAccess;  // Vba Helper class
    }
    }
}

class SC_DLLPUBLIC ScCellRangesBase : public com::sun::star::beans::XPropertySet,
						 public com::sun::star::beans::XMultiPropertySet,
						 public com::sun::star::beans::XPropertyState,
						 public com::sun::star::sheet::XSheetOperation,
						 public com::sun::star::chart::XChartDataArray,
						 public com::sun::star::util::XIndent,
						 public com::sun::star::sheet::XCellRangesQuery,
						 public com::sun::star::sheet::XFormulaQuery,
						 public com::sun::star::util::XReplaceable,
						 public com::sun::star::util::XModifyBroadcaster,
						 public com::sun::star::lang::XServiceInfo,
						 public com::sun::star::lang::XUnoTunnel,
						 public com::sun::star::lang::XTypeProvider,
                         public com::sun::star::beans::XTolerantMultiPropertySet,
						 public cppu::OWeakObject,
						 public SfxListener
{
	friend class ScTabViewObj;		// fuer select()
	friend class ScTableSheetObj;	// fuer createCursorByRange()
 	friend class ooo::vba::excel::ScVbaCellRangeAccess;

private:
	const SfxItemPropertySet* pPropSet;
	ScDocShell*				pDocShell;
	ScLinkListener*			pValueListener;
	ScPatternAttr*			pCurrentFlat;
	ScPatternAttr*			pCurrentDeep;
	SfxItemSet*				pCurrentDataSet;
	SfxItemSet*				pNoDfltCurrentDataSet;
	ScMarkData*				pMarkData;
	ScRangeList				aRanges;
    sal_Int64               nObjectId;
	sal_Bool					bChartColAsHdr;
	sal_Bool					bChartRowAsHdr;
	sal_Bool					bCursorOnly;
	sal_Bool					bGotDataChangedHint;
	XModifyListenerArr_Impl aValueListeners;

	DECL_LINK( ValueListenerHdl, SfxHint* );

private:
	void			PaintRanges_Impl( sal_uInt16 nPart );
	ScRangeListRef	GetLimitedChartRanges_Impl( long nDataColumns, long nDataRows ) const;
	void			ForceChartListener_Impl();
    ScMemChart*     CreateMemChart_Impl() const;

	const ScPatternAttr*	GetCurrentAttrsFlat();
	const ScPatternAttr*	GetCurrentAttrsDeep();
	SfxItemSet*				GetCurrentDataSet(bool bNoDflt = false);
    void                    ForgetMarkData();
	void					ForgetCurrentAttrs();

	com::sun::star::uno::Reference<com::sun::star::sheet::XSheetCellRanges>
							QueryDifferences_Impl(const com::sun::star::table::CellAddress& aCompare,
													sal_Bool bColumnDiff);
	com::sun::star::uno::Reference<com::sun::star::uno::XInterface>
							Find_Impl(const com::sun::star::uno::Reference<
										com::sun::star::util::XSearchDescriptor>& xDesc,
									const ScAddress* pLastPos);

protected:
    const ScMarkData*       GetMarkData();

	// GetItemPropertyMap for derived classes must contain all entries, including base class
	virtual const SfxItemPropertyMap* GetItemPropertyMap();
	virtual ::com::sun::star::beans::PropertyState GetOnePropertyState(
                                sal_uInt16 nItemWhich, const SfxItemPropertySimpleEntry* pEntry );
    virtual void            GetOnePropertyValue( const SfxItemPropertySimpleEntry* pEntry,
								::com::sun::star::uno::Any& );
    virtual void            SetOnePropertyValue( const SfxItemPropertySimpleEntry* pEntry,
												const ::com::sun::star::uno::Any& aValue );

public:
							ScCellRangesBase();		// fuer SMART_REFLECTION Krempel
							ScCellRangesBase(ScDocShell* pDocSh, const ScRange& rR);
							ScCellRangesBase(ScDocShell* pDocSh, const ScRangeList& rR);
	virtual					~ScCellRangesBase();

	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
								const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL	acquire() throw();
	virtual void SAL_CALL	release() throw();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );
	virtual void			RefChanged();

							// aus Ableitungen, aber auch per getImplementation
	ScDocShell*				GetDocShell() const		{ return pDocShell; }
	ScDocument*				GetDocument() const;
	const ScRangeList&		GetRangeList() const	{ return aRanges; }
	void					AddRange(const ScRange& rRange, const sal_Bool bMergeRanges);

							// per Service erzeugtes Objekt zum Leben erwecken:
	void					InitInsertRange(ScDocShell* pDocSh, const ScRange& rR);

	void					SetNewRange(const ScRange& rNew);	// fuer Cursor
	void					SetNewRanges(const ScRangeList& rNew);

	void					SetCursorOnly(sal_Bool bSet);
	sal_Bool					IsCursorOnly() const			{ return bCursorOnly; }

							// XSheetOperation
	virtual double SAL_CALL	computeFunction( ::com::sun::star::sheet::GeneralFunction nFunction );
	virtual void SAL_CALL	clearContents( sal_Int32 nContentFlags );

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo();
	virtual void SAL_CALL	setPropertyValue( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Any& aValue );
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue(
									const ::rtl::OUString& PropertyName );
	virtual void SAL_CALL	addPropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& xListener );
	virtual void SAL_CALL	removePropertyChangeListener( const ::rtl::OUString& aPropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XPropertyChangeListener >& aListener );
	virtual void SAL_CALL	addVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );
	virtual void SAL_CALL	removeVetoableChangeListener( const ::rtl::OUString& PropertyName,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::beans::XVetoableChangeListener >& aListener );

							// XMultiPropertySet
	virtual void SAL_CALL	setPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames,
									const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues );
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL
							getPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames );
	virtual void SAL_CALL addPropertiesChangeListener( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames,
									const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
	virtual void SAL_CALL removePropertiesChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
	virtual void SAL_CALL firePropertiesChangeEvent( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames,
									const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );

                            // XTolerantMultiPropertySet
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::SetPropertyTolerantFailed > SAL_CALL
        setPropertyValuesTolerant( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames,
                                    const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::GetPropertyTolerantResult > SAL_CALL
        getPropertyValuesTolerant( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::GetDirectPropertyTolerantResult > SAL_CALL
        getDirectPropertyValuesTolerant( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames );

							// XPropertyState
	virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState(
									const ::rtl::OUString& PropertyName );
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL
							getPropertyStates( const ::com::sun::star::uno::Sequence<
										::rtl::OUString >& aPropertyName );
	virtual void SAL_CALL	setPropertyToDefault( const ::rtl::OUString& PropertyName );
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault(
									const ::rtl::OUString& aPropertyName );

							// XIndent
	virtual void SAL_CALL	decrementIndent();
	virtual void SAL_CALL	incrementIndent();

							// XChartData
	virtual void SAL_CALL	addChartDataChangeEventListener(
								const ::com::sun::star::uno::Reference<
									::com::sun::star::chart::XChartDataChangeEventListener >& aListener );
	virtual void SAL_CALL	removeChartDataChangeEventListener( const ::com::sun::star::uno::Reference<
									::com::sun::star::chart::XChartDataChangeEventListener >& aListener );
	virtual double SAL_CALL	getNotANumber();
	virtual sal_Bool SAL_CALL isNotANumber( double nNumber );

							// XChartDataArray
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< double > > SAL_CALL
							getData();
	virtual void SAL_CALL	setData( const ::com::sun::star::uno::Sequence<
								::com::sun::star::uno::Sequence< double > >& aData );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getRowDescriptions();
	virtual void SAL_CALL setRowDescriptions( const ::com::sun::star::uno::Sequence<
								::rtl::OUString >& aRowDescriptions );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getColumnDescriptions();
	virtual void SAL_CALL	setColumnDescriptions( const ::com::sun::star::uno::Sequence<
								::rtl::OUString >& aColumnDescriptions );

							// XCellRangesQuery
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetCellRanges > SAL_CALL
							queryVisibleCells();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetCellRanges > SAL_CALL
							queryEmptyCells();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetCellRanges > SAL_CALL
							queryContentCells( sal_Int16 nContentFlags );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetCellRanges > SAL_CALL
							queryFormulaCells( sal_Int32 nResultFlags );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetCellRanges > SAL_CALL
							queryColumnDifferences(
								const ::com::sun::star::table::CellAddress& aCompare );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetCellRanges > SAL_CALL
							queryRowDifferences(
								const ::com::sun::star::table::CellAddress& aCompare );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetCellRanges > SAL_CALL
							queryIntersection(
								const ::com::sun::star::table::CellRangeAddress& aRange );

							// XFormulaQuery
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetCellRanges > SAL_CALL
							queryDependents( sal_Bool bRecursive );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetCellRanges > SAL_CALL
							queryPrecedents( sal_Bool bRecursive );

							// XSearchable
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XSearchDescriptor > SAL_CALL
							createSearchDescriptor();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > SAL_CALL
							findAll( const ::com::sun::star::uno::Reference<
								::com::sun::star::util::XSearchDescriptor >& xDesc );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
							findFirst( const ::com::sun::star::uno::Reference<
								::com::sun::star::util::XSearchDescriptor >& xDesc );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
							findNext( const ::com::sun::star::uno::Reference<
										::com::sun::star::uno::XInterface >& xStartAt,
									  const ::com::sun::star::uno::Reference<
										::com::sun::star::util::XSearchDescriptor >& xDesc );

							// XReplaceable
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XReplaceDescriptor > SAL_CALL
							createReplaceDescriptor();
	virtual sal_Int32 SAL_CALL replaceAll( const ::com::sun::star::uno::Reference<
								::com::sun::star::util::XSearchDescriptor >& xDesc );

							// XModifyBroadcaster
	virtual void SAL_CALL	addModifyListener( const ::com::sun::star::uno::Reference<
									::com::sun::star::util::XModifyListener >& aListener );
	virtual void SAL_CALL	removeModifyListener( const ::com::sun::star::uno::Reference<
									::com::sun::star::util::XModifyListener >& aListener );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

							// XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence<
									sal_Int8 >& aIdentifier );

	static const com::sun::star::uno::Sequence<sal_Int8>& getUnoTunnelId();
	static ScCellRangesBase* getImplementation( const com::sun::star::uno::Reference<
									com::sun::star::uno::XInterface> xObj );

							// XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();
	virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();
};


class SC_DLLPUBLIC ScCellRangesObj : public ScCellRangesBase,
						public com::sun::star::sheet::XSheetCellRangeContainer,
						public com::sun::star::container::XNameContainer,
						public com::sun::star::container::XEnumerationAccess
{
private:
	ScNamedEntryArr_Impl	aNamedEntries;

    ScCellRangeObj*         GetObjectByIndex_Impl(sal_Int32 nIndex) const;

public:
							ScCellRangesObj(ScDocShell* pDocSh, const ScRangeList& rR);
	virtual					~ScCellRangesObj();

	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
								const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL	acquire() throw();
	virtual void SAL_CALL	release() throw();

	virtual void			RefChanged();

							// XSheetCellRanges
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumerationAccess > SAL_CALL
							getCells();
	virtual ::rtl::OUString SAL_CALL getRangeAddressesAsString();
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::table::CellRangeAddress > SAL_CALL
							getRangeAddresses();

							// XSheetCellRangeContainer
	virtual void SAL_CALL	addRangeAddress( const ::com::sun::star::table::CellRangeAddress& rRange,
										sal_Bool bMergeRanges );
	virtual void SAL_CALL	removeRangeAddress( const ::com::sun::star::table::CellRangeAddress& rRange );
	virtual void SAL_CALL	addRangeAddresses( const ::com::sun::star::uno::Sequence<
										::com::sun::star::table::CellRangeAddress >& rRanges,
										sal_Bool bMergeRanges );
	virtual void SAL_CALL	removeRangeAddresses( const ::com::sun::star::uno::Sequence<
										::com::sun::star::table::CellRangeAddress >& rRanges );

							// XNameContainer
	virtual void SAL_CALL	insertByName( const ::rtl::OUString& aName,
								const ::com::sun::star::uno::Any& aElement );
	virtual void SAL_CALL	removeByName( const ::rtl::OUString& Name );

							// XNameReplace
	virtual void SAL_CALL	replaceByName( const ::rtl::OUString& aName,
								const ::com::sun::star::uno::Any& aElement );

							// XNameAccess
	virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
	virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

							// XIndexAccess
	virtual sal_Int32 SAL_CALL getCount();
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

							// XElementAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
	virtual sal_Bool SAL_CALL hasElements();

							// XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL
							createEnumeration();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

							// XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();
	virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();
};


class SC_DLLPUBLIC ScCellRangeObj : public ScCellRangesBase,
					   public com::sun::star::sheet::XCellRangeAddressable,
					   public com::sun::star::sheet::XSheetCellRange,
					   public com::sun::star::sheet::XArrayFormulaRange,
					   public com::sun::star::sheet::XArrayFormulaTokens,
					   public com::sun::star::sheet::XCellRangeData,
					   public com::sun::star::sheet::XCellRangeFormula,
					   public com::sun::star::sheet::XMultipleOperation,
					   public com::sun::star::util::XMergeable,
					   public com::sun::star::sheet::XCellSeries,
					   public com::sun::star::table::XAutoFormattable,
					   public com::sun::star::util::XSortable,
					   public com::sun::star::sheet::XSheetFilterableEx,
					   public com::sun::star::sheet::XSubTotalCalculatable,
					   public com::sun::star::util::XImportable,
					   public com::sun::star::sheet::XCellFormatRangesSupplier,
					   public com::sun::star::sheet::XUniqueCellFormatRangesSupplier,
					   public com::sun::star::table::XColumnRowRange
{
private:
	const SfxItemPropertySet*		pRangePropSet;
	ScRange					aRange;

protected:
	const ScRange&			GetRange() const	{ return aRange; }
	virtual const SfxItemPropertyMap* GetItemPropertyMap();
    virtual void GetOnePropertyValue( const SfxItemPropertySimpleEntry* pEntry,
								::com::sun::star::uno::Any& );
    virtual void            SetOnePropertyValue( const SfxItemPropertySimpleEntry* pEntry,
												const ::com::sun::star::uno::Any& aValue );

	::com::sun::star::uno::Reference< ::com::sun::star::table::XCell >
							GetCellByPosition_Impl( sal_Int32 nColumn, sal_Int32 nRow );

            void            SetArrayFormula_Impl( const rtl::OUString& rFormula,
                                const rtl::OUString& rFormulaNmsp,
                                const formula::FormulaGrammar::Grammar eGrammar );

public:
							ScCellRangeObj(ScDocShell* pDocSh, const ScRange& rR);
	virtual					~ScCellRangeObj();

							// uses ObjectShell from document, if set (returns NULL otherwise)
	static com::sun::star::uno::Reference<com::sun::star::table::XCellRange>
							CreateRangeFromDoc( ScDocument* pDoc, const ScRange& rR );

	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
								const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL	acquire() throw();
	virtual void SAL_CALL	release() throw();

	virtual void			RefChanged();

                            // via getImplementation()
    virtual void            SetArrayFormulaWithGrammar( const ::rtl::OUString& rFormula,
                                    const ::rtl::OUString& rFormulaNmsp,
                                    const formula::FormulaGrammar::Grammar );

	// XCellRange ist Basisklasse von XSheetCellRange und XSheetOperation
//	operator XCellRangeRef() const	{ return (XSheetCellRange*)this; }

							// XCellRangeAddressable
	virtual ::com::sun::star::table::CellRangeAddress SAL_CALL getRangeAddress();

							// XSheetCellRange
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSpreadsheet > SAL_CALL
							getSpreadsheet();

							// XArrayFormulaRange
	virtual ::rtl::OUString SAL_CALL getArrayFormula();
	virtual void SAL_CALL 	setArrayFormula( const ::rtl::OUString& aFormula );

                            // XArrayFormulaTokens
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::FormulaToken > SAL_CALL getArrayTokens();
    virtual void SAL_CALL   setArrayTokens( const ::com::sun::star::uno::Sequence<
                                    ::com::sun::star::sheet::FormulaToken >& aTokens );

							// XCellRangeData
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence<
							::com::sun::star::uno::Any > > SAL_CALL getDataArray();
	virtual void SAL_CALL	setDataArray( const ::com::sun::star::uno::Sequence<
								::com::sun::star::uno::Sequence<
									::com::sun::star::uno::Any > >& aArray );

							// XCellRangeFormula
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence<
							::rtl::OUString > > SAL_CALL getFormulaArray();
	virtual void SAL_CALL	setFormulaArray( const ::com::sun::star::uno::Sequence<
								::com::sun::star::uno::Sequence< ::rtl::OUString > >& aArray );

							// XMultipleOperation
	virtual void SAL_CALL	setTableOperation(
								const ::com::sun::star::table::CellRangeAddress& aFormulaRange,
								::com::sun::star::sheet::TableOperationMode nMode,
								const ::com::sun::star::table::CellAddress& aColumnCell,
								const ::com::sun::star::table::CellAddress& aRowCell );

							// XMergeable
	virtual void SAL_CALL	merge( sal_Bool bMerge );
	virtual sal_Bool SAL_CALL getIsMerged();

							// XCellSeries
	virtual void SAL_CALL	fillSeries( ::com::sun::star::sheet::FillDirection nFillDirection,
								::com::sun::star::sheet::FillMode nFillMode,
								::com::sun::star::sheet::FillDateMode nFillDateMode,
								double fStep, double fEndValue );
	virtual void SAL_CALL	fillAuto( ::com::sun::star::sheet::FillDirection nFillDirection,
								sal_Int32 nSourceCount );

							// XAutoFormattable
	virtual void SAL_CALL	autoFormat( const ::rtl::OUString& aName );

							// XSortable
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL
							createSortDescriptor();
	virtual void SAL_CALL	sort( const ::com::sun::star::uno::Sequence<
								::com::sun::star::beans::PropertyValue >& xDescriptor );

							// XSheetFilterableEx
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetFilterDescriptor > SAL_CALL
							createFilterDescriptorByObject( const ::com::sun::star::uno::Reference<
								::com::sun::star::sheet::XSheetFilterable >& xObject );

							// XSheetFilterable
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetFilterDescriptor > SAL_CALL
							createFilterDescriptor( sal_Bool bEmpty );
	virtual void SAL_CALL	filter( const ::com::sun::star::uno::Reference<
								::com::sun::star::sheet::XSheetFilterDescriptor >& xDescriptor );

							// XSubTotalCalculatable
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSubTotalDescriptor > SAL_CALL
							createSubTotalDescriptor( sal_Bool bEmpty );
	virtual void SAL_CALL	applySubTotals( const ::com::sun::star::uno::Reference<
								::com::sun::star::sheet::XSubTotalDescriptor >& xDescriptor,
								sal_Bool bReplace );
	virtual void SAL_CALL	removeSubTotals();

							// XImportable
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL
							createImportDescriptor( sal_Bool bEmpty );
	virtual void SAL_CALL	doImport( const ::com::sun::star::uno::Sequence<
								::com::sun::star::beans::PropertyValue >& aDescriptor );

							// XCellFormatRangesSupplier
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > SAL_CALL
							getCellFormatRanges();

							// XUniqueCellFormatRangesSupplier
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > SAL_CALL
							getUniqueCellFormatRanges();

							// XColumnRowRange
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XTableColumns > SAL_CALL
							getColumns();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XTableRows > SAL_CALL
							getRows();

							// XCellRange
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCell > SAL_CALL
							getCellByPosition( sal_Int32 nColumn, sal_Int32 nRow );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange > SAL_CALL
							getCellRangeByPosition( sal_Int32 nLeft, sal_Int32 nTop,
								sal_Int32 nRight, sal_Int32 nBottom );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange > SAL_CALL
							getCellRangeByName( const ::rtl::OUString& aRange );
	::com::sun::star::uno::Reference< ::com::sun::star::table::XCellRange >
							getCellRangeByName( const ::rtl::OUString& aRange,  const ScAddress::Details& rDetails );

							// XPropertySet ueberladen wegen Range-Properties
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

							// XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();
	virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();
};


//!	really derive cell from range?

class SC_DLLPUBLIC ScCellObj : public ScCellRangeObj,
				  public com::sun::star::text::XText,
				  public com::sun::star::container::XEnumerationAccess,
				  public com::sun::star::table::XCell,
                  public com::sun::star::sheet::XFormulaTokens,
				  public com::sun::star::sheet::XCellAddressable,
				  public com::sun::star::sheet::XSheetAnnotationAnchor,
				  public com::sun::star::text::XTextFieldsSupplier,
					public com::sun::star::document::XActionLockable
{
private:
	SvxUnoText*				pUnoText;
	const SfxItemPropertySet*	pCellPropSet;
	ScAddress				aCellPos;
	sal_Int16				nActionLockCount;

private:
	String		GetInputString_Impl(sal_Bool bEnglish) const;
	String		GetOutputString_Impl() const;
	void		SetString_Impl(const String& rString, sal_Bool bInterpret, sal_Bool bEnglish);
	double		GetValue_Impl() const;
	void		SetValue_Impl(double fValue);
	com::sun::star::table::CellContentType GetResultType_Impl();

protected:
	virtual const SfxItemPropertyMap* GetItemPropertyMap();
    virtual void GetOnePropertyValue( const SfxItemPropertySimpleEntry* pEntry,
								::com::sun::star::uno::Any& );
    virtual void            SetOnePropertyValue( const SfxItemPropertySimpleEntry* pEntry,
												const ::com::sun::star::uno::Any& aValue );

public:
    static const SvxItemPropertySet* GetEditPropertySet();
    static const SfxItemPropertyMap* GetCellPropertyMap();

							ScCellObj(ScDocShell* pDocSh, const ScAddress& rP);
	virtual					~ScCellObj();

	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
								const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL	acquire() throw();
	virtual void SAL_CALL	release() throw();

	virtual void			RefChanged();

	SvxUnoText&				GetUnoText();

							// XML import needs to set results at formula cells,
							// not meant for any other purpose.
	void					SetFormulaResultString( const ::rtl::OUString& rResult );
	void					SetFormulaResultDouble( double fResult );
    void                    SetFormulaWithGrammar( const ::rtl::OUString& rFormula,
                                const ::rtl::OUString& rFormulaNmsp, const formula::FormulaGrammar::Grammar );
	const ScAddress&		GetPosition() const { return aCellPos; }

    void                    InputEnglishString( const ::rtl::OUString& rText );

							// XText
	virtual void SAL_CALL	insertTextContent( const ::com::sun::star::uno::Reference<
									::com::sun::star::text::XTextRange >& xRange,
								const ::com::sun::star::uno::Reference<
									::com::sun::star::text::XTextContent >& xContent,
								sal_Bool bAbsorb );
	virtual void SAL_CALL	removeTextContent( const ::com::sun::star::uno::Reference<
								::com::sun::star::text::XTextContent >& xContent );

							// XSimpleText
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor > SAL_CALL
							createTextCursor();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor > SAL_CALL
							createTextCursorByRange( const ::com::sun::star::uno::Reference<
										::com::sun::star::text::XTextRange >& aTextPosition );
	virtual void SAL_CALL	insertString( const ::com::sun::star::uno::Reference<
										::com::sun::star::text::XTextRange >& xRange,
										const ::rtl::OUString& aString, sal_Bool bAbsorb );
	virtual void SAL_CALL	insertControlCharacter( const ::com::sun::star::uno::Reference<
										::com::sun::star::text::XTextRange >& xRange,
										sal_Int16 nControlCharacter, sal_Bool bAbsorb );

							// XTextRange
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XText > SAL_CALL
							getText();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL
							getStart();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL
							getEnd();
	virtual ::rtl::OUString SAL_CALL getString();
	virtual void SAL_CALL	setString( const ::rtl::OUString& aString );

							// XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL
							createEnumeration();

							// XElementAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
	virtual sal_Bool SAL_CALL hasElements();

							// XCell
	virtual ::rtl::OUString SAL_CALL getFormula();
	virtual void SAL_CALL	setFormula( const ::rtl::OUString& aFormula );
	virtual double SAL_CALL getValue();
	virtual void SAL_CALL	setValue( double nValue );
	virtual ::com::sun::star::table::CellContentType SAL_CALL getType();
	virtual sal_Int32 SAL_CALL getError();

                            // XFormulaTokens
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::FormulaToken > SAL_CALL getTokens();
    virtual void SAL_CALL   setTokens( const ::com::sun::star::uno::Sequence<
                                    ::com::sun::star::sheet::FormulaToken >& aTokens );

							// XCellAddressable
	virtual ::com::sun::star::table::CellAddress SAL_CALL getCellAddress();

							// XSheetAnnotationAnchor
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetAnnotation > SAL_CALL
							getAnnotation();

							// XTextFieldsSupplier
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumerationAccess > SAL_CALL
							getTextFields();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > SAL_CALL
							getTextFieldMasters();

							// XPropertySet ueberladen wegen Zell-Properties
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

							// XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();
	virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();

							// XActionLockable
	virtual sal_Bool SAL_CALL isActionLocked();
	virtual void SAL_CALL	addActionLock();
	virtual void SAL_CALL	removeActionLock();
	virtual void SAL_CALL	setActionLocks( sal_Int16 nLock );
	virtual sal_Int16 SAL_CALL resetActionLocks();

	static String		GetOutputString_Impl(ScDocument* pDoc, const ScAddress& aPos);
};


class ScTableSheetObj : public ScCellRangeObj,
						public com::sun::star::sheet::XSpreadsheet,
						public com::sun::star::container::XNamed,
						public com::sun::star::sheet::XSheetPageBreak,
						public com::sun::star::sheet::XCellRangeMovement,
						public com::sun::star::table::XTableChartsSupplier,
						public com::sun::star::sheet::XDataPilotTablesSupplier,
						public com::sun::star::sheet::XScenariosSupplier,
						public com::sun::star::sheet::XSheetAnnotationsSupplier,
						public com::sun::star::drawing::XDrawPageSupplier,
						public com::sun::star::sheet::XPrintAreas,
						public com::sun::star::sheet::XSheetLinkable,
						public com::sun::star::sheet::XSheetAuditing,
						public com::sun::star::sheet::XSheetOutline,
						public com::sun::star::util::XProtectable,
						public com::sun::star::sheet::XScenario,
						public com::sun::star::sheet::XScenarioEnhanced,
                        public com::sun::star::sheet::XExternalSheetName,
                        public com::sun::star::document::XEventsSupplier
{
	friend class ScTableSheetsObj;		// fuer insertByName()

private:
	const SfxItemPropertySet*		pSheetPropSet;

	SCTAB					GetTab_Impl() const;
	void					PrintAreaUndo_Impl( ScPrintRangeSaver* pOldRanges );

protected:
	virtual const SfxItemPropertyMap* GetItemPropertyMap();
    virtual void GetOnePropertyValue( const SfxItemPropertySimpleEntry* pEntry,
								::com::sun::star::uno::Any& );
    virtual void            SetOnePropertyValue( const SfxItemPropertySimpleEntry* pEntry,
												const ::com::sun::star::uno::Any& aValue );

public:
							ScTableSheetObj(ScDocShell* pDocSh, SCTAB nTab);
	virtual					~ScTableSheetObj();

	void					InitInsertSheet(ScDocShell* pDocSh, SCTAB nTab);

	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
								const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL	acquire() throw();
	virtual void SAL_CALL	release() throw();

							// XSpreadsheet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetCellCursor >
							SAL_CALL createCursor();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetCellCursor > SAL_CALL
							createCursorByRange( const ::com::sun::star::uno::Reference<
								::com::sun::star::sheet::XSheetCellRange >& aRange );

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

							// XNamed
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL	setName( const ::rtl::OUString& aName );

							// XSheetPageBreak
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::TablePageBreakData > SAL_CALL
							getColumnPageBreaks();
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::sheet::TablePageBreakData > SAL_CALL
							getRowPageBreaks();
	virtual void SAL_CALL	removeAllManualPageBreaks();

							// XCellRangeMovement
	virtual void SAL_CALL	insertCells( const ::com::sun::star::table::CellRangeAddress& aRange,
								::com::sun::star::sheet::CellInsertMode nMode );
	virtual void SAL_CALL	removeRange( const ::com::sun::star::table::CellRangeAddress& aRange,
								::com::sun::star::sheet::CellDeleteMode nMode );
	virtual void SAL_CALL	moveRange( const ::com::sun::star::table::CellAddress& aDestination,
								const ::com::sun::star::table::CellRangeAddress& aSource );
	virtual void SAL_CALL	copyRange( const ::com::sun::star::table::CellAddress& aDestination,
								const ::com::sun::star::table::CellRangeAddress& aSource );

							// XTableChartsSupplier
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::table::XTableCharts > SAL_CALL
							getCharts();

							// XDataPilotTablesSupplier
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XDataPilotTables > SAL_CALL
							getDataPilotTables();

							// XScenariosSupplier
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XScenarios > SAL_CALL
							getScenarios();

							// XSheetAnnotationsSupplier
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetAnnotations > SAL_CALL
							getAnnotations();

							// XDrawPageSupplier
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage > SAL_CALL
							getDrawPage();

							// XPrintAreas
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::table::CellRangeAddress > SAL_CALL
							getPrintAreas();
	virtual void SAL_CALL setPrintAreas( const ::com::sun::star::uno::Sequence<
								::com::sun::star::table::CellRangeAddress >& aPrintAreas );
	virtual sal_Bool SAL_CALL getPrintTitleColumns();
	virtual void SAL_CALL	setPrintTitleColumns( sal_Bool bPrintTitleColumns );
	virtual ::com::sun::star::table::CellRangeAddress SAL_CALL getTitleColumns();
	virtual void SAL_CALL	setTitleColumns(
								const ::com::sun::star::table::CellRangeAddress& aTitleColumns );
	virtual sal_Bool SAL_CALL getPrintTitleRows();
	virtual void SAL_CALL	setPrintTitleRows( sal_Bool bPrintTitleRows );
	virtual ::com::sun::star::table::CellRangeAddress SAL_CALL getTitleRows();
	virtual void SAL_CALL	setTitleRows(
								const ::com::sun::star::table::CellRangeAddress& aTitleRows );

							// XSheetLinkable
	virtual ::com::sun::star::sheet::SheetLinkMode SAL_CALL getLinkMode();
	virtual void SAL_CALL	setLinkMode( ::com::sun::star::sheet::SheetLinkMode nLinkMode );
	virtual ::rtl::OUString SAL_CALL getLinkUrl();
	virtual void SAL_CALL	setLinkUrl( const ::rtl::OUString& aLinkUrl );
	virtual ::rtl::OUString SAL_CALL getLinkSheetName();
	virtual void SAL_CALL	setLinkSheetName( const ::rtl::OUString& aLinkSheetName );
	virtual void SAL_CALL	link( const ::rtl::OUString& aUrl,
								const ::rtl::OUString& aSheetName,
								const ::rtl::OUString& aFilterName,
								const ::rtl::OUString& aFilterOptions,
								::com::sun::star::sheet::SheetLinkMode nMode );

							// XSheetAuditing
	virtual sal_Bool SAL_CALL hideDependents( const ::com::sun::star::table::CellAddress& aPosition );
	virtual sal_Bool SAL_CALL hidePrecedents( const ::com::sun::star::table::CellAddress& aPosition );
	virtual sal_Bool SAL_CALL showDependents( const ::com::sun::star::table::CellAddress& aPosition );
	virtual sal_Bool SAL_CALL showPrecedents( const ::com::sun::star::table::CellAddress& aPosition );
	virtual sal_Bool SAL_CALL showErrors( const ::com::sun::star::table::CellAddress& aPosition );
	virtual sal_Bool SAL_CALL showInvalid();
	virtual void SAL_CALL	clearArrows();

							// XSheetOutline
	virtual void SAL_CALL	group( const ::com::sun::star::table::CellRangeAddress& aRange,
								::com::sun::star::table::TableOrientation nOrientation );
	virtual void SAL_CALL	ungroup( const ::com::sun::star::table::CellRangeAddress& aRange,
								::com::sun::star::table::TableOrientation nOrientation );
	virtual void SAL_CALL	autoOutline( const ::com::sun::star::table::CellRangeAddress& aRange );
	virtual void SAL_CALL	clearOutline();
	virtual void SAL_CALL	hideDetail( const ::com::sun::star::table::CellRangeAddress& aRange );
	virtual void SAL_CALL	showDetail( const ::com::sun::star::table::CellRangeAddress& aRange );
	virtual void SAL_CALL	showLevel( sal_Int16 nLevel,
								::com::sun::star::table::TableOrientation nOrientation );

							// XProtectable
	virtual void SAL_CALL	protect( const ::rtl::OUString& aPassword );
	virtual void SAL_CALL	unprotect( const ::rtl::OUString& aPassword );
	virtual sal_Bool SAL_CALL isProtected();

							// XScenario
	virtual sal_Bool SAL_CALL getIsScenario();
	virtual ::rtl::OUString SAL_CALL getScenarioComment();
	virtual void SAL_CALL	setScenarioComment( const ::rtl::OUString& aScenarioComment );
	virtual void SAL_CALL	addRanges( const ::com::sun::star::uno::Sequence<
								::com::sun::star::table::CellRangeAddress >& aRanges );
	virtual void SAL_CALL	apply();
							// XScenarioEnhanced
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::table::CellRangeAddress > SAL_CALL
                            getRanges(  );

                            // XExternalSheetName
    virtual void SAL_CALL   setExternalName( const ::rtl::OUString& aUrl, const ::rtl::OUString& aSheetName );

                            // XEventsSupplier
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameReplace > SAL_CALL getEvents();

							// XPropertySet ueberladen wegen Sheet-Properties
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

							// XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence<
									sal_Int8 >& aIdentifier );

	static const com::sun::star::uno::Sequence<sal_Int8>& getUnoTunnelId();
	static ScTableSheetObj* getImplementation( const com::sun::star::uno::Reference<
									com::sun::star::uno::XInterface> xObj );

							// XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();
	virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();
};


class ScTableColumnObj : public ScCellRangeObj,
						 public com::sun::star::container::XNamed
{
private:
	const SfxItemPropertySet*		pColPropSet;

protected:
	virtual const SfxItemPropertyMap* GetItemPropertyMap();
    virtual void GetOnePropertyValue( const SfxItemPropertySimpleEntry* pEntry,
								::com::sun::star::uno::Any& );
    virtual void            SetOnePropertyValue( const SfxItemPropertySimpleEntry* pEntry,
												const ::com::sun::star::uno::Any& aValue );

public:
							ScTableColumnObj(ScDocShell* pDocSh, SCCOL nCol, SCTAB nTab);
	virtual					~ScTableColumnObj();

	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
								const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL	acquire() throw();
	virtual void SAL_CALL	release() throw();

							// XNamed
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL	setName( const ::rtl::OUString& aName );

							// XPropertySet ueberladen wegen Spalten-Properties
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

							// XTypeProvider
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();
	virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();
};


class ScTableRowObj : public ScCellRangeObj
{
private:
	const SfxItemPropertySet*		pRowPropSet;

protected:
	virtual const SfxItemPropertyMap* GetItemPropertyMap();
    virtual void GetOnePropertyValue( const SfxItemPropertySimpleEntry* pEntry,
								::com::sun::star::uno::Any& );
    virtual void            SetOnePropertyValue( const SfxItemPropertySimpleEntry* pEntry,
												const ::com::sun::star::uno::Any& aValue );

public:
							ScTableRowObj(ScDocShell* pDocSh, SCROW nRow, SCTAB nTab);
	virtual					~ScTableRowObj();

							// XPropertySet ueberladen wegen Zeilen-Properties
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class ScCellsObj : public cppu::WeakImplHelper2<
							com::sun::star::container::XEnumerationAccess,
							com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*				pDocShell;
	ScRangeList				aRanges;

public:
							ScCellsObj(ScDocShell* pDocSh, const ScRangeList& rR);
	virtual					~ScCellsObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL
							createEnumeration();

							// XElementAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
	virtual sal_Bool SAL_CALL hasElements();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class ScCellsEnumeration : public cppu::WeakImplHelper2<
								com::sun::star::container::XEnumeration,
								com::sun::star::lang::XServiceInfo >,
							public SfxListener
{
private:
	ScDocShell*				pDocShell;
	ScRangeList				aRanges;
	ScAddress				aPos;
	ScMarkData*				pMark;
	sal_Bool					bAtEnd;

private:
	void					Advance_Impl();
	void					CheckPos_Impl();

public:
							ScCellsEnumeration(ScDocShell* pDocSh, const ScRangeList& rR);
	virtual					~ScCellsEnumeration();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XEnumeration
	virtual sal_Bool SAL_CALL hasMoreElements();
	virtual ::com::sun::star::uno::Any SAL_CALL nextElement();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};

class ScCellFormatsObj : public cppu::WeakImplHelper3<
							com::sun::star::container::XIndexAccess,
							com::sun::star::container::XEnumerationAccess,
							com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*				pDocShell;
	ScRange					aTotalRange;

private:
	ScCellRangeObj*			GetObjectByIndex_Impl(long nIndex) const;

public:
							ScCellFormatsObj(ScDocShell* pDocSh, const ScRange& rR);
	virtual					~ScCellFormatsObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XIndexAccess
	virtual sal_Int32 SAL_CALL getCount();
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

							// XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL
							createEnumeration();

							// XElementAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
	virtual sal_Bool SAL_CALL hasElements();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class ScCellFormatsEnumeration : public cppu::WeakImplHelper2<
									com::sun::star::container::XEnumeration,
									com::sun::star::lang::XServiceInfo >,
								 public SfxListener
{
private:
	ScDocShell*				pDocShell;
	SCTAB					nTab;
	ScAttrRectIterator*		pIter;
	ScRange					aNext;
	sal_Bool					bAtEnd;
	sal_Bool					bDirty;

private:
	void					Advance_Impl();
	ScCellRangeObj*			NextObject_Impl();

public:
							ScCellFormatsEnumeration(ScDocShell* pDocSh, const ScRange& rR);
	virtual					~ScCellFormatsEnumeration();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XEnumeration
	virtual sal_Bool SAL_CALL hasMoreElements();
	virtual ::com::sun::star::uno::Any SAL_CALL nextElement();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};

typedef std::vector< ScRangeList > ScMyRangeLists;

class ScUniqueCellFormatsObj : public cppu::WeakImplHelper3<
							com::sun::star::container::XIndexAccess,
							com::sun::star::container::XEnumerationAccess,
							com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*						pDocShell;
	ScRange							aTotalRange;
	ScMyRangeLists					aRangeLists;

private:
	void							GetObjects_Impl();

public:
							ScUniqueCellFormatsObj(ScDocShell* pDocSh, const ScRange& rR);
	virtual					~ScUniqueCellFormatsObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XIndexAccess
	virtual sal_Int32 SAL_CALL getCount();
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

							// XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL
							createEnumeration();

							// XElementAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
	virtual sal_Bool SAL_CALL hasElements();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};

class ScUniqueCellFormatsEnumeration : public cppu::WeakImplHelper2<
									com::sun::star::container::XEnumeration,
									com::sun::star::lang::XServiceInfo >,
								 public SfxListener
{
private:
	ScMyRangeLists					aRangeLists;
	ScDocShell*						pDocShell;
	sal_Int32						nCurrentPosition;

public:
							ScUniqueCellFormatsEnumeration(ScDocShell* pDocShell, const ScMyRangeLists& rRangeLists);
	virtual					~ScUniqueCellFormatsEnumeration();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XEnumeration
	virtual sal_Bool SAL_CALL hasMoreElements();
	virtual ::com::sun::star::uno::Any SAL_CALL nextElement();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


#endif
