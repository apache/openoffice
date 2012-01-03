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



#ifndef _XMLOFF_NAMEDBOOLPROPERTYHANDLER_HXX
#include <NamedBoolPropertyHdl.hxx>
#endif


#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif

#include "rtl/ustring.hxx"


namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star::uno;

///////////////////////////////////////////////////////////////////////////////
//
// class XMLNamedBoolPropertyHdl
//

XMLNamedBoolPropertyHdl::~XMLNamedBoolPropertyHdl()
{
	// Nothing to do
}

sal_Bool XMLNamedBoolPropertyHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
	if( rStrImpValue == maTrueStr )
	{
		rValue = ::cppu::bool2any( sal_True );
		return sal_True;
	}

	if( rStrImpValue == maFalseStr )
	{
		rValue = ::cppu::bool2any( sal_False );
		return sal_True;
	}

	return sal_False;
}

sal_Bool XMLNamedBoolPropertyHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{
	if( ::cppu::any2bool( rValue ) )
	{
		rStrExpValue = maTrueStr;
	}
	else
	{
		rStrExpValue = maFalseStr;
	}

	return sal_True;
}

}//end of namespace binfilter
