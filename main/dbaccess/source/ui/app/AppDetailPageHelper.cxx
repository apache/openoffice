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
#include "precompiled_dbui.hxx"
#ifndef DBAUI_APPDETAILPAGEHELPER_HXX
#include "AppDetailPageHelper.hxx"
#endif
#ifndef _DBAUI_MODULE_DBU_HXX_
#include "moduledbu.hxx"
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef TOOLS_DIAGNOSE_EX_H
#include <tools/diagnose_ex.h>
#endif
#ifndef _CONNECTIVITY_DBTOOLS_HXX_
#include <connectivity/dbtools.hxx>
#endif
#ifndef _DBAUI_TABLETREE_HXX_
#include "tabletree.hxx"
#endif
#ifndef DBAUI_TOOLS_HXX
#include "UITools.hxx"
#endif
#ifndef DBAUI_DBTREELISTBOX_HXX
#include "dbtreelistbox.hxx"
#endif
#ifndef _COM_SUN_STAR_FRAME_XLAYOUTMANAGER_HPP_
#include <com/sun/star/frame/XLayoutManager.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XTABCONTROLLER_HPP_
#include <com/sun/star/awt/XTabController.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XCONNECTION_HPP_
#include <com/sun/star/sdbc/XConnection.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XTABLESSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XTablesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XVIEWSSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XViewsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_FRAMESEARCHFLAG_HPP_
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XCOMPONENTLOADER_HPP_
#include <com/sun/star/frame/XComponentLoader.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAMES_HPP_
#include <com/sun/star/frame/XFrames.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XFRAMESSUPPLIER_HPP_
#include <com/sun/star/frame/XFramesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/helper/vclunohelper.hxx>
#endif
#ifndef _COM_SUN_STAR_UCB_XCOMMANDENVIRONMENT_HPP_
#include <com/sun/star/ucb/XCommandEnvironment.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCOMMANDPROCESSOR_HPP_
#include <com/sun/star/ucb/XCommandProcessor.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_COMMAND_HPP_
#include <com/sun/star/ucb/Command.hpp>
#endif
#ifndef _COM_SUN_STAR_FORM_XLOADABLE_HPP_
#include <com/sun/star/form/XLoadable.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCHILD_HPP_
#include <com/sun/star/container/XChild.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCONTAINER_HPP_
#include <com/sun/star/container/XContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_COMMANDTYPE_HPP_
#include <com/sun/star/sdb/CommandType.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XCLOSEABLE_HPP_
#include <com/sun/star/util/XCloseable.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_APPLICATION_XDATABASEDOCUMENTUI_HPP_
#include <com/sun/star/sdb/application/XDatabaseDocumentUI.hpp>
#endif
#include <com/sun/star/sdb/application/DatabaseObject.hpp>
#include <com/sun/star/sdb/application/DatabaseObjectContainer.hpp>
#ifndef DBAUI_APPVIEW_HXX
#include "AppView.hxx"
#endif
#ifndef _DBA_DBACCESS_HELPID_HRC_
#include "dbaccess_helpid.hrc"
#endif
#ifndef _DBU_APP_HRC_
#include "dbu_app.hrc"
#endif
#ifndef _DBACCESS_UI_CALLBACKS_HXX_
#include "callbacks.hxx"
#endif
#ifndef DBAUI_ICONTROLLER_HXX
#include "IController.hxx"
#endif
#ifndef DBACCESS_SHARED_DBUSTRINGS_HRC
#include "dbustrings.hrc"
#endif
#ifndef _DBACCESS_SLOTID_HRC_
#include "dbaccess_slotid.hrc"
#endif
#ifndef DBACCESS_DATABASE_OBJECT_VIEW_HXX
#include "databaseobjectview.hxx"
#endif
#ifndef DBACCESS_IMAGEPROVIDER_HXX
#include "imageprovider.hxx"
#endif
#ifndef _SV_WAITOBJ_HXX
#include <vcl/waitobj.hxx>
#endif
#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#include "IApplicationController.hxx"

#include <com/sun/star/document/XDocumentProperties.hpp>


using namespace ::dbaui;
using namespace ::com::sun::star::container;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::ucb;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::form;
using namespace ::com::sun::star::sdb;
using namespace ::com::sun::star::sdb::application;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::sdbcx;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star;
using ::com::sun::star::awt::XTabController;

namespace dbaui
{
    namespace DatabaseObject = ::com::sun::star::sdb::application::DatabaseObject;
    namespace DatabaseObjectContainer = ::com::sun::star::sdb::application::DatabaseObjectContainer;
}

