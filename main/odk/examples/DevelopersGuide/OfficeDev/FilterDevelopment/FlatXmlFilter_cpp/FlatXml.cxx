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



#include <cppuhelper/factory.hxx>
#include <cppuhelper/servicefactory.hxx>
#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase.hxx>

#include <com/sun/star/lang/XComponent.hpp>

#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Type.hxx>

#include <com/sun/star/beans/PropertyValue.hpp>

#include <com/sun/star/xml/sax/XParser.hpp>
#include <com/sun/star/xml/sax/InputSource.hpp>
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#include <com/sun/star/xml/sax/XExtendedDocumentHandler.hpp>
#include <com/sun/star/xml/sax/SAXException.hpp>
#include <com/sun/star/xml/XImportFilter.hpp>
#include <com/sun/star/xml/XExportFilter.hpp>

#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/io/XActiveDataSource.hpp>
#include <com/sun/star/io/XSeekable.hpp>

using namespace ::rtl;
using namespace ::cppu;
using namespace ::osl;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::registry;
using namespace ::com::sun::star::xml;
using namespace ::com::sun::star::xml::sax;

namespace XFlatXml {

class XFlatXml : public WeakImplHelper3< XImportFilter, XExportFilter, XDocumentHandler>
{
private:
    // the UNO ServiceFactory
    Reference< XMultiServiceFactory > m_rServiceFactory;

    // DocumentHandler interface of the css::xml::sax::Writer service
    Reference < XExtendedDocumentHandler > m_rDocumentHandler;

    // controls pretty-printing
    sal_Bool m_bPrettyPrint;

public:

    // ctor...
    XFlatXml( const Reference< XMultiServiceFactory > &r )
        : m_rServiceFactory(r)
        , m_bPrettyPrint(sal_True)
	{}

    // XImportFilter
    virtual sal_Bool SAL_CALL importer(
            const Sequence<PropertyValue>& aSourceData,
            const Reference<XDocumentHandler>& xHandler,
            const Sequence<OUString>& msUserData)
        throw(RuntimeException);

    // XExportFilter
    virtual sal_Bool SAL_CALL exporter(
            const Sequence<PropertyValue>& aSourceData,
            const Sequence<OUString>& msUserData)
        throw(RuntimeException);

