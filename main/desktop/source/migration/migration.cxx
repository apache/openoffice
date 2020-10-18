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
#include "precompiled_desktop.hxx"

#include <map>
#include <new>
#include <set>

#include "migration.hxx"
#include "migration_impl.hxx"
#include "cfgfilter.hxx"

#include <unotools/textsearch.hxx>
#include <comphelper/processfactory.hxx>
#include <comphelper/sequence.hxx>
#include <unotools/bootstrap.hxx>
#include <rtl/bootstrap.hxx>
#include <rtl/uri.hxx>
#include <tools/config.hxx>
#include <i18npool/lang.h>
#include <tools/urlobj.hxx>
#include <osl/file.hxx>
#include <osl/mutex.hxx>
#include <ucbhelper/content.hxx>
#include <osl/security.hxx>
#include <unotools/configmgr.hxx>

#include <com/sun/star/configuration/Update.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/task/XJob.hpp>
#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/util/XRefreshable.hpp>
#include <com/sun/star/util/XChangesBatch.hpp>
#include <com/sun/star/util/XStringSubstitution.hpp>
#include <com/sun/star/embed/ElementModes.hpp>
#include <com/sun/star/embed/XStorage.hpp>
#include <com/sun/star/ui/XUIConfiguration.hpp>
#include <com/sun/star/ui/XUIConfigurationStorage.hpp>
#include <com/sun/star/ui/XUIConfigurationPersistence.hpp>

using namespace rtl;
using namespace osl;
using namespace std;
using namespace com::sun::star::task;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::util;
using namespace com::sun::star::container;
using com::sun::star::uno::Exception;
using namespace com::sun::star;

namespace desktop {

static const ::rtl::OUString ITEM_DESCRIPTOR_COMMANDURL = ::rtl::OUString::createFromAscii("CommandURL");
static const ::rtl::OUString ITEM_DESCRIPTOR_CONTAINER = ::rtl::OUString::createFromAscii("ItemDescriptorContainer");
static const ::rtl::OUString ITEM_DESCRIPTOR_LABEL = ::rtl::OUString::createFromAscii("Label");

static const ::rtl::OUString MENU_SEPERATOR = ::rtl::OUString::createFromAscii(" | ");
static const ::rtl::OUString MENU_SUBMENU = ::rtl::OUString::createFromAscii("...");

::rtl::OUString retrieveLabelFromCommand(const ::rtl::OUString& sCommand, const ::rtl::OUString& sModuleIdentifier)
{
	::rtl::OUString sLabel;

	uno::Reference< container::XNameAccess > xUICommands;
	uno::Reference< container::XNameAccess > xNameAccess( ::comphelper::getProcessServiceFactory()->createInstance( ::rtl::OUString::createFromAscii("com.sun.star.frame.UICommandDescription") ), uno::UNO_QUERY );
	if ( xNameAccess.is() )
	{
		uno::Any a = xNameAccess->getByName( sModuleIdentifier );
		a >>= xUICommands;
	}
	if (xUICommands.is())
	{
		if ( sCommand.getLength() > 0 )
		{
			rtl::OUString aStr;
			::uno::Sequence< beans::PropertyValue > aPropSeq;
			try
			{
				uno::Any a( xUICommands->getByName( sCommand ));
				if ( a >>= aPropSeq )
				{
					for ( sal_Int32 i = 0; i < aPropSeq.getLength(); i++ )
					{
						if ( aPropSeq[i].Name.equalsAscii( "Label" ))
						{
							aPropSeq[i].Value >>= aStr;
							break;
						}
					}
				}

				sLabel = aStr;
			}

			catch(container::NoSuchElementException&)
			{
				sLabel = sCommand;
				sal_Int32 nIndex = sLabel.indexOf(':');
				if (nIndex>=0 && nIndex <= sLabel.getLength()-1)
					sLabel = sLabel.copy(nIndex+1);
			}

		}
	}

	return sLabel;
}

::rtl::OUString stripHotKey( const ::rtl::OUString& str )
{
	sal_Int32 index = str.indexOf( '~' );
	if ( index == -1 )
	{
		return str;
	}
	else
	{
		return str.replaceAt( index, 1, ::rtl::OUString() );
	}
}

::rtl::OUString mapModuleShortNameToIdentifier(const ::rtl::OUString& sShortName)
{
	::rtl::OUString sIdentifier;

	if (sShortName.equals(::rtl::OUString::createFromAscii("StartModule")))
		sIdentifier = ::rtl::OUString::createFromAscii("com.sun.star.frame.StartModule");

	else if (sShortName.equals(::rtl::OUString::createFromAscii("swriter")))
		sIdentifier = ::rtl::OUString::createFromAscii("com.sun.star.text.TextDocument");

	else if (sShortName.equals(::rtl::OUString::createFromAscii("scalc")))
		sIdentifier = ::rtl::OUString::createFromAscii("com.sun.star.sheet.SpreadsheetDocument");

	else if (sShortName.equals(::rtl::OUString::createFromAscii("sdraw")))
		sIdentifier = ::rtl::OUString::createFromAscii("com.sun.star.drawing.DrawingDocument");

	else if (sShortName.equals(::rtl::OUString::createFromAscii("simpress")))
		sIdentifier = ::rtl::OUString::createFromAscii("com.sun.star.presentation.PresentationDocument");

	else if (sShortName.equals(::rtl::OUString::createFromAscii("smath")))
		sIdentifier = ::rtl::OUString::createFromAscii("com.sun.star.formula.FormulaProperties");

	else if (sShortName.equals(::rtl::OUString::createFromAscii("schart")))
		sIdentifier = ::rtl::OUString::createFromAscii("com.sun.star.chart2.ChartDocument");

	else if (sShortName.equals(::rtl::OUString::createFromAscii("BasicIDE")))
		sIdentifier = ::rtl::OUString::createFromAscii("com.sun.star.script.BasicIDE");

	else if (sShortName.equals(::rtl::OUString::createFromAscii("dbapp")))
		sIdentifier = ::rtl::OUString::createFromAscii("com.sun.star.sdb.OfficeDatabaseDocument");

	else if (sShortName.equals(::rtl::OUString::createFromAscii("sglobal")))
		sIdentifier = ::rtl::OUString::createFromAscii("com.sun.star.text.GlobalDocument");

	else if (sShortName.equals(::rtl::OUString::createFromAscii("sweb")))
		sIdentifier = ::rtl::OUString::createFromAscii("com.sun.star.text.WebDocument");

	else if (sShortName.equals(::rtl::OUString::createFromAscii("swxform")))
		sIdentifier = ::rtl::OUString::createFromAscii("com.sun.star.xforms.XMLFormDocument");

	else if (sShortName.equals(::rtl::OUString::createFromAscii("sbibliography")))
		sIdentifier = ::rtl::OUString::createFromAscii("com.sun.star.frame.Bibliography");

	return sIdentifier;
}

static MigrationImpl *pImpl = 0;
static Mutex aMutex;
static MigrationImpl *getImpl()
{
    MutexGuard aGuard(aMutex);
    if (pImpl == 0)
        pImpl = new MigrationImpl(comphelper::getProcessServiceFactory());
    return pImpl;
}

static void releaseImpl()
{
    MutexGuard aGuard(aMutex);
    if (pImpl != 0)
    {
        delete pImpl;
        pImpl = 0;
    }
}

// static main entry point for the migration process
void Migration::doMigration()
{
    sal_Bool bResult = sal_False;
    try {
        bResult = getImpl()->doMigration();
    } catch (Exception& e)
    {
        OString aMsg("doMigration() exception: ");
        aMsg += OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US);
        OSL_ENSURE(sal_False, aMsg.getStr());
    }
    OSL_ENSURE(bResult, "Migration has not been successful");
    // shut down migration framework
    releaseImpl();
}

