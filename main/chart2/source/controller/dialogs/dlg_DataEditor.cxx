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
#include "precompiled_chart2.hxx"

#include "dlg_DataEditor.hxx"
#include "dlg_DataEditor.hrc"
#include "Strings.hrc"
#include "DataBrowser.hxx"

#include "ResId.hxx"
#include "Strings.hrc"
#include "SchSlotIds.hxx"
#include <sfx2/dispatch.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/taskpanelist.hxx>
#include <svtools/miscopt.hxx>
#include <unotools/pathoptions.hxx>

// for SfxBoolItem
#include <svl/eitem.hxx>

#include <vcl/edit.hxx>

#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/chart2/XChartDocument.hpp>

// for storing/reading the position and size of the dialog
// #include <svtools/viewoptions.hxx>

using namespace ::com::sun::star;
using ::com::sun::star::uno::Reference;
using ::rtl::OUString;


namespace chart
{

DataEditor::DataEditor(
    Window* pParent,
    const Reference< chart2::XChartDocument > & xChartDoc,
    const Reference< uno::XComponentContext > & xContext ) :
        ModalDialog( pParent, SchResId( DLG_DIAGRAM_DATA )),
        m_bReadOnly( false ),
        m_apBrwData( new DataBrowser( this, SchResId( CTL_DATA ), true /* bLiveUpdate */)),
        m_aTbxData( this, SchResId( TBX_DATA )),
        m_xChartDoc( xChartDoc ),
        m_xContext( xContext ),
        m_aToolboxImageList( SchResId( IL_DIAGRAM_DATA )),
        m_aToolboxImageListHighContrast( SchResId( IL_HC_DIAGRAM_DATA ))
{
    FreeResource();

    // set min size to current size
    SetMinOutputSizePixel( GetOutputSizePixel() );

    ApplyImageList();

    m_aTbxData.SetSizePixel( m_aTbxData.CalcWindowSizePixel() );
    m_aTbxData.SetSelectHdl( LINK( this, DataEditor, ToolboxHdl ));

    m_apBrwData->SetCursorMovedHdl( LINK( this, DataEditor,   BrowserCursorMovedHdl ));
    m_apBrwData->SetCellModifiedHdl( LINK( this, DataEditor,  CellModified ));

    UpdateData();
    GrabFocus();
    m_apBrwData->GrabFocus();

    bool bReadOnly = true;
    Reference< frame::XStorable > xStor( m_xChartDoc, uno::UNO_QUERY );
    if( xStor.is())
        bReadOnly = xStor->isReadonly();
    SetReadOnly( bReadOnly );

    // #101228# change buttons to flat-look if set so by user
    SvtMiscOptions aMiscOptions;
    const sal_Int16 nStyle( aMiscOptions.GetToolboxStyle() );
    // react on changes
    aMiscOptions.AddListenerLink( LINK( this, DataEditor, MiscHdl ) );
    m_aTbxData.SetOutStyle( nStyle );

    // set good window width
    Size aWinSize( GetOutputSizePixel());
    Size aWinSizeWithBorder( GetSizePixel());
    Point aWinPos( OutputToAbsoluteScreenPixel( GetPosPixel()));
    sal_Int32 nMinWidth = aWinSize.getWidth();
    sal_Int32 nMaxWidth = GetDesktopRectPixel().getWidth() -
        (aWinSizeWithBorder.getWidth() - aWinSize.getWidth() + aWinPos.getX()) - 10; // leave some space
    sal_Int32 nBrowserWidth = m_apBrwData->GetTotalWidth() + 12 + 16; // plus padding + 16?
    sal_Int32 nWindowWidth = ::std::max( nMinWidth, nBrowserWidth );
    nWindowWidth = ::std::min( nMaxWidth, nBrowserWidth );
    aWinSize.setWidth( nWindowWidth );
    SetOutputSizePixel( aWinSize );
    AdaptBrowseBoxSize();

//     ImplAdjustHeaderControls( false /* bRefreshFromModel */ );

    // allow travelling to toolbar with F6
    notifySystemWindow( this, & m_aTbxData, ::comphelper::mem_fun( & TaskPaneList::AddWindow ));
}

DataEditor::~DataEditor()
{
    notifySystemWindow( this, & m_aTbxData, ::comphelper::mem_fun( & TaskPaneList::RemoveWindow ));

    SvtMiscOptions aMiscOptions;
    aMiscOptions.RemoveListenerLink( LINK( this, DataEditor, MiscHdl ) );

    OSL_TRACE( "DataEditor: DTOR" );
}

// react on click (or keypress) on toolbar icon
IMPL_LINK( DataEditor, ToolboxHdl, void *, EMPTYARG )
{
    switch( m_aTbxData.GetCurItemId() )
    {
        case TBI_DATA_INSERT_ROW:
            m_apBrwData->InsertRow();
            break;
        case TBI_DATA_INSERT_COL:
            m_apBrwData->InsertColumn();
            break;
        case TBI_DATA_INSERT_TEXT_COL:
            m_apBrwData->InsertTextColumn();
            break;
        case TBI_DATA_DELETE_ROW:
            m_apBrwData->RemoveRow();
            break;
        case TBI_DATA_DELETE_COL:
            m_apBrwData->RemoveColumn();
            break;
        case TBI_DATA_SWAP_COL :
            m_apBrwData->SwapColumn ();
            break;
        case TBI_DATA_SWAP_ROW :
            m_apBrwData->SwapRow ();
            break;
    }

    return 0;
}

// refresh toolbar icons according to currently selected cell in browse box
IMPL_LINK( DataEditor, BrowserCursorMovedHdl, void *, EMPTYARG )
{
    if( m_bReadOnly )
        return 0;

    bool bIsDataValid = m_apBrwData->IsEnableItem();

    m_aTbxData.EnableItem( TBI_DATA_INSERT_ROW, bIsDataValid && m_apBrwData->MayInsertRow() );
    m_aTbxData.EnableItem( TBI_DATA_INSERT_COL, bIsDataValid && m_apBrwData->MayInsertColumn() );
    m_aTbxData.EnableItem( TBI_DATA_INSERT_TEXT_COL, bIsDataValid && m_apBrwData->MayInsertColumn() );
    m_aTbxData.EnableItem( TBI_DATA_DELETE_ROW, m_apBrwData->MayDeleteRow() );
    m_aTbxData.EnableItem( TBI_DATA_DELETE_COL, m_apBrwData->MayDeleteColumn() );

    m_aTbxData.EnableItem( TBI_DATA_SWAP_COL,   bIsDataValid && m_apBrwData->MaySwapColumns() );
    m_aTbxData.EnableItem( TBI_DATA_SWAP_ROW,   bIsDataValid && m_apBrwData->MaySwapRows() );

    return 0;
}

// disable all modifying controls
void DataEditor::SetReadOnly( bool bReadOnly )
{
    m_bReadOnly = bReadOnly;
    if( m_bReadOnly )
    {
        m_aTbxData.EnableItem( TBI_DATA_INSERT_ROW, sal_False );
        m_aTbxData.EnableItem( TBI_DATA_INSERT_COL, sal_False );
        m_aTbxData.EnableItem( TBI_DATA_INSERT_TEXT_COL, sal_False );
        m_aTbxData.EnableItem( TBI_DATA_DELETE_ROW, sal_False );
        m_aTbxData.EnableItem( TBI_DATA_DELETE_COL, sal_False );
        m_aTbxData.EnableItem( TBI_DATA_SWAP_COL, sal_False );
        m_aTbxData.EnableItem( TBI_DATA_SWAP_ROW, sal_False );
    }

    m_apBrwData->SetReadOnly( m_bReadOnly );
}

IMPL_LINK( DataEditor, MiscHdl, void*, EMPTYARG )
{
    SvtMiscOptions aMiscOptions;
    sal_Int16 nStyle( aMiscOptions.GetToolboxStyle() );

    m_aTbxData.SetOutStyle( nStyle );

    return 0L;
}

IMPL_LINK( DataEditor, CellModified, void*, EMPTYARG )
{
    return 0;
}

// IMPL_LINK( DataEditor, BrowserColumnResized, void*, EMPTYARG )
// {
//     ImplAdjustHeaderControls( false /* bRefreshFromModel */ );
//     return 0;
// }

// IMPL_LINK( DataEditor, BrowserContentScrolled, void*, EMPTYARG )
// {
//     ImplAdjustHeaderControls( false /* bRefreshFromModel */ );
//     return 0;
// }

void DataEditor::UpdateData()
{
    m_apBrwData->SetDataFromModel( m_xChartDoc, m_xContext );
}

// react on the change of the underlying document by displaying the new data
// void DataEditor::SFX_NOTIFY(
//     SfxBroadcaster& rBC,
//     const TypeId& rBCType,
//     const SfxHint& rHint,
//     const TypeId& rHintType )
// {
//     if( rHint.Type() == TYPE(SfxSimpleHint) )
//     {
//         // note: if dynamic_cast works this should be changed
//         switch( static_cast< const SfxSimpleHint & >( rHint ).GetId())
//         {
//             case SFX_HINT_DOCCHANGED:
//                 UpdateData();
//                 break;

//             case SFX_HINT_DYING:
//                 break;
//         }
//     }
// }

// {
//     sal_Bool bRet = sal_True;

//     // confirm changes currently made and not saved
//     m_apBrwData->EndEditing();

//     if( m_apBrwData->IsDirty() )
//     {
//         QueryBox aSafetyQuery( this, WB_YES_NO_CANCEL | WB_DEF_YES,
//                                String( SchResId( STR_DIAGRAM_DATA_SAFETY_QUERY )));
//         long nQueryResult = aSafetyQuery.Execute();

//         bRet = ( nQueryResult != RET_CANCEL );

//         if( nQueryResult == RET_YES )
//         {
//             // save changes
//             ApplyChangesToModel();
//         }
//     }

//     if( bRet )
//     {
//         // close child window
//         SfxBoolItem aItem( SID_DIAGRAM_DATA, sal_False );
//         if( m_pBindings )
//         {
//             SfxDispatcher* pDisp = m_pBindings->GetDispatcher();
//             if( pDisp )
//                 pDisp->Execute( SID_DIAGRAM_DATA, SFX_CALLMODE_ASYNCHRON, &aItem, 0L);
//             else
//                 DBG_ERROR( "Couldn't dispatch command" );
//         }
//     }

//     return ( bRet? SfxFloatingWindow::Close(): sal_False );
// }

void DataEditor::AdaptBrowseBoxSize()
{
    Size aSize( PixelToLogic( GetResizeOutputSizePixel(), MAP_APPFONT ));
    Size aDataSize;

    aDataSize.setWidth( aSize.getWidth() - 12 );
    aDataSize.setHeight( aSize.getHeight() - 31 -24 );

    m_apBrwData->SetSizePixel( LogicToPixel( aDataSize, MAP_APPFONT ));
}

void DataEditor::Resize()
{
    Dialog::Resize();
    AdaptBrowseBoxSize();
//     ImplAdjustHeaderControls( false /* bRefreshFromModel */ );
}

sal_Bool DataEditor::Close()
{
    if( ApplyChangesToModel() )
        return ModalDialog::Close();
    else
        return sal_True;
}

bool DataEditor::ApplyChangesToModel()
{
    return m_apBrwData->EndEditing();
}

// sets the correct toolbar icons depending on the current mode (e.g. high contrast)
void DataEditor::ApplyImageList()
{
    bool bIsHighContrast = ( true && GetSettings().GetStyleSettings().GetHighContrastMode() );

    ImageList& rImgLst = bIsHighContrast
        ? m_aToolboxImageListHighContrast
        : m_aToolboxImageList;

    m_aTbxData.SetImageList( rImgLst );
}

// add/remove a window (the toolbar) to/from the global list, so that F6
// travels/no longer travels over this window. _rMemFunc may be
// TaskPaneList::AddWindow or TaskPaneList::RemoveWindow
void DataEditor::notifySystemWindow(
    Window* pWindow, Window* pToRegister,
    ::comphelper::mem_fun1_t< TaskPaneList, Window* > rMemFunc )
{
    OSL_ENSURE( pWindow, "Window must not be null!" );
    if( !pWindow )
        return;
    Window* pParent = pWindow->GetParent();
    while( pParent && ! pParent->IsSystemWindow() )
    {
        pParent = pParent->GetParent();
    }
    if ( pParent && pParent->IsSystemWindow())
    {
        SystemWindow* pSystemWindow = static_cast< SystemWindow* >( pParent );
        rMemFunc( pSystemWindow->GetTaskPaneList(),( pToRegister ));
    }
}

} // namespace chart
