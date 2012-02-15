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



#ifndef _BF_MIGRATEFILTER_HXX
#include <bf_migratefilter.hxx>
#endif

#ifndef _COM_SUN_STAR_UTIL_XCLOSEABLE_HPP_
#include <com/sun/star/util/XCloseable.hpp>
#endif

#include <com/sun/star/document/XExporter.hpp>

#ifndef _COM_SUN_STAR_XML_SAX_XPARSER_HPP_
#include <com/sun/star/xml/sax/XParser.hpp>
#endif


#ifndef _COM_SUN_STAR_FRAME_XLOADABLE_HPP_
#include <com/sun/star/frame/XLoadable.hpp>
#endif
#include <com/sun/star/frame/XStorable.hpp>

#ifndef _COM_SUN_STAR_IO_XSEEKABLE_HPP_
#include <com/sun/star/io/XSeekable.hpp>
#endif


#ifndef _LEGACYBINFILTERMGR_HXX
#include <legacysmgr/legacy_binfilters_smgr.hxx>
#endif

#ifndef _COM_SUN_STAR_TASK_XINTERACTIONHANDLER_HPP_
#include <com/sun/star/task/XInteractionHandler.hpp>
#endif

#ifndef _SFXAPP_HXX
#include <bf_sfx2/app.hxx>
#endif

#ifndef _SFX_FCONTNR_HXX
#include <bf_sfx2/fcontnr.hxx>
#endif

#ifndef _SFX_OBJFAC_HXX
#include <bf_sfx2/docfac.hxx>
#endif

#include <bf_so3/staticbaseurl.hxx>

