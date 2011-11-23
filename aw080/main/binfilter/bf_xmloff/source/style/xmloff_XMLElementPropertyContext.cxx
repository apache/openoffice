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




#ifndef _XMLOFF_XMLELEMENTPROPERTYCONTEXT_HXX
#include "XMLElementPropertyContext.hxx"
#endif
namespace binfilter {

using namespace ::rtl;


TYPEINIT1( XMLElementPropertyContext , SvXMLImportContext );

XMLElementPropertyContext::XMLElementPropertyContext (
								SvXMLImport& rImport, sal_uInt16 nPrfx,
								const OUString& rLName,
								const XMLPropertyState& rProp,
				 				::std::vector< XMLPropertyState > &rProps ) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	aProp( rProp ),
  	rProperties( rProps ),
	bInsert( sal_False )
{
}

XMLElementPropertyContext::~XMLElementPropertyContext()
{
}

void XMLElementPropertyContext::EndElement( )
{
	if( bInsert )
		rProperties.push_back( aProp );
}


}//end of namespace binfilter