void Migration::cancelMigration()
{
    releaseImpl();
}

sal_Bool Migration::checkMigration()
{
    return getImpl()->checkMigration();
}

OUString Migration::getOldVersionName()
{
    return getImpl()->getOldVersionName();
}

OUString MigrationImpl::getOldVersionName()
{
    return m_aInfo.productname;
}

sal_Bool MigrationImpl::checkMigration()
{
    if (m_aInfo.userdata.getLength() > 0 && ! checkMigrationCompleted())
        return sal_True;
    else
        return sal_False;
}

MigrationImpl::MigrationImpl(const uno::Reference< XMultiServiceFactory >& xFactory)
    : m_vrVersions(new strings_v)
    , m_xFactory(xFactory)
{
    readAvailableMigrations(m_vMigrationsAvailable);
    sal_Int32 nIndex = findPreferedMigrationProcess(m_vMigrationsAvailable);
    if ( nIndex >= 0 )
        m_vrMigrations = readMigrationSteps(m_vMigrationsAvailable[nIndex].name);
}

MigrationImpl::~MigrationImpl()
{

}

sal_Bool MigrationImpl::doMigration()
{
    // compile file list for migration
    m_vrFileList = compileFileList();

    sal_Bool result = sal_False;
    try
	{
		NewVersionUIInfo aNewVersionUIInfo;
		::std::vector< MigrationModuleInfo > vModulesInfo = dectectUIChangesForAllModules();
		aNewVersionUIInfo.init(vModulesInfo);

        copyFiles();

		const ::rtl::OUString sMenubarResourceURL = ::rtl::OUString::createFromAscii("private:resource/menubar/menubar");
		const ::rtl::OUString sToolbarResourcePre = ::rtl::OUString::createFromAscii("private:resource/toolbar/");
		for (sal_uInt32 i=0; i<vModulesInfo.size(); ++i)
		{
			::rtl::OUString sModuleIdentifier = mapModuleShortNameToIdentifier(vModulesInfo[i].sModuleShortName);
			if (sModuleIdentifier.getLength()==0)
				continue;

			uno::Sequence< uno::Any > lArgs(2);
			::rtl::OUString aOldCfgDataPath = m_aInfo.userdata + ::rtl::OUString::createFromAscii("/user/config/soffice.cfg/modules/");
			lArgs[0] <<= aOldCfgDataPath + vModulesInfo[i].sModuleShortName;
			lArgs[1] <<= embed::ElementModes::READ;

			uno::Reference< lang::XSingleServiceFactory > xStorageFactory(m_xFactory->createInstance(::rtl::OUString::createFromAscii("com.sun.star.embed.FileSystemStorageFactory")), uno::UNO_QUERY);
			uno::Reference< embed::XStorage >             xModules;

			xModules = uno::Reference< embed::XStorage >(xStorageFactory->createInstanceWithArguments(lArgs), uno::UNO_QUERY);
			uno::Reference< ui::XUIConfigurationManager > xOldCfgManager( m_xFactory->createInstance( rtl::OUString::createFromAscii("com.sun.star.ui.UIConfigurationManager")), uno::UNO_QUERY );
			uno::Reference< ui::XUIConfigurationStorage > xOldCfgStorage( xOldCfgManager, uno::UNO_QUERY );
			uno::Reference< ui::XUIConfigurationPersistence > xOldCfgPersistence( xOldCfgManager, uno::UNO_QUERY );

			if ( xOldCfgStorage.is() && xOldCfgPersistence.is() && xModules.is() )
			{
					xOldCfgStorage->setStorage( xModules );
					xOldCfgPersistence->reload();
			}

			uno::Reference< ui::XUIConfigurationManager > xCfgManager = aNewVersionUIInfo.getConfigManager(vModulesInfo[i].sModuleShortName);

			if (vModulesInfo[i].bHasMenubar)
			{
				uno::Reference< container::XIndexContainer > xOldVersionMenuSettings = uno::Reference< container::XIndexContainer >(xOldCfgManager->getSettings(sMenubarResourceURL, sal_True), uno::UNO_QUERY);
				uno::Reference< container::XIndexContainer > xNewVersionMenuSettings = aNewVersionUIInfo.getNewMenubarSettings(vModulesInfo[i].sModuleShortName);
				::rtl::OUString sParent;
				compareOldAndNewConfig(sParent, xOldVersionMenuSettings, xNewVersionMenuSettings, sMenubarResourceURL);
				mergeOldToNewVersion(xCfgManager, xNewVersionMenuSettings, sModuleIdentifier, sMenubarResourceURL);
			}

			sal_Int32 nToolbars = vModulesInfo[i].m_vToolbars.size();
			if (nToolbars >0)
            {
				for (sal_Int32 j=0; j<nToolbars; ++j)
				{
					::rtl::OUString sToolbarName = vModulesInfo[i].m_vToolbars[j];
					::rtl::OUString sToolbarResourceURL = sToolbarResourcePre + sToolbarName;

					uno::Reference< container::XIndexContainer > xOldVersionToolbarSettings = uno::Reference< container::XIndexContainer >(xOldCfgManager->getSettings(sToolbarResourceURL, sal_True), uno::UNO_QUERY);
					uno::Reference< container::XIndexContainer > xNewVersionToolbarSettings = aNewVersionUIInfo.getNewToolbarSettings(vModulesInfo[i].sModuleShortName, sToolbarName);
					::rtl::OUString sParent;
					compareOldAndNewConfig(sParent, xOldVersionToolbarSettings, xNewVersionToolbarSettings, sToolbarResourceURL);
					mergeOldToNewVersion(xCfgManager, xNewVersionToolbarSettings, sModuleIdentifier, sToolbarResourceURL);
				}
			}

			m_aOldVersionItemsHashMap.clear();
			m_aNewVersionItemsHashMap.clear();
		}

		// execute the migration items from Setup.xcu
        copyConfig();

		// execute custom migration services from Setup.xcu
		// and refresh the cache
        runServices();
        refresh();

        result = sal_True;
    } catch (...)
    {
        OString aMsg("An unexpected exception was thrown during migration");
        aMsg += "\nOldVersion: " + OUStringToOString(m_aInfo.productname, RTL_TEXTENCODING_ASCII_US);
        aMsg += "\nDataPath  : " + OUStringToOString(m_aInfo.userdata, RTL_TEXTENCODING_ASCII_US);
        OSL_ENSURE(sal_False, aMsg.getStr());
    }

	// prevent running the migration multiple times
	setMigrationCompleted();
    return result;
}

