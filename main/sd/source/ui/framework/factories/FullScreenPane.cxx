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

#include "precompiled_sd.hxx"

#include "FullScreenPane.hxx"
#include "ViewShellBase.hxx"
#include <cppcanvas/vclfactory.hxx>
#include <sfx2/dispatch.hxx>
#include <vcl/wrkwin.hxx>
#include <vcl/svapp.hxx>
#include <vcl/dialog.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XLayoutManager.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/util/URL.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::drawing::framework;
using ::rtl::OUString;

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

namespace sd { namespace framework {

FullScreenPane::FullScreenPane (
	const Reference<XComponentContext>& rxComponentContext,
	const Reference<XResourceId>& rxPaneId,
	const ::Window* pViewShellWindow)
	:FrameWindowPane(rxPaneId,NULL),
	 mxComponentContext(rxComponentContext),
	 mpWorkWindow(NULL)
{
	::Window* pParent = NULL;
	mpWorkWindow.reset(new WorkWindow(
		pParent,
		0)); // For debugging (non-fullscreen) use WB_BORDER | WB_MOVEABLE | WB_SIZEABLE));

	if ( ! rxPaneId.is())
		throw lang::IllegalArgumentException();

	sal_Int32 nScreenNumber = 1;
	ExtractArguments(rxPaneId, nScreenNumber);

	if (mpWorkWindow.get() == NULL)
		return;

	// Create a new top-level window that is displayed full screen.
	mpWorkWindow->ShowFullScreenMode(sal_True, nScreenNumber);
	// For debugging (non-fullscreen) use mpWorkWindow->SetScreenNumber(nScreenNumber);
	mpWorkWindow->SetMenuBarMode(MENUBAR_MODE_HIDE);
	mpWorkWindow->SetBorderStyle(WINDOW_BORDER_REMOVEBORDER);
	mpWorkWindow->SetBackground(Wallpaper());
	// Don't show the window right now in order to allow the setting of an
	// accessibility object: accessibility objects are typically
	// requested by AT-tools when the window is shown. Changing it
	// afterwards may or may not work.

	// Add resize listener at the work window.
	Link aWindowEventHandler (LINK(this, FullScreenPane, WindowEventHandler));
	mpWorkWindow->AddEventListener(aWindowEventHandler);

	// Set title and icon of the new window to those of the current window
	// of the view shell.
	if (pViewShellWindow != NULL)
	{
		const SystemWindow* pSystemWindow = pViewShellWindow->GetSystemWindow();
		mpWorkWindow->SetText(pSystemWindow->GetText());
		mpWorkWindow->SetIcon(pSystemWindow->GetIcon());
	}

	// For some reason the VCL canvas can not paint into a WorkWindow.
	// Therefore a child window is created that covers the WorkWindow
	// completely.
	mpWindow = new ::Window(mpWorkWindow.get());
	mpWindow->SetPosSizePixel(Point(0,0), mpWorkWindow->GetSizePixel());
	mpWindow->SetBackground(Wallpaper());
	mxWindow = VCLUnoHelper::GetInterface(mpWindow);

	// Create the canvas.
	mxCanvas = CreateCanvas();

	mpWindow->GrabFocus();
}


FullScreenPane::~FullScreenPane (void) throw()
{
}


void SAL_CALL FullScreenPane::disposing (void)
{
	// We have created the window pointed to by mpWindow, we delete it.
	if (mpWindow != NULL)
	{
		delete mpWindow;
	}

	if (mpWorkWindow.get() != NULL)
	{
		Link aWindowEventHandler (LINK(this, FullScreenPane, WindowEventHandler));
		mpWorkWindow->RemoveEventListener(aWindowEventHandler);
		mpWorkWindow.reset();
	}


	FrameWindowPane::disposing();
}


//----- XPane -----------------------------------------------------------------

sal_Bool SAL_CALL FullScreenPane::isVisible (void)
	throw (RuntimeException)
{
	ThrowIfDisposed();

	if (mpWindow != NULL)
		return mpWindow->IsReallyVisible();
	else
		return false;
}


void SAL_CALL FullScreenPane::setVisible (const sal_Bool bIsVisible)
	throw (RuntimeException)
{
	ThrowIfDisposed();

	if (mpWindow != NULL)
		mpWindow->Show(bIsVisible);
	if( bool(mpWorkWindow))
		mpWorkWindow->Show(bIsVisible);
}


Reference<accessibility::XAccessible> SAL_CALL FullScreenPane::getAccessible (void)
	throw (RuntimeException)
{
	ThrowIfDisposed();

	if( bool(mpWorkWindow))
		return mpWorkWindow->GetAccessible(sal_False);
	else
		return NULL;
}


void SAL_CALL FullScreenPane::setAccessible (
	const Reference<accessibility::XAccessible>& rxAccessible)
	throw (RuntimeException)
{
	ThrowIfDisposed();

	if (mpWindow != NULL)
	{
		Reference<lang::XInitialization> xInitializable (rxAccessible, UNO_QUERY);
		if (xInitializable.is())
		{
			::Window* pParentWindow = mpWindow->GetParent();
			Reference<accessibility::XAccessible> xAccessibleParent;
			if (pParentWindow != NULL)
				xAccessibleParent = pParentWindow->GetAccessible();
			Sequence<Any> aArguments (1);
			aArguments[0] = Any(xAccessibleParent);
			xInitializable->initialize(aArguments);
		}
		GetWindow()->SetAccessible(rxAccessible);
	}
}


//-----------------------------------------------------------------------------

IMPL_LINK(FullScreenPane, WindowEventHandler, VclWindowEvent*, pEvent)
{
	switch (pEvent->GetId())
	{
		case VCLEVENT_WINDOW_RESIZE:
			GetWindow()->SetPosPixel(Point(0,0));
			GetWindow()->SetSizePixel(Size(
				mpWorkWindow->GetSizePixel().Width(),
				mpWorkWindow->GetSizePixel().Height()));
			break;

		case VCLEVENT_OBJECT_DYING:
			mpWorkWindow.reset();
			break;
	}
	return 1;
}


Reference<rendering::XCanvas> FullScreenPane::CreateCanvas (void)
	throw (RuntimeException)
{
	::Window* pWindow = VCLUnoHelper::GetWindow(mxWindow);
	if (pWindow != NULL)
	{
		Sequence<Any> aArg (5);

		// common: first any is VCL pointer to window (for VCL canvas)
		aArg[0] = makeAny(reinterpret_cast<sal_Int64>(pWindow));
		aArg[1] = Any();
		aArg[2] = makeAny(::com::sun::star::awt::Rectangle());
		aArg[3] = makeAny(sal_False);
		aArg[4] = makeAny(mxWindow);

		Reference<lang::XMultiServiceFactory> xFactory (
			mxComponentContext->getServiceManager(), UNO_QUERY_THROW);
		return Reference<rendering::XCanvas>(
			xFactory->createInstanceWithArguments(
				OUString::createFromAscii("com.sun.star.rendering.SpriteCanvas.VCL"),
				aArg),
			UNO_QUERY);
	}
	else
		throw RuntimeException();
}


void FullScreenPane::ExtractArguments (
	const Reference<XResourceId>& rxPaneId,
	sal_Int32& rnScreenNumberReturnValue)
{
	// Extract arguments from the resource URL.
	const util::URL aURL = rxPaneId->getFullResourceURL();
	sal_Int32 nIndex = 0;
	while (nIndex >= 0)
	{
		const OUString aToken = aURL.Arguments.getToken(0, '&', nIndex);
		if (aToken.getLength() > 0)
		{
			// Split at the first '='.
			const sal_Int32 nAssign = aToken.indexOf('=');
			const OUString sKey = aToken.copy(0, nAssign);
			const OUString sValue = aToken.copy(nAssign+1);

			if (sKey.compareToAscii("ScreenNumber") == 0)
			{
				rnScreenNumberReturnValue = sValue.toInt32();
			}
		}
	}
}

} } // end of namespace sd::framework

/* vim: set noet sw=4 ts=4: */
