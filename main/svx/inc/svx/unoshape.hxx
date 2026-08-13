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


#ifndef _SVX_UNOSHAPE_HXX
#define _SVX_UNOSHAPE_HXX

#include <com/sun/star/document/XActionLockable.hpp>
#include <com/sun/star/drawing/XEnhancedCustomShapeDefaulter.hpp>
#include <com/sun/star/drawing/XGluePointsSupplier.hpp>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/uno/XAggregation.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/awt/Point.hpp>
#include <com/sun/star/drawing/PolygonKind.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/beans/XMultiPropertyStates.hpp>
#include <tools/gen.hxx>
#include <tools/weakbase.hxx>
#include <svl/lstner.hxx>
#include <editeng/unoipset.hxx>
#include <cppuhelper/weak.hxx>
#include <cppuhelper/weakagg.hxx>
#include <cppuhelper/interfacecontainer.h>
#include <osl/mutex.hxx>
#include "svx/svxdllapi.h"
#include <rtl/ref.hxx>
#include <com/sun/star/uno/Any.hxx>

#include <svx/svdouno.hxx>

#include <comphelper/servicehelper.hxx>

#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase12.hxx>

#include <svx/unoprov.hxx>

class SfxItemSet;
class SdrModel;
class SvxDrawPage;
class SvGlobalName;
// --> OD 2009-01-16 #i59051#
namespace basegfx
    {
        class B2DPolyPolygon;
    } // end of namespace basegfx
// <--

class SvxShapeMutex
{
protected:
	::osl::Mutex maMutex;
};

struct SvxShapeImpl;
class SvxShapeMaster;
class SvxItemPropertySet;
class SvxItemPropertySimpleEntry;
class SfxItemSet;

void SVX_DLLPUBLIC SvxItemPropertySet_setPropertyValue( const SvxItemPropertySet& rPropSet, const SfxItemPropertySimpleEntry* pMap,
        const com::sun::star::uno::Any& rVal, SfxItemSet& rSet );

com::sun::star::uno::Any SVX_DLLPUBLIC SvxItemPropertySet_getPropertyValue( const SvxItemPropertySet& rPropSet, const SfxItemPropertySimpleEntry* pMap, const SfxItemSet& rSet );


// WARNING: if you update the supported interfaces,
//          also update SvxShape::_getTypes()
typedef ::cppu::WeakAggImplHelper12<
    ::com::sun::star::drawing::XShape,
    ::com::sun::star::lang::XComponent,
    ::com::sun::star::beans::XPropertySet,
    ::com::sun::star::beans::XMultiPropertySet,
    ::com::sun::star::beans::XPropertyState,
    ::com::sun::star::lang::XUnoTunnel,
    ::com::sun::star::container::XNamed,
    ::com::sun::star::drawing::XGluePointsSupplier,
    ::com::sun::star::container::XChild,
    ::com::sun::star::lang::XServiceInfo,
    ::com::sun::star::document::XActionLockable,
    ::com::sun::star::beans::XMultiPropertyStates> SvxShape_UnoImplHelper;

