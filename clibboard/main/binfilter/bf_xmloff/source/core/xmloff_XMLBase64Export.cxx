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




#ifndef _COM_SUN_STAR_IO_XINPUTSTREAM_HPP_ 
#include <com/sun/star/io/XInputStream.hpp>
#endif

#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif
#ifndef _XMLOFF_XMLEXP_HXX 
#include "xmlexp.hxx"
#endif
#ifndef _XMLOFF_XMLNMSPE_HXX 
#include "xmlnmspe.hxx"
#endif

#ifndef _XMLOFF_XMLBASE64EXPORT_HXX
#include "XMLBase64Export.hxx"
#endif
namespace binfilter {

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::io;
using namespace ::rtl;

#define INPUT_BUFFER_SIZE 54
#define OUTPUT_BUFFER_SIZE 72

XMLBase64Export::XMLBase64Export( SvXMLExport& rExp ) :
	rExport( rExp ){
}

sal_Bool XMLBase64Export::exportXML( const Reference < XInputStream> & rIn )
{
	sal_Bool bRet = sal_True;
	try
	{
		Sequence < sal_Int8 > aInBuff( INPUT_BUFFER_SIZE );
		OUStringBuffer aOutBuff( OUTPUT_BUFFER_SIZE );
		sal_Int32 nRead;
		do
		{
			nRead = rIn->readBytes( aInBuff, INPUT_BUFFER_SIZE );
			if( nRead > 0 )
			{
				GetExport().GetMM100UnitConverter().encodeBase64( aOutBuff,
																  aInBuff );
				GetExport().Characters( aOutBuff.makeStringAndClear() );
				if( nRead == INPUT_BUFFER_SIZE )
					GetExport().IgnorableWhitespace();
			}
		}
		while( nRead == INPUT_BUFFER_SIZE );
	}
	catch( ... )
	{
		bRet = sal_False;
	}

	return bRet;
}

sal_Bool XMLBase64Export::exportElement(
			const Reference < XInputStream > & rIn,
			sal_uInt16 nNamespace,
			enum ::binfilter::xmloff::token::XMLTokenEnum eName )
{
	SvXMLElementExport aElem( GetExport(), nNamespace, eName, sal_True,
							  sal_True );
	return exportXML( rIn );
}

sal_Bool XMLBase64Export::exportOfficeBinaryDataElement(
			const Reference < XInputStream > & rIn )
{
	return exportElement( rIn, XML_NAMESPACE_OFFICE,
				   		  ::binfilter::xmloff::token::XML_BINARY_DATA );
}

}//end of namespace binfilter
