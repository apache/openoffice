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
#include "precompiled_sd.hxx"

#include "view/SlsFontProvider.hxx"

#include "controller/SlideSorterController.hxx"

#include <sfx2/app.hxx>
#include <com/sun/star/uno/RuntimeException.hpp>


using namespace ::sd::slidesorter;

namespace sd { namespace slidesorter { namespace view {

FontProvider* FontProvider::mpInstance = NULL;

FontProvider& FontProvider::Instance (void)
{
	if (mpInstance == NULL)
	{
		::osl::GetGlobalMutex aMutexFunctor;
		::osl::MutexGuard aGuard (aMutexFunctor());
		if (mpInstance == NULL)
		{
			// Create an instance of the class and register it at the
			// SdGlobalResourceContainer so that it is eventually released.
			FontProvider* pInstance = new FontProvider();
			SdGlobalResourceContainer::Instance().AddResource (
				::std::auto_ptr<SdGlobalResource>(pInstance));
			OSL_DOUBLE_CHECKED_LOCKING_MEMORY_BARRIER();
			mpInstance = pInstance;
		}
	}
	else
	{
		OSL_DOUBLE_CHECKED_LOCKING_MEMORY_BARRIER();
	}

	// We throw an exception when for some strange reason no instance of
	// this class exists.
	if (mpInstance == NULL)
		throw ::com::sun::star::uno::RuntimeException(::rtl::OUString(
			RTL_CONSTASCII_USTRINGPARAM("com.sun.star.document.IndexedPropertyValues")),
			NULL);

	return *mpInstance;
}




FontProvider::FontProvider (void)
	: maFont(),
	  maMapMode()
{
}




FontProvider::~FontProvider (void)
{
}




void FontProvider::Invalidate (void)
{
	maFont.reset();
}




FontProvider::SharedFontPointer FontProvider::GetFont (const OutputDevice& rDevice)
{
	// Reset the font when the map mode has changed since its creation.
	if (maMapMode != rDevice.GetMapMode())
		maFont.reset();

	if (maFont.get() == NULL)
	{
		// Initialize the font from the application style settings.
		maFont.reset(new Font (Application::GetSettings().GetStyleSettings().GetAppFont()));
		maFont->SetTransparent(sal_True);
		maFont->SetWeight(WEIGHT_NORMAL);

		// Transform the point size to pixel size.
		MapMode aFontMapMode (MAP_POINT);
		Size aFontSize (rDevice.LogicToPixel(maFont->GetSize(), aFontMapMode));

		// Transform the font size to the logical coordinates of the device.
		maFont->SetSize (rDevice.PixelToLogic(aFontSize));

		// Remember the map mode of the given device to detect different
		// devices or modified zoom scales on future calls.
		maMapMode = rDevice.GetMapMode();
	}

	return maFont;
}

} } } // end of namespace ::sd::slidesorter::view
