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


#ifndef _UNOFIELD_HXX
#define _UNOFIELD_HXX

#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/util/XRefreshable.hpp>
#include <com/sun/star/util/XUpdatable.hpp>
#include <com/sun/star/text/XDependentTextField.hpp>

#include <cppuhelper/implbase5.hxx>

#include <calbck.hxx>
#include <unocoll.hxx>
#include <RefreshListenerContainer.hxx>


class SwFieldType;
class SwDoc;
class SwFmtFld;
class SwField;
class SwSetExpField;
class SwTextAPIObject;

/* -----------------04.12.98 12:49-------------------
 *
 * --------------------------------------------------*/
class SwXFieldMaster : public cppu::WeakImplHelper4
<
	::com::sun::star::beans::XPropertySet,
	::com::sun::star::lang::XServiceInfo,
	::com::sun::star::lang::XUnoTunnel,
	::com::sun::star::lang::XComponent
>,
	public SwClient
{
	SwEventListenerContainer	aLstnrCntnr;
    sal_uInt16                  nResTypeId;
	SwDoc* 						m_pDoc;

    sal_Bool                    m_bIsDescriptor;

	String 						sParam1;	//Content / Database / NumberingSeparator
	String 						sParam2;    // -	/DataTablename
	String 						sParam3;    // -	/DataFieldName
    String                      sParam4;
    String                      sParam5;    // -    /DataBaseURL
    String                      sParam6;    // -    /DataBaseResource
	double						fParam1;	// Value / -
	sal_Int8					nParam1;	// ChapterNumberingLevel
	sal_Bool					bParam1;    // IsExpression
    sal_Int32                   nParam2;

protected:
	virtual ~SwXFieldMaster();
	//SwClient
virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);

public:

	SwXFieldMaster(SwDoc* pDoc, sal_uInt16 nResId);
	SwXFieldMaster(SwFieldType& rType, SwDoc* pDoc);


	TYPEINFO();

	static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();

	//XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier );

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

	//XComponent
	virtual void SAL_CALL dispose(void);
	virtual void SAL_CALL addEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener);
	virtual void SAL_CALL removeEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener);

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

	static rtl::OUString GetProgrammaticName(const SwFieldType& rType, SwDoc& rDoc);
	static rtl::OUString LocalizeFormula(const SwSetExpField& rFld, const rtl::OUString& rFormula, sal_Bool bQuery);

    SwFieldType* GetFldType(sal_Bool bDontCreate = sal_False) const;
	SwDoc*		GetDoc() {return m_pDoc;}

	sal_Bool		IsDescriptor()const  {return m_bIsDescriptor;}
	sal_uInt16 		GetResId() const {return nResTypeId;}
};

struct SwFieldProperties_Impl;
class SwXTextField : public cppu::WeakImplHelper5
<
	::com::sun::star::text::XDependentTextField,
	::com::sun::star::lang::XServiceInfo,
	::com::sun::star::beans::XPropertySet,
    ::com::sun::star::lang::XUnoTunnel,
    ::com::sun::star::util::XUpdatable
>,
			public SwClient
{
	SwEventListenerContainer	m_aLstnrCntnr;
	const SwFmtFld* 			m_pFmtFld;
	SwDoc* 						m_pDoc;
	SwTextAPIObject*			m_pTextObject;

	sal_Bool 						m_bIsDescriptor;
    SwClient                        m_aFieldTypeClient; // required to access field master of not yet inserted fields
    sal_Bool                        m_bCallUpdate;
    sal_uInt16                      m_nServiceId;
	SwFieldProperties_Impl* 	m_pProps;
	String 						m_sTypeName;


	//SwFieldType* GetFldType() const { return (SwFieldType*)GetRegisteredIn(); }
	SwDoc* 		 GetDoc() {return m_pDoc;}
protected:
	virtual ~SwXTextField();
    //SwClient
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);

