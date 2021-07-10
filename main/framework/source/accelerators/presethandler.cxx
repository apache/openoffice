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
#include <accelerators/presethandler.hxx>

//_______________________________________________
// own includes
#include <classes/fwkresid.hxx>

#include "classes/resource.hrc"
#include <threadhelp/readguard.hxx>
#include <threadhelp/writeguard.hxx>
#include <services.h>

//_______________________________________________
// interface includes

#ifndef __COM_SUN_STAR_CONFIGURATION_CORRUPTEDUICONFIGURATIONEXCEPTION_HPP_
#include <com/sun/star/configuration/CorruptedUIConfigurationException.hpp>
#endif

#ifndef __COM_SUN_STAR_CONTAINER_NOSUCHELEMENTEXCEPTION_HPP_
#include <com/sun/star/container/NoSuchElementException.hpp>
#endif

#ifndef __COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_
#include <com/sun/star/container/XNameAccess.hpp>
#endif

#ifndef __COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif

#ifndef __COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif

#ifndef __COM_SUN_STAR_EMBED_XTRANSACTEDOBJECT_HPP_
#include <com/sun/star/embed/XTransactedObject.hpp>
#endif

#ifndef __COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif

//_______________________________________________
// other includes
#include <vcl/svapp.hxx>

#ifndef _RTL_USTRBUF_HXX
#include <rtl/ustrbuf.hxx>
#endif

//_______________________________________________
// const

#define SUBSTORAGE_GLOBAL       DECLARE_ASCII("global" )
#define SUBSTORAGE_MODULES      DECLARE_ASCII("modules")

#define BASEPATH_SHARE_LAYER    DECLARE_ASCII("UIConfig"  )
#define BASEPATH_USER_LAYER     DECLARE_ASCII("UserConfig")

#define RELPATH_SHARE_LAYER     DECLARE_ASCII("soffice.cfg")
#define RELPATH_USER_LAYER      DECLARE_ASCII("soffice.cfg")
// #define RELPATH_SHARE_LAYER     DECLARE_ASCII("soffice.cfg/uiconfig.zip")
// #define RELPATH_USER_LAYER      DECLARE_ASCII("soffice.cfg/uiconfig.zip")

#define FILE_EXTENSION          DECLARE_ASCII(".xml")

#define PATH_SEPERATOR          DECLARE_ASCII("/")

static const ::sal_Int32 ID_CORRUPT_UICONFIG_SHARE   = 1;
static const ::sal_Int32 ID_CORRUPT_UICONFIG_USER    = 2;
static const ::sal_Int32 ID_CORRUPT_UICONFIG_GENERAL = 3;

//_______________________________________________
// namespace

