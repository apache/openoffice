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




#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _REFRESH_LISTENER_CONTAINER_HXX_
#include <RefreshListenerContainer.hxx>
#endif

#ifndef _COM_SUN_STAR_UTIL_XREFRESHLISTENER_HPP_
#include <com/sun/star/util/XRefreshListener.hpp>
#endif
namespace binfilter {

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::util;

SwRefreshListenerContainer::SwRefreshListenerContainer( uno::XInterface* pxParent)
: SwEventListenerContainer ( pxParent )
{
}

void SwRefreshListenerContainer::Refreshed ()
{
	if(!pListenerArr)
		return;

	lang::EventObject aObj(pxParent);
	for(sal_uInt16 i = 0, nEnd = pListenerArr->Count(); i < nEnd ; i++)
	{
		Reference < XRefreshListener > xRefreshListener = Reference < XRefreshListener > ( *pListenerArr->GetObject(i), UNO_QUERY );
		xRefreshListener->refreshed(aObj);
	}
}
}