    // XDocumentHandler
    virtual void SAL_CALL startDocument()
        throw (SAXException,RuntimeException);
    virtual void SAL_CALL endDocument()
        throw (SAXException, RuntimeException);
    virtual void SAL_CALL startElement(const OUString& str, const Reference<XAttributeList>& attriblist)
        throw (SAXException,RuntimeException);
    virtual void SAL_CALL endElement(const OUString& str)
        throw (SAXException, RuntimeException);
    virtual void SAL_CALL characters(const OUString& str)
        throw (SAXException, RuntimeException);
    virtual void SAL_CALL ignorableWhitespace(const OUString& str)
        throw (SAXException, RuntimeException);
    virtual void SAL_CALL processingInstruction(const OUString& str, const OUString& str2)
        throw (com::sun::star::xml::sax::SAXException,RuntimeException);
    virtual void SAL_CALL setDocumentLocator(const Reference<XLocator>& doclocator)
        throw (SAXException,RuntimeException);
};

sal_Bool XFlatXml::importer(
        const Sequence<PropertyValue>& aSourceData,
        const Reference<XDocumentHandler>& xHandler,
        const Sequence<OUString>& msUserData)
    throw (RuntimeException)
{
    // get information from media descriptor
    // the input stream that represents the imported file
    // is most important here since we need to supply it to
    // the sax parser that drives the supplied document handler
    sal_Int32 nLength = aSourceData.getLength();
    OUString aName, aFileName, aURL;
    Reference< XInputStream > xInputStream;
    for ( sal_Int32 i = 0 ; i < nLength; i++)
	{
        aName = aSourceData[i].Name;
	    if (aName.equalsAscii("InputStream"))
            aSourceData[i].Value >>= xInputStream;
	    else if ( aName.equalsAscii("FileName"))
			aSourceData[i].Value >>= aFileName;
	    else if ( aName.equalsAscii("URL"))
			aSourceData[i].Value >>= aURL;
	}

    // we need an input stream
    OSL_ASSERT(xInputStream.is());
    if (!xInputStream.is()) return sal_False;

    // rewind seekable stream
    Reference< XSeekable > xSeek(xInputStream, UNO_QUERY);
    if (xSeek.is())
        xSeek->seek(0);


    // create SAX parser that will read the document file
    // and provide events to xHandler passed to this call
	Reference < XParser > xSaxParser( m_rServiceFactory->createInstance(
        OUString::createFromAscii("com.sun.star.xml.sax.Parser")), UNO_QUERY );
    OSL_ASSERT(xSaxParser.is());
	if(!xSaxParser.is())return sal_False;

    // let the parser try to send the sax event to the document handler
    try
	{
	    InputSource aInput;
	    aInput.sSystemId = aURL;
        aInput.sPublicId = aURL;
	    aInput.aInputStream = xInputStream;
	    xSaxParser->setDocumentHandler(xHandler);
	    xSaxParser->parseStream(aInput);
	}
	catch( Exception &exc)
	{
        // something went wrong
        OSL_ENSURE(0, rtl::OUStringToOString(exc.Message,RTL_TEXTENCODING_UTF8).getStr());
        return sal_False;
	}

    // done
    return sal_True;
}

sal_Bool XFlatXml::exporter(
        const Sequence<PropertyValue>& aSourceData,
        const Sequence<OUString>& msUserData)
    throw (RuntimeException)
{

    // read source data
    // we are especially interested in the output stream
    // since that is where our xml-writer will push the data
    // from its data-source interface
    OUString aName, sURL;
    Reference<XOutputStream> rOutputStream;
    sal_Int32 nLength = aSourceData.getLength();
    for ( sal_Int32 i = 0 ; i < nLength; i++)
    {
        aName = aSourceData[i].Name;
        if ( aName.equalsAscii("OutputStream"))
	        aSourceData[i].Value >>= rOutputStream;
        else if ( aName.equalsAscii("URL" ))
            aSourceData[i].Value >>= sURL;
    }

    if (!m_rDocumentHandler.is()) {
        // get the document writer
        m_rDocumentHandler = Reference<XExtendedDocumentHandler>(
            m_rServiceFactory->createInstance(
            OUString::createFromAscii("com.sun.star.xml.sax.Writer")),
                UNO_QUERY);
        OSL_ASSERT(m_rDocumentHandler.is());
        if (!m_rDocumentHandler.is()) return sal_False;
    }
    // get data source interface ...
    Reference< XActiveDataSource > rDataSource(m_rDocumentHandler, UNO_QUERY);
    OSL_ASSERT(rDataSource.is());
    if (!rDataSource.is()) return sal_False;
    OSL_ASSERT(rOutputStream.is());
    if (!rOutputStream.is()) return sal_False;
    // ... and set output stream
    rDataSource->setOutputStream(rOutputStream);

    return sal_True;
}

// for the DocumentHandler implementation, we just proxy the the
// events to the XML writer that we created upon the output stream
// that was provided by the XMLFilterAdapter
void XFlatXml::startDocument() throw (SAXException,RuntimeException){
    OSL_ASSERT(m_rDocumentHandler.is());
    m_rDocumentHandler->startDocument();
}

void XFlatXml::endDocument() throw (SAXException,RuntimeException){
    OSL_ASSERT(m_rDocumentHandler.is());
    m_rDocumentHandler->endDocument();
}

void XFlatXml::startElement(const OUString& str, const Reference<XAttributeList>& attriblist)
    throw (SAXException, RuntimeException)
{
    OSL_ASSERT(m_rDocumentHandler.is());
    m_rDocumentHandler->startElement(str, attriblist);
}

void XFlatXml::endElement(const OUString& str)
    throw (SAXException, RuntimeException)
{
    OSL_ASSERT(m_rDocumentHandler.is());
    m_rDocumentHandler->endElement(str);
}

void XFlatXml::characters(const OUString& str)
    throw (SAXException, RuntimeException)
{
    OSL_ASSERT(m_rDocumentHandler.is());
    m_rDocumentHandler->characters(str);
}

void XFlatXml::ignorableWhitespace(const OUString& str)
    throw (SAXException, RuntimeException)
{
    OSL_ASSERT(m_rDocumentHandler.is());
    if (!m_bPrettyPrint) return;
    m_rDocumentHandler->ignorableWhitespace(str);
}

void  XFlatXml::processingInstruction(const OUString& str, const OUString& str2)
    throw (SAXException, RuntimeException)
{
    OSL_ASSERT(m_rDocumentHandler.is());
    m_rDocumentHandler->processingInstruction(str, str2);
}

void XFlatXml::setDocumentLocator(const Reference<XLocator>& doclocator)
    throw (SAXException, RuntimeException)
{
    OSL_ASSERT(m_rDocumentHandler.is());
    m_rDocumentHandler->setDocumentLocator(doclocator);
}

// --------------------------------------
// Component management
// --------------------------------------
Reference< XInterface > SAL_CALL CreateInstance( const Reference< XMultiServiceFactory > &r)
{
    return Reference< XInterface >(( OWeakObject *)new XFlatXml(r));
}

Sequence< OUString > getSupportedServiceNames()
{
	static Sequence < OUString > *pNames = 0;
	if( ! pNames )
	{
		MutexGuard guard( Mutex::getGlobalMutex() );
		if( !pNames )
		{
			static Sequence< OUString > seqNames(1);
			seqNames.getArray()[0] = OUString::createFromAscii(
    			"devguide.officedev.samples.filter.FlatXmlCpp");
			pNames = &seqNames;
		}
	}
	return *pNames;
}

}

