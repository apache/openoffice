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
#include "precompiled_comphelper.hxx"
#include <comphelper/ChainablePropertySet.hxx>
#include <comphelper/ChainablePropertySetInfo.hxx>
#include <vos/mutex.hxx>

#include <memory> // STL auto_ptr


using namespace ::rtl;
using namespace ::comphelper;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
using ::vos::IMutex;

ChainablePropertySet::ChainablePropertySet( comphelper::ChainablePropertySetInfo* pInfo, vos::IMutex *pMutex )
	throw()
: mpInfo ( pInfo )
, mpMutex ( pMutex )
, mxInfo ( pInfo )
{
}

ChainablePropertySet::~ChainablePropertySet()
	throw()
{
}

// XPropertySet
Reference< XPropertySetInfo > SAL_CALL ChainablePropertySet::getPropertySetInfo(  )
	throw(RuntimeException)
{
	return mxInfo;
}

void ChainablePropertySet::lockMutex()
{
	if (mpMutex)
		mpMutex->acquire();
}

void ChainablePropertySet::unlockMutex()
{
	if (mpMutex)
		mpMutex->release();
}

void SAL_CALL ChainablePropertySet::setPropertyValue( const ::rtl::OUString& rPropertyName, const Any& rValue )
	throw(UnknownPropertyException, PropertyVetoException, IllegalArgumentException, WrappedTargetException, RuntimeException)
{
	// acquire mutex in c-tor and releases it in the d-tor (exception safe!).
	std::auto_ptr< vos::OGuard > pMutexGuard;
	if (mpMutex)
		pMutexGuard.reset( new vos::OGuard(mpMutex) );

	PropertyInfoHash::const_iterator aIter = mpInfo->maMap.find ( rPropertyName );

	if( aIter == mpInfo->maMap.end())
		throw UnknownPropertyException( rPropertyName, static_cast< XPropertySet* >( this ) );

	_preSetValues();
	_setSingleValue( *((*aIter).second), rValue );
	_postSetValues();
}

Any SAL_CALL ChainablePropertySet::getPropertyValue( const ::rtl::OUString& rPropertyName )
	throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
	// acquire mutex in c-tor and releases it in the d-tor (exception safe!).
	std::auto_ptr< vos::OGuard > pMutexGuard;
	if (mpMutex)
		pMutexGuard.reset( new vos::OGuard(mpMutex) );

	PropertyInfoHash::const_iterator aIter = mpInfo->maMap.find ( rPropertyName );

	if( aIter == mpInfo->maMap.end())
		throw UnknownPropertyException( rPropertyName, static_cast< XPropertySet* >( this ) );

	Any aAny;
	_preGetValues ();
	_getSingleValue( *((*aIter).second), aAny );
	_postGetValues ();

	return aAny;
}

void SAL_CALL ChainablePropertySet::addPropertyChangeListener( const ::rtl::OUString&, const Reference< XPropertyChangeListener >& )
	throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
	// todo
}

void SAL_CALL ChainablePropertySet::removePropertyChangeListener( const ::rtl::OUString&, const Reference< XPropertyChangeListener >& )
	throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
	// todo
}

void SAL_CALL ChainablePropertySet::addVetoableChangeListener( const ::rtl::OUString&, const Reference< XVetoableChangeListener >& )
	throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
	// todo
}

void SAL_CALL ChainablePropertySet::removeVetoableChangeListener( const ::rtl::OUString&, const Reference< XVetoableChangeListener >& )
	throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
	// todo
}

// XMultiPropertySet
void SAL_CALL ChainablePropertySet::setPropertyValues( const Sequence< ::rtl::OUString >& aPropertyNames, const Sequence< Any >& aValues )
	throw(PropertyVetoException, IllegalArgumentException, WrappedTargetException, RuntimeException)
{
	// acquire mutex in c-tor and releases it in the d-tor (exception safe!).
	std::auto_ptr< vos::OGuard > pMutexGuard;
	if (mpMutex)
		pMutexGuard.reset( new vos::OGuard(mpMutex) );

	const sal_Int32 nCount = aPropertyNames.getLength();

	if( nCount != aValues.getLength() )
		throw IllegalArgumentException();

	if( nCount )
	{
		_preSetValues();

		const Any * pAny = aValues.getConstArray();
		const OUString * pString = aPropertyNames.getConstArray();
		PropertyInfoHash::const_iterator aEnd = mpInfo->maMap.end(), aIter;

		for ( sal_Int32 i = 0; i < nCount; ++i, ++pString, ++pAny )
		{
			aIter = mpInfo->maMap.find ( *pString );
			if ( aIter == aEnd )
				throw UnknownPropertyException( *pString, static_cast< XPropertySet* >( this ) );

			_setSingleValue ( *((*aIter).second), *pAny );
		}

		_postSetValues();
	}
}