class SVX_DLLPUBLIC SvxShape : public SvxShape_UnoImplHelper,
                 public SfxListener,
                 public SvxShapeMutex
{
private:
	::com::sun::star::awt::Size maSize;
	::com::sun::star::awt::Point maPosition;
	::rtl::OUString	maShapeType;
	::rtl::OUString maShapeName;

	/** these members are used to optimize XMultiProperty calls */
	SvxShapeImpl* mpImpl;
	bool mbIsMultiPropertyCall;

	::com::sun::star::uno::WeakReference< ::com::sun::star::container::XIndexContainer > mxGluePoints;

protected:
	friend class SvxDrawPage;
	friend class SvxShapeConnector;
	friend class SdXShape;

	const SvxItemPropertySet* mpPropSet;
    const SfxItemPropertyMapEntry* maPropMapEntries;

    ::tools::WeakReference< SdrObject > mpObj;
	SdrModel* mpModel;
	// Umrechnungen fuer den Writer, der in TWIPS arbeitet
	void ForceMetricToItemPoolMetric(Pair& rPoint) const throw();
	void ForceMetricTo100th_mm(Pair& rPoint) const throw();
    // --> OD 2009-01-16 #i59051#
    void ForceMetricToItemPoolMetric(basegfx::B2DPolyPolygon& rPolyPolygon) const throw();
    void ForceMetricTo100th_mm(basegfx::B2DPolyPolygon& rPolyPolygon) const throw();
    // <--

    ::com::sun::star::uno::Any GetAnyForItem( SfxItemSet& aSet, const SfxItemPropertySimpleEntry* pMap ) const;

	sal_Bool tryQueryAggregation( const com::sun::star::uno::Type & rType, com::sun::star::uno::Any& rAny );

	sal_Bool SAL_CALL SetFillAttribute( sal_Int32 nWID, const ::rtl::OUString& rName );

	/** called from the XActionLockable interface methods on initial locking */
	virtual void lock();

	/** called from the XActionLockable interface methods on final unlock */
	virtual void unlock();

	/** used from the XActionLockable interface */
	sal_uInt16 mnLockCount;

    const SfxItemPropertyMapEntry* getPropertyMapEntries() const { return maPropMapEntries; }

	void updateShapeKind();
	void endSetPropertyValues();

	// overide these for special property handling in subcasses. Return true if property is handled
    virtual bool setPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, const ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyStateImpl( const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::beans::PropertyState& rState );
    virtual bool setPropertyToDefaultImpl( const SfxItemPropertySimpleEntry* pProperty );

public:
	SvxShape( SdrObject* pObj ) throw ();
    SvxShape( SdrObject* pObject, const SfxItemPropertyMapEntry* pEntries, const SvxItemPropertySet* pPropertySet ) throw ();
	SvxShape() throw ();
	virtual ~SvxShape() throw ();

	// Internals
	void ObtainSettingsFromPropertySet(const SvxItemPropertySet& rPropSet);
	virtual void Create( SdrObject* pNewOpj, SvxDrawPage* pNewPage = NULL );
    /** takes the ownership of the SdrObject.

        When the shape is disposed, and it has the ownership of its associated SdrObject, then
        it will delete this object.
    */
    void TakeSdrObjectOwnership();
    bool HasSdrObjectOwnership() const;

	void ChangeModel( SdrModel* pNewModel );

	void InvalidateSdrObject() { mpObj.reset( NULL ); };
	const SvxItemPropertySet& GetPropertySet() { return *mpPropSet; }
	SdrObject* GetSdrObject() const {return mpObj.get();}
	void SetShapeType( const ::rtl::OUString& ShapeType ) { maShapeType = ShapeType; }
	::com::sun::star::uno::Any GetBitmap( sal_Bool bMetaFile = sal_False ) const throw ();
	static SvxShape* GetShapeForSdrObj( SdrObject* pObj ) throw ();

    ::svx::PropertyChangeNotifier& getShapePropertyChangeNotifier();

	void setShapeKind( sal_uInt32 nKind );
	sal_uInt32 getShapeKind() const;

	// styles need this
	static sal_Bool SAL_CALL SetFillAttribute( sal_Int32 nWID, const ::rtl::OUString& rName, SfxItemSet& rSet, SdrModel* pModel );
	static sal_Bool SAL_CALL SetFillAttribute( sal_Int32 nWID, const ::rtl::OUString& rName, SfxItemSet& rSet );

    /** same as SetFillAttribute but for property names instead of which ids,
        and the property found is returned instead of set at the object
        directly.
     */
// os: unused function
//    static ::com::sun::star::uno::Any SAL_CALL GetFillAttributeByName(
//        const ::rtl::OUString& rPropertyName, const ::rtl::OUString& rName, SdrModel* pModel );

	UNO3_GETIMPLEMENTATION_DECL( SvxShape )

	// access methods for master objects
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL _getPropertySetInfo(  );
    void SAL_CALL _setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
	::com::sun::star::uno::Any SAL_CALL _getPropertyValue( const ::rtl::OUString& PropertyName );

    ::com::sun::star::beans::PropertyState SAL_CALL _getPropertyState( const ::rtl::OUString& PropertyName );
	void SAL_CALL _setPropertyToDefault( const ::rtl::OUString& PropertyName );
    ::com::sun::star::uno::Any SAL_CALL _getPropertyDefault( const ::rtl::OUString& aPropertyName );

    ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL _getSupportedServiceNames();

    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL _getTypes(  );

	void setMaster( SvxShapeMaster* pMaster );
	const SvxShapeMaster* getMaster() const;
	SvxShapeMaster* getMaster();

	// SfxListener
	virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint ) throw ();


	/** @obsolete
        not used anymore
	*/
	virtual void onUserCall(SdrUserCallType eUserCall, const Rectangle& rBoundRect);

	// XAggregation
    virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type& aType );

    // XNamed
    virtual ::rtl::OUString SAL_CALL getName(  );
    virtual void SAL_CALL setName( const ::rtl::OUString& aName );

    // XShapeDescriptor
    virtual ::rtl::OUString SAL_CALL getShapeType();

	// XShape
    virtual ::com::sun::star::awt::Point SAL_CALL getPosition();
    virtual void SAL_CALL setPosition( const ::com::sun::star::awt::Point& aPosition );
    virtual ::com::sun::star::awt::Size SAL_CALL getSize();
    virtual void SAL_CALL setSize( const ::com::sun::star::awt::Size& aSize );

	// XComponent
    virtual void SAL_CALL dispose();
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener );

	// XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

	// XMultiPropertySet
    virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames );
    virtual void SAL_CALL addPropertiesChangeListener( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
    virtual void SAL_CALL removePropertiesChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
    virtual void SAL_CALL firePropertiesChangeEvent( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );

	// XPropertyState
    virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState( const ::rtl::OUString& PropertyName );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL getPropertyStates( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyName );
	virtual void SAL_CALL setPropertyToDefault( const ::rtl::OUString& PropertyName );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault( const ::rtl::OUString& aPropertyName );

    // XMultiPropertyStates
    virtual void SAL_CALL setAllPropertiesToDefault();
    virtual void SAL_CALL setPropertiesToDefault(
        const ::com::sun::star::uno::Sequence<
        ::rtl::OUString >& aPropertyNames );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >
    SAL_CALL getPropertyDefaults(
        const ::com::sun::star::uno::Sequence<
        ::rtl::OUString >& aPropertyNames );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );

    // XGluePointsSupplier
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexContainer > SAL_CALL getGluePoints(  );

	// XChild
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL getParent(  );
    virtual void SAL_CALL setParent( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& Parent );

	// XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

    // XActionLockable
    virtual sal_Bool SAL_CALL isActionLocked(  );
    virtual void SAL_CALL addActionLock(  );
    virtual void SAL_CALL removeActionLock(  );
    virtual void SAL_CALL setActionLocks( sal_Int16 nLock );
    virtual sal_Int16 SAL_CALL resetActionLocks(  );