void MigrationImpl::refresh()
{
	uno::Reference< XRefreshable > xRefresh(m_xFactory->createInstance(
                OUString::createFromAscii("com.sun.star.configuration.ConfigurationProvider")), uno::UNO_QUERY);
    if (xRefresh.is())
        xRefresh->refresh();
    else
        OSL_ENSURE(sal_False, "could not get XRefresh interface from default config provider. No refresh done.");

}

void MigrationImpl::setMigrationCompleted()
{
	try {
		uno::Reference< XPropertySet > aPropertySet(getConfigAccess("org.openoffice.Setup/Office", true), uno::UNO_QUERY_THROW);
		aPropertySet->setPropertyValue(OUString::createFromAscii("MigrationCompleted"), uno::makeAny(sal_True));
		uno::Reference< XChangesBatch >(aPropertySet, uno::UNO_QUERY_THROW)->commitChanges();
	} catch (...) {
		// fail silently
	}
}

sal_Bool MigrationImpl::checkMigrationCompleted()
{
    sal_Bool bMigrationCompleted = sal_False;
    try {
		uno::Reference< XPropertySet > aPropertySet(
            getConfigAccess("org.openoffice.Setup/Office"), uno::UNO_QUERY_THROW);
        aPropertySet->getPropertyValue(
            OUString::createFromAscii("MigrationCompleted")) >>= bMigrationCompleted;
    } catch (Exception&) {
        // just return false...
    }
    return bMigrationCompleted;
}

static void insertSorted(migrations_available& rAvailableMigrations, supported_migration& aSupportedMigration)
{
    bool                           bInserted( false );
    migrations_available::iterator pIter = rAvailableMigrations.begin();
    while ( !bInserted && pIter != rAvailableMigrations.end())
    {
        if ( pIter->nPriority < aSupportedMigration.nPriority )
        {
            rAvailableMigrations.insert(pIter, aSupportedMigration );
            bInserted = true;
            break; // i111193: insert invalidates iterator!
        }
        ++pIter;
    }
    if ( !bInserted )
        rAvailableMigrations.push_back( aSupportedMigration );
}

bool MigrationImpl::readAvailableMigrations(migrations_available& rAvailableMigrations)
{
    // get supported version names
	uno::Reference< XNameAccess > aMigrationAccess(getConfigAccess("org.openoffice.Setup/Migration/SupportedVersions"), uno::UNO_QUERY_THROW);
    uno::Sequence< OUString > seqSupportedVersions = aMigrationAccess->getElementNames();

    const OUString aVersionIdentifiers( RTL_CONSTASCII_USTRINGPARAM( "VersionIdentifiers" ));
    const OUString aPriorityIdentifier( RTL_CONSTASCII_USTRINGPARAM( "Priority" ));

    for (sal_Int32 i=0; i<seqSupportedVersions.getLength(); i++)
    {
        sal_Int32                 nPriority( 0 );
        uno::Sequence< OUString > seqVersions;
        uno::Reference< XNameAccess > xMigrationData( aMigrationAccess->getByName(seqSupportedVersions[i]), uno::UNO_QUERY_THROW );
        xMigrationData->getByName( aVersionIdentifiers ) >>= seqVersions;
        xMigrationData->getByName( aPriorityIdentifier ) >>= nPriority;

        supported_migration aSupportedMigration;
        aSupportedMigration.name      = seqSupportedVersions[i];
        aSupportedMigration.nPriority = nPriority;
        for (sal_Int32 j=0; j<seqVersions.getLength(); j++)
            aSupportedMigration.supported_versions.push_back(seqVersions[j].trim());
        insertSorted( rAvailableMigrations, aSupportedMigration );
    }

    return true;
}

migrations_vr MigrationImpl::readMigrationSteps(const ::rtl::OUString& rMigrationName)
{
    // get migration access
	uno::Reference< XNameAccess > aMigrationAccess(getConfigAccess("org.openoffice.Setup/Migration/SupportedVersions"), uno::UNO_QUERY_THROW);
    uno::Reference< XNameAccess > xMigrationData( aMigrationAccess->getByName(rMigrationName), uno::UNO_QUERY_THROW );

    // get migration description from from org.openoffice.Setup/Migration
    // and build vector of migration steps
	OUString aMigrationSteps( RTL_CONSTASCII_USTRINGPARAM( "MigrationSteps" ));
    uno::Reference< XNameAccess > theNameAccess(xMigrationData->getByName(aMigrationSteps), uno::UNO_QUERY_THROW);
	uno::Sequence< OUString > seqMigrations = theNameAccess->getElementNames();
	uno::Reference< XNameAccess > tmpAccess;
	uno::Reference< XNameAccess > tmpAccess2;
	uno::Sequence< OUString > tmpSeq;
    migrations_vr vrMigrations(new migrations_v);
    for (sal_Int32 i = 0; i < seqMigrations.getLength(); i++)
    {
        // get current migration step
        theNameAccess->getByName(seqMigrations[i]) >>= tmpAccess;
        // tmpStepPtr = new migration_step();
        migration_step tmpStep;
        tmpStep.name = seqMigrations[i];

        // read included files from current step description
        ::rtl::OUString aSeqEntry;
        if (tmpAccess->getByName(OUString::createFromAscii("IncludedFiles")) >>= tmpSeq)
        {
            for (sal_Int32 j=0; j<tmpSeq.getLength(); j++)
            {
                aSeqEntry = tmpSeq[j];
                tmpStep.includeFiles.push_back(aSeqEntry);
            }
        }

        // excluded files...
        if (tmpAccess->getByName(OUString::createFromAscii("ExcludedFiles")) >>= tmpSeq)
        {
            for (sal_Int32 j=0; j<tmpSeq.getLength(); j++)
                tmpStep.excludeFiles.push_back(tmpSeq[j]);
        }

        // included nodes...
        if (tmpAccess->getByName(OUString::createFromAscii("IncludedNodes")) >>= tmpSeq)
        {
            for (sal_Int32 j=0; j<tmpSeq.getLength(); j++)
                tmpStep.includeConfig.push_back(tmpSeq[j]);
        }

        // excluded nodes...
        if (tmpAccess->getByName(OUString::createFromAscii("ExcludedNodes")) >>= tmpSeq)
        {
            for (sal_Int32 j=0; j<tmpSeq.getLength(); j++)
                tmpStep.excludeConfig.push_back(tmpSeq[j]);
        }

        // included extensions...
        if (tmpAccess->getByName(OUString::createFromAscii("IncludedExtensions")) >>= tmpSeq)
        {
            for (sal_Int32 j=0; j<tmpSeq.getLength(); j++)
                tmpStep.includeExtensions.push_back(tmpSeq[j]);
        }

        // excluded extensions...
        if (tmpAccess->getByName(OUString::createFromAscii("ExcludedExtensions")) >>= tmpSeq)
        {
            for (sal_Int32 j=0; j<tmpSeq.getLength(); j++)
            {
                aSeqEntry = tmpSeq[j];
                tmpStep.excludeExtensions.push_back(aSeqEntry);
            }
        }

        // generic service
        tmpAccess->getByName(OUString::createFromAscii("MigrationService")) >>= tmpStep.service;

        vrMigrations->push_back(tmpStep);
    }
    return vrMigrations;
}

