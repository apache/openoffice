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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_xmloff.hxx"
#include <com/sun/star/beans/XPropertyState.hpp>
#include "PropertySetMerger.hxx"

using ::rtl::OUString;

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;

#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase3.hxx>
#endif

class SvXMLAttrContainerItem_Impl;

class PropertySetMergerImpl : public ::cppu::WeakAggImplHelper3< XPropertySet, XPropertyState, XPropertySetInfo >
{
private:
	Reference< XPropertySet > mxPropSet1;
	Reference< XPropertyState > mxPropSet1State;
	Reference< XPropertySetInfo > mxPropSet1Info;

	Reference< XPropertySet > mxPropSet2;
	Reference< XPropertyState > mxPropSet2State;
	Reference< XPropertySetInfo > mxPropSet2Info;

public:
	PropertySetMergerImpl( const Reference< XPropertySet > rPropSet1, const Reference< XPropertySet > rPropSet2 );
	virtual ~PropertySetMergerImpl();

    // XPropertySet
    virtual Reference< XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
    virtual void SAL_CALL setPropertyValue( const OUString& aPropertyName, const Any& aValue );
    virtual Any SAL_CALL getPropertyValue( const OUString& PropertyName );
    virtual void SAL_CALL addPropertyChangeListener( const OUString& aPropertyName, const Reference< XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const OUString& aPropertyName, const Reference< XPropertyChangeListener >& aListener );
    virtual void SAL_CALL addVetoableChangeListener( const OUString& PropertyName, const Reference< XVetoableChangeListener >& aListener );
    virtual void SAL_CALL removeVetoableChangeListener( const OUString& PropertyName, const Reference< XVetoableChangeListener >& aListener );

    // XPropertyState
    virtual PropertyState SAL_CALL getPropertyState( const OUString& PropertyName );
    virtual Sequence< PropertyState > SAL_CALL getPropertyStates( const Sequence< OUString >& aPropertyName );
    virtual void SAL_CALL setPropertyToDefault( const OUString& PropertyName );
    virtual Any SAL_CALL getPropertyDefault( const OUString& aPropertyName );

    // XPropertySetInfo
    virtual Sequence< Property > SAL_CALL getProperties(  );
    virtual Property SAL_CALL getPropertyByName( const OUString& aName );
    virtual sal_Bool SAL_CALL hasPropertyByName( const OUString& Name );
};

// --------------------------------------------------------------------
// Interface implementation
// --------------------------------------------------------------------

PropertySetMergerImpl::PropertySetMergerImpl( Reference< XPropertySet > rPropSet1, Reference< XPropertySet > rPropSet2 )
: mxPropSet1( rPropSet1 )
, mxPropSet1State( rPropSet1, UNO_QUERY )
, mxPropSet1Info( rPropSet1->getPropertySetInfo() )
, mxPropSet2( rPropSet2 )
, mxPropSet2State( rPropSet2, UNO_QUERY )
, mxPropSet2Info( rPropSet2->getPropertySetInfo() )
{
}

PropertySetMergerImpl::~PropertySetMergerImpl()
{
}

// XPropertySet
Reference< XPropertySetInfo > SAL_CALL PropertySetMergerImpl::getPropertySetInfo(  )
{
	return this;
}

void SAL_CALL PropertySetMergerImpl::setPropertyValue( const OUString& aPropertyName, const Any& aValue )
{
	if( mxPropSet1Info->hasPropertyByName( aPropertyName ) )
	{
		mxPropSet1->setPropertyValue( aPropertyName, aValue );
	}
	else
	{
		mxPropSet2->setPropertyValue( aPropertyName, aValue );
	}
}

Any SAL_CALL PropertySetMergerImpl::getPropertyValue( const OUString& PropertyName )
{
	if( mxPropSet1Info->hasPropertyByName( PropertyName ) )
	{
		return mxPropSet1->getPropertyValue( PropertyName );
	}
	else
	{
		return mxPropSet2->getPropertyValue( PropertyName );
	}
}

void SAL_CALL PropertySetMergerImpl::addPropertyChangeListener( const OUString& /*aPropertyName*/, const Reference< XPropertyChangeListener >& /*xListener*/ )
{
}

void SAL_CALL PropertySetMergerImpl::removePropertyChangeListener( const OUString& /*aPropertyName*/, const Reference< XPropertyChangeListener >& /*aListener*/ )
{
}

