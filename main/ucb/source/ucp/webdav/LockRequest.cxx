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
#include "LockRequest.hxx"

using namespace http_dav_ucp;

#define LOCK_HEADER "<?xml version=\"1.0\" encoding=\"utf-8\"?><lockinfo xmlns=\"DAV:\">"
#define LOCK_TYPE "<locktype><write/></locktype>"
#define LOCK_TRAILER "</lockinfo>"

rtl::OString LockRequest::generateRequestBody( const ucb::Lock& inLock )
{
    const char *lockScope = NULL;
    switch ( inLock.Scope )
    {
        //i126305 TODO investigate on this case...
    case ucb::LockScope_MAKE_FIXED_SIZE:

    case ucb::LockScope_EXCLUSIVE:
        lockScope = "<lockscope><exclusive/></lockscope>";
        break;
    case ucb::LockScope_SHARED:
        lockScope = "<lockscope><shared/></lockscope>";
        break;
    }

    rtl::OUStringBuffer aBuffer;
    aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( LOCK_HEADER ));
    aBuffer.appendAscii( lockScope );
    aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( LOCK_TYPE ));
    aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "<owner>" ));
    rtl::OUString aStr;
    inLock.Owner >>= aStr;
    aBuffer.appendAscii( rtl::OUStringToOString( aStr, RTL_TEXTENCODING_UTF8 ).getStr() );
    aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( "</owner>" ));
    aBuffer.appendAscii( RTL_CONSTASCII_STRINGPARAM( LOCK_TRAILER ));
    return rtl::OUStringToOString( aBuffer.makeStringAndClear(), RTL_TEXTENCODING_UTF8 );
}
