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


#ifndef _SWLINGUCONFIG_HXX
#define _SWLINGUCONFIG_HXX

#ifndef _COM_SUN_STAR_UNO_ANY_H_
#include <com/sun/star/uno/Any.h>
#endif

namespace binfilter {

class SvtLinguConfig; 
struct SvtLinguOptions; 
// predeclarations


// #107253#
class SwLinguConfig
{
public:
	SwLinguConfig();
	~SwLinguConfig();

	sal_Bool SetProperty(const ::rtl::OUString &rPropertyName, const ::com::sun::star::uno::Any &rValue);
	sal_Bool GetOptions(SvtLinguOptions &rOptions) const;
	::com::sun::star::uno::Any GetProperty(const ::rtl::OUString &rPropertyName) const;
};

} //namespace binfilter
#endif // _SWLINGUCONFIG_HXX
