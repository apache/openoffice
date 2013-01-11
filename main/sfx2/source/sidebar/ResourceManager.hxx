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

#ifndef SFX_SIDEBAR_RESOURCE_MANAGER_HXX
#define SFX_SIDEBAR_RESOURCE_MANAGER_HXX

#include "DeckDescriptor.hxx"
#include "PanelDescriptor.hxx"

#include <set>
#include <boost/shared_ptr.hpp>


namespace css = ::com::sun::star;
namespace cssu = ::com::sun::star::uno;

namespace sfx2 { namespace sidebar {

/** Read the content of the Sidebar.xcu file and provide access
    methods so that the sidebar can easily decide which content panels
    to display for a certain context.
*/
class ResourceManager
{
public:
    static ResourceManager& Instance (void);

    const DeckDescriptor* GetBestMatchingDeck (
        const Context& rContext,
        const cssu::Reference<css::frame::XFrame>& rxFrame);

    typedef ::std::vector<DeckDescriptor> DeckContainer;
    typedef ::std::vector<PanelDescriptor> PanelContainer;

    const DeckContainer& GetMatchingDecks (
        DeckContainer& rDeckDescriptors,
        const Context& rContext,
        const cssu::Reference<css::frame::XFrame>& rxFrame);

    const PanelContainer& GetMatchingPanels (
        PanelContainer& rPanelDescriptors,
        const Context& rContext,
        const ::rtl::OUString& rsDeckId,
        const cssu::Reference<css::frame::XFrame>& rxFrame);

private:
    ResourceManager (void);
    ~ResourceManager (void);
    class Deleter;
    friend class Deleter;

    DeckContainer maDecks;
    PanelContainer maPanels;
    mutable ::std::set<rtl::OUString> maProcessedApplications;

    void ReadDeckList (void);
    void ReadPanelList (void);
    void ReadContextList (
        const ::utl::OConfigurationNode& rNode,
        ::std::vector<Context>& rContextContainer) const;
    void ReadLegacyAddons (
        const cssu::Reference<css::frame::XFrame>& rxFrame);
    ::rtl::OUString GetModuleName (
        const cssu::Reference<css::frame::XFrame>& rxFrame) const;
    ::utl::OConfigurationTreeRoot GetLegacyAddonRootNode (
        const ::rtl::OUString& rsModuleName) const;
    void GetToolPanelNodeNames (
        ::std::vector<rtl::OUString>& rMatchingNames,
        const ::utl::OConfigurationTreeRoot aRoot) const;
};


} } // end of namespace sfx2::sidebar

#endif
