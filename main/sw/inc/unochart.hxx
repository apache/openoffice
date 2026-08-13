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


#ifndef _UNOCHART_HXX
#define _UNOCHART_HXX

#include <map>
#include <set>

#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/chart2/data/XDataProvider.hpp>
#include <com/sun/star/chart2/data/XDataSource.hpp>
#include <com/sun/star/chart2/data/XDataSequence.hpp>
#include <com/sun/star/chart2/data/XTextualDataSequence.hpp>
#include <com/sun/star/chart2/data/XNumericalDataSequence.hpp>
#include <com/sun/star/chart2/data/XLabeledDataSequence.hpp>
#include <com/sun/star/chart2/data/XRangeXMLConversion.hpp>
#include <com/sun/star/chart2/data/DataSequenceRole.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/util/XCloneable.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>
#include <com/sun/star/util/XModifyListener.hpp>
#include <com/sun/star/chart/ChartDataRowSource.hpp>

#include <cppuhelper/interfacecontainer.h>  //OMultiTypeInterfaceContainerHelper
#include <cppuhelper/implbase2.hxx> // helper for implementations
#include <cppuhelper/implbase4.hxx> // helper for implementations
#include <cppuhelper/implbase6.hxx> // helper for implementations
#include <cppuhelper/implbase10.hxx> // helper for implementations
#include <cppuhelper/weakref.hxx>

#include <tools/string.hxx>
#include <tools/link.hxx>
#include <vcl/timer.hxx>

#include <calbck.hxx>


class SfxItemPropertySet;
class SwDoc;
class SwTable;
class SwTableBox;
class SwUnoCrsr;
struct SwRangeDescriptor;
class SwSelBoxes;
class SwFrmFmt;

//////////////////////////////////////////////////////////////////////

sal_Bool FillRangeDescriptor( SwRangeDescriptor &rDesc, const String &rCellRangeName );

//////////////////////////////////////////////////////////////////////

class SwChartHelper
{
public:
    static void DoUpdateAllCharts( SwDoc* pDoc );
};

//////////////////////////////////////////////////////////////////////

class SwChartLockController_Helper
{
    SwDoc   *pDoc;

    DECL_LINK( DoUnlockAllCharts, Timer * );
    Timer   aUnlockTimer;   // timer to unlock chart controllers
    bool    bIsLocked;


    // disallow use of d-tor, copy c-tor and assignment operator
    SwChartLockController_Helper( const SwChartLockController_Helper & );
    SwChartLockController_Helper & operator = ( const SwChartLockController_Helper & );

    void LockUnlockAllCharts( sal_Bool bLock );
    void LockAllCharts()    { LockUnlockAllCharts( sal_True ); };
    void UnlockAllCharts()  { LockUnlockAllCharts( sal_False ); };

public:
    SwChartLockController_Helper( SwDoc *pDocument );
    ~SwChartLockController_Helper();

    void StartOrContinueLocking();
    void Disconnect();
};

//////////////////////////////////////////////////////////////////////

typedef cppu::WeakImplHelper4
<
    ::com::sun::star::chart2::data::XDataProvider,
    ::com::sun::star::chart2::data::XRangeXMLConversion,
    ::com::sun::star::lang::XComponent,
    ::com::sun::star::lang::XServiceInfo
>
SwChartDataProviderBaseClass;

class SwChartDataProvider :
    public SwChartDataProviderBaseClass,
    public SwClient
{

    // used to keep weak-references to all data-sequences of a single table
    // see set definition below...
    struct lt_DataSequenceRef
    {
        bool operator()( ::com::sun::star::uno::WeakReference< ::com::sun::star::chart2::data::XDataSequence > xWRef1, ::com::sun::star::uno::WeakReference< ::com::sun::star::chart2::data::XDataSequence > xWRef2 ) const
        {
            ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSequence > xRef1( xWRef1 );
            ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSequence > xRef2( xWRef2 );
            return xRef1.get() < xRef2.get();
        }
    };
    typedef std::set< ::com::sun::star::uno::WeakReference < ::com::sun::star::chart2::data::XDataSequence >, lt_DataSequenceRef > Set_DataSequenceRef_t;

    // map of data-sequence sets for each table
    struct lt_SwTable_Ptr
    {
        bool operator()( const SwTable *p1, const SwTable *p2 ) const
        {
            return p1 < p2;
        }
    };
    typedef std::map< const SwTable *, Set_DataSequenceRef_t, lt_SwTable_Ptr > Map_Set_DataSequenceRef_t;


    // map of all data-sequences provided directly or indirectly (e.g. via
    // data-source) by this object. Since there is only one object of this type
    // for each document it should hold references to all used data-sequences for
    // all tables of the document.
    mutable Map_Set_DataSequenceRef_t       aDataSequences;

    ::cppu::OInterfaceContainerHelper       aEvtListeners;
    const SwDoc *                           pDoc;
    sal_Bool                                bDisposed;


    // disallow use of c-tor and assignment operator
    SwChartDataProvider( const SwChartDataProvider & );
    SwChartDataProvider & operator = ( const SwChartDataProvider & );

    ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSource > SAL_CALL Impl_createDataSource( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArguments, sal_Bool bTestOnly = sal_False );
    ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSequence > SAL_CALL Impl_createDataSequenceByRangeRepresentation( const ::rtl::OUString& aRangeRepresentation, sal_Bool bTestOnly = sal_False  );

    rtl::OUString GetBrokenCellRangeForExport( const rtl::OUString &rCellRangeRepresentation );

protected:
    //SwClient
    virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);