namespace framework
{

//-----------------------------------------------
::rtl::OUString PresetHandler::PRESET_DEFAULT()
{
    static ::rtl::OUString RSTYPE = DECLARE_ASCII("default");
    return RSTYPE;
}

//-----------------------------------------------
::rtl::OUString PresetHandler::TARGET_CURRENT()
{
    static ::rtl::OUString RSTYPE = DECLARE_ASCII("current");
    return RSTYPE;
}

//-----------------------------------------------
::rtl::OUString PresetHandler::RESOURCETYPE_MENUBAR()
{
    static ::rtl::OUString RSTYPE = DECLARE_ASCII("menubar");
    return RSTYPE;
}

//-----------------------------------------------
::rtl::OUString PresetHandler::RESOURCETYPE_TOOLBAR()
{
    static ::rtl::OUString RSTYPE = DECLARE_ASCII("toolbar");
    return RSTYPE;
}

//-----------------------------------------------
::rtl::OUString PresetHandler::RESOURCETYPE_ACCELERATOR()
{
    static ::rtl::OUString RSTYPE = DECLARE_ASCII("accelerator");
    return RSTYPE;
}

//-----------------------------------------------
::rtl::OUString PresetHandler::RESOURCETYPE_STATUSBAR()
{
    static ::rtl::OUString RSTYPE = DECLARE_ASCII("statusbar");
    return RSTYPE;
}

//-----------------------------------------------
PresetHandler::PresetHandler(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR)
    : ThreadHelpBase     (&Application::GetSolarMutex()        )
    , m_xSMGR            (xSMGR                                )
    , m_aSharedStorages  (                                     )
    , m_lDocumentStorages(xSMGR                                )
    , m_aLocale          (::comphelper::Locale::X_NOTRANSLATE())
{
}

//-----------------------------------------------
PresetHandler::PresetHandler(const PresetHandler& rCopy)
    : ThreadHelpBase     (&Application::GetSolarMutex()        )
{
    m_xSMGR                 = rCopy.m_xSMGR;
    m_eConfigType           = rCopy.m_eConfigType;
    m_sResourceType         = rCopy.m_sResourceType;
    m_sModule               = rCopy.m_sModule;
    m_aSharedStorages       = rCopy.m_aSharedStorages;
    m_xWorkingStorageShare  = rCopy.m_xWorkingStorageShare;
    m_xWorkingStorageNoLang = rCopy.m_xWorkingStorageNoLang;
    m_xWorkingStorageUser   = rCopy.m_xWorkingStorageUser;
    m_lPresets              = rCopy.m_lPresets;
    m_lTargets              = rCopy.m_lTargets;
    m_aLocale               = rCopy.m_aLocale;
    m_lDocumentStorages     = rCopy.m_lDocumentStorages;
    m_sRelPathShare         = rCopy.m_sRelPathShare;
    m_sRelPathNoLang        = rCopy.m_sRelPathNoLang;
    m_sRelPathUser          = rCopy.m_sRelPathUser;
}

//-----------------------------------------------
PresetHandler::~PresetHandler()
{
    m_xWorkingStorageShare.clear();
    m_xWorkingStorageNoLang.clear();
    m_xWorkingStorageUser.clear();

    /* #i46497#
        Don't call forgetCachedStorages() here for shared storages.
        Because we opened different sub storages by using openPath().
        And every already open path was reused and referenced (means it's
        ref count was increased!)
        So now we have to release our ref counts to these shared storages
        only... and not to free all used storages.
        Otherwise we will disconnect all other open configuration access
        objects which base on these storages.
     */
    m_aSharedStorages->m_lStoragesShare.closePath(m_sRelPathShare);
    m_aSharedStorages->m_lStoragesUser.closePath (m_sRelPathUser );

    /* On the other side closePath() is not needed for our special handled
       document storage. Because it's not shared with others... so we can
       free it.
     */
    m_lDocumentStorages.forgetCachedStorages();
}

//-----------------------------------------------
void PresetHandler::forgetCachedStorages()
{
    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    if (m_eConfigType == E_DOCUMENT)
    {
        m_xWorkingStorageShare.clear();
        m_xWorkingStorageNoLang.clear();
        m_xWorkingStorageUser.clear();
    }

    m_lDocumentStorages.forgetCachedStorages();

    aWriteLock.unlock();
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
::rtl::OUString lcl_getLocalizedMessage(::sal_Int32 nID)
{
    ::rtl::OUString sMessage = ::rtl::OUString::createFromAscii("Unknown error.");

    switch(nID)
    {
        case ID_CORRUPT_UICONFIG_SHARE :
                sMessage = ::rtl::OUString( String( FwkResId( STR_CORRUPT_UICFG_SHARE )));
                break;

        case ID_CORRUPT_UICONFIG_USER :
                sMessage = ::rtl::OUString( String( FwkResId( STR_CORRUPT_UICFG_USER )));
                break;

        case ID_CORRUPT_UICONFIG_GENERAL :
                sMessage = ::rtl::OUString( String( FwkResId( STR_CORRUPT_UICFG_GENERAL )));
                break;
    }

    return sMessage;
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::getOrCreateRootStorageShare()
{
    css::uno::Reference< css::embed::XStorage > xRoot = m_aSharedStorages->m_lStoragesShare.getRootStorage();
    if (xRoot.is())
        return xRoot;

    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    css::uno::Reference< css::beans::XPropertySet > xPathSettings(
        xSMGR->createInstance(SERVICENAME_PATHSETTINGS),
        css::uno::UNO_QUERY_THROW);

    ::rtl::OUString sShareLayer;
    xPathSettings->getPropertyValue(BASEPATH_SHARE_LAYER) >>= sShareLayer;

    // "UIConfig" is a "multi path" ... use first part only here!
    sal_Int32 nPos = sShareLayer.indexOf(';');
    if (nPos > 0)
        sShareLayer = sShareLayer.copy(0, nPos);

    // Note: Maybe a user uses URLs without a final slash! Check it ...
    nPos = sShareLayer.lastIndexOf('/');
    if (nPos != sShareLayer.getLength()-1)
        sShareLayer += ::rtl::OUString::createFromAscii("/");

    sShareLayer += RELPATH_SHARE_LAYER; // folder
    /*
    // TODO remove me!
    // Attention: This is temp. workaround ... We create a temp. storage file
    // based of a system directory. This must be used so, till the storage implementation
    // can work on directories too.
    */
    css::uno::Sequence< css::uno::Any > lArgs(2);
    lArgs[0] <<= sShareLayer;
    lArgs[1] <<= css::embed::ElementModes::READ | css::embed::ElementModes::NOCREATE;

    css::uno::Reference< css::lang::XSingleServiceFactory > xStorageFactory(xSMGR->createInstance(SERVICENAME_FILESYSTEMSTORAGEFACTORY)  , css::uno::UNO_QUERY_THROW);
    css::uno::Reference< css::embed::XStorage >             xStorage;

    try
    {
        xStorage = css::uno::Reference< css::embed::XStorage >(xStorageFactory->createInstanceWithArguments(lArgs), css::uno::UNO_QUERY_THROW);
    }
    catch(const css::uno::Exception& ex)
    {
        throw css::configuration::CorruptedUIConfigurationException(
            lcl_getLocalizedMessage(ID_CORRUPT_UICONFIG_SHARE),
            css::uno::Reference< css::uno::XInterface >(),
            ex.Message);
    }

    m_aSharedStorages->m_lStoragesShare.setRootStorage(xStorage);

    return xStorage;
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::getOrCreateRootStorageUser()
{
    css::uno::Reference< css::embed::XStorage > xRoot = m_aSharedStorages->m_lStoragesUser.getRootStorage();
    if (xRoot.is())
        return xRoot;

    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    css::uno::Reference< css::beans::XPropertySet > xPathSettings(
        xSMGR->createInstance(SERVICENAME_PATHSETTINGS),
        css::uno::UNO_QUERY_THROW);

    ::rtl::OUString sUserLayer;
    xPathSettings->getPropertyValue(BASEPATH_USER_LAYER) >>= sUserLayer ;

    // Note: Maybe a user uses URLs without a final slash! Check it ...
    sal_Int32 nPos = sUserLayer.lastIndexOf('/');
    if (nPos != sUserLayer.getLength()-1)
        sUserLayer += ::rtl::OUString::createFromAscii("/");

    sUserLayer  += RELPATH_USER_LAYER; // storage file

    css::uno::Sequence< css::uno::Any > lArgs(2);
    lArgs[0] <<= sUserLayer;
    lArgs[1] <<= css::embed::ElementModes::READWRITE;

    css::uno::Reference< css::lang::XSingleServiceFactory > xStorageFactory(xSMGR->createInstance(SERVICENAME_FILESYSTEMSTORAGEFACTORY)  , css::uno::UNO_QUERY_THROW);
    css::uno::Reference< css::embed::XStorage >             xStorage;

    try
    {
        xStorage = css::uno::Reference< css::embed::XStorage >(xStorageFactory->createInstanceWithArguments(lArgs), css::uno::UNO_QUERY_THROW);
    }
    catch(const css::uno::Exception& ex)
    {
        throw css::configuration::CorruptedUIConfigurationException(
            lcl_getLocalizedMessage(ID_CORRUPT_UICONFIG_USER),
            css::uno::Reference< css::uno::XInterface >(),
            ex.Message);
    }

    m_aSharedStorages->m_lStoragesUser.setRootStorage(xStorage);

    return xStorage;
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::getWorkingStorageShare()
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    return m_xWorkingStorageShare;
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::getWorkingStorageUser()
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    return m_xWorkingStorageUser;
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::getParentStorageShare(const css::uno::Reference< css::embed::XStorage >& /*xChild*/)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xWorking = m_xWorkingStorageShare;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    return m_aSharedStorages->m_lStoragesShare.getParentStorage(xWorking);
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::getParentStorageUser(const css::uno::Reference< css::embed::XStorage >& /*xChild*/)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xWorking = m_xWorkingStorageUser;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    return m_aSharedStorages->m_lStoragesUser.getParentStorage(xWorking);
}

//-----------------------------------------------
void PresetHandler::connectToResource(      PresetHandler::EConfigType                   eConfigType  ,
                                      const ::rtl::OUString&                             sResource    ,
                                      const ::rtl::OUString&                             sModule      ,
                                      const css::uno::Reference< css::embed::XStorage >& xDocumentRoot,
                                      const ::comphelper::Locale&                        aLocale      )
{
    // TODO free all current open storages!

    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    m_eConfigType   = eConfigType  ;
    m_sResourceType = sResource    ;
    m_sModule       = sModule      ;
    m_aLocale       = aLocale      ;

    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    css::uno::Reference< css::embed::XStorage > xShare;
    css::uno::Reference< css::embed::XStorage > xNoLang;
    css::uno::Reference< css::embed::XStorage > xUser;

    // special case for documents
    // use outside root storage, if we run in E_DOCUMENT mode!
    if (eConfigType == E_DOCUMENT)
    {
        if (!xDocumentRoot.is())
            throw css::uno::RuntimeException(
                    ::rtl::OUString::createFromAscii("There is valid root storage, where the UI configuration can work on."),
                    css::uno::Reference< css::uno::XInterface >());
        m_lDocumentStorages.setRootStorage(xDocumentRoot);
        xShare = xDocumentRoot;
        xUser  = xDocumentRoot;
    }
    else
    {
        xShare = getOrCreateRootStorageShare();
        xUser  = getOrCreateRootStorageUser();
    }

    // #...#
    try
    {

    // a) inside share layer we should not create any new structures ... We have to use
    //    existing ones only!
    // b) inside user layer we can (SOFT mode!) but sometimes we shouldn't (HARD mode!)
    //    create new empty structures. We should prefer using of any existing structure.
    sal_Int32 eShareMode = (css::embed::ElementModes::READ      | css::embed::ElementModes::NOCREATE);
    sal_Int32 eUserMode  = (css::embed::ElementModes::READWRITE                                     );

    ::rtl::OUStringBuffer sRelPathBuf(1024);
    ::rtl::OUString       sRelPathShare;
    ::rtl::OUString       sRelPathNoLang;
    ::rtl::OUString       sRelPathUser;
    switch(eConfigType)
    {
        case E_GLOBAL :
        {
            sRelPathBuf.append(SUBSTORAGE_GLOBAL);
            sRelPathBuf.append(PATH_SEPERATOR   );
            sRelPathBuf.append(sResource        );
            sRelPathShare = sRelPathBuf.makeStringAndClear();
            sRelPathUser  = sRelPathShare;

            xShare = impl_openPathIgnoringErrors(sRelPathShare, eShareMode, sal_True );
            xUser  = impl_openPathIgnoringErrors(sRelPathUser , eUserMode , sal_False);
        }
        break;

        case E_MODULES :
        {
            sRelPathBuf.append(SUBSTORAGE_MODULES);
            sRelPathBuf.append(PATH_SEPERATOR    );
            sRelPathBuf.append(sModule           );
            sRelPathBuf.append(PATH_SEPERATOR    );
            sRelPathBuf.append(sResource         );
            sRelPathShare = sRelPathBuf.makeStringAndClear();
            sRelPathUser  = sRelPathShare;

            xShare = impl_openPathIgnoringErrors(sRelPathShare, eShareMode, sal_True );
            xUser  = impl_openPathIgnoringErrors(sRelPathUser , eUserMode , sal_False);
        }
        break;

        case E_DOCUMENT :
        {
            // A document does not have a share layer in real.
            // It has one layer only, and this one should be opened READ_WRITE.
            // So we open the user layer here only and set the share layer equals to it .-)

            sRelPathBuf.append(sResource);
            sRelPathUser  = sRelPathBuf.makeStringAndClear();
            sRelPathShare = sRelPathUser;

            try
            {
                xUser  = m_lDocumentStorages.openPath(sRelPathUser , eUserMode );
                xShare = xUser;
            }
            catch(const css::uno::RuntimeException& exRun)
                { throw exRun; }
            catch(const css::uno::Exception&)
                { xShare.clear(); xUser.clear(); }
        }
        break;
    }

    // Non-localized global share
    xNoLang = xShare;
    sRelPathNoLang = sRelPathShare;

    if (
        (aLocale     != ::comphelper::Locale::X_NOTRANSLATE()) && // localized level?
        (eConfigType != E_DOCUMENT                           )    // no localization in document mode!
       )
    {
        // First try to find the right localized set inside share layer.
        // Fallbacks are allowed there.
        ::comphelper::Locale aShareLocale       = aLocale      ;
        ::rtl::OUString      sLocalizedSharePath(sRelPathShare);
        sal_Bool             bAllowFallbacks    = sal_True     ;
        xShare = impl_openLocalizedPathIgnoringErrors(sLocalizedSharePath, eShareMode, sal_True , aShareLocale, bAllowFallbacks);

        // The try to locate the right sub dir inside user layer ... without using fallbacks!
        // Normally the corresponding sub dir should be created matching the specified locale.
        // Because we allow creation of storages inside user layer by default.
        ::comphelper::Locale aUserLocale        = aLocale    ;
        ::rtl::OUString      sLocalizedUserPath(sRelPathUser);
                             bAllowFallbacks    = sal_False  ;
        xUser = impl_openLocalizedPathIgnoringErrors(sLocalizedUserPath, eUserMode , sal_False, aUserLocale, bAllowFallbacks);

        sRelPathShare = sLocalizedSharePath;
        sRelPathUser  = sLocalizedUserPath ;
    }

    // read content of level 3 (presets, targets)
          css::uno::Reference< css::container::XNameAccess > xAccess ;
          css::uno::Sequence< ::rtl::OUString >              lNames  ;
    const ::rtl::OUString*                                   pNames  ;
          sal_Int32                                          c       ;
          sal_Int32                                          i       ;
          OUStringList                                       lPresets;
          OUStringList                                       lTargets;

    // read preset names of share layer
    xAccess = css::uno::Reference< css::container::XNameAccess >(xShare, css::uno::UNO_QUERY);
    if (xAccess.is())
    {
        lNames  = xAccess->getElementNames();
        pNames  = lNames.getConstArray();
        c       = lNames.getLength();

        for (i=0; i<c; ++i)
        {
            ::rtl::OUString sTemp = pNames[i];
            sal_Int32       nPos  = sTemp.indexOf(FILE_EXTENSION);
            if (nPos > -1)
                sTemp = sTemp.copy(0,nPos);
            lPresets.push_back(sTemp);
        }
    }

    // read preset names of user layer
    xAccess = css::uno::Reference< css::container::XNameAccess >(xUser, css::uno::UNO_QUERY);
    if (xAccess.is())
    {
        lNames  = xAccess->getElementNames();
        pNames  = lNames.getConstArray();
        c       = lNames.getLength();

        for (i=0; i<c; ++i)
        {
            ::rtl::OUString sTemp = pNames[i];
            sal_Int32       nPos  = sTemp.indexOf(FILE_EXTENSION);
            if (nPos > -1)
                sTemp = sTemp.copy(0,nPos);
            lTargets.push_back(sTemp);
        }
    }

    // SAFE -> ----------------------------------
    aWriteLock.lock();

    m_xWorkingStorageShare = xShare  ;
    m_xWorkingStorageNoLang= xNoLang;
    m_xWorkingStorageUser  = xUser   ;
    m_lPresets             = lPresets;
    m_lTargets             = lTargets;
    m_sRelPathShare        = sRelPathShare;
    m_sRelPathNoLang       = sRelPathNoLang;
    m_sRelPathUser         = sRelPathUser;

    aWriteLock.unlock();
    // <- SAFE ----------------------------------

    }
    catch(const css::uno::Exception& ex)
    {
        throw css::configuration::CorruptedUIConfigurationException(
            lcl_getLocalizedMessage(ID_CORRUPT_UICONFIG_GENERAL),
            css::uno::Reference< css::uno::XInterface >(),
            ex.Message);
    }
}

//-----------------------------------------------
void PresetHandler::copyPresetToTarget(const ::rtl::OUString& sPreset,
                                       const ::rtl::OUString& sTarget)
{
    // don't check our preset list, if element exists
    // We try to open it and forward all errors to the user!

    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xWorkingShare = m_xWorkingStorageShare;
    css::uno::Reference< css::embed::XStorage > xWorkingNoLang= m_xWorkingStorageNoLang;
    css::uno::Reference< css::embed::XStorage > xWorkingUser  = m_xWorkingStorageUser ;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // e.g. module without any config data ?!
    if (
        (!xWorkingShare.is()) ||
        (!xWorkingUser.is() )
       )
    {
       return;
    }

    ::rtl::OUString sPresetFile(sPreset);
    sPresetFile += FILE_EXTENSION;

    ::rtl::OUString sTargetFile(sTarget);
    sTargetFile += FILE_EXTENSION;

    // remove existing elements before you try to copy the preset to that location ...
    // Otherwise w will get an ElementExistException inside copyElementTo()!
    css::uno::Reference< css::container::XNameAccess > xCheckingUser(xWorkingUser, css::uno::UNO_QUERY_THROW);
    if (xCheckingUser->hasByName(sTargetFile))
        xWorkingUser->removeElement(sTargetFile);

    xWorkingShare->copyElementTo(sPresetFile, xWorkingUser, sTargetFile);

    // If our storages work in transacted mode, we have
    // to commit all changes from bottom to top!
    commitUserChanges();
}

//-----------------------------------------------
css::uno::Reference< css::io::XStream > PresetHandler::openPreset(const ::rtl::OUString& sPreset,
                                                                  sal_Bool bUseNoLangGlobal)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xFolder = bUseNoLangGlobal? m_xWorkingStorageNoLang: m_xWorkingStorageShare;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // e.g. module without any config data ?!
    if (!xFolder.is())
       return css::uno::Reference< css::io::XStream >();

    ::rtl::OUString sFile(sPreset);
    sFile += FILE_EXTENSION;

    // inform user about errors (use original exceptions!)
    css::uno::Reference< css::io::XStream > xStream = xFolder->openStreamElement(sFile, css::embed::ElementModes::READ);
    return xStream;
}

//-----------------------------------------------
css::uno::Reference< css::io::XStream > PresetHandler::openTarget(const ::rtl::OUString& sTarget         ,
                                                                        sal_Bool         bCreateIfMissing)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xFolder = m_xWorkingStorageUser;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // e.g. module without any config data ?!
    if (!xFolder.is())
       return css::uno::Reference< css::io::XStream >();

    ::rtl::OUString sFile(sTarget);
    sFile += FILE_EXTENSION;

    sal_Int32 nOpenMode = css::embed::ElementModes::READWRITE;
    if (!bCreateIfMissing)
        nOpenMode |= css::embed::ElementModes::NOCREATE;

    // try it in read/write mode first and ignore errors.
    css::uno::Reference< css::io::XStream > xStream;
    try
    {
        xStream = xFolder->openStreamElement(sFile, nOpenMode);
        return xStream;
    }
    catch(const css::uno::RuntimeException&)
        { throw; }
    catch(const css::uno::Exception&)
        { xStream.clear(); }

    // try it readonly if it failed before.
    // inform user about errors (use original exceptions!)
    nOpenMode &= ~css::embed::ElementModes::WRITE;
    xStream    = xFolder->openStreamElement(sFile, nOpenMode);

    return xStream;
}

//-----------------------------------------------
void PresetHandler::commitUserChanges()
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xWorking = m_xWorkingStorageUser;
    EConfigType                                 eCfgType = m_eConfigType;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // e.g. module without any config data ?!
    if (!xWorking.is())
       return;

    ::rtl::OUString sPath;

    switch(eCfgType)
    {
        case E_GLOBAL :
        case E_MODULES :
        {
            sPath = m_aSharedStorages->m_lStoragesUser.getPathOfStorage(xWorking);
            m_aSharedStorages->m_lStoragesUser.commitPath(sPath);
            m_aSharedStorages->m_lStoragesUser.notifyPath(sPath);
        }
        break;

        case E_DOCUMENT :
        {
            sPath = m_lDocumentStorages.getPathOfStorage(xWorking);
            m_lDocumentStorages.commitPath(sPath);
            m_lDocumentStorages.notifyPath(sPath);
        }
        break;
    }
}

//-----------------------------------------------
void PresetHandler::addStorageListener(IStorageListener* pListener)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    ::rtl::OUString sRelPath = m_sRelPathUser; // use user path ... because we don't work directly on the share layer!
    EConfigType     eCfgType = m_eConfigType;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    if (!sRelPath.getLength())
        return;

