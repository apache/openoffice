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

#include "dp_misc.h"
#include "unopkg_main.h"
#include "unopkg_shared.h"
#include "dp_identifier.hxx"
#include "sal/main.h"
#include "tools/extendapplicationenvironment.hxx"
#include "rtl/ustrbuf.hxx"
#include "rtl/uri.hxx"
#include "rtl/bootstrap.hxx"
#include "osl/thread.h"
#include "osl/process.h"
#include "osl/conditn.hxx"
#include "osl/file.hxx"
#include "cppuhelper/implbase1.hxx"
#include "cppuhelper/exc_hlp.hxx"
#include "comphelper/anytostring.hxx"
#include "comphelper/sequence.hxx"
#include "com/sun/star/deployment/ExtensionManager.hpp"

#include "com/sun/star/deployment/ui/PackageManagerDialog.hpp"
#include "com/sun/star/ui/dialogs/XExecutableDialog.hpp"
#include "com/sun/star/lang/DisposedException.hpp"
#include "boost/scoped_array.hpp"
#include "com/sun/star/ui/dialogs/XDialogClosedListener.hpp"
#include "com/sun/star/bridge/XBridgeFactory.hpp"
#include <stdio.h>
#include <vector>


using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::unopkg;
using ::rtl::OUString;
namespace css = ::com::sun::star;
namespace {

struct ExtensionName
{
    OUString m_str;
    ExtensionName( OUString const & str ) : m_str( str ) {}
    bool operator () ( Reference<deployment::XPackage> const & e ) const
    {
        if (m_str.equals(dp_misc::getIdentifier(e))
             || m_str.equals(e->getName()))
            return true;
        return false;
    }
};

//------------------------------------------------------------------------------
const char s_usingText [] =
"\n"
"using: " APP_NAME " add <options> extension-path...\n"
"       " APP_NAME " validate <options> extension-identifier...\n"
"       " APP_NAME " remove <options> extension-identifier...\n"
"       " APP_NAME " list <options> extension-identifier...\n"
"       " APP_NAME " reinstall <options>\n"
"       " APP_NAME " gui\n"
"       " APP_NAME " -V\n"
"       " APP_NAME " -h\n"
"\n"
"sub-commands:\n"
" add                     add extension\n"
" validate                checks the prerequisites of an installed extension and"
"                         registers it if possible\n"
" remove                  remove extensions by identifier\n"
" reinstall               expert feature: reinstall all deployed extensions\n"
" list                    list information about deployed extensions\n"
" gui                     raise Extension Manager Graphical User Interface (GUI)\n"
"\n"
"options:\n"
" -h, --help              this help\n"
" -V, --version           version information\n"
" -v, --verbose           verbose output to stdout\n"
" -f, --force             force overwriting existing extensions\n"
" -s, --suppress-license  prevents showing the license provided that\n"
"                         the extension allows it\n"
" --log-file <file>       custom log file; default: <cache-dir>/log.txt\n"
" --shared                expert feature: operate on shared installation\n"
"                                         deployment context;\n"
"                                         run only when no concurrent Office\n"
"                                         process(es) are running!\n"
" --bundled               expert feature: operate on bundled extensions. Only\n"
"                                         works with list, validate, reinstall;\n"
" --deployment-context    expert feature: explicit deployment context\n"
"     <context>\n"
"\n"
"To learn more about the Extension Manager and extensions, see:\n"
"http://wiki.services.openoffice.org/wiki/Documentation/DevGuide/Extensions/Extensions\n\n";

//------------------------------------------------------------------------------
const OptionInfo s_option_infos [] = {
    { RTL_CONSTASCII_STRINGPARAM("help"), 'h', false },
    { RTL_CONSTASCII_STRINGPARAM("version"), 'V', false },
    { RTL_CONSTASCII_STRINGPARAM("verbose"), 'v', false },
    { RTL_CONSTASCII_STRINGPARAM("force"), 'f', false },
    { RTL_CONSTASCII_STRINGPARAM("log-file"), '\0', true },
    { RTL_CONSTASCII_STRINGPARAM("shared"), '\0', false },
    { RTL_CONSTASCII_STRINGPARAM("deployment-context"), '\0', true },
    { RTL_CONSTASCII_STRINGPARAM("bundled"), '\0', false},
    { RTL_CONSTASCII_STRINGPARAM("suppress-license"), 's', false},

    { 0, 0, '\0', false }
};

class DialogClosedListenerImpl :
    public ::cppu::WeakImplHelper1< ui::dialogs::XDialogClosedListener >
{
    osl::Condition & m_rDialogClosedCondition;

public:
    DialogClosedListenerImpl( osl::Condition & rDialogClosedCondition )
        : m_rDialogClosedCondition( rDialogClosedCondition ) {}

    // XEventListener (base of XDialogClosedListener)
    virtual void SAL_CALL disposing( lang::EventObject const & Source )
        throw (RuntimeException);

    // XDialogClosedListener
    virtual void SAL_CALL dialogClosed(
        ui::dialogs::DialogClosedEvent const & aEvent )
        throw (RuntimeException);
};

// XEventListener (base of XDialogClosedListener)
void DialogClosedListenerImpl::disposing( lang::EventObject const & )
    throw (RuntimeException)
{
    // nothing to do
}

// XDialogClosedListener
void DialogClosedListenerImpl::dialogClosed(
    ui::dialogs::DialogClosedEvent const & )
    throw (RuntimeException)
{
    m_rDialogClosedCondition.set();
}

// If a package had been installed with a pre OOo 2.2, it could not normally be
// found via its identifier; similarly (and for ease of use), a package
// installed with OOo 2.2 or later could not normally be found via its file
// name.
Reference<deployment::XPackage> findPackage(
    OUString const & repository,
    Reference<deployment::XExtensionManager> const & manager,
    Reference<ucb::XCommandEnvironment > const & environment,
    OUString const & idOrFileName )
{
    Sequence< Reference<deployment::XPackage> > ps(
        manager->getDeployedExtensions(repository,
            Reference<task::XAbortChannel>(), environment ) );
    for ( sal_Int32 i = 0; i < ps.getLength(); ++i )
        if ( dp_misc::getIdentifier( ps[i] ) == idOrFileName )
            return ps[i];
    for ( sal_Int32 i = 0; i < ps.getLength(); ++i )
        if ( ps[i]->getName() == idOrFileName )
            return ps[i];
    return Reference<deployment::XPackage>();
}

} // anon namespace


