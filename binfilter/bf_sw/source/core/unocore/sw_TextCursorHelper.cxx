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


#ifndef _SW_TEXTCURSORHELPER_HXX
#include "TextCursorHelper.hxx"
#endif

#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

#include "unoobj.hxx"
namespace binfilter {

using namespace ::com::sun::star;
/* -----------------------------03.03.03 11:07--------------------------------

 ---------------------------------------------------------------------------*/
const uno::Sequence< sal_Int8 > & OTextCursorHelper::getUnoTunnelId()
{
	static uno::Sequence< sal_Int8 > aSeq = ::binfilter::CreateUnoTunnelId();
	return aSeq;
}
/* -----------------------------03.03.03 11:07--------------------------------

 ---------------------------------------------------------------------------*/
//XUnoTunnel
sal_Int64 SAL_CALL OTextCursorHelper::getSomething(
	const uno::Sequence< sal_Int8 >& rId )
		throw(uno::RuntimeException)
{
    if( rId.getLength() == 16
        && 0 == rtl_compareMemory( getUnoTunnelId().getConstArray(),
										rId.getConstArray(), 16 ) )
        {
                return (sal_Int64)this;
        }
	return 0;
}
// -----------------------------------------------------------------------------

}
