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



#ifndef OOX_CORE_ENCRYPTION_HXX
#define OOX_CORE_ENCRYPTION_HXX

#include <sal/types.h>
#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include "com/sun/star/uno/Reference.hxx"
#include <com/sun/star/uno/XComponentContext.hpp>
#include <comphelper/sequenceashashmap.hxx>
#include "oox/helper/binaryinputstream.hxx"
#include "oox/helper/binaryoutputstream.hxx"


namespace oox {
namespace core {

// ============================================================================

class EncryptionInfo
{
public:
    // Parses the given stream, and returns a subclass which implements the virtual methods below.
    static EncryptionInfo* readEncryptionInfo(
        const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& context,
        ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream>& inputStream ) throw ( ::com::sun::star::uno::Exception );

    virtual ~EncryptionInfo() {}
    // Checks whether decryption can work, ie. we support all the algorithms, key sizes, etc.
    virtual bool isImplemented() = 0;
    // On success, returns a non-empty sequence, and internally stores whatever is needed for a subsequent call to decryptStream() to work.
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue > verifyPassword( const ::rtl::OUString& rPassword ) throw ( ::com::sun::star::uno::Exception ) = 0;
    // On success, returns true, and internally stores whatever is needed for a subsequent call to decryptStream() to work.
    virtual bool verifyEncryptionData( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue >& rEncryptionData ) throw ( ::com::sun::star::uno::Exception ) = 0;
    // Decrypts the stream using keys derived from previous calls to verifyPassword() or verifyEncryptionData().
    virtual void decryptStream( BinaryXInputStream &aEncryptedPackage, BinaryXOutputStream &aDecryptedPackage ) throw ( ::com::sun::star::uno::Exception ) = 0;
};

// ============================================================================

} // namespace core
} // namespace oox

#endif
