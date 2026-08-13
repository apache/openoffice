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



#ifndef SC_DAPIUNO_HXX
#define SC_DAPIUNO_HXX

#include "global.hxx"
#include "dpobject.hxx"
#include "rangeutl.hxx"		// ScArea
#include "cellsuno.hxx"     // for XModifyListenerArr_Impl

#include <svl/lstner.hxx>
#include <svl/itemprop.hxx>

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>

#include <com/sun/star/sheet/DataPilotFieldAutoShowInfo.hpp>
#include <com/sun/star/sheet/DataPilotFieldGroupInfo.hpp>
#include <com/sun/star/sheet/DataPilotFieldLayoutInfo.hpp>
#include <com/sun/star/sheet/DataPilotFieldOrientation.hpp>
#include <com/sun/star/sheet/DataPilotFieldReference.hpp>
#include <com/sun/star/sheet/DataPilotFieldSortInfo.hpp>
#include <com/sun/star/sheet/GeneralFunction.hpp>
#include <com/sun/star/sheet/XDataPilotDataLayoutFieldSupplier.hpp>
#include <com/sun/star/sheet/XDataPilotDescriptor.hpp>
#include <com/sun/star/sheet/XDataPilotField.hpp>
#include <com/sun/star/sheet/XDataPilotFieldGrouping.hpp>
#include <com/sun/star/sheet/XDataPilotTable.hpp>
#include <com/sun/star/sheet/XDataPilotTable2.hpp>
#include <com/sun/star/sheet/XDataPilotTables.hpp>

#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase4.hxx>
#include <cppuhelper/implbase5.hxx>

#include <memory>
#include <vector>

namespace com { namespace sun { namespace star { namespace sheet {
    struct DataPilotFieldFilter;
    struct DataPilotTablePositionData;
}}}}

class ScDocShell;
class ScDPSaveDimension;
class ScDPSaveGroupDimension;
class ScDPSaveNumGroupDimension;
struct ScDPNumGroupInfo;

class ScDataPilotTableObj;
class ScDataPilotFieldObj;
class ScDataPilotItemObj;


class ScDataPilotConversion
{
public:
	static com::sun::star::sheet::GeneralFunction	FirstFunc( sal_uInt16 nBits );
	static sal_uInt16			FunctionBit( com::sun::star::sheet::GeneralFunction eFunc );

    static void         FillGroupInfo(
                            ::com::sun::star::sheet::DataPilotFieldGroupInfo& rInfo,
                            const ScDPNumGroupInfo& rGroupInfo );
};

// ============================================================================

/** DataPilotTables collection per sheet. */
class ScDataPilotTablesObj : public cppu::WeakImplHelper4<
										com::sun::star::sheet::XDataPilotTables,
										com::sun::star::container::XEnumerationAccess,
										com::sun::star::container::XIndexAccess,
										com::sun::star::lang::XServiceInfo>,
									public SfxListener
{
private:
	ScDocShell*				pDocShell;
	SCTAB					nTab;

    ScDataPilotTableObj*    GetObjectByIndex_Impl( sal_Int32 nIndex );
	ScDataPilotTableObj*	GetObjectByName_Impl(const ::rtl::OUString& aName);

public:
							ScDataPilotTablesObj(ScDocShell* pDocSh, SCTAB nT);
	virtual					~ScDataPilotTablesObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XDataPilotTables
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XDataPilotDescriptor > SAL_CALL
							createDataPilotDescriptor();
	virtual void SAL_CALL	insertNewByName( const ::rtl::OUString& aName,
									const ::com::sun::star::table::CellAddress& aOutputAddress,
									const ::com::sun::star::uno::Reference<
										::com::sun::star::sheet::XDataPilotDescriptor >& xDescriptor );
	virtual void SAL_CALL	removeByName( const ::rtl::OUString& aName );

							// XNameAccess
	virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
	virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

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

// ============================================================================

//	ScDataPilotDescriptorBase is never instantiated directly
class ScDataPilotDescriptorBase : public com::sun::star::sheet::XDataPilotDescriptor,
                                  public com::sun::star::beans::XPropertySet,
                                  public com::sun::star::sheet::XDataPilotDataLayoutFieldSupplier,
								  public com::sun::star::lang::XServiceInfo,
								  public com::sun::star::lang::XUnoTunnel,
								  public com::sun::star::lang::XTypeProvider,
								  public cppu::OWeakObject,
								  public SfxListener
{
private:
    SfxItemPropertySet  maPropSet;
    ScDocShell*         pDocShell;

public:
							ScDataPilotDescriptorBase(ScDocShell* pDocSh);
	virtual					~ScDataPilotDescriptorBase();

	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
								const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL	acquire() throw();
	virtual void SAL_CALL	release() throw();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

	ScDocShell*				GetDocShell() const		{ return pDocShell; }

							// in den Ableitungen:
    virtual ScDPObject* GetDPObject() const = 0;
    virtual void SetDPObject(ScDPObject* pDPObj) = 0;

							// XDataPilotDescriptor
							//	getName, setName, getTag, setTag in derived classes

	virtual ::com::sun::star::table::CellRangeAddress SAL_CALL getSourceRange();
	virtual void SAL_CALL setSourceRange( const ::com::sun::star::table::CellRangeAddress& aSourceRange );
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XSheetFilterDescriptor > SAL_CALL
							getFilterDescriptor();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > SAL_CALL
							getDataPilotFields();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > SAL_CALL
							getColumnFields();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > SAL_CALL
							getRowFields();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > SAL_CALL
							getPageFields();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > SAL_CALL
							getDataFields();
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > SAL_CALL
							getHiddenFields();

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo(  );
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

                            // XDataPilotDataLayoutFieldSupplier
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::sheet::XDataPilotField >
                            SAL_CALL getDataLayoutField();

							// XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence<
									sal_Int8 >& aIdentifier );

	static const com::sun::star::uno::Sequence<sal_Int8>& getUnoTunnelId();
	static ScDataPilotDescriptorBase* getImplementation( const com::sun::star::uno::Reference<
									com::sun::star::sheet::XDataPilotDescriptor> xObj );

							// XTypeProvider (overloaded in ScDataPilotTableObj)
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();
	virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();

							// XServiceInfo is in derived classes
};

