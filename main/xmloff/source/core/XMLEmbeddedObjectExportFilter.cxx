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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_xmloff.hxx"
#include "xmloff/XMLEmbeddedObjectExportFilter.hxx"

using ::rtl::OUString;

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;


XMLEmbeddedObjectExportFilter::XMLEmbeddedObjectExportFilter() throw()
{
}

XMLEmbeddedObjectExportFilter::XMLEmbeddedObjectExportFilter(
		const Reference< XDocumentHandler > & rHandler ) throw() :
	xHandler( rHandler ),
	xExtHandler( rHandler, UNO_QUERY )
{
}

XMLEmbeddedObjectExportFilter::~XMLEmbeddedObjectExportFilter () throw()
{
}


void SAL_CALL XMLEmbeddedObjectExportFilter::startDocument( void )
{
	// do nothing, filter this
}

void SAL_CALL XMLEmbeddedObjectExportFilter::endDocument( void )
{
	// do nothing, filter this
}

void SAL_CALL XMLEmbeddedObjectExportFilter::startElement(
		const OUString& rName,
		const Reference< XAttributeList >& xAttrList )
{
	xHandler->startElement( rName, xAttrList );
}

void SAL_CALL XMLEmbeddedObjectExportFilter::endElement( const OUString& rName )
{
	xHandler->endElement( rName );
}

void SAL_CALL XMLEmbeddedObjectExportFilter::characters( const OUString& rChars )
{
	xHandler->characters( rChars );
}

void SAL_CALL XMLEmbeddedObjectExportFilter::ignorableWhitespace(
		const OUString& rWhitespaces )
{
	xHandler->ignorableWhitespace( rWhitespaces );
}

void SAL_CALL XMLEmbeddedObjectExportFilter::processingInstruction(
		const OUString& rTarget,
	    const OUString& rData )
{
	xHandler->processingInstruction( rTarget, rData );
}

void SAL_CALL XMLEmbeddedObjectExportFilter::setDocumentLocator(
		const Reference< XLocator >& rLocator )
{
	xHandler->setDocumentLocator( rLocator );
}

// XExtendedDocumentHandler
void SAL_CALL XMLEmbeddedObjectExportFilter::startCDATA( void )
{
	if( xExtHandler.is() )
		xExtHandler->startCDATA();
}

void SAL_CALL XMLEmbeddedObjectExportFilter::endCDATA( void )
{
	if( xExtHandler.is() )
		xExtHandler->endCDATA();
}

void SAL_CALL XMLEmbeddedObjectExportFilter::comment( const OUString& rComment )
{
	if( xExtHandler.is() )
		xExtHandler->comment( rComment );
}

void SAL_CALL XMLEmbeddedObjectExportFilter::allowLineBreak( void )
{
	if( xExtHandler.is() )
		xExtHandler->allowLineBreak();
}

void SAL_CALL XMLEmbeddedObjectExportFilter::unknown( const OUString& rString )
{
	if( xExtHandler.is() )
		xExtHandler->unknown( rString );
}

// XInitialize
void SAL_CALL XMLEmbeddedObjectExportFilter::initialize(
		const Sequence< Any >& aArguments )
{
	const sal_Int32 nAnyCount = aArguments.getLength();
	const Any* pAny = aArguments.getConstArray();

	for( sal_Int32 nIndex = 0; nIndex < nAnyCount; nIndex++, pAny++ )
	{
		if( pAny->getValueType() ==
				::getCppuType((const Reference< XDocumentHandler >*)0))
		{
			*pAny >>= xHandler;
			*pAny >>= xExtHandler;
		}
	}
}

// XServiceInfo
OUString SAL_CALL XMLEmbeddedObjectExportFilter::getImplementationName()
{
	OUString aStr;
	return aStr;
}

sal_Bool SAL_CALL XMLEmbeddedObjectExportFilter::supportsService( const OUString& )
{
	return sal_False;
}

Sequence< OUString > SAL_CALL XMLEmbeddedObjectExportFilter::getSupportedServiceNames(  )
{
	Sequence< OUString > aSeq;
	return aSeq;
}