public:
	SwXTextField(sal_uInt16 nServiceId, SwDoc* pDoc=0);
	SwXTextField(const SwFmtFld& rFmt, SwDoc* pDoc);


	TYPEINFO();

	static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();

	//XUnoTunnel
	virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier );

	//XDependentTextField
	virtual void SAL_CALL attachTextFieldMaster(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > & xFieldMaster);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >  SAL_CALL getTextFieldMaster(void);

	//XTextField
	virtual rtl::OUString SAL_CALL getPresentation(sal_Bool bShowCommand);

	//XTextContent
	virtual void SAL_CALL attach(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & xTextRange);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange >  SAL_CALL getAnchor(void);

	//XComponent
	virtual void SAL_CALL dispose(void);
	virtual void SAL_CALL addEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener);
	virtual void SAL_CALL removeEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener);

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

    //XUpdatable
    virtual void SAL_CALL update(  );

	void attachToRange(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & xTextRange);

	const SwField*  	GetField() const;
	const SwFmtFld* 	GetFldFmt(){return GetField() ? m_pFmtFld : 0; };

	void Invalidate();

    /// @return an SwXTextField, either an already existing one or a new one
    static SwXTextField* CreateSwXTextField(SwDoc & rDoc, SwFmtFld const& rFmt);
    sal_uInt16 GetServiceId();
};


typedef
cppu::WeakImplHelper2
<
	::com::sun::star::container::XNameAccess,
	::com::sun::star::lang::XServiceInfo
>
SwXTextFieldMastersBaseClass;
class SwXTextFieldMasters : public SwXTextFieldMastersBaseClass,
	public SwUnoCollection
{
protected:
	virtual ~SwXTextFieldMasters();
public:
	SwXTextFieldMasters(SwDoc* pDoc);


	//XNameAccess
	virtual ::com::sun::star::uno::Any SAL_CALL getByName(const rtl::OUString& Name);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getElementNames(void);
	virtual sal_Bool SAL_CALL hasByName(const rtl::OUString& Name);

	//XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
    virtual sal_Bool SAL_CALL hasElements(  );

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);


	static sal_Bool getInstanceName(const SwFieldType& rFldType, String& rName);
};

/* -----------------21.12.98 10:26-------------------
 *
 * --------------------------------------------------*/
typedef
cppu::WeakImplHelper3
<
	::com::sun::star::container::XEnumerationAccess,
	::com::sun::star::lang::XServiceInfo,
	::com::sun::star::util::XRefreshable
>
SwXTextFieldTypesBaseClass;
class SwXTextFieldTypes : public SwXTextFieldTypesBaseClass,
		public SwUnoCollection
{
    SwRefreshListenerContainer      aRefreshCont;
protected:
	virtual ~SwXTextFieldTypes();
public:
	SwXTextFieldTypes(SwDoc* pDoc);


	//XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration >  SAL_CALL createEnumeration(void);

	//XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
    virtual sal_Bool SAL_CALL hasElements(  );

	// ::com::sun::star::util::XRefreshable
    virtual void SAL_CALL refresh(  );
    virtual void SAL_CALL addRefreshListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XRefreshListener >& l );
    virtual void SAL_CALL removeRefreshListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XRefreshListener >& l );

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

    // SwUnoCollection
    virtual void    Invalidate();
};

class SwXFieldEnumeration : public cppu::WeakImplHelper2
<
	::com::sun::star::container::XEnumeration,
	::com::sun::star::lang::XServiceInfo
>,
	public SwClient
{
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextField > > aItems;
    sal_Int32       nNextIndex;     // index of next element to be returned

	SwDoc* pDoc;

protected:
	virtual ~SwXFieldEnumeration();
	//SwClient
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);
public:
	SwXFieldEnumeration(SwDoc* pDoc);

	//XEnumeration
	virtual sal_Bool SAL_CALL hasMoreElements(void);
	virtual ::com::sun::star::uno::Any SAL_CALL nextElement(void);

	//XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName(void);
	virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName);
	virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames(void);

};
#endif
