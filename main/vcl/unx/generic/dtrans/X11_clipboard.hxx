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



#ifndef _DTRANS_X11_CLIPBOARD_HXX_
#define _DTRANS_X11_CLIPBOARD_HXX_

#include <X11_selection.hxx>

#ifndef _COM_SUN_STAR_DATATRANSFER_CLIPBAORD_XCLIPBOARDEX_HPP_
#include <com/sun/star/datatransfer/clipboard/XClipboardEx.hpp>
#endif

#ifndef _COM_SUN_STAR_DATATRANSFER_CLIPBAORD_XCLIPBOARDNOTIFIER_HPP_
#include <com/sun/star/datatransfer/clipboard/XClipboardNotifier.hpp>
#endif
#include <cppuhelper/compbase4.hxx>

// ------------------------------------------------------------------------

#define X11_CLIPBOARD_IMPLEMENTATION_NAME "com.sun.star.datatransfer.X11ClipboardSupport"

namespace x11 {

	class X11Clipboard :
		public ::cppu::WeakComponentImplHelper4 <
        ::com::sun::star::datatransfer::clipboard::XClipboardEx,
    	::com::sun::star::datatransfer::clipboard::XClipboardNotifier,
        ::com::sun::star::lang::XServiceInfo,
        ::com::sun::star::lang::XInitialization
		>,
		public SelectionAdaptor
	{
		com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > m_aContents;
		com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboardOwner > m_aOwner;

		SelectionManager&										m_rSelectionManager;
		com::sun::star::uno::Reference< ::com::sun::star::lang::XInitialization >	m_xSelectionManager;
        ::std::list< com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboardListener > > m_aListeners;
		Atom													m_aSelection;

	protected:


		friend class SelectionManager;
		friend class X11_Transferable;

		void fireChangedContentsEvent();
		void clearContents();

	public:

		X11Clipboard( SelectionManager& rManager, Atom aSelection );
		virtual ~X11Clipboard();

		static X11Clipboard* get( const ::rtl::OUString& rDisplayName, Atom aSelection );

		/*
		 *	XInitialization
		 */
		virtual void SAL_CALL initialize( const Sequence< Any >& arguments );

		/*
		 * XServiceInfo
		 */

		virtual ::rtl::OUString SAL_CALL getImplementationName(	 );

		virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );

		virtual Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

		/*
		 * XClipboard
		 */

		virtual com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > SAL_CALL getContents();

		virtual void SAL_CALL setContents(
			const com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable >& xTrans,
			const com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboardOwner >& xClipboardOwner );

		virtual ::rtl::OUString SAL_CALL getName();

		/*
		 * XClipboardEx
		 */

		virtual sal_Int8 SAL_CALL getRenderingCapabilities();

		/*
		 * XClipboardNotifier
		 */
		virtual void SAL_CALL addClipboardListener(
			const com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboardListener >& listener );

		virtual void SAL_CALL removeClipboardListener(
			const com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboardListener >& listener );

		/*
		 *	SelectionAdaptor
		 */
		virtual com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > getTransferable();
		virtual void clearTransferable();
        virtual void fireContentsChanged();
        virtual com::sun::star::uno::Reference< XInterface > getReference() throw();
	};

// ------------------------------------------------------------------------

	Sequence< ::rtl::OUString > SAL_CALL X11Clipboard_getSupportedServiceNames();
	com::sun::star::uno::Reference< XInterface > SAL_CALL X11Clipboard_createInstance(
		const com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & xMultiServiceFactory);

// ------------------------------------------------------------------------

} // namepspace

#endif