namespace
{
    SvLBoxEntry* lcl_findEntry_impl(DBTreeListBox& rTree,const ::rtl::OUString& _rName,SvLBoxEntry* _pFirst)
    {
        SvLBoxEntry* pReturn = NULL;
        sal_Int32 nIndex = 0;
        String sName( _rName.getToken(0,'/',nIndex) );

        SvLBoxEntry* pEntry = _pFirst;
        while( pEntry )
        {
            if ( rTree.GetEntryText(pEntry) == sName )
            {
                if ( nIndex != -1 )
                {
                    sName = _rName.getToken(0,'/',nIndex);
                    pEntry = rTree.FirstChild(pEntry);
                }
                else
                {
                    pReturn = pEntry;
                    break;
                }
            }
            else
                pEntry = rTree.NextSibling(pEntry);
        }
        return pReturn;
    }
    SvLBoxEntry* lcl_findEntry(DBTreeListBox& rTree,const ::rtl::OUString& _rName,SvLBoxEntry* _pFirst)
    {
        sal_Int32 nIndex = 0;
        ::rtl::OUString sErase = _rName.getToken(0,'/',nIndex); // we don't want to have the "private:forms" part
        return (nIndex != -1 ? lcl_findEntry_impl(rTree,_rName.copy(sErase.getLength() + 1),_pFirst) : NULL);
    }
    //==================================================================
    // class OPreviewWindow
    //==================================================================
    class OTablePreviewWindow : public Window
    {
        DECL_LINK(OnDisableInput, void*);
        void ImplInitSettings( sal_Bool bFont, sal_Bool bForeground, sal_Bool bBackground );
    protected:
        virtual void DataChanged(const DataChangedEvent& rDCEvt);
    public:
        OTablePreviewWindow( Window* pParent, WinBits nStyle = 0 );
        virtual long Notify( NotifyEvent& rNEvt );
    };
    // -----------------------------------------------------------------------------
    OTablePreviewWindow::OTablePreviewWindow(Window* pParent, WinBits nStyle) : Window( pParent, nStyle)
    {
        ImplInitSettings( sal_True, sal_True, sal_True );
    }
    // -----------------------------------------------------------------------------
    long OTablePreviewWindow::Notify( NotifyEvent& rNEvt )
    {
        long nRet = Window::Notify( rNEvt );
        if ( rNEvt.GetType() == EVENT_INPUTENABLE && IsInputEnabled() )
            PostUserEvent( LINK( this, OTablePreviewWindow, OnDisableInput) );
        return nRet;
    }
    // -----------------------------------------------------------------------------
    IMPL_LINK(OTablePreviewWindow, OnDisableInput, void*, EMPTYARG)
    {
        EnableInput(sal_False);
        return 0L;
    }
    // -----------------------------------------------------------------------------
    void OTablePreviewWindow::DataChanged( const DataChangedEvent& rDCEvt )
    {
        Window::DataChanged( rDCEvt );

        if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
            (rDCEvt.GetFlags() & SETTINGS_STYLE) )
        {
            ImplInitSettings( sal_True, sal_True, sal_True );
            Invalidate();
        }
    }
    // -----------------------------------------------------------------------------
    void OTablePreviewWindow::ImplInitSettings( sal_Bool bFont, sal_Bool bForeground, sal_Bool bBackground )
    {
        const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();
        if( bFont )
        {
            Font aFont;
            aFont = rStyleSettings.GetFieldFont();
            aFont.SetColor( rStyleSettings.GetWindowTextColor() );
            SetPointFont( aFont );
        }

        if( bForeground || bFont )
        {
            SetTextColor( rStyleSettings.GetFieldTextColor() );
            SetTextFillColor();
        } // if( bForeground || bFont )

        if( bBackground )
            SetBackground( rStyleSettings.GetFieldColor() );
    }

}
//==================================================================
// class OAppDetailPageHelper
DBG_NAME(OAppDetailPageHelper)
//==================================================================
OAppDetailPageHelper::OAppDetailPageHelper(Window* _pParent,OAppBorderWindow& _rBorderWin,PreviewMode _ePreviewMode) : Window(_pParent,WB_DIALOGCONTROL)
    ,m_rBorderWin(_rBorderWin)
    ,m_aFL(this,WB_VERT)
    ,m_aTBPreview(this,WB_TABSTOP )
    ,m_aBorder(this,WB_BORDER | WB_READONLY)
    ,m_aPreview(&m_aBorder)
    ,m_aDocumentInfo(&m_aBorder,WB_LEFT | WB_VSCROLL | WB_READONLY )
    ,m_pTablePreview(NULL)
    ,m_ePreviewMode(_ePreviewMode)
{
    DBG_CTOR(OAppDetailPageHelper,NULL);

    m_aBorder.SetBorderStyle(WINDOW_BORDER_MONO);

    m_aMenu.reset(new PopupMenu( ModuleRes( RID_MENU_APP_PREVIEW ) ));

    m_aTBPreview.SetOutStyle(TOOLBOX_STYLE_FLAT);
    m_aTBPreview.InsertItem(SID_DB_APP_DISABLE_PREVIEW,m_aMenu->GetItemText(SID_DB_APP_DISABLE_PREVIEW),TIB_LEFT|TIB_DROPDOWN|TIB_AUTOSIZE|TIB_RADIOCHECK);
    m_aTBPreview.SetHelpId(HID_APP_VIEW_PREVIEW_CB);
    m_aTBPreview.SetDropdownClickHdl( LINK( this, OAppDetailPageHelper, OnDropdownClickHdl ) );
    m_aTBPreview.EnableMenuStrings();
    m_aTBPreview.Enable(sal_True);

    m_aBorder.SetUniqueId(UID_APP_VIEW_PREVIEW_1);

    m_aPreview.SetHelpId(HID_APP_VIEW_PREVIEW_1);

    m_pTablePreview = new OTablePreviewWindow(&m_aBorder,WB_READONLY | WB_DIALOGCONTROL );
    m_pTablePreview->SetHelpId(HID_APP_VIEW_PREVIEW_2);

    m_aDocumentInfo.SetHelpId(HID_APP_VIEW_PREVIEW_3);

    m_xWindow = VCLUnoHelper::GetInterface( m_pTablePreview );

    SetUniqueId(UID_APP_DETAILPAGE_HELPER);
    for (int i=0; i < E_ELEMENT_TYPE_COUNT; ++i)
        m_pLists[i] = NULL;
    ImplInitSettings();
}
// -----------------------------------------------------------------------------
OAppDetailPageHelper::~OAppDetailPageHelper()
{
    try
    {
        Reference< ::util::XCloseable> xCloseable(m_xFrame,UNO_QUERY);
        if ( xCloseable.is() )
            xCloseable->close(sal_True);
    }
    catch(Exception)
    {
        OSL_ENSURE(0,"Exception thrown while disposing preview frame!");
    }

    for (int i=0; i < E_ELEMENT_TYPE_COUNT; ++i)
    {
        if ( m_pLists[i] )
        {
            m_pLists[i]->clearCurrentSelection();
            m_pLists[i]->Hide();
            ::std::auto_ptr<DBTreeListBox> aTemp(m_pLists[i]);
            m_pLists[i]->clearCurrentSelection();   // why a second time?
            m_pLists[i] = NULL;
        }

    }

    DBG_DTOR(OAppDetailPageHelper,NULL);
}
// -----------------------------------------------------------------------------
int OAppDetailPageHelper::getVisibleControlIndex() const
{
    int i = 0;
    for (; i < E_ELEMENT_TYPE_COUNT ; ++i)
    {
        if ( m_pLists[i] && m_pLists[i]->IsVisible() )
            break;
    }
    return i;
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::selectAll()
{
    int nPos = getVisibleControlIndex();
    if ( nPos < E_ELEMENT_TYPE_COUNT )
    {
        m_pLists[nPos]->SelectAll(sal_True);
    }
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::sort(int _nPos,SvSortMode _eSortMode )
{
    OSL_ENSURE(m_pLists[_nPos],"List can not be NULL! ->GPF");
    SvLBoxTreeList* pModel = m_pLists[_nPos]->GetModel();
    SvSortMode eOldSortMode = pModel->GetSortMode();
    pModel->SetSortMode(_eSortMode);
    if ( eOldSortMode != _eSortMode )
        pModel->Resort();
}
// -----------------------------------------------------------------------------
sal_Bool OAppDetailPageHelper::isSortUp() const
{
    SvSortMode eSortMode = SortNone;
    int nPos = getVisibleControlIndex();
    if ( nPos < E_ELEMENT_TYPE_COUNT )
    {
        SvLBoxTreeList* pModel = m_pLists[nPos]->GetModel();
        eSortMode = pModel->GetSortMode();
    }
    return eSortMode == SortAscending;
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::sortDown()
{
    int nPos = getVisibleControlIndex();
    if ( nPos < E_ELEMENT_TYPE_COUNT )
        sort(nPos,SortDescending);
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::sortUp()
{
    int nPos = getVisibleControlIndex();
    if ( nPos < E_ELEMENT_TYPE_COUNT )
        sort(nPos,SortAscending);
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::getSelectionElementNames( ::std::vector< ::rtl::OUString>& _rNames ) const
{
    int nPos = getVisibleControlIndex();
    if ( nPos < E_ELEMENT_TYPE_COUNT )
    {
        DBTreeListBox& rTree = *m_pLists[nPos];
        sal_Int32 nCount = rTree.GetEntryCount();
        _rNames.reserve(nCount);
        SvLBoxEntry* pEntry = rTree.FirstSelected();
        ElementType eType = getElementType();
        while( pEntry )
        {
            if ( eType == E_TABLE )
            {
                if( rTree.GetChildCount(pEntry) == 0 )
                    _rNames.push_back( getQualifiedName( pEntry ) );
            }
            else
            {
                ::rtl::OUString sName = rTree.GetEntryText(pEntry);
                SvLBoxEntry* pParent = rTree.GetParent(pEntry);
                while(pParent)
                {
                    sName = rTree.GetEntryText(pParent) + ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/")) + sName;
                    pParent = rTree.GetParent(pParent);
                }
                _rNames.push_back(sName);
            }
            pEntry = rTree.NextSelected(pEntry);
        }
    }
}

// -----------------------------------------------------------------------------
void OAppDetailPageHelper::describeCurrentSelectionForControl( const Control& _rControl, Sequence< NamedDatabaseObject >& _out_rSelectedObjects )
{
    enum ElementType eType = E_TABLE;
    for ( size_t i=0; i < E_ELEMENT_TYPE_COUNT; eType = static_cast< ElementType >( ++i ) )
    {
        if ( m_pLists[eType] == &_rControl )
        {
            describeCurrentSelectionForType( eType, _out_rSelectedObjects );
            return;
        }
    }
    OSL_ENSURE( false, "OAppDetailPageHelper::describeCurrentSelectionForControl: invalid control!" );
}

// -----------------------------------------------------------------------------
void OAppDetailPageHelper::describeCurrentSelectionForType( const ElementType _eType, Sequence< NamedDatabaseObject >& _out_rSelectedObjects )
{
    OSL_ENSURE( _eType < E_ELEMENT_TYPE_COUNT, "OAppDetailPageHelper::describeCurrentSelectionForType: invalid type!" );
    DBTreeListBox* pList = ( _eType < E_ELEMENT_TYPE_COUNT ) ? m_pLists[ _eType ] : NULL;
    OSL_ENSURE( pList, "OAppDetailPageHelper::describeCurrentSelectionForType: "
                       "You really should ensure this type has already been viewed before!" );
    if ( !pList )
        return;

    ::std::vector< NamedDatabaseObject > aSelected;

    SvLBoxEntry* pEntry = pList->FirstSelected();
    while( pEntry )
    {
        NamedDatabaseObject aObject;
        switch ( _eType )
        {
        case E_TABLE:
        {
            OTableTreeListBox& rTableTree = dynamic_cast< OTableTreeListBox& >( *pList );
            aObject = rTableTree.describeObject( pEntry );
        }
        break;
        case E_QUERY:
            aObject.Type = DatabaseObject::QUERY;
            aObject.Name = pList->GetEntryText( pEntry );
            break;

        case E_FORM:
        case E_REPORT:
        {
            ::rtl::OUString sName = pList->GetEntryText(pEntry);
            SvLBoxEntry* pParent = pList->GetParent(pEntry);
            while ( pParent )
            {
                ::rtl::OUStringBuffer buffer;
                buffer.append( pList->GetEntryText( pParent ) );
                buffer.append( sal_Unicode( '/' ) );
                buffer.append( sName );
                sName = buffer.makeStringAndClear();

                pParent = pList->GetParent( pParent );
            }

            if ( isLeaf( pEntry ) )
                aObject.Type = ( _eType == E_FORM ) ? DatabaseObject::FORM : DatabaseObject::REPORT;
            else
                aObject.Type = ( _eType == E_FORM ) ? DatabaseObjectContainer::FORMS_FOLDER : DatabaseObjectContainer::REPORTS_FOLDER;
            aObject.Name = sName;
        }
        break;
        default:
            OSL_ENSURE( false, "OAppDetailPageHelper::describeCurrentSelectionForType: unexpected type!" );
            break;
        }

        if ( aObject.Name.getLength() )
        {
            aSelected.push_back( aObject );
        }

        pEntry = pList->NextSelected(pEntry);
    }

    _out_rSelectedObjects.realloc( aSelected.size() );
    ::std::copy( aSelected.begin(), aSelected.end(), _out_rSelectedObjects.getArray() );
}

// -----------------------------------------------------------------------------
void OAppDetailPageHelper::selectElements(const Sequence< ::rtl::OUString>& _aNames)
{
    int nPos = getVisibleControlIndex();
    if ( nPos < E_ELEMENT_TYPE_COUNT )
    {
        DBTreeListBox& rTree = *m_pLists[nPos];
        rTree.SelectAll(sal_False);
        const ::rtl::OUString* pIter = _aNames.getConstArray();
        const ::rtl::OUString* pEnd  = pIter + _aNames.getLength();
        for(;pIter != pEnd;++pIter)
        {
            SvLBoxEntry* pEntry = rTree.GetEntryPosByName(*pIter);
            if ( pEntry )
                rTree.Select(pEntry);
        }
    }
}
// -----------------------------------------------------------------------------
::rtl::OUString OAppDetailPageHelper::getQualifiedName( SvLBoxEntry* _pEntry ) const
{
    int nPos = getVisibleControlIndex();
    ::rtl::OUString sComposedName;

    if ( nPos >= E_ELEMENT_TYPE_COUNT )
        return sComposedName;

    OSL_ENSURE(m_pLists[nPos],"Tables tree view is NULL! -> GPF");
    DBTreeListBox& rTree = *m_pLists[nPos];

    SvLBoxEntry* pEntry = _pEntry;
    if ( !pEntry )
        pEntry = rTree.FirstSelected();

    if ( !pEntry )
        return sComposedName;

    if ( getElementType() == E_TABLE )
    {
        const OTableTreeListBox& rTreeView = dynamic_cast< const OTableTreeListBox& >( *m_pLists[nPos] );
        sComposedName = rTreeView.getQualifiedTableName( pEntry );
    }
    else
    {
        sComposedName = rTree.GetEntryText(pEntry);
        SvLBoxEntry* pParent = rTree.GetParent(pEntry);
        while(pParent)
        {
            sComposedName = rTree.GetEntryText(pParent) + ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("/")) + sComposedName;
            pParent = rTree.GetParent(pParent);
        }
    }

    return sComposedName;
}
// -----------------------------------------------------------------------------
ElementType OAppDetailPageHelper::getElementType() const
{
    int nPos = getVisibleControlIndex();
    return static_cast<ElementType>(nPos);
}
// -----------------------------------------------------------------------------
sal_Int32 OAppDetailPageHelper::getSelectionCount()
{
    sal_Int32 nCount = 0;
    int nPos = getVisibleControlIndex();
    if ( nPos < E_ELEMENT_TYPE_COUNT )
    {
        DBTreeListBox& rTree = *m_pLists[nPos];
        SvLBoxEntry* pEntry = rTree.FirstSelected();
        while( pEntry )
        {
            ++nCount;
            pEntry = rTree.NextSelected(pEntry);
        }
    }
    return nCount;
}
// -----------------------------------------------------------------------------
sal_Int32 OAppDetailPageHelper::getElementCount()
{
    sal_Int32 nCount = 0;
    int nPos = getVisibleControlIndex();
    if ( nPos < E_ELEMENT_TYPE_COUNT )
    {
        nCount = m_pLists[nPos]->GetEntryCount();
    }
    return nCount;
}
// -----------------------------------------------------------------------------
sal_Bool OAppDetailPageHelper::isCutAllowed()
{
    return sal_False;
}
// -----------------------------------------------------------------------------
sal_Bool OAppDetailPageHelper::isCopyAllowed()
{
    //int nPos = getVisibleControlIndex();
    return sal_True;
}
// -----------------------------------------------------------------------------
sal_Bool OAppDetailPageHelper::isPasteAllowed()
{
    //int nPos = getVisibleControlIndex();
    return sal_True;
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::copy()
{
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::cut()
{
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::paste()
{
}
// -----------------------------------------------------------------------------
bool OAppDetailPageHelper::isLeaf(SvLBoxEntry* _pEntry) const
{
    if ( !_pEntry )
        return false;
    sal_Int32 nEntryType = reinterpret_cast< sal_IntPtr >( _pEntry->GetUserData() );
    if  (   ( nEntryType == DatabaseObjectContainer::TABLES )
        ||  ( nEntryType == DatabaseObjectContainer::CATALOG )
        ||  ( nEntryType == DatabaseObjectContainer::SCHEMA )
        ||  ( nEntryType == DatabaseObjectContainer::FORMS_FOLDER )
        ||  ( nEntryType == DatabaseObjectContainer::REPORTS_FOLDER )
        )
        return false;

    return true;
}

// -----------------------------------------------------------------------------
sal_Bool OAppDetailPageHelper::isALeafSelected() const
{
    int nPos = getVisibleControlIndex();
    sal_Bool bLeafSelected = sal_False;
    if ( nPos < E_ELEMENT_TYPE_COUNT )
    {
        DBTreeListBox& rTree = *m_pLists[nPos];
        SvLBoxEntry* pEntry = rTree.FirstSelected( );
        while( !bLeafSelected && pEntry )
        {
            bLeafSelected = isLeaf( pEntry );
            pEntry = rTree.NextSelected(pEntry);
        }
    }
    return bLeafSelected;
}
// -----------------------------------------------------------------------------
SvLBoxEntry* OAppDetailPageHelper::getEntry( const Point& _aPosPixel) const
{
    SvLBoxEntry* pReturn = NULL;
    int nPos = getVisibleControlIndex();
    if ( nPos < E_ELEMENT_TYPE_COUNT )
        pReturn = m_pLists[nPos]->GetEntry( _aPosPixel,sal_True );
    return pReturn;
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::createTablesPage(const Reference< XConnection>& _xConnection)
{
    OSL_ENSURE(_xConnection.is(),"Connection is NULL! -> GPF");

    if ( !m_pLists[E_TABLE] )
    {
        OTableTreeListBox* pTreeView = new OTableTreeListBox(this
                                                            ,getBorderWin().getView()->getORB()
                                                            ,WB_HASLINES | WB_SORT | WB_HASBUTTONS | WB_HSCROLL |WB_HASBUTTONSATROOT | WB_TABSTOP
                                                            ,sal_False);
        pTreeView->SetHelpId(HID_APP_TABLE_TREE);
        m_pLists[E_TABLE] = pTreeView;

        ImageProvider aImageProvider( _xConnection );
        createTree( pTreeView,
            aImageProvider.getDefaultImage( DatabaseObject::TABLE, false ),
            aImageProvider.getDefaultImage( DatabaseObject::TABLE, true )
        );

        pTreeView->notifyHiContrastChanged();
        m_aBorder.SetZOrder(pTreeView, WINDOW_ZORDER_BEHIND);
    }
    if ( !m_pLists[E_TABLE]->GetEntryCount() )
    {
        static_cast<OTableTreeListBox*>(m_pLists[E_TABLE])->UpdateTableList(_xConnection);

        SvLBoxEntry* pEntry = m_pLists[E_TABLE]->First();
        if ( pEntry )
            m_pLists[E_TABLE]->Expand(pEntry);
        m_pLists[E_TABLE]->SelectAll(sal_False);
    }

    setDetailPage(m_pLists[E_TABLE]);
}

// -----------------------------------------------------------------------------
void OAppDetailPageHelper::getElementIcons( ElementType _eType, sal_uInt16& _rImageId, sal_uInt16& _rHighContrastImageId )
{
    ImageProvider aImageProvider;
    _rImageId = _rHighContrastImageId = 0;

    sal_Int32 nDatabaseObjectType( 0 );
    switch(_eType )
    {
        case E_FORM:    nDatabaseObjectType = DatabaseObject::FORM; break;
        case E_REPORT:  nDatabaseObjectType = DatabaseObject::REPORT; break;
        case E_QUERY:   nDatabaseObjectType = DatabaseObject::QUERY; break;
        default:
            OSL_ENSURE( sal_False, "OAppDetailPageHelper::GetElementIcons: invalid element type!" );
            return;
    }
    _rImageId = aImageProvider.getDefaultImageResourceID( nDatabaseObjectType, false );
    _rHighContrastImageId = aImageProvider.getDefaultImageResourceID( nDatabaseObjectType, true );
}

// -----------------------------------------------------------------------------
void OAppDetailPageHelper::createPage(ElementType _eType,const Reference< XNameAccess >& _xContainer)
{
    OSL_ENSURE(E_TABLE != _eType,"E_TABLE isn't allowed.");

    sal_uInt16 nImageId = 0, nImageIdH = 0;
    rtl::OString sHelpId;
    ImageProvider aImageProvider;
    Image aFolderImage, aFolderImageHC;
    switch( _eType )
    {
        case E_FORM:
            sHelpId = HID_APP_FORM_TREE;
            aFolderImage = aImageProvider.getFolderImage( DatabaseObject::FORM, false );
            aFolderImageHC = aImageProvider.getFolderImage( DatabaseObject::FORM, true );
            break;
        case E_REPORT:
            sHelpId = HID_APP_REPORT_TREE;
            aFolderImage = aImageProvider.getFolderImage( DatabaseObject::REPORT, false );
            aFolderImageHC = aImageProvider.getFolderImage( DatabaseObject::REPORT, true );
            break;
        case E_QUERY:
            sHelpId = HID_APP_QUERY_TREE;
            aFolderImage = aImageProvider.getFolderImage( DatabaseObject::QUERY, false );
            aFolderImageHC = aImageProvider.getFolderImage( DatabaseObject::QUERY, true );
            break;
        default:
            OSL_ENSURE(0,"Illegal call!");
    }
    getElementIcons( _eType, nImageId, nImageIdH );

    if ( !m_pLists[_eType] )
    {
        m_pLists[_eType] = createSimpleTree( sHelpId, aFolderImage, aFolderImageHC );
    }

    if ( m_pLists[_eType] )
    {
        if ( !m_pLists[_eType]->GetEntryCount() && _xContainer.is() )
        {
            fillNames( _xContainer, _eType, nImageId, nImageIdH, NULL );

            m_pLists[_eType]->SelectAll(sal_False);
        }
        setDetailPage(m_pLists[_eType]);
    }
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::setDetailPage(Window* _pWindow)
{
    OSL_ENSURE(_pWindow,"OAppDetailPageHelper::setDetailPage: Window is NULL!");
    Window* pCurrent = getCurrentView();
    if ( pCurrent )
        pCurrent->Hide();

    showPreview(NULL);
    sal_Bool bHasFocus = sal_False;
    m_aFL.Show();
    {
        bHasFocus = pCurrent->HasChildPathFocus();
    _pWindow->Show();
    }
    m_aTBPreview.Show();
    m_aBorder.Show();
    switchPreview(m_ePreviewMode,sal_True);

    if ( bHasFocus )
        _pWindow->GrabFocus();
    Resize();
}

// -----------------------------------------------------------------------------
namespace
{
    namespace DatabaseObject = ::com::sun::star::sdb::application::DatabaseObject;
    namespace DatabaseObjectContainer = ::com::sun::star::sdb::application::DatabaseObjectContainer;

    static sal_Int32 lcl_getFolderIndicatorForType( const ElementType _eType )
    {
        const sal_Int32 nFolderIndicator =
                ( _eType == E_FORM ) ? DatabaseObjectContainer::FORMS_FOLDER
            :   ( _eType == E_REPORT ) ? DatabaseObjectContainer::REPORTS_FOLDER : -1;
        return nFolderIndicator;
    }
}

// -----------------------------------------------------------------------------
void OAppDetailPageHelper::fillNames( const Reference< XNameAccess >& _xContainer, const ElementType _eType,
                                      const sal_uInt16 _nImageId, const sal_uInt16 _nHighContrastImageId, SvLBoxEntry* _pParent )
{
    OSL_ENSURE(_xContainer.is(),"Data source is NULL! -> GPF");
    OSL_ENSURE( ( _eType >= E_TABLE ) && ( _eType < E_ELEMENT_TYPE_COUNT ), "OAppDetailPageHelper::fillNames: invalid type!" );

    DBTreeListBox* pList = m_pLists[ _eType ];
    OSL_ENSURE( pList, "OAppDetailPageHelper::fillNames: you really should create the list before calling this!" );
    if ( !pList )
        return;

    if ( _xContainer.is() && _xContainer->hasElements() )
    {
        const sal_Int32 nFolderIndicator = lcl_getFolderIndicatorForType( _eType );

        Sequence< ::rtl::OUString> aSeq = _xContainer->getElementNames();
        const ::rtl::OUString* pIter = aSeq.getConstArray();
        const ::rtl::OUString* pEnd  = pIter + aSeq.getLength();
        for(;pIter != pEnd;++pIter)
        {
            SvLBoxEntry* pEntry = NULL;
            Reference<XNameAccess> xSubElements(_xContainer->getByName(*pIter),UNO_QUERY);
            if ( xSubElements.is() )
            {
                pEntry = pList->InsertEntry( *pIter, _pParent, sal_False, LIST_APPEND, reinterpret_cast< void* >( nFolderIndicator ) );
                getBorderWin().getView()->getAppController().containerFound( Reference< XContainer >( xSubElements, UNO_QUERY ) );
                fillNames( xSubElements, _eType, _nImageId, _nHighContrastImageId, pEntry );
            }
            else
            {
                pEntry = pList->InsertEntry( *pIter, _pParent );

                Image aImage = Image( ModuleRes( _nImageId ) );
                pList->SetExpandedEntryBmp( pEntry, aImage, BMP_COLOR_NORMAL );
                pList->SetCollapsedEntryBmp( pEntry, aImage, BMP_COLOR_NORMAL );

                Image aHCImage = Image( ModuleRes( _nHighContrastImageId ) );
                pList->SetExpandedEntryBmp( pEntry, aHCImage, BMP_COLOR_HIGHCONTRAST );
                pList->SetCollapsedEntryBmp( pEntry, aHCImage, BMP_COLOR_HIGHCONTRAST );
            }
        }
    }
}
// -----------------------------------------------------------------------------
DBTreeListBox* OAppDetailPageHelper::createSimpleTree( const rtl::OString& _sHelpId, const Image& _rImage, const Image& _rImageHC )
{
    DBTreeListBox* pTreeView = new DBTreeListBox(this,getBorderWin().getView()->getORB(),WB_HASLINES | WB_SORT | WB_HASBUTTONS | WB_HSCROLL |WB_HASBUTTONSATROOT | WB_TABSTOP);
    pTreeView->SetHelpId( _sHelpId );
    return createTree( pTreeView, _rImage, _rImageHC );
}

// -----------------------------------------------------------------------------
DBTreeListBox* OAppDetailPageHelper::createTree( DBTreeListBox* _pTreeView, const Image& _rImage, const Image& _rImageHC )
{
    WaitObject aWaitCursor(this);

    _pTreeView->SetStyle(_pTreeView->GetStyle() | WB_HASLINES | WB_SORT | WB_HASBUTTONS | WB_HSCROLL |WB_HASBUTTONSATROOT | WB_TABSTOP);
    _pTreeView->GetModel()->SetSortMode(SortAscending);
    _pTreeView->EnableCheckButton( NULL ); // do not show any buttons
    _pTreeView->SetSelectionMode(MULTIPLE_SELECTION);

    _pTreeView->SetDefaultCollapsedEntryBmp( _rImage );
    _pTreeView->SetDefaultCollapsedEntryBmp( _rImageHC, BMP_COLOR_HIGHCONTRAST );
    _pTreeView->SetDefaultExpandedEntryBmp( _rImage );
    _pTreeView->SetDefaultExpandedEntryBmp( _rImageHC, BMP_COLOR_HIGHCONTRAST );

    _pTreeView->SetDoubleClickHdl(LINK(this, OAppDetailPageHelper, OnEntryDoubleClick));
    _pTreeView->SetEnterKeyHdl(LINK(this, OAppDetailPageHelper, OnEntryDoubleClick));
    _pTreeView->SetSelChangeHdl(LINK(this, OAppDetailPageHelper, OnEntrySelChange));

    _pTreeView->setCutHandler(LINK(this, OAppDetailPageHelper, OnCutEntry));
    _pTreeView->setCopyHandler(LINK(this, OAppDetailPageHelper, OnCopyEntry));
    _pTreeView->setPasteHandler(LINK(this, OAppDetailPageHelper, OnPasteEntry));
    _pTreeView->setDeleteHandler(LINK(this, OAppDetailPageHelper, OnDeleteEntry));

    _pTreeView->setControlActionListener( &getBorderWin().getView()->getAppController() );
    _pTreeView->setContextMenuProvider( &getBorderWin().getView()->getAppController() );

    return _pTreeView;
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::clearPages()
{
    showPreview(NULL);
    for (size_t i=0; i < E_ELEMENT_TYPE_COUNT; ++i)
    {
        if ( m_pLists[i] )
            m_pLists[i]->Clear();
    }
}
// -----------------------------------------------------------------------------
sal_Bool OAppDetailPageHelper::isFilled() const
{
    size_t i = 0;
    for (; i < E_ELEMENT_TYPE_COUNT && !m_pLists[i]; ++i)
        ;
    return i != E_ELEMENT_TYPE_COUNT;
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::elementReplaced(ElementType _eType
                                                    ,const ::rtl::OUString& _rOldName
                                                    ,const ::rtl::OUString& _rNewName )
{
    DBTreeListBox* pTreeView = getCurrentView();
    if ( pTreeView )
    {
        ::rtl::OUString sNewName = _rNewName;
        SvLBoxEntry* pEntry = NULL;
        switch( _eType )
        {
            case E_TABLE:
                static_cast<OTableTreeListBox*>(pTreeView)->removedTable( _rOldName );
                static_cast<OTableTreeListBox*>(pTreeView)->addedTable( _rNewName );
                return;

            case E_QUERY:
                pEntry = lcl_findEntry_impl(*pTreeView,_rOldName,pTreeView->First());
                break;
            case E_FORM:
            case E_REPORT:
                pEntry = lcl_findEntry(*pTreeView,_rOldName,pTreeView->First());
                break;
            default:
                OSL_ENSURE(0,"Invalid element type");
        }
        OSL_ENSURE(pEntry,"Do you know that the name isn't existence!");
        if ( pEntry )
        {
            pTreeView->SetEntryText(pEntry,sNewName);
        }
    }
}
// -----------------------------------------------------------------------------
SvLBoxEntry* OAppDetailPageHelper::elementAdded(ElementType _eType,const ::rtl::OUString& _rName, const Any& _rObject )
{
    SvLBoxEntry* pRet = NULL;
    DBTreeListBox* pTreeView = m_pLists[_eType];
    if( _eType == E_TABLE && pTreeView )
    {
        pRet = static_cast<OTableTreeListBox*>(pTreeView)->addedTable( _rName );
    }
    else if ( pTreeView )
    {

        SvLBoxEntry* pEntry = NULL;
        Reference<XChild> xChild(_rObject,UNO_QUERY);
        if ( xChild.is() && E_QUERY != _eType )
        {
            Reference<XContent> xContent(xChild->getParent(),UNO_QUERY);
            if ( xContent.is() )
            {
                ::rtl::OUString sName = xContent->getIdentifier()->getContentIdentifier();
                pEntry = lcl_findEntry(*pTreeView,sName,pTreeView->First());
            }
        }

        sal_uInt16 nImageId = 0, nImageIdH = 0;
        getElementIcons( _eType, nImageId, nImageIdH );
        Reference<XNameAccess> xContainer(_rObject,UNO_QUERY);
        if ( xContainer.is() )
        {
            const sal_Int32 nFolderIndicator = lcl_getFolderIndicatorForType( _eType );
            pRet = pTreeView->InsertEntry( _rName, pEntry, sal_False, LIST_APPEND, reinterpret_cast< void* >( nFolderIndicator ) );
            fillNames( xContainer, _eType, nImageId, nImageIdH, pRet );
        }
        else
        {
            pRet = pTreeView->InsertEntry( _rName, pEntry );

            Image aImage = Image( ModuleRes( nImageId ) );
            pTreeView->SetExpandedEntryBmp( pRet, aImage, BMP_COLOR_NORMAL );
            pTreeView->SetCollapsedEntryBmp( pRet, aImage, BMP_COLOR_NORMAL );

            Image aHCImage = Image( ModuleRes( nImageIdH ) );
            pTreeView->SetExpandedEntryBmp( pRet, aHCImage, BMP_COLOR_HIGHCONTRAST );
            pTreeView->SetCollapsedEntryBmp( pRet, aHCImage, BMP_COLOR_HIGHCONTRAST );
        }
    }
    return pRet;
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::elementRemoved( ElementType _eType,const ::rtl::OUString& _rName )
{
    DBTreeListBox* pTreeView = getCurrentView();
    if ( pTreeView )
    {
        switch( _eType )
        {
            case E_TABLE:
                // we don't need to clear the table here, it is already done by the dispose listener
                static_cast< OTableTreeListBox* >( pTreeView )->removedTable( _rName );
                break;
            case E_QUERY:
                if ( pTreeView )
                {
                    SvLBoxEntry* pEntry = lcl_findEntry_impl(*pTreeView,_rName,pTreeView->First());
                    if ( pEntry )
                        pTreeView->GetModel()->Remove(pEntry);
                }
                break;
            case E_FORM:
            case E_REPORT:
                {
                    if ( pTreeView )
                    {
                        SvLBoxEntry* pEntry = lcl_findEntry(*pTreeView,_rName,pTreeView->First());
                        if ( pEntry )
                            pTreeView->GetModel()->Remove(pEntry);
                    }
                }
                break;
            default:
                OSL_ENSURE(0,"Invalid element type");
        }
        if ( !pTreeView->GetEntryCount() )
            showPreview(NULL);
    }
}
// -----------------------------------------------------------------------------
IMPL_LINK(OAppDetailPageHelper, OnEntryDoubleClick, SvTreeListBox*, _pTree)
{
    OSL_ENSURE( _pTree, "OAppDetailPageHelper, OnEntryDoubleClick: invalid callback!" );
    bool bHandled = ( _pTree != NULL ) && getBorderWin().getView()->getAppController().onEntryDoubleClick( *_pTree );
    return bHandled ? 1L : 0L;
}
// -----------------------------------------------------------------------------
IMPL_LINK(OAppDetailPageHelper, OnEntrySelChange, void*, /*NOINTERESTEDIN*/)
{
    getBorderWin().getView()->getAppController().onSelectionChanged();
    return 1L;
}
// -----------------------------------------------------------------------------
IMPL_LINK( OAppDetailPageHelper, OnCutEntry, void*, /*NOINTERESTEDIN*/ )
{
    getBorderWin().getView()->getAppController().onCutEntry();
    return 1L;
}
// -----------------------------------------------------------------------------
IMPL_LINK( OAppDetailPageHelper, OnCopyEntry, void*, /*NOINTERESTEDIN*/ )
{
    getBorderWin().getView()->getAppController().onCopyEntry();
    return 1L;
}
// -----------------------------------------------------------------------------
IMPL_LINK( OAppDetailPageHelper, OnPasteEntry, void*, /*NOINTERESTEDIN*/ )
{
    getBorderWin().getView()->getAppController().onPasteEntry();
    return 1L;
}
// -----------------------------------------------------------------------------
IMPL_LINK( OAppDetailPageHelper, OnDeleteEntry, void*, /*NOINTERESTEDIN*/ )
{
    getBorderWin().getView()->getAppController().onDeleteEntry();
    return 1L;
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::Resize()
{
    //////////////////////////////////////////////////////////////////////
    // Abmessungen parent window
    Size aOutputSize( GetOutputSize() );
    long nOutputWidth  = aOutputSize.Width();
    long nOutputHeight = aOutputSize.Height();

    Window* pWindow = getCurrentView();
    if ( pWindow )
    {
        Size aFLSize = LogicToPixel( Size( 2, 6 ), MAP_APPFONT );
        sal_Int32 n6PPT = aFLSize.Height();
        long nHalfOutputWidth = static_cast<long>(nOutputWidth * 0.5);

        pWindow->SetPosSizePixel( Point(0, 0), Size(nHalfOutputWidth - n6PPT, nOutputHeight) );

        m_aFL.SetPosSizePixel( Point(nHalfOutputWidth , 0 ), Size(aFLSize.Width(), nOutputHeight ) );

        String sText = m_aTBPreview.GetItemText(SID_DB_APP_DISABLE_PREVIEW);
        Size aTBSize = m_aTBPreview.CalcWindowSizePixel();
        m_aTBPreview.SetPosSizePixel(Point(nOutputWidth - aTBSize.getWidth(), 0 ),
                                     aTBSize );

        m_aBorder.SetPosSizePixel(Point(nHalfOutputWidth + aFLSize.Width() + n6PPT, aTBSize.getHeight() + n6PPT ),
                                  Size(nHalfOutputWidth - aFLSize.Width() - n6PPT, nOutputHeight - 2*n6PPT - aTBSize.getHeight()) );
        m_aPreview.SetPosSizePixel(Point(0,0),m_aBorder.GetSizePixel() );
        m_aDocumentInfo.SetPosSizePixel(Point(0,0),m_aBorder.GetSizePixel() );
        m_pTablePreview->SetPosSizePixel(Point(0,0),m_aBorder.GetSizePixel() );
    }
}
// -----------------------------------------------------------------------------
PreviewMode OAppDetailPageHelper::getPreviewMode()
{
    return m_ePreviewMode;
}
// -----------------------------------------------------------------------------
sal_Bool OAppDetailPageHelper::isPreviewEnabled()
{
    return m_ePreviewMode != E_PREVIEWNONE;
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::switchPreview(PreviewMode _eMode,sal_Bool _bForce)
{
    if ( m_ePreviewMode != _eMode || _bForce )
    {
        m_ePreviewMode = _eMode;

        getBorderWin().getView()->getAppController().previewChanged(static_cast<sal_Int32>(m_ePreviewMode));

        sal_uInt16 nSelectedAction = SID_DB_APP_DISABLE_PREVIEW;
        switch ( m_ePreviewMode )
        {
            case E_PREVIEWNONE:
                nSelectedAction = SID_DB_APP_DISABLE_PREVIEW;
                break;
            case E_DOCUMENT:
                nSelectedAction = SID_DB_APP_VIEW_DOC_PREVIEW;
                break;
            case E_DOCUMENTINFO:
                if ( getBorderWin().getView()->getAppController().isCommandEnabled(SID_DB_APP_VIEW_DOCINFO_PREVIEW) )
                    nSelectedAction = SID_DB_APP_VIEW_DOCINFO_PREVIEW;
                else
                    m_ePreviewMode = E_PREVIEWNONE;
                break;
        }

        m_aMenu->CheckItem(nSelectedAction);
        m_aTBPreview.SetItemText(SID_DB_APP_DISABLE_PREVIEW, m_aMenu->GetItemText(nSelectedAction));
        Resize();

        // simulate a selectionChanged event at the controller, to force the preview to be updated
        if ( isPreviewEnabled() )
        {
            if ( getCurrentView() && getCurrentView()->FirstSelected() )
            {
                getBorderWin().getView()->getAppController().onSelectionChanged();
            }
        }
        else
        {
            m_pTablePreview->Hide();
            m_aPreview.Hide();
            m_aDocumentInfo.Hide();
        }
    }
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::showPreview(const Reference< XContent >& _xContent)
{
    if ( isPreviewEnabled() )
    {
        m_pTablePreview->Hide();

        WaitObject aWaitCursor( this );
        try
        {
            Reference<XCommandProcessor> xContent(_xContent,UNO_QUERY);
            if ( xContent.is() )
            {
                com::sun::star::ucb::Command aCommand;
                if ( m_ePreviewMode == E_DOCUMENT )
                    aCommand.Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("preview"));
                else
                    aCommand.Name = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("getDocumentInfo"));

                Any aPreview = xContent->execute(aCommand,xContent->createCommandIdentifier(),Reference< XCommandEnvironment >());
                if ( m_ePreviewMode == E_DOCUMENT )
                {
                    m_aDocumentInfo.Hide();
                    m_aPreview.Show();

                    Graphic aGraphic;
                    Sequence < sal_Int8 > aBmpSequence;
                    if ( aPreview >>= aBmpSequence )
                    {
                        SvMemoryStream  aData( aBmpSequence.getArray(),
                                            aBmpSequence.getLength(),
                                            STREAM_READ );

                        GraphicConverter::Import(aData,aGraphic);
                    }
                    m_aPreview.setGraphic( aGraphic );
                    m_aPreview.Invalidate();
                }
                else
                {
                    m_aPreview.Hide();
                    m_aDocumentInfo.Clear();
                    m_aDocumentInfo.Show();
                    Reference<document::XDocumentProperties> xProp(
                        aPreview, UNO_QUERY);
                    if ( xProp.is() )
                        m_aDocumentInfo.fill(xProp,String());
                }
            }
            else
            {
                m_aPreview.Hide();
                m_aDocumentInfo.Hide();

                // Why the below code? It might have side effects, as the tree view needs to know
                // its current selection for other purposes than the preview, too.
//                DBTreeListBox* pTreeView = getCurrentView();
//                if ( pTreeView )
//                    pTreeView->clearCurrentSelection();
            }
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::showPreview( const ::rtl::OUString& _sDataSourceName,
                                        const ::rtl::OUString& _sName,
                                        sal_Bool _bTable)
{
    if ( isPreviewEnabled() )
    {
        WaitObject aWaitCursor( this );
        m_aPreview.Hide();
        m_aDocumentInfo.Hide();
        m_pTablePreview->Show();
        if ( !m_xFrame.is() )
        {
            try
            {
                m_xFrame = Reference < XFrame > ( getBorderWin().getView()->getORB()->createInstance( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Frame")) ), UNO_QUERY );
                m_xFrame->initialize( m_xWindow );

                // work-around for #150518#: no layout manager (and thus no toolbars) in the preview
                // Must be called after initialize ... but before any other call to this frame.
                // Otherwise frame throws "life time exceptions" as e.g. NON_INITIALIZED
                Reference< XPropertySet > xFrameProps( m_xFrame, UNO_QUERY_THROW );
                xFrameProps->setPropertyValue( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "LayoutManager" ) ), makeAny(Reference< XLayoutManager >()) );

                Reference<XFramesSupplier> xSup(getBorderWin().getView()->getAppController().getXController()->getFrame(),UNO_QUERY);
                if ( xSup.is() )
                {
                    Reference<XFrames> xFrames = xSup->getFrames();
                    xFrames->append(m_xFrame);
                }
            }
            catch(Exception)
            {
            }
        }

        Reference< XDatabaseDocumentUI > xApplication( getBorderWin().getView()->getAppController().getXController(), UNO_QUERY );
        ::std::auto_ptr< DatabaseObjectView > pDispatcher( new ResultSetBrowser(
            getBorderWin().getView()->getORB(), xApplication, NULL, _bTable
        ) );
        pDispatcher->setTargetFrame( m_xFrame );

        ::comphelper::NamedValueCollection aArgs;
        aArgs.put( "Preview", sal_True );
        aArgs.put( "ReadOnly", sal_True );
        aArgs.put( "AsTemplate", sal_False );
        aArgs.put( (::rtl::OUString)PROPERTY_SHOWMENU, sal_False );

        Reference< XController > xPreview( pDispatcher->openExisting( makeAny( _sDataSourceName ), _sName, aArgs ), UNO_QUERY );
        sal_Bool bClearPreview = !xPreview.is();

        // clear the preview when the query or table could not be loaded
        if ( !bClearPreview )
        {
            Reference< XTabController > xTabController( xPreview, UNO_QUERY );
            bClearPreview = !xTabController.is();
            if ( !bClearPreview )
            {
                Reference< XLoadable > xLoadable( xTabController->getModel(), UNO_QUERY );
                bClearPreview = !( xLoadable.is() && xLoadable->isLoaded() );
            }
        }
        if ( bClearPreview )
            showPreview(NULL);
    }
}
// -----------------------------------------------------------------------------
IMPL_LINK(OAppDetailPageHelper, OnDropdownClickHdl, ToolBox*, /*pToolBox*/)
{
    m_aTBPreview.EndSelection();

    // tell the toolbox that the item is pressed down
    m_aTBPreview.SetItemDown( SID_DB_APP_DISABLE_PREVIEW, sal_True );

    // simulate a mouse move (so the "down" state is really painted)
    Point aPoint = m_aTBPreview.GetItemRect( SID_DB_APP_DISABLE_PREVIEW ).TopLeft();
    MouseEvent aMove( aPoint, 0, MOUSE_SIMPLEMOVE | MOUSE_SYNTHETIC );
    m_aTBPreview.MouseMove( aMove );

    m_aTBPreview.Update();

    // execute the menu
    ::std::auto_ptr<PopupMenu> aMenu(new PopupMenu( ModuleRes( RID_MENU_APP_PREVIEW ) ));

    sal_uInt16 pActions[] = { SID_DB_APP_DISABLE_PREVIEW
                            , SID_DB_APP_VIEW_DOC_PREVIEW
                            , SID_DB_APP_VIEW_DOCINFO_PREVIEW
    };

    for(size_t i=0; i < sizeof(pActions)/sizeof(pActions[0]);++i)
    {
        aMenu->CheckItem(pActions[i],m_aMenu->IsItemChecked(pActions[i]));
    }
    aMenu->EnableItem( SID_DB_APP_VIEW_DOCINFO_PREVIEW, getBorderWin().getView()->getAppController().isCommandEnabled(SID_DB_APP_VIEW_DOCINFO_PREVIEW) );

    // no disabled entries
    aMenu->RemoveDisabledEntries();

    sal_uInt16 nSelectedAction = aMenu->Execute(&m_aTBPreview, m_aTBPreview.GetItemRect( SID_DB_APP_DISABLE_PREVIEW ));
    // "cleanup" the toolbox state
    MouseEvent aLeave( aPoint, 0, MOUSE_LEAVEWINDOW | MOUSE_SYNTHETIC );
    m_aTBPreview.MouseMove( aLeave );
    m_aTBPreview.SetItemDown( SID_DB_APP_DISABLE_PREVIEW, sal_False);
    if ( nSelectedAction )
    {
        m_aTBPreview.SetItemText(SID_DB_APP_DISABLE_PREVIEW, aMenu->GetItemText(nSelectedAction));
        Resize();
        getBorderWin().getView()->getAppController().executeChecked(nSelectedAction,Sequence<PropertyValue>());
    }
    return 0L;
}

// -----------------------------------------------------------------------------
void OAppDetailPageHelper::KeyInput( const KeyEvent& rKEvt )
{
    SvTreeListBox* pCurrentView = getCurrentView();
    OSL_PRECOND( pCurrentView, "OAppDetailPageHelper::KeyInput: how this?" );

    KeyFuncType eFunc = rKEvt.GetKeyCode().GetFunction(); (void)eFunc;
    sal_uInt16      nCode = rKEvt.GetKeyCode().GetCode();

    if ( ( KEY_RETURN == nCode ) && pCurrentView )
    {
        getBorderWin().getView()->getAppController().onEntryDoubleClick( *pCurrentView );
    }
    else
        Window::KeyInput(rKEvt);
}

// -----------------------------------------------------------------------------
void OAppDetailPageHelper::DataChanged( const DataChangedEvent& rDCEvt )
{
    Window::DataChanged( rDCEvt );

        if ( (rDCEvt.GetType() == DATACHANGED_FONTS) ||
        (rDCEvt.GetType() == DATACHANGED_DISPLAY) ||
        (rDCEvt.GetType() == DATACHANGED_FONTSUBSTITUTION) ||
        ((rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
        (rDCEvt.GetFlags() & SETTINGS_STYLE)) )

    {
        ImplInitSettings();
        if ( m_pLists[ E_TABLE ] )
        {
            OTableTreeListBox* pTableTree = dynamic_cast< OTableTreeListBox* >( m_pLists[ E_TABLE ] );
            OSL_ENSURE( pTableTree != NULL, "OAppDetailPageHelper::DataChanged: a tree list for tables which is no TableTreeList?" );
            if ( pTableTree )
                pTableTree->notifyHiContrastChanged();
        }
    }
}
// -----------------------------------------------------------------------------
void OAppDetailPageHelper::ImplInitSettings()
{
    const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();
    if( true )
    {
        Font aFont;
        aFont = rStyleSettings.GetFieldFont();
        aFont.SetColor( rStyleSettings.GetWindowTextColor() );
        SetPointFont( aFont );
        m_aTBPreview.SetPointFont( aFont );
    }

    if( true )
    {
        SetTextColor( rStyleSettings.GetFieldTextColor() );
        SetTextFillColor();
        m_aBorder.SetTextColor( rStyleSettings.GetFieldTextColor() );
        m_aBorder.SetTextFillColor();
        m_aTBPreview.SetTextColor( rStyleSettings.GetFieldTextColor() );
        m_aTBPreview.SetTextFillColor();
    } // if( true )

    if( true )
    {
        SetBackground( rStyleSettings.GetFieldColor() );
        m_aBorder.SetBackground( rStyleSettings.GetFieldColor() );
        m_aFL.SetBackground( rStyleSettings.GetFieldColor() );
        m_aDocumentInfo.SetBackground( rStyleSettings.GetFieldColor() );
        m_aTBPreview.SetBackground( rStyleSettings.GetFieldColor() );
        m_pTablePreview->SetBackground( rStyleSettings.GetFieldColor() );
    } // if( true )
}
// -----------------------------------------------------------------------------
OPreviewWindow::OPreviewWindow(Window* _pParent)
: Window(_pParent)
{
    ImplInitSettings( sal_True, sal_True, sal_True );
}
// -----------------------------------------------------------------------------
sal_Bool OPreviewWindow::ImplGetGraphicCenterRect( const Graphic& rGraphic, Rectangle& rResultRect ) const
{
    const Size aWinSize( GetOutputSizePixel() );
    Size       aNewSize( LogicToPixel( rGraphic.GetPrefSize(), rGraphic.GetPrefMapMode() ) );
    sal_Bool       bRet = sal_False;

    if( aNewSize.Width() && aNewSize.Height() )
    {
        // scale to fit window
        const double fGrfWH = (double) aNewSize.Width() / aNewSize.Height();
        const double fWinWH = (double) aWinSize.Width() / aWinSize.Height();

        if ( fGrfWH < fWinWH )
        {
            aNewSize.Width() = (long) ( aWinSize.Height() * fGrfWH );
            aNewSize.Height()= aWinSize.Height();
        }
        else
        {
            aNewSize.Width() = aWinSize.Width();
            aNewSize.Height()= (long) ( aWinSize.Width() / fGrfWH);
        }

        const Point aNewPos( ( aWinSize.Width()  - aNewSize.Width() ) >> 1,
                             ( aWinSize.Height() - aNewSize.Height() ) >> 1 );

        rResultRect = Rectangle( aNewPos, aNewSize );
        bRet = sal_True;
    }

    return bRet;
}

// ------------------------------------------------------------------------

void OPreviewWindow::Paint( const Rectangle& rRect )
{
    Window::Paint( rRect );

    if( ImplGetGraphicCenterRect( m_aGraphicObj.GetGraphic(), m_aPreviewRect ) )
    {
        const Point aPos( m_aPreviewRect.TopLeft() );
        const Size  aSize( m_aPreviewRect.GetSize() );

        if( m_aGraphicObj.IsAnimated() )
            m_aGraphicObj.StartAnimation( this, aPos, aSize );
        else
            m_aGraphicObj.Draw( this, aPos, aSize );
    }
}
// -----------------------------------------------------------------------------
void OPreviewWindow::DataChanged( const DataChangedEvent& rDCEvt )
{
    Window::DataChanged( rDCEvt );

    if ( (rDCEvt.GetType() == DATACHANGED_SETTINGS) &&
         (rDCEvt.GetFlags() & SETTINGS_STYLE) )
    {
        ImplInitSettings( sal_True, sal_True, sal_True );
        Invalidate();
    }
}
// -----------------------------------------------------------------------------
void OPreviewWindow::ImplInitSettings( sal_Bool bFont, sal_Bool bForeground, sal_Bool bBackground )
{
    const StyleSettings& rStyleSettings = GetSettings().GetStyleSettings();
    if( bFont )
    {
        Font aFont;
        aFont = rStyleSettings.GetFieldFont();
        aFont.SetColor( rStyleSettings.GetWindowTextColor() );
        SetPointFont( aFont );
    }

    if( bForeground || bFont )
    {
        SetTextColor( rStyleSettings.GetFieldTextColor() );
        SetTextFillColor();
    } // if( bForeground || bFont )

    if( bBackground )
        SetBackground( rStyleSettings.GetFieldColor() );
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

