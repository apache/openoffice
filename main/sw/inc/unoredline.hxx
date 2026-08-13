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


#ifndef _UNOREDLINE_HXX
#define _UNOREDLINE_HXX

#include <unoport.hxx>
#include <unotext.hxx>


class SwRedline;

/* -----------------------------19.12.00 11:35--------------------------------

 ---------------------------------------------------------------------------*/

/**
 * SwXRedlineText provides an XText which may be used to write
 * directly into a redline node. It got implemented to enable XML
 * import of redlines and should not be used directly via the API.
 */
class SwXRedlineText :
	public SwXText,
	public cppu::OWeakObject,
	public ::com::sun::star::container::XEnumerationAccess
{
	SwNodeIndex aNodeIndex;
	virtual const SwStartNode *GetStartNode() const;

public:
	SwXRedlineText(SwDoc* pDoc, SwNodeIndex aNodeIndex);

    virtual 	::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType );
    virtual void SAL_CALL acquire(  ) throw(){OWeakObject::acquire();}
    virtual void SAL_CALL release(  ) throw(){OWeakObject::release();}

	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

	//XText
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor >  SAL_CALL createTextCursor(void);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor >  SAL_CALL createTextCursorByRange(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & aTextPosition);

	//XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration >  SAL_CALL createEnumeration(void);

	//XElementAccess (via XEnumerationAccess)
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
    virtual sal_Bool SAL_CALL hasElements(  );
};
/* ---------------------------------------------------------------------------

 ---------------------------------------------------------------------------*/
class SwXRedlinePortion : public SwXTextPortion
{
	const SwRedline*	pRedline;

	void Validate();

    using SwXTextPortion::GetPropertyValue;

public:
	SwXRedlinePortion(	const SwRedline* pRed,
						const SwUnoCrsr* pPortionCrsr,
						::com::sun::star::uno::Reference< ::com::sun::star::text::XText >  xParent,
						sal_Bool bIsStart);

	~SwXRedlinePortion();

    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

	//XPropertySet
	virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );

	static ::com::sun::star::uno::Any  GetPropertyValue( const ::rtl::OUString& PropertyName, const SwRedline& rRedline ) throw();
    static ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > CreateRedlineProperties( const SwRedline& rRedline, sal_Bool bIsStart ) throw();

};
/* -----------------------------11.01.01 16:52--------------------------------

 ---------------------------------------------------------------------------*/
typedef
cppu::WeakImplHelper1
<
	::com::sun::star::container::XEnumerationAccess
>
SwXRedlineBaseClass;
class SwXRedline :
		public SwXRedlineBaseClass,
		public SwXText,
		public SwClient
{
	SwDoc* 		pDoc;
	SwRedline* 	pRedline;
public:
	SwXRedline(SwRedline& rRedline, SwDoc& rDoc);
	~SwXRedline();

	TYPEINFO();

	virtual 	::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType );
    virtual void SAL_CALL acquire(  ) throw(){OWeakObject::acquire();}
    virtual void SAL_CALL release(  ) throw(){OWeakObject::release();}

	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes(  );
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId(  );

	//XText
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor >  SAL_CALL createTextCursor(void);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor >  SAL_CALL createTextCursorByRange(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & aTextPosition);

	//XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

	//XEnumerationAccess
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration >  SAL_CALL createEnumeration(void);

	//XElementAccess
    virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  );
    virtual sal_Bool SAL_CALL hasElements(  );

	const SwRedline* 	GetRedline() const {return pRedline;}
protected:
	//SwClient
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);
};
#endif
