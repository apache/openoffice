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
#include "precompiled_filter.hxx"

#include "cacheupdatelistener.hxx"
#include "constant.hxx"

//_______________________________________________
// includes
#include <com/sun/star/util/XChangesNotifier.hpp>
#include <com/sun/star/util/XRefreshable.hpp>
#include <salhelper/singletonref.hxx>
#include <unotools/configpathes.hxx>
#include <rtl/ustring.hxx>

//_______________________________________________
// namespace

namespace filter{
    namespace config{

namespace css = ::com::sun::star;

//_______________________________________________
// definitions

/*-----------------------------------------------
    05.03.2004 08:36
-----------------------------------------------*/
CacheUpdateListener::CacheUpdateListener(const css::uno::Reference< css::lang::XMultiServiceFactory >& xSMGR        ,
                                         const css::uno::Reference< css::uno::XInterface >&            xConfigAccess,
                                               FilterCache::EItemType                                  eConfigType  )
    : BaseLock     (             )
    , m_xSMGR      (xSMGR        )
    , m_rCache     (             )
    , m_xConfig    (xConfigAccess)
    , m_eConfigType(eConfigType  )
{
}

/*-----------------------------------------------
    05.03.2004 08:37
-----------------------------------------------*/
CacheUpdateListener::~CacheUpdateListener()
{
}

/*-----------------------------------------------
    07.03.2004 07:59
-----------------------------------------------*/
void CacheUpdateListener::startListening()
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    css::uno::Reference< css::util::XChangesNotifier > xNotifier(m_xConfig, css::uno::UNO_QUERY);
    aLock.clear();
    // <- SAFE

    if (!xNotifier.is())
        return;

    css::uno::Reference< css::util::XChangesListener > xThis(static_cast< css::util::XChangesListener* >(this), css::uno::UNO_QUERY_THROW);
    xNotifier->addChangesListener(xThis);

/*
    css::uno::Reference< css::container::XContainer > xNotifier(xConfigAccess, css::uno::UNO_QUERY);
    if (!xNotifier.is())
        return;

    css::uno::Reference< css::container::XContainerListener > xThis(static_cast< css::container::XContainerListener* >(this), css::uno::UNO_QUERY);
    xNotifier->addContainerListener(xThis);
*/
}

/*-----------------------------------------------
    07.03.2004 07:59
-----------------------------------------------*/
void CacheUpdateListener::stopListening()
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    css::uno::Reference< css::util::XChangesNotifier > xNotifier(m_xConfig, css::uno::UNO_QUERY);
    aLock.clear();
    // <- SAFE

    if (!xNotifier.is())
        return;

    css::uno::Reference< css::util::XChangesListener > xThis(static_cast< css::util::XChangesListener* >(this), css::uno::UNO_QUERY);
    xNotifier->removeChangesListener(xThis);

/*
    css::uno::Reference< css::container::XContainer > xNotifier(xConfigAccess, css::uno::UNO_QUERY);
    if (!xNotifier.is())
        return;

    css::uno::Reference< css::container::XContainerListener > xThis(static_cast< css::container::XContainerListener* >(this), css::uno::UNO_QUERY);
    xNotifier->removeContainerListener(xThis);
*/
}

/*-----------------------------------------------
    07.03.2004 08:17
-----------------------------------------------*/
void SAL_CALL  CacheUpdateListener::changesOccurred(const css::util::ChangesEvent& aEvent)
    throw(css::uno::RuntimeException)
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);

    // disposed ?
    if ( ! m_xConfig.is())
        return;

    css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = m_xSMGR;
    FilterCache::EItemType                                 eType = m_eConfigType;

    aLock.clear();
    // <- SAFE

    OUStringList lChangedItems;
    sal_Int32    c = aEvent.Changes.getLength();
    sal_Int32    i = 0;

    for (i=0; i<c; ++i)
    {
        const css::util::ElementChange& aChange = aEvent.Changes[i];

        ::rtl::OUString sOrgPath ;
        ::rtl::OUString sTempPath;

        ::rtl::OUString sProperty;
        ::rtl::OUString sNode    ;
        ::rtl::OUString sLocale  ;

        /*  at least we must be able to retrieve 2 path elements
            But sometimes the original path can contain 3 of them ... in case
            a localized value was changed.
            =>
            1) Filters/Filter["filtername"]/Property
            2) Filters/Filter["filtername"]/LocalizedProperty/Locale
        */

        aChange.Accessor >>= sOrgPath;
        if ( ! ::utl::splitLastFromConfigurationPath(sOrgPath, sTempPath, sLocale))
            continue;
        sOrgPath = sTempPath;
        if ( ! ::utl::splitLastFromConfigurationPath(sOrgPath, sTempPath, sProperty))
        {
            sNode     = sLocale;
            sProperty = ::rtl::OUString();
            sLocale   = ::rtl::OUString();
        }
        else
        {
            sOrgPath = sTempPath;
            if ( ! ::utl::splitLastFromConfigurationPath(sOrgPath, sTempPath, sNode))
            {
                sNode     = sProperty;
                sProperty = sLocale;
                sLocale   = ::rtl::OUString();
            }
        }

        if ( ! sNode.getLength() )
            continue;

        OUStringList::const_iterator pIt = ::std::find(lChangedItems.begin(), lChangedItems.end(), sNode);
        if (pIt == lChangedItems.end())
            lChangedItems.push_back(sNode);
    }

    sal_Bool                     bNotifyRefresh = sal_False;
    OUStringList::const_iterator pIt;
    for (  pIt  = lChangedItems.begin();
           pIt != lChangedItems.end()  ;
         ++pIt                         )
    {
        const ::rtl::OUString& sItem = *pIt;
        try
        {
            m_rCache->refreshItem(eType, sItem);
        }
        catch(const css::container::NoSuchElementException&)
            {
                // can be ignored! Because we must be aware that
                // sItem was removed from the condfiguration and we forced an update of the cache.
                // But we know, that the cache is up-to-date know and has thrown this exception afterwards .-)
            }
        // NO FLUSH! Otherwise we start a never ending story here .-)
        bNotifyRefresh = sal_True;
    }

    // notify sfx cache about the changed filter cache .-)
    if (bNotifyRefresh)
    {
        css::uno::Reference< css::util::XRefreshable > xRefreshBroadcaster(
            xSMGR->createInstance(SERVICE_FILTERCONFIGREFRESH),
            css::uno::UNO_QUERY);
        if (xRefreshBroadcaster.is())
            xRefreshBroadcaster->refresh();
    }
}

/*-----------------------------------------------
    05.03.2004 08:44
-----------------------------------------------*/
void SAL_CALL CacheUpdateListener::disposing(const css::lang::EventObject& aEvent)
    throw(css::uno::RuntimeException)
{
    // SAFE ->
    ::osl::ResettableMutexGuard aLock(m_aLock);
    if (aEvent.Source == m_xConfig)
        m_xConfig.clear();
    aLock.clear();
    // <- SAFE
}

    } // namespace config
} // namespace filter
