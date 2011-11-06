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



#ifndef SVX_SQLPARSERCLIENT_HXX
#define SVX_SQLPARSERCLIENT_HXX

#ifndef SVX_DBTOOLSCLIENT_HXX
#include "dbtoolsclient.hxx"
#endif
#ifndef SVX_QUERYDESIGNCONTEXT_HXX
#include "ParseContext.hxx"
#endif
namespace binfilter {

//........................................................................
namespace svxform
{
//........................................................................

	//====================================================================
	//= OSQLParserClient
	//====================================================================
	class OSQLParserClient : public ODbtoolsClient
		,public ::binfilter::svxform::OParseContextClient//STRIP008 							,public ::svxform::OParseContextClient
	{
	private:
		//add by BerryJia for fixing Bug97420 Time:2002-9-12-11:00(PRC time)
		::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xORB;

	protected:
		mutable ::rtl::Reference< ::connectivity::simple::ISQLParser >	m_xParser;

	protected:
		OSQLParserClient(
			const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxORB);
		//add by BerryJia for fixing Bug97420 Time:2002-9-12-11:00(PRC time)

	protected:
	};

//........................................................................
}	// namespace svxform
//........................................................................

}//end of namespace binfilter
#endif // SVX_SQLPARSERCLIENT_HXX


