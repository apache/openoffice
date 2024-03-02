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



#ifndef OOX_HELPER_OPENSSL_WRAPPER_HXX
#define OOX_HELPER_OPENSSL_WRAPPER_HXX

#include <sal/types.h>
#include "com/sun/star/uno/Exception.hpp"

#include <openssl/evp.h>

namespace oox {

// ============================================================================

extern void throwOpenSSLException( const char *prefix ) throw ( ::com::sun::star::uno::Exception );


class OpenSSLDigest
{
public:
    OpenSSLDigest() throw ( ::com::sun::star::uno::Exception )
    {
        digest_ctx = EVP_MD_CTX_new();
        if( digest_ctx == NULL )
            throwOpenSSLException( "Failed to create digest context" );
    }

    ~OpenSSLDigest()
    {
        EVP_MD_CTX_free( digest_ctx );
    }

    void initialize( const EVP_MD* aDigest ) throw ( ::com::sun::star::uno::Exception )
    {
        if( 1 != EVP_DigestInit_ex( digest_ctx, aDigest, NULL ) )
            throwOpenSSLException( "Failed to initialize digest context" );
        digest = aDigest;
    }

    int digestSize() throw ( ::com::sun::star::uno::Exception )
    {
        return digestSize( digest );
    }

    void update( const void *data, unsigned int count ) throw ( ::com::sun::star::uno::Exception )
    {
        if( 1 != EVP_DigestUpdate( digest_ctx, data, count ) )
            throwOpenSSLException( "Failed to update the digest context" );
    }

    void final( unsigned char *md, unsigned int *count ) throw ( ::com::sun::star::uno::Exception )
    {
        if( 1 != EVP_DigestFinal_ex( digest_ctx, md, count ) )
            throwOpenSSLException( "Failed to finalize digest" );
    }

    static int digestSize( const EVP_MD* digest ) throw ( ::com::sun::star::uno::Exception )
    {
        int digest_size = EVP_MD_size( digest );
        if( digest_size < 0 )
            throwOpenSSLException( "Failed to get digest size" );
        return digest_size;
    }

private:
    OpenSSLDigest( const OpenSSLDigest& rValue );
    OpenSSLDigest& operator=( const OpenSSLDigest& rValue );

    const EVP_MD*       digest;
    EVP_MD_CTX*         digest_ctx;
};

// ============================================================================

class OpenSSLCipher
{
public:
    OpenSSLCipher() throw ( ::com::sun::star::uno::Exception )
    {
        cipher_ctx = EVP_CIPHER_CTX_new();
        if( cipher_ctx == NULL )
            throwOpenSSLException( "Failed to create cipher context" );
    }

    ~OpenSSLCipher()
    {
        EVP_CIPHER_CTX_free( cipher_ctx );
    }

    void initialize( const EVP_CIPHER *aCipher, const unsigned char *key, const unsigned char *iv, int enc ) throw ( ::com::sun::star::uno::Exception )
    {
        if( 1 != EVP_CipherInit_ex( cipher_ctx, aCipher, NULL, key, iv, enc ) )
            throwOpenSSLException( "Failed to initialize the cipher context for decryption" );
        cipher = aCipher;
    }

    void setPadding( int padding) throw ( ::com::sun::star::uno::Exception )
    {
        if( 1 != EVP_CIPHER_CTX_set_padding( cipher_ctx, padding ) )
            throwOpenSSLException( "Failed to set cipher padding" );
    }

    void update( const unsigned char* dataIn, int dataInSize, unsigned char *dataOut, int *dataOutSize ) throw ( ::com::sun::star::uno::Exception )
    {
        if( 1 != EVP_CipherUpdate( cipher_ctx, dataOut, dataOutSize, dataIn, dataInSize ) )
            throwOpenSSLException( "EVP_CipherUpdate failed" );
    }

    void final( unsigned char *dataOut, int *dataOutSize ) throw ( ::com::sun::star::uno::Exception )
    {
        if( 1 != EVP_CipherFinal( cipher_ctx, dataOut, dataOutSize ) )
            throwOpenSSLException( "EVP_CipherFinal failed" );
    }

    int blockSize()
    {
        return blockSize( cipher );
    }

    static int blockSize( const EVP_CIPHER *cipherAlgorithm )
    {
        return EVP_CIPHER_block_size( cipherAlgorithm );
    }

private:
    OpenSSLCipher( const OpenSSLCipher& rValue );
    OpenSSLCipher& operator=( const OpenSSLCipher& rValue );

    EVP_CIPHER_CTX*     cipher_ctx;
    const EVP_CIPHER*   cipher;
};

// ============================================================================

} // namespace oox

#endif
