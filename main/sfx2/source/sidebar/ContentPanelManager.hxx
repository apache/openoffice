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

#ifndef SFX_CONTENT_PANEL_SIDEBAR_HXX
#define SFX_CONTENT_PANEL_SIDEBAR_HXX

#include "DeckDescriptor.hxx"
#include "ContentPanelDescriptor.hxx"

#include <boost/shared_ptr.hpp>



namespace css = ::com::sun::star;
namespace cssu = ::com::sun::star::uno;

namespace sfx2 {

class ContentPanelManager;
typedef ::boost::shared_ptr<ContentPanelManager> SharedContentPanelManager;


/** Read the content of the Sidebar.xcu file and provide access
    methods so that the sidebar can easily decide which content panels
    to display for a certain context.
*/
class ContentPanelManager
{
public:
    static SharedContentPanelManager Instance (void);

    void CallForBestMatchingDeck (
        const ::boost::function<void(const DeckDescriptor&)>& rFunctor,
        const Context& rContext) const;

    void CallForMatchingPanels (
        const ::boost::function<void(const ContentPanelDescriptor&)>& rFunctor,
        const Context& rContext,
        const ::rtl::OUString& rsDeckId) const;

private:
    ContentPanelManager (void);
    ~ContentPanelManager (void);
    class Deleter;
    friend class Deleter;

    typedef ::std::vector<boost::shared_ptr<DeckDescriptor> > DeckContainer;
    typedef ::std::vector<boost::shared_ptr<ContentPanelDescriptor> > PanelContainer;
    DeckContainer maDecks;
    PanelContainer maPanels;

    void ReadDeckList (void);
    void ReadPanelList (void);
    void ReadContextList (
        const ::utl::OConfigurationNode& rNode,
        ::std::vector<Context>& rContextContainer) const;
};


} // end of namespace sfx2

#endif