//workaround for some reason the bridge threads which communicate with the uno.exe
//process are not released on time
void disposeBridges(Reference<css::uno::XComponentContext> ctx)
{
    if (!ctx.is())
        return;

    Reference<css::bridge::XBridgeFactory> bridgeFac(
        ctx->getServiceManager()->createInstanceWithContext(
            OUSTR("com.sun.star.bridge.BridgeFactory"), ctx),
        UNO_QUERY);

    if (bridgeFac.is())
    {
        const Sequence< Reference<css::bridge::XBridge> >seqBridges = bridgeFac->getExistingBridges();
        for (sal_Int32 i = 0; i < seqBridges.getLength(); i++)
        {
            Reference<css::lang::XComponent> comp(seqBridges[i], UNO_QUERY);
            if (comp.is())
            {
                try {
                    comp->dispose();
                }
                catch (css::lang::DisposedException& )
                {
                }
            }
        }
    }
}

//##############################################################################
extern "C" int unopkg_main()
{
    tools::extendApplicationEnvironment();
    DisposeGuard disposeGuard;
    bool bNoOtherErrorMsg = false;
    OUString subCommand;
    bool option_shared = false;
    bool option_force = false;
    bool option_verbose = false;
    bool option_bundled = false;
    bool option_suppressLicense = false;
    bool subcmd_add = false;
	bool subcmd_gui = false;
    OUString logFile;
    OUString repository;
    OUString cmdArg;
    ::std::vector<OUString> cmdPackages;

    OptionInfo const * info_shared = getOptionInfo(
        s_option_infos, OUSTR("shared") );
    OptionInfo const * info_force = getOptionInfo(
        s_option_infos, OUSTR("force") );
    OptionInfo const * info_verbose = getOptionInfo(
        s_option_infos, OUSTR("verbose") );
    OptionInfo const * info_log = getOptionInfo(
        s_option_infos, OUSTR("log-file") );
    OptionInfo const * info_context = getOptionInfo(
        s_option_infos, OUSTR("deployment-context") );
    OptionInfo const * info_help = getOptionInfo(
        s_option_infos, OUSTR("help") );
    OptionInfo const * info_version = getOptionInfo(
        s_option_infos, OUSTR("version") );
    OptionInfo const * info_bundled = getOptionInfo(
        s_option_infos, OUSTR("bundled") );
    OptionInfo const * info_suppressLicense = getOptionInfo(
        s_option_infos, OUSTR("suppress-license") );


    Reference<XComponentContext> xComponentContext;
    Reference<XComponentContext> xLocalComponentContext;

    try {
        sal_uInt32 nPos = 0;
        sal_uInt32 nCount = osl_getCommandArgCount();
        if (nCount == 0 || isOption( info_help, &nPos ))
        {
            dp_misc::writeConsole(s_usingText);
            return 0;
        }
        else if (isOption( info_version, &nPos )) {
            dp_misc::writeConsole( "\n" APP_NAME " Version 3.3\n");
            return 0;
        }
        // consume all bootstrap variables which may occur before the subcommand
        while(isBootstrapVariable(&nPos));

        if(nPos >= nCount)
            return 0;
        //get the sub command
        osl_getCommandArg( nPos, &subCommand.pData );
        ++nPos;
        subCommand = subCommand.trim();
        subcmd_add = subCommand.equalsAsciiL(
            RTL_CONSTASCII_STRINGPARAM("add") );
		subcmd_gui = subCommand.equalsAsciiL(
            RTL_CONSTASCII_STRINGPARAM("gui") );

        // sun-command options and packages:
        while (nPos < nCount)
        {
            if (readArgument( &cmdArg, info_log, &nPos )) {
                logFile = makeAbsoluteFileUrl(
                    cmdArg.trim(), getProcessWorkingDir() );
            }
            else if (!readOption( &option_verbose, info_verbose, &nPos ) &&
                     !readOption( &option_shared, info_shared, &nPos ) &&
                     !readOption( &option_force, info_force, &nPos ) &&
                     !readOption( &option_bundled, info_bundled, &nPos ) &&
                     !readOption( &option_suppressLicense, info_suppressLicense, &nPos ) &&
                     !readArgument( &repository, info_context, &nPos ) &&
                     !isBootstrapVariable(&nPos))
            {
                osl_getCommandArg( nPos, &cmdArg.pData );
                ++nPos;
                cmdArg = cmdArg.trim();
                if (cmdArg.getLength() > 0)
                {
                    if (cmdArg[ 0 ] == '-')
                    {
                        // is option:
                        dp_misc::writeConsoleError(
                                 OUSTR("\nERROR: unexpected option ") +
                                 cmdArg +
                                 OUSTR("!\n") +
                                 OUSTR("       Use " APP_NAME " ") +
                                 toString(info_help) +
                                 OUSTR(" to print all options.\n"));
                        return 1;
                    }
                    else
                    {
                        // is package:
                        cmdPackages.push_back(
                            subcmd_add || subcmd_gui
                            ? makeAbsoluteFileUrl(
                                cmdArg, getProcessWorkingDir() )
                            : cmdArg );
                    }
                }
            }
        }

        if (repository.getLength() == 0)
        {
            if (option_shared)
                repository = OUSTR("shared");
            else if (option_bundled)
                repository = OUSTR("bundled");
            else
                repository = OUSTR("user");
        }
        else
        {
            if (repository.equalsAsciiL(
                    RTL_CONSTASCII_STRINGPARAM("shared") )) {
                option_shared = true;
            }
            else if (option_shared) {
                dp_misc::writeConsoleError(
                    OUSTR("WARNING: explicit context given!  ") +
                    OUSTR("Ignoring option ") +
                    toString( info_shared ) +
                    OUSTR("!\n") );
            }
        }

        if (subCommand.equals(OUSTR("reinstall")))
        {
            //We must prevent that services and types are loaded by UNO,
            //otherwise we cannot delete the registry data folder.
            OUString extensionUnorc;
            if (repository.equals(OUSTR("user")))
                extensionUnorc = OUSTR("$UNO_USER_PACKAGES_CACHE/registry/com.sun.star.comp.deployment.component.PackageRegistryBackend/unorc");
            else if (repository.equals(OUSTR("shared")))
                extensionUnorc = OUSTR("$SHARED_EXTENSIONS_USER/registry/com.sun.star.comp.deployment.component.PackageRegistryBackend/unorc");
            else if (repository.equals(OUSTR("bundled")))
                extensionUnorc = OUSTR("$BUNDLED_EXTENSIONS_USER/registry/com.sun.star.comp.deployment.component.PackageRegistryBackend/unorc");
            else
                OSL_ASSERT(0);

            ::rtl::Bootstrap::expandMacros(extensionUnorc);
            oslFileError e = osl_removeFile(extensionUnorc.pData);
            if (e != osl_File_E_None && e != osl_File_E_NOENT)
                throw Exception(OUSTR("Could not delete ") + extensionUnorc, 0);
        }
        else if (subCommand.equals(OUSTR("sync")))
        {
            //sync is private!!!! Only to be called from setup!!!
            //The UserInstallation is diverted to the prereg folder. But only
            //the lock file is written! This requires that
            //-env:UNO_JAVA_JFW_INSTALL_DATA is passed to javaldx and unopkg otherwise the
            //javasettings file is written to the prereg folder.
            //
            //For performance reasons unopkg sync is called during the setup and
            //creates the registration data for the repository of the bundled
            //extensions. It is then copied to the user installation during
            //startup of AOO (userdata/extensions/bundled). The registration
            //data is in the brand installation and must be removed when
            //uninstalling AOO. We do this here, before UNO is
            //bootstrapped. Otherwise files could be locked by this process.

            //If there is no folder left in
            //$OOO_BASE_DIR/share/extensions
            //then we can delete the registration data at
            //$BUNDLED_EXTENSIONS_USER
            if (hasNoFolder(OUSTR("$OOO_BASE_DIR/share/extensions")))
            {
                removeFolder(OUSTR("$BUNDLED_EXTENSIONS_PREREG"));
                //return otherwise we create the registration data again
                return 0;
            }
            //redirect the UserInstallation, so we do not create a
            //user installation for the admin and we also do not need
            //to call unopkg with -env:UserInstallation
            ::rtl::Bootstrap::set(OUSTR("UserInstallation"),
                                  OUSTR("$BUNDLED_EXTENSIONS_PREREG/.."));
            //Setting UNO_JAVA_JFW_INSTALL_DATA causes the javasettings to be written
            //in the office installation. We do not want to create the user data folder
            //for the admin. The value must also be set in the unopkg script (Linux, etc.)
            //when calling javaldx
            ::rtl::Bootstrap::set(OUSTR("UNO_JAVA_JFW_INSTALL_DATA"),
                                  OUSTR("$OOO_BASE_DIR/share/config/javasettingsunopkginstall.xml"));

        }

        xComponentContext = getUNO(
            disposeGuard, option_verbose, option_shared, subcmd_gui,
            xLocalComponentContext );

        Reference<deployment::XExtensionManager> xExtensionManager(
            deployment::ExtensionManager::get( xComponentContext ) );

        Reference< ::com::sun::star::ucb::XCommandEnvironment > xCmdEnv(
            createCmdEnv( xComponentContext, logFile,
                          option_force, option_verbose) );

        //synchronize bundled/shared extensions
        //Do not synchronize when command is "reinstall". This could add types and services to UNO and
        //prevent the deletion of the registry data folder
        //synching is done in XExtensionManager.reinstall
        if (!subcmd_gui && ! subCommand.equals(OUSTR("reinstall"))
            && ! subCommand.equals(OUSTR("sync"))
            && ! dp_misc::office_is_running())
            dp_misc::syncRepositories(xCmdEnv);

        if (subcmd_add ||
            subCommand.equalsAsciiL(
                RTL_CONSTASCII_STRINGPARAM("remove") ))
        {
            for ( ::std::size_t pos = 0; pos < cmdPackages.size(); ++pos )
            {
                OUString const & cmdPackage = cmdPackages[ pos ];
                if (subcmd_add)
                {
                    beans::NamedValue nvSuppress(
                        OUSTR("SUPPRESS_LICENSE"), option_suppressLicense ?
                        makeAny(OUSTR("1")):makeAny(OUSTR("0")));
                        xExtensionManager->addExtension(
                            cmdPackage, Sequence<beans::NamedValue>(&nvSuppress, 1),
                            repository, Reference<task::XAbortChannel>(), xCmdEnv);
                }
                else
                {
                    try
                    {
                        xExtensionManager->removeExtension(
                            cmdPackage, cmdPackage, repository,
                            Reference<task::XAbortChannel>(), xCmdEnv );
                    }
                    catch (lang::IllegalArgumentException &)
                    {
                        Reference<deployment::XPackage> p(
                             findPackage(repository,
                                xExtensionManager, xCmdEnv, cmdPackage ) );
                        if ( !p.is())
                            throw;
                        else if (p.is())
                            xExtensionManager->removeExtension(
                                ::dp_misc::getIdentifier(p), p->getName(),
                                repository,
                                Reference<task::XAbortChannel>(), xCmdEnv );
                    }
                }
            }
        }
        else if (subCommand.equalsAsciiL(
                     RTL_CONSTASCII_STRINGPARAM("reinstall") ))
        {
            xExtensionManager->reinstallDeployedExtensions(
                repository, Reference<task::XAbortChannel>(), xCmdEnv);
        }
        else if (subCommand.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("list") ))
        {
            ::std::vector<Reference<deployment::XPackage> > vecExtUnaccepted;
            ::comphelper::sequenceToContainer(vecExtUnaccepted,
                    xExtensionManager->getExtensionsWithUnacceptedLicenses(
                        repository, xCmdEnv));

            //This vector tells what XPackage in allExtensions has an
            //unaccepted license.
            std::vector<bool> vecUnaccepted;
            std::vector<Reference<deployment::XPackage> > allExtensions;
            if (cmdPackages.empty())
            {
                Sequence< Reference<deployment::XPackage> >
                    packages = xExtensionManager->getDeployedExtensions(
                        repository, Reference<task::XAbortChannel>(), xCmdEnv );

                ::std::vector<Reference<deployment::XPackage> > vec_packages;
                ::comphelper::sequenceToContainer(vec_packages, packages);

                //First copy the extensions with the unaccepted license
                //to vector allExtensions.
                allExtensions.resize(vecExtUnaccepted.size() + vec_packages.size());

                ::std::vector<Reference<deployment::XPackage> >::iterator i_all_ext =
                      ::std::copy(vecExtUnaccepted.begin(), vecExtUnaccepted.end(),
                                  allExtensions.begin());
                //Now copy those we got from getDeployedExtensions
                ::std::copy(vec_packages.begin(), vec_packages.end(), i_all_ext);

                //Now prepare the vector which tells what extension has an
                //unaccepted license
                vecUnaccepted.resize(vecExtUnaccepted.size() + vec_packages.size());
                ::std::fill_n( vecUnaccepted.begin(), vecExtUnaccepted.size(), true);
                std::vector<bool>::iterator i_unaccepted = vecUnaccepted.begin() + vecExtUnaccepted.size();
                ::std::fill_n(i_unaccepted, vec_packages.size(), false);

                dp_misc::writeConsole(
                    OUSTR("All deployed ") + repository + OUSTR(" extensions:\n\n"));
            }
            else
            {
                //The user provided the names (ids or file names) of the extensions
                //which shall be listed
                for ( ::std::size_t pos = 0; pos < cmdPackages.size(); ++pos )
                {
                    Reference<deployment::XPackage> extension;
                    try
                    {
                        extension = xExtensionManager->getDeployedExtension(
                            repository, cmdPackages[ pos ], cmdPackages[ pos ], xCmdEnv );
                    }
                    catch (lang::IllegalArgumentException &)
                    {
                        extension = findPackage(repository,
                            xExtensionManager, xCmdEnv, cmdPackages[ pos ] );
                    }

                    //Now look if the requested extension has an unaccepted license
                    bool bUnacceptedLic = false;
                    if (!extension.is())
                    {
                        ::std::vector<Reference<deployment::XPackage> >::const_iterator
                            i = ::std::find_if(
                                vecExtUnaccepted.begin(),
                                vecExtUnaccepted.end(), ExtensionName(cmdPackages[pos]));
                        if (i != vecExtUnaccepted.end())
                        {
                            extension = *i;
                            bUnacceptedLic = true;
                        }
                    }

                    if (extension.is())
                    {
                        allExtensions.push_back(extension);
                        vecUnaccepted.push_back(bUnacceptedLic);
                    }

                    else
                        throw lang::IllegalArgumentException(
                            OUSTR("There is no such extension deployed: ") +
                            cmdPackages[pos],0,-1);
                }

            }

            printf_packages(allExtensions, vecUnaccepted, xCmdEnv );
        }
        else if (subCommand.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("validate") ))
        {
            ::std::vector<Reference<deployment::XPackage> > vecExtUnaccepted;
            ::comphelper::sequenceToContainer(
                vecExtUnaccepted, xExtensionManager->getExtensionsWithUnacceptedLicenses(
                    repository, xCmdEnv));

            for ( ::std::size_t pos = 0; pos < cmdPackages.size(); ++pos )
            {
                Reference<deployment::XPackage> extension;
                try
                {
                    extension = xExtensionManager->getDeployedExtension(
                        repository, cmdPackages[ pos ], cmdPackages[ pos ], xCmdEnv );
                }
                catch (lang::IllegalArgumentException &)
                {
                    extension = findPackage(
                        repository, xExtensionManager, xCmdEnv, cmdPackages[ pos ] );
                }

                if (!extension.is())
                {
                    ::std::vector<Reference<deployment::XPackage> >::const_iterator
                        i = ::std::find_if(
                            vecExtUnaccepted.begin(),
                            vecExtUnaccepted.end(), ExtensionName(cmdPackages[pos]));
                    if (i != vecExtUnaccepted.end())
                    {
                        extension = *i;
                    }
                }

                if (extension.is())
                    xExtensionManager->checkPrerequisitesAndEnable(
                        extension, Reference<task::XAbortChannel>(), xCmdEnv);
            }
        }
        else if (subCommand.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("gui") ))
        {
            Reference<ui::dialogs::XAsynchronousExecutableDialog> xDialog(
                deployment::ui::PackageManagerDialog::createAndInstall(
                    xComponentContext,
                    cmdPackages.size() > 0 ? cmdPackages[0] : OUString() ));

            osl::Condition dialogEnded;
            dialogEnded.reset();

            Reference< ui::dialogs::XDialogClosedListener > xListener(
                new DialogClosedListenerImpl( dialogEnded ) );

            xDialog->startExecuteModal(xListener);
            dialogEnded.wait();
        }
        else if (subCommand.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("sync")))
        {
            if (! dp_misc::office_is_running())
            {
                xExtensionManager->synchronizeBundledPrereg(
                    Reference<task::XAbortChannel>(), xCmdEnv);
            }
            else
            {
                dp_misc::writeConsoleError(OUSTR("\nError: office is running"));
            }
        }
        else
        {
            dp_misc::writeConsoleError(
                OUSTR("\nERROR: unknown sub-command ") +
                subCommand +
                OUSTR("!\n") +
                OUSTR("       Use " APP_NAME " ") +
                toString(info_help) +
                OUSTR(" to print all options.\n"));
            return 1;
        }

        if (option_verbose)
            dp_misc::writeConsole( OUSTR( "\n" APP_NAME " done.\n"));
        //Force to release all bridges which connect us to the child processes
        disposeBridges(xLocalComponentContext);
        return 0;
    }
    catch (ucb::CommandFailedException &e)
    {
        dp_misc::writeConsoleError(e.Message + OUSTR("\n"));
        bNoOtherErrorMsg = true;
    }
    catch (ucb::CommandAbortedException &)
    {
        dp_misc::writeConsoleError( "\n" APP_NAME " aborted!\n");
    }
    catch (deployment::DeploymentException & exc)
    {
		OUString cause;
		if (option_verbose)
		{
			cause = ::comphelper::anyToString(exc.Cause);
		}
		else
		{
			css::uno::Exception e;
			if (exc.Cause >>= e)
				cause = e.Message;
		}

		dp_misc::writeConsoleError(
            OUSTR("\nERROR: ") + exc.Message + OUSTR("\n"));
		if (cause.getLength())
			dp_misc::writeConsoleError(
				OUSTR("       Cause: ") + cause + OUSTR("\n"));
    }
    catch (LockFileException & e)
    {
        if (!subcmd_gui)
            dp_misc::writeConsoleError(e.Message);
        bNoOtherErrorMsg = true;
    }
    catch (::com::sun::star::uno::Exception & e ) {
        Any exc( ::cppu::getCaughtException() );

        dp_misc::writeConsoleError(
            OUSTR("\nERROR: ") +
            OUString(option_verbose  ? e.Message + OUSTR("\nException details: \n") +
            ::comphelper::anyToString(exc) : e.Message) +
            OUSTR("\n"));
    }
    if (!bNoOtherErrorMsg)
        dp_misc::writeConsoleError( "\n" APP_NAME " failed.\n");
    disposeBridges(xLocalComponentContext);
    return 1;
}

