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
#include "precompiled_extensions.hxx"

#include <memory>

#include "updatecheck.hxx"
#include "updatecheckconfig.hxx"
#include "updatehdl.hxx"
#include "updateprotocol.hxx"

#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implementationentry.hxx>

#include "com/sun/star/frame/XDesktop.hpp"
#include "com/sun/star/frame/XTerminateListener.hpp"
#include <com/sun/star/task/XJob.hpp>

namespace beans = com::sun::star::beans ;
namespace frame = com::sun::star::frame ;
namespace lang = com::sun::star::lang ;
namespace task = com::sun::star::task ;
namespace uno = com::sun::star::uno ;

#define UNISTRING(s) rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(s))

namespace
{

class UpdateCheckJob :
    public ::cppu::WeakImplHelper3< task::XJob, lang::XServiceInfo, frame::XTerminateListener >
{
    virtual ~UpdateCheckJob();

public:

    UpdateCheckJob(const uno::Reference<uno::XComponentContext>& xContext);

    static uno::Sequence< rtl::OUString > getServiceNames();
    static rtl::OUString getImplName();

    // Allows runtime exceptions to be thrown by const methods
    inline SAL_CALL operator uno::Reference< uno::XInterface > () const
        { return const_cast< cppu::OWeakObject * > (static_cast< cppu::OWeakObject const * > (this)); };

    // XJob
    virtual uno::Any SAL_CALL execute(const uno::Sequence<beans::NamedValue>&);

    // XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName();
    virtual sal_Bool SAL_CALL supportsService(rtl::OUString const & serviceName);
    virtual uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames();

    // XEventListener
    virtual void SAL_CALL disposing( ::com::sun::star::lang::EventObject const & evt );

    // XTerminateListener
    virtual void SAL_CALL queryTermination( lang::EventObject const & evt );
    virtual void SAL_CALL notifyTermination( lang::EventObject const & evt );

private:
    uno::Reference<uno::XComponentContext>  m_xContext;
    uno::Reference< frame::XDesktop >       m_xDesktop;

    void handleExtensionUpdates( const uno::Sequence< beans::NamedValue > &rListProp );
};

//------------------------------------------------------------------------------

UpdateCheckJob::UpdateCheckJob( const uno::Reference<uno::XComponentContext>& xContext ) :
    m_xContext(xContext)
{
    m_xDesktop.set( xContext->getServiceManager()->createInstanceWithContext( UNISTRING("com.sun.star.frame.Desktop"), xContext ), uno::UNO_QUERY );
    if ( m_xDesktop.is() )
        m_xDesktop->addTerminateListener( this );
}

//------------------------------------------------------------------------------

UpdateCheckJob::~UpdateCheckJob()
{
}

//------------------------------------------------------------------------------

uno::Sequence< rtl::OUString >
UpdateCheckJob::getServiceNames()
{
    uno::Sequence< rtl::OUString > aServiceList(1);
    aServiceList[0] = UNISTRING( "com.sun.star.setup.UpdateCheck");
    return aServiceList;
};

//------------------------------------------------------------------------------

rtl::OUString
UpdateCheckJob::getImplName()
{
    return UNISTRING( "vnd.sun.UpdateCheck");
}


//------------------------------------------------------------------------------

uno::Any
UpdateCheckJob::execute(const uno::Sequence<beans::NamedValue>& namedValues)
{
    for ( sal_Int32 n=namedValues.getLength(); n-- > 0; )
    {
        if ( namedValues[ n ].Name.equalsAscii( "DynamicData" ) )
        {
            uno::Sequence<beans::NamedValue> aListProp;
            if ( namedValues[n].Value >>= aListProp )
            {
                for ( sal_Int32 i=aListProp.getLength(); i-- > 0; )
                {
                    if ( aListProp[ i ].Name.equalsAscii( "updateList" ) )
                    {
                        handleExtensionUpdates( aListProp );
                        return uno::Any();
                    }
                }
            }
        }
    }

    uno::Sequence<beans::NamedValue> aConfig =
        getValue< uno::Sequence<beans::NamedValue> > (namedValues, "JobConfig");

    rtl::Reference< UpdateCheck > aController( UpdateCheck::get() );
    aController->initialize( aConfig, m_xContext );
    aController->showDialog( true );

    return uno::Any();
}

//------------------------------------------------------------------------------
void UpdateCheckJob::handleExtensionUpdates( const uno::Sequence< beans::NamedValue > &rListProp )
{
    try {
        uno::Sequence< uno::Sequence< rtl::OUString > > aList =
            getValue< uno::Sequence< uno::Sequence< rtl::OUString > > > ( rListProp, "updateList" );
        bool bPrepareOnly = getValue< bool > ( rListProp, "prepareOnly" );

        // we will first store any new found updates and then check, if there are any
        // pending updates.
        storeExtensionUpdateInfos( m_xContext, aList );

        if ( bPrepareOnly )
            return;

        bool bHasUpdates = checkForPendingUpdates( m_xContext );

        rtl::Reference<UpdateCheck> aController( UpdateCheck::get() );
        if ( ! aController.is() )
            return;

        aController->setHasExtensionUpdates( bHasUpdates );

        if ( ! aController->hasOfficeUpdate() )
        {
            if ( bHasUpdates )
                aController->setUIState( UPDATESTATE_EXT_UPD_AVAIL, true );
            else
                aController->setUIState( UPDATESTATE_NO_UPDATE_AVAIL, true );
        }
    }
    catch( const uno::Exception& e )
    {
         OSL_TRACE( "Caught exception: %s\n thread terminated.\n",
            rtl::OUStringToOString(e.Message, RTL_TEXTENCODING_UTF8).getStr());
    }
}

//------------------------------------------------------------------------------

rtl::OUString SAL_CALL
UpdateCheckJob::getImplementationName()
{
    return getImplName();
}

//------------------------------------------------------------------------------

uno::Sequence< rtl::OUString > SAL_CALL
UpdateCheckJob::getSupportedServiceNames()
{
    return getServiceNames();
}

//------------------------------------------------------------------------------

sal_Bool SAL_CALL
UpdateCheckJob::supportsService( rtl::OUString const & serviceName )
{
    uno::Sequence< rtl::OUString > aServiceNameList = getServiceNames();

    for( sal_Int32 n=0; n < aServiceNameList.getLength(); n++ )
        if( aServiceNameList[n].equals(serviceName) )
            return sal_True;

    return sal_False;
}

//------------------------------------------------------------------------------
// XEventListener
void SAL_CALL UpdateCheckJob::disposing( lang::EventObject const & rEvt )
{
    bool shutDown = ( rEvt.Source == m_xDesktop );

    if ( shutDown && m_xDesktop.is() )
    {
        m_xDesktop->removeTerminateListener( this );
        m_xDesktop.clear();
    }
}

//------------------------------------------------------------------------------
// XTerminateListener
void SAL_CALL UpdateCheckJob::queryTermination( lang::EventObject const & )
{
}

//------------------------------------------------------------------------------
void SAL_CALL UpdateCheckJob::notifyTermination( lang::EventObject const & )
{
}

} // anonymous namespace