void SAL_CALL PropertySetMergerImpl::addVetoableChangeListener( const OUString& /*PropertyName*/, const Reference< XVetoableChangeListener >& /*aListener*/ )
{
}

void SAL_CALL PropertySetMergerImpl::removeVetoableChangeListener( const OUString& /*PropertyName*/, const Reference< XVetoableChangeListener >& /*aListener*/ )
{
}

// XPropertyState
PropertyState SAL_CALL PropertySetMergerImpl::getPropertyState( const OUString& PropertyName )
{
	if( mxPropSet1Info->hasPropertyByName( PropertyName ) )
	{
		if( mxPropSet1State.is() )
		{
			return mxPropSet1State->getPropertyState( PropertyName );
		}
		else
		{
			return PropertyState_DIRECT_VALUE;
		}
	}
	else
	{
		if( mxPropSet2State.is() )
		{
			return mxPropSet2State->getPropertyState( PropertyName );
		}
		else
		{
			return PropertyState_DIRECT_VALUE;
		}
	}
}

Sequence< PropertyState > SAL_CALL PropertySetMergerImpl::getPropertyStates( const Sequence< OUString >& aPropertyName )
{
	const sal_Int32 nCount = aPropertyName.getLength();
	Sequence< PropertyState > aPropStates( nCount );
	PropertyState* pPropStates = aPropStates.getArray();
	const OUString* pPropNames = aPropertyName.getConstArray();

	sal_Int32 nIndex;
	for( nIndex = 0; nIndex < nCount; nIndex++ )
		*pPropStates++ = getPropertyState( *pPropNames++ );

	return aPropStates;
}

void SAL_CALL PropertySetMergerImpl::setPropertyToDefault( const OUString& PropertyName )
{
	if( mxPropSet1State.is() && mxPropSet1Info->hasPropertyByName( PropertyName ) )
	{
		mxPropSet1State->setPropertyToDefault( PropertyName );
	}
	else
	{
		if( mxPropSet2State.is() )
		{
			mxPropSet2State->setPropertyToDefault( PropertyName );
		}
	}
}

Any SAL_CALL PropertySetMergerImpl::getPropertyDefault( const OUString& aPropertyName )
{
	if( mxPropSet1State.is() && mxPropSet1Info->hasPropertyByName( aPropertyName ) )
	{
		return mxPropSet1State->getPropertyDefault( aPropertyName );
	}
	else
	{
		if( mxPropSet2State.is() )
		{
			return mxPropSet2State->getPropertyDefault( aPropertyName );
		}
		else
		{
			Any aAny;
			return aAny;
		}
	}
}

// XPropertySetInfo
Sequence< Property > SAL_CALL PropertySetMergerImpl::getProperties()
{
	Sequence< Property > aProps1( mxPropSet1Info->getProperties() );
	const Property* pProps1 = aProps1.getArray();
	const sal_Int32 nCount1 = aProps1.getLength();

	Sequence< Property > aProps2( mxPropSet1Info->getProperties() );
	const Property* pProps2 = aProps2.getArray();
	const sal_Int32 nCount2 = aProps2.getLength();

	Sequence< Property > aProperties( nCount1 + nCount2 );

	sal_Int32 nIndex;

	Property* pProperties = aProperties.getArray();

	for( nIndex = 0; nIndex < nCount1; nIndex++ )
		*pProperties++ = *pProps1++;

	for( nIndex = 0; nIndex < nCount2; nIndex++ )
		*pProperties++ = *pProps2++;

	return aProperties;
}

Property SAL_CALL PropertySetMergerImpl::getPropertyByName( const OUString& aName )
{
	if( mxPropSet1Info->hasPropertyByName( aName ) )
		return mxPropSet1Info->getPropertyByName( aName );

	return mxPropSet2Info->getPropertyByName( aName );
}

sal_Bool SAL_CALL PropertySetMergerImpl::hasPropertyByName( const OUString& Name )
{
	if(mxPropSet1Info->hasPropertyByName( Name ) )
		return sal_True;

	return mxPropSet2Info->hasPropertyByName( Name );
}

Reference< XPropertySet > PropertySetMerger_CreateInstance( Reference< XPropertySet > rPropSet1, Reference< XPropertySet > rPropSet2 ) throw()
{
	return new PropertySetMergerImpl( rPropSet1, rPropSet2 );
}