public:
    SwChartDataProvider( const SwDoc* pDoc );
    virtual ~SwChartDataProvider();

    // XDataProvider
    virtual ::sal_Bool SAL_CALL createDataSourcePossible( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArguments );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSource > SAL_CALL createDataSource( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aArguments );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL detectArguments( const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSource >& xDataSource );
    virtual ::sal_Bool SAL_CALL createDataSequenceByRangeRepresentationPossible( const ::rtl::OUString& aRangeRepresentation );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSequence > SAL_CALL createDataSequenceByRangeRepresentation( const ::rtl::OUString& aRangeRepresentation );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XRangeSelection > SAL_CALL getRangeSelection(  );

    // XRangeXMLConversion
    virtual ::rtl::OUString SAL_CALL convertRangeToXML( const ::rtl::OUString& aRangeRepresentation );
    virtual ::rtl::OUString SAL_CALL convertRangeFromXML( const ::rtl::OUString& aXMLRange );

    // XComponent
    virtual void SAL_CALL dispose(  );
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

    SwFrmFmt*       GetFrmFmt() const { return (SwFrmFmt*)GetRegisteredIn(); }

    void        AddDataSequence( const SwTable &rTable, ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSequence > &rxDataSequence );
    void        RemoveDataSequence( const SwTable &rTable, ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSequence > &rxDataSequence );

    // will send modified events for all data-sequences of the table
    void        InvalidateTable( const SwTable *pTable );
    sal_Bool    DeleteBox( const SwTable *pTable, const SwTableBox &rBox );
    void        DisposeAllDataSequences( const SwTable *pTable );

    // functionality needed to get notified about new added rows/cols
    void        AddRowCols( const SwTable &rTable, const SwSelBoxes& rBoxes, sal_uInt16 nLines, sal_Bool bBehind );
};

//////////////////////////////////////////////////////////////////////

typedef cppu::WeakImplHelper2
<
    ::com::sun::star::chart2::data::XDataSource,
    ::com::sun::star::lang::XServiceInfo
>
SwChartDataSourceBaseClass;

class SwChartDataSource :
    public SwChartDataSourceBaseClass
{
    com::sun::star::uno::Sequence<
        com::sun::star::uno::Reference<
            com::sun::star::chart2::data::XLabeledDataSequence > > aLDS;

    // disallow use of c-tor and assignment operator
    SwChartDataSource( const SwChartDataSource & );
    SwChartDataSource & operator = ( const SwChartDataSource & );

public:
    SwChartDataSource( const com::sun::star::uno::Sequence< com::sun::star::uno::Reference< com::sun::star::chart2::data::XLabeledDataSequence > > &rLDS );
    virtual ~SwChartDataSource();

    // XDataSource
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XLabeledDataSequence > > SAL_CALL getDataSequences(  );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );
};

//////////////////////////////////////////////////////////////////////

typedef cppu::WeakImplHelper10
<
    ::com::sun::star::chart2::data::XDataSequence,
    ::com::sun::star::chart2::data::XTextualDataSequence,
    ::com::sun::star::chart2::data::XNumericalDataSequence,
    ::com::sun::star::util::XCloneable,
    ::com::sun::star::beans::XPropertySet,
    ::com::sun::star::lang::XServiceInfo,
    ::com::sun::star::lang::XUnoTunnel,
    ::com::sun::star::util::XModifiable,
    ::com::sun::star::lang::XEventListener,
    ::com::sun::star::lang::XComponent
>
SwChartDataSequenceBaseClass;

