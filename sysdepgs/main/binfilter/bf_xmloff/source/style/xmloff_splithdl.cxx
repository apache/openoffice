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



#ifndef _XMLOFF_PROPERTYHANDLER_FMTSPLITTYPES_HXX
#include "splithdl.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif


// --

#ifndef _XMLOFF_XMLTOKEN_HXX
#include <xmltoken.hxx>
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::binfilter::xmloff::token;

///////////////////////////////////////////////////////////////////////////////
//
// class XMLFmtSplitPropHdl
//

XMLFmtSplitPropHdl::~XMLFmtSplitPropHdl()
{
	// nothing to do
}

sal_Bool XMLFmtSplitPropHdl::importXML( const OUString& rStrImpValue, Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bValue = IsXMLToken( rStrImpValue, XML_COLUMNSPLIT_AUTO );
	rValue <<= sal_Bool(bValue);
	return  sal_True;
}

sal_Bool XMLFmtSplitPropHdl::exportXML( OUString& rStrExpValue, const Any& rValue, const SvXMLUnitConverter& rUnitConverter ) const
{ 
	sal_Bool bRet = sal_False;
	sal_Bool bValue;

	if (rValue >>= bValue)
	{
		if( bValue )
		{
			rStrExpValue = GetXMLToken( XML_COLUMNSPLIT_AUTO );
		}
		else
		{
			rStrExpValue = GetXMLToken( XML_COLUMNSPLIT_AVOID );
		}

		bRet = sal_True;
	}

	return bRet;
}
}//end of namespace binfilter
