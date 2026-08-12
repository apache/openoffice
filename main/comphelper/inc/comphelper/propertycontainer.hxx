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



#ifndef _COMPHELPER_PROPERTYCONTAINER_HXX_
#define _COMPHELPER_PROPERTYCONTAINER_HXX_

#include <comphelper/propertycontainerhelper.hxx>
#include <cppuhelper/propshlp.hxx>
#include <com/sun/star/uno/Type.hxx>
#include "comphelper/comphelperdllapi.h"

//.........................................................................
namespace comphelper
{
//.........................................................................

//==========================================================================
//= OPropertyContainer
//==========================================================================
typedef ::cppu::OPropertySetHelper OPropertyContainer_Base;
/** a OPropertySetHelper implementation which is just a simple container for properties represented
	by class members, usually in a derived class.
	<BR>
	A restriction of this class is that no value conversions are made on a setPropertyValue call. Though
	the base class supports this with the convertFastPropertyValue method, the OPropertyContainer accepts only
	values which already have the correct type, it's unable to convert, for instance, a long to a short.
*/
class COMPHELPER_DLLPUBLIC OPropertyContainer
            :public OPropertyContainer_Base
            ,public OPropertyContainerHelper
{
public:
	// this dtor is needed otherwise we can get a wrong delete operator
	/*	Classes derived from this one also derive from a UNO base whose destructor
		is declared SAL_THROW( (RuntimeException) ).  From C++11 on a destructor with
		no written specification gets an implicit "never throws", so the derived
		class's own deduced specification came out weaker than this one, which an
		override may not be (C2694).  Inert under C++03, where no implicit
		specification exists, and MSVC does not enforce a dynamic specification at
		run time in any case -- it only uses it for this compile-time check. */
	virtual ~OPropertyContainer();

protected:
	OPropertyContainer(::cppu::OBroadcastHelper& _rBHelper);

	/// for scripting : the types of the interfaces supported by this class
	virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes() throw (::com::sun::star::uno::RuntimeException);

// OPropertySetHelper overridables
	virtual sal_Bool SAL_CALL convertFastPropertyValue(
							::com::sun::star::uno::Any & rConvertedValue,
							::com::sun::star::uno::Any & rOldValue,
							sal_Int32 nHandle,
							const ::com::sun::star::uno::Any& rValue )
								throw (::com::sun::star::lang::IllegalArgumentException);

	virtual void SAL_CALL   setFastPropertyValue_NoBroadcast(
							    sal_Int32 nHandle,
							    const ::com::sun::star::uno::Any& rValue
                            )
                            throw (::com::sun::star::uno::Exception);

    using OPropertyContainer_Base::getFastPropertyValue;
	virtual void SAL_CALL getFastPropertyValue(
								::com::sun::star::uno::Any& rValue,
								sal_Int32 nHandle
									 ) const;

    // disambiguate a base class method (XFastPropertySet)
	virtual void SAL_CALL setFastPropertyValue( sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue )
		throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
};

//.........................................................................
}	// namespace comphelper
//.........................................................................

#endif // _COMPHELPER_PROPERTYCONTAINER_HXX_
