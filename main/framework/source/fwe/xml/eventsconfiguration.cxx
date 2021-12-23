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
#include "precompiled_framework.hxx"
#include <framework/eventsconfiguration.hxx>
#include <xml/eventsdocumenthandler.hxx>
#include <services.h>

#ifndef __FRAMEWORK_XML_SAXNAMESPACEFILTER_HXX_
#include <xml/saxnamespacefilter.hxx>
#endif

//_________________________________________________________________________________________________________________
//	interface includes
//_________________________________________________________________________________________________________________
#include <com/sun/star/xml/sax/XParser.hpp>
#include <com/sun/star/io/XActiveDataSource.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>

//_________________________________________________________________________________________________________________
//	other includes
//_________________________________________________________________________________________________________________

#ifndef _UNOTOOLS_PROCESSFACTORY_HXX
#include <comphelper/processfactory.hxx>
#endif
#include <unotools/streamwrap.hxx>
#include <tools/debug.hxx>

//_________________________________________________________________________________________________________________
//	namespace
//_________________________________________________________________________________________________________________

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::io;


namespace framework
{

static Reference< XParser > GetSaxParser(
	// #110897#
	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory
	)
{
	//Reference< XMultiServiceFactory > xServiceManager = ::comphelper::getProcessServiceFactory();
	//return Reference< XParser >( xServiceManager->createInstance( SERVICENAME_SAXPARSER), UNO_QUERY);
	return Reference< XParser >( xServiceFactory->createInstance( SERVICENAME_SAXPARSER), UNO_QUERY);
}

static Reference< XDocumentHandler > GetSaxWriter(
	// #110897#
	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory
	)
{
	//Reference< XMultiServiceFactory > xServiceManager = ::comphelper::getProcessServiceFactory();
	//return Reference< XDocumentHandler >( xServiceManager->createInstance( SERVICENAME_SAXWRITER), UNO_QUERY) ;
	return Reference< XDocumentHandler >( xServiceFactory->createInstance( SERVICENAME_SAXWRITER), UNO_QUERY) ;
}

// #110897#
sal_Bool EventsConfiguration::LoadEventsConfig(
	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
	SvStream& rInStream, EventsConfig& aItems )
{
	Reference< XParser > xParser( GetSaxParser( xServiceFactory ) );
	Reference< XInputStream > xInputStream(
								(::cppu::OWeakObject *)new utl::OInputStreamWrapper( rInStream ),
								UNO_QUERY );

	// connect stream to input stream to the parser
	InputSource aInputSource;

	aInputSource.aInputStream = xInputStream;

	// create namespace filter and set events document handler inside to support xml namespaces
	Reference< XDocumentHandler > xDocHandler( new OReadEventsDocumentHandler( aItems ));
	Reference< XDocumentHandler > xFilter( new SaxNamespaceFilter( xDocHandler ));

	// connect parser and filter
	xParser->setDocumentHandler( xFilter );

	try
	{
		xParser->parseStream( aInputSource );
		return sal_True;
	}
	catch ( RuntimeException& )
	{
		return sal_False;
	}
	catch( SAXException& )
	{
		return sal_False;
	}
	catch( ::com::sun::star::io::IOException& )
	{
		return sal_False;
	}
}

// #110897#
sal_Bool EventsConfiguration::StoreEventsConfig(
	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
	SvStream& rOutStream, const EventsConfig& aItems )
{
	Reference< XDocumentHandler > xWriter( GetSaxWriter( xServiceFactory ) );

	Reference< XOutputStream > xOutputStream(
								(::cppu::OWeakObject *)new utl::OOutputStreamWrapper( rOutStream ),
								UNO_QUERY );

	Reference< ::com::sun::star::io::XActiveDataSource> xDataSource( xWriter , UNO_QUERY );
	xDataSource->setOutputStream( xOutputStream );

	try
	{
		OWriteEventsDocumentHandler aWriteEventsDocumentHandler( aItems, xWriter );
		aWriteEventsDocumentHandler.WriteEventsDocument();
		return sal_True;
	}
	catch ( RuntimeException& )
	{
		return sal_False;
	}
	catch ( SAXException& )
	{
		return sal_False;
	}
	catch ( ::com::sun::star::io::IOException& )
	{
		return sal_False;
	}
}

}
