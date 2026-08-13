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


#ifndef TOOLKIT_TOOLKIT_CONTROLS_TKSCROLLBAR_HXX
#define TOOLKIT_TOOLKIT_CONTROLS_TKSCROLLBAR_HXX

#include <toolkit/controls/unocontrolmodel.hxx>
#include <toolkit/helper/servicenames.hxx>
#include <toolkit/controls/unocontrolbase.hxx>
#include <toolkit/helper/macros.hxx>
#include <com/sun/star/awt/XScrollBar.hpp>
#include <com/sun/star/awt/XAdjustmentListener.hpp>
#include <com/sun/star/awt/AdjustmentType.hpp>

//........................................................................
namespace toolkit
{
//........................................................................

	//====================================================================
	//= UnoControlScrollBarModel
	//====================================================================
    class UnoControlScrollBarModel : public UnoControlModel
    {
    protected:
	    ::com::sun::star::uno::Any		ImplGetDefaultValue( sal_uInt16 nPropId ) const;
	    ::cppu::IPropertyArrayHelper&	SAL_CALL getInfoHelper();

    public:
						    UnoControlScrollBarModel( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& i_factory );
						    UnoControlScrollBarModel( const UnoControlScrollBarModel& rModel ) : UnoControlModel( rModel ) {;}

	    UnoControlModel*	Clone() const { return new UnoControlScrollBarModel( *this ); }

	    // ::com::sun::star::beans::XMultiPropertySet
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  );

	    // ::com::sun::star::io::XPersistObject
        ::rtl::OUString SAL_CALL getServiceName();

	    // XServiceInfo
	    DECLIMPL_SERVICEINFO_DERIVED( UnoControlScrollBarModel, UnoControlModel, szServiceName2_UnoControlScrollBarModel )
    };

    //====================================================================
	//= UnoControlScrollBarModel
	//====================================================================
    class UnoScrollBarControl :	public UnoControlBase,
							    public ::com::sun::star::awt::XAdjustmentListener,
							    public ::com::sun::star::awt::XScrollBar
    {
    private:
	    AdjustmentListenerMultiplexer maAdjustmentListeners;

    public:
								    UnoScrollBarControl( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& i_factory );
	    ::rtl::OUString				GetComponentServiceName();

	    ::com::sun::star::uno::Any	SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) { return UnoControlBase::queryInterface(rType); }
        ::com::sun::star::uno::Any	SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType );
	    void						SAL_CALL acquire() throw()	{ OWeakAggObject::acquire(); }
	    void						SAL_CALL release() throw()	{ OWeakAggObject::release(); }
        void SAL_CALL createPeer( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XToolkit >& Toolkit, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer >& Parent );
        void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) { UnoControlBase::disposing( Source ); }
        void SAL_CALL dispose(  );

        // ::com::sun::star::lang::XTypeProvider
	    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes();
	    ::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId();

	    // ::com::sun::star::awt::XAdjustmentListener
        void SAL_CALL adjustmentValueChanged( const ::com::sun::star::awt::AdjustmentEvent& rEvent );

	    // ::com::sun::star::awt::XScrollBar
        void SAL_CALL addAdjustmentListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XAdjustmentListener >& l );
        void SAL_CALL removeAdjustmentListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XAdjustmentListener >& l );
        void SAL_CALL setValue( sal_Int32 n );
        void SAL_CALL setValues( sal_Int32 nValue, sal_Int32 nVisible, sal_Int32 nMax );
        sal_Int32 SAL_CALL getValue(  );
        void SAL_CALL setMaximum( sal_Int32 n );
        sal_Int32 SAL_CALL getMaximum(  );
        void SAL_CALL setLineIncrement( sal_Int32 n );
        sal_Int32 SAL_CALL getLineIncrement(  );
        void SAL_CALL setBlockIncrement( sal_Int32 n );
        sal_Int32 SAL_CALL getBlockIncrement(  );
        void SAL_CALL setVisibleSize( sal_Int32 n );
        sal_Int32 SAL_CALL getVisibleSize(  );
        void SAL_CALL setOrientation( sal_Int32 n );
        sal_Int32 SAL_CALL getOrientation(  );

	    // ::com::sun::star::lang::XServiceInfo
	    DECLIMPL_SERVICEINFO_DERIVED( UnoScrollBarControl, UnoControlBase, szServiceName2_UnoControlScrollBar )
    };



//........................................................................
} // namespacetoolkit
//........................................................................

#endif // TOOLKIT_TOOLKIT_CONTROLS_TKSCROLLBAR_HXX