static FileBase::RC _checkAndCreateDirectory(INetURLObject& dirURL)
{
    FileBase::RC result = Directory::create(dirURL.GetMainURL(INetURLObject::DECODE_TO_IURI));
    if (result == FileBase::E_NOENT)
    {
        INetURLObject baseURL(dirURL);
        baseURL.removeSegment();
        _checkAndCreateDirectory(baseURL);
        return Directory::create(dirURL.GetMainURL(INetURLObject::DECODE_TO_IURI));
    } else
        return result;
}

install_info MigrationImpl::findInstallation(const strings_v& rVersions)
{
    rtl::OUString aProductName;
	uno::Any aRet = ::utl::ConfigManager::GetDirectConfigProperty( ::utl::ConfigManager::PRODUCTNAME );
    aRet >>= aProductName;
	aProductName = aProductName.toAsciiLowerCase();

    install_info aInfo;
    strings_v::const_iterator i_ver = rVersions.begin();
	uno::Reference < util::XStringSubstitution > xSubst( ::comphelper::getProcessServiceFactory()->createInstance(::rtl::OUString::createFromAscii("com.sun.star.util.PathSubstitution")), uno::UNO_QUERY );
    while (i_ver != rVersions.end())
	{
		::rtl::OUString aVersion, aProfileName;
		sal_Int32 nSeparatorIndex = (*i_ver).indexOf('=');
		if ( nSeparatorIndex != -1 )
		{
			aVersion = (*i_ver).copy( 0, nSeparatorIndex );
			aProfileName = (*i_ver).copy( nSeparatorIndex+1 );
		}

		if ( aVersion.getLength() && aProfileName.getLength() &&
				( !aInfo.userdata.getLength() || !aProfileName.toAsciiLowerCase().compareTo( aProductName, aProductName.getLength() ) )
		   )
		{
			::rtl::OUString aUserInst;
			osl::Security().getConfigDir( aUserInst );
			if ( aUserInst.getLength() && aUserInst[ aUserInst.getLength()-1 ] != '/' )
				aUserInst += ::rtl::OUString::createFromAscii("/");
#if defined UNX && ! defined MACOSX
            // tribute to whoever had the "great" idea to use different names on Windows and Unix
            aUserInst += ::rtl::OUString::createFromAscii(".");
#endif
			aUserInst += aProfileName;
			try
			{
				INetURLObject aObj(aUserInst);
				::ucbhelper::Content aCnt( aObj.GetMainURL( INetURLObject::NO_DECODE ), uno::Reference< ucb::XCommandEnvironment > () );
				aCnt.isDocument();
				aInfo.userdata = aObj.GetMainURL( INetURLObject::NO_DECODE );
				aInfo.productname = aVersion;
			}
			catch( uno::Exception& ){}
		}
		++i_ver;
	}

	return aInfo;
}

sal_Int32 MigrationImpl::findPreferedMigrationProcess(const migrations_available& rAvailableMigrations)
{
    sal_Int32    nIndex( -1 );
    sal_Int32    i( 0 );

    migrations_available::const_iterator rIter = rAvailableMigrations.begin();
    while ( rIter != rAvailableMigrations.end() )
    {
        install_info aInstallInfo = findInstallation(rIter->supported_versions);
        if (aInstallInfo.productname.getLength() > 0 )
        {
            m_aInfo = aInstallInfo;
            nIndex  = i;
            break;
        }
        ++i;
        ++rIter;
    }

    return nIndex;
}

strings_vr MigrationImpl::applyPatterns(const strings_v& vSet, const strings_v& vPatterns) const
{
    using namespace utl;
    strings_vr vrResult(new strings_v);
    strings_v::const_iterator i_set;
    strings_v::const_iterator i_pat = vPatterns.begin();
    while (i_pat != vPatterns.end())
    {
        // find matches for this pattern in input set
        // and copy them to the result
        SearchParam param(*i_pat, SearchParam::SRCH_REGEXP);
        TextSearch ts(param, LANGUAGE_DONTKNOW);
        i_set = vSet.begin();
        xub_StrLen start = 0;
        xub_StrLen end = 0;
        while (i_set != vSet.end())
        {
            end = (xub_StrLen)(i_set->getLength());
            if (ts.SearchFrwrd(*i_set, &start, &end))
                vrResult->push_back(*i_set);
            i_set++;
        }
        i_pat++;
    }
    return vrResult;
}

strings_vr MigrationImpl::getAllFiles(const OUString& baseURL) const
{
    using namespace osl;
    strings_vr vrResult(new strings_v);

    // get sub dirs
    Directory dir(baseURL);
    if (dir.open() == FileBase::E_None)
    {
        strings_v vSubDirs;
        strings_vr vrSubResult;

        // work through directory contents...
        DirectoryItem item;
        FileStatus fs(FileStatusMask_Type | FileStatusMask_FileURL);
        while (dir.getNextItem(item) == FileBase::E_None)
        {
            if (item.getFileStatus(fs) == FileBase::E_None)
            {
                if (fs.getFileType() == FileStatus::Directory)
                    vSubDirs.push_back(fs.getFileURL());
                else
                    vrResult->push_back(fs.getFileURL());
            }
        }

        // recurse subfolders
        strings_v::const_iterator i = vSubDirs.begin();
        while (i != vSubDirs.end())
        {
            vrSubResult = getAllFiles(*i);
            vrResult->insert(vrResult->end(), vrSubResult->begin(), vrSubResult->end());
            i++;
        }
    }
    return vrResult;
}

strings_vr MigrationImpl::compileFileList()
{

    strings_vr vrResult(new strings_v);
    strings_vr vrInclude;
    strings_vr vrExclude;
    strings_vr vrTemp;

#ifdef SAL_OS2
    if (m_aInfo.userdata.getLength() == 0)
        return vrResult;
#endif

    // get a list of all files:
    strings_vr vrFiles = getAllFiles(m_aInfo.userdata);

    // get a file list result for each migration step
    migrations_v::const_iterator i_migr = m_vrMigrations->begin();
    while (i_migr != m_vrMigrations->end())
    {
        vrInclude = applyPatterns(*vrFiles, i_migr->includeFiles);
        vrExclude = applyPatterns(*vrFiles, i_migr->excludeFiles);
        substract(*vrInclude, *vrExclude);
        vrResult->insert(vrResult->end(), vrInclude->begin(), vrInclude->end());
        i_migr++;
    }
    return vrResult;
}

