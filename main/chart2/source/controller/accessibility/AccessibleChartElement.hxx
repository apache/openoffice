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


#ifndef _CHART2_ACCESSIBLECHARTELEMENT_HXX_
#define _CHART2_ACCESSIBLECHARTELEMENT_HXX_

#include "AccessibleBase.hxx"
#include <com/sun/star/chart2/XChartDocument.hpp>
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleContext.hpp>
#include <com/sun/star/accessibility/XAccessibleComponent.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/document/XEventListener.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/accessibility/XAccessibleEventBroadcaster.hpp>
#include <com/sun/star/accessibility/XAccessibleExtendedComponent.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <comphelper/accessibleeventnotifier.hxx>
#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/interfacecontainer.hxx>
#include <unotools/accessiblestatesethelper.hxx>

#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

class SfxItemSet;
class SdrObject;

namespace chart
{

/** Base class for all Chart Accessibility objects except the root node (see AccessibleChartView)

    This class contains a reference to the ChartModel, thus, components can easily access all core functionality.

    Usage Instructions:

    <ul>
     <li>define the getAccessibleName() method of XAccessibleContext</li>
     <li>set the ChartModel using SetChartModel() for the first node before
         creating any children</li>
     <li>overload UpdateChildren()</li>
    </ul>
 */

namespace impl
{
typedef ::cppu::ImplInheritanceHelper1<
        AccessibleBase,
        ::com::sun::star::accessibility::XAccessibleExtendedComponent
        > AccessibleChartElement_Base;
}

class AccessibleChartElement :
    public impl::AccessibleChartElement_Base
{
public:
    AccessibleChartElement( const AccessibleElementInfo & rAccInfo,
                            bool bMayHaveChildren,
                            bool bAlwaysTransparent = false );
    virtual ~AccessibleChartElement();

    // ________ AccessibleBase ________
    virtual bool ImplUpdateChildren();
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessible >
        ImplGetAccessibleChildById( sal_Int32 i ) const;
    virtual sal_Int32 ImplGetAccessibleChildCount() const;

    // ________ XAccessibleContext ________
    virtual ::rtl::OUString SAL_CALL getAccessibleName();
    virtual ::rtl::OUString SAL_CALL getAccessibleDescription();

    // ________ XAccessibleExtendedComponent ________
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFont > SAL_CALL getFont();
    virtual ::rtl::OUString SAL_CALL getTitledBorderText();
    virtual ::rtl::OUString SAL_CALL getToolTipText();

    // the following interface is implemented in AccessibleBase, however it is
    // also a (non-virtual) base class of XAccessibleExtendedComponent Thus
    // these methods have to be overloaded and forward to AccessibleBase

    // ________ XAccessibleComponent ________
    virtual sal_Bool SAL_CALL containsPoint( const ::com::sun::star::awt::Point& aPoint );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > SAL_CALL getAccessibleAtPoint( const ::com::sun::star::awt::Point& aPoint );
    virtual ::com::sun::star::awt::Rectangle SAL_CALL getBounds();
    virtual ::com::sun::star::awt::Point SAL_CALL getLocation();
    virtual ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen();
    virtual ::com::sun::star::awt::Size SAL_CALL getSize();
    virtual void SAL_CALL grabFocus();
    virtual sal_Int32 SAL_CALL getForeground();
    virtual sal_Int32 SAL_CALL getBackground();

    // ________ XServiceInfo ________
    virtual ::rtl::OUString SAL_CALL getImplementationName();

private:
    bool                m_bHasText;
    ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessibleContext >
                        m_xTextHelper;

    void InitTextEdit();
};

}  // namespace chart

#endif
