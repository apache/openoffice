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



#include "oox/helper/openssl_wrapper.hxx"
#include "rtl/ustrbuf.hxx"

#include <openssl/err.h>


namespace oox {

// ============================================================================

using namespace ::com::sun::star::uno;

using ::rtl::OUString;
using ::rtl::OUStringBuffer;

// ============================================================================

static int error_cb( const char *message, size_t len, void *userData )
{
    OUStringBuffer* buffer = (OUStringBuffer*) userData;
    buffer->appendAscii( "\n    " );
    // The message often ends in its own '\n', remove this:
    if( len > 0 && message[ len - 1 ] == '\n' )
        buffer->appendAscii( message, len - 1 );
    else
        buffer->appendAscii( message, len );
    return 1;
}

void throwOpenSSLException( const char *prefix ) throw ( Exception )
{
    OUStringBuffer buffer;
    buffer.appendAscii( prefix );
    ERR_print_errors_cb( error_cb, &buffer );
    throw Exception( buffer.makeStringAndClear(), Reference< XInterface >() );
}

// ============================================================================

} // namespace oox