namespace {

struct componentParts {
    std::set< rtl::OUString > includedPaths;
    std::set< rtl::OUString > excludedPaths;
};

typedef std::map< rtl::OUString, componentParts > Components;

bool getComponent(rtl::OUString const & path, rtl::OUString * component) {
    OSL_ASSERT(component != 0);
    if (path.getLength() == 0 || path[0] != '/') {
        OSL_TRACE(
            ("configuration migration in/exclude path %s ignored (does not"
             " start with slash)"),
            rtl::OUStringToOString(path, RTL_TEXTENCODING_UTF8).getStr());
        return false;
    }
    sal_Int32 i = path.indexOf('/', 1);
    *component = i < 0 ? path.copy(1) : path.copy(1, i - 1);
    return true;
}

uno::Sequence< rtl::OUString > setToSeq(std::set< rtl::OUString > const & set) {
    std::set< rtl::OUString >::size_type n = set.size();
    if (n > SAL_MAX_INT32) {
        throw std::bad_alloc();
    }
    uno::Sequence< rtl::OUString > seq(static_cast< sal_Int32 >(n));
    sal_Int32 i = 0;
    for (std::set< rtl::OUString >::const_iterator j(set.begin());
         j != set.end(); ++j)
    {
        seq[i++] = *j;
    }
    return seq;
}

}

void MigrationImpl::copyConfig() {
    Components comps;
    for (migrations_v::const_iterator i(m_vrMigrations->begin());
         i != m_vrMigrations->end(); ++i)
    {
        for (strings_v::const_iterator j(i->includeConfig.begin());
             j != i->includeConfig.end(); ++j)
        {
            rtl::OUString comp;
            if (getComponent(*j, &comp)) {
                comps[comp].includedPaths.insert(*j);
            }
        }
        for (strings_v::const_iterator j(i->excludeConfig.begin());
             j != i->excludeConfig.end(); ++j)
        {
            rtl::OUString comp;
            if (getComponent(*j, &comp)) {
                comps[comp].excludedPaths.insert(*j);
            }
        }
    }
    for (Components::const_iterator i(comps.begin()); i != comps.end(); ++i) {
        if (!i->second.includedPaths.empty()) {
            rtl::OUStringBuffer buf(m_aInfo.userdata);
            if ( m_aInfo.productname.equals( OUString::createFromAscii("OpenOffice.org 3") ) )
            {
                // OpenOffice.org 3 configuration file
                buf.appendAscii(RTL_CONSTASCII_STRINGPARAM("/user/registrymodifications.xcu"));
            }
            else
            {
                // OpenOffice.org 2 configuration files
                buf.appendAscii(RTL_CONSTASCII_STRINGPARAM("/user/registry/data"));
                sal_Int32 n = 0;
                do {
                    rtl::OUString seg(i->first.getToken(0, '.', n));
                    rtl::OUString enc(
                        rtl::Uri::encode(
                            seg, rtl_UriCharClassPchar, rtl_UriEncodeStrict,
                            RTL_TEXTENCODING_UTF8));
                    if (enc.getLength() == 0 && seg.getLength() != 0) {
                        OSL_TRACE(
                            ("configuration migration component %s ignored (cannot"
                             " be encoded as file path)"),
                            rtl::OUStringToOString(
                                i->first, RTL_TEXTENCODING_UTF8).getStr());
                        goto next;
                    }
                    buf.append(sal_Unicode('/'));
                    buf.append(enc);
                } while (n >= 0);
                buf.appendAscii(RTL_CONSTASCII_STRINGPARAM(".xcu"));
            }
            configuration::Update::get(
                comphelper::getProcessComponentContext())->
                insertModificationXcuFile(
                    buf.makeStringAndClear(), setToSeq(i->second.includedPaths),
                    setToSeq(i->second.excludedPaths));
        } else {
            OSL_TRACE(
                ("configuration migration component %s ignored (only excludes,"
                 " no includes)"),
                rtl::OUStringToOString(
                    i->first, RTL_TEXTENCODING_UTF8).getStr());
        }
    next:;
    }
}

// removes elements of vector 2 in vector 1
void MigrationImpl::substract(strings_v& va, const strings_v& vb_c) const
{
    strings_v vb(vb_c);
    // ensure uniqueness of entries
    sort(va.begin(), va.end());
    sort(vb.begin(), vb.end());
    unique(va.begin(), va.end());
    unique(vb.begin(), vb.end());

    strings_v::const_iterator i_ex = vb.begin();
    strings_v::iterator i_in;
    strings_v::iterator i_next;
    while (i_ex != vb.end())
    {
        i_in = va.begin();
        while (i_in != va.end())
        {
            if ( *i_in == *i_ex)
            {
                i_next = i_in+1;
                va.erase(i_in);
                i_in = i_next;
                // we can only find one match since we
                // ensured uniqueness of the entries. ergo:
                break;
            }
            else
                i_in++;
        }
        i_ex++;
    }
}

uno::Reference< XNameAccess > MigrationImpl::getConfigAccess(const sal_Char* pPath, sal_Bool bUpdate)
{
	uno::Reference< XNameAccess > xNameAccess;
    try{
        OUString sConfigSrvc = OUString::createFromAscii("com.sun.star.configuration.ConfigurationProvider");
        OUString sAccessSrvc;
        if (bUpdate)
            sAccessSrvc = OUString::createFromAscii("com.sun.star.configuration.ConfigurationUpdateAccess");
        else
            sAccessSrvc = OUString::createFromAscii("com.sun.star.configuration.ConfigurationAccess");

        OUString sConfigURL = OUString::createFromAscii(pPath);

        // get configuration provider
		uno::Reference< XMultiServiceFactory > theMSF = comphelper::getProcessServiceFactory();
		uno::Reference< XMultiServiceFactory > theConfigProvider = uno::Reference< XMultiServiceFactory > (
                theMSF->createInstance( sConfigSrvc ),uno::UNO_QUERY_THROW );

        // access the provider
        uno::Sequence< uno::Any > theArgs(1);
        theArgs[ 0 ] <<= sConfigURL;
		xNameAccess = uno::Reference< XNameAccess > (
                theConfigProvider->createInstanceWithArguments(
                sAccessSrvc, theArgs ), uno::UNO_QUERY_THROW );
    } catch (com::sun::star::uno::Exception& e)
    {
        OString aMsg = OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US);
        OSL_ENSURE(sal_False, aMsg.getStr());
    }
    return xNameAccess;
}

void MigrationImpl::copyFiles()
{
    strings_v::const_iterator i_file = m_vrFileList->begin();
    OUString localName;
    OUString destName;
    OUString userInstall;
    utl::Bootstrap::PathStatus aStatus;
    aStatus = utl::Bootstrap::locateUserInstallation(userInstall);
    if (aStatus == utl::Bootstrap::PATH_EXISTS)
    {
        while (i_file != m_vrFileList->end())
        {

            // remove installation prefix from file
            localName = i_file->copy(m_aInfo.userdata.getLength());
            destName = userInstall + localName;
            INetURLObject aURL(destName);
            // check whether destination directory exists
            aURL.removeSegment();
            _checkAndCreateDirectory(aURL);
            FileBase::RC copyResult = File::copy(*i_file, destName);
            if (copyResult != FileBase::E_None)
            {
                OString msg("Cannot copy ");
                msg += OUStringToOString(*i_file, RTL_TEXTENCODING_UTF8) + " to "
                    +  OUStringToOString(destName, RTL_TEXTENCODING_UTF8);
                OSL_ENSURE(sal_False, msg.getStr());
            }
            i_file++;
        }
    }
    else
    {
        OSL_ENSURE(sal_False, "copyFiles: UserInstall does not exist");
    }
}

