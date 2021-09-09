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

#include "comphelper_module.hxx"

#include <com/sun/star/container/XIndexContainer.hpp>
#include <com/sun/star/uno/Sequence.h>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>


#ifndef __SGI_STL_VECTOR
#include <vector>
#endif

using namespace com::sun::star;

typedef std::vector < uno::Sequence< beans::PropertyValue > > IndexedPropertyValues;

class IndexedPropertyValuesContainer : public cppu::WeakImplHelper2< container::XIndexContainer, lang::XServiceInfo >
{
public:
	IndexedPropertyValuesContainer() throw();
	virtual ~IndexedPropertyValuesContainer() throw();

	// XIndexContainer
	virtual void SAL_CALL insertByIndex( sal_Int32 nIndex, const ::com::sun::star::uno::Any& aElement )
		throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IndexOutOfBoundsException,
			::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL removeByIndex( sal_Int32 nIndex )
		throw(::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException,
			::com::sun::star::uno::RuntimeException);

	// XIndexReplace
	virtual void SAL_CALL replaceByIndex( sal_Int32 nIndex, const ::com::sun::star::uno::Any& aElement )
		throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IndexOutOfBoundsException,
			::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

	// XIndexAccess
	virtual sal_Int32 SAL_CALL getCount(  )
		throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Any SAL_CALL getByIndex( sal_Int32 nIndex )
		throw(::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException,
			::com::sun::star::uno::RuntimeException);

	// XElementAccess
	virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  )
		throw(::com::sun::star::uno::RuntimeException);
	virtual sal_Bool SAL_CALL hasElements(  )
		throw(::com::sun::star::uno::RuntimeException);

	//XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException);
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException);

	// XServiceInfo - static versions (used for component registration)
	static ::rtl::OUString SAL_CALL getImplementationName_static();
	static uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_static();
	static uno::Reference< uno::XInterface > SAL_CALL Create( const uno::Reference< uno::XComponentContext >& );

private:
	IndexedPropertyValues maProperties;
};

IndexedPropertyValuesContainer::IndexedPropertyValuesContainer() throw()
{
}

IndexedPropertyValuesContainer::~IndexedPropertyValuesContainer() throw()
{
}

// XIndexContainer
void SAL_CALL IndexedPropertyValuesContainer::insertByIndex( sal_Int32 nIndex, const ::com::sun::star::uno::Any& aElement )
	throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IndexOutOfBoundsException,
		::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
	sal_Int32 nSize(maProperties.size());
	if ((nSize >= nIndex) && (nIndex >= 0))
	{
		uno::Sequence<beans::PropertyValue> aProps;
		if (!(aElement >>= aProps))
			throw lang::IllegalArgumentException();
		if (nSize == nIndex)
			maProperties.push_back(aProps);
		else
		{
			IndexedPropertyValues::iterator aItr;
			if ((nIndex * 2) < nSize)
			{
				aItr = maProperties.begin();
				sal_Int32 i(0);
				while(i < nIndex)
				{
					i++;
					aItr++;
				}
			}
			else
			{
				aItr = maProperties.end();
				sal_Int32 i(nSize - 1);
				while(i > nIndex)
				{
					i--;
					aItr--;
				}
			}
			maProperties.insert(aItr, aProps);
		}
	}
	else
		throw lang::IndexOutOfBoundsException();
}

void SAL_CALL IndexedPropertyValuesContainer::removeByIndex( sal_Int32 nIndex )
	throw(::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException,
		::com::sun::star::uno::RuntimeException)
{
	sal_Int32 nSize(maProperties.size());
	if ((nIndex < nSize) && (nIndex >= 0))
	{
		IndexedPropertyValues::iterator aItr;
		if ((nIndex * 2) < nSize)
		{
			aItr = maProperties.begin();
			sal_Int32 i(0);
			while(i < nIndex)
			{
				i++;
				aItr++;
			}
		}
		else
		{
			aItr = maProperties.end();
			sal_Int32 i(nSize - 1);
			while(i > nIndex)
			{
				i--;
				aItr--;
			}
		}
		maProperties.erase(aItr);
	}
	else
		throw lang::IndexOutOfBoundsException();
}

// XIndexReplace
void SAL_CALL IndexedPropertyValuesContainer::replaceByIndex( sal_Int32 nIndex, const ::com::sun::star::uno::Any& aElement )
	throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::IndexOutOfBoundsException,
		::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException)
{
	sal_Int32 nSize(maProperties.size());
	if ((nIndex < nSize) && (nIndex >= 0))
	{
		uno::Sequence<beans::PropertyValue> aProps;
		if (!(aElement >>= aProps))
			throw lang::IllegalArgumentException();
		maProperties[nIndex] = aProps;
	}
	else
		throw lang::IndexOutOfBoundsException();
}

// XIndexAccess
sal_Int32 SAL_CALL IndexedPropertyValuesContainer::getCount(  )
	throw(::com::sun::star::uno::RuntimeException)
{
	return maProperties.size();
}

::com::sun::star::uno::Any SAL_CALL IndexedPropertyValuesContainer::getByIndex( sal_Int32 nIndex )
	throw(::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::WrappedTargetException,
		::com::sun::star::uno::RuntimeException)
{
	sal_Int32 nSize(maProperties.size());
	if (!((nIndex < nSize) && (nIndex >= 0)))
		throw lang::IndexOutOfBoundsException();

	uno::Any aAny;
	aAny <<= maProperties[nIndex];
	return aAny;
}

// XElementAccess
::com::sun::star::uno::Type SAL_CALL IndexedPropertyValuesContainer::getElementType(  )
	throw(::com::sun::star::uno::RuntimeException)
{
	return ::getCppuType((uno::Sequence<beans::PropertyValue> *)0);
}

sal_Bool SAL_CALL IndexedPropertyValuesContainer::hasElements(  )
	throw(::com::sun::star::uno::RuntimeException)
{
	return !maProperties.empty();
}

//XServiceInfo
::rtl::OUString SAL_CALL IndexedPropertyValuesContainer::getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException)
{
	return getImplementationName_static();
}

::rtl::OUString SAL_CALL IndexedPropertyValuesContainer::getImplementationName_static(  )
{
	return rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "IndexedPropertyValuesContainer" ) );
}

sal_Bool SAL_CALL IndexedPropertyValuesContainer::supportsService( const ::rtl::OUString& ServiceName ) throw(::com::sun::star::uno::RuntimeException)
{
	rtl::OUString aServiceName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.document.IndexedPropertyValues" ) );
	return aServiceName == ServiceName;
}

::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL IndexedPropertyValuesContainer::getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException)
{
	return getSupportedServiceNames_static();
}


::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL IndexedPropertyValuesContainer::getSupportedServiceNames_static(  )
{
	const rtl::OUString aServiceName( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.document.IndexedPropertyValues" ) );
	const uno::Sequence< rtl::OUString > aSeq( &aServiceName, 1 );
	return aSeq;
}


uno::Reference< uno::XInterface > SAL_CALL IndexedPropertyValuesContainer::Create(
				const uno::Reference< uno::XComponentContext >&)
{
	return (cppu::OWeakObject*)new IndexedPropertyValuesContainer();
}

void createRegistryInfo_IndexedPropertyValuesContainer()
{
	static ::comphelper::module::OAutoRegistration< IndexedPropertyValuesContainer > aAutoRegistration;
}