    switch(eCfgType)
    {
        case E_GLOBAL :
        case E_MODULES :
        {
            m_aSharedStorages->m_lStoragesUser.addStorageListener(pListener, sRelPath);
        }
        break;

        case E_DOCUMENT :
        {
            m_lDocumentStorages.addStorageListener(pListener, sRelPath);
        }
        break;
    }
}

//-----------------------------------------------
void PresetHandler::removeStorageListener(IStorageListener* pListener)
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    ::rtl::OUString sRelPath = m_sRelPathUser; // use user path ... because we don't work directly on the share layer!
    EConfigType     eCfgType = m_eConfigType;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    if (!sRelPath.getLength())
        return;

    switch(eCfgType)
    {
        case E_GLOBAL :
        case E_MODULES :
        {
            m_aSharedStorages->m_lStoragesUser.removeStorageListener(pListener, sRelPath);
        }
        break;

        case E_DOCUMENT :
        {
            m_lDocumentStorages.removeStorageListener(pListener, sRelPath);
        }
        break;
    }
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::impl_openPathIgnoringErrors(const ::rtl::OUString& sPath ,
                                                                                             sal_Int32        eMode ,
                                                                                             sal_Bool         bShare)
{
    css::uno::Reference< css::embed::XStorage > xPath;
    try
    {
        if (bShare)
            xPath = m_aSharedStorages->m_lStoragesShare.openPath(sPath, eMode);
        else
            xPath = m_aSharedStorages->m_lStoragesUser.openPath(sPath, eMode);
    }
    catch(const css::uno::RuntimeException& exRun)
        { throw exRun; }
    catch(const css::uno::Exception&)
        { xPath.clear(); }
    return xPath;
}

