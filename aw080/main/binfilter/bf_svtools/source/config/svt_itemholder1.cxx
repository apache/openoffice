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


#include "itemholder1.hxx"

//-----------------------------------------------
// includes

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif

#include <bf_svtools/addxmltostorageoptions.hxx>
#include <cacheoptions.hxx>
#include <bf_svtools/cmdoptions.hxx>
#include <bf_svtools/compatibility.hxx>
#include <bf_svtools/defaultoptions.hxx>
#include <bf_svtools/dynamicmenuoptions.hxx>
#include <bf_svtools/extendedsecurityoptions.hxx>
#include <fltrcfg.hxx>
#include <bf_svtools/fontoptions.hxx>
#include <bf_svtools/historyoptions.hxx>
#include <bf_svtools/inetoptions.hxx>
#include <bf_svtools/internaloptions.hxx>
#include <javaoptions.hxx>
#include <bf_svtools/lingucfg.hxx>
#include <bf_svtools/localisationoptions.hxx>
#include <bf_svtools/menuoptions.hxx>
#include <bf_svtools/moduleoptions.hxx>
#include <bf_svtools/options3d.hxx>
#include <bf_svtools/pathoptions.hxx>
#include <bf_svtools/printwarningoptions.hxx>
//#include <regoptions.hxx>
#include <bf_svtools/optionsdlg.hxx>
#include <bf_svtools/saveopt.hxx>
#include <searchopt.hxx>
#include <bf_svtools/securityoptions.hxx>
#include <bf_svtools/sourceviewconfig.hxx>
#include <bf_svtools/startoptions.hxx>
#include <bf_svtools/viewoptions.hxx>
#include <bf_svtools/workingsetoptions.hxx>
#include <bf_svtools/options.hxx>

//-----------------------------------------------
// namespaces

namespace css = ::com::sun::star;

