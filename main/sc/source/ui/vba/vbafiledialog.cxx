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

#include "vbafiledialog.hxx"
#include "comphelper/processfactory.hxx"
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/ui/dialogs/XFilePicker.hpp>
#include <com/sun/star/ui/dialogs/XFilePicker2.hpp>
#include <com/sun/star/ui/dialogs/XFilterManager.hpp>
#include <com/sun/star/ui/dialogs/TemplateDescription.hpp>
#include "tools/urlobj.hxx"
#include <sfx2/filedlghelper.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;


ScVbaFileDialog::ScVbaFileDialog( const css::uno::Reference< ooo::vba::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext > &xContext, const css::uno::Reference< css::frame::XModel >& xModel )
:	ScVbaFileDialog_BASE( xParent, xContext, xModel )
{
    m_nDialogType = 3;
    m_bAllowMultiSelect = sal_True;
	m_pSelectedItems = new VbaFileDialogSelectedItems(xParent, xContext, (com::sun::star::container::XIndexAccess *)&m_FileDialogSelectedObj);
}

ScVbaFileDialog::~ScVbaFileDialog()
{
    if (m_pSelectedItems != NULL)
    {
        delete m_pSelectedItems;
    }
}

rtl::OUString&
ScVbaFileDialog::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaFileDialog") );
    return sImplName;
}

css::uno::Sequence<rtl::OUString>
ScVbaFileDialog::getServiceNames()
{
    static Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.excel.FileDialog" ) );
    }
    return aServiceNames;
}

sal_Int32 SAL_CALL
ScVbaFileDialog::getMsoFileDialogType() throw ( css::uno::RuntimeException )
{
    return m_nDialogType;
}

void SAL_CALL
ScVbaFileDialog::setMsoFileDialogType( sal_Int32 nType ) throw ( css::uno::RuntimeException )
{
    m_nDialogType = nType;
}

rtl::OUString SAL_CALL
ScVbaFileDialog::getTitle() throw ( css::uno::RuntimeException )
{
    return m_sTitle;
}

void SAL_CALL
ScVbaFileDialog::setTitle( const rtl::OUString& rTitle ) throw ( css::uno::RuntimeException )
{
    m_sTitle = rTitle;
}

sal_Bool SAL_CALL
ScVbaFileDialog::getAllowMultiSelect() throw ( css::uno::RuntimeException )
{
    return m_bAllowMultiSelect;
}

void SAL_CALL
ScVbaFileDialog::setAllowMultiSelect( sal_Bool bAllow ) throw ( css::uno::RuntimeException )
{
    m_bAllowMultiSelect = bAllow;
}

css::uno::Reference< ooo::vba::XFileDialogSelectedItems > SAL_CALL
ScVbaFileDialog::getSelectedItems() throw ( css::uno::RuntimeException )
{
    css::uno::Reference< ooo::vba::XFileDialogSelectedItems > xFileDlgSlc = (ooo::vba::XFileDialogSelectedItems *)m_pSelectedItems;
    return xFileDlgSlc;
}

