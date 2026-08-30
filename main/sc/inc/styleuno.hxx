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



#ifndef SC_STYLEUNO_HXX
#define SC_STYLEUNO_HXX

#include <svl/itemprop.hxx>
#include <svl/lstner.hxx>
#include <rsc/rscsfx.hxx>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/style/XStyleLoader.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/container/XIndexReplace.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/beans/XMultiPropertyStates.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
//#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase4.hxx>
#include <cppuhelper/implbase7.hxx>

class SfxStyleSheetBase;
class ScDocShell;

class ScStyleFamilyObj;
class ScStyleObj;


class ScStyleNameConversion
{
public:
	static String DisplayToProgrammaticName( const String& rDispName, sal_uInt16 nType );
	static String ProgrammaticToDisplayName( const String& rProgName, sal_uInt16 nType );
};


class ScStyleFamiliesObj : public ::cppu::WeakImplHelper4<
							::com::sun::star::container::XIndexAccess,
							::com::sun::star::container::XNameAccess,
							::com::sun::star::style::XStyleLoader,
                            ::com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*				pDocShell;

	ScStyleFamilyObj*		GetObjectByType_Impl(sal_uInt16 Type) const;
	ScStyleFamilyObj*		GetObjectByIndex_Impl(sal_uInt32 nIndex) const;
	ScStyleFamilyObj*		GetObjectByName_Impl(const rtl::OUString& aName) const;

public:
							ScStyleFamiliesObj(ScDocShell* pDocSh);
	virtual					~ScStyleFamiliesObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XIndexAccess
	virtual sal_Int32 SAL_CALL getCount();
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 Index );

							// XNameAccess
	virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames();
	virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName );

							// XElementAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType();
	virtual sal_Bool SAL_CALL hasElements();

							// XStyleLoader
	virtual void SAL_CALL	loadStylesFromURL( const ::rtl::OUString& URL,
								const ::com::sun::star::uno::Sequence<
									::com::sun::star::beans::PropertyValue >& aOptions );
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL
							getStyleLoaderOptions();

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class ScStyleFamilyObj : public ::cppu::WeakImplHelper4<
							::com::sun::star::container::XNameContainer,
                            ::com::sun::star::container::XIndexAccess,
                            ::com::sun::star::beans::XPropertySet,
							::com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*				pDocShell;
	SfxStyleFamily 			eFamily;		// Familie

	ScStyleObj*				GetObjectByIndex_Impl(sal_uInt32 nIndex);
	ScStyleObj*				GetObjectByName_Impl(const rtl::OUString& Name);

public:
							ScStyleFamilyObj(ScDocShell* pDocSh, SfxStyleFamily eFam);
	virtual					~ScStyleFamilyObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

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

                            // XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo();
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

                            // XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();
};


class ScStyleObj : public ::cppu::WeakImplHelper7<
					::com::sun::star::style::XStyle,
					::com::sun::star::beans::XPropertySet,
					::com::sun::star::beans::XMultiPropertySet,
					::com::sun::star::beans::XPropertyState,
					::com::sun::star::beans::XMultiPropertyStates,
					::com::sun::star::lang::XUnoTunnel,
					::com::sun::star::lang::XServiceInfo >,
				public SfxListener
{
private:
	const SfxItemPropertySet* pPropSet;
	ScDocShell*				pDocShell;
	SfxStyleFamily 			eFamily;		// Familie
	String 					aStyleName;

	SfxStyleSheetBase*		GetStyle_Impl();
    const SfxItemSet*       GetStyleItemSet_Impl( const ::rtl::OUString& rPropName, const SfxItemPropertySimpleEntry*& rpEntry );
    void                    SetOnePropertyValue(    const ::rtl::OUString& rPropertyName,
                                                    const SfxItemPropertySimpleEntry* pEntry,
													const ::com::sun::star::uno::Any* pValue );

    // enhance ODS load performance
	bool StyleSheetChangedUpdate();
	void SetOnePropertyValueWithoutUpdate(  const ::rtl::OUString& rPropertyName,
													const SfxItemPropertySimpleEntry* pEntry,
													const ::com::sun::star::uno::Any* pValue );
	//end add

	ScStyleObj(); // disabled
public:
							ScStyleObj(ScDocShell* pDocSh, SfxStyleFamily eFam, const String& rName);
	virtual					~ScStyleObj();

							// per getImplementation gerufen:
	sal_Bool				IsInserted() const		{ return pDocShell != NULL; }
	SfxStyleFamily 			GetFamily() const 		{ return eFamily; }
	void					InitDoc( ScDocShell* pNewDocSh, const String& rNewName );

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

	static ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexReplace >
							CreateEmptyNumberingRules();

							// XStyle
	virtual sal_Bool SAL_CALL isUserDefined();
	virtual sal_Bool SAL_CALL isInUse();
	virtual ::rtl::OUString	SAL_CALL getParentStyle();
	virtual void SAL_CALL	setParentStyle( const ::rtl::OUString& aParentStyle );

							// XNamed
	virtual ::rtl::OUString SAL_CALL getName();
	virtual void SAL_CALL	setName( const ::rtl::OUString& aName );

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

							// XPropertyState
	virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState(
									const ::rtl::OUString& PropertyName );
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL
							getPropertyStates( const ::com::sun::star::uno::Sequence<
										::rtl::OUString >& aPropertyName );
	virtual void SAL_CALL	setPropertyToDefault( const ::rtl::OUString& PropertyName );
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault(
									const ::rtl::OUString& aPropertyName );

							// XMultiPropertyStates
	// getPropertyStates already defined for XPropertyState
	virtual void SAL_CALL	setAllPropertiesToDefault();
	virtual void SAL_CALL	setPropertiesToDefault( const ::com::sun::star::uno::Sequence<
										::rtl::OUString >& aPropertyNames );
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL
							getPropertyDefaults( const ::com::sun::star::uno::Sequence<
										::rtl::OUString >& aPropertyNames );

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName();
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames();

							// XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence<
									sal_Int8 >& aIdentifier );

	static const ::com::sun::star::uno::Sequence<sal_Int8>& getUnoTunnelId();
	static ScStyleObj* getImplementation( const ::com::sun::star::uno::Reference<
									::com::sun::star::uno::XInterface> xObj );

};

#endif
