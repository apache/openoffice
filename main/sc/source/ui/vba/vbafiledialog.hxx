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
#include <vbahelper/vbadialogsbase.hxx>
#include <ooo/vba/XFileDialog.hpp>
#include "vbafiledialogselecteditems.hxx"

typedef cppu::ImplInheritanceHelper1< VbaDialogsBase, ooo::vba::XFileDialog > ScVbaFileDialog_BASE;

class ScVbaFileDialog : public ScVbaFileDialog_BASE
{
public:
    ScVbaFileDialog( const css::uno::Reference< ooo::vba::XHelperInterface >& xParent,
                     const css::uno::Reference< css::uno::XComponentContext >& xContext,
                     const css::uno::Reference< css::frame::XModel >& xModel );
    virtual ~ScVbaFileDialog();

    // XHelperInterface
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence< rtl::OUString > getServiceNames();

    // XFileDialog
    virtual sal_Int32 SAL_CALL getMsoFileDialogType() throw ( css::uno::RuntimeException );
    virtual void SAL_CALL setMsoFileDialogType( sal_Int32 nType ) throw ( css::uno::RuntimeException );
    virtual rtl::OUString SAL_CALL getTitle() throw ( css::uno::RuntimeException );
    virtual void SAL_CALL setTitle( const rtl::OUString& rTitle ) throw ( css::uno::RuntimeException );
    virtual sal_Bool SAL_CALL getAllowMultiSelect() throw ( css::uno::RuntimeException );
    virtual void SAL_CALL setAllowMultiSelect( sal_Bool bAllow ) throw ( css::uno::RuntimeException );
    virtual css::uno::Reference< ooo::vba::XFileDialogSelectedItems > SAL_CALL getSelectedItems() throw ( css::uno::RuntimeException );
    virtual sal_Int32 SAL_CALL Show() throw ( css::uno::RuntimeException );

private:
    sal_Int32 m_nDialogType;
    rtl::OUString m_sTitle;
    sal_Bool m_bAllowMultiSelect;
    css::uno::Sequence< rtl::OUString > m_sSelectedItems;
    VbaFileDialogSelectedItems *m_pSelectedItems;
    VbaFileDialogSelectedObj m_FileDialogSelectedObj;
};

#endif