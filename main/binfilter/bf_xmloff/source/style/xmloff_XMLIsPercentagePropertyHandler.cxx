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



#ifndef _TOOLS_DEBUG_HXX 
#include <tools/debug.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif


#ifndef _XMLOFF_XMLISPERCENTAGEPROPERTYHANDLER_HXX
#include "XMLIsPercentagePropertyHandler.hxx"
#endif
namespace binfilter {

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::rtl;

XMLIsPercentagePropertyHandler::~XMLIsPercentagePropertyHandler()
{
}

sal_Bool XMLIsPercentagePropertyHandler::importXML(
	const OUString& rStrImpValue,
	Any& rValue,
	const SvXMLUnitConverter& rUnitConverter ) const
{
	rValue <<= (sal_Bool)(rStrImpValue.indexOf( sal_Unicode('%') ) != -1);
	return sal_True;
}

sal_Bool XMLIsPercentagePropertyHandler::exportXML(
	OUString& rStrExpValue,
	const Any& rValue,
	const SvXMLUnitConverter& rUnitConverter ) const
{
	DBG_ERROR( "XMLIsPercentagePropertyHandler is not for export!" ); 
	return sal_False;
}

}//end of namespace binfilter
