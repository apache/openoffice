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



#ifndef SD_UNO_OUTLINE_VIEW_HXX
#define SD_UNO_OUTLINE_VIEW_HXX

#include "DrawSubController.hxx"
#include <cppuhelper/basemutex.hxx>
#include <com/sun/star/lang/EventObject.hpp>


class SdPage;

namespace css = ::com::sun::star;

namespace sd {

class DrawController;
class OutlineViewShell;
class View;


/** This class implements the OutlineViewShell specific part of the controller.
*/
class SdUnoOutlineView
    : private cppu::BaseMutex,
      public DrawSubControllerInterfaceBase
{
public:
	SdUnoOutlineView (
        DrawController& rController,
        OutlineViewShell& rViewShell,
        View& rView) throw();
	virtual ~SdUnoOutlineView (void) throw();

    virtual void SAL_CALL disposing (void);


	// XSelectionSupplier

    virtual sal_Bool SAL_CALL select (
        const css::uno::Any& aSelection);

    virtual css::uno::Any SAL_CALL getSelection (void);

    virtual void SAL_CALL addSelectionChangeListener (
        const css::uno::Reference<css::view::XSelectionChangeListener>& rxListener);

    virtual void SAL_CALL removeSelectionChangeListener (
        const css::uno::Reference<css::view::XSelectionChangeListener>& rxListener);


	// XDrawView

    virtual void SAL_CALL setCurrentPage (
        const css::uno::Reference<css::drawing::XDrawPage >& xPage);

    virtual css::uno::Reference<css::drawing::XDrawPage> SAL_CALL getCurrentPage (void);


    // XFastPropertySet

    virtual void SAL_CALL setFastPropertyValue (
        sal_Int32 nHandle,
        const css::uno::Any& rValue);

    virtual css::uno::Any SAL_CALL getFastPropertyValue (
        sal_Int32 nHandle);

    // lang::XEventListener
    virtual void SAL_CALL
        disposing (const ::com::sun::star::lang::EventObject& rEventObject);

	// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName(  );
	virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

private:
    DrawController& mrController;
    OutlineViewShell& mrOutlineViewShell;
    sd::View& mrView;
};

} // end of namespace sd

#endif
