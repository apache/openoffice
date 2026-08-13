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



#ifndef _CHART2_NAMECONTAINER_HXX
#define _CHART2_NAMECONTAINER_HXX

#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/XCloneable.hpp>
#include <cppuhelper/implbase3.hxx>
#include "charttoolsdllapi.hxx"

#include <map>

//.............................................................................
namespace chart
{
//.............................................................................

OOO_DLLPUBLIC_CHARTTOOLS ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer > createNameContainer(
    const ::com::sun::star::uno::Type& rType, const rtl::OUString& rServicename, const rtl::OUString& rImplementationName );

namespace impl
{
typedef ::cppu::WeakImplHelper3<
        ::com::sun::star::container::XNameContainer,
        ::com::sun::star::lang::XServiceInfo,
        ::com::sun::star::util::XCloneable >
    NameContainer_Base;
}

class NameContainer : public impl::NameContainer_Base
{
public:
	NameContainer( const ::com::sun::star::uno::Type& rType, const rtl::OUString& rServicename, const rtl::OUString& rImplementationName );
    explicit NameContainer( const NameContainer & rOther );
	virtual ~NameContainer();

	// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

	// XNameContainer
	virtual void SAL_CALL insertByName( const  rtl::OUString& aName, const  com::sun::star::uno::Any& aElement );
	virtual void SAL_CALL removeByName( const  rtl::OUString& Name );

	// XNameReplace
    virtual void SAL_CALL replaceByName( const  rtl::OUString& aName, const  com::sun::star::uno::Any& aElement );

	// XNameAccess
    virtual com::sun::star::uno::Any SAL_CALL getByName( const  rtl::OUString& aName );
    virtual com::sun::star::uno::Sequence<  rtl::OUString > SAL_CALL getElementNames(  );
    virtual sal_Bool SAL_CALL hasByName( const  rtl::OUString& aName );

	// XElementAccess
    virtual sal_Bool SAL_CALL hasElements(  );
    virtual com::sun::star::uno::Type SAL_CALL getElementType(  );

    // XCloneable
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::util::XCloneable > SAL_CALL createClone();

private: //methods
    NameContainer();//no default constructor

private: //member
    const ::com::sun::star::uno::Type   m_aType;
    const rtl::OUString                 m_aServicename;
    const rtl::OUString                 m_aImplementationName;

    typedef ::std::map< ::rtl::OUString, com::sun::star::uno::Any > tContentMap;

    tContentMap m_aMap;
};

//.............................................................................
} //namespace chart
//.............................................................................
#endif
