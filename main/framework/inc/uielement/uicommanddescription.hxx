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



#ifndef __FRAMEWORK_UIELEMENT_UICOMMANDDESCRPTION_HXX_
#define __FRAMEWORK_UIELEMENT_UICOMMANDDESCRPTION_HXX_

/** Attention: stl headers must(!) be included at first. Otherwise it can make trouble
               with solaris headers ...
*/
#include <vector>
#include <list>
#include <hash_map>

//_________________________________________________________________________________________________________________
//	my own includes
//_________________________________________________________________________________________________________________
#include <threadhelp/threadhelpbase.hxx>
#include <macros/generic.hxx>
#include <macros/xinterface.hxx>
#include <macros/xtypeprovider.hxx>
#include <macros/xserviceinfo.hxx>
#include <stdtypes.h>

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/frame/XModuleManager.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________
#include <cppuhelper/implbase2.hxx>
#include <rtl/ustring.hxx>

namespace framework
{
class UICommandDescription :  private ThreadHelpBase						,	// Struct for right initialization of mutex member! Must be first of baseclasses.
							  public ::cppu::WeakImplHelper2< com::sun::star::lang::XServiceInfo		,
							                                  com::sun::star::container::XNameAccess >
{
	public:
		UICommandDescription( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager );
		virtual ~UICommandDescription();

        //  XInterface, XTypeProvider, XServiceInfo
		DECLARE_XSERVICEINFO
private:
        // XNameAccess
        virtual ::com::sun::star::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName ) 
            throw ( ::com::sun::star::container::NoSuchElementException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
        
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames() 
            throw (::com::sun::star::uno::RuntimeException);
        
        virtual sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName ) 
            throw (::com::sun::star::uno::RuntimeException);

        // XElementAccess
        virtual ::com::sun::star::uno::Type SAL_CALL getElementType() 
            throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Bool SAL_CALL hasElements() 
            throw (::com::sun::star::uno::RuntimeException);

public:
        typedef ::std::hash_map< ::rtl::OUString,
                                 ::rtl::OUString,
                                 rtl::OUStringHash,
                                 ::std::equal_to< ::rtl::OUString > > ModuleToCommandFileMap;

        typedef ::std::hash_map< ::rtl::OUString,
                                 ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >,
                                 rtl::OUStringHash,
								 ::std::equal_to< ::rtl::OUString > > UICommandsHashMap;

    protected:
        UICommandDescription( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceManager,bool  );
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess > impl_createConfigAccess(const ::rtl::OUString& _sName);
        void impl_fillElements(const sal_Char* _pName);
        sal_Bool                                                                            m_bConfigRead;
        rtl::OUString                                                                       m_aPrivateResourceURL;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >    m_xServiceManager;
        ModuleToCommandFileMap                                                              m_aModuleToCommandFileMap;
        UICommandsHashMap                                                                   m_aUICommandsHashMap;
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >        m_xGenericUICommands;
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModuleManager >         m_xModuleManager;
};

} // namespace framework

#endif // __FRAMEWORK_SERVICES_UICOMMANDDESCRPTION_HXX_