using namespace XFlatXml;
#define IMPLEMENTATION_NAME "devguide.officedev.samples.filter.FlatXmlCpp"


extern "C"
{
SAL_DLLPUBLIC_EXPORT void SAL_CALL component_getImplementationEnvironment(
	const sal_Char ** ppEnvTypeName, uno_Environment ** ppEnv )
{
    *ppEnvTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}

// This method not longer necessary since OOo 3.4 where the component registration was
// was changed to passive component registration. For more details see
// https://wiki.openoffice.org/wiki/Passive_Component_Registration
//
// SAL_DLLPUBLIC_EXPORT sal_Bool SAL_CALL component_writeInfo(void * pServiceManager, void * pRegistryKey )
// {
//     if (pRegistryKey)
// 	{
//         try
//         {
//             Reference< XRegistryKey > xNewKey(
//                 reinterpret_cast< XRegistryKey * >( pRegistryKey )->createKey(
//                     OUString::createFromAscii( "/" IMPLEMENTATION_NAME "/UNO/SERVICES" ) ) );

//             const Sequence< OUString > & rSNL = getSupportedServiceNames();
//             const OUString * pArray = rSNL.getConstArray();
//             for ( sal_Int32 nPos = rSNL.getLength(); nPos--; )
//                 xNewKey->createKey( pArray[nPos] );

//             return sal_True;
//         }
//         catch (InvalidRegistryException &)
//         {
//             OSL_ENSURE( sal_False, "### InvalidRegistryException!" );
//         }
//     }
//     return sal_False;
// }

SAL_DLLPUBLIC_EXPORT void * SAL_CALL component_getFactory(
    const sal_Char * pImplName, void * pServiceManager, void * pRegistryKey )
{
    void * pRet = 0;

    if (pServiceManager && rtl_str_compare( pImplName, IMPLEMENTATION_NAME ) == 0)
    {
        Reference< XSingleServiceFactory > xFactory( createSingleFactory(
            reinterpret_cast< XMultiServiceFactory * >( pServiceManager ),
            OUString::createFromAscii( pImplName ),
            CreateInstance, getSupportedServiceNames() ) );

        if (xFactory.is())
        {
            xFactory->acquire();
            pRet = xFactory.get();
        }
    }
    return pRet;
}

} // extern "C"