sal_Int32 SAL_CALL
ScVbaFileDialog::Show() throw ( css::uno::RuntimeException )
{
    // Returns an Integer indicating if user pressed "Open" button(-1) or "Cancel" button(0).
    sal_Int32 nResult = -1;
    try
    {
        m_sSelectedItems.realloc(0);

        const ::rtl::OUString sServiceName = ::rtl::OUString::createFromAscii( "com.sun.star.ui.dialogs.FilePicker" );

        Reference< lang::XMultiServiceFactory > xMSF( comphelper::getProcessServiceFactory(), uno::UNO_QUERY );
        // Set the type of File Picker Dialog: TemplateDescription::FILEOPEN_SIMPLE.
        Sequence< uno::Any > aDialogType( 1 );
        aDialogType[0] <<= ui::dialogs::TemplateDescription::FILEOPEN_SIMPLE;
        Reference< ui::dialogs::XFilePicker > xFilePicker( xMSF->createInstanceWithArguments( sServiceName, aDialogType ), UNO_QUERY );
        Reference< ui::dialogs::XFilePicker2 > xFilePicker2( xFilePicker, UNO_QUERY );
        Reference< ui::dialogs::XFilterManager > xFilterManager( xFilePicker, UNO_QUERY );
        if ( xFilePicker.is() )
        {
            xFilePicker->setMultiSelectionMode( m_bAllowMultiSelect );
            // Only when there is no any filter in the msoFileDialogFilePicker dialog, then we need to add the All File Filter.
            if ( xFilterManager.is() && xFilterManager->getCurrentFilter().equalsAscii("") )
            {
                xFilterManager->appendFilter( sfx2::FileDialogHelper::GetAllFileFilterName(), rtl::OUString::createFromAscii( "*.*" ) );
            }
            if ( xFilePicker->execute() )
            {
                sal_Bool bUseXFilePicker2 = sal_False;
                Reference< lang::XServiceInfo > xServiceInfo( xFilePicker, UNO_QUERY );
                if (xServiceInfo.is())
                {
                    rtl::OUString sImplName = xServiceInfo->getImplementationName();
                    if (sImplName.equalsAscii("com.sun.star.comp.fpicker.VistaFileDialog") ||
                        sImplName.equalsAscii("com.sun.star.ui.dialogs.SalGtkFilePicker"))
                    {
                        bUseXFilePicker2 = sal_True;
                    }
                }
                if ( bUseXFilePicker2 && xFilePicker2.is() )
                {
                    // On Linux, XFilePicker->getFiles() always return one selected file although we select
                    // more than one file, also on Vista XFilePicker->getFiles() does not work well too,
                    // so we call XFilePicker2->getSelectedFiles() to get selected files.
                    m_sSelectedItems = xFilePicker2->getSelectedFiles();
                }
                else
                {
                    // If only one file is selected, the first entry of the sequence contains the complete path/filename in
                    // URL format. If multiple files are selected, the first entry of the sequence contains the path in URL
                    // format, and the other entries contains the names of the selected files without path information.
                    Sequence< rtl::OUString > aSelectedFiles = xFilePicker->getFiles();
                    sal_Int32 iFileCount = aSelectedFiles.getLength();
                    if ( iFileCount > 1 )
                    {
                        m_sSelectedItems.realloc( iFileCount - 1 );
                        INetURLObject aPath( aSelectedFiles[0] );
                        aPath.setFinalSlash();
                        for ( sal_Int32 i = 1; i < iFileCount; i++ )
                        {
                            if ( aSelectedFiles[i].indexOf ('/') > 0 || aSelectedFiles[i].indexOf ('\\') > 0 )
                            {
                                m_sSelectedItems[i - 1] = aSelectedFiles[i];
                            }
                            else
                            {
                                if ( i == 1 )
                                    aPath.Append( aSelectedFiles[i] );
                                else
                                    aPath.setName( aSelectedFiles[i] );
                                m_sSelectedItems[i - 1] = aPath.GetMainURL(INetURLObject::NO_DECODE);
                            }
                        }
                    }
                    else if ( iFileCount == 1 )
                    {
                        m_sSelectedItems = aSelectedFiles;
                    }
                }

                sal_Int32 iFileCount = m_sSelectedItems.getLength();
                rtl::OUString aTemp;
                for ( sal_Int32 i = 0; i < iFileCount; i++ )
                {
                    INetURLObject aObj( m_sSelectedItems[i] );
                    if ( aObj.GetProtocol() == INET_PROT_FILE )
                    {
                        aTemp = aObj.PathToFileName();
                        m_sSelectedItems[i] = aTemp.getLength() > 0 ? aTemp : m_sSelectedItems[i];
                    }
                }
            }
            else
            {
                nResult = 0;
            }
        }

        m_FileDialogSelectedObj.SetSelectedFile(m_sSelectedItems);
    }
    catch( const uno::Exception& )
    {
        return 0;
    }

    return nResult;
}

namespace filedialog
{
namespace sdecl = comphelper::service_decl;
sdecl::vba_service_class_<ScVbaFileDialog, sdecl::with_args<true> > serviceImpl;
extern sdecl::ServiceDecl const serviceDecl(
    serviceImpl,
    "ScVbaFileDialog",
    "ooo.vba.excel.FileDialog" );
}
