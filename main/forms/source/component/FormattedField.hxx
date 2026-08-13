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



#ifndef _FORMS_FORMATTEDFIELD_HXX_
#define _FORMS_FORMATTEDFIELD_HXX_

#include "EditBase.hxx"
#include <comphelper/propmultiplex.hxx>
#include <cppuhelper/implbase1.hxx>
#include "errorbroadcaster.hxx"

//.........................................................................
namespace frm
{

	//==================================================================
	//= OFormattedModel
	//==================================================================

	class OFormattedModel
					:public OEditBaseModel
					,public OErrorBroadcaster
	{
		// das Original, falls ich die Format-Properties meines aggregierten Models gefaket, d.h. von dem Feld, an das
		// ich gebunden bin, weitergereicht habe (nur gueltig wenn loaded)
		::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatsSupplier>	m_xOriginalFormatter;
		::com::sun::star::util::Date		m_aNullDate;
		::com::sun::star::uno::Any			m_aSaveValue;

		sal_Int32							m_nFieldType;
		sal_Int16							m_nKeyType;
		sal_Bool							m_bOriginalNumeric		: 1,
											m_bNumeric				: 1;	// analog fuer TreatAsNumeric-Property

	protected:
		::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatsSupplier>  calcDefaultFormatsSupplier() const;
		::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatsSupplier>  calcFormFormatsSupplier() const;
		::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatsSupplier>  calcFormatsSupplier() const;

		DECLARE_DEFAULT_LEAF_XTOR( OFormattedModel );

		friend class OFormattedFieldWrapper;

	protected:
	// XInterface
		DECLARE_UNO3_AGG_DEFAULTS( OFormattedModel, OEditBaseModel );

	// XTypeProvider
		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type> _getTypes();

	// XAggregation
		virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation(const ::com::sun::star::uno::Type& _rType);

	// OComponentHelper
		virtual void SAL_CALL disposing();

	// XServiceInfo
		IMPLEMENTATION_NAME(OFormattedModel);
		virtual StringSequence SAL_CALL getSupportedServiceNames() throw();

	// XPersistObject
		virtual void SAL_CALL write(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream>& _rxOutStream);
		virtual void SAL_CALL read(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream>& _rxInStream);
		virtual ::rtl::OUString SAL_CALL getServiceName();

	// XPropertySet
		virtual void SAL_CALL getFastPropertyValue(::com::sun::star::uno::Any& rValue, sal_Int32 nHandle ) const;
		virtual sal_Bool SAL_CALL convertFastPropertyValue(::com::sun::star::uno::Any& rConvertedValue, ::com::sun::star::uno::Any& rOldValue,
											  sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue );
		virtual void SAL_CALL setFastPropertyValue_NoBroadcast(sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue);

	// XLoadListener
		virtual void SAL_CALL loaded(const ::com::sun::star::lang::EventObject& rEvent);

	// XPropertyState
		void setPropertyToDefaultByHandle(sal_Int32 nHandle);
		::com::sun::star::uno::Any getPropertyDefaultByHandle(sal_Int32 nHandle) const;

		void SAL_CALL setPropertyToDefault(const ::rtl::OUString& aPropertyName);
		::com::sun::star::uno::Any SAL_CALL getPropertyDefault( const ::rtl::OUString& aPropertyName );

        // OControlModel's property handling
	    virtual void describeFixedProperties(
			::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rProps
        ) const;
        virtual void describeAggregateProperties(
		    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rAggregateProps
        ) const;

	// XPropertyChangeListener
		virtual void _propertyChanged(const ::com::sun::star::beans::PropertyChangeEvent& evt);

    // prevent method hiding
        using OEditBaseModel::disposing;
        using OEditBaseModel::getFastPropertyValue;

	protected:
		virtual sal_uInt16 getPersistenceFlags() const;
			// as we have an own version handling for persistence

        // OBoundControlModel overridables
        virtual ::com::sun::star::uno::Any
                            translateDbColumnToControlValue( );
        virtual sal_Bool    commitControlValueToDbColumn( bool _bPostReset );

        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >
                            getSupportedBindingTypes();
        virtual ::com::sun::star::uno::Any
                            translateExternalValueToControlValue( const ::com::sun::star::uno::Any& _rExternalValue ) const;
        virtual ::com::sun::star::uno::Any
                            translateControlValueToExternalValue( ) const;
        virtual void onConnectedExternalValue( );

        virtual ::com::sun::star::uno::Any
                            getDefaultForReset() const;
	    virtual void        resetNoBroadcast();

		virtual void        onConnectedDbColumn( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxForm );
		virtual void        onDisconnectedDbColumn();

    private:
		DECLARE_XCLONEABLE();

		void implConstruct();

        void    updateFormatterNullDate();
	};

	//==================================================================
	//= OFormattedControl
	//==================================================================
	typedef ::cppu::ImplHelper1< ::com::sun::star::awt::XKeyListener> OFormattedControl_BASE;
	class OFormattedControl :	 public OBoundControl
								,public OFormattedControl_BASE
	{
		sal_uInt32				m_nKeyEvent;

	public:
		OFormattedControl(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rxFactory);
		virtual ~OFormattedControl();

		DECLARE_UNO3_AGG_DEFAULTS(OFormattedControl, OBoundControl);
		virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation(const ::com::sun::star::uno::Type& _rType);

		virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type> _getTypes();

	// ::com::sun::star::lang::XServiceInfo
		IMPLEMENTATION_NAME(OFormattedControl);
		virtual StringSequence SAL_CALL getSupportedServiceNames() throw();

	// ::com::sun::star::lang::XEventListener
		virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& _rSource);

	// ::com::sun::star::awt::XKeyListener
		virtual void SAL_CALL keyPressed(const ::com::sun::star::awt::KeyEvent& e);
		virtual void SAL_CALL keyReleased(const ::com::sun::star::awt::KeyEvent& e);

	// ::com::sun::star::awt::XControl
		virtual void SAL_CALL setDesignMode(sal_Bool bOn);

    // disambiguation
        using OBoundControl::disposing;

	private:
		DECL_LINK( OnKeyPressed, void* );
	};

//.........................................................................
}
//.........................................................................

#endif // _FORMS_FORMATTEDFIELD_HXX_