namespace binfilter {

using namespace rtl;
using namespace com::sun::star;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::io;
using namespace com::sun::star::beans;
using namespace com::sun::star::document;
using namespace com::sun::star::xml;
using namespace com::sun::star::xml::sax;
using namespace com::sun::star::frame;
using namespace com::sun::star::task;
using namespace com::sun::star::util;

const OUString sServiceNameTextDocument(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.TextDocument"));
const OUString sServiceNameGlobalDocument(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.GlobalDocument"));
const OUString sServiceNameWebDocument(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.WebDocument"));
const OUString sServiceNamePresentationDocument(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.presentation.PresentationDocument"));
const OUString sServiceNameDrawingDocument(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.drawing.DrawingDocument"));
const OUString sServiceNameSpreadsheetDocument(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.sheet.SpreadsheetDocument"));
const OUString sServiceNameChartDocument(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.chart.ChartDocument"));
const OUString sServiceNameFormulaProperties(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.formula.FormulaProperties"));

// try to get a connection to the Stripped office here
sal_Bool bf_MigrateFilter::getContactToLegacyProcessServiceFactory()
	throw (RuntimeException)
{
	sal_Bool bRetval(sal_False);

	if(!mxLegServFact.is())
	{
		mxLegServFact = ::legacy_binfilters::getLegacyProcessServiceFactory();
	}

	if( mxLegServFact.is() )
	{
		bRetval = sal_True;
	}

	return bRetval;
}

sal_Bool bf_MigrateFilter::importImpl(const Sequence< ::com::sun::star::beans::PropertyValue >& aDescriptor)
	throw (RuntimeException)
{
	sal_Int32 nLength(aDescriptor.getLength());
	const PropertyValue* pValue = aDescriptor.getConstArray();

	Reference < XInputStream > xInputStream;
	Reference< XCloseable > rStrippedDocument;
	Reference< XMultiServiceFactory > rStrippedMSF;
	Reference < XDocumentHandler > xLocalDocumentHandler;
	Reference < XExporter > xStrippedExporter;
	Reference< XInteractionHandler > xInteractionHandler;

	OUString sFilterName;
	OUString sURL;
	OUString sXMLImportService;
	OUString sXMLExportService;
	OUString sStrippedDocumentType;

	sal_Bool bRetval(sal_True);
	sal_Bool bStrippedDocumentCreated(sal_False);
	sal_Bool bInserting(sal_False);

	for(sal_Int32 a(0); a < nLength; a++)
	{
		OUString sName(pValue[a].Name);

		if(pValue[a].Name.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("InputStream")))
			pValue[a].Value >>= xInputStream;

		else if(pValue[a].Name.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("FilterName")))
			pValue[a].Value >>= sFilterName;

		else if(pValue[a].Name.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("URL")))
			pValue[a].Value >>= sURL;

		else if(pValue[a].Name.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("InteractionHandler")))
			pValue[a].Value >>= xInteractionHandler;

		else if(pValue[a].Name.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("InsertMode")))
			pValue[a].Value >>= bInserting;
	}

	if(bRetval)
	{
		// test for necessary parameters
		if(!xInputStream.is())
		{
			OSL_ASSERT(0);
			bRetval = sal_False;
		}
		else if(!sURL.getLength())
		{
			OSL_ASSERT(0);
			bRetval = sal_False;
		}
		else if(!sFilterName.getLength())
		{
			OSL_ASSERT(0);
			bRetval = sal_False;
		}
	}

	if(bRetval)
	{
		// try to get connection to Stripped office
		sal_Bool bGotLegacyServiceManager = getContactToLegacyProcessServiceFactory();

		if(!bGotLegacyServiceManager)
		{
			OSL_ASSERT(0);
			bRetval = sal_False;
		}
	}

	if(bRetval)
	{
		// get contact to the Stripped ServiceFactory
		rStrippedMSF = Reference< XMultiServiceFactory >::query(mxLegServFact);

		if(!rStrippedMSF.is())
		{
			OSL_ASSERT(0);
			bRetval = sal_False;
		}
	}

	if(bRetval)
	{
		// test if the InputStream is seekable
		Reference <XSeekable> xSeekable;
		xSeekable = Reference<XSeekable>::query(xInputStream);

		if(!xSeekable.is())
		{
			OSL_ASSERT(0);
			bRetval = sal_False;
		}
	}

    // Get DocumentType using GetFilterMatcher() (test)
    const SfxFilter* pFilter = SFX_APP()->GetFilterMatcher().GetFilter4FilterName( sFilterName );

	if(bRetval)
	{
		if(pFilter)
		{
			// Get sStrippedDocumentType. For SRX645 this is more simple, look below.
			const SfxFilterContainer* pFilCont = pFilter->GetFilterContainer();
			const SfxFactoryFilterContainer* pFacFilCont = (SfxFactoryFilterContainer*)pFilCont;
			const SfxObjectFactory& rFacRef = pFacFilCont->GetFactory();
			sStrippedDocumentType = rFacRef.GetDocumentServiceName();

			// for srx645:
			// sStrippedDocumentType = pFilter->GetServiceName();

			// ...fill rStrippedDocument somehow
			rStrippedDocument = Reference< XCloseable >::query(rStrippedMSF->createInstance(sStrippedDocumentType));

			if(rStrippedDocument.is())
			{
				bStrippedDocumentCreated = sal_True;
			}
			else
			{
				OSL_ASSERT(0);
				bRetval = sal_False;
			}
		}
        else
            bRetval = sal_False;
	}

	if(bRetval)
	{
		// Make the Stripped office load the document
		Reference < XLoadable > rStrippedLoadable(rStrippedDocument, UNO_QUERY);

		if(rStrippedLoadable.is())
		{
			try
			{
				Sequence < PropertyValue > seqPropValues(xInteractionHandler.is() ? 5 : 4);

				seqPropValues[0].Name = OUString(RTL_CONSTASCII_USTRINGPARAM("ReadOnly"));
				seqPropValues[0].Value <<= sal_True;

				seqPropValues[1].Name = OUString(RTL_CONSTASCII_USTRINGPARAM("InputStream"));
				seqPropValues[1].Value <<= xInputStream;

				seqPropValues[2].Name = OUString(RTL_CONSTASCII_USTRINGPARAM("URL"));
				seqPropValues[2].Value <<= sURL;

				seqPropValues[3].Name = OUString(RTL_CONSTASCII_USTRINGPARAM("FilterName"));
				seqPropValues[3].Value <<= sFilterName;

				if(xInteractionHandler.is())
				{
					seqPropValues[4].Name = OUString(RTL_CONSTASCII_USTRINGPARAM("InteractionHandler"));
					seqPropValues[4].Value <<= xInteractionHandler;
				}

				rStrippedLoadable->load(seqPropValues);
			}
			catch(IOException& /*e*/)
			{
				OSL_ENSURE(sal_False, "IO exception.");
				bRetval = sal_False;
			}
			catch(IllegalArgumentException& /*e*/)
			{
				OSL_ENSURE(sal_False, "uno url invalid");
				bRetval = sal_False;
			}
		}
		else
		{
			OSL_ASSERT(0);
			bRetval = sal_False;
		}
	}

    // only the binary part should touch URLs; make sure that no BaseURL is passed to the importer also
    ::binfilter::StaticBaseUrl::SetBaseURL( String() );

	try
	{
	if(bRetval)
	{
		::rtl::OUString aFilterName;
        Reference < XServiceInfo > rStrippedServiceInfo(rStrippedDocument, UNO_QUERY_THROW);
        if( (rStrippedServiceInfo->supportsService(sServiceNameTextDocument)
			|| rStrippedServiceInfo->supportsService(sServiceNameGlobalDocument)
            || rStrippedServiceInfo->supportsService(sServiceNameWebDocument) ) && pFilter->UsesStorage() )
		{
			// writer document
			aFilterName = ::rtl::OUString::createFromAscii("StarOffice XML (Writer)");
		}
		else if(rStrippedServiceInfo->supportsService(sServiceNamePresentationDocument))
		{
			// presentation: Ask BEFORE draw since presentation supports draw, too
			aFilterName = ::rtl::OUString::createFromAscii("StarOffice XML (Impress)");
		}
		else if(rStrippedServiceInfo->supportsService(sServiceNameDrawingDocument))
		{
			// drawing document
			aFilterName = ::rtl::OUString::createFromAscii("StarOffice XML (Draw)");
		}
		else if(rStrippedServiceInfo->supportsService(sServiceNameSpreadsheetDocument))
		{
			// calc document
			aFilterName = ::rtl::OUString::createFromAscii("StarOffice XML (Calc)");
		}
/*
		else if(rStrippedServiceInfo->supportsService(sServiceNameChartDocument))
		{
			// chart document
			aFilterName = ::rtl::OUString::createFromAscii("StarOffice XML (Chart)");
		}
		else if(rStrippedServiceInfo->supportsService(sServiceNameFormulaProperties))
		{
			// formula document
			aFilterName = ::rtl::OUString::createFromAscii("StarOffice XML (Math)");
		}
*/
		if ( aFilterName.getLength() && !bInserting )
		{
			uno::Reference < io::XStream > xTempFile(
				mxMSF->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.io.TempFile" ) ),
					uno::UNO_QUERY_THROW );
			uno::Reference < frame::XStorable > xDoc( rStrippedDocument, uno::UNO_QUERY );
			uno::Sequence < beans::PropertyValue > args(2);
			args[0].Name = ::rtl::OUString::createFromAscii("OutputStream");
			args[0].Value <<= xTempFile->getOutputStream();
			args[1].Name = ::rtl::OUString::createFromAscii("FilterName");
			args[1].Value <<= aFilterName;
			xDoc->storeToURL( ::rtl::OUString::createFromAscii("private:stream/"), args );
			xTempFile->getOutputStream()->closeOutput();

			Sequence< Any > aArgs( 2 );
			aArgs[0] <<= mxDoc;
			aArgs[1] <<= xTempFile;

			Reference< XFilter > xSubFilter;
			try {
				xSubFilter = Reference<XFilter>(
					mxMSF->createInstanceWithArguments(
						OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.document.OwnSubFilter" ) ),
						aArgs ),
					UNO_QUERY );
				bRetval = xSubFilter->filter( aDescriptor );
			}
			catch(Exception& )
			{
			}
		}
		else 
		{
			if(rStrippedServiceInfo->supportsService(sServiceNameTextDocument)
				|| rStrippedServiceInfo->supportsService(sServiceNameGlobalDocument)
				|| rStrippedServiceInfo->supportsService(sServiceNameWebDocument))
			{
				// writer document
				sXMLImportService = OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.Writer.XMLImporter"));
				sXMLExportService = OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.Writer.XMLExporter"));
			}
			else if(rStrippedServiceInfo->supportsService(sServiceNamePresentationDocument))
			{
				// presentation: Ask BEFORE draw since presentation supports draw, too
				sXMLImportService = OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.Impress.XMLImporter"));
				sXMLExportService = OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.Impress.XMLExporter"));
			}
			else if(rStrippedServiceInfo->supportsService(sServiceNameDrawingDocument))
			{
				// drawing document
				sXMLImportService = OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.Draw.XMLImporter"));
				sXMLExportService = OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.Draw.XMLExporter"));
			}
			else if(rStrippedServiceInfo->supportsService(sServiceNameSpreadsheetDocument))
			{
				// calc document
				sXMLImportService = OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.Calc.XMLImporter"));
				sXMLExportService = OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.Calc.XMLExporter"));
			}
			else if(rStrippedServiceInfo->supportsService(sServiceNameChartDocument))
			{
				// chart document
				sXMLImportService = OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.Chart.XMLImporter"));
				sXMLExportService = OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.Chart.XMLExporter"));
			}
			else if(rStrippedServiceInfo->supportsService(sServiceNameFormulaProperties))
			{
				// formula document
				sXMLImportService = OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.Math.XMLImporter"));
				sXMLExportService = OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.Math.XMLExporter"));
			}
			else
			{
				OSL_ASSERT(0);
				bRetval = sal_False;
			}

			if(bRetval)
			{
				// try to get local document handler
				xLocalDocumentHandler = Reference< XDocumentHandler >( mxMSF->createInstance(sXMLImportService), UNO_QUERY_THROW );

				// set target document at local document handler
				Reference < XImporter > xLocalImporter(xLocalDocumentHandler, UNO_QUERY);
				xLocalImporter->setTargetDocument(mxDoc);

				// try to create Stripped exporter, give local document handler as target
				Sequence < Any > aAnys(1);
				aAnys[0] <<= xLocalDocumentHandler;
				xStrippedExporter = Reference< XExporter >( rStrippedMSF->createInstanceWithArguments(sXMLExportService, aAnys), UNO_QUERY_THROW );

				// set source document to Strippedly loaded read-only document
					xStrippedExporter->setSourceDocument( Reference < XComponent >( rStrippedDocument, UNO_QUERY ) );

				// lock target document controllers
				Reference < com::sun::star::frame::XModel > xTargetDocumentModel(mxDoc, UNO_QUERY);
				sal_Bool bTargetDocumentModelWasNotLocked(sal_False);

				if(xTargetDocumentModel.is() && !xTargetDocumentModel->hasControllersLocked())
				{
					bTargetDocumentModelWasNotLocked = sal_True;
					xTargetDocumentModel->lockControllers();
				}

				// now start the data exchange
				Reference < XFilter > xFilter(xStrippedExporter, UNO_QUERY);
				bRetval = xFilter->filter(aDescriptor);

				if(xTargetDocumentModel.is() && bTargetDocumentModelWasNotLocked)
				{
					xTargetDocumentModel->unlockControllers();
				}
			}                           
		}
	}
	}
	catch ( Exception& )
	{
		OSL_ASSERT(0);
		bRetval = sal_False;
	}

	if(bStrippedDocumentCreated)
	{
		// close Stripped document again
		try
		{
			rStrippedDocument->close(sal_True);
		}
		catch(Exception& /*e*/)
		{
		}
	}

	return bRetval;
}

sal_Bool SAL_CALL bf_MigrateFilter::filter(const Sequence< ::com::sun::star::beans::PropertyValue >& aDescriptor)
	throw (RuntimeException)
{
	sal_Bool bRetval(sal_False);
	sal_Bool bGotLegacyServiceManager = getContactToLegacyProcessServiceFactory();

	if(bGotLegacyServiceManager)
	{
		Reference < XComponent > xWrapper( mxLegServFact->createInstance(
			::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.office.OfficeWrapper" ))), UNO_QUERY );

		bRetval = importImpl(aDescriptor);

		xWrapper->dispose();
		xWrapper = 0;
	}

	return bRetval;
}

void SAL_CALL bf_MigrateFilter::cancel()
	throw (RuntimeException)
{
}

// XImporter
void SAL_CALL bf_MigrateFilter::setTargetDocument(const Reference< ::com::sun::star::lang::XComponent >& xDoc)
	throw (::com::sun::star::lang::IllegalArgumentException, RuntimeException)
{
	mxDoc = xDoc;
}

// XInitialization
void SAL_CALL bf_MigrateFilter::initialize(const Sequence< Any >& aArguments)
	throw (Exception, RuntimeException)
{
	Sequence < PropertyValue > aAnySeq;
	sal_Int32 nLength(aArguments.getLength());

	if(nLength && (aArguments[0] >>= aAnySeq))
	{
		const PropertyValue* pValue = aAnySeq.getConstArray();
		nLength = aAnySeq.getLength();

		for(sal_Int32 a(0); a < nLength; a++)
		{
			OUString sName(pValue[a].Name);

			if(pValue[a].Name.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Type")))
				pValue[a].Value >>= msFilterName;
		}
	}
}

OUString bf_MigrateFilter_getImplementationName()
	throw (RuntimeException)
{
	return OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.office.BF_MigrateFilter"));
}


#define SERVICE_NAME2 "com.sun.star.document.ImportFilter"

sal_Bool SAL_CALL bf_MigrateFilter_supportsService(const OUString& ServiceName)
	throw (RuntimeException)
{
    return ServiceName.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM(SERVICE_NAME2));
}

Sequence< OUString > SAL_CALL bf_MigrateFilter_getSupportedServiceNames()
	throw (RuntimeException)
{
	Sequence < OUString > aRet(1);
    OUString* pArray = aRet.getArray();

    pArray[0] =  OUString(RTL_CONSTASCII_USTRINGPARAM(SERVICE_NAME2));

	return aRet;
}

#undef SERVICE_NAME2

Reference< XInterface > SAL_CALL bf_MigrateFilter_createInstance(const Reference< XMultiServiceFactory > & rSMgr)
	throw( Exception )
{
    static ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > mxLegServFact;
    if ( !mxLegServFact.is() )
    {
        mxLegServFact = ::legacy_binfilters::getLegacyProcessServiceFactory();
        ::com::sun::star::uno::Reference < XComponent > xWrapper( mxLegServFact->createInstance(
            ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.office.OfficeWrapper" ))), UNO_QUERY );
    }

	return (cppu::OWeakObject*)new bf_MigrateFilter(rSMgr);
}

// XServiceInfo
OUString SAL_CALL bf_MigrateFilter::getImplementationName()
	throw (RuntimeException)
{
	return bf_MigrateFilter_getImplementationName();
}

sal_Bool SAL_CALL bf_MigrateFilter::supportsService(const OUString& rServiceName)
	throw (RuntimeException)
{
    return bf_MigrateFilter_supportsService(rServiceName);
}

Sequence< OUString > SAL_CALL bf_MigrateFilter::getSupportedServiceNames()
	throw (RuntimeException)
{
    return bf_MigrateFilter_getSupportedServiceNames();
}

// eof
}