private:
    /** initializes SdrObj-dependent members. Only to be called when GetSdrObject() != NULL
    */
	SVX_DLLPRIVATE void	impl_initFromSdrObject();
    /// CTOR-Impl
    SVX_DLLPRIVATE void impl_construct();
};

#include <editeng/unotext.hxx>

class SVX_DLLPUBLIC SvxShapeText : public SvxShape, public SvxUnoTextBase
{
protected:
	/** called from the XActionLockable interface methods on initial locking */
	virtual void lock();

	/** called from the XActionLockable interface methods on final unlock */
	virtual void unlock();

protected:
	// overide these for special property handling in subcasses. Return true if property is handled
    virtual bool setPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, const ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyStateImpl( const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::beans::PropertyState& rState );
    virtual bool setPropertyToDefaultImpl( const SfxItemPropertySimpleEntry* pProperty );

public:
	SvxShapeText() throw ();
	SvxShapeText( SdrObject* pObj ) throw ();
    SvxShapeText( SdrObject* pObject, const SfxItemPropertyMapEntry* pPropertyMap, const SvxItemPropertySet* pPropertySet ) throw ();
	virtual ~SvxShapeText() throw ();

	virtual void Create( SdrObject* pNewOpj, SvxDrawPage* pNewPage = NULL );

	// XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType );
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL acquire() throw();
	virtual void SAL_CALL release() throw();

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );

	// ::com::sun::star::text::XTextRange
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL getStart();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL getEnd();
    virtual ::rtl::OUString SAL_CALL getString();
    virtual void SAL_CALL setString( const ::rtl::OUString& aString );

	// XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier );

	// XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );
};

class SvxShapeRect : public SvxShapeText
{
public:
	SvxShapeRect( SdrObject* pObj ) throw ();
	virtual ~SvxShapeRect() throw ();

	// XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType );
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL acquire() throw();
	virtual void SAL_CALL release() throw();

	// XServiceInfo
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};
#include <com/sun/star/drawing/XShapes.hpp>
#include <com/sun/star/drawing/XShapeGroup.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>

/***********************************************************************
*                                                                      *
***********************************************************************/
class SvxShapeGroup : public SvxShape,
					  public ::com::sun::star::drawing::XShapeGroup,
					  public ::com::sun::star::drawing::XShapes
{
private:
	rtl::Reference< SvxDrawPage> mxPage;

public:
	SvxShapeGroup( SdrObject* pObj,SvxDrawPage* pDrawPage ) throw ();
	virtual ~SvxShapeGroup() throw ();

	virtual void Create( SdrObject* pNewOpj, SvxDrawPage* pNewPage = NULL );

	// XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType );
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL acquire() throw();
	virtual void SAL_CALL release() throw();

	// XShapes
    virtual void SAL_CALL add( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape );
    virtual void SAL_CALL remove( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape );

	// XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
    virtual sal_Bool SAL_CALL hasElements();

	// XIndexAccess
    virtual sal_Int32 SAL_CALL getCount() ;
    virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

    // XShapeDescriptor
    virtual ::rtl::OUString SAL_CALL getShapeType();

	// XShape
    virtual ::com::sun::star::awt::Point SAL_CALL getPosition();
    virtual void SAL_CALL setPosition( const ::com::sun::star::awt::Point& aPosition );
    virtual ::com::sun::star::awt::Size SAL_CALL getSize();
    virtual void SAL_CALL setSize( const ::com::sun::star::awt::Size& aSize );

	// XShapeGroup
    virtual void SAL_CALL enterGroup(  );
    virtual void SAL_CALL leaveGroup(  );

	// XServiceInfo
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	// XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );
};
#include <com/sun/star/drawing/XConnectorShape.hpp>

/***********************************************************************
*                                                                      *
***********************************************************************/
class SvxShapeConnector : public ::com::sun::star::drawing::XConnectorShape,
						  public SvxShapeText
{
public:
	SvxShapeConnector( SdrObject* pObj ) throw();
	virtual ~SvxShapeConnector() throw();

	// XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType );
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL acquire() throw();
	virtual void SAL_CALL release() throw();

    // XShapeDescriptor
    virtual ::rtl::OUString SAL_CALL getShapeType();

	// XShape
    virtual ::com::sun::star::awt::Point SAL_CALL getPosition();
    virtual void SAL_CALL setPosition( const ::com::sun::star::awt::Point& aPosition );
    virtual ::com::sun::star::awt::Size SAL_CALL getSize();
    virtual void SAL_CALL setSize( const ::com::sun::star::awt::Size& aSize );

	// XConnectorShape
    virtual void SAL_CALL connectStart( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XConnectableShape >& xShape, ::com::sun::star::drawing::ConnectionType nPos );
    virtual void SAL_CALL connectEnd( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XConnectableShape >& xShape, ::com::sun::star::drawing::ConnectionType nPos );
    virtual void SAL_CALL disconnectBegin( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XConnectableShape >& xShape );
    virtual void SAL_CALL disconnectEnd( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XConnectableShape >& xShape );

	// XServiceInfo
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	// XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );
};
#include <com/sun/star/drawing/XControlShape.hpp>

