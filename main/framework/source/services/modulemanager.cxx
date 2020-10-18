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
#include "precompiled_framework.hxx"

#include "services/modulemanager.hxx"
#include "services/frame.hxx"

//_______________________________________________
// own includes
#include <threadhelp/readguard.hxx>
#include <threadhelp/writeguard.hxx>
#include <services.h>

//_______________________________________________
// interface includes
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XModule.hpp>
#include <comphelper/configurationhelper.hxx>
#include <comphelper/sequenceashashmap.hxx>
#include <comphelper/sequenceasvector.hxx>
#include <comphelper/enumhelper.hxx>

//_______________________________________________
// other includes
#include <rtl/logfile.hxx>

namespace framework
{

static const ::rtl::OUString CFGPATH_FACTORIES     = ::rtl::OUString::createFromAscii("/org.openoffice.Setup/Office/Factories");
static const ::rtl::OUString MODULEPROP_IDENTIFIER = ::rtl::OUString::createFromAscii("ooSetupFactoryModuleIdentifier" );

/*-----------------------------------------------
    04.12.2003 09:32
-----------------------------------------------*/
DEFINE_XINTERFACE_7(ModuleManager                                    ,
                    OWeakObject                                      ,
                    DIRECT_INTERFACE(css::lang::XTypeProvider       ),
                    DIRECT_INTERFACE(css::lang::XServiceInfo        ),
                    DIRECT_INTERFACE(css::container::XNameReplace   ),
                    DIRECT_INTERFACE(css::container::XNameAccess    ),
                    DIRECT_INTERFACE(css::container::XElementAccess ),
                    DIRECT_INTERFACE(css::container::XContainerQuery),
                    DIRECT_INTERFACE(css::frame::XModuleManager     ))

/*-----------------------------------------------
    04.12.2003 09:32
-----------------------------------------------*/
DEFINE_XTYPEPROVIDER_7(ModuleManager                  ,
                       css::lang::XTypeProvider       ,
                       css::lang::XServiceInfo        ,
                       css::container::XNameReplace   ,
                       css::container::XNameAccess    ,
                       css::container::XElementAccess ,
                       css::container::XContainerQuery,
                       css::frame::XModuleManager     )

/*-----------------------------------------------
    04.12.2003 09:35
-----------------------------------------------*/
DEFINE_XSERVICEINFO_ONEINSTANCESERVICE(ModuleManager                   ,
                                       ::cppu::OWeakObject             ,
                                       SERVICENAME_MODULEMANAGER       ,
                                       IMPLEMENTATIONNAME_MODULEMANAGER)

/*-----------------------------------------------
    04.12.2003 09:35
-----------------------------------------------*/
DEFINE_INIT_SERVICE(
                    ModuleManager,
                    {
                        /*Attention
                            I think we don't need any mutex or lock here ... because we are called by our own static method impl_createInstance()
                            to create a new instance of this class by our own supported service factory.
                            see macro DEFINE_XSERVICEINFO_MULTISERVICE and "impl_initService()" for further informations!
                        */
                    }
                   )

/*-----------------------------------------------
    04.12.2003 09:30
-----------------------------------------------*/
ModuleManager::ModuleManager(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR)
    : ThreadHelpBase(     )
    , m_xSMGR       (xSMGR)
{
}

/*-----------------------------------------------
    10.12.2003 11:59
-----------------------------------------------*/
ModuleManager::~ModuleManager()
{
    if (m_xCFG.is())
        m_xCFG.clear();
}

/*-----------------------------------------------
    10.12.2003 11:02
-----------------------------------------------*/
::rtl::OUString SAL_CALL ModuleManager::identify(const css::uno::Reference< css::uno::XInterface >& xModule)
    throw(css::lang::IllegalArgumentException,
          css::frame::UnknownModuleException,
          css::uno::RuntimeException         )
{
    // valid parameter?
    css::uno::Reference< css::frame::XFrame >      xFrame     (xModule, css::uno::UNO_QUERY);
    css::uno::Reference< css::awt::XWindow >       xWindow    (xModule, css::uno::UNO_QUERY);
    css::uno::Reference< css::frame::XController > xController(xModule, css::uno::UNO_QUERY);
    css::uno::Reference< css::frame::XModel >      xModel     (xModule, css::uno::UNO_QUERY);

    if (
        (!xFrame.is()     ) &&
        (!xWindow.is()    ) &&
        (!xController.is()) &&
        (!xModel.is()     )
       )
    {
        throw css::lang::IllegalArgumentException(
                ::rtl::OUString::createFromAscii("Given module is not a frame nor a window, controller or model."),
                static_cast< ::cppu::OWeakObject* >(this),
                1);
    }

	if (xFrame.is())
    {
		xController = xFrame->getController();
        xWindow     = xFrame->getComponentWindow();
    }
    if (xController.is())
        xModel = xController->getModel();

    // modules are implemented by the deepest component in hierarchy ...
    // Means: model -> controller -> window
    // No fallbacks to higher components are allowed !
    // Note : A frame provides access to module components only ... but it's not a module by himself.
    
    ::rtl::OUString sModule;
    if (xModel.is())
        sModule = implts_identify(xModel);
    else
    if (xController.is())
        sModule = implts_identify(xController);
    else
    if (xWindow.is())
        sModule = implts_identify(xWindow);
    
    if (sModule.getLength() < 1)
        throw css::frame::UnknownModuleException(
                ::rtl::OUString::createFromAscii("Can't find suitable module for the given component."),
                static_cast< ::cppu::OWeakObject* >(this));
    
    return sModule;
}

/*-----------------------------------------------
    08.03.2007 09:55
-----------------------------------------------*/
void SAL_CALL ModuleManager::replaceByName(const ::rtl::OUString& sName ,
                                           const css::uno::Any&   aValue)
    throw (css::lang::IllegalArgumentException   ,
           css::container::NoSuchElementException,
           css::lang::WrappedTargetException     ,
           css::uno::RuntimeException            )
{
    ::comphelper::SequenceAsHashMap lProps(aValue);
    if (lProps.empty() )
    {
        throw css::lang::IllegalArgumentException(
                ::rtl::OUString::createFromAscii("No properties given to replace part of module."),
                static_cast< css::container::XNameAccess* >(this),
                2);
    }

    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // get access to the element
    // Note: Dont use impl_getConfig() method here. Because it creates a readonly access only, further
    // it cache it as a member of this module manager instance. If we change some props there ... but dont
    // flush changes (because an error occurred) we will read them later. If we use a different config access
    // we can close it without a flush ... and our read data won't be affected .-)
    css::uno::Reference< css::uno::XInterface >         xCfg      = ::comphelper::ConfigurationHelper::openConfig(
                                                                        xSMGR,
                                                                        CFGPATH_FACTORIES,
                                                                        ::comphelper::ConfigurationHelper::E_STANDARD);
    css::uno::Reference< css::container::XNameAccess >  xModules (xCfg, css::uno::UNO_QUERY_THROW);
    css::uno::Reference< css::container::XNameReplace > xModule  ;

    xModules->getByName(sName) >>= xModule;
    if (!xModule.is())
    {
        throw css::uno::RuntimeException(
                ::rtl::OUString::createFromAscii("Was not able to get write access to the requested module entry inside configuration."),
                static_cast< css::container::XNameAccess* >(this));
    }

    ::comphelper::SequenceAsHashMap::const_iterator pProp;
    for (  pProp  = lProps.begin();
           pProp != lProps.end()  ;
         ++pProp                  )
    {
        const ::rtl::OUString& sPropName  = pProp->first;
        const css::uno::Any&   aPropValue = pProp->second;
    
        // let "NoSuchElementException" out ! We support the same API ...
        // and without a flush() at the end all changed data before will be ignored !
        xModule->replaceByName(sPropName, aPropValue);
    }

    ::comphelper::ConfigurationHelper::flush(xCfg);
}

/*-----------------------------------------------
    10.12.2003 12:05
-----------------------------------------------*/
css::uno::Any SAL_CALL ModuleManager::getByName(const ::rtl::OUString& sName)
    throw(css::container::NoSuchElementException,
          css::lang::WrappedTargetException     ,
          css::uno::RuntimeException            )
{
    // get access to the element
    css::uno::Reference< css::container::XNameAccess > xCFG = implts_getConfig();
    css::uno::Reference< css::container::XNameAccess > xModule;
    xCFG->getByName(sName) >>= xModule;
    if (!xModule.is())
    {
        throw css::uno::RuntimeException(
                ::rtl::OUString::createFromAscii("Was not able to get write access to the requested module entry inside configuration."),
                static_cast< css::container::XNameAccess* >(this));
    }

    // convert it to seq< PropertyValue >
    const css::uno::Sequence< ::rtl::OUString > lPropNames = xModule->getElementNames();
          ::comphelper::SequenceAsHashMap       lProps     ;
          sal_Int32                             c          = lPropNames.getLength();
          sal_Int32                             i          = 0;

    lProps[MODULEPROP_IDENTIFIER] <<= sName;
    for (i=0; i<c; ++i)
    {
        const ::rtl::OUString& sPropName         = lPropNames[i];
                               lProps[sPropName] = xModule->getByName(sPropName);
    }

    return css::uno::makeAny(lProps.getAsConstPropertyValueList());
}

/*-----------------------------------------------
    10.12.2003 11:58
-----------------------------------------------*/
css::uno::Sequence< ::rtl::OUString > SAL_CALL ModuleManager::getElementNames()
    throw(css::uno::RuntimeException)
{
    css::uno::Reference< css::container::XNameAccess > xCFG = implts_getConfig();
    return xCFG->getElementNames();
}

/*-----------------------------------------------
    10.12.2003 11:57
-----------------------------------------------*/
sal_Bool SAL_CALL ModuleManager::hasByName(const ::rtl::OUString& sName)
    throw(css::uno::RuntimeException)
{
    css::uno::Reference< css::container::XNameAccess > xCFG = implts_getConfig();
    return xCFG->hasByName(sName);
}

/*-----------------------------------------------
    10.12.2003 11:35
-----------------------------------------------*/
css::uno::Type SAL_CALL ModuleManager::getElementType()
    throw(css::uno::RuntimeException)
{
    return ::getCppuType((const css::uno::Sequence< css::beans::PropertyValue >*)0);
}

/*-----------------------------------------------
    10.12.2003 11:56
-----------------------------------------------*/
sal_Bool SAL_CALL ModuleManager::hasElements()
    throw(css::uno::RuntimeException)
{
    css::uno::Reference< css::container::XNameAccess > xCFG = implts_getConfig();
    return xCFG->hasElements();
}

/*-----------------------------------------------
    07.03.2007 12:55
-----------------------------------------------*/
css::uno::Reference< css::container::XEnumeration > SAL_CALL ModuleManager::createSubSetEnumerationByQuery(const ::rtl::OUString&)
    throw(css::uno::RuntimeException)
{
    return css::uno::Reference< css::container::XEnumeration >();
}

/*-----------------------------------------------
    07.03.2007 12:55
-----------------------------------------------*/
css::uno::Reference< css::container::XEnumeration > SAL_CALL ModuleManager::createSubSetEnumerationByProperties(const css::uno::Sequence< css::beans::NamedValue >& lProperties)
    throw(css::uno::RuntimeException)
{
    ::comphelper::SequenceAsHashMap                 lSearchProps (lProperties);
    css::uno::Sequence< ::rtl::OUString >           lModules     = getElementNames();
    sal_Int32                                       c            = lModules.getLength();
    sal_Int32                                       i            = 0;
    ::comphelper::SequenceAsVector< css::uno::Any > lResult      ;

    for (i=0; i<c; ++i)
    {
        try
        {
            const ::rtl::OUString&                sModule      = lModules[i];
                  ::comphelper::SequenceAsHashMap lModuleProps = getByName(sModule);
        
            if (lModuleProps.match(lSearchProps))
                lResult.push_back(css::uno::makeAny(lModuleProps.getAsConstPropertyValueList()));
        }
        catch(const css::uno::Exception&)
            {}
    }
    
    ::comphelper::OAnyEnumeration*                      pEnum = new ::comphelper::OAnyEnumeration(lResult.getAsConstList());
    css::uno::Reference< css::container::XEnumeration > xEnum(static_cast< css::container::XEnumeration* >(pEnum), css::uno::UNO_QUERY_THROW);
    return xEnum;
}

/*-----------------------------------------------
    14.12.2003 09:45
-----------------------------------------------*/
css::uno::Reference< css::container::XNameAccess > ModuleManager::implts_getConfig()
    throw(css::uno::RuntimeException)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    if (m_xCFG.is())
        return m_xCFG;
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    css::uno::Reference< css::uno::XInterface > xCfg;
    try
    {
        xCfg = ::comphelper::ConfigurationHelper::openConfig(
                    xSMGR,
                    CFGPATH_FACTORIES,
                    ::comphelper::ConfigurationHelper::E_READONLY);
    }
    catch(const css::uno::RuntimeException& exRun)
        { throw exRun; }
    catch(const css::uno::Exception&)
        { xCfg.clear(); }

    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);
    m_xCFG = css::uno::Reference< css::container::XNameAccess >(xCfg, css::uno::UNO_QUERY_THROW);
    return m_xCFG;
    // <- SAFE ----------------------------------
}

/*-----------------------------------------------
    30.01.2004 07:54
-----------------------------------------------*/
::rtl::OUString ModuleManager::implts_identify(const css::uno::Reference< css::uno::XInterface >& xComponent)
{
    // Search for an optional (!) interface XModule first.
    // It's used to overrule an existing service name. Used e.g. by our database form designer
    // which uses a writer module internally.
    css::uno::Reference< css::frame::XModule > xModule(xComponent, css::uno::UNO_QUERY);
    if (xModule.is())
        return xModule->getIdentifier();

    // detect modules in a generic way ...          
    // comparing service names with configured entries ...
    css::uno::Reference< css::lang::XServiceInfo > xInfo(xComponent, css::uno::UNO_QUERY);
    if (!xInfo.is())
        return ::rtl::OUString();

    const css::uno::Sequence< ::rtl::OUString > lKnownModules = getElementNames();
    const ::rtl::OUString*                      pKnownModules = lKnownModules.getConstArray();
          sal_Int32                             c             = lKnownModules.getLength();
          sal_Int32                             i             = 0;
      
    for (i=0; i<c; ++i)
    {
        if (xInfo->supportsService(pKnownModules[i]))
            return pKnownModules[i];
    }

    return ::rtl::OUString();
}

} // namespace framework
