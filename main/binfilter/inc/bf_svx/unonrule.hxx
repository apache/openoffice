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



#ifndef _SVX_UNONRULE_HXX
#define _SVX_UNONRULE_HXX

#ifndef _COM_SUN_STAR_CONTAINER_XINDEXREPLACE_HPP_
#include <com/sun/star/container/XIndexReplace.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XANYCOMPARE_HPP_ 
#include <com/sun/star/ucb/XAnyCompare.hpp>
#endif
namespace binfilter {

class SdrModel;
class SvxNumRule;

::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexReplace > SvxCreateNumRule( const SvxNumRule* pRule ) throw();
::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexReplace > SvxCreateNumRule( SdrModel* pModel ) throw();
const SvxNumRule& SvxGetNumRule( ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexReplace > xRule ) throw( ::com::sun::star::lang::IllegalArgumentException );
::com::sun::star::uno::Reference< ::com::sun::star::ucb::XAnyCompare > SvxCreateNumRuleCompare() throw();

}//end of namespace binfilter
#endif

