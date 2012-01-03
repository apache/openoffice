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



#ifndef FORMS_COMPONENT_CLONEABLE_HXX
#include "cloneable.hxx"
#endif

#ifndef _COM_SUN_STAR_UTIL_XCLONEABLE_HPP_
#include <com/sun/star/util/XCloneable.hpp>
#endif
#ifndef _COMPHELPER_UNO3_HXX_ 
#include <comphelper/uno3.hxx>
#endif
#ifndef _TOOLS_DEBUG_HXX 
#include <tools/debug.hxx>
#endif
namespace binfilter {

//.........................................................................
namespace frm
{
//.........................................................................

	using namespace ::comphelper;
	using namespace ::com::sun::star::uno;
	using namespace ::com::sun::star::util;

	//====================================================================
	//= OCloneableAggregation
	//====================================================================
	//------------------------------------------------------------------
	Reference< XAggregation > OCloneableAggregation::createAggregateClone( const OCloneableAggregation* _pOriginal )
	{
		Reference< XCloneable > xAggregateCloneable;	// will be the aggregates XCloneable
		Reference< XAggregation > xAggregateClone;		// will be the aggregates clone

		if ( query_aggregation( _pOriginal->m_xAggregate, xAggregateCloneable ) )
		{
			xAggregateClone = xAggregateClone.query( xAggregateCloneable->createClone() );
			DBG_ASSERT( xAggregateClone.is(), "OCloneableAggregation::createAggregateClone: invalid clone returned by the aggregate!" );
		}
		else
			DBG_ASSERT( !_pOriginal->m_xAggregate.is(), "OCloneableAggregation::createAggregateClone: aggregate is not cloneable!" );

		return xAggregateClone;
	}

//.........................................................................
}	// namespace frm
//.........................................................................

}
