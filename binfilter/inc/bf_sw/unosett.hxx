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


#ifndef _UNOSETT_HXX
#define _UNOSETT_HXX

#include <bf_svtools/bf_solar.h>

#include <swtypes.hxx>
#ifndef _CALBCK_HXX //autogen
#include <calbck.hxx>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTCOLUMNS_HPP_
#include <com/sun/star/text/XTextColumns.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXREPLACE_HPP_
#include <com/sun/star/container/XIndexReplace.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMED_HPP_
#include <com/sun/star/container/XNamed.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XUNOTUNNEL_HPP_
#include <com/sun/star/lang/XUnoTunnel.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE2_HXX_
#include <cppuhelper/implbase2.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE4_HXX_
#include <cppuhelper/implbase4.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE5_HXX_
#include <cppuhelper/implbase5.hxx>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
namespace binfilter {
struct SfxItemPropertyMap; 

class SwDoc;
class SwFmtCol;
class SwDocShell;
class SwNumRule;


/******************************************************************************
 *
 ******************************************************************************/
/* -----------------04.05.98 12:28-------------------
 *
 * --------------------------------------------------*/
class SwXFootnoteProperties : public cppu::WeakAggImplHelper2
<
	::com::sun::star::beans::XPropertySet,
	::com::sun::star::lang::XServiceInfo
>
{
	SwDoc* 						pDoc;
	const SfxItemPropertyMap*	_pMap;
protected:
	virtual ~SwXFootnoteProperties();
public:
	SwXFootnoteProperties(SwDoc* pDoc);
	

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

	//XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(void) throw( ::com::sun::star::uno::RuntimeException );
	virtual BOOL SAL_CALL supportsService(const ::rtl::OUString& ServiceName) throw( ::com::sun::star::uno::RuntimeException );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(void) throw( ::com::sun::star::uno::RuntimeException );

	void			Invalidate() {pDoc = 0;}
};

/* -----------------04.05.98 12:28-------------------
 *
 * --------------------------------------------------*/
class SwXEndnoteProperties : public cppu::WeakAggImplHelper2
<
	::com::sun::star::beans::XPropertySet,
	::com::sun::star::lang::XServiceInfo
>
{
	SwDoc* 						pDoc;
	const SfxItemPropertyMap*	_pMap;
protected:
	virtual ~SwXEndnoteProperties();
public:
	SwXEndnoteProperties(SwDoc* pDoc);
	

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

	//XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(void) throw( ::com::sun::star::uno::RuntimeException );
	virtual BOOL SAL_CALL supportsService(const ::rtl::OUString& ServiceName) throw( ::com::sun::star::uno::RuntimeException );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(void) throw( ::com::sun::star::uno::RuntimeException );

	void			Invalidate() {pDoc = 0;}
};

/* -----------------04.05.98 12:28-------------------
 *
 * --------------------------------------------------*/
class SwXLineNumberingProperties : public cppu::WeakAggImplHelper2
<
	::com::sun::star::beans::XPropertySet,
	::com::sun::star::lang::XServiceInfo
>
{
	SwDoc* 						pDoc;
	const SfxItemPropertyMap*	_pMap;
protected:
	virtual ~SwXLineNumberingProperties();
public:
	SwXLineNumberingProperties(SwDoc* pDoc);
	

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

	//XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(void) throw( ::com::sun::star::uno::RuntimeException );
	virtual BOOL SAL_CALL supportsService(const ::rtl::OUString& ServiceName) throw( ::com::sun::star::uno::RuntimeException );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(void) throw( ::com::sun::star::uno::RuntimeException );

	void			Invalidate() {pDoc = 0;}
};

/* -----------------25.05.98 08:21-------------------
 *
 * --------------------------------------------------*/
class SwXNumberingRules : public cppu::WeakAggImplHelper5
<
	::com::sun::star::container::XIndexReplace,
	::com::sun::star::lang::XUnoTunnel,
	::com::sun::star::beans::XPropertySet,
	::com::sun::star::container::XNamed,
	::com::sun::star::lang::XServiceInfo
>,
	public SwClient
{
	String 						sNewCharStyleNames[MAXLEVEL];
	String 						sNewBulletFontNames[MAXLEVEL];
	String 						sCreatedNumRuleName; //connects to a numbering in SwDoc
	SwDoc*						pDoc; // wird nur fuers anmelden gebraucht
	SwDocShell*					pDocShell; //nur, wenn als ChapterNumbering verwendet
	SwNumRule*					pNumRule;
	const SfxItemPropertyMap* 	_pMap;
	sal_Bool					bOwnNumRuleCreated;
	static String				sInvalidStyle;
protected:
	virtual ~SwXNumberingRules();
public:
	SwXNumberingRules(SwDocShell& rDocSh);	// chapter numbering
	SwXNumberingRules(const SwNumRule& rRule); // NumRule for paragraphs, numbering styles
	SwXNumberingRules(SwDoc& rDoc); //create a new instance
	

	static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();

	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);


