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
#include <comphelper/namedvaluecollection.hxx>
#include <tools/diagnose_ex.h>


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
    
    sal_Int32 nBestMatch (Context::NoMatch);
    const DeckContainer::const_iterator iEnd (maDecks.end());
    DeckContainer::const_iterator iBestDeck (iEnd);
    
    for (DeckContainer::const_iterator iDeck(maDecks.begin());
         iDeck!=iEnd;
         ++iDeck)
    {
        const sal_Int32 nMatch (rContext.EvaluateMatch(iDeck->maContexts));
        if (nMatch < nBestMatch)
        {
            // Found a better matching decks.
            nBestMatch = nMatch;
            iBestDeck = iDeck;
            if (nBestMatch == Context::OptimalMatch)
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




const ResourceManager::DeckContainer& ResourceManager::GetMatchingDecks (
    DeckContainer& rDeckDescriptors,
    const Context& rContext,
    const Reference<frame::XFrame>& rxFrame)
{
    ReadLegacyAddons(rxFrame);

    for (DeckContainer::const_iterator
             iDeck(maDecks.begin()),
             iEnd (maDecks.end());
         iDeck!=iEnd;
         ++iDeck)
    {
        if (rContext.EvaluateMatch(iDeck->maContexts) != Context::NoMatch)
            rDeckDescriptors.push_back(*iDeck);
    }

    return rDeckDescriptors;
}




const ResourceManager::PanelContainer& ResourceManager::GetMatchingPanels (
    PanelContainer& rPanelDescriptors,
    const Context& rContext,
    const ::rtl::OUString& rsDeckId,
    const Reference<frame::XFrame>& rxFrame)
{
    ReadLegacyAddons(rxFrame);

    for (PanelContainer::const_iterator
             iPanel(maPanels.begin()),
             iEnd(maPanels.end());
         iPanel!=iEnd;
         ++iPanel)
    {
        const PanelDescriptor& rPanelDescriptor (*iPanel);
        if (rPanelDescriptor.msDeckId.equals(rsDeckId))
            if (rContext.EvaluateMatch(rPanelDescriptor.maContexts) != Context::NoMatch)
                rPanelDescriptors.push_back(*iPanel);
    }

    return rPanelDescriptors;
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

        rDeckDescriptor.msTitle = ::comphelper::getString(aDeckNode.getNodeValue("Title"));
        rDeckDescriptor.msId = ::comphelper::getString(aDeckNode.getNodeValue("Id"));
        rDeckDescriptor.msIconURL = ::comphelper::getString(aDeckNode.getNodeValue("IconURL"));
        rDeckDescriptor.msHighContrastIconURL = ::comphelper::getString(aDeckNode.getNodeValue("HighContrastIconURL"));
        rDeckDescriptor.msHelpURL = ::comphelper::getString(aDeckNode.getNodeValue("HelpURL"));
        rDeckDescriptor.msHelpText = rDeckDescriptor.msTitle;
        ReadContextList(aDeckNode.openNode("ContextList"), rDeckDescriptor.maContexts);
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
        rPanelDescriptor.msLayout = ::comphelper::getString(
            aPanelNode.getNodeValue("Layout"));
        rPanelDescriptor.msImplementationURL = ::comphelper::getString(
            aPanelNode.getNodeValue("ImplementationURL"));
        ReadContextList(aPanelNode.openNode("ContextList"), rPanelDescriptor.maContexts);
    }

    // When there where invalid nodes then we have to adapt the size
    // of the deck vector.
    if (nWriteIndex<nCount)
        maPanels.resize(nWriteIndex);
}




void ResourceManager::ReadContextList (
    const ::utl::OConfigurationNode& rNode,
    ::std::vector<Context>& rContextContainer) const
{
    const Sequence<OUString> aChildNodeNames (rNode.getNodeNames());
    const sal_Int32 nCount (aChildNodeNames.getLength());
    rContextContainer.resize(nCount);
    for (sal_Int32 nIndex(0); nIndex<nCount; ++nIndex)
    {
        const ::utl::OConfigurationNode aChildNode (rNode.openNode(aChildNodeNames[nIndex]));
        Context& rContext (rContextContainer[nIndex]);

        rContext.msApplication = ::comphelper::getString(aChildNode.getNodeValue("Application"));
        rContext.msContext = ::comphelper::getString(aChildNode.getNodeValue("ApplicationContext"));
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
    sal_Int32 nDeckWriteIndex (maDecks.size());
    sal_Int32 nPanelWriteIndex (maPanels.size());
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
        rDeckDescriptor.maContexts.resize(1);
        rDeckDescriptor.maContexts.front() = Context(A2S("any"), A2S("any"));

        PanelDescriptor& rPanelDescriptor (maPanels[nPanelWriteIndex++]);
        rPanelDescriptor.msTitle = ::comphelper::getString(aChildNode.getNodeValue("UIName"));
        rPanelDescriptor.mbIsTitleBarOptional = false;
        rPanelDescriptor.msId = rsNodeName;
        rPanelDescriptor.msDeckId = rsNodeName;
        rPanelDescriptor.msHelpURL = ::comphelper::getString(aChildNode.getNodeValue("HelpURL"));
        rPanelDescriptor.maContexts.resize(1);
        rPanelDescriptor.maContexts.front() = Context(A2S("any"), A2S("any"));
        rPanelDescriptor.msLayout = A2S("full");
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
    const cssu::Reference<css::frame::XFrame>& rxFrame) const
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