//-----------------------------------------------
::std::vector< ::rtl::OUString >::const_iterator PresetHandler::impl_findMatchingLocalizedValue(const ::std::vector< ::rtl::OUString >& lLocalizedValues,
                                                                                                      ::comphelper::Locale&             aLocale         ,
                                                                                                      sal_Bool                          bAllowFallbacks )
{
    ::std::vector< ::rtl::OUString >::const_iterator pFound = lLocalizedValues.end();
    if (bAllowFallbacks)
    {
        pFound = ::comphelper::Locale::getFallback(lLocalizedValues, aLocale.toISO());
    }
    else
    {
        for (  pFound  = lLocalizedValues.begin();
               pFound != lLocalizedValues.end()  ;
             ++pFound                            )
        {
            const ::rtl::OUString&     sCheckISO   = *pFound;
                  ::comphelper::Locale aCheckLocale(sCheckISO);
            if (aCheckLocale.equals(aLocale))
                break;
        }
    }

    // if we found a valid locale ... take it over to our in/out parameter aLocale
    if (pFound != lLocalizedValues.end())
    {
        const ::rtl::OUString& sISOLocale = *pFound;
        aLocale.fromISO(sISOLocale);
    }

    return pFound;
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > PresetHandler::impl_openLocalizedPathIgnoringErrors(::rtl::OUString&      sPath         ,
                                                                                                sal_Int32             eMode         ,
                                                                                                sal_Bool              bShare        ,
                                                                                                ::comphelper::Locale& aLocale       ,
                                                                                                sal_Bool              bAllowFallback)
{
    css::uno::Reference< css::embed::XStorage >      xPath         = impl_openPathIgnoringErrors(sPath, eMode, bShare);
    ::std::vector< ::rtl::OUString >                 lSubFolders   = impl_getSubFolderNames(xPath);
    ::std::vector< ::rtl::OUString >::const_iterator pLocaleFolder = impl_findMatchingLocalizedValue(lSubFolders, aLocale, bAllowFallback);

    // no fallback ... creation not allowed => no storage
    if (
        (pLocaleFolder == lSubFolders.end()                                                ) &&
        ((eMode & css::embed::ElementModes::NOCREATE) == css::embed::ElementModes::NOCREATE)
       )
        return css::uno::Reference< css::embed::XStorage >();

    // it doesn't matter, if there is a locale fallback or not
    // If creation of storages is allowed, we do it anyway.
    // Otherwise we have no acc config at all, which can make other trouble.
    ::rtl::OUString sLocalizedPath;
    sLocalizedPath  = sPath;
    sLocalizedPath += PATH_SEPERATOR;
    if (pLocaleFolder != lSubFolders.end())
        sLocalizedPath += *pLocaleFolder;
    else
        sLocalizedPath += aLocale.toISO();

    css::uno::Reference< css::embed::XStorage > xLocalePath = impl_openPathIgnoringErrors(sLocalizedPath, eMode, bShare);

    if (xLocalePath.is())
        sPath = sLocalizedPath;
    else
        sPath = ::rtl::OUString();

    return xLocalePath;
}

//-----------------------------------------------
::std::vector< ::rtl::OUString > PresetHandler::impl_getSubFolderNames(const css::uno::Reference< css::embed::XStorage >& xFolder)
{
    css::uno::Reference< css::container::XNameAccess > xAccess(xFolder, css::uno::UNO_QUERY);
    if (!xAccess.is())
        return ::std::vector< ::rtl::OUString >();

          ::std::vector< ::rtl::OUString >      lSubFolders;
    const css::uno::Sequence< ::rtl::OUString > lNames = xAccess->getElementNames();
    const ::rtl::OUString*                      pNames = lNames.getConstArray();
          sal_Int32                             c      = lNames.getLength();
          sal_Int32                             i      = 0;

    for (i=0; i<c; ++i)
    {
        try
        {
            if (xFolder->isStorageElement(pNames[i]))
                lSubFolders.push_back(pNames[i]);
        }
        catch(const css::uno::RuntimeException& exRun)
            { throw exRun; }
        catch(const css::uno::Exception&)
            {}
    }

    return lSubFolders;
}

//-----------------------------------------------
} // namespace framework
