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



#ifndef _XMLOFF_XMLTEXTNUMRULEINFO_HXX
#define _XMLOFF_XMLTEXTNUMRULEINFO_HXX

#ifndef _COM_SUN_STAR_CONTAINER_XINDEXREPLACE_HPP_ 
#include <com/sun/star/container/XIndexReplace.hpp>
#endif

namespace com { namespace sun { namespace star {
	namespace text { class XTextContent; }
} } }

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
namespace binfilter {

class XMLTextNumRuleInfo
{
	const ::rtl::OUString sNumberingRules;
	const ::rtl::OUString sNumberingLevel;
	const ::rtl::OUString sNumberingStartValue;
	const ::rtl::OUString sParaIsNumberingRestart;
	const ::rtl::OUString sNumberingType;
	const ::rtl::OUString sIsNumbering;
	const ::rtl::OUString sNumberingIsNumber;

	::com::sun::star::uno::Reference <
		::com::sun::star::container::XIndexReplace > xNumRules;

	::rtl::OUString		sName;
	sal_Int16			nStartValue;
	sal_Int16			nLevel;
	
	sal_Bool			bIsNumbered : 1;
	sal_Bool			bIsOrdered : 1;
	sal_Bool			bIsRestart : 1;
	sal_Bool			bIsNamed : 1;
	
public:
	
	XMLTextNumRuleInfo();

	inline XMLTextNumRuleInfo& operator=( const XMLTextNumRuleInfo& rInfo );
	
	void Set(
		const ::com::sun::star::uno::Reference < 
			::com::sun::star::text::XTextContent > & rTextContnt );
	inline void Reset();

	const ::rtl::OUString& GetName() const { return sName; }
	const ::com::sun::star::uno::Reference <
		::com::sun::star::container::XIndexReplace >& GetNumRules() const
		{ return xNumRules; }
	sal_Int16 GetLevel() const { return nLevel; }
	
	sal_Bool HasStartValue() const { return nStartValue != -1; }
	sal_uInt32 GetStartValue() const { return nStartValue; }

	sal_Bool IsNumbered() const { return bIsNumbered; }
	sal_Bool IsOrdered() const { return bIsOrdered; }
	sal_Bool IsRestart() const { return bIsRestart; }
	sal_Bool IsNamed() const { return bIsNamed; }

	inline sal_Bool HasSameNumRules( const XMLTextNumRuleInfo& rCmp ) const;
};

inline XMLTextNumRuleInfo& XMLTextNumRuleInfo::operator=( 
		const XMLTextNumRuleInfo& rInfo )
{
	sName = rInfo.sName;
	xNumRules = rInfo.xNumRules;
	nStartValue = rInfo.nStartValue;
	nLevel = rInfo.nLevel;
	bIsNumbered = rInfo.bIsNumbered;
	bIsOrdered = rInfo.bIsOrdered;
	bIsRestart = rInfo.bIsRestart;
	bIsNamed = rInfo.bIsNamed;

	return *this;
}

inline void XMLTextNumRuleInfo::Reset()
{
	sName = ::rtl::OUString();
	xNumRules = 0;
	nStartValue = -1;
	nLevel = 0;
	bIsNumbered = bIsOrdered = bIsRestart = bIsNamed = sal_False;
}

inline sal_Bool XMLTextNumRuleInfo::HasSameNumRules(
						const XMLTextNumRuleInfo& rCmp ) const
{
	return (bIsNamed && rCmp.bIsNamed) ? (rCmp.sName == sName)
								   : (rCmp.xNumRules == xNumRules);
}

}//end of namespace binfilter
#endif	//  _XMLOFF_XMLTEXTNUMRULEINFO_HXX
