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
#include <accelerators/storageholder.hxx>

//===============================================
// own includes
#include <threadhelp/readguard.hxx>
#include <threadhelp/writeguard.hxx>
#include <services.h>

//===============================================
// interface includes

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

#ifndef __COM_SUN_STAR_EMBED_XPACKAGESTRUCTURECREATOR_HPP_
#include <com/sun/star/embed/XPackageStructureCreator.hpp>
#endif

#ifndef __COM_SUN_STAR_LANG_XSINGLESERVICEFACTORY_HPP_
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#endif

#ifndef __COM_SUN_STAR_IO_XSEEKABLE_HPP_
#include <com/sun/star/io/XSeekable.hpp>
#endif

//===============================================
// other includes
#include <comphelper/processfactory.hxx>

//===============================================
// const

#define PATH_SEPERATOR_ASCII        "/"
#define PATH_SEPERATOR_UNICODE      ((sal_Unicode)'/')
#define PATH_SEPERATOR              ::rtl::OUString::createFromAscii(PATH_SEPERATOR_ASCII)

//===============================================
// namespace

namespace framework
{

namespace css = ::com::sun::star;    

//-----------------------------------------------
StorageHolder::StorageHolder()
    : ThreadHelpBase(                                        )
    , m_xSMGR       (::comphelper::getProcessServiceFactory())
{
}

//-----------------------------------------------
StorageHolder::StorageHolder(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR)
    : ThreadHelpBase(     )
    , m_xSMGR       (xSMGR)
{
}

//-----------------------------------------------
StorageHolder::~StorageHolder()
{
    // TODO implement me
    // dispose/clear etcpp.
}

//-----------------------------------------------
void StorageHolder::forgetCachedStorages()
{
    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    TPath2StorageInfo::iterator pIt;
    for (  pIt  = m_lStorages.begin();
           pIt != m_lStorages.end()  ;
         ++pIt                       )
    {
        TStorageInfo& rInfo = pIt->second;
        // TODO think about listener !
        rInfo.Storage.clear();
    }
    m_lStorages.clear();
    
    aWriteLock.unlock();
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
void StorageHolder::setRootStorage(const css::uno::Reference< css::embed::XStorage >& xRoot)
{
    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);
    m_xRoot = xRoot;
    aWriteLock.unlock();
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > StorageHolder::getRootStorage() const
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    return m_xRoot;
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > StorageHolder::openPath(const ::rtl::OUString& sPath    ,
                                                                          sal_Int32        nOpenMode)
{
    ::rtl::OUString sNormedPath = StorageHolder::impl_st_normPath(sPath);
    OUStringList    lFolders    = StorageHolder::impl_st_parsePath(sNormedPath);
    
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    css::uno::Reference< css::embed::XStorage > xParent = m_xRoot;
    aReadLock.unlock();
    // <- SAFE ----------------------------------

    css::uno::Reference< css::embed::XStorage > xChild  ;
    ::rtl::OUString                             sRelPath;    
    OUStringList::const_iterator                pIt     ;
    
    for (  pIt  = lFolders.begin();
           pIt != lFolders.end()  ;
         ++pIt                    )
    {
        const ::rtl::OUString& sChild     = *pIt;
              ::rtl::OUString  sCheckPath (sRelPath);
                               sCheckPath += sChild;
                               sCheckPath += PATH_SEPERATOR;
              
        // SAFE -> ------------------------------
        aReadLock.lock();

        // If we found an already open storage ... we must increase
        // its use count. Otherwise it will maybe closed to early :-)
        TPath2StorageInfo::iterator pCheck = m_lStorages.find(sCheckPath);
        TStorageInfo*               pInfo  = 0;
        if (pCheck != m_lStorages.end())
        {
            pInfo = &(pCheck->second);
            ++(pInfo->UseCount);
            xChild = pInfo->Storage;
        }            
        else
        {
            aReadLock.unlock();
            // <- SAFE ------------------------------

            try
            {
                xChild = StorageHolder::openSubStorageWithFallback(xParent, sChild, nOpenMode, sal_True); // TODO think about delegating fallback decision to our own calli!
            }
            catch(const css::uno::RuntimeException& exRun)
                { throw exRun; }
            catch(const css::uno::Exception& exAny)
                {
                    /* TODO URGENT!
                        in case we found some "already existing storages" on the path before and increased its UseCount ...
                        and now we will get an exception on creating a new sub storage ...
                        we must decrease all UseCounts, which was touched before. Otherwise these storages can't be closed!

                        Idea: Using of another structure member "PossibleUseCount" as vector of unique numbers.
                        Every thread use another unique number to identify all "owned candidates".
                        A flush method with the same unique number force increasing of the "UseCount" variable then
                        inside a synchronized block ...
                    */
                    throw exAny;
                }
            
            // SAFE -> ------------------------------
            WriteGuard aWriteLock(m_aLock);
            pInfo = &(m_lStorages[sCheckPath]); 
            pInfo->Storage  = xChild;
            pInfo->UseCount = 1;                
            aWriteLock.unlock();
            // <- SAFE ------------------------------
        }
        
        xParent   = xChild;
        sRelPath += sChild;
        sRelPath += PATH_SEPERATOR;
    }         
    
    // TODO think about return last storage as working storage ... but don't cache it inside this holder!
    // => otherwise the same storage is maybe committed more than once.
    
    return xChild;
}

//-----------------------------------------------
StorageHolder::TStorageList StorageHolder::getAllPathStorages(const ::rtl::OUString& sPath)
{
    ::rtl::OUString sNormedPath = StorageHolder::impl_st_normPath(sPath);
    OUStringList    lFolders    = StorageHolder::impl_st_parsePath(sNormedPath);
    
    StorageHolder::TStorageList  lStoragesOfPath;
    ::rtl::OUString              sRelPath       ;    
    OUStringList::const_iterator pIt            ;
    
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    
    for (  pIt  = lFolders.begin();
           pIt != lFolders.end()  ;
         ++pIt                    )
    {
        const ::rtl::OUString& sChild     = *pIt;
              ::rtl::OUString  sCheckPath (sRelPath);
                               sCheckPath += sChild;
                               sCheckPath += PATH_SEPERATOR;
              
        TPath2StorageInfo::iterator pCheck = m_lStorages.find(sCheckPath);
        if (pCheck == m_lStorages.end())
        {
            // at least one path element was not found
            // Seems that this path isn't open ...
            lStoragesOfPath.clear();
            return lStoragesOfPath;
        }

        TStorageInfo& rInfo = pCheck->second;
        lStoragesOfPath.push_back(rInfo.Storage);

        sRelPath += sChild;
        sRelPath += PATH_SEPERATOR;
    }        
        
    aReadLock.unlock();
    // <- SAFE ----------------------------------
    
    return lStoragesOfPath;
}

//-----------------------------------------------
void StorageHolder::commitPath(const ::rtl::OUString& sPath)
{
    StorageHolder::TStorageList lStorages = getAllPathStorages(sPath);
    
    css::uno::Reference< css::embed::XTransactedObject > xCommit;
    StorageHolder::TStorageList::reverse_iterator pIt;
    for (  pIt  = lStorages.rbegin(); // order of commit is important ... otherwise changes are not recognized!
           pIt != lStorages.rend()  ;
         ++pIt                      ) 
    {
        xCommit = css::uno::Reference< css::embed::XTransactedObject >(*pIt, css::uno::UNO_QUERY);
        if (!xCommit.is())
            continue;
        xCommit->commit();        
    }
         
    // SAFE -> ------------------------------
    ReadGuard aReadLock(m_aLock);
    xCommit = css::uno::Reference< css::embed::XTransactedObject >(m_xRoot, css::uno::UNO_QUERY);
    aReadLock.unlock();
    // <- SAFE ------------------------------
    
    if (xCommit.is())
        xCommit->commit();
}

//-----------------------------------------------
void StorageHolder::closePath(const ::rtl::OUString& rPath)
{
    ::rtl::OUString sNormedPath = StorageHolder::impl_st_normPath(rPath);
    OUStringList    lFolders    = StorageHolder::impl_st_parsePath(sNormedPath);
    
    /* convert list of paths in the following way:
        [0] = "path_1" => "path_1
        [1] = "path_2" => "path_1/path_2"
        [2] = "path_3" => "path_1/path_2/path_3"
    */
    OUStringList::iterator pIt1       ;
    ::rtl::OUString        sParentPath;
    for (  pIt1  = lFolders.begin();
           pIt1 != lFolders.end()  ;
         ++pIt1                    )
    {
        ::rtl::OUString sCurrentRelPath  = sParentPath;
                        sCurrentRelPath += *pIt1;
                        sCurrentRelPath += PATH_SEPERATOR;
        *pIt1       = sCurrentRelPath;                         
        sParentPath = sCurrentRelPath;                        
    }

    // SAFE -> ------------------------------
    ReadGuard aReadLock(m_aLock);
    
    OUStringList::reverse_iterator pIt2;
    for (  pIt2  = lFolders.rbegin();
           pIt2 != lFolders.rend()  ;
         ++pIt2                     )
    {
        ::rtl::OUString             sPath = *pIt2;
        TPath2StorageInfo::iterator pPath = m_lStorages.find(sPath);
        if (pPath == m_lStorages.end())
            continue; // ???
        
        TStorageInfo& rInfo = pPath->second;
        --rInfo.UseCount;
        if (rInfo.UseCount < 1)
        {
            rInfo.Storage.clear();
            m_lStorages.erase(pPath);
        }    
    }
    
    aReadLock.unlock();
    // <- SAFE ------------------------------
}

//-----------------------------------------------
void StorageHolder::notifyPath(const ::rtl::OUString& sPath)
{
    ::rtl::OUString sNormedPath = StorageHolder::impl_st_normPath(sPath);
    
    // SAFE -> ------------------------------
    ReadGuard aReadLock(m_aLock);
    
    TPath2StorageInfo::iterator pIt1 = m_lStorages.find(sNormedPath);
    if (pIt1 == m_lStorages.end())
        return;
    
    TStorageInfo& rInfo = pIt1->second;
    TStorageListenerList::iterator pIt2;
    for (  pIt2  = rInfo.Listener.begin();
           pIt2 != rInfo.Listener.end()  ;
         ++pIt2                          )
    {         
        IStorageListener* pListener = *pIt2;
        if (pListener)
            pListener->changesOccured(sNormedPath);
    }
    
    aReadLock.unlock();
    // <- SAFE ------------------------------
}

//-----------------------------------------------
void StorageHolder::addStorageListener(      IStorageListener* pListener,
                                       const ::rtl::OUString&  sPath    )
{
    ::rtl::OUString sNormedPath = StorageHolder::impl_st_normPath(sPath);
    
    // SAFE -> ------------------------------
    ReadGuard aReadLock(m_aLock);
    
    TPath2StorageInfo::iterator pIt1 = m_lStorages.find(sNormedPath);
    if (pIt1 == m_lStorages.end())
        return;
    
    TStorageInfo& rInfo = pIt1->second;
    TStorageListenerList::iterator pIt2 = ::std::find(rInfo.Listener.begin(), rInfo.Listener.end(), pListener);
    if (pIt2 == rInfo.Listener.end())
        rInfo.Listener.push_back(pListener);
    
    aReadLock.unlock();
    // <- SAFE ------------------------------
}

//-----------------------------------------------
void StorageHolder::removeStorageListener(      IStorageListener* pListener,
                                          const ::rtl::OUString&  sPath    )
{
    ::rtl::OUString sNormedPath = StorageHolder::impl_st_normPath(sPath);
    
    // SAFE -> ------------------------------
    ReadGuard aReadLock(m_aLock);
    
    TPath2StorageInfo::iterator pIt1 = m_lStorages.find(sNormedPath);
    if (pIt1 == m_lStorages.end())
        return;
    
    TStorageInfo& rInfo = pIt1->second;
    TStorageListenerList::iterator pIt2 = ::std::find(rInfo.Listener.begin(), rInfo.Listener.end(), pListener);
    if (pIt2 != rInfo.Listener.end())
        rInfo.Listener.erase(pIt2);
    
    aReadLock.unlock();
    // <- SAFE ------------------------------
}
    
//-----------------------------------------------
::rtl::OUString StorageHolder::getPathOfStorage(const css::uno::Reference< css::embed::XStorage >& xStorage)
{
    // SAFE -> ------------------------------
    ReadGuard aReadLock(m_aLock);
    
    TPath2StorageInfo::const_iterator pIt;
    for (  pIt  = m_lStorages.begin();
           pIt != m_lStorages.end()  ;
         ++pIt                       )
    {
        const TStorageInfo& rInfo = pIt->second;
        if (rInfo.Storage == xStorage)
            break;
    }

    if (pIt == m_lStorages.end())
        return ::rtl::OUString();

    return pIt->first;
    
    // <- SAFE ------------------------------
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > StorageHolder::getParentStorage(const css::uno::Reference< css::embed::XStorage >& xChild)
{
    ::rtl::OUString sChildPath = getPathOfStorage(xChild);
    return getParentStorage(sChildPath);
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > StorageHolder::getParentStorage(const ::rtl::OUString& sChildPath)
{
    // normed path = "a/b/c/" ... we search for "a/b/"
    ::rtl::OUString sNormedPath = StorageHolder::impl_st_normPath(sChildPath);
    OUStringList    lFolders    = StorageHolder::impl_st_parsePath(sNormedPath);
    sal_Int32       c           = lFolders.size();    

    // a) ""       => -       => no parent
    // b) "a/b/c/" => "a/b/"  => return storage "a/b/"
    // c) "a/"     => ""      => return root !

    // a)
    if (c < 1)
        return css::uno::Reference< css::embed::XStorage >(); 

    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);
    
    // b)
    if (c < 2)
        return m_xRoot;
    
    // c)
    ::rtl::OUString sParentPath;
    sal_Int32       i = 0;
    for (i=0; i<c-1; ++i)
    {        
        sParentPath += lFolders[i];
        sParentPath += PATH_SEPERATOR;
    }

    TPath2StorageInfo::const_iterator pParent = m_lStorages.find(sParentPath);
    if (pParent != m_lStorages.end())
        return pParent->second.Storage;
    
    aReadLock.unlock();
    // <- SAFE ----------------------------------
    
    // ?
    LOG_WARNING("StorageHolder::getParentStorage()", "Unexpected situation. Cached storage item seems to be wrong.")
    return css::uno::Reference< css::embed::XStorage >();
}

//-----------------------------------------------
void StorageHolder::operator=(const StorageHolder& rCopy)
{
    // SAFE -> ----------------------------------
    WriteGuard aWriteLock(m_aLock);

    m_xSMGR     = rCopy.m_xSMGR; // ???
    m_xRoot     = rCopy.m_xRoot;
    m_lStorages = rCopy.m_lStorages;
    
    aWriteLock.unlock();
    // <- SAFE ----------------------------------
}

//-----------------------------------------------
css::uno::Reference< css::embed::XStorage > StorageHolder::openSubStorageWithFallback(const css::uno::Reference< css::embed::XStorage >& xBaseStorage  ,
                                                                                      const ::rtl::OUString&                             sSubStorage   ,
                                                                                            sal_Int32                                    eOpenMode     ,
                                                                                            sal_Bool                                     bAllowFallback)
{
    // a) try it first with user specified open mode
    //    ignore errors ... but save it for later use!
    css::uno::Exception exResult;
    try
    {
        css::uno::Reference< css::embed::XStorage > xSubStorage = xBaseStorage->openStorageElement(sSubStorage, eOpenMode);
        if (xSubStorage.is())
            return xSubStorage;
    }
    catch(const css::uno::RuntimeException&)
        { throw; }
    catch(const css::uno::Exception& ex)
        { exResult = ex; }
        
    // b) readonly already tried? => forward last error!
    if (
        (!bAllowFallback                                                                 ) ||   // fallback allowed  ?
        ((eOpenMode & css::embed::ElementModes::WRITE) != css::embed::ElementModes::WRITE)      // fallback possible ?
       )
        throw exResult;

    // c) try it readonly
    //    don't catch exception here! Outside code whish to know, if operation failed or not.
    //    Otherwise they work on NULL references ...
    sal_Int32 eNewMode = (eOpenMode & ~css::embed::ElementModes::WRITE);
    css::uno::Reference< css::embed::XStorage > xSubStorage = xBaseStorage->openStorageElement(sSubStorage, eNewMode);
    if (xSubStorage.is())
        return xSubStorage;
        
    // d) no chance!
    LOG_WARNING("openSubStorageWithFallback()", "Unexpected situation! Got no exception for missing storage ...")        
    return css::uno::Reference< css::embed::XStorage >();        
}

//-----------------------------------------------
css::uno::Reference< css::io::XStream > StorageHolder::openSubStreamWithFallback(const css::uno::Reference< css::embed::XStorage >& xBaseStorage  ,
                                                                                 const ::rtl::OUString&                             sSubStream    ,
                                                                                       sal_Int32                                    eOpenMode     ,
                                                                                       sal_Bool                                     bAllowFallback)
{
    // a) try it first with user specified open mode
    //    ignore errors ... but save it for later use!
    css::uno::Exception exResult;
    try
    {
        css::uno::Reference< css::io::XStream > xSubStream = xBaseStorage->openStreamElement(sSubStream, eOpenMode);
        if (xSubStream.is())
            return xSubStream;
    }
    catch(const css::uno::RuntimeException&)
        { throw; }
    catch(const css::uno::Exception& ex)
        { exResult = ex; }
        
    // b) readonly already tried? => forward last error!
    if (
        (!bAllowFallback                                                                 ) ||   // fallback allowed  ?
        ((eOpenMode & css::embed::ElementModes::WRITE) != css::embed::ElementModes::WRITE)      // fallback possible ?
       )
        throw exResult;

    // c) try it readonly
    //    don't catch exception here! Outside code whish to know, if operation failed or not.
    //    Otherwise they work on NULL references ...
    sal_Int32 eNewMode = (eOpenMode & ~css::embed::ElementModes::WRITE);
    css::uno::Reference< css::io::XStream > xSubStream = xBaseStorage->openStreamElement(sSubStream, eNewMode);
    if (xSubStream.is())
        return xSubStream;
        
    // d) no chance!
    LOG_WARNING("openSubStreamWithFallbacks()", "Unexpected situation! Got no exception for missing stream ...")        
    return css::uno::Reference< css::io::XStream >();        
}

//-----------------------------------------------
::rtl::OUString StorageHolder::impl_st_normPath(const ::rtl::OUString& sPath)
{
    // path must start without "/" but end with "/"!
    
    ::rtl::OUString sNormedPath = sPath;
    
    // "/bla" => "bla" && "/" => "" (!)
    if (sNormedPath.indexOf(PATH_SEPERATOR) == 0)
        sNormedPath += sNormedPath.copy(1);

    // "/" => "" || "" => "" ?
    if (sNormedPath.getLength() < 1)
        return ::rtl::OUString();
    
    // "bla" => "bla/"
    if (sNormedPath.lastIndexOf(PATH_SEPERATOR) != (sNormedPath.getLength()-1))
        sNormedPath += PATH_SEPERATOR;
    
    return sNormedPath;        
}

//-----------------------------------------------
OUStringList StorageHolder::impl_st_parsePath(const ::rtl::OUString& sPath)
{
    OUStringList lToken;
    sal_Int32    i  = 0;
    while (sal_True)
    {
        ::rtl::OUString sToken = sPath.getToken(0, PATH_SEPERATOR_UNICODE, i);
        if (i < 0)
            break;
        lToken.push_back(sToken);
    }
    return lToken;        
}                                                                                       

//===============================================
} // namespace framework