namespace binfilter
{

//-----------------------------------------------
// declarations

//-----------------------------------------------
ItemHolder1::ItemHolder1()
    : ItemHolderMutexBase()
{
    try
    {
        css::uno::Reference< css::lang::XMultiServiceFactory > xSMGR = ::comphelper::getProcessServiceFactory();
        css::uno::Reference< css::lang::XComponent > xCfg(
            xSMGR->createInstance(::rtl::OUString::createFromAscii("com.sun.star.configuration.ConfigurationProvider")),
            css::uno::UNO_QUERY);
        if (xCfg.is())
            xCfg->addEventListener(static_cast< css::lang::XEventListener* >(this));
    }
// #i37892  got errorhandling from   ConfigManager::GetConfigurationProvider()
#ifdef DBG_UTIL
    catch(css::uno::Exception& rEx)
	{
        static sal_Bool bMessage = sal_True;
        if(bMessage)
        {
            bMessage = sal_False;
            ::rtl::OString sMsg("CreateInstance with arguments exception: ");
            sMsg += ::rtl::OString(rEx.Message.getStr(),
                        rEx.Message.getLength(),
                        RTL_TEXTENCODING_ASCII_US);
            OSL_ENSURE(sal_False, sMsg.getStr());
        }
	}
#else
	catch(css::uno::Exception&){}
#endif
}

//-----------------------------------------------
ItemHolder1::~ItemHolder1()
{
    impl_releaseAllItems();
}

//-----------------------------------------------
void ItemHolder1::holdConfigItem(EItem eItem)
{
    static ItemHolder1* pHolder = new ItemHolder1();
    pHolder->impl_addItem(eItem);
}

//-----------------------------------------------
void SAL_CALL ItemHolder1::disposing(const css::lang::EventObject&)
    throw(css::uno::RuntimeException)
{
    css::uno::Reference< css::uno::XInterface > xSelfHold(static_cast< css::lang::XEventListener* >(this), css::uno::UNO_QUERY);
    impl_releaseAllItems();
}

//-----------------------------------------------
void ItemHolder1::impl_addItem(EItem eItem)
{
    ::osl::ResettableMutexGuard aLock(m_aLock);

    TItems::const_iterator pIt;
    for (  pIt  = m_lItems.begin();
           pIt != m_lItems.end()  ;
         ++pIt                    )
    {
        const TItemInfo& rInfo = *pIt;
        if (rInfo.eItem == eItem)
            return;
    }

    TItemInfo aNewItem;
    aNewItem.eItem = eItem;
    impl_newItem(aNewItem);
    if (aNewItem.pItem)
        m_lItems.push_back(aNewItem);
}

//-----------------------------------------------
void ItemHolder1::impl_releaseAllItems()
{
    ::osl::ResettableMutexGuard aLock(m_aLock);

    TItems::iterator pIt;
    for (  pIt  = m_lItems.begin();
           pIt != m_lItems.end()  ;
         ++pIt                    )
    {
        TItemInfo& rInfo = *pIt;
        impl_deleteItem(rInfo);
    }
    m_lItems.clear();
}

//-----------------------------------------------
void ItemHolder1::impl_newItem(TItemInfo& rItem)
{
    switch(rItem.eItem)
    {
        case E_ADDXMLTOSTORAGEOPTIONS :
            rItem.pItem = new SvtAddXMLToStorageOptions();
            break;

        case E_CMDOPTIONS :
            rItem.pItem = new SvtCommandOptions();
            break;

        case E_COMPATIBILITY :
            rItem.pItem = new SvtCompatibilityOptions();
            break;

        case E_DEFAULTOPTIONS :
            rItem.pItem = new SvtDefaultOptions();
            break;

        case E_DYNAMICMENUOPTIONS :
            rItem.pItem = new SvtDynamicMenuOptions();
            break;

        case E_EVENTCFG :
            break;

        case E_EXTENDEDSECURITYOPTIONS :
            rItem.pItem = new SvtExtendedSecurityOptions();
            break;

        case E_FLTRCFG :
            break;

        case E_FONTOPTIONS :
            rItem.pItem = new SvtFontOptions();
            break;

        case E_HISTORYOPTIONS :
            rItem.pItem = new SvtHistoryOptions();
            break;

        case E_INETOPTIONS :
            rItem.pItem = new SvtInetOptions();
            break;

        case E_INTERNALOPTIONS :
            rItem.pItem = new SvtInternalOptions();
            break;

        case E_JAVAOPTIONS :
            break;

        case E_LINGUCFG :
            rItem.pItem = new SvtLinguConfig();
            break;

        case E_LOCALISATIONOPTIONS :
            rItem.pItem = new SvtLocalisationOptions();
            break;

        case E_MENUOPTIONS :
            rItem.pItem = new SvtMenuOptions();
            break;

        case E_MODULEOPTIONS :
            rItem.pItem = new SvtModuleOptions();
            break;

        case E_OPTIONSDLGOPTIONS :
            rItem.pItem = new SvtOptionsDialogOptions();
            break;

        case E_OPTIONS3D :
            rItem.pItem = new SvtOptions3D();
            break;

        case E_PATHOPTIONS :
            rItem.pItem = new SvtPathOptions();
            break;

        case E_PRINTWARNINGOPTIONS :
            rItem.pItem = new SvtPrintWarningOptions();
            break;

        case E_SAVEOPTIONS :
            rItem.pItem = new SvtSaveOptions();
            break;

        case E_SEARCHOPT :
// no ref count            rItem.pItem = new SvtSearchOptions();
            break;

        case E_SECURITYOPTIONS :
            rItem.pItem = new SvtSecurityOptions();
            break;

        case E_SOURCEVIEWCONFIG :
            rItem.pItem = new SourceViewConfig();
            break;

        case E_STARTOPTIONS :
            rItem.pItem = new SvtStartOptions();
            break;

        case E_VIEWOPTIONS_DIALOG :
            rItem.pItem = new SvtViewOptions(E_DIALOG, ::rtl::OUString());
            break;

        case E_VIEWOPTIONS_TABDIALOG :
            rItem.pItem = new SvtViewOptions(E_TABDIALOG, ::rtl::OUString());
            break;

        case E_VIEWOPTIONS_TABPAGE :
            rItem.pItem = new SvtViewOptions(E_TABPAGE, ::rtl::OUString());
            break;

        case E_VIEWOPTIONS_WINDOW :
            rItem.pItem = new SvtViewOptions(E_WINDOW, ::rtl::OUString());
            break;

        case E_WORKINGSETOPTIONS :
            rItem.pItem = new SvtWorkingSetOptions();
            break;

        case E_XMLACCELCFG :
            // ??? TODO
            break;
        default:
            OSL_ASSERT( "unknown item type" );
            break;
    }
}

//-----------------------------------------------
void ItemHolder1::impl_deleteItem(TItemInfo& rItem)
{
    if (rItem.pItem)
    {
        delete rItem.pItem;
        rItem.pItem = 0;
    }
}

}
