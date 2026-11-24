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



#include "oox/core/fragmenthandler.hxx"

#include "oox/core/xmlfilterbase.hxx"
#include "oox/core/mcealternatecontext.hxx"
#include "oox/token/tokens.hxx"

namespace oox {
namespace core {

// ============================================================================

using namespace ::com::sun::star::io;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;

using ::rtl::OUString;

// ============================================================================

const ::rtl::OUString& FragmentHandler::getMarkupCompatNamespace()
{
    static const ::rtl::OUString aNamespace =
        ::rtl::OUString::createFromAscii(
            "http://schemas.openxmlformats.org/markup-compatibility/2006" );
    return aNamespace;
}

void FragmentHandler::pushNamespaces(
    const ::com::sun::star::uno::Reference<
        ::com::sun::star::xml::sax::XFastAttributeList >& rAttribs )
{
    std::vector< std::pair< ::rtl::OUString, ::rtl::OUString > > aMap;
    MceProcessor::extractNamespaceDeclarations( rAttribs, aMap );
    maNamespaceStack.push_back( aMap );
}

void FragmentHandler::popNamespaces()
{
    if ( !maNamespaceStack.empty() )
        maNamespaceStack.pop_back();
}

::rtl::OUString FragmentHandler::resolvePrefix(
    const ::rtl::OUString& rPrefix ) const
{
    // Search from top of stack (most local) to bottom (most global).
    for ( auto it = maNamespaceStack.rbegin(); it != maNamespaceStack.rend(); ++it )
    {
        for ( const auto& rPair : *it )
        {
            if ( rPair.first == rPrefix )
                return rPair.second;
        }
    }
    return ::rtl::OUString(); // Empty = unresolved
}

// ============================================================================

FragmentBaseData::FragmentBaseData( XmlFilterBase& rFilter, const OUString& rFragmentPath, RelationsRef xRelations ) :
    mrFilter( rFilter ),
    maFragmentPath( rFragmentPath ),
    mxRelations( xRelations )
{
}

// ============================================================================

FragmentHandler::FragmentHandler( XmlFilterBase& rFilter, const OUString& rFragmentPath ) :
    FragmentHandler_BASE( FragmentBaseDataRef( new FragmentBaseData( rFilter, rFragmentPath, rFilter.importRelations( rFragmentPath ) ) ) )
{
}

FragmentHandler::FragmentHandler( XmlFilterBase& rFilter, const OUString& rFragmentPath, RelationsRef xRelations ) :
    FragmentHandler_BASE( FragmentBaseDataRef( new FragmentBaseData( rFilter, rFragmentPath, xRelations ) ) )
{
}

FragmentHandler::~FragmentHandler()
{
}

// com.sun.star.xml.sax.XFastDocumentHandler interface ------------------------

void FragmentHandler::startDocument()
{
}

void FragmentHandler::endDocument()
{
}

void FragmentHandler::setDocumentLocator( const Reference< XLocator >& rxLocator )
{
    implSetLocator( rxLocator );
}

// com.sun.star.xml.sax.XFastContextHandler interface -------------------------

void FragmentHandler::startFastElement( sal_Int32 Element, const Reference< XFastAttributeList >& rAttribs )
{
    pushNamespaces( rAttribs );
}

void FragmentHandler::startUnknownElement( const OUString&, const OUString&, const Reference< XFastAttributeList >& )
{
}

void FragmentHandler::endFastElement( sal_Int32 )
{
    popNamespaces();
}

void FragmentHandler::endUnknownElement( const OUString&, const OUString& )
{
}

Reference< XFastContextHandler > FragmentHandler::createFastChildContext( sal_Int32 Element, const Reference< XFastAttributeList >& rAttribs )
{
    // Phase 1 MCE: intercept mc:AlternateContent and delegate to MceAlternateContext
    // for Choice/Fallback selection. All other elements continue to
    // return 0 (unchanged behavior).
    if ( Element == XML_AlternateContent )
    {
        MceAlternateContext* pMceCtx = new MceAlternateContext(
            getFastContextHandler() );
        return Reference< XFastContextHandler >(
            static_cast< XFastContextHandler* >( pMceCtx ) );
    }

    return 0;
}

Reference< XFastContextHandler > FragmentHandler::createUnknownChildContext( const OUString&, const OUString&, const Reference< XFastAttributeList >& )
{
    return 0;
}

void FragmentHandler::characters( const OUString& )
{
}

void FragmentHandler::ignorableWhitespace( const OUString& )
{
}

void FragmentHandler::processingInstruction( const OUString&, const OUString& )
{
}

// XML stream handling --------------------------------------------------------

Reference< XInputStream > FragmentHandler::openFragmentStream() const
{
    return getFilter().openInputStream( getFragmentPath() );
}

// binary records -------------------------------------------------------------

const RecordInfo* FragmentHandler::getRecordInfos() const
{
    // default: no support for binary records
    return 0;
}

// ============================================================================

} // namespace core
} // namespace oox
