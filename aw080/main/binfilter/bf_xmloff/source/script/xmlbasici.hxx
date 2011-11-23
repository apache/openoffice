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



#ifndef _XMLOFF_XMLBASICI_HXX
#define _XMLOFF_XMLBASICI_HXX

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _COM_SUN_STAR_XML_SAX_XDOCUMENTHANDLER_HPP_
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#endif

#ifndef _XMLOFF_XMLICTXT_HXX
#include "xmlictxt.hxx"
#endif

//.........................................................................
namespace binfilter
{
//.........................................................................

    // =============================================================================
    // class XMLBasicImportContext
    // =============================================================================

    class XMLBasicImportContext : public SvXMLImportContext
    {
    private:
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >                 m_xModel;
	    ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler >    m_xHandler;

    public:
        XMLBasicImportContext( SvXMLImport& rImport, USHORT nPrfx, const ::rtl::OUString& rLName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >& rxModel );
                            
        virtual ~XMLBasicImportContext();

        virtual SvXMLImportContext* CreateChildContext( USHORT nPrefix, const ::rtl::OUString& rLocalName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& rxAttrList );

        virtual void StartElement( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& rxAttrList );

        virtual void EndElement();

        virtual void Characters( const ::rtl::OUString& rChars );
    };


    // =============================================================================
    // class XMLBasicImportChildContext
    // =============================================================================

    class XMLBasicImportChildContext : public SvXMLImportContext
    {
    private:
	    ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler >    m_xHandler;

    public:
        XMLBasicImportChildContext( SvXMLImport& rImport, USHORT nPrfx, const ::rtl::OUString& rLName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler >& rxHandler );
                            
        virtual ~XMLBasicImportChildContext();

        virtual SvXMLImportContext* CreateChildContext( USHORT nPrefix, const ::rtl::OUString& rLocalName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );

        virtual void StartElement( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );

        virtual void EndElement();

        virtual void Characters( const ::rtl::OUString& rChars );
    };

//.........................................................................
}	// namespace binfilter
//.........................................................................

#endif // _XMLOFF_XMLBASICI_HXX
