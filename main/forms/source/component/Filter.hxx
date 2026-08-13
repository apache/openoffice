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



#ifndef FORMS_COMPONENT_FILTER_HXX
#define FORMS_COMPONENT_FILTER_HXX

#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/form/XBoundComponent.hpp>
#include <com/sun/star/util/XNumberFormatter.hpp>
#include <com/sun/star/awt/XTextComponent.hpp>
#include <com/sun/star/sdb/SQLContext.hpp>
#include <toolkit/controls/unocontrol.hxx>

#include <toolkit/helper/listenermultiplexer.hxx>
#include <cppuhelper/implbase5.hxx>
#include <comphelper/uno3.hxx>
#include <comphelper/componentcontext.hxx>
#include <cppuhelper/implbase4.hxx>
#include <connectivity/sqlparse.hxx>
#include <svx/ParseContext.hxx>

#include <hash_map>

//.........................................................................
namespace frm
{
//.........................................................................

	//=====================================================================
	// OFilterControl
	//=====================================================================
	typedef ::cppu::ImplHelper5	<	::com::sun::star::awt::XTextComponent
								,	::com::sun::star::awt::XFocusListener
								,	::com::sun::star::awt::XItemListener
								,	::com::sun::star::form::XBoundComponent
								,	::com::sun::star::lang::XInitialization
								>	OFilterControl_BASE;

	class OFilterControl	:public UnoControl
							,public OFilterControl_BASE
							,public ::svxform::OParseContextClient
	{
		TextListenerMultiplexer		m_aTextListeners;

		::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > 				m_xField;
		::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatter > 			m_xFormatter;
		::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection > 			    m_xConnection;
		::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData > 			m_xMetaData;
		::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >						m_xMessageParent;

        typedef ::std::hash_map< ::rtl::OUString, ::rtl::OUString, ::rtl::OUStringHash > MapString2String;
        MapString2String                m_aDisplayItemToValueItem;

		::rtl::OUString					m_aText;
		::connectivity::OSQLParser		m_aParser;
		sal_Int16						m_nControlClass;		// which kind of control do we use?
		sal_Bool						m_bFilterList : 1;
		sal_Bool						m_bMultiLine : 1;
		sal_Bool						m_bFilterListFilled : 1;

	private:
//		OFilterControl(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB);

		void implInitFilterList();

	public:
		OFilterControl(	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB );

		DECLARE_UNO3_AGG_DEFAULTS(OFilterControl,OWeakAggObject);
		::com::sun::star::uno::Any	SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType );

		virtual ::rtl::OUString	GetComponentServiceName();
		virtual void SAL_CALL	createPeer( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XToolkit > & rxToolkit, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer >  & rParentPeer );

	// ::com::sun::star::lang::XComponent
		virtual void SAL_CALL	dispose(void);

	// ::com::sun::star::awt::XTextComponent
		virtual void			SAL_CALL addTextListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTextListener > & l );
		virtual void			SAL_CALL removeTextListener( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTextListener > & l );
		virtual void			SAL_CALL setText( const ::rtl::OUString& aText );
		virtual void			SAL_CALL insertText( const ::com::sun::star::awt::Selection& rSel, const ::rtl::OUString& aText );
		virtual ::rtl::OUString	SAL_CALL getText();
		virtual ::rtl::OUString	SAL_CALL getSelectedText();
		virtual void			SAL_CALL setSelection( const ::com::sun::star::awt::Selection& aSelection );
		virtual ::com::sun::star::awt::Selection SAL_CALL getSelection();
		virtual sal_Bool		SAL_CALL isEditable();
		virtual void			SAL_CALL setEditable( sal_Bool bEditable );
		virtual void			SAL_CALL setMaxTextLen( sal_Int16 nLength );
		virtual sal_Int16		SAL_CALL getMaxTextLen();

	// ::com::sun::star::form::XBoundComponent
		virtual void			SAL_CALL addUpdateListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XUpdateListener > & /*l*/) {}
		virtual void			SAL_CALL removeUpdateListener(const ::com::sun::star::uno::Reference< ::com::sun::star::form::XUpdateListener > & /*l*/) {}
		virtual sal_Bool		SAL_CALL commit();

	// ::com::sun::star::lang::XEventListener
		virtual void			SAL_CALL disposing(const ::com::sun::star::lang::EventObject& Source);

	// ::com::sun::star::awt::XFocusListener
		virtual void			SAL_CALL focusGained(const ::com::sun::star::awt::FocusEvent& e);
        virtual void			SAL_CALL focusLost(const ::com::sun::star::awt::FocusEvent& e);

	// ::com::sun::star::awt::XItemListener
		virtual void			SAL_CALL itemStateChanged(const ::com::sun::star::awt::ItemEvent& rEvent);

	// ::com::sun::star::util::XInitialization
		virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

		// XServiceInfo
		virtual ::rtl::OUString SAL_CALL getImplementationName(  );
		virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

		// com::sun::star::lang::XServiceInfo - static version
		static	::rtl::OUString	SAL_CALL getImplementationName_Static();
		static	::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_Static();
		static	::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL Create( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory );

	protected:
		virtual void PrepareWindowDescriptor( ::com::sun::star::awt::WindowDescriptor& rDesc );
		virtual void ImplSetPeerProperty( const ::rtl::OUString& rPropName, const ::com::sun::star::uno::Any& rVal );

		sal_Bool ensureInitialized( );

		void displayException( const ::com::sun::star::sdb::SQLContext& _rExcept );
	};
//.........................................................................
}	// namespace frm
//.........................................................................

#endif // FORMS_COMPONENT_FILTER_HXX
