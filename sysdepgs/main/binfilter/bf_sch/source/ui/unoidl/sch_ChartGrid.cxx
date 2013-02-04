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


#include "ChartGrid.hxx"

#include "mapprov.hxx"

// header for class OGuard
#ifndef _VOS_MUTEX_HXX_
#include <vos/mutex.hxx>
#endif
// header for class Application
#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif
#ifndef _RTL_UUID_H_
#include <rtl/uuid.h>
#endif
namespace binfilter {

using namespace ::com::sun::star;

ChartGrid::ChartGrid( ChartModel* pModel, sal_Int32 nObjectId ) :
		ChXChartObject( CHMAP_LINE, pModel, nObjectId )
{
}

ChartGrid::~ChartGrid()
{}

// XServiceInfo
::rtl::OUString SAL_CALL ChartGrid::getImplementationName()
	throw( uno::RuntimeException )
{
	return ::rtl::OUString::createFromAscii( "ChartGrid" );
}

uno::Sequence< ::rtl::OUString > SAL_CALL ChartGrid::getSupportedServiceNames()
	throw( uno::RuntimeException )
{
	::vos::OGuard aGuard( Application::GetSolarMutex() );

	uno::Sequence< ::rtl::OUString > aSeq( 3 );
	aSeq[ 0 ] = ::rtl::OUString::createFromAscii( "com.sun.star.chart.ChartGrid" );
	aSeq[ 1 ] = ::rtl::OUString::createFromAscii( "com.sun.star.drawing.LineProperties" );
	aSeq[ 2 ] = ::rtl::OUString::createFromAscii( "com.sun.star.xml.UserDefinedAttributeSupplier" );

	return aSeq;
}

// XTypeProvider
uno::Sequence< sal_Int8 > SAL_CALL ChartGrid::getImplementationId()
	throw( uno::RuntimeException )
{
	static uno::Sequence< sal_Int8 > aId;
	if( aId.getLength() == 0 )
	{
		aId.realloc( 16 );
		rtl_createUuid( (sal_uInt8 *)aId.getArray(), 0, sal_True );
	}
	return aId;
}

// XShapeDescriptor
::rtl::OUString SAL_CALL ChartGrid::getShapeType()
	throw( uno::RuntimeException )
{
	return ::rtl::OUString::createFromAscii( "com.sun.star.chart.ChartGrid" );
}

// XUnoTunnel
sal_Int64 SAL_CALL ChartGrid::getSomething( const uno::Sequence< sal_Int8 >& aIdentifier )
	throw( uno::RuntimeException )
{
	if( aIdentifier.getLength() == 16 && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
																 aIdentifier.getConstArray(), 16 ) )
	{
		return (sal_Int64)this;
	}
	return ChXChartObject::getSomething( aIdentifier );
}

// helpers for XUnoTunnel 
const uno::Sequence< sal_Int8 > & ChartGrid::getUnoTunnelId() throw()
{
	static uno::Sequence< sal_Int8 > * pSeq = 0;
	if( !pSeq )
	{
		osl::Guard< osl::Mutex > aGuard( osl::Mutex::getGlobalMutex() );
		if( !pSeq )
		{
			static uno::Sequence< sal_Int8 > aSeq( 16 );
			rtl_createUuid( (sal_uInt8*)aSeq.getArray(), 0, sal_True );
			pSeq = &aSeq;
		}
	}
	return *pSeq;
}

}
