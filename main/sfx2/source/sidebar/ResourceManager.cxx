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

#include "precompiled_sfx2.hxx"

#include "ResourceManager.hxx"
#include <unotools/confignode.hxx>
#include <comphelper/componentcontext.hxx>
#include <comphelper/processfactory.hxx>
#include <comphelper/namedvaluecollection.hxx>
#include <comphelper/types.hxx>
#include <comphelper/stlunosequence.hxx>

#include <rtl/ustrbuf.hxx>
#include <tools/diagnose_ex.h>

#include <com/sun/star/frame/XModuleManager.hpp>

#include <map>


#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

using ::rtl::OUString;
using namespace css;
using namespace cssu;

namespace sfx2 { namespace sidebar {

#define gsPrivateResourceToolpanelPrefix "private:resource/toolpanel/"



class ResourceManager::Deleter
{
public:
    void operator() (ResourceManager* pObject)
    {
        delete pObject;
    }
};


ResourceManager& ResourceManager::Instance (void)
{
    static ResourceManager maInstance;
    return maInstance;
}




ResourceManager::ResourceManager (void)
    : maDecks(),
      maPanels(),
      maProcessedApplications()
{
    ReadDeckList();
    ReadPanelList();
}




ResourceManager::~ResourceManager (void)
{
    maPanels.clear();
    maDecks.clear();
}




const DeckDescriptor* ResourceManager::GetBestMatchingDeck (
    const Context& rContext,
    const Reference<frame::XFrame>& rxFrame)
{
    ReadLegacyAddons(rxFrame);
    
    sal_Int32 nBestMatch (EnumContext::NoMatch);
    const DeckContainer::const_iterator iEnd (maDecks.end());
    DeckContainer::const_iterator iBestDeck (iEnd);
    
    for (DeckContainer::const_iterator iDeck(maDecks.begin());
         iDeck!=iEnd;
         ++iDeck)
    {
        const sal_Int32 nMatch (iDeck->maContextMatcher.EvaluateMatch(rContext));
        if (nMatch < nBestMatch)
        {
            // Found a better matching deck.
            nBestMatch = nMatch;
            iBestDeck = iDeck;
            if (nBestMatch == EnumContext::OptimalMatch)
            {
                // We will not find a better match.
                break;
            }
        }
    }
    if (iBestDeck != iEnd)
        return &*iBestDeck;
    else
        return NULL;
}




const DeckDescriptor* ResourceManager::GetDeckDescriptor (
    const ::rtl::OUString& rsDeckId) const
{
    for (DeckContainer::const_iterator
             iDeck(maDecks.begin()),
             iEnd(maDecks.end());
         iDeck!=iEnd;
         ++iDeck)
    {
        if (iDeck->msId.equals(rsDeckId))
            return &*iDeck;
    }
    return NULL;
}




const PanelDescriptor* ResourceManager::GetPanelDescriptor (
    const ::rtl::OUString& rsPanelId) const
{
    for (PanelContainer::const_iterator
             iPanel(maPanels.begin()),
             iEnd(maPanels.end());
         iPanel!=iEnd;
         ++iPanel)
    {
        if (iPanel->msId.equals(rsPanelId))
            return &*iPanel;
    }
    return NULL;
}




void ResourceManager::SetIsDeckEnabled (
    const ::rtl::OUString& rsDeckId,
    const bool bIsEnabled)
{
    for (DeckContainer::iterator
             iDeck(maDecks.begin()),
             iEnd(maDecks.end());
         iDeck!=iEnd;
         ++iDeck)
    {
        if (iDeck->msId.equals(rsDeckId))
        {
            iDeck->mbIsEnabled = bIsEnabled;
            return;
        }
    }
}




const ResourceManager::IdContainer& ResourceManager::GetMatchingDecks (
    IdContainer& rDeckIds,
    const Context& rContext,
    const Reference<frame::XFrame>& rxFrame)
{
    ReadLegacyAddons(rxFrame);

    ::std::multimap<sal_Int32,OUString> aOrderedIds;
    for (DeckContainer::const_iterator
             iDeck(maDecks.begin()),
             iEnd (maDecks.end());
         iDeck!=iEnd;
         ++iDeck)
    {
        const DeckDescriptor& rDeckDescriptor (*iDeck);
        if (rDeckDescriptor.maContextMatcher.EvaluateMatch(rContext) != EnumContext::NoMatch)
            aOrderedIds.insert(::std::multimap<sal_Int32,OUString>::value_type(
                    rDeckDescriptor.mnOrderIndex,
                    rDeckDescriptor.msId));
    }

    for (::std::multimap<sal_Int32,OUString>::const_iterator
             iId(aOrderedIds.begin()),
             iEnd(aOrderedIds.end());
         iId!=iEnd;
         ++iId)
    {
        rDeckIds.push_back(iId->second);
    }
    
    return rDeckIds;
}




const ResourceManager::IdContainer& ResourceManager::GetMatchingPanels (
    IdContainer& rPanelIds,
    const Context& rContext,
    const ::rtl::OUString& rsDeckId,
    const Reference<frame::XFrame>& rxFrame)
{
    ReadLegacyAddons(rxFrame);

    ::std::multimap<sal_Int32,OUString> aOrderedIds;
    for (PanelContainer::const_iterator
             iPanel(maPanels.begin()),
             iEnd(maPanels.end());
         iPanel!=iEnd;
         ++iPanel)
    {
        const PanelDescriptor& rPanelDescriptor (*iPanel);
        if (rPanelDescriptor.msDeckId.equals(rsDeckId))
            if (rPanelDescriptor.maContextMatcher.EvaluateMatch(rContext) != EnumContext::NoMatch)
                aOrderedIds.insert(::std::multimap<sal_Int32,OUString>::value_type(
                        rPanelDescriptor.mnOrderIndex,
                        rPanelDescriptor.msId));
    }

    for (::std::multimap<sal_Int32,OUString>::const_iterator
             iId(aOrderedIds.begin()),
             iEnd(aOrderedIds.end());
         iId!=iEnd;
         ++iId)
    {
        rPanelIds.push_back(iId->second);
    }
    
    return rPanelIds;
}




void ResourceManager::ReadDeckList (void)
{
    const ::comphelper::ComponentContext aContext (::comphelper::getProcessServiceFactory());
    const ::utl::OConfigurationTreeRoot aDeckRootNode (
        aContext,
        A2S("org.openoffice.Office.UI.Sidebar/Content/DeckList"),
        false);
    if ( ! aDeckRootNode.isValid() )
        return;

    const Sequence<OUString> aDeckNodeNames (aDeckRootNode.getNodeNames());
    const sal_Int32 nCount (aDeckNodeNames.getLength());
    maDecks.resize(nCount);
    sal_Int32 nWriteIndex(0);
    for (sal_Int32 nReadIndex(0); nReadIndex<nCount; ++nReadIndex)
    {
        const ::utl::OConfigurationNode aDeckNode (aDeckRootNode.openNode(aDeckNodeNames[nReadIndex]));
        if ( ! aDeckNode.isValid())
            continue;

        DeckDescriptor& rDeckDescriptor (maDecks[nWriteIndex++]);

        rDeckDescriptor.msTitle = ::comphelper::getString(
            aDeckNode.getNodeValue("Title"));
        rDeckDescriptor.msId = ::comphelper::getString(
            aDeckNode.getNodeValue("Id"));
        rDeckDescriptor.msIconURL = ::comphelper::getString(
            aDeckNode.getNodeValue("IconURL"));
        rDeckDescriptor.msHighContrastIconURL = ::comphelper::getString(
            aDeckNode.getNodeValue("HighContrastIconURL"));
        rDeckDescriptor.msHelpURL = ::comphelper::getString(
            aDeckNode.getNodeValue("HelpURL"));
        rDeckDescriptor.msHelpText = rDeckDescriptor.msTitle;
        rDeckDescriptor.mbIsEnabled = true;
        rDeckDescriptor.mnOrderIndex = ::comphelper::getINT32(
            aDeckNode.getNodeValue("OrderIndex"));

        ReadContextMatcher(aDeckNode.openNode("ContextMatchers"), rDeckDescriptor.maContextMatcher);
    }

    // When there where invalid nodes then we have to adapt the size
    // of the deck vector.
    if (nWriteIndex<nCount)
        maDecks.resize(nWriteIndex);
}




void ResourceManager::ReadPanelList (void)
{
    const ::comphelper::ComponentContext aContext (::comphelper::getProcessServiceFactory());
    const ::utl::OConfigurationTreeRoot aPanelRootNode (
        aContext,
        A2S("org.openoffice.Office.UI.Sidebar/Content/PanelList"),
        false);
    if ( ! aPanelRootNode.isValid() )
        return;

    const Sequence<OUString> aPanelNodeNames (aPanelRootNode.getNodeNames());
    const sal_Int32 nCount (aPanelNodeNames.getLength());
    maPanels.resize(nCount);
    sal_Int32 nWriteIndex (0);
    for (sal_Int32 nReadIndex(0); nReadIndex<nCount; ++nReadIndex)
    {
        const ::utl::OConfigurationNode aPanelNode (aPanelRootNode.openNode(aPanelNodeNames[nReadIndex]));
        if ( ! aPanelNode.isValid())
            continue;

        PanelDescriptor& rPanelDescriptor (maPanels[nWriteIndex++]);

        rPanelDescriptor.msTitle = ::comphelper::getString(
            aPanelNode.getNodeValue("Title"));
        rPanelDescriptor.mbIsTitleBarOptional = ::comphelper::getBOOL(
            aPanelNode.getNodeValue("TitleBarIsOptional"));
        rPanelDescriptor.msId = ::comphelper::getString(
            aPanelNode.getNodeValue("Id"));
        rPanelDescriptor.msDeckId = ::comphelper::getString(
            aPanelNode.getNodeValue("DeckId"));
        rPanelDescriptor.msHelpURL = ::comphelper::getString(
            aPanelNode.getNodeValue("HelpURL"));
        rPanelDescriptor.msImplementationURL = ::comphelper::getString(
            aPanelNode.getNodeValue("ImplementationURL"));
        rPanelDescriptor.mnOrderIndex = ::comphelper::getINT32(
            aPanelNode.getNodeValue("OrderIndex"));
        rPanelDescriptor.mbHasMenu = ::comphelper::getBOOL(
            aPanelNode.getNodeValue("HasMenu"));
        rPanelDescriptor.mbWantsCanvas = ::comphelper::getBOOL(
            aPanelNode.getNodeValue("WantsCanvas"));
        ReadContextMatcher(aPanelNode.openNode("ContextMatchers"), rPanelDescriptor.maContextMatcher);
    }

    // When there where invalid nodes then we have to adapt the size
    // of the deck vector.
    if (nWriteIndex<nCount)
        maPanels.resize(nWriteIndex);
}




void ResourceManager::ReadContextMatcher (
    const ::utl::OConfigurationNode& rNode,
    ContextMatcher& rContextMatcher) const
{
    const Sequence<OUString> aMatcherNodeNames (rNode.getNodeNames());
    const sal_Int32 nMatcherCount (aMatcherNodeNames.getLength());
    for (sal_Int32 nMatcherIndex(0); nMatcherIndex<nMatcherCount; ++nMatcherIndex)
    {
        const ::utl::OConfigurationNode aMatcherNode (rNode.openNode(aMatcherNodeNames[nMatcherIndex]));
        
        const OUString sApplicationName (
            ::comphelper::getString(aMatcherNode.getNodeValue("Application")));
        const bool bIsContextListNegated (
            ::comphelper::getBOOL(aMatcherNode.getNodeValue("IsContextListNegated")));

        // Read the context names.
        Any aContextListValue (aMatcherNode.getNodeValue("ContextList"));
        Sequence<OUString> aContextList;
        ::std::vector<OUString> aContextVector;
        if (aContextListValue >>= aContextList)
        {
            aContextVector.reserve(aContextList.getLength());
            ::std::copy(
                ::comphelper::stl_begin(aContextList),
                ::comphelper::stl_end(aContextList),
                ::std::back_inserter(aContextVector));
        }
        // Empty list defaults to "any".
        if (aContextVector.empty())
            aContextVector.push_back(A2S("any"));
        
        rContextMatcher.AddMatcher(
                sApplicationName,
                aContextVector,
                bIsContextListNegated);
    }
}




void ResourceManager::ReadLegacyAddons (const Reference<frame::XFrame>& rxFrame)
{
    // Get module name for given frame.
    ::rtl::OUString sModuleName (GetModuleName(rxFrame));
    if (sModuleName.getLength() == 0)
        return;
    if (maProcessedApplications.find(sModuleName) != maProcessedApplications.end())
    {
        // Addons for this application have already been read.
        // There is nothing more to do.
        return;
    }

    // Mark module as processed.  Even when there is an error that
    // prevents the configuration data from being read, this error
    // will not be triggered a second time.
    maProcessedApplications.insert(sModuleName);

    // Get access to the configuration root node for the application.
    ::utl::OConfigurationTreeRoot aLegacyRootNode (GetLegacyAddonRootNode(sModuleName));
    if ( ! aLegacyRootNode.isValid())
        return;

    // Process child nodes.
    ::std::vector<OUString> aMatchingNodeNames;
    GetToolPanelNodeNames(aMatchingNodeNames, aLegacyRootNode);
    const sal_Int32 nCount (aMatchingNodeNames.size());
    size_t nDeckWriteIndex (maDecks.size());
    size_t nPanelWriteIndex (maPanels.size());
    maDecks.resize(maDecks.size() + nCount);
    maPanels.resize(maPanels.size() + nCount);
    for (sal_Int32 nReadIndex(0); nReadIndex<nCount; ++nReadIndex)
    {
        const OUString& rsNodeName (aMatchingNodeNames[nReadIndex]);
        const ::utl::OConfigurationNode aChildNode (aLegacyRootNode.openNode(rsNodeName));
        if ( ! aChildNode.isValid())
            continue;

        DeckDescriptor& rDeckDescriptor (maDecks[nDeckWriteIndex++]);
        rDeckDescriptor.msTitle = ::comphelper::getString(aChildNode.getNodeValue("UIName"));
        rDeckDescriptor.msId = rsNodeName;
        rDeckDescriptor.msIconURL = ::comphelper::getString(aChildNode.getNodeValue("ImageURL"));
        rDeckDescriptor.msHighContrastIconURL = rDeckDescriptor.msIconURL;
        rDeckDescriptor.msHelpURL = ::comphelper::getString(aChildNode.getNodeValue("HelpURL"));
        rDeckDescriptor.msHelpText = rDeckDescriptor.msTitle;
        rDeckDescriptor.maContextMatcher.AddMatcher(sModuleName, A2S("any"));
        rDeckDescriptor.mbIsEnabled = true;

        PanelDescriptor& rPanelDescriptor (maPanels[nPanelWriteIndex++]);
        rPanelDescriptor.msTitle = ::comphelper::getString(aChildNode.getNodeValue("UIName"));
        rPanelDescriptor.mbIsTitleBarOptional = true;
        rPanelDescriptor.msId = rsNodeName;
        rPanelDescriptor.msDeckId = rsNodeName;
        rPanelDescriptor.msHelpURL = ::comphelper::getString(aChildNode.getNodeValue("HelpURL"));
        rPanelDescriptor.maContextMatcher.AddMatcher(sModuleName, A2S("any"));
        rPanelDescriptor.msImplementationURL = rsNodeName;            
    }

    // When there where invalid nodes then we have to adapt the size
    // of the deck and panel vectors.
    if (nDeckWriteIndex < maDecks.size())
        maDecks.resize(nDeckWriteIndex);
    if (nPanelWriteIndex < maPanels.size())
        maPanels.resize(nPanelWriteIndex);
}




::rtl::OUString ResourceManager::GetModuleName (
    const cssu::Reference<css::frame::XFrame>& rxFrame)
{
    try
    {
        const ::comphelper::ComponentContext aContext (::comphelper::getProcessServiceFactory());
        const Reference<frame::XModuleManager> xModuleManager (
            aContext.createComponent("com.sun.star.frame.ModuleManager" ),
            UNO_QUERY_THROW );
        return xModuleManager->identify(rxFrame);
    }
    catch (const Exception&)
    {
        DBG_UNHANDLED_EXCEPTION();
    }
    return OUString();
}   




::utl::OConfigurationTreeRoot ResourceManager::GetLegacyAddonRootNode (
    const ::rtl::OUString& rsModuleName) const
{
    try
    {
        const ::comphelper::ComponentContext aContext (::comphelper::getProcessServiceFactory());
        const Reference<container::XNameAccess> xModuleAccess (
            aContext.createComponent("com.sun.star.frame.ModuleManager"),
            UNO_QUERY_THROW);
        const ::comphelper::NamedValueCollection aModuleProperties (xModuleAccess->getByName(rsModuleName));
        const ::rtl::OUString sWindowStateRef (aModuleProperties.getOrDefault(
                "ooSetupFactoryWindowStateConfigRef",
                ::rtl::OUString()));

        ::rtl::OUStringBuffer aPathComposer;
        aPathComposer.appendAscii("org.openoffice.Office.UI.");
        aPathComposer.append(sWindowStateRef);
        aPathComposer.appendAscii("/UIElements/States");

        return ::utl::OConfigurationTreeRoot(aContext, aPathComposer.makeStringAndClear(), false);
    }
    catch( const Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }

    return ::utl::OConfigurationTreeRoot();
}




void ResourceManager::GetToolPanelNodeNames (
    ::std::vector<OUString>& rMatchingNames,
    const ::utl::OConfigurationTreeRoot aRoot) const
{
    Sequence<OUString> aChildNodeNames (aRoot.getNodeNames());
    const sal_Int32 nCount (aChildNodeNames.getLength());
    for (sal_Int32 nIndex(0); nIndex<nCount; ++nIndex)
    {
        if (aChildNodeNames[nIndex].matchAsciiL(
                RTL_CONSTASCII_STRINGPARAM( "private:resource/toolpanel/")))
            rMatchingNames.push_back(aChildNodeNames[nIndex]);
    }
}



} } // end of namespace sfx2::sidebar