void MigrationImpl::runServices()
{
    // Build argument array
    uno::Sequence< uno::Any > seqArguments(3);
    seqArguments[0] = uno::makeAny(NamedValue(
        OUString::createFromAscii("Productname"),
        uno::makeAny(m_aInfo.productname)));
    seqArguments[1] = uno::makeAny(NamedValue(
        OUString::createFromAscii("UserData"),
        uno::makeAny(m_aInfo.userdata)));


    // create an instance of every migration service
    // and execute the migration job
	uno::Reference< XJob > xMigrationJob;

    migrations_v::const_iterator i_mig = m_vrMigrations->begin();
    while (i_mig != m_vrMigrations->end())
    {
        if( i_mig->service.getLength() > 0)
        {

            try
            {
                // set black list for extension migration
                uno::Sequence< rtl::OUString > seqExtBlackList;
                sal_uInt32 nSize = i_mig->excludeExtensions.size();
                if ( nSize > 0 )
                    seqExtBlackList = comphelper::arrayToSequence< ::rtl::OUString >(
                        &i_mig->excludeExtensions[0], nSize );
                seqArguments[2] = uno::makeAny(NamedValue(
                    OUString::createFromAscii("ExtensionBlackList"),
                    uno::makeAny( seqExtBlackList )));

				xMigrationJob = uno::Reference< XJob >(m_xFactory->createInstanceWithArguments(
                    i_mig->service, seqArguments), uno::UNO_QUERY_THROW);

                xMigrationJob->execute(uno::Sequence< NamedValue >());


            } catch (Exception& e)
            {
                OString aMsg("Execution of migration service failed (Exception caught).\nService: ");
                aMsg += OUStringToOString(i_mig->service, RTL_TEXTENCODING_ASCII_US) + "\nMessage: ";
                aMsg += OUStringToOString(e.Message, RTL_TEXTENCODING_ASCII_US);
                OSL_ENSURE(sal_False, aMsg.getStr());
            } catch (...)
            {
                OString aMsg("Execution of migration service failed (Exception caught).\nService: ");
                aMsg += OUStringToOString(i_mig->service, RTL_TEXTENCODING_ASCII_US) +
					"\nNo message available";
                OSL_ENSURE(sal_False, aMsg.getStr());
            }

        }
        i_mig++;
    }
}

::std::vector< MigrationModuleInfo > MigrationImpl::dectectUIChangesForAllModules() const
{
	::std::vector< MigrationModuleInfo > vModulesInfo;
	const ::rtl::OUString MENUBAR = ::rtl::OUString::createFromAscii("menubar");
	const ::rtl::OUString TOOLBAR = ::rtl::OUString::createFromAscii("toolbar");

	uno::Sequence< uno::Any > lArgs(2);
	lArgs[0] <<= m_aInfo.userdata + ::rtl::OUString::createFromAscii("/user/config/soffice.cfg/modules");
	lArgs[1] <<= embed::ElementModes::READ;

	uno::Reference< lang::XSingleServiceFactory > xStorageFactory(m_xFactory->createInstance(::rtl::OUString::createFromAscii("com.sun.star.embed.FileSystemStorageFactory")), uno::UNO_QUERY);
	uno::Reference< embed::XStorage >             xModules;

	xModules = uno::Reference< embed::XStorage >(xStorageFactory->createInstanceWithArguments(lArgs), uno::UNO_QUERY);
	if (!xModules.is())
		return vModulesInfo;

	uno::Reference< container::XNameAccess > xAccess = uno::Reference< container::XNameAccess >(xModules, uno::UNO_QUERY);
	uno::Sequence< ::rtl::OUString > lNames = xAccess->getElementNames();
	sal_Int32 nLength = lNames.getLength();
	for (sal_Int32 i=0; i<nLength; ++i)
	{
        ::rtl::OUString sModuleShortName = lNames[i];
        uno::Reference< embed::XStorage > xModule = xModules->openStorageElement(sModuleShortName, embed::ElementModes::READ);
		if (xModule.is())
		{
			MigrationModuleInfo aModuleInfo;

			uno::Reference< embed::XStorage > xMenubar = xModule->openStorageElement(MENUBAR, embed::ElementModes::READ);
			if (xMenubar.is())
			{
				uno::Reference< container::XNameAccess > xNameAccess = uno::Reference< container::XNameAccess >(xMenubar, uno::UNO_QUERY);
				if (xNameAccess->getElementNames().getLength() > 0)
				{
					aModuleInfo.sModuleShortName = sModuleShortName;
					aModuleInfo.bHasMenubar = sal_True;
				}
			}

			uno::Reference< embed::XStorage > xToolbar = xModule->openStorageElement(TOOLBAR, embed::ElementModes::READ);
			if (xToolbar.is())
			{
				const ::rtl::OUString RESOURCEURL_CUSTOM_ELEMENT = ::rtl::OUString::createFromAscii("custom_");
				sal_Int32 nCustomLen = 7;

				uno::Reference< container::XNameAccess > xNameAccess = uno::Reference< container::XNameAccess >(xToolbar, uno::UNO_QUERY);
				::uno::Sequence< ::rtl::OUString > lToolbars = xNameAccess->getElementNames();
				for (sal_Int32 j=0; j<lToolbars.getLength(); ++j)
				{
                    ::rtl::OUString sToolbarName = lToolbars[j];
                    if (sToolbarName.getLength()>=nCustomLen &&
						sToolbarName.copy(0, nCustomLen).equals(RESOURCEURL_CUSTOM_ELEMENT))
						continue;

					aModuleInfo.sModuleShortName = sModuleShortName;
					sal_Int32 nIndex = sToolbarName.lastIndexOf('.');
					if (nIndex > 0)
					{
						::rtl::OUString sExtension(sToolbarName.copy(nIndex));
						::rtl::OUString sToolbarResourceName(sToolbarName.copy(0, nIndex));
						if (sToolbarResourceName.getLength()>0 && sExtension.equalsAsciiL(".xml", 4))
							aModuleInfo.m_vToolbars.push_back(sToolbarResourceName);
					}
				}
			}

			if (aModuleInfo.sModuleShortName.getLength()>0)
				vModulesInfo.push_back(aModuleInfo);
		}
	}

	return vModulesInfo;
}

