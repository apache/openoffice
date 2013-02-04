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



#ifndef SVX_DBTOOLSCLIENT_HXX
#define SVX_DBTOOLSCLIENT_HXX

#ifndef CONNECTIVITY_VIRTUAL_DBTOOLS_HXX
#include <connectivity/virtualdbtools.hxx>
#endif
#ifndef _OSL_MODULE_H_ 
#include <osl/module.h>
#endif
#include <bf_svtools/bf_solar.h>
namespace binfilter {
//........................................................................
namespace svxform
{
//........................................................................

	//====================================================================
	//= ODbtoolsClient
	//====================================================================
	/** base class for classes which want to use dbtools features with load-on-call
		of the dbtools lib.
	*/
	class ODbtoolsClient
	{
	private:
		static	::osl::Mutex	s_aMutex;
		static	sal_Int32		s_nClients;
		static oslModule		s_hDbtoolsModule;
		static ::connectivity::simple::createDataAccessToolsFactoryFunction
								s_pFactoryCreationFunc;
		//add by BerryJia for fixing Bug97420 Time:2002-9-12-11:00(PRC time)
		mutable BOOL            m_bCreateAlready;

	private:
		mutable ::rtl::Reference< ::connectivity::simple::IDataAccessToolsFactory >	m_xDataAccessFactory;

	protected:
		const ::rtl::Reference< ::connectivity::simple::IDataAccessToolsFactory >&
			getFactory() const { return m_xDataAccessFactory; }

	protected:
		ODbtoolsClient();
		~ODbtoolsClient();
		//add by BerryJia for fixing Bug97420 Time:2002-9-12-11:00(PRC time)

	private:
	};

	//====================================================================
	//= OStaticDataAccessTools
	//====================================================================
	class OStaticDataAccessTools : public ODbtoolsClient
	{
	protected:
		mutable ::rtl::Reference< ::connectivity::simple::IDataAccessTools >	m_xDataAccessTools;
		//add by BerryJia for fixing Bug97420 Time:2002-9-12-11:00(PRC time)
		void create() const;

		void checkIfLoaded() const;

	public:
		OStaticDataAccessTools();

		::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection> getRowSetConnection(
				const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet>& _rxRowSet) 
				const SAL_THROW ( (::com::sun::star::uno::RuntimeException) );

		// ------------------------------------------------

		// ------------------------------------------------
		::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDataSource > getDataSource(
				const ::rtl::OUString& _rsRegisteredName,
				const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rxFactory
			) const;

		// ------------------------------------------------
		/** check if the property "Privileges" supports ::com::sun::star::sdbcx::Privilege::INSERT
			@param		_rxCursorSet	the property set
		*/
		sal_Bool canInsert(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _rxCursorSet) const;

		// ------------------------------------------------
		/** check if the property "Privileges" supports ::com::sun::star::sdbcx::Privilege::UPDATE
			@param		_rxCursorSet	the property set
		*/
		sal_Bool canUpdate(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _rxCursorSet) const;

		// ------------------------------------------------
		/** check if the property "Privileges" supports ::com::sun::star::sdbcx::Privilege::DELETE
			@param		_rxCursorSet	the property set
		*/
		sal_Bool canDelete(const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet>& _rxCursorSet) const;

		// ------------------------------------------------
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >
			getFieldsByCommandDescriptor(
				const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection,
				const sal_Int32 _nCommandType,
				const ::rtl::OUString& _rCommand,
				::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& _rxKeepFieldsAlive,
				::dbtools::SQLExceptionInfo* _pErrorInfo = NULL
			)	SAL_THROW( ( ) );

		// ------------------------------------------------
		virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >
			getFieldNamesByCommandDescriptor(
				const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _rxConnection,
				const sal_Int32 _nCommandType,
				const ::rtl::OUString& _rCommand,
				::dbtools::SQLExceptionInfo* _pErrorInfo = NULL
			)	SAL_THROW( ( ) );
	};
//........................................................................
}	// namespace svxform
//........................................................................

}//end of namespace binfilter
#endif // SVX_DBTOOLSCLIENT_HXX