Sequence< Any > SAL_CALL ChainablePropertySet::getPropertyValues( const Sequence< ::rtl::OUString >& aPropertyNames )
	throw(RuntimeException)
{
	// acquire mutex in c-tor and releases it in the d-tor (exception safe!).
	std::auto_ptr< vos::OGuard > pMutexGuard;
	if (mpMutex)
		pMutexGuard.reset( new vos::OGuard(mpMutex) );

	const sal_Int32 nCount = aPropertyNames.getLength();

	Sequence < Any > aValues ( nCount );

	if( nCount )
	{
		_preGetValues();

		Any * pAny = aValues.getArray();
		const OUString * pString = aPropertyNames.getConstArray();
		PropertyInfoHash::const_iterator aEnd = mpInfo->maMap.end(), aIter;

		for ( sal_Int32 i = 0; i < nCount; ++i, ++pString, ++pAny )
		{
			aIter = mpInfo->maMap.find ( *pString );
			if ( aIter == aEnd )
				throw UnknownPropertyException( *pString, static_cast< XPropertySet* >( this ) );

			_getSingleValue ( *((*aIter).second), *pAny );
		}

		_postGetValues();
	}
	return aValues;
}

void SAL_CALL ChainablePropertySet::addPropertiesChangeListener( const Sequence< ::rtl::OUString >&, const Reference< XPropertiesChangeListener >& )
	throw(RuntimeException)
{
	// todo
}

void SAL_CALL ChainablePropertySet::removePropertiesChangeListener( const Reference< XPropertiesChangeListener >& )
	throw(RuntimeException)
{
	// todo
}

void SAL_CALL ChainablePropertySet::firePropertiesChangeEvent( const Sequence< ::rtl::OUString >&, const Reference< XPropertiesChangeListener >& )
	throw(RuntimeException)
{
	// todo
}

// XPropertyState
PropertyState SAL_CALL ChainablePropertySet::getPropertyState( const ::rtl::OUString& PropertyName )
	throw(UnknownPropertyException, RuntimeException)
{
	PropertyInfoHash::const_iterator aIter = mpInfo->maMap.find( PropertyName );
	if( aIter == mpInfo->maMap.end())
		throw UnknownPropertyException( PropertyName, static_cast< XPropertySet* >( this ) );

	PropertyState aState;

	_preGetPropertyState();
	_getPropertyState( *((*aIter).second), aState );
	_postGetPropertyState();

	return aState;
}

Sequence< PropertyState > SAL_CALL ChainablePropertySet::getPropertyStates( const Sequence< ::rtl::OUString >& rPropertyNames )
	throw(UnknownPropertyException, RuntimeException)
{
	const sal_Int32 nCount = rPropertyNames.getLength();

	Sequence< PropertyState > aStates( nCount );
	if( nCount )
	{
		PropertyState * pState = aStates.getArray();
		const OUString * pString = rPropertyNames.getConstArray();
		PropertyInfoHash::const_iterator aEnd = mpInfo->maMap.end(), aIter;
		_preGetPropertyState();

		for ( sal_Int32 i = 0; i < nCount; ++i, ++pString, ++pState )
		{
			aIter = mpInfo->maMap.find ( *pString );
			if ( aIter == aEnd )
				throw UnknownPropertyException( *pString, static_cast< XPropertySet* >( this ) );

			_getPropertyState ( *((*aIter).second), *pState );
		}
		_postGetPropertyState();
	}
	return aStates;
}

void SAL_CALL ChainablePropertySet::setPropertyToDefault( const ::rtl::OUString& rPropertyName )
	throw(UnknownPropertyException, RuntimeException)
{
	PropertyInfoHash::const_iterator aIter = mpInfo->maMap.find ( rPropertyName );

	if( aIter == mpInfo->maMap.end())
		throw UnknownPropertyException( rPropertyName, static_cast< XPropertySet* >( this ) );
	_setPropertyToDefault( *((*aIter).second) );
}

Any SAL_CALL ChainablePropertySet::getPropertyDefault( const ::rtl::OUString& rPropertyName )
	throw(UnknownPropertyException, WrappedTargetException, RuntimeException)
{
	PropertyInfoHash::const_iterator aIter = mpInfo->maMap.find ( rPropertyName );

	if( aIter == mpInfo->maMap.end())
		throw UnknownPropertyException( rPropertyName, static_cast< XPropertySet* >( this ) );
	return _getPropertyDefault( *((*aIter).second) );
}

void ChainablePropertySet::_preGetPropertyState ()
	throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException )
{
	OSL_ENSURE( sal_False, "you have to implement this yourself!");
}

void ChainablePropertySet::_getPropertyState( const comphelper::PropertyInfo&, PropertyState& )
	throw(UnknownPropertyException )
{
	OSL_ENSURE( sal_False, "you have to implement this yourself!");
}

void ChainablePropertySet::_postGetPropertyState ()
	throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException )
{
	OSL_ENSURE( sal_False, "you have to implement this yourself!");
}

void ChainablePropertySet::_setPropertyToDefault( const comphelper::PropertyInfo& )
	throw(UnknownPropertyException )
{
	OSL_ENSURE( sal_False, "you have to implement this yourself!");
}

Any ChainablePropertySet::_getPropertyDefault( const comphelper::PropertyInfo& )
	throw(UnknownPropertyException, WrappedTargetException )
{
	OSL_ENSURE( sal_False, "you have to implement this yourself!");

	Any aAny;
	return aAny;
}
