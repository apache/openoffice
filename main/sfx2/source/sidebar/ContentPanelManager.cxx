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

#include "ContentPanelManager.hxx"
#include <unotools/confignode.hxx>
#include <comphelper/componentcontext.hxx>


#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

using ::rtl::OUString;
using namespace css;
using namespace cssu;

namespace sfx2 {


class ContentPanelManager::Deleter
{
public:
    void operator() (ContentPanelManager* pObject)
    {
        delete pObject;
    }
};


SharedContentPanelManager ContentPanelManager::Instance (void)
{
    static SharedContentPanelManager mpInstance;

    if ( ! mpInstance)
    {
        mpInstance.reset(new ContentPanelManager(), ContentPanelManager::Deleter());
    }
    
    return mpInstance;
}




ContentPanelManager::ContentPanelManager (void)
    : maDecks(),
      maPanels()
{
    ReadDeckList();
    ReadPanelList();
}




ContentPanelManager::~ContentPanelManager (void)
{
}




void ContentPanelManager::CallForBestMatchingDeck (
    const ::boost::function<void(const DeckDescriptor&)>& rFunctor,
    const Context& rContext) const
{
    sal_Int32 nBestMatch (Context::NoMatch);
    const DeckContainer::const_iterator iEnd (maDecks.end());
    DeckContainer::const_iterator iBestDeck (iEnd);
    
    for (DeckContainer::const_iterator iDeck(maDecks.begin());
         iDeck!=iEnd;
         ++iDeck)
    {
        const sal_Int32 nMatch (rContext.EvaluateMatch((*iDeck)->maContexts));
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
        rFunctor(**iBestDeck);
}




void ContentPanelManager::CallForMatchingPanels (
    const ::boost::function<void(const ContentPanelDescriptor&)>& rFunctor,
    const Context& rContext,
    const ::rtl::OUString& rsDeckId) const
{
    for (PanelContainer::const_iterator
             iPanel(maPanels.begin()),
             iEnd(maPanels.end());
         iPanel!=iEnd;
         ++iPanel)
    {
        const ContentPanelDescriptor& rPanelDescriptor (**iPanel);
        if (rPanelDescriptor.msDeckId.equals(rsDeckId))
            if (rContext.EvaluateMatch(rPanelDescriptor.maContexts) != Context::NoMatch)
                rFunctor(rPanelDescriptor);
    }
}




void ContentPanelManager::ReadDeckList (void)
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
    maDecks.reserve(nCount);
    for (sal_Int32 nIndex(0); nIndex<nCount; ++nIndex)
    {
        const ::utl::OConfigurationNode aDeckNode (aDeckRootNode.openNode(aDeckNodeNames[nIndex]));
        if ( ! aDeckNode.isValid())
            continue;

        ::boost::shared_ptr<DeckDescriptor> pDeckDescriptor (new DeckDescriptor());

        pDeckDescriptor->msTitle = ::comphelper::getString(aDeckNode.getNodeValue("Title"));
        pDeckDescriptor->msId = ::comphelper::getString(aDeckNode.getNodeValue("Id"));
        pDeckDescriptor->msIconURL = ::comphelper::getString(aDeckNode.getNodeValue("IconURL"));
        pDeckDescriptor->msHelpURL = ::comphelper::getString(aDeckNode.getNodeValue("HelpURL"));
        ReadContextList(aDeckNode.openNode("ContextList"), pDeckDescriptor->maContexts);

        maDecks.push_back(pDeckDescriptor);
    }
}




void ContentPanelManager::ReadPanelList (void)
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
    maPanels.reserve(nCount);
    for (sal_Int32 nIndex(0); nIndex<nCount; ++nIndex)
    {
        const ::utl::OConfigurationNode aPanelNode (aPanelRootNode.openNode(aPanelNodeNames[nIndex]));
        if ( ! aPanelNode.isValid())
            continue;

        ::boost::shared_ptr<ContentPanelDescriptor> pPanelDescriptor (new ContentPanelDescriptor());

        pPanelDescriptor->msTitle = ::comphelper::getString(
            aPanelNode.getNodeValue("Title"));
        pPanelDescriptor->mbIsTitleBarOptional = ::comphelper::getBOOL(
            aPanelNode.getNodeValue("TitleBarIsOptional"));
        pPanelDescriptor->msId = ::comphelper::getString(
            aPanelNode.getNodeValue("Id"));
        pPanelDescriptor->msDeckId = ::comphelper::getString(
            aPanelNode.getNodeValue("DeckId"));
        pPanelDescriptor->msHelpURL = ::comphelper::getString(
            aPanelNode.getNodeValue("HelpURL"));
        pPanelDescriptor->msLayout = ::comphelper::getString(
            aPanelNode.getNodeValue("Layout"));
        pPanelDescriptor->msImplementationURL = ::comphelper::getString(
            aPanelNode.getNodeValue("ImplementationURL"));
        ReadContextList(aPanelNode.openNode("ContextList"), pPanelDescriptor->maContexts);

        maPanels.push_back(pPanelDescriptor);
    }
}




void ContentPanelManager::ReadContextList (
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

   

} // end of namespace sfx2
