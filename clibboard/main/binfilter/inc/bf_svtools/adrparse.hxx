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



#ifndef _ADRPARSE_HXX
#define _ADRPARSE_HXX

#ifndef _LIST_HXX
#include <tools/list.hxx>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

namespace binfilter {

//============================================================================
struct SvAddressEntry_Impl
{
	UniString m_aAddrSpec;
	UniString m_aRealName;

	SvAddressEntry_Impl() {};
	SvAddressEntry_Impl(UniString const & rTheAddrSpec,
						UniString const & rTheRealName):
		m_aAddrSpec(rTheAddrSpec), m_aRealName(rTheRealName) {}
};

//============================================================================
DECLARE_LIST(SvAddressList_Impl, SvAddressEntry_Impl *)

//============================================================================
class  SvAddressParser
{
	friend class SvAddressParser_Impl;

	SvAddressEntry_Impl m_aFirst;
	SvAddressList_Impl m_aRest;
	bool m_bHasFirst;

public:
	SvAddressParser(UniString const & rInput);

	~SvAddressParser();

	sal_Int32 Count() const { return m_bHasFirst ? m_aRest.Count() + 1 : 0; }

	inline UniString const & GetEmailAddress(sal_Int32 nIndex) const;

	inline UniString const &GetRealName(sal_Int32 nIndex) const;
};

inline UniString const & SvAddressParser::GetEmailAddress(sal_Int32 nIndex)
	const
{
	return nIndex == 0 ? m_aFirst.m_aAddrSpec :
		                 m_aRest.GetObject(nIndex - 1)->m_aAddrSpec;
}

inline UniString const & SvAddressParser::GetRealName(sal_Int32 nIndex) const
{
	return nIndex == 0 ? m_aFirst.m_aRealName :
		                 m_aRest.GetObject(nIndex - 1)->m_aRealName;
}

}

#endif // _ADRPARSE_HXX