void MigrationImpl::compareOldAndNewConfig(const ::rtl::OUString& sParent,
										   const uno::Reference< container::XIndexContainer >& xIndexOld,
										   const uno::Reference< container::XIndexContainer >& xIndexNew,
										   const ::rtl::OUString& sResourceURL)
{
	::std::vector< MigrationItem > vOldItems;
	::std::vector< MigrationItem > vNewItems;
	uno::Sequence< beans::PropertyValue > aProp;
	sal_Int32 nOldCount = xIndexOld->getCount();
	sal_Int32 nNewCount = xIndexNew->getCount();

	for (int n=0; n<nOldCount; ++n)
	{
		MigrationItem aMigrationItem;
		if (xIndexOld->getByIndex(n) >>= aProp)
		{
			for(int i=0; i<aProp.getLength(); ++i)
			{
				if (aProp[i].Name.equals(ITEM_DESCRIPTOR_COMMANDURL))
					aProp[i].Value >>= aMigrationItem.m_sCommandURL;
				else if (aProp[i].Name.equals(ITEM_DESCRIPTOR_CONTAINER))
					aProp[i].Value >>= aMigrationItem.m_xPopupMenu;
			}

			if (aMigrationItem.m_sCommandURL.getLength())
				vOldItems.push_back(aMigrationItem);
		}
	}

	for (int n=0; n<nNewCount; ++n)
	{
		MigrationItem aMigrationItem;
		if (xIndexNew->getByIndex(n) >>= aProp)
		{
			for(int i=0; i<aProp.getLength(); ++i)
			{
				if (aProp[i].Name.equals(ITEM_DESCRIPTOR_COMMANDURL))
					aProp[i].Value >>= aMigrationItem.m_sCommandURL;
				else if (aProp[i].Name.equals(ITEM_DESCRIPTOR_CONTAINER))
					aProp[i].Value >>= aMigrationItem.m_xPopupMenu;
			}

			if (aMigrationItem.m_sCommandURL.getLength())
				vNewItems.push_back(aMigrationItem);
		}
	}

	::std::vector< MigrationItem >::iterator it;

	::rtl::OUString sSibling;
	for (it = vOldItems.begin(); it!=vOldItems.end(); ++it)
	{
		::std::vector< MigrationItem >::iterator pFound = ::std::find(vNewItems.begin(), vNewItems.end(), *it);
		if (pFound != vNewItems.end() && it->m_xPopupMenu.is())
		{
			::rtl::OUString sName;
			if (sParent.getLength()>0)
				sName = sParent + MENU_SEPERATOR + it->m_sCommandURL;
			else
				sName = it->m_sCommandURL;
			compareOldAndNewConfig(sName, it->m_xPopupMenu, pFound->m_xPopupMenu, sResourceURL);
		}
		else if (pFound == vNewItems.end())
		{
			MigrationItem aMigrationItem(sParent, sSibling, it->m_sCommandURL, it->m_xPopupMenu);
			if (m_aOldVersionItemsHashMap.find(sResourceURL)==m_aOldVersionItemsHashMap.end())
			{
				::std::vector< MigrationItem > vMigrationItems;
				m_aOldVersionItemsHashMap.insert(MigrationHashMap::value_type(sResourceURL, vMigrationItems));
				m_aOldVersionItemsHashMap[sResourceURL].push_back(aMigrationItem);
			}
			else
			{
				if (::std::find(m_aOldVersionItemsHashMap[sResourceURL].begin(), m_aOldVersionItemsHashMap[sResourceURL].end(), aMigrationItem)==m_aOldVersionItemsHashMap[sResourceURL].end())
					m_aOldVersionItemsHashMap[sResourceURL].push_back(aMigrationItem);
			}
		}

		sSibling = it->m_sCommandURL;
	}

	::rtl::OUString sNewSibling;
	uno::Reference< container::XIndexContainer > xPopup;
	for (it = vNewItems.begin(); it!=vNewItems.end(); ++it)
	{
		::std::vector< MigrationItem >::iterator pFound = ::std::find(vOldItems.begin(), vOldItems.end(), *it);
		if (pFound != vOldItems.end() && it->m_xPopupMenu.is())
		{
			::rtl::OUString sName;
			if (sParent.getLength()>0)
				sName = sParent + MENU_SEPERATOR + it->m_sCommandURL;
			else
				sName = it->m_sCommandURL;
			compareOldAndNewConfig(sName, pFound->m_xPopupMenu, it->m_xPopupMenu, sResourceURL);
		}
		else if (::std::find(vOldItems.begin(), vOldItems.end(), *it) == vOldItems.end())
		{
			MigrationItem aMigrationItem(sParent, sSibling, it->m_sCommandURL, it->m_xPopupMenu);
			if (m_aNewVersionItemsHashMap.find(sResourceURL)==m_aNewVersionItemsHashMap.end())
			{
				::std::vector< MigrationItem > vMigrationItems;
				m_aNewVersionItemsHashMap.insert(MigrationHashMap::value_type(sResourceURL, vMigrationItems));
				m_aNewVersionItemsHashMap[sResourceURL].push_back(aMigrationItem);
			}
			else
			{
				if (::std::find(m_aNewVersionItemsHashMap[sResourceURL].begin(), m_aNewVersionItemsHashMap[sResourceURL].end(), aMigrationItem)==m_aNewVersionItemsHashMap[sResourceURL].end())
					m_aNewVersionItemsHashMap[sResourceURL].push_back(aMigrationItem);
			}
		}
	}
}

void MigrationImpl::mergeOldToNewVersion(const uno::Reference< ui::XUIConfigurationManager >& xCfgManager,
										 const uno::Reference< container::XIndexContainer>& xIndexContainer,
										 const ::rtl::OUString& sModuleIdentifier,
										 const ::rtl::OUString& sResourceURL)
{
	MigrationHashMap::iterator pFound = m_aOldVersionItemsHashMap.find(sResourceURL);
	if (pFound==m_aOldVersionItemsHashMap.end())
		return;

	::std::vector< MigrationItem >::iterator it;
	for (it=pFound->second.begin(); it!=pFound->second.end(); ++it)
	{
		uno::Reference< container::XIndexContainer > xTemp = xIndexContainer;

		::rtl::OUString sParentNodeName = it->m_sParentNodeName;
		sal_Int32 nIndex = 0;
		do
		{
			::rtl::OUString sToken = sParentNodeName.getToken(0, '|', nIndex).trim();
			if (sToken.getLength()<=0)
				break;

			sal_Int32 nCount = xTemp->getCount();
			for (sal_Int32 i=0; i<nCount; ++i)
			{
				::rtl::OUString sCommandURL;
				::rtl::OUString sLabel;
				uno::Reference< container::XIndexContainer > xChild;

				uno::Sequence< beans::PropertyValue > aPropSeq;
				xTemp->getByIndex(i) >>= aPropSeq;
				for (sal_Int32 j=0; j<aPropSeq.getLength(); ++j)
				{
					::rtl::OUString sPropName = aPropSeq[j].Name;
					if (sPropName.equals(ITEM_DESCRIPTOR_COMMANDURL))
						aPropSeq[j].Value >>= sCommandURL;
					else if (sPropName.equals(ITEM_DESCRIPTOR_LABEL))
						aPropSeq[j].Value >>= sLabel;
					else if (sPropName.equals(ITEM_DESCRIPTOR_CONTAINER))
						aPropSeq[j].Value >>= xChild;
				}

				if (sCommandURL == sToken)
				{
					xTemp = xChild;
					break;
				}
			}

		} while (nIndex>=0);

		if (nIndex == -1)
		{
			uno::Sequence< beans::PropertyValue > aPropSeq(3);

			aPropSeq[0].Name = ITEM_DESCRIPTOR_COMMANDURL;
			aPropSeq[0].Value <<= it->m_sCommandURL;
			aPropSeq[1].Name = ITEM_DESCRIPTOR_LABEL;
			aPropSeq[1].Value <<= retrieveLabelFromCommand(it->m_sCommandURL, sModuleIdentifier);
			aPropSeq[2].Name = ITEM_DESCRIPTOR_CONTAINER;
			aPropSeq[2].Value <<= it->m_xPopupMenu;

			if (it->m_sPrevSibling.getLength() == 0)
				xTemp->insertByIndex(0, uno::makeAny(aPropSeq));
			else if (it->m_sPrevSibling.getLength() > 0)
			{
				sal_Int32 nCount = xTemp->getCount();
				sal_Int32 i = 0;
				for (; i<nCount; ++i)
				{
					::rtl::OUString sCmd;
					uno::Sequence< beans::PropertyValue > aTempPropSeq;
					xTemp->getByIndex(i) >>= aTempPropSeq;
					for (sal_Int32 j=0; j<aTempPropSeq.getLength(); ++j)
					{
						if (aTempPropSeq[j].Name.equals(ITEM_DESCRIPTOR_COMMANDURL))
						{
							aTempPropSeq[j].Value >>= sCmd;
							break;
						}
					}

					if (sCmd.equals(it->m_sPrevSibling))
						break;
				}

				xTemp->insertByIndex(i+1, uno::makeAny(aPropSeq));
			}
		}
	}

	uno::Reference< container::XIndexAccess > xIndexAccess(xIndexContainer, uno::UNO_QUERY);
	if (xIndexAccess.is())
		xCfgManager->replaceSettings(sResourceURL, xIndexAccess);

	uno::Reference< ui::XUIConfigurationPersistence > xUIConfigurationPersistence(xCfgManager, uno::UNO_QUERY);
	if (xUIConfigurationPersistence.is())
		xUIConfigurationPersistence->store();
}

