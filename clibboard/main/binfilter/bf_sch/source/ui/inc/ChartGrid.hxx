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


#ifndef _CHARTGRID_HXX_
#define _CHARTGRID_HXX_

#include "ChXChartObject.hxx"
namespace binfilter {

class ChartGrid : public ChXChartObject
{
public:
	ChartGrid( ChartModel* pModel, sal_Int32 nObjectId );
	virtual ~ChartGrid();

	// helpers for XUnoTunnel
	static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId() throw();

	// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName()
		throw( ::com::sun::star::uno::RuntimeException );
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames()
		throw( ::com::sun::star::uno::RuntimeException );

	// XTypeProvider
    virtual ::com::sun::star::uno::Sequence< sal_Int8 > SAL_CALL getImplementationId()
		throw( ::com::sun::star::uno::RuntimeException );

	// XShapeDescriptor
    virtual ::rtl::OUString SAL_CALL getShapeType() throw( ::com::sun::star::uno::RuntimeException );

	// XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier )
		throw( ::com::sun::star::uno::RuntimeException );

};

} //namespace binfilter
#endif	// _CHARTGRID_HXX_
