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


#ifndef CHART_TITLEWRAPPER_HXX
#define CHART_TITLEWRAPPER_HXX

#include "WrappedPropertySet.hxx"
#include "ReferenceSizePropertyProvider.hxx"
#include "Chart2ModelContact.hxx"
#include "ServiceMacros.hxx"
#include "TitleHelper.hxx"
#include <cppuhelper/implbase3.hxx>
#include <comphelper/uno3.hxx>
#include <cppuhelper/interfacecontainer.hxx>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/chart2/XTitle.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>

#include <boost/shared_ptr.hpp>

namespace chart
{
namespace wrapper
{

class TitleWrapper : public ::cppu::ImplInheritanceHelper3<
                      WrappedPropertySet
                    , com::sun::star::drawing::XShape
                    , com::sun::star::lang::XComponent
                    , com::sun::star::lang::XServiceInfo
                    >
                    , public ReferenceSizePropertyProvider
{
public:
    TitleWrapper( ::chart::TitleHelper::eTitleType eTitleType,
                  ::boost::shared_ptr< Chart2ModelContact > spChart2ModelContact );
	virtual ~TitleWrapper();

    /// XServiceInfo declarations
    APPHELPER_XSERVICEINFO_DECL()

    //ReferenceSizePropertyProvider
    virtual void updateReferenceSize();
    virtual ::com::sun::star::uno::Any getReferenceSize();
    virtual ::com::sun::star::awt::Size getCurrentSizeForReference();

protected:
    // ____ XShape ____
    virtual ::com::sun::star::awt::Point SAL_CALL getPosition();
    virtual void SAL_CALL setPosition( const ::com::sun::star::awt::Point& aPosition );
    virtual ::com::sun::star::awt::Size SAL_CALL getSize();
    virtual void SAL_CALL setSize( const ::com::sun::star::awt::Size& aSize );

    // ____ XShapeDescriptor (base of XShape) ____
    virtual ::rtl::OUString SAL_CALL getShapeType();

    // ____ XComponent ____
    virtual void SAL_CALL dispose();
    virtual void SAL_CALL addEventListener( const ::com::sun::star::uno::Reference<
                                            ::com::sun::star::lang::XEventListener >& xListener );
    virtual void SAL_CALL removeEventListener( const ::com::sun::star::uno::Reference<
                                               ::com::sun::star::lang::XEventListener >& aListener );

    // character properties have to be handled differently (via the XFormattedString elements)
    void getFastCharacterPropertyValue( sal_Int32 nHandle, ::com::sun::star::uno::Any& rValue );
    void setFastCharacterPropertyValue( sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue );

    // ____ WrappedPropertySet ____
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );

    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );

    virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState( const ::rtl::OUString& PropertyName );

    virtual void SAL_CALL setPropertyToDefault( const ::rtl::OUString& PropertyName );
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault( const ::rtl::OUString& aPropertyName );

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > getInnerPropertySet();

    virtual const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& getPropertySequence();
    virtual const std::vector< WrappedProperty* > createWrappedProperties();

    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > getFirstCharacterPropertySet();

private:
    ::com::sun::star::uno::Reference< ::com::sun::star::chart2::XTitle > getTitleObject();

private:
    ::boost::shared_ptr< Chart2ModelContact >   m_spChart2ModelContact;
	::cppu::OInterfaceContainerHelper           m_aEventListenerContainer;

    ::chart::TitleHelper::eTitleType        m_eTitleType;
};

} //  namespace wrapper
} //  namespace chart

// CHART_TITLEWRAPPER_HXX
#endif