// ============================================================================

class ScDataPilotDescriptor : public ScDataPilotDescriptorBase
{
private:
    ScDPObject*             mpDPObject;

public:
							ScDataPilotDescriptor(ScDocShell* pDocSh);
	virtual					~ScDataPilotDescriptor();

    virtual ScDPObject* GetDPObject() const;
    virtual void SetDPObject(ScDPObject* pDPObj);

							// rest of XDataPilotDescriptor (incl. XNamed)
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL	setName( const ::rtl::OUString& aName );
	virtual ::rtl::OUString SAL_CALL getTag();
	virtual void SAL_CALL	setTag( const ::rtl::OUString& aTag );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};

// ============================================================================

class ScDataPilotTableObj : public ScDataPilotDescriptorBase,
                            public com::sun::star::sheet::XDataPilotTable2,
                            public com::sun::star::util::XModifyBroadcaster
{
private:
	SCTAB					nTab;
	String					aName;
    XModifyListenerArr_Impl aModifyListeners;

    void                    Refreshed_Impl();

public:
							ScDataPilotTableObj(ScDocShell* pDocSh, SCTAB nT, const String& rN);
	virtual					~ScDataPilotTableObj();

	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface(
								const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL	acquire() throw();
	virtual void SAL_CALL	release() throw();

    virtual void            Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

    virtual ScDPObject* GetDPObject() const;
    virtual void SetDPObject(ScDPObject* pDPObj);

							// rest of XDataPilotDescriptor (incl. XNamed)
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL	setName( const ::rtl::OUString& aName );
	virtual ::rtl::OUString SAL_CALL getTag();
	virtual void SAL_CALL	setTag( const ::rtl::OUString& aTag );

							// XDataPilotTable
	virtual ::com::sun::star::table::CellRangeAddress SAL_CALL getOutputRange();
	virtual void SAL_CALL	refresh();

                            // XDataPilotTable2
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > >
        SAL_CALL getDrillDownData(const ::com::sun::star::table::CellAddress& aAddr);

    virtual ::com::sun::star::sheet::DataPilotTablePositionData
        SAL_CALL getPositionData(const ::com::sun::star::table::CellAddress& aAddr);

    virtual void SAL_CALL insertDrillDownSheet(const ::com::sun::star::table::CellAddress& aAddr);

    virtual ::com::sun::star::table::CellRangeAddress SAL_CALL getOutputRangeByType( sal_Int32 nType );

                            // XModifyBroadcaster
    virtual void SAL_CALL   addModifyListener( const ::com::sun::star::uno::Reference<
                                                ::com::sun::star::util::XModifyListener >& aListener );
    virtual void SAL_CALL   removeModifyListener( const ::com::sun::star::uno::Reference<
                                                ::com::sun::star::util::XModifyListener >& aListener );

							// XTypeProvider (overloaded)
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes();
	virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};

// ============================================================================

struct ScFieldIdentifier
{
    ::rtl::OUString     maFieldName;    /// Source field name.
    sal_Int32           mnFieldIdx;     /// Field index (if several fields with same name exist).
    bool                mbDataLayout;   /// True = data layout field collecting all data fields as items.

    inline explicit     ScFieldIdentifier() :
                            mnFieldIdx( 0 ), mbDataLayout( false ) {}

    inline explicit     ScFieldIdentifier( const ::rtl::OUString& rFieldName, sal_Int32 nFieldIdx, bool bDataLayout ) :
                            maFieldName( rFieldName ), mnFieldIdx( nFieldIdx ), mbDataLayout( bDataLayout ) {}
};

// ============================================================================

/** Base class of all implementation objects based on a DataPilot descriptor
    or DataPilot table object. Wraps acquiring and releasing the parent. */
class ScDataPilotChildObjBase
{
protected:
    explicit            ScDataPilotChildObjBase( ScDataPilotDescriptorBase& rParent );
    explicit            ScDataPilotChildObjBase( ScDataPilotDescriptorBase& rParent, const ScFieldIdentifier& rFieldId );
    virtual             ~ScDataPilotChildObjBase();

    /** Returns the wrapped DataPilot object (calls GetDPObject() at parent). */
    ScDPObject*         GetDPObject() const;
    /** Sets the passed DataPilot object (calls SetDPObject() at parent). */
    void                SetDPObject( ScDPObject* pDPObject );

    /** Returns the DataPilot dimension object related to the field described by maFieldId. */
    ScDPSaveDimension*  GetDPDimension( ScDPObject** ppDPObject = 0 ) const;

    /** Returns the number of members for the field described by maFieldId. */
    sal_Int32           GetMemberCount() const;
    /** Returns the collection of members for the field described by maFieldId. */
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >
                        GetMembers() const;

protected:
    ScDataPilotDescriptorBase& mrParent;
    ScFieldIdentifier   maFieldId;

private:
    ScDataPilotChildObjBase& operator=( const ScDataPilotChildObjBase& );
};

// ============================================================================

typedef ::cppu::WeakImplHelper4
<
    ::com::sun::star::container::XEnumerationAccess,
    ::com::sun::star::container::XIndexAccess,
    ::com::sun::star::container::XNameAccess,
    ::com::sun::star::lang::XServiceInfo
>
ScDataPilotFieldsObjImpl;

/** Collection of all DataPilot fields, or of all fields from a specific dimension. */
class ScDataPilotFieldsObj : public ScDataPilotChildObjBase, public ScDataPilotFieldsObjImpl
{
public:
    explicit            ScDataPilotFieldsObj(
                            ScDataPilotDescriptorBase& rParent );

    explicit            ScDataPilotFieldsObj(
                            ScDataPilotDescriptorBase& rParent,
                            ::com::sun::star::sheet::DataPilotFieldOrientation eOrient );

    virtual             ~ScDataPilotFieldsObj();

							// XNameAccess
	virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
	virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

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

private:
    ScDataPilotFieldObj*    GetObjectByIndex_Impl( sal_Int32 nIndex ) const;
    ScDataPilotFieldObj*    GetObjectByName_Impl( const ::rtl::OUString& rName ) const;

private:
    ::com::sun::star::uno::Any maOrient;    /// Field orientation, no value = all fields.
};

// ============================================================================

typedef ::cppu::WeakImplHelper5
<
    ::com::sun::star::container::XNamed,
    ::com::sun::star::beans::XPropertySet,
    ::com::sun::star::sheet::XDataPilotField,
    ::com::sun::star::sheet::XDataPilotFieldGrouping,
    ::com::sun::star::lang::XServiceInfo
>
ScDataPilotFieldObjImpl;

/** Implementation of a single DataPilot field. */
class ScDataPilotFieldObj : public ScDataPilotChildObjBase, public ScDataPilotFieldObjImpl
{
public:
                        ScDataPilotFieldObj(
                            ScDataPilotDescriptorBase& rParent,
                            const ScFieldIdentifier& rIdent );

                        ScDataPilotFieldObj(
                            ScDataPilotDescriptorBase& rParent,
                            const ScFieldIdentifier& rIdent,
                            const ::com::sun::star::uno::Any& rOrient );

    virtual             ~ScDataPilotFieldObj();

							// XNamed
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL	setName( const ::rtl::OUString& aName );

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo(  );
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

                            // XDatePilotField
    virtual com::sun::star::uno::Reference<com::sun::star::container::XIndexAccess> SAL_CALL
                                getItems();

							// nur noch aus Property-Funktionen gerufen:
	com::sun::star::sheet::DataPilotFieldOrientation getOrientation(void) const;
	void setOrientation(com::sun::star::sheet::DataPilotFieldOrientation Orientation);
	com::sun::star::sheet::GeneralFunction getFunction(void) const;
	void setFunction(com::sun::star::sheet::GeneralFunction Function);
    com::sun::star::uno::Sequence< com::sun::star::sheet::GeneralFunction > getSubtotals() const;
    void setSubtotals(const com::sun::star::uno::Sequence< com::sun::star::sheet::GeneralFunction >& rFunctions);
    rtl::OUString getCurrentPage() const;
    void setCurrentPage(const rtl::OUString& sPage);
    sal_Bool getUseCurrentPage() const;
    void setUseCurrentPage(sal_Bool bUse);
    const com::sun::star::sheet::DataPilotFieldAutoShowInfo* getAutoShowInfo();
    void setAutoShowInfo(const com::sun::star::sheet::DataPilotFieldAutoShowInfo* pInfo);
    const com::sun::star::sheet::DataPilotFieldLayoutInfo* getLayoutInfo();
    void setLayoutInfo(const com::sun::star::sheet::DataPilotFieldLayoutInfo* pInfo);
    const com::sun::star::sheet::DataPilotFieldReference* getReference();
    void setReference(const com::sun::star::sheet::DataPilotFieldReference* pInfo);
    const com::sun::star::sheet::DataPilotFieldSortInfo* getSortInfo();
    void setSortInfo(const com::sun::star::sheet::DataPilotFieldSortInfo* pInfo);
    sal_Bool getShowEmpty() const;
    void setShowEmpty(sal_Bool bShow);

    sal_Bool hasGroupInfo();
    com::sun::star::sheet::DataPilotFieldGroupInfo getGroupInfo();
    void setGroupInfo(const com::sun::star::sheet::DataPilotFieldGroupInfo* pInfo);

                            // XDataPilotFieldGrouping
    sal_Bool HasString(const com::sun::star::uno::Sequence< ::rtl::OUString >& aItems, const ::rtl::OUString& aString);
    virtual com::sun::star::uno::Reference < com::sun::star::sheet::XDataPilotField > SAL_CALL
        createNameGroup(const com::sun::star::uno::Sequence< ::rtl::OUString >& aItems);
    virtual com::sun::star::uno::Reference < com::sun::star::sheet::XDataPilotField > SAL_CALL
        createDateGroup(const com::sun::star::sheet::DataPilotFieldGroupInfo& rInfo);

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

private:
    ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >
                        mxItems;
    SfxItemPropertySet  maPropSet;
    ::com::sun::star::uno::Any maOrient;
};

// ============================================================================

typedef ::std::vector< ::rtl::OUString > ScFieldGroupMembers;

struct ScFieldGroup
{
    ::rtl::OUString     maName;
    ScFieldGroupMembers maMembers;
};

typedef ::std::vector< ScFieldGroup > ScFieldGroups;

// ============================================================================

typedef ::cppu::WeakImplHelper4
<
    ::com::sun::star::container::XNameContainer,
    ::com::sun::star::container::XEnumerationAccess,
    ::com::sun::star::container::XIndexAccess,
    ::com::sun::star::lang::XServiceInfo
>
ScDataPilotFieldGroupsObjImpl;

/** Implementation of all grouped items in a DataPilot field.

    This is a stand-alone object without any connection to the base DataPilot
    field. Grouping info has to be written back with the GroupInfo property of
    the DataPilot field after modifying this object.
 */
class ScDataPilotFieldGroupsObj : public ScDataPilotFieldGroupsObjImpl
{
public:
    explicit            ScDataPilotFieldGroupsObj( const ScFieldGroups& rGroups );
    virtual             ~ScDataPilotFieldGroupsObj();

							// XNameAccess
	virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
	virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

                                // XNameReplace
    virtual void SAL_CALL replaceByName( const ::rtl::OUString& aName,
                                const ::com::sun::star::uno::Any& aElement );

                            // XNameContainer
    virtual void SAL_CALL insertByName( const ::rtl::OUString& aName,
                                const ::com::sun::star::uno::Any& aElement );
    virtual void SAL_CALL removeByName( const ::rtl::OUString& Name );

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

    // implementation
    ScFieldGroup&       getFieldGroup( const ::rtl::OUString& rName );
    void                renameFieldGroup( const ::rtl::OUString& rOldName, const ::rtl::OUString& rNewName );

private:
    ScFieldGroups::iterator implFindByName( const ::rtl::OUString& rName );

private:
    ScFieldGroups       maGroups;
};

// ============================================================================

typedef ::cppu::WeakImplHelper5
<
    ::com::sun::star::container::XNameContainer,
    ::com::sun::star::container::XEnumerationAccess,
    ::com::sun::star::container::XIndexAccess,
    ::com::sun::star::container::XNamed,
    ::com::sun::star::lang::XServiceInfo
>
ScDataPilotFieldGroupObjImpl;

class ScDataPilotFieldGroupObj : public ScDataPilotFieldGroupObjImpl
{
public:
    explicit            ScDataPilotFieldGroupObj( ScDataPilotFieldGroupsObj& rParent, const ::rtl::OUString& rGroupName );
    virtual             ~ScDataPilotFieldGroupObj();

                            // XNameAccess
	virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
	virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

                                // XNameReplace
    virtual void SAL_CALL replaceByName( const ::rtl::OUString& aName,
                                const ::com::sun::star::uno::Any& aElement );

                                // XNameContainer
    virtual void SAL_CALL insertByName( const ::rtl::OUString& aName,
                                const ::com::sun::star::uno::Any& aElement );
    virtual void SAL_CALL removeByName( const ::rtl::OUString& Name );

                    // XIndexAccess
	virtual sal_Int32 SAL_CALL getCount();
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

							// XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration > SAL_CALL
							createEnumeration();

							// XElementAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
	virtual sal_Bool SAL_CALL hasElements();

                            // XNamed
    virtual ::rtl::OUString SAL_CALL getName();
    virtual void SAL_CALL   setName( const ::rtl::OUString& aName );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

private:
    ScDataPilotFieldGroupsObj& mrParent;
    ::rtl::OUString     maGroupName;
};

// ============================================================================

typedef ::cppu::WeakImplHelper2
<
    ::com::sun::star::container::XNamed,
    ::com::sun::star::lang::XServiceInfo
>
ScDataPilotFieldGroupItemObjImpl;

class ScDataPilotFieldGroupItemObj : public ScDataPilotFieldGroupItemObjImpl
{
public:
    explicit            ScDataPilotFieldGroupItemObj( ScDataPilotFieldGroupObj& rParent, const ::rtl::OUString& rName );
    virtual             ~ScDataPilotFieldGroupItemObj();

							// XNamed
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL	setName( const ::rtl::OUString& aName );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

private:
    ScDataPilotFieldGroupObj& mrParent;
    ::rtl::OUString     maName;
};

// ============================================================================

typedef ::cppu::WeakImplHelper4
<
    ::com::sun::star::container::XEnumerationAccess,
    ::com::sun::star::container::XIndexAccess,
    ::com::sun::star::container::XNameAccess,
    ::com::sun::star::lang::XServiceInfo
>
ScDataPilotItemsObjImpl;

class ScDataPilotItemsObj : public ScDataPilotChildObjBase, public ScDataPilotItemsObjImpl
{
public:
    explicit            ScDataPilotItemsObj( ScDataPilotDescriptorBase& rParent, const ScFieldIdentifier& rFieldId );
    virtual             ~ScDataPilotItemsObj();

							// XNameAccess
	virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
	virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

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

private:
    ScDataPilotItemObj* GetObjectByIndex_Impl( sal_Int32 nIndex ) const;
};

// ============================================================================

typedef ::cppu::WeakImplHelper3
<
    ::com::sun::star::container::XNamed,
    ::com::sun::star::beans::XPropertySet,
    ::com::sun::star::lang::XServiceInfo
>
ScDataPilotItemObjImpl;

class ScDataPilotItemObj : public ScDataPilotChildObjBase, public ScDataPilotItemObjImpl
{
public:
    explicit            ScDataPilotItemObj(
                            ScDataPilotDescriptorBase& rParent,
                            const ScFieldIdentifier& rFieldId,
                            sal_Int32 nIndex );

    virtual             ~ScDataPilotItemObj();

							// XNamed
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL	setName( const ::rtl::OUString& aName );

							// XPropertySet
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
							SAL_CALL getPropertySetInfo(  );
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

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

private:
    SfxItemPropertySet  maPropSet;
    sal_Int32           mnIndex;
};

// ============================================================================

#endif
