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



#ifndef _XMLOFF_PROPERTYHANDLER_NUMRULE_HXX
#define _XMLOFF_PROPERTYHANDLER_NUMRULE_HXX

#ifndef _COM_SUN_STAR_UCB_XANYCOMPARE_HPP_ 
#include <com/sun/star/ucb/XAnyCompare.hpp>
#endif

#ifndef _XMLOFF_PROPERTYHANDLERBASE_HXX
#include <xmlprhdl.hxx>
#endif
namespace binfilter {

/**
    PropertyHandler for the list-style
*/
class XMLNumRulePropHdl : public XMLPropertyHandler
{
private:
	::com::sun::star::uno::Reference< ::com::sun::star::ucb::XAnyCompare > mxNumRuleCompare;
public:
	XMLNumRulePropHdl( ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XAnyCompare > xNumRuleCompare );
	virtual ~XMLNumRulePropHdl();

	virtual sal_Bool equals( const ::com::sun::star::uno::Any& r1, const ::com::sun::star::uno::Any& r2 ) const;

	/// NumRules will be imported/exported as XML-Elements. So the Import/Export-work must be done at another place.
	virtual sal_Bool importXML( const ::rtl::OUString& rStrImpValue, ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
	virtual sal_Bool exportXML( ::rtl::OUString& rStrExpValue, const ::com::sun::star::uno::Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const;
};

}//end of namespace binfilter
#endif		// _XMLOFF_PROPERTYHANDLER_NUMRULE_HXX