//------------------------------------------------------------------------------

static uno::Reference<uno::XInterface> SAL_CALL
createJobInstance(const uno::Reference<uno::XComponentContext>& xContext)
{
    return *new UpdateCheckJob(xContext);
}

//------------------------------------------------------------------------------

static uno::Reference<uno::XInterface> SAL_CALL
createConfigInstance(const uno::Reference<uno::XComponentContext>& xContext)
{
    return *UpdateCheckConfig::get(xContext, *UpdateCheck::get());
}

//------------------------------------------------------------------------------

static const cppu::ImplementationEntry kImplementations_entries[] =
{
    {
        createJobInstance,
        UpdateCheckJob::getImplName,
        UpdateCheckJob::getServiceNames,
        cppu::createSingleComponentFactory,
        NULL,
        0
    },
    {
        createConfigInstance,
        UpdateCheckConfig::getImplName,
        UpdateCheckConfig::getServiceNames,
        cppu::createSingleComponentFactory,
        NULL,
        0
    },
	{ NULL, NULL, NULL, NULL, NULL, 0 }
} ;

//------------------------------------------------------------------------------

extern "C" void SAL_CALL
component_getImplementationEnvironment( const sal_Char **aEnvTypeName, uno_Environment **)
{
    *aEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME ;
}

//------------------------------------------------------------------------------

extern "C" void *
component_getFactory(const sal_Char *pszImplementationName, void *pServiceManager, void *pRegistryKey)
{
    return cppu::component_getFactoryHelper(
        pszImplementationName,
        pServiceManager,
        pRegistryKey,
        kImplementations_entries) ;
}