uno::Reference< ui::XUIConfigurationManager > NewVersionUIInfo::getConfigManager(const ::rtl::OUString& sModuleShortName) const
{
	uno::Reference< ui::XUIConfigurationManager > xCfgManager;

	for (sal_Int32 i=0; i<m_lCfgManagerSeq.getLength(); ++i)
	{
		if (m_lCfgManagerSeq[i].Name.equals(sModuleShortName))
		{
			m_lCfgManagerSeq[i].Value >>= xCfgManager;
			break;
		}
	}

	return xCfgManager;
}

uno::Reference< container::XIndexContainer > NewVersionUIInfo::getNewMenubarSettings(const ::rtl::OUString& sModuleShortName) const
{
	uno::Reference< container::XIndexContainer > xNewMenuSettings;

	for (sal_Int32 i=0; i<m_lNewVersionMenubarSettingsSeq.getLength(); ++i)
	{
		if (m_lNewVersionMenubarSettingsSeq[i].Name.equals(sModuleShortName))
		{
			m_lNewVersionMenubarSettingsSeq[i].Value >>= xNewMenuSettings;
			break;
		}
	}

	return xNewMenuSettings;
}

uno::Reference< container::XIndexContainer > NewVersionUIInfo::getNewToolbarSettings(const ::rtl::OUString& sModuleShortName, const ::rtl::OUString& sToolbarName) const
{
	uno::Reference< container::XIndexContainer > xNewToolbarSettings;

	for (sal_Int32 i=0; i<m_lNewVersionToolbarSettingsSeq.getLength(); ++i)
	{
		if (m_lNewVersionToolbarSettingsSeq[i].Name.equals(sModuleShortName))
		{
			uno::Sequence< beans::PropertyValue > lToolbarSettingsSeq;
			m_lNewVersionToolbarSettingsSeq[i].Value >>= lToolbarSettingsSeq;
			for (sal_Int32 j=0; j<lToolbarSettingsSeq.getLength(); ++j)
			{
				if (lToolbarSettingsSeq[j].Name.equals(sToolbarName))
				{
					lToolbarSettingsSeq[j].Value >>= xNewToolbarSettings;
					break;
				}
			}

			break;
		}
	}

	return xNewToolbarSettings;
}

void NewVersionUIInfo::init(const ::std::vector< MigrationModuleInfo >& vModulesInfo)
{
	m_lCfgManagerSeq.realloc(vModulesInfo.size());
	m_lNewVersionMenubarSettingsSeq.realloc(vModulesInfo.size());
	m_lNewVersionToolbarSettingsSeq.realloc(vModulesInfo.size());

	const ::rtl::OUString sModuleCfgSupplier = ::rtl::OUString::createFromAscii("com.sun.star.ui.ModuleUIConfigurationManagerSupplier");
	const ::rtl::OUString sMenubarResourceURL = ::rtl::OUString::createFromAscii("private:resource/menubar/menubar");
	const ::rtl::OUString sToolbarResourcePre = ::rtl::OUString::createFromAscii("private:resource/toolbar/");

	uno::Reference< ui::XModuleUIConfigurationManagerSupplier > xModuleCfgSupplier = uno::Reference< ui::XModuleUIConfigurationManagerSupplier >(::comphelper::getProcessServiceFactory()->createInstance(sModuleCfgSupplier), uno::UNO_QUERY);

	for (sal_uInt32 i=0; i<vModulesInfo.size(); ++i)
	{
		::rtl::OUString sModuleIdentifier = mapModuleShortNameToIdentifier(vModulesInfo[i].sModuleShortName);
		if (sModuleIdentifier.getLength() > 0)
		{
			uno::Reference< ui::XUIConfigurationManager > xCfgManager = xModuleCfgSupplier->getUIConfigurationManager(sModuleIdentifier);
			m_lCfgManagerSeq[i].Name = vModulesInfo[i].sModuleShortName;
			m_lCfgManagerSeq[i].Value <<= xCfgManager;

			if (vModulesInfo[i].bHasMenubar)
			{
				m_lNewVersionMenubarSettingsSeq[i].Name = vModulesInfo[i].sModuleShortName;
				m_lNewVersionMenubarSettingsSeq[i].Value <<= xCfgManager->getSettings(sMenubarResourceURL, sal_True);
			}

			sal_Int32 nToolbars = vModulesInfo[i].m_vToolbars.size();
			if (nToolbars > 0)
			{
				uno::Sequence< beans::PropertyValue > lPropSeq(nToolbars);
				for (sal_Int32 j=0; j<nToolbars; ++j)
				{
					::rtl::OUString sToolbarName = vModulesInfo[i].m_vToolbars[j];
					::rtl::OUString sToolbarResourceURL = sToolbarResourcePre + sToolbarName;

					lPropSeq[j].Name = sToolbarName;
					lPropSeq[j].Value <<= xCfgManager->getSettings(sToolbarResourceURL, sal_True);
				}

				m_lNewVersionToolbarSettingsSeq[i].Name = vModulesInfo[i].sModuleShortName;
				m_lNewVersionToolbarSettingsSeq[i].Value <<= lPropSeq;
			}
		}
	}
}

} // namespace desktop

