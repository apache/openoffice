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



#ifndef FORMS_MODULE_HXX
#define FORMS_MODULE_HXX

#ifndef _CPPUHELPER_FACTORY_HXX_ 
#include <cppuhelper/factory.hxx>
#endif
namespace binfilter {

//.........................................................................
namespace frm
{
//.........................................................................

typedef ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory > (SAL_CALL *FactoryInstantiation)
		(
			const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rServiceManager,
			const ::rtl::OUString & _rComponentName,
			::cppu::ComponentInstantiation _pCreateFunction,
			const ::com::sun::star::uno::Sequence< ::rtl::OUString > & _rServiceNames,
			rtl_ModuleCount* _pModuleCounter
		);

	//=========================================================================
	//= OFormsModule
	//=========================================================================
	class OFormsModule
	{
	private:
		OFormsModule();
			// not implemented. OFormsModule is a static class

	protected:
		// auto registration administration
		static	::com::sun::star::uno::Sequence< ::rtl::OUString >*
			s_pImplementationNames;
		static	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< ::rtl::OUString > >*
			s_pSupportedServices;
		static	::com::sun::star::uno::Sequence< sal_Int64 >*
			s_pCreationFunctionPointers;
		static	::com::sun::star::uno::Sequence< sal_Int64 >*
			s_pFactoryFunctionPointers;

	public:
		/** register a component implementing a service with the given data.
			@param	_rImplementationName
						the implementation name of the component
			@param	_rServiceNames
						the services the component supports
			@param	_pCreateFunction
						a function for creating an instance of the component
			@param	_pFactoryFunction
						a function for creating a factory for that component
			@see revokeComponent
		*/
		static void registerComponent(
			const ::rtl::OUString& _rImplementationName,
			const ::com::sun::star::uno::Sequence< ::rtl::OUString >& _rServiceNames,
			::cppu::ComponentInstantiation _pCreateFunction,
			FactoryInstantiation _pFactoryFunction);

		/** revoke the registration for the specified component
			@param	_rImplementationName
				the implementation name of the component
		*/
		static void revokeComponent(
			const ::rtl::OUString& _rImplementationName);

		/** creates a Factory for the component with the given implementation name.
			<p>Usually used from within component_getFactory.<p/>
			@param	_rxServiceManager
						a pointer to an XMultiServiceFactory interface as got in component_getFactory
			@param	_pImplementationName
						the implementation name of the component
			@return
						the XInterface access to a factory for the component
		*/
		static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > getComponentFactory(
			const ::rtl::OUString& _rImplementationName,
			const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxServiceManager
			);

	private:
		/** ensure that the impl class exists
			@precond m_aMutex is guarded when this method gets called
		*/
		static void ensureImpl();
	};

	//==========================================================================
	//= OMultiInstanceAutoRegistration
	//==========================================================================
	template <class TYPE>
	class OMultiInstanceAutoRegistration
	{
	public:
		/** automatically registeres a multi instance component
			<p>Assumed that the template argument has the three methods
				<ul>
					<li><code>static ::rtl::OUString getImplementationName_Static()</code><li/>
					<li><code>static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static()</code><li/>
					<li><code>static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
						Create(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&)</code>
						</li>
				<ul/>
			the instantiation of this object will automatically register the class via <method>OFormsModule::registerComponent</method>.
			<p/>
			<p>The factory creation function used is <code>::cppu::createSingleFactory</code>.</p>

			@see OOneInstanceAutoRegistration
		*/
		OMultiInstanceAutoRegistration();
		~OMultiInstanceAutoRegistration();
	};

	template <class TYPE>
	OMultiInstanceAutoRegistration<TYPE>::OMultiInstanceAutoRegistration()
	{
		OFormsModule::registerComponent(
			TYPE::getImplementationName_Static(),
			TYPE::getSupportedServiceNames_Static(),
			TYPE::Create,
			::cppu::createSingleFactory
			);
	}

	template <class TYPE>
	OMultiInstanceAutoRegistration<TYPE>::~OMultiInstanceAutoRegistration()
	{
		OFormsModule::revokeComponent(TYPE::getImplementationName_Static());
	}

	//==========================================================================
	//= OOneInstanceAutoRegistration
	//==========================================================================
	template <class TYPE>
	class OOneInstanceAutoRegistration
	{
	public:
		/** automatically registeres a single instance component
			<p>Assumed that the template argument has the three methods
				<ul>
					<li><code>static ::rtl::OUString getImplementationName_Static()</code><li/>
					<li><code>static ::com::sun::star::uno::Sequence< ::rtl::OUString > getSupportedServiceNames_Static()</code><li/>
					<li><code>static ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
						Create(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >&)</code>
						</li>
				<ul/>
			the instantiation of this object will automatically register the class via <method>OFormsModule::registerComponent</method>.
			<p/>
			The factory creation function used is <code>::cppu::createOneInstanceFactory</code>.
			@see OOneInstanceAutoRegistration
		*/
		OOneInstanceAutoRegistration();
		~OOneInstanceAutoRegistration();
	};

	template <class TYPE>
	OOneInstanceAutoRegistration<TYPE>::OOneInstanceAutoRegistration()
	{
		OFormsModule::registerComponent(
			TYPE::getImplementationName_Static(),
			TYPE::getSupportedServiceNames_Static(),
			TYPE::Create,
			::cppu::createOneInstanceFactory
			);
	}

	template <class TYPE>
	OOneInstanceAutoRegistration<TYPE>::~OOneInstanceAutoRegistration()
	{
		OFormsModule::revokeComponent(TYPE::getImplementationName_Static());
	}

//.........................................................................
}	// namespace frm
//.........................................................................

}//end of namespace binfilter
#endif // FORMS_MODULE_HXX

