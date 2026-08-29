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


#ifndef CHART_CHARTDOCUMENTWRAPPER_HXX
#define CHART_CHARTDOCUMENTWRAPPER_HXX

#include "WrappedPropertySet.hxx"
//#include "OPropertySet.hxx"
#include "ServiceMacros.hxx"
#include <com/sun/star/chart2/XChartDocument.hpp>
#include <com/sun/star/chart/XChartDocument.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uno/XAggregation.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/util/XRefreshable.hpp>
#include <cppuhelper/implbase5.hxx>
#include <osl/mutex.hxx>
#include <unotools/eventlisteneradapter.hxx>
#include <comphelper/uno3.hxx>

#include <boost/shared_ptr.hpp>

namespace chart
{

namespace wrapper
{

class Chart2ModelContact;

class ChartDocumentWrapper_Base : public ::cppu::ImplInheritanceHelper5
                                < WrappedPropertySet
                                , ::com::sun::star::chart::XChartDocument
                                , ::com::sun::star::drawing::XDrawPageSupplier
                                , ::com::sun::star::lang::XMultiServiceFactory
                                , ::com::sun::star::lang::XServiceInfo
                                , ::com::sun::star::uno::XAggregation
                                >
{
};

class ChartDocumentWrapper : public ChartDocumentWrapper_Base
                           , public ::utl::OEventListenerAdapter
{
public:
	explicit ChartDocumentWrapper(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::uno::XComponentContext > & xContext );
	virtual ~ChartDocumentWrapper();

    /// XServiceInfo declarations
    APPHELPER_XSERVICEINFO_DECL()
    APPHELPER_SERVICE_FACTORY_HELPER(ChartDocumentWrapper)

    void setAddIn( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XRefreshable >& xAddIn );
    ::com::sun::star::uno::Reference< ::com::sun::star::util::XRefreshable > getAddIn() const;

    void setUpdateAddIn( sal_Bool bUpdateAddIn );
    sal_Bool getUpdateAddIn() const;

    void setBaseDiagram( const rtl::OUString& rBaseDiagram );
    rtl::OUString getBaseDiagram() const;

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes > getAdditionalShapes() const;

    ::com::sun::star::uno::Reference<
            ::com::sun::star::drawing::XDrawPage > impl_getDrawPage() const;

protected:

    // ____ chart::XChartDocument ____
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShape > SAL_CALL getTitle();
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShape > SAL_CALL getSubTitle();
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XShape > SAL_CALL getLegend();
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::beans::XPropertySet > SAL_CALL getArea();
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::chart::XDiagram > SAL_CALL getDiagram();
    virtual void SAL_CALL setDiagram( const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::chart::XDiagram >& xDiagram );
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::chart::XChartData > SAL_CALL getData();
    virtual void SAL_CALL attachData( const ::com::sun::star::uno::Reference<
                                      ::com::sun::star::chart::XChartData >& xData );

    // ____ XModel ____
    virtual sal_Bool SAL_CALL attachResource( const ::rtl::OUString& URL, const ::com::sun::star::uno::Sequence<
                                              ::com::sun::star::beans::PropertyValue >& Arguments );
    virtual ::rtl::OUString SAL_CALL getURL();
    virtual ::com::sun::star::uno::Sequence<
        ::com::sun::star::beans::PropertyValue > SAL_CALL getArgs();
    virtual void SAL_CALL connectController( const ::com::sun::star::uno::Reference<
                                             ::com::sun::star::frame::XController >& Controller );
    virtual void SAL_CALL disconnectController( const ::com::sun::star::uno::Reference<
                                                ::com::sun::star::frame::XController >& Controller );
    virtual void SAL_CALL lockControllers();
    virtual void SAL_CALL unlockControllers();
    virtual sal_Bool SAL_CALL hasControllersLocked();
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::frame::XController > SAL_CALL getCurrentController();
    virtual void SAL_CALL setCurrentController( const ::com::sun::star::uno::Reference<
                                                ::com::sun::star::frame::XController >& Controller );
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::uno::XInterface > SAL_CALL getCurrentSelection();

    // ____ XComponent ____
    virtual void SAL_CALL dispose();
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference<
                                            ::com::sun::star::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference<
                                               ::com::sun::star::lang::XEventListener >& aListener );

    // ____ XInterface (for new interfaces) ____
    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType );

    // ____ ::utl::OEventListenerAdapter ____
    virtual void _disposing( const ::com::sun::star::lang::EventObject& rSource );

    // ____ XDrawPageSupplier ____
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::drawing::XDrawPage > SAL_CALL getDrawPage();

    // ____ XMultiServiceFactory ____
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::uno::XInterface > SAL_CALL createInstance( const ::rtl::OUString& aServiceSpecifier );
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::uno::XInterface > SAL_CALL createInstanceWithArguments(
                const ::rtl::OUString& ServiceSpecifier,
                const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& Arguments );
    virtual ::com::sun::star::uno::Sequence<
            ::rtl::OUString > SAL_CALL getAvailableServiceNames();

    // ____ XAggregation ____
    virtual void SAL_CALL setDelegator(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::uno::XInterface >& rDelegator );
    virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type& aType );

    // ____ WrappedPropertySet ____
    virtual const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& getPropertySequence();
    virtual const std::vector< WrappedProperty* > createWrappedProperties();
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > getInnerPropertySet();

private: //methods
    void impl_resetAddIn();

private: //member
    ::boost::shared_ptr< Chart2ModelContact >   m_spChart2ModelContact;

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >   m_xDelegator;

    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >   m_xTitle;
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >   m_xSubTitle;
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >   m_xLegend;
    ::com::sun::star::uno::Reference< ::com::sun::star::chart::XChartData > m_xChartData;
    ::com::sun::star::uno::Reference< ::com::sun::star::chart::XDiagram >   m_xDiagram;
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > m_xArea;

    ::com::sun::star::uno::Reference< ::com::sun::star::util::XRefreshable > m_xAddIn;
    rtl::OUString   m_aBaseDiagram;
    sal_Bool        m_bUpdateAddIn;

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >   m_xChartView;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>
			                                                                m_xShapeFactory;

    bool                                                                    m_bIsDisposed;
};

} //  namespace wrapper
} //  namespace chart

// CHART_CHARTDOCUMENT_HXX
#endif
