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
#include "precompiled_webdav.hxx"

#include <rtl/ustrbuf.hxx>
#include "DAVProperties.hxx"
#include "PropfindRequest.hxx"

using namespace http_dav_ucp;

rtl::OString PropfindRequest::generatePROPFINDRequestBody(
            const std::vector< ::rtl::OUString > * propNames,
            const bool onlyPropertyNames )
{
#define PROPFIND_HEADER "<?xml version=\"1.0\" encoding=\"utf-8\"?><propfind xmlns=\"DAV:\">"
#define PROPFIND_TRAILER "</propfind>"

    // TODO is it really needed a Unicode string buffer?
    // All properties and property names aren't supposed to be ASCII?
    rtl::OUStringBuffer aBuffer;
    aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( PROPFIND_HEADER ));

    // create and fill body with requested properties
    const int nPropCount = ( !onlyPropertyNames && propNames )
                           ? propNames->size() 
                           : 0;
    if ( nPropCount > 0 )
    {
        aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "<prop>" ) );
        CurlPropName thePropName;
        for ( int theIndex = 0; theIndex < nPropCount; theIndex ++ )
        {
            // split fullname into namespace and name!
            DAVProperties::createCurlPropName( (*propNames)[ theIndex ], 
                                               thePropName );

            /* <*propname* xmlns="*propns*" /> */
            aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "<" ));
            aBuffer.appendAscii( thePropName.name );
            aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( " xmlnx=\"" ));
            aBuffer.appendAscii( thePropName.nspace );
            aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "\"/>" ));
        }

        aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "</prop>" ));
    }
    else
    {
        if ( onlyPropertyNames )
        {
            aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "<propname/>" ));
        }
        else
        {
            aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "<allprop/>" ));
        }
    }

    aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( PROPFIND_TRAILER ));
    return rtl::OUStringToOString( aBuffer.makeStringAndClear(), RTL_TEXTENCODING_UTF8 );
}
