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
#include <comphelper/namecontainer.hxx>
#include <cppuhelper/implbase1.hxx>
#include <osl/diagnose.h>
#include <osl/mutex.hxx>
#include <comphelper/stl_types.hxx>

DECLARE_STL_USTRINGACCESS_MAP( ::com::sun::star::uno::Any, SvGenericNameContainerMapImpl );

namespace comphelper
{
	class NameContainerImpl
	{
	public:
		osl::Mutex maMutex;
	};

	/** this is the base helper class for NameContainer that's also declared in this header. */
	class NameContainer : public ::cppu::WeakImplHelper1< ::com::sun::star::container::XNameContainer >, private NameContainerImpl
	{
	public:
		NameContainer( ::com::sun::star::uno::Type aType );
		virtual ~NameContainer();

		// XNameContainer
		virtual void SAL_CALL insertByName( const ::rtl::OUString& aName, const ::com::sun::star::uno::Any& aElement )
			throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::container::ElementExistException,
			::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL removeByName( const ::rtl::OUString& Name )
			throw(::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException,
				::com::sun::star::uno::RuntimeException);

		// XNameReplace
		virtual void SAL_CALL replaceByName( const ::rtl::OUString& aName, const ::com::sun::star::uno::Any& aElement )
			throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::container::NoSuchElementException,
				::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

		// XNameAccess
		virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName )
			throw(::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException,
				::com::sun::star::uno::RuntimeException);
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames(  )
			throw(::com::sun::star::uno::RuntimeException);
		virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName )
			throw(::com::sun::star::uno::RuntimeException);

		// XElementAccess
		virtual sal_Bool SAL_CALL hasElements(  )
			throw(::com::sun::star::uno::RuntimeException);
		virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  )
			throw(::com::sun::star::uno::RuntimeException);

	private:
		SvGenericNameContainerMapImpl maProperties;
		const ::com::sun::star::uno::Type maType;
	};
}

using namespace ::comphelper;
using namespace ::osl;
using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::lang;


NameContainer::NameContainer( ::com::sun::star::uno::Type aType )
: maType( aType )
{
}

NameContainer::~NameContainer()
{
}

// XNameContainer
void SAL_CALL NameContainer::insertByName( const rtl::OUString& aName, const Any& aElement )
	throw(IllegalArgumentException, ElementExistException,
		WrappedTargetException, RuntimeException)
{
	MutexGuard aGuard( maMutex );

	if( maProperties.find( aName ) != maProperties.end() )
		throw ElementExistException();

	if( aElement.getValueType() != maType )
		throw IllegalArgumentException();

	maProperties.insert( SvGenericNameContainerMapImpl::value_type(aName,aElement));
}

void SAL_CALL NameContainer::removeByName( const ::rtl::OUString& Name )
	throw(NoSuchElementException, WrappedTargetException,
		RuntimeException)
{
	MutexGuard aGuard( maMutex );

	SvGenericNameContainerMapImpl::iterator aIter = maProperties.find( Name );
	if( aIter == maProperties.end() )
		throw NoSuchElementException();

	maProperties.erase( aIter );
}

// XNameReplace

void SAL_CALL NameContainer::replaceByName( const ::rtl::OUString& aName, const Any& aElement )
	throw(IllegalArgumentException, NoSuchElementException,
		WrappedTargetException, RuntimeException)
{
	MutexGuard aGuard( maMutex );

	SvGenericNameContainerMapImpl::iterator aIter( maProperties.find( aName ) );
	if( aIter == maProperties.end() )
		throw NoSuchElementException();

	if( aElement.getValueType() != maType )
		throw IllegalArgumentException();

	(*aIter).second = aElement;
}

// XNameAccess

Any SAL_CALL NameContainer::getByName( const ::rtl::OUString& aName )
	throw(NoSuchElementException, WrappedTargetException,
		RuntimeException)
{
	MutexGuard aGuard( maMutex );

	SvGenericNameContainerMapImpl::iterator aIter = maProperties.find( aName );
	if( aIter == maProperties.end() )
		throw NoSuchElementException();

	return (*aIter).second;
}

Sequence< ::rtl::OUString > SAL_CALL NameContainer::getElementNames(  )
	throw(RuntimeException)
{
	MutexGuard aGuard( maMutex );

	SvGenericNameContainerMapImpl::iterator aIter = maProperties.begin();
	const SvGenericNameContainerMapImpl::iterator aEnd = maProperties.end();

	Sequence< rtl::OUString > aNames( maProperties.size() );
	rtl::OUString* pNames = aNames.getArray();

	while( aIter != aEnd )
	{
		*pNames++ = (*aIter++).first;
	}

	return aNames;
}

sal_Bool SAL_CALL NameContainer::hasByName( const ::rtl::OUString& aName )
	throw(RuntimeException)
{
	MutexGuard aGuard( maMutex );

	SvGenericNameContainerMapImpl::iterator aIter = maProperties.find( aName );
	return aIter != maProperties.end();
}

sal_Bool SAL_CALL NameContainer::hasElements(  )
	throw(RuntimeException)
{
	MutexGuard aGuard( maMutex );

	return !maProperties.empty();
}

Type SAL_CALL NameContainer::getElementType()
	throw( RuntimeException )
{
	return maType;
}

Reference< XNameContainer > comphelper::NameContainer_createInstance( Type aType )
{
	return (XNameContainer*) new NameContainer( aType );
}