class SwChartDataSequence :
    public SwChartDataSequenceBaseClass,
    public SwClient
{
    ::cppu::OInterfaceContainerHelper                       aEvtListeners;
    ::cppu::OInterfaceContainerHelper                       aModifyListeners;
    ::com::sun::star::chart2::data::DataSequenceRole        aRole;

    String  aRowLabelText;
    String  aColLabelText;

    // holds a reference to the data-provider to guarantee it's lifetime last as
    // long as the pointer may be used.
    ::com::sun::star::uno::Reference< com::sun::star::chart2::data::XDataProvider >    xDataProvider;
    SwChartDataProvider *                   pDataProvider;

    SwUnoCrsr*                  pTblCrsr;   // cursor spanned over cells to use
	SwDepend					aCursorDepend; //the cursor is removed after the doc has been removed

    const SfxItemPropertySet*   _pPropSet;

    sal_Bool    bDisposed;

    // disallow use of c-tor and assignment operator
    SwChartDataSequence( const SwChartDataSequence &rObj );
    SwChartDataSequence & operator = ( const SwChartDataSequence & );

protected:
    //SwClient
    virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);

public:
    SwChartDataSequence( SwChartDataProvider &rProvider,
                         SwFrmFmt   &rTblFmt,
                         SwUnoCrsr  *pTableCursor );
    virtual ~SwChartDataSequence();

    static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();

    //XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier );

    // XDataSequence
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getData(  );
    virtual ::rtl::OUString SAL_CALL getSourceRangeRepresentation(  );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL generateLabel( ::com::sun::star::chart2::data::LabelOrigin eLabelOrigin );
    virtual ::sal_Int32 SAL_CALL getNumberFormatKeyByIndex( ::sal_Int32 nIndex );

    // XTextualDataSequence
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getTextualData(  );

    // XNumericalDataSequence
    virtual ::com::sun::star::uno::Sequence< double > SAL_CALL getNumericalData(  );

    // XCloneable
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloneable > SAL_CALL createClone(  );

    // XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

    // XModifiable
    virtual ::sal_Bool SAL_CALL isModified(  );
    virtual void SAL_CALL setModified( ::sal_Bool bModified );

    // XModifyBroadcaster
    virtual void SAL_CALL addModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );
    virtual void SAL_CALL removeModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );

    // XEventListener
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source );

    // XComponent
    virtual void SAL_CALL dispose(  );
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener );

    SwFrmFmt*   GetFrmFmt() const { return (SwFrmFmt*)GetRegisteredIn(); }
    sal_Bool    DeleteBox( const SwTableBox &rBox );

    void        FillRangeDesc( SwRangeDescriptor &rRangeDesc ) const;
    bool        ExtendTo( bool bExtendCol, sal_Int32 nFirstNew, sal_Int32 nCount );
};

//////////////////////////////////////////////////////////////////////

typedef cppu::WeakImplHelper6
<
    ::com::sun::star::chart2::data::XLabeledDataSequence,
    ::com::sun::star::util::XCloneable,
    ::com::sun::star::lang::XServiceInfo,
    ::com::sun::star::util::XModifyListener,
    ::com::sun::star::util::XModifyBroadcaster,
    ::com::sun::star::lang::XComponent
>
SwChartLabeledDataSequenceBaseClass;

class SwChartLabeledDataSequence :
    public SwChartLabeledDataSequenceBaseClass
{
    ::cppu::OInterfaceContainerHelper                           aEvtListeners;
    ::cppu::OInterfaceContainerHelper                           aModifyListeners;

    ::com::sun::star::uno::Reference<
            ::com::sun::star::chart2::data::XDataSequence >     xData;
    ::com::sun::star::uno::Reference<
            ::com::sun::star::chart2::data::XDataSequence >     xLabels;

    sal_Bool    bDisposed;

    // disallow use of c-tor and assignment operator
    SwChartLabeledDataSequence( const SwChartLabeledDataSequence & );
    SwChartLabeledDataSequence & operator = ( const SwChartLabeledDataSequence & );

	void	SetDataSequence( ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSequence >& rxDest, const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSequence >& rxSource );


public:
    SwChartLabeledDataSequence();
    virtual ~SwChartLabeledDataSequence();

    // XLabeledDataSequence
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSequence > SAL_CALL getValues(  );
    virtual void SAL_CALL setValues( const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSequence >& xSequence );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSequence > SAL_CALL getLabel(  );
    virtual void SAL_CALL setLabel( const ::com::sun::star::uno::Reference< ::com::sun::star::chart2::data::XDataSequence >& xSequence );

    // XCloneable
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloneable > SAL_CALL createClone(  );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

    // XEventListener
    virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source );

    // XModifyListener
    virtual void SAL_CALL modified( const ::com::sun::star::lang::EventObject& aEvent );

    // XModifyBroadcaster
    virtual void SAL_CALL addModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );
    virtual void SAL_CALL removeModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );

    // XComponent
    virtual void SAL_CALL dispose(  );
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener );
};

//////////////////////////////////////////////////////////////////////

#endif
