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
#include "ProppatchRequest.hxx"
#include "UCBDeadPropertyValue.hxx"

using namespace http_dav_ucp;

#define PROPPATCH_HEADER "<?xml version=\"1.0\" encoding=\"utf-8\"?><propertyupdate xmlns=\"DAV:\">"
#define PROPPATCH_TRAILER "</propertyupdate>"

rtl::OString ProppatchRequest::generatePROPPATCHRequestBody(
            const std::vector< ProppatchValue > & inProperties )
{
    rtl::OString aBodyText;
    static const struct
    {
        const char *str;
        sal_Int32   len;
    }
    OpCode [] = {
        { RTL_CONSTASCII_STRINGPARAM( "set" ) },
        { RTL_CONSTASCII_STRINGPARAM( "remove" ) }
    };
    if ( inProperties.size() > 0 )
    {
        rtl::OUStringBuffer aBuffer;
        // add PropPatch xml header in front
        aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( PROPPATCH_HEADER ));

        // <*operation code*><prop>

        ProppatchOperation lastOp = inProperties[ 0 ].operation;
        aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "<" ));
        aBuffer.appendAscii( OpCode[lastOp].str, OpCode[lastOp].len );
        aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "><prop>" ));

        CurlPropName thePropName;
        for ( unsigned long n = 0; n < inProperties.size(); ++n )
        {
            const ProppatchValue & rProperty = inProperties[ n ];
            // split fullname into namespace and name!
            DAVProperties::createCurlPropName( rProperty.name, 
                                               thePropName );

            if ( rProperty.operation != lastOp )
            {
                // </prop></*last operation code*><*operation code><prop>
                aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "</prop></" ));
                aBuffer.appendAscii( OpCode[lastOp].str, OpCode[lastOp].len );
                aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "><" ));
                aBuffer.appendAscii( OpCode[rProperty.operation].str, OpCode[rProperty.operation].len );
                aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "><prop>" ));
            }

            // <*propname* xmlns="*propns*"
            aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "<" ));
            aBuffer.appendAscii( thePropName.name );
            aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( " xmlns=\"" ));
            aBuffer.appendAscii( thePropName.nspace );
            aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "\"" ));

            if ( rProperty.operation == PROPSET )
            {
                // >*property value*</*propname*>
                aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( ">" ));

                rtl::OUString aStringValue;
                if ( DAVProperties::isUCBDeadProperty( thePropName ) )
                {
                    UCBDeadPropertyValue::toXML( rProperty.value,
                                                 aStringValue );
                }
                else
                {
                    rProperty.value >>= aStringValue;
                }
                aBuffer.append( aStringValue );
                aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "</" ));
                aBuffer.appendAscii( thePropName.name );
                aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( ">" ));
            }
            else
            {
                // />
                aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "/>" ));
            }

            lastOp = rProperty.operation;
        }

        // </prop></*last operation code*>
        aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "</prop></" ));
        aBuffer.appendAscii( OpCode[lastOp].str, OpCode[lastOp].len );
        aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( ">" ));

        // add PropPatch xml trailer at end
        aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( PROPPATCH_TRAILER ));

        return rtl::OUStringToOString( aBuffer.makeStringAndClear(), RTL_TEXTENCODING_UTF8 );
    }
    return rtl::OString();
}
