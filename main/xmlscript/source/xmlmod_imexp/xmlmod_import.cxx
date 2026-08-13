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
#include "precompiled_xmlscript.hxx"
#include "imp_share.hxx"

#include <osl/diagnose.h>

#include <rtl/ustrbuf.hxx>

#include <xml_import.hxx>
#include <comphelper/processfactory.hxx>


namespace xmlscript
{

//##################################################################################################

//__________________________________________________________________________________________________
Reference< xml::input::XElement > ModuleElement::getParent()
{
    return static_cast< xml::input::XElement * >( _pParent );
}
//__________________________________________________________________________________________________
OUString ModuleElement::getLocalName()
{
    return _aLocalName;
}
//__________________________________________________________________________________________________
sal_Int32 ModuleElement::getUid()
{
    return _pImport->XMLNS_SCRIPT_UID;
}
//__________________________________________________________________________________________________
Reference< xml::input::XAttributes > ModuleElement::getAttributes()
{
    return _xAttributes;
}
//__________________________________________________________________________________________________
void ModuleElement::ignorableWhitespace(
    OUString const & /*rWhitespaces*/ )
{
    // not used
}
//__________________________________________________________________________________________________
void ModuleElement::characters( OUString const & rChars )
{
    _StrBuffer.append( rChars );
}
//__________________________________________________________________________________________________
void ModuleElement::processingInstruction(
    OUString const & /*rTarget*/, OUString const & /*rData*/ )
{
}
//__________________________________________________________________________________________________
void ModuleElement::endElement()
{
    _pImport->mrModuleDesc.aCode = _StrBuffer.makeStringAndClear();
}
//__________________________________________________________________________________________________
Reference< xml::input::XElement > ModuleElement::startChildElement(
    sal_Int32 /*nUid*/, OUString const & /*rLocalName*/,
    Reference< xml::input::XAttributes > const & /*xAttributes*/ )
{
    throw xml::sax::SAXException(
        OUString( RTL_CONSTASCII_USTRINGPARAM("unexpected element!") ),
        Reference< XInterface >(), Any() );
}

//__________________________________________________________________________________________________
ModuleElement::ModuleElement(
    OUString const & rLocalName,
    Reference< xml::input::XAttributes > const & xAttributes,
    ModuleElement * pParent, ModuleImport * pImport )
    SAL_THROW( () )
    : _pImport( pImport )
    , _pParent( pParent )
    , _aLocalName( rLocalName )
    , _xAttributes( xAttributes )
{
    _pImport->acquire();

    if (_pParent)
    {
        _pParent->acquire();
    }
}
//__________________________________________________________________________________________________
ModuleElement::~ModuleElement()
    SAL_THROW( () )
{
    _pImport->release();

    if (_pParent)
    {
        _pParent->release();
    }

#if OSL_DEBUG_LEVEL > 1
    OString aStr( OUStringToOString( _aLocalName, RTL_TEXTENCODING_ASCII_US ) );
    OSL_TRACE( "ModuleElement::~ModuleElement(): %s\n", aStr.getStr() );
#endif
}

//##################################################################################################

// XRoot

//______________________________________________________________________________
void ModuleImport::startDocument(
    Reference< xml::input::XNamespaceMapping > const & xNamespaceMapping )
{
    XMLNS_SCRIPT_UID = xNamespaceMapping->getUidByUri(
        OUSTR(XMLNS_SCRIPT_URI) );
    XMLNS_LIBRARY_UID = xNamespaceMapping->getUidByUri(
        OUSTR(XMLNS_LIBRARY_URI) );
    XMLNS_XLINK_UID = xNamespaceMapping->getUidByUri(
        OUSTR(XMLNS_XLINK_URI) );
}

//__________________________________________________________________________________________________
void ModuleImport::endDocument()
{
    // ignored
}
//__________________________________________________________________________________________________
void ModuleImport::processingInstruction(
    OUString const & /*rTarget*/, OUString const & /*rData*/ )
{
}
//__________________________________________________________________________________________________
void ModuleImport::setDocumentLocator(
    Reference< xml::sax::XLocator > const & /*xLocator*/ )
{
}
//__________________________________________________________________________________________________
Reference< xml::input::XElement > ModuleImport::startRootElement(
    sal_Int32 nUid, OUString const & rLocalName,
    Reference< xml::input::XAttributes > const & xAttributes )
{
    if (XMLNS_SCRIPT_UID != nUid)
    {
        throw xml::sax::SAXException(
            OUString( RTL_CONSTASCII_USTRINGPARAM("illegal namespace!") ),
            Reference< XInterface >(), Any() );
    }
    // window
    else if (rLocalName.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("module") ))
    {
        mrModuleDesc.aName = xAttributes->getValueByUidName(
            XMLNS_SCRIPT_UID,
            OUString( RTL_CONSTASCII_USTRINGPARAM("name") ) );
        mrModuleDesc.aLanguage = xAttributes->getValueByUidName(
            XMLNS_SCRIPT_UID,
            OUString( RTL_CONSTASCII_USTRINGPARAM("language") ) );
        mrModuleDesc.aModuleType = xAttributes->getValueByUidName(
            XMLNS_SCRIPT_UID,
            OUString( RTL_CONSTASCII_USTRINGPARAM("moduleType") ) );

        return new ModuleElement( rLocalName, xAttributes, 0, this );
    }
    else
    {
        throw xml::sax::SAXException(
            OUString( RTL_CONSTASCII_USTRINGPARAM(
                          "illegal root element (expected module) given: ") ) +
            rLocalName, Reference< XInterface >(), Any() );
    }
}
//__________________________________________________________________________________________________
ModuleImport::~ModuleImport()
    SAL_THROW( () )
{
#if OSL_DEBUG_LEVEL > 1
    OSL_TRACE( "ModuleImport::~ModuleImport().\n" );
#endif
}

//##################################################################################################

Reference< xml::sax::XDocumentHandler >
SAL_CALL importScriptModule( ModuleDescriptor& rMod )
{
    return ::xmlscript::createDocumentHandler(
        static_cast< xml::input::XRoot * >( new ModuleImport( rMod ) ) );
}

}
