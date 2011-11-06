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



#ifndef _XMLOFF_FORMS_EVENTEXPORT_HXX_
#define _XMLOFF_FORMS_EVENTEXPORT_HXX_

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEREPLACE_HPP_
#include <com/sun/star/container/XNameReplace.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_SCRIPTEVENTDESCRIPTOR_HPP_
#include <com/sun/star/script/ScriptEventDescriptor.hpp>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif
#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif
namespace binfilter {

//.........................................................................
namespace xmloff
{
//.........................................................................

	//=====================================================================
	//= OEventDescriptorMapper
	//=====================================================================
	typedef ::cppu::WeakImplHelper1	<	::com::sun::star::container::XNameReplace
									>	OEventDescriptorMapper_Base;
	/** helper class wrapping different script event representations

		<p>In the form layer, the script events are represented by <type scope="com.sun.star.script">ScriptEventDescriptor</type>
		instances. The office applications, on the other hand, represent their a single script event as sequence
		of <type scope="com.sun.star.beans">PropertyValue</type>s, where all events of a given object are
		accessible through a <type scope="com.sun.star.container">XNameReplace</type> interface.</p>
		<p>This class maps the first representation of events of a single object to the second one.</p>
		<p>This way, we can use the helper classes here in the project.</p>
	*/
	class OEventDescriptorMapper : public OEventDescriptorMapper_Base
	{
	protected:
		DECLARE_STL_USTRINGACCESS_MAP( ::com::sun::star::uno::Sequence < ::com::sun::star::beans::PropertyValue >, MapString2PropertyValueSequence );
		MapString2PropertyValueSequence	m_aMappedEvents;

	public:
		OEventDescriptorMapper(
			const ::com::sun::star::uno::Sequence< ::com::sun::star::script::ScriptEventDescriptor >& _rEvents);

		// XNameReplace
	    virtual void SAL_CALL replaceByName( const ::rtl::OUString& aName, const ::com::sun::star::uno::Any& aElement ) throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

		// XNameAccess
		virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName ) throw(::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames(  ) throw(::com::sun::star::uno::RuntimeException);
		virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName ) throw(::com::sun::star::uno::RuntimeException);

		// XElementAccess
		virtual ::com::sun::star::uno::Type SAL_CALL getElementType(  ) throw(::com::sun::star::uno::RuntimeException);
		virtual sal_Bool SAL_CALL hasElements(  ) throw(::com::sun::star::uno::RuntimeException);
	};


//.........................................................................
}	// namespace xmloff
//.........................................................................

}//end of namespace binfilter
#endif // _XMLOFF_FORMS_EVENTEXPORT_HXX_

