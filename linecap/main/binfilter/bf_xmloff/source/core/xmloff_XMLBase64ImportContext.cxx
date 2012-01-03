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



#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif
#ifndef _XMLOFF_XMLUCONV_HXX 
#include "xmluconv.hxx"
#endif

#ifndef _COM_SUN_STAR_IO_XOUTPUTSTREAM_HPP_ 
#include <com/sun/star/io/XOutputStream.hpp>
#endif

#ifndef _XMLOFF_XMLBASE64IMPORTCONTEXT_HXX
#include "XMLBase64ImportContext.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;
using namespace ::com::sun::star::io;

//-----------------------------------------------------------------------------

TYPEINIT1( XMLBase64ImportContext, SvXMLImportContext );


XMLBase64ImportContext::XMLBase64ImportContext(
		SvXMLImport& rImport, USHORT nPrfx, const OUString& rLName,
		const Reference< XAttributeList >& xAttrList,
		const Reference< XOutputStream >& rOut ) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	xOut( rOut )
{
}

XMLBase64ImportContext::~XMLBase64ImportContext()
{
}


void XMLBase64ImportContext::EndElement()
{
	xOut->closeOutput();
}

void XMLBase64ImportContext::Characters( const ::rtl::OUString& rChars )
{
	OUString sTrimmedChars( rChars. trim() );
	if( sTrimmedChars.getLength() )
	{
		OUString sChars;
		if( sBase64CharsLeft )
		{
			sChars = sBase64CharsLeft;
			sChars += sTrimmedChars;
			sBase64CharsLeft = OUString();
		}
		else
		{
			sChars = sTrimmedChars;
		}
		Sequence< sal_Int8 > aBuffer( (sChars.getLength() / 4) * 3 );
		sal_Int32 nCharsDecoded = 
			GetImport().GetMM100UnitConverter().
				decodeBase64SomeChars( aBuffer, sChars );
		xOut->writeBytes( aBuffer );	
		if( nCharsDecoded != sChars.getLength() )
			sBase64CharsLeft = sChars.copy( nCharsDecoded );
	}
}
	
}//end of namespace binfilter
