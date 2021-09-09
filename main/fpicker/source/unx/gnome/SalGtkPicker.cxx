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
#include "precompiled_fpicker.hxx"

//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uri/XExternalUriReferenceTranslator.hpp>
#include <com/sun/star/lang/SystemDependent.hpp>
#include <com/sun/star/awt/XSystemDependentWindowPeer.hpp>
#include <com/sun/star/awt/SystemDependentXWindow.hpp>
#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <comphelper/processfactory.hxx>
#include <cppuhelper/interfacecontainer.h>
#include <rtl/process.h>
#include <osl/diagnose.h>
#include <com/sun/star/uno/Any.hxx>
#include <FPServiceInfo.hxx>
#include <vos/mutex.hxx>
#include <vcl/svapp.hxx>
#include "SalGtkPicker.hxx"
#include <tools/urlobj.hxx>
#include <stdio.h>

//------------------------------------------------------------------------
// namespace directives
//------------------------------------------------------------------------

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::uno;

rtl::OUString SalGtkPicker::uritounicode(const gchar* pIn)
{
	if (!pIn)
		return rtl::OUString();

	rtl::OUString sURL( const_cast<const sal_Char *>(pIn), strlen(pIn),
		RTL_TEXTENCODING_UTF8 );

	INetURLObject aURL(sURL);
	if (INET_PROT_FILE == aURL.GetProtocol())
	{
		// all the URLs are handled by office in UTF-8
		// so the Gnome FP related URLs should be converted accordingly
		gchar *pEncodedFileName = g_filename_from_uri(pIn, NULL, NULL);
		if ( pEncodedFileName )
		{
			rtl::OUString sEncoded(pEncodedFileName, strlen(pEncodedFileName),
				osl_getThreadTextEncoding());
			INetURLObject aCurrentURL(sEncoded, INetURLObject::FSYS_UNX);
			aCurrentURL.SetHost(aURL.GetHost());
			sURL = aCurrentURL.getExternalURL();
		}
		else
		{
			OUString aNewURL = Reference<uri::XExternalUriReferenceTranslator>(Reference<XMultiServiceFactory>(comphelper::getProcessServiceFactory(), UNO_QUERY_THROW)->createInstance(OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.uri.ExternalUriReferenceTranslator"))), UNO_QUERY_THROW)->translateToInternal(sURL);
			if( aNewURL.getLength() )
				sURL = aNewURL;
		}
	}
	return sURL;
}

rtl::OString SalGtkPicker::unicodetouri(const rtl::OUString &rURL)
{
	// all the URLs are handled by office in UTF-8 ( and encoded with "%xx" codes based on UTF-8 )
	// so the Gnome FP related URLs should be converted accordingly
	OString sURL = OUStringToOString(rURL, RTL_TEXTENCODING_UTF8);
	INetURLObject aURL(rURL);
	if (INET_PROT_FILE == aURL.GetProtocol())
	{
		OUString aNewURL = Reference<uri::XExternalUriReferenceTranslator>(Reference<XMultiServiceFactory>(comphelper::getProcessServiceFactory(), UNO_QUERY_THROW)->createInstance(OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.uri.ExternalUriReferenceTranslator"))), UNO_QUERY_THROW)->translateToExternal( rURL );

		if( aNewURL.getLength() )
		{
			// At this point the URL should contain ascii characters only actually
			sURL = OUStringToOString( aNewURL, osl_getThreadTextEncoding() );
		}
	}
	return sURL;
}

gboolean canceldialog(RunDialog *pDialog)
{
	pDialog->cancel();
	return false;
}

extern "C"
{
	struct Display;
	extern GdkDisplay* gdk_x11_lookup_xdisplay (void*xdisplay);
}

RunDialog::RunDialog( GtkWidget *pDialog, uno::Reference< awt::XExtendedToolkit >& rToolkit ) :
	cppu::WeakComponentImplHelper1< awt::XTopWindowListener >( maLock ),
	mpDialog(pDialog), mpCreatedParent(NULL), mxToolkit(rToolkit)
{
	awt::SystemDependentXWindow aWindowHandle;

	if (mxToolkit.is())
	{
		uno::Reference< awt::XTopWindow > xWindow(mxToolkit->getActiveTopWindow());
		if (xWindow.is())
		{
			uno::Reference< awt::XSystemDependentWindowPeer > xSystemDepParent(xWindow, uno::UNO_QUERY);
			if (xSystemDepParent.is())
			{

				sal_Int8 processID[16];

				rtl_getGlobalProcessId( (sal_uInt8*)processID );
				uno::Sequence<sal_Int8> processIdSeq(processID, 16);
				uno::Any anyHandle = xSystemDepParent->getWindowHandle(processIdSeq, SystemDependent::SYSTEM_XWINDOW);

				anyHandle >>= aWindowHandle;
			}
		}
	}

	GdkThreadLock aLock;

	GdkDisplay *pDisplay = aWindowHandle.DisplayPointer ? gdk_x11_lookup_xdisplay(reinterpret_cast<void*>(static_cast<sal_IntPtr>(aWindowHandle.DisplayPointer))) : NULL;
	GdkWindow* pParent = pDisplay ? gdk_window_lookup_for_display(pDisplay, aWindowHandle.WindowHandle) : NULL;
	if (!pParent && pDisplay)
		pParent = mpCreatedParent = gdk_window_foreign_new_for_display( pDisplay, aWindowHandle.WindowHandle);
	if (pParent)
	{
		gtk_widget_realize(mpDialog);
		gdk_window_set_transient_for(mpDialog->window, pParent);
	}
}


RunDialog::~RunDialog()
{
	if (mpCreatedParent)
	{
		GdkThreadLock aLock;
		gdk_window_destroy (mpCreatedParent);
	}
}

void SAL_CALL RunDialog::windowOpened( const ::com::sun::star::lang::EventObject& )
	throw (::com::sun::star::uno::RuntimeException)
{
	GdkThreadLock aLock;
	g_timeout_add_full(G_PRIORITY_HIGH_IDLE, 0, (GSourceFunc)canceldialog, this, NULL);
}

void RunDialog::cancel()
{
	GdkThreadLock aLock;
	gtk_dialog_response( GTK_DIALOG( mpDialog ), GTK_RESPONSE_CANCEL );
	gtk_widget_hide( mpDialog );
}

gint RunDialog::run()
{
	if (mxToolkit.is())
		mxToolkit->addTopWindowListener(this);

	GdkThreadLock aLock;
	gint nStatus = gtk_dialog_run( GTK_DIALOG( mpDialog ) );

	if (mxToolkit.is())
		mxToolkit->removeTopWindowListener(this);

	if (nStatus != 1)	//PLAY
		gtk_widget_hide( mpDialog );

	return nStatus;
}

static void lcl_setGTKLanguage(const uno::Reference<lang::XMultiServiceFactory>& xServiceMgr)
{
	static bool bSet = false;
	if (bSet)
		return;

	OUString sUILocale;
	try
	{
		uno::Reference<lang::XMultiServiceFactory> xConfigMgr =
			uno::Reference<lang::XMultiServiceFactory>(xServiceMgr->createInstance(
				OUString::createFromAscii("com.sun.star.configuration.ConfigurationProvider")),
				UNO_QUERY_THROW );

		Sequence< Any > theArgs(1);
		theArgs[ 0 ] <<= OUString::createFromAscii("org.openoffice.Office.Linguistic/General");

		uno::Reference< container::XNameAccess > xNameAccess =
			uno::Reference< container::XNameAccess >(xConfigMgr->createInstanceWithArguments(
				OUString::createFromAscii("com.sun.star.configuration.ConfigurationAccess"), theArgs ),
				UNO_QUERY_THROW );

		if (xNameAccess.is())
			xNameAccess->getByName(OUString::createFromAscii("UILocale")) >>= sUILocale;
	} catch (...) {}

	if (sUILocale.getLength())
	{
		sUILocale = sUILocale.replace('-', '_');
		rtl::OUString envVar(RTL_CONSTASCII_USTRINGPARAM("LANGUAGE"));
		osl_setEnvironment(envVar.pData, sUILocale.pData);
	}
	bSet = true;
}

SalGtkPicker::SalGtkPicker(const uno::Reference<lang::XMultiServiceFactory>& xServiceMgr) : m_pDialog(0)
{
	lcl_setGTKLanguage(xServiceMgr);
}

SalGtkPicker::~SalGtkPicker()
{
	if (m_pDialog)
	{
		GdkThreadLock aLock;
		gtk_widget_destroy(m_pDialog);
	}
}

void SAL_CALL SalGtkPicker::implsetDisplayDirectory( const rtl::OUString& aDirectory )
	throw( lang::IllegalArgumentException, uno::RuntimeException )
{
	OSL_ASSERT( m_pDialog != NULL );

	OString aTxt = unicodetouri(aDirectory);

	if( aTxt.lastIndexOf('/') == aTxt.getLength() - 1 )
		aTxt = aTxt.copy( 0, aTxt.getLength() - 1 );

	OSL_TRACE( "setting path to %s\n", aTxt.getStr() );

	GdkThreadLock aLock;
	gtk_file_chooser_set_current_folder_uri( GTK_FILE_CHOOSER( m_pDialog ),
		aTxt.getStr() );
}

rtl::OUString SAL_CALL SalGtkPicker::implgetDisplayDirectory() throw( uno::RuntimeException )
{
	OSL_ASSERT( m_pDialog != NULL );

	GdkThreadLock aLock;

	gchar* pCurrentFolder =
		gtk_file_chooser_get_current_folder_uri( GTK_FILE_CHOOSER( m_pDialog ) );
	::rtl::OUString aCurrentFolderName = uritounicode(pCurrentFolder);
	g_free( pCurrentFolder );

	return aCurrentFolderName;
}

void SAL_CALL SalGtkPicker::implsetTitle( const rtl::OUString& aTitle ) throw( uno::RuntimeException )
{
	OSL_ASSERT( m_pDialog != NULL );

	::rtl::OString aWindowTitle = OUStringToOString( aTitle, RTL_TEXTENCODING_UTF8 );

	GdkThreadLock aLock;
	gtk_window_set_title( GTK_WINDOW( m_pDialog ), aWindowTitle.getStr() );
}
