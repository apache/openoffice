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



#ifndef XMLOFF_SOURCE_FORMS_GRIDCOLUMNPROPTRANSLATOR_HXX
#define XMLOFF_SOURCE_FORMS_GRIDCOLUMNPROPTRANSLATOR_HXX

/** === begin UNO includes === **/
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
/** === end UNO includes === **/
#include <cppuhelper/implbase2.hxx>

//........................................................................
namespace xmloff
{
//........................................................................

	//====================================================================
	//= OGridColumnPropertyTranslator
	//====================================================================
    typedef ::cppu::WeakImplHelper2 <   ::com::sun::star::beans::XPropertySet
                                    ,   ::com::sun::star::beans::XMultiPropertySet
                                    >   OGridColumnPropertyTranslator_Base;
    class OGridColumnPropertyTranslator : public OGridColumnPropertyTranslator_Base
	{
    private:
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XMultiPropertySet >
                m_xGridColumn;

    public:
        OGridColumnPropertyTranslator(
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XMultiPropertySet >& _rxGridColumn
        );

    protected:
        virtual ~OGridColumnPropertyTranslator();

        // XPropertySet
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );
        virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue );
        virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName );
        virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener );
        virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener );
        virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );
        virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener );

        // XMultiPropertySet
        virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues );
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames );
        virtual void SAL_CALL addPropertiesChangeListener( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
        virtual void SAL_CALL removePropertiesChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
        virtual void SAL_CALL firePropertiesChangeEvent( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener );
	};

//........................................................................
} // namespace xmloff
//........................................................................

#endif // XMLOFF_SOURCE_FORMS_GRIDCOLUMNPROPTRANSLATOR_HXX