/***********************************************************************
*                                                                      *
***********************************************************************/
class SVX_DLLPUBLIC SvxShapeControl : public ::com::sun::star::drawing::XControlShape, public SvxShapeText
{
protected:
    using SvxUnoTextRangeBase::setPropertyValue;
    using SvxUnoTextRangeBase::getPropertyValue;

public:
	SvxShapeControl( SdrObject* pObj ) throw();
	virtual ~SvxShapeControl() throw();

	// XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType );
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL acquire() throw();
	virtual void SAL_CALL release() throw();

	// XPropertySet
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );

	// XPropertyState
    virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState( const ::rtl::OUString& PropertyName );
	virtual void SAL_CALL setPropertyToDefault( const ::rtl::OUString& PropertyName );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault( const ::rtl::OUString& aPropertyName );

    // XShapeDescriptor
    virtual ::rtl::OUString SAL_CALL getShapeType();

	// XShape
    virtual ::com::sun::star::awt::Point SAL_CALL getPosition();
    virtual void SAL_CALL setPosition( const ::com::sun::star::awt::Point& aPosition );
    virtual ::com::sun::star::awt::Size SAL_CALL getSize();
    virtual void SAL_CALL setSize( const ::com::sun::star::awt::Size& aSize );

	// XControlShape
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel > SAL_CALL getControl();
    virtual void SAL_CALL setControl( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >& xControl );

	// XServiceInfo
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	// XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );
};

/***********************************************************************
*                                                                      *
***********************************************************************/
class SvxShapeDimensioning : public SvxShapeText
{
public:
	SvxShapeDimensioning( SdrObject* pObj ) throw();
	virtual ~SvxShapeDimensioning() throw();

	// XServiceInfo
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};

/***********************************************************************
*                                                                      *
***********************************************************************/
class SvxShapeCircle : public SvxShapeText
{
public:
	SvxShapeCircle( SdrObject* pObj ) throw ();
	virtual ~SvxShapeCircle() throw ();

	// XServiceInfo
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};

/***********************************************************************
*                                                                      *
***********************************************************************/

// #i118485# changed parent to SvxShapeText to allow Text handling over UNO API
class SVX_DLLPUBLIC SvxOle2Shape : public SvxShapeText
{
protected:
    // overide these for special property handling in subcasses. Return true if property is handled
    virtual bool setPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, const ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::uno::Any& rValue );

	void resetModifiedState();

	const SvGlobalName GetClassName_Impl(rtl::OUString& rHexCLSID);
public:
	SvxOle2Shape( SdrObject* pObj ) throw();
    SvxOle2Shape( SdrObject* pObject, const SfxItemPropertyMapEntry* pPropertyMap, const SvxItemPropertySet* pPropertySet ) throw ();
	virtual ~SvxOle2Shape() throw();

	sal_Bool createObject( const SvGlobalName &aClassName );

	sal_Bool createLink( const ::rtl::OUString& aLinkURL );

	// XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType );
};


#include <basegfx/polygon/b2dpolypolygon.hxx>

/***********************************************************************
*                                                                      *
***********************************************************************/
class SvxShapePolyPolygon : public SvxShapeText
{
private:
	::com::sun::star::drawing::PolygonKind mePolygonKind;

protected:
    using SvxUnoTextRangeBase::setPropertyValue;
    using SvxUnoTextRangeBase::getPropertyValue;

