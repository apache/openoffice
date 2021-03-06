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



#ifndef _XMLOFF_OASIS2OOO_HXX
#define _XMLOFF_OASIS2OOO_HXX

#include "ActionMapTypesOASIS.hxx"
#ifndef _XMLOFF_TRANSFORMERBASE_HXX_
#include "TransformerBase.hxx"
#endif

class XMLTransformerOASISEventMap_Impl;

class Oasis2OOoTransformer : public XMLTransformerBase
{
	XMLTransformerActions		*m_aActions[MAX_OASIS_ACTIONS];
	XMLTransformerOASISEventMap_Impl *m_pEventMap;
	XMLTransformerOASISEventMap_Impl *m_pFormEventMap;

protected:

	virtual XMLTransformerContext *CreateUserDefinedContext( 
									  const TransformerAction_Impl& rAction,
									  const ::rtl::OUString& rQName,
		   							  sal_Bool bPersistent=sal_False );

	virtual XMLTransformerActions *GetUserDefinedActions( sal_uInt16 n );

public:
	Oasis2OOoTransformer () throw();
	virtual ~Oasis2OOoTransformer() throw();

	static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId() throw();

	// XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException);

	// XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);

	virtual ::rtl::OUString GetEventName( const ::rtl::OUString& rName,
		   								  sal_Bool bForm );
};

#endif	//  _XMLOFF_OASIS2OOO_HXX
