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

#ifndef SC_VBA_FILEDIALOG_HXX
#define SC_VBA_FILEDIALOG_HXX

#include <cppuhelper/implbase1.hxx>
#include <ooo/vba/XFileDialog.hpp>
#include <vbahelper/vbahelperinterface.hxx>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include "vbafiledialogselecteditems.hxx"

typedef InheritedHelperInterfaceImpl1< ooo::vba::XFileDialog > ScVbaFileDialog_BASE;

class ScVbaFileDialog : public ScVbaFileDialog_BASE
{
public:
    // MS Excel VBA: Constructs FileDialog with parent, context, and model.
    // Status: implemented
    ScVbaFileDialog( const css::uno::Reference< ooo::vba::XHelperInterface >& xParent,
                     const css::uno::Reference< css::uno::XComponentContext >& xContext,
                     const css::uno::Reference< css::frame::XModel >& xModel );
    // MS Excel VBA: Factory constructor for service instantiation.
    // Status: implemented
    ScVbaFileDialog( const css::uno::Sequence< css::uno::Any >& args,
                     const css::uno::Reference< css::uno::XComponentContext >& xContext );
    // MS Excel VBA: Destructor.
    // Status: implemented
    virtual ~ScVbaFileDialog();

    // XHelperInterface
    // MS Excel VBA: Returns the implementation name for UNO service identification.
    // Status: implemented - returns "ScVbaFileDialog"
    virtual rtl::OUString& getServiceImplName();
    // MS Excel VBA: Returns the service names this implementation supports.
    // Status: implemented - returns "ooo.vba.excel.FileDialog"
    virtual css::uno::Sequence<rtl::OUString> getServiceNames();

    // XFileDialog
    // MS Excel VBA: Returns the dialog type (msoFileDialogFilePicker, etc.).
    // Status: implemented - returns internal dialog type (default 3 = file picker)
    virtual sal_Int32 SAL_CALL getMsoFileDialogType() throw ( css::uno::RuntimeException );
    // MS Excel VBA: Sets the dialog type.
    // Status: implemented
    virtual void SAL_CALL setMsoFileDialogType( sal_Int32 nType ) throw ( css::uno::RuntimeException );
    // MS Excel VBA: Returns the dialog title.
    // Status: implemented
    virtual rtl::OUString SAL_CALL getTitle() throw ( css::uno::RuntimeException );
    // MS Excel VBA: Sets the dialog title.
    // Status: implemented
    virtual void SAL_CALL setTitle( const rtl::OUString& rTitle ) throw ( css::uno::RuntimeException );
    // MS Excel VBA: Gets/sets whether multiple file selection is enabled.
    // Status: implemented
    virtual sal_Bool SAL_CALL getAllowMultiSelect() throw ( css::uno::RuntimeException );
    virtual void SAL_CALL setAllowMultiSelect( sal_Bool bAllow ) throw ( css::uno::RuntimeException );
    // MS Excel VBA: Returns the selected items in the dialog.
    // Status: implemented - returns SelectedItems collection
    virtual css::uno::Reference< ooo::vba::XFileDialogSelectedItems > SAL_CALL getSelectedItems() throw ( css::uno::RuntimeException );
    // MS Excel VBA: Displays the file dialog and returns user's selection status.
    // Status: implemented - wraps UNO FilePicker service
    virtual sal_Int32 SAL_CALL Show() throw ( css::uno::RuntimeException );

private:
    css::uno::Reference< css::frame::XModel > m_xModel;
    sal_Int32 m_nDialogType;
    rtl::OUString m_sTitle;
    sal_Bool m_bAllowMultiSelect;
    css::uno::Sequence< rtl::OUString > m_sSelectedItems;
    VbaFileDialogSelectedItems *m_pSelectedItems;
    VbaFileDialogSelectedObj m_FileDialogSelectedObj;
};

#endif
