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



#ifndef _SD_STLSHEET_HXX
#define _SD_STLSHEET_HXX

#include <rtl/ref.hxx>

#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>

#include <cppuhelper/interfacecontainer.h>
#include <cppuhelper/implbase5.hxx>
#include <cppuhelper/basemutex.hxx>

#include <svl/style.hxx>

#include <editeng/unoipset.hxx>

#include <boost/scoped_ptr.hpp>

class ModifyListenerForewarder;

typedef cppu::ImplInheritanceHelper5< SfxUnoStyleSheet,
									::com::sun::star::beans::XPropertySet,
									::com::sun::star::lang::XServiceInfo,
									::com::sun::star::beans::XPropertyState,
									::com::sun::star::util::XModifyBroadcaster,
									::com::sun::star::lang::XComponent > SdStyleSheetBase ;

class SdStyleSheet : public SdStyleSheetBase, private ::cppu::BaseMutex
{
public:
	SdStyleSheet( const rtl::OUString& rDisplayName, SfxStyleSheetBasePool& rPool, SfxStyleFamily eFamily, sal_uInt16 nMask );
	SdStyleSheet( const SdStyleSheet& );

	virtual sal_Bool		SetParent (const String& rParentName);
	virtual SfxItemSet& GetItemSet();
	virtual sal_Bool		IsUsed() const;
	virtual sal_Bool		HasFollowSupport() const;
	virtual sal_Bool		HasParentSupport() const;
	virtual sal_Bool		HasClearParentSupport() const;
	virtual sal_Bool		SetName( const UniString& );
	virtual void		SetHelpId( const String& r, sal_uLong nId );

	void        AdjustToFontHeight(SfxItemSet& rSet, sal_Bool bOnlyMissingItems = sal_True);

	SdStyleSheet* GetRealStyleSheet() const;
	SdStyleSheet* GetPseudoStyleSheet() const;

	void SetApiName( const ::rtl::OUString& rApiName );
	rtl::OUString GetApiName() const;

	static rtl::OUString GetFamilyString( SfxStyleFamily eFamily );

	static SdStyleSheet* CreateEmptyUserStyle( SfxStyleSheetBasePool& rPool, SfxStyleFamily eFamily );

	// XInterface
    virtual void SAL_CALL release(  ) throw ();

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

	// XNamed
    virtual ::rtl::OUString SAL_CALL getName(  );
    virtual void SAL_CALL setName( const ::rtl::OUString& aName );

	// XStyle
    virtual sal_Bool SAL_CALL isUserDefined(  );
    virtual sal_Bool SAL_CALL isInUse(  );
    virtual ::rtl::OUString SAL_CALL getParentStyle(  );
    virtual void SAL_CALL setParentStyle( const ::rtl::OUString& aParentStyle );

	// XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo();
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

	// XPropertyState
    virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState( const ::rtl::OUString& PropertyName );
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL getPropertyStates( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyName );
    virtual void SAL_CALL setPropertyToDefault( const ::rtl::OUString& PropertyName );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault( const ::rtl::OUString& aPropertyName );

	// XModifyBroadcaster
	virtual void SAL_CALL addModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );
    virtual void SAL_CALL removeModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener );

	// XComponent
    virtual void SAL_CALL dispose(  );
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& xListener );
	virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener >& aListener );

	void notifyModifyListener();

protected:
	const SfxItemPropertySimpleEntry* getPropertyMapEntry( const ::rtl::OUString& rPropertyName ) const throw();

	virtual void Load (SvStream& rIn, sal_uInt16 nVersion);
	virtual void Store(SvStream& rOut);

	virtual void Notify(SfxBroadcaster& rBC, const SfxHint& rHint);
	virtual 			~SdStyleSheet();

	void throwIfDisposed();

	virtual void disposing();

	rtl::OUString	msApiName;
	rtl::Reference< SfxStyleSheetBasePool > mxPool;

    /** broadcast helper for events */
    ::cppu::OBroadcastHelper mrBHelper;

	boost::scoped_ptr< ModifyListenerForewarder > mpModifyListenerForewarder;

private:
	SdStyleSheet& operator=( const SdStyleSheet& );	// not implemented
};

typedef rtl::Reference< SdStyleSheet > SdStyleSheetRef;
typedef std::vector< SdStyleSheetRef > SdStyleSheetVector;

#endif	   // _SD_STLSHEET_HXX