	// overide these for special property handling in subcasses. Return true if property is handled
    virtual bool setPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, const ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::uno::Any& rValue );

public:
	SvxShapePolyPolygon( SdrObject* pObj , ::com::sun::star::drawing::PolygonKind eNew = com::sun::star::drawing::PolygonKind_LINE );
	virtual ~SvxShapePolyPolygon() throw();

	// Local support functions
	::com::sun::star::drawing::PolygonKind GetPolygonKind() const throw();
	void SetPolygon(const basegfx::B2DPolyPolygon& rNew) throw();
	basegfx::B2DPolyPolygon GetPolygon() const throw();

	// XServiceInfo
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};

/***********************************************************************
*                                                                      *
***********************************************************************/

class SvxShapePolyPolygonBezier : public SvxShapeText
{
private:
	::com::sun::star::drawing::PolygonKind mePolygonKind;

protected:
    using SvxUnoTextRangeBase::setPropertyValue;
    using SvxUnoTextRangeBase::getPropertyValue;

public:
	// overide these for special property handling in subcasses. Return true if property is handled
    virtual bool setPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, const ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::uno::Any& rValue );

	SvxShapePolyPolygonBezier( SdrObject* pObj , ::com::sun::star::drawing::PolygonKind eNew = com::sun::star::drawing::PolygonKind_PATHLINE) throw();
	virtual ~SvxShapePolyPolygonBezier() throw();

	// Local support functions
	::com::sun::star::drawing::PolygonKind GetPolygonKind() const throw();
	void SetPolygon(const basegfx::B2DPolyPolygon & rNew) throw();
	basegfx::B2DPolyPolygon GetPolygon() const throw();

	// XServiceInfo
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};

/***********************************************************************
*                                                                      *
***********************************************************************/
class SvxGraphicObject : public SvxShapeText
{
protected:
    using SvxUnoTextRangeBase::setPropertyValue;
    using SvxUnoTextRangeBase::getPropertyValue;

	// overide these for special property handling in subcasses. Return true if property is handled
    virtual bool setPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, const ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::uno::Any& rValue );

public:
	SvxGraphicObject( SdrObject* pObj ) throw();
	virtual ~SvxGraphicObject() throw();
};

/***********************************************************************
*                                                                      *
***********************************************************************/
class Svx3DSceneObject : public ::com::sun::star::drawing::XShapes, public SvxShape
{
private:
	rtl::Reference< SvxDrawPage > mxPage;

protected:
    using SvxShape::setPropertyValue;
    using SvxShape::getPropertyValue;

public:
	Svx3DSceneObject( SdrObject* pObj, SvxDrawPage* pDrawPage ) throw();
	// overide these for special property handling in subcasses. Return true if property is handled
    virtual bool setPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, const ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::uno::Any& rValue );

	virtual ~Svx3DSceneObject() throw();

	virtual void Create( SdrObject* pNewOpj, SvxDrawPage* pNewPage = NULL );

	// XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType );
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL acquire() throw();
	virtual void SAL_CALL release() throw();

	// XShapes
    virtual void SAL_CALL add( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape );
    virtual void SAL_CALL remove( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >& xShape );

	// XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
    virtual sal_Bool SAL_CALL hasElements(  );

	// XIndexAccess
    virtual sal_Int32 SAL_CALL getCount(  ) ;
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

	// XServiceInfo
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	// XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );
};

/***********************************************************************
*                                                                      *
***********************************************************************/
class Svx3DCubeObject : public SvxShape
{
protected:
	// overide these for special property handling in subcasses. Return true if property is handled
    virtual bool setPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, const ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::uno::Any& rValue );

public:
	Svx3DCubeObject( SdrObject* pObj ) throw();
	virtual ~Svx3DCubeObject() throw();

	// XServiceInfo
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );
};