	//XIndexReplace
    virtual void SAL_CALL replaceByIndex( sal_Int32 Index, const ::com::sun::star::uno::Any& Element ) throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

	//XIndexAccess
    virtual sal_Int32 SAL_CALL getCount(  ) throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex(sal_Int32 nIndex) throw( ::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException );

	//XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL hasElements(  ) throw(::com::sun::star::uno::RuntimeException);

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

	//XNamed
	virtual ::rtl::OUString SAL_CALL getName(void) throw( ::com::sun::star::uno::RuntimeException );
	virtual void SAL_CALL setName(const ::rtl::OUString& Name_) throw( ::com::sun::star::uno::RuntimeException );

	//XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(void) throw( ::com::sun::star::uno::RuntimeException );
	virtual BOOL SAL_CALL supportsService(const ::rtl::OUString& ServiceName) throw( ::com::sun::star::uno::RuntimeException );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(void) throw( ::com::sun::star::uno::RuntimeException );

	::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue> getNumberingRuleByIndex(const SwNumRule& rNumRule, sal_Int32 nIndex)const;
	void setNumberingRuleByIndex(SwNumRule& rNumRule,
								const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue>& rProperties, sal_Int32 nIndex)
								throw( ::com::sun::star::uno::RuntimeException, ::com::sun::star::lang::IllegalArgumentException );

	//SwClient
	virtual void Modify( SfxPoolItem *pOld, SfxPoolItem *pNew);

	const String*			GetNewCharStyleNames() const {return sNewCharStyleNames;}
	const String*			GetBulletFontNames() const {return sNewBulletFontNames;}
	const SwNumRule*		GetNumRule() {return pNumRule;}

	static const String&	GetInvalidStyle();
	void	Invalidate() 	{pDocShell = 0;}
	const String& 			GetCreatedNumRuleName() const{return sCreatedNumRuleName; }
};
/*-----------------12.02.98 08:27-------------------

--------------------------------------------------*/
class SwXChapterNumbering : public SwXNumberingRules
{
protected:
	virtual ~SwXChapterNumbering();
public:
	SwXChapterNumbering(SwDocShell& rDocSh);
	

	void	Invalidate() {SwXNumberingRules::Invalidate();}

	//XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(void) throw( ::com::sun::star::uno::RuntimeException );
	virtual BOOL SAL_CALL supportsService(const ::rtl::OUString& ServiceName) throw( ::com::sun::star::uno::RuntimeException );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(void) throw( ::com::sun::star::uno::RuntimeException );

};

/* -----------------27.05.98 15:44-------------------
 *
 * --------------------------------------------------*/
class SwXTextColumns : public cppu::WeakAggImplHelper4
<

	::com::sun::star::lang::XUnoTunnel,
	::com::sun::star::beans::XPropertySet,
	::com::sun::star::text::XTextColumns,
	::com::sun::star::lang::XServiceInfo
>
{
	sal_uInt32					nReference;
	::com::sun::star::uno::Sequence< ::com::sun::star::text::TextColumn>	aTextColumns;
    sal_Bool                    bIsAutomaticWidth;
    sal_Int32                   nAutoDistance;

	const SfxItemPropertyMap*	_pMap;

	//separator line
	sal_Int32 					nSepLineWidth;
	sal_Int32 					nSepLineColor;
	sal_Int8  					nSepLineHeightRelative;
	sal_Int8					nSepLineVertAlign;//style::VerticalAlignment
	sal_Bool					bSepLineIsOn;
protected:
	virtual ~SwXTextColumns();
public:
	SwXTextColumns(sal_uInt16 nColCount);
	SwXTextColumns(const SwFmtCol& rFmtCol);
	


	static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();

	//XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);

	//XTextColumns
    virtual sal_Int32 SAL_CALL getReferenceValue(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Int16 SAL_CALL getColumnCount(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setColumnCount( sal_Int16 nColumns ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::text::TextColumn > SAL_CALL getColumns(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setColumns( const ::com::sun::star::uno::Sequence< ::com::sun::star::text::TextColumn >& Columns ) throw(::com::sun::star::uno::RuntimeException);

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

	//XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(void) throw( ::com::sun::star::uno::RuntimeException );
	virtual BOOL SAL_CALL supportsService(const ::rtl::OUString& ServiceName) throw( ::com::sun::star::uno::RuntimeException );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(void) throw( ::com::sun::star::uno::RuntimeException );

	sal_Int32 	GetSepLineWidth() const {return nSepLineWidth;}
	sal_Int32 	GetSepLineColor() const {return 	nSepLineColor;}
	sal_Int8  	GetSepLineHeightRelative() const {return 	nSepLineHeightRelative;}
	sal_Int8	GetSepLineVertAlign() const {return 	nSepLineVertAlign;}
	sal_Bool	GetSepLineIsOn() const {return 	bSepLineIsOn;}

    sal_Bool    IsAutomaticWidth() const {return bIsAutomaticWidth;}
};
} //namespace binfilter
#endif


