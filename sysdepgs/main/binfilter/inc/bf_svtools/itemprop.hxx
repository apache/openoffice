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


#ifndef _SFX_ITEMPROP_HXX
#define _SFX_ITEMPROP_HXX

#include <bf_svtools/bf_solar.h>
#include <tools/string.hxx>
#include <bf_svtools/itemset.hxx>
#include <cppuhelper/implbase1.hxx>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/beans/PropertyState.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>

namespace binfilter
{

/* -----------------------------21.02.00 11:03--------------------------------
	UNO III - Implementation
 ---------------------------------------------------------------------------*/
#define MAP_CHAR_LEN(cchar) cchar, sizeof(cchar) - 1
struct SfxItemPropertyMap
{
	const char* 						pName;
	USHORT 								nNameLen;
	USHORT								nWID;
	const ::com::sun::star::uno::Type*	pType;
	long								nFlags;
	BYTE								nMemberId;

	 static const SfxItemPropertyMap*	GetByName(
		const SfxItemPropertyMap *pMap,
		const ::rtl::OUString &rName );
};
/* -----------------------------21.02.00 11:19--------------------------------

 ---------------------------------------------------------------------------*/
class  SfxItemPropertySet
{
	const SfxItemPropertyMap* 	_pMap;
protected:
	virtual BOOL			FillItem(SfxItemSet& rSet, USHORT nWhich, BOOL bGetProperty) const;

public:
							SfxItemPropertySet( const SfxItemPropertyMap *pMap ) :
								_pMap(pMap) {}

	void getPropertyValue( const SfxItemPropertyMap& rMap,
											const SfxItemSet& rSet,
											::com::sun::star::uno::Any& rAny) const
											throw(::com::sun::star::uno::RuntimeException);
	::com::sun::star::uno::Any
		getPropertyValue( const SfxItemPropertyMap& rMap,
											const SfxItemSet& rSet ) const
											throw(::com::sun::star::uno::RuntimeException);
	void getPropertyValue( const ::rtl::OUString &rName,
											const SfxItemSet& rSet,
											::com::sun::star::uno::Any& rAny) const
											throw(::com::sun::star::uno::RuntimeException,
													::com::sun::star::beans::UnknownPropertyException);
	::com::sun::star::uno::Any
		getPropertyValue( const ::rtl::OUString &rName,
											const SfxItemSet& rSet ) const
											throw(::com::sun::star::uno::RuntimeException,
													::com::sun::star::beans::UnknownPropertyException);
	void					setPropertyValue( const SfxItemPropertyMap& rMap,
		const ::com::sun::star::uno::Any& aVal,
											SfxItemSet& rSet ) const
											throw(::com::sun::star::uno::RuntimeException,
											::com::sun::star::lang::IllegalArgumentException);
	void					setPropertyValue( const ::rtl::OUString& rPropertyName,
		const ::com::sun::star::uno::Any& aVal,
											SfxItemSet& rSet ) const
											throw(::com::sun::star::uno::RuntimeException,
											::com::sun::star::lang::IllegalArgumentException,
													::com::sun::star::beans::UnknownPropertyException);

	::com::sun::star::beans::PropertyState
		getPropertyState(const ::rtl::OUString& rName, const SfxItemSet& rSet)
		throw(::com::sun::star::beans::UnknownPropertyException);
	::com::sun::star::beans::PropertyState
        getPropertyState(const SfxItemPropertyMap& rMap, const SfxItemSet& rSet)
                                    throw();

	::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo >
		getPropertySetInfo() const;
	const SfxItemPropertyMap*
		getPropertyMap() const {return _pMap;}

};

class  SfxItemPropertySetInfo : public cppu::WeakImplHelper1< ::com::sun::star::beans::XPropertySetInfo>
{
	const SfxItemPropertyMap* 	_pMap;

public:
	SfxItemPropertySetInfo(const SfxItemPropertyMap *pMap ) :
								_pMap(pMap) {}

    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property > SAL_CALL
		getProperties(  )
			throw(::com::sun::star::uno::RuntimeException);

	virtual ::com::sun::star::beans::Property SAL_CALL
		getPropertyByName( const ::rtl::OUString& aName )
			throw(::com::sun::star::beans::UnknownPropertyException,
					::com::sun::star::uno::RuntimeException);

	virtual sal_Bool SAL_CALL
		hasPropertyByName( const ::rtl::OUString& Name )
			throw(::com::sun::star::uno::RuntimeException);

	const SfxItemPropertyMap* getMap() const { return _pMap; }

    // XInterface
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL acquire(  ) throw ();
    virtual void SAL_CALL release(  ) throw ();
};
/* -----------------------------21.02.00 12:01--------------------------------

 ---------------------------------------------------------------------------*/
class  SfxExtItemPropertySetInfo: public cppu::WeakImplHelper1< ::com::sun::star::beans::XPropertySetInfo >
{
	const SfxItemPropertyMap* 											_pExtMap;
	::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property>		aPropertySeq;
public:
							SfxExtItemPropertySetInfo(
								const SfxItemPropertyMap *pMap,
								const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property>& rPropSeq );

    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property > SAL_CALL
		getProperties(  )
			throw(::com::sun::star::uno::RuntimeException);

	virtual ::com::sun::star::beans::Property SAL_CALL
		getPropertyByName( const ::rtl::OUString& aName )
			throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);

	virtual sal_Bool SAL_CALL
		hasPropertyByName( const ::rtl::OUString& Name )
			throw(::com::sun::star::uno::RuntimeException);

    // XInterface
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL acquire(  ) throw ();
    virtual void SAL_CALL release(  ) throw ();
};

}

#endif