/***********************************************************************
*                                                                      *
***********************************************************************/
class Svx3DSphereObject : public SvxShape
{
public:
	Svx3DSphereObject( SdrObject* pObj ) throw();
protected:
	// overide these for special property handling in subcasses. Return true if property is handled
    virtual bool setPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, const ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::uno::Any& rValue );

	virtual ~Svx3DSphereObject() throw();

	// XServiceInfo
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );
};

/***********************************************************************
*                                                                      *
***********************************************************************/
class Svx3DLatheObject : public SvxShape
{
protected:
	// overide these for special property handling in subcasses. Return true if property is handled
    virtual bool setPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, const ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::uno::Any& rValue );

public:
	Svx3DLatheObject( SdrObject* pObj ) throw();
	virtual ~Svx3DLatheObject() throw();

	// XServiceInfo
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );
};

/***********************************************************************
*                                                                      *
***********************************************************************/
class Svx3DExtrudeObject : public SvxShape
{
public:
	Svx3DExtrudeObject( SdrObject* pObj ) throw();
protected:
	// overide these for special property handling in subcasses. Return true if property is handled
    virtual bool setPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, const ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::uno::Any& rValue );

	virtual ~Svx3DExtrudeObject() throw();

	// XServiceInfo
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );
};

/***********************************************************************
*                                                                      *
***********************************************************************/
class Svx3DPolygonObject : public SvxShape
{
protected:
	// overide these for special property handling in subcasses. Return true if property is handled
    virtual bool setPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, const ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::uno::Any& rValue );

public:
	Svx3DPolygonObject( SdrObject* pObj ) throw();
	virtual ~Svx3DPolygonObject() throw();

	// XServiceInfo
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );
};

/***********************************************************************
*                                                                      *
***********************************************************************/
typedef ::cppu::WeakAggImplHelper1<
    ::com::sun::star::drawing::XEnhancedCustomShapeDefaulter
	> SvxShape_UnoImplHelper1;

class SVX_DLLPUBLIC SvxCustomShape : public SvxShapeText, public SvxShape_UnoImplHelper1
{
private:
	rtl::Reference< SvxDrawPage > mxPage;

protected:
    using SvxUnoTextRangeBase::setPropertyValue;
    using SvxUnoTextRangeBase::getPropertyValue;

public:
	SvxCustomShape( SdrObject* pObj ) throw ();
	// overide these for special property handling in subcasses. Return true if property is handled
    //virtual bool setPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertyMapEntry* pProperty, const ::com::sun::star::uno::Any& rValue ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual bool getPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::uno::Any& rValue );


	virtual ~SvxCustomShape() throw ();

	virtual void Create( SdrObject* pNewOpj, SvxDrawPage* pNewPage = NULL );

	// XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType );
	virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType );
	virtual void SAL_CALL acquire() throw();
	virtual void SAL_CALL release() throw();

    // XShapeDescriptor
    virtual ::rtl::OUString SAL_CALL getShapeType();

	// XShape
    virtual ::com::sun::star::awt::Point SAL_CALL getPosition();
    virtual void SAL_CALL setPosition( const ::com::sun::star::awt::Point& aPosition );
    virtual ::com::sun::star::awt::Size SAL_CALL getSize();
    virtual void SAL_CALL setSize( const ::com::sun::star::awt::Size& aSize );

    // XPropertySet
    void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );

	// XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

	//XEnhancedCustomShapeDefaulter
	virtual void SAL_CALL createCustomShapeDefaults( const rtl::OUString& rShapeType );
};

/***********************************************************************
*                                                                      *
***********************************************************************/

class SvxMediaShape : public SvxShape
{
public:
	SvxMediaShape( SdrObject* pObj ) throw();
	virtual 	~SvxMediaShape() throw();

protected:
	// overide these for special property handling in subcasses. Return true if property is handled
    virtual bool setPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, const ::com::sun::star::uno::Any& rValue );
    virtual bool getPropertyValueImpl( const ::rtl::OUString& rName, const SfxItemPropertySimpleEntry* pProperty, ::com::sun::star::uno::Any& rValue );
};

#endif
