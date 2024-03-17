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



#include "oox/core/encryption.hxx"
#include "oox/core/fastparser.hxx"
#include "oox/helper/attributelist.hxx"
#include "oox/helper/helper.hxx"
#include "oox/helper/openssl_wrapper.hxx"

#include <rtl/digest.h>
#include <cppuhelper/implbase1.hxx>
#include <openssl/evp.h>

#include <com/sun/star/io/XStream.hpp>



namespace oox {
namespace core {

// ============================================================================

using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;

using ::com::sun::star::io::XInputStream;
using ::comphelper::SequenceAsHashMap;
using ::rtl::OUString;
using ::std::vector;

// ============================================================================


/* =========================================================================== */
/*  Kudos to Caolan McNamara who provided the core decryption implementation   */
/*  of Standard Encryption (MS-OFFCRYPTO section 2.3.4.5).                     */
/* =========================================================================== */

#define ENCRYPTINFO_CRYPTOAPI        0x00000004U
#define ENCRYPTINFO_DOCPROPS         0x00000008U
#define ENCRYPTINFO_EXTERNAL         0x00000010U
#define ENCRYPTINFO_AES              0x00000020U

#define ENCRYPT_ALGO_AES128          0x0000660EU
#define ENCRYPT_ALGO_AES192          0x0000660FU
#define ENCRYPT_ALGO_AES256          0x00006610U
#define ENCRYPT_ALGO_RC4             0x00006801U

#define ENCRYPT_HASH_SHA1            0x00008004U

class StandardEncryptionInfo : public EncryptionInfo
{
public:
    StandardEncryptionInfo( BinaryInputStream& rStrm ) throw ( Exception );
    ~StandardEncryptionInfo() {}
    bool isImplemented();
    Sequence< NamedValue > verifyPassword( const OUString& rPassword ) throw ( Exception );
    bool verifyEncryptionData( const Sequence< NamedValue >& rEncryptionData ) throw ( Exception );
    bool checkEncryptionData( const sal_uInt8* pnKey, sal_uInt32 nKeySize, const sal_uInt8* pnVerifier, sal_uInt32 nVerifierSize, const sal_uInt8* pnVerifierHash, sal_uInt32 nVerifierHashSize ) throw ( Exception );
    void decryptStream( BinaryXInputStream &aEncryptedPackage, BinaryXOutputStream &aDecryptedPackage ) throw ( Exception );

private:
    sal_uInt8           mpnSalt[ 16 ];
    sal_uInt8           mpnEncrVerifier[ 16 ];
    sal_uInt8           mpnEncrVerifierHash[ 32 ];
    sal_uInt32          mnFlags;
    sal_uInt32          mnAlgorithmId;
    sal_uInt32          mnAlgorithmIdHash;
    sal_uInt32          mnKeySize;
    sal_uInt32          mnSaltSize;
    sal_uInt32          mnVerifierHashSize;
    vector< sal_uInt8>  encryptionKey;
};

StandardEncryptionInfo::StandardEncryptionInfo( BinaryInputStream& rStrm ) throw ( Exception )
{
    char msg[ 1024 ];
    rStrm >> mnFlags;
    if( getFlag( mnFlags, (sal_uInt32) ENCRYPTINFO_EXTERNAL ) )
        throw Exception( OUString::createFromAscii( "EncryptionInfo::readEncryptionInfo() error: \"Extensible encryption\" is not currently supported, please report" ), Reference< XInterface >() );

    sal_uInt32 nHeaderSize, nRepeatedFlags;
    rStrm >> nHeaderSize >> nRepeatedFlags;
    if( nHeaderSize < 20 )
    {
        snprintf( msg, sizeof( msg ), "EncryptionInfo::readEncryptionInfo() error: header size %u is too short", nHeaderSize );
        throw Exception( OUString::createFromAscii( msg ), Reference< XInterface >() );
    }
    if( nRepeatedFlags != mnFlags )
        throw Exception( OUString::createFromAscii( "EncryptionInfo::readEncryptionInfo() error: flags don't match" ), Reference< XInterface>() );

    rStrm.skip( 4 );
    rStrm >> mnAlgorithmId >> mnAlgorithmIdHash >> mnKeySize;
    rStrm.skip( nHeaderSize - 20 );
    rStrm >> mnSaltSize;
    if( mnSaltSize != 16 )
    {
        snprintf( msg, sizeof( msg ), "EncryptionInfo::readEncryptionInfo() error: salt size is %u instead of 16", mnSaltSize );
        throw Exception( OUString::createFromAscii( msg ), Reference< XInterface >() );
    }

    rStrm.readMemory( mpnSalt, 16 );
    rStrm.readMemory( mpnEncrVerifier, 16 );
    rStrm >> mnVerifierHashSize;
    rStrm.readMemory( mpnEncrVerifierHash, 32 );
    if( rStrm.isEof() )
        throw Exception( OUString::createFromAscii( "EncryptionInfo::readEncryptionInfo() error: standard encryption header too short" ), Reference< XInterface >() );
}

bool StandardEncryptionInfo::isImplemented()
{
    return getFlag( mnFlags, (sal_uInt32) ENCRYPTINFO_CRYPTOAPI ) &&
        getFlag( mnFlags, (sal_uInt32) ENCRYPTINFO_AES ) &&
        // algorithm ID 0 defaults to AES128 too, if ENCRYPTINFO_AES flag is set
       ( ( mnAlgorithmId == 0 ) || ( mnAlgorithmId == ENCRYPT_ALGO_AES128 ) ) &&
       // hash algorithm ID 0 defaults to SHA-1 too
       ( ( mnAlgorithmIdHash == 0 ) || ( mnAlgorithmIdHash == ENCRYPT_HASH_SHA1 ) ) &&
       ( mnVerifierHashSize == 20 );
}

static void deriveKey( const sal_uInt8* pnHash, sal_uInt32 nHashLen, sal_uInt8* pnKeyDerived, sal_uInt32 nRequiredKeyLen )
{
    sal_uInt8 pnBuffer[ 64 ];
    memset( pnBuffer, 0x36, sizeof( pnBuffer ) );
    for( sal_uInt32 i = 0; i < nHashLen; ++i )
        pnBuffer[ i ] ^= pnHash[ i ];

    rtlDigest aDigest = rtl_digest_create( rtl_Digest_AlgorithmSHA1 );
    rtlDigestError aError = rtl_digest_update( aDigest, pnBuffer, sizeof( pnBuffer ) );
    sal_uInt8 pnX1[ RTL_DIGEST_LENGTH_SHA1 ];
    aError = rtl_digest_get( aDigest, pnX1, RTL_DIGEST_LENGTH_SHA1 );
    rtl_digest_destroy( aDigest );

    memset( pnBuffer, 0x5C, sizeof( pnBuffer ) );
    for( sal_uInt32 i = 0; i < nHashLen; ++i )
        pnBuffer[ i ] ^= pnHash[ i ];

    aDigest = rtl_digest_create( rtl_Digest_AlgorithmSHA1 );
    aError = rtl_digest_update( aDigest, pnBuffer, sizeof( pnBuffer ) );
    sal_uInt8 pnX2[ RTL_DIGEST_LENGTH_SHA1 ];
    aError = rtl_digest_get( aDigest, pnX2, RTL_DIGEST_LENGTH_SHA1 );
    rtl_digest_destroy( aDigest );

    if( nRequiredKeyLen > RTL_DIGEST_LENGTH_SHA1 )
    {
        memcpy( pnKeyDerived + RTL_DIGEST_LENGTH_SHA1, pnX2, nRequiredKeyLen - RTL_DIGEST_LENGTH_SHA1 );
        nRequiredKeyLen = RTL_DIGEST_LENGTH_SHA1;
    }
    memcpy( pnKeyDerived, pnX1, nRequiredKeyLen );
}

Sequence< NamedValue > StandardEncryptionInfo::verifyPassword( const OUString& rPassword ) throw ( Exception )
{
    size_t nBufferSize = mnSaltSize + 2 * rPassword.getLength();
    sal_uInt8* pnBuffer = new sal_uInt8[ nBufferSize ];
    memcpy( pnBuffer, mpnSalt, mnSaltSize );
 
    sal_uInt8* pnPasswordLoc = pnBuffer + mnSaltSize;
    const sal_Unicode* pStr = rPassword.getStr();
    for( sal_Int32 i = 0, nLen = rPassword.getLength(); i < nLen; ++i, ++pStr, pnPasswordLoc += 2 )
        ByteOrderConverter::writeLittleEndian( pnPasswordLoc, static_cast< sal_uInt16 >( *pStr ) );

    rtlDigest aDigest = rtl_digest_create( rtl_Digest_AlgorithmSHA1 );
    rtlDigestError aError = rtl_digest_update( aDigest, pnBuffer, nBufferSize );
    delete[] pnBuffer;

    size_t nHashSize = RTL_DIGEST_LENGTH_SHA1 + 4;
    sal_uInt8* pnHash = new sal_uInt8[ nHashSize ];
    aError = rtl_digest_get( aDigest, pnHash + 4, RTL_DIGEST_LENGTH_SHA1 );
    rtl_digest_destroy( aDigest );

    for( sal_uInt32 i = 0; i < 50000; ++i )
    {
        ByteOrderConverter::writeLittleEndian( pnHash, i );
        aDigest = rtl_digest_create( rtl_Digest_AlgorithmSHA1 );                                             
        aError = rtl_digest_update( aDigest, pnHash, nHashSize );
        aError = rtl_digest_get( aDigest, pnHash + 4, RTL_DIGEST_LENGTH_SHA1 );
        rtl_digest_destroy( aDigest );
    }

    memmove( pnHash, pnHash + 4, RTL_DIGEST_LENGTH_SHA1 );
    memset( pnHash + RTL_DIGEST_LENGTH_SHA1, 0, 4 );
    aDigest = rtl_digest_create( rtl_Digest_AlgorithmSHA1 );
    aError = rtl_digest_update( aDigest, pnHash, nHashSize );
    aError = rtl_digest_get( aDigest, pnHash, RTL_DIGEST_LENGTH_SHA1 );
    rtl_digest_destroy( aDigest );

    vector< sal_uInt8 > key( mnKeySize / 8 );
    deriveKey( pnHash, RTL_DIGEST_LENGTH_SHA1, &key[ 0 ], key.size() );
    delete[] pnHash;

    Sequence< NamedValue > aResult;
    if( checkEncryptionData( &key[ 0 ], key.size(), mpnEncrVerifier, sizeof( mpnEncrVerifier ), mpnEncrVerifierHash, sizeof( mpnEncrVerifierHash ) ) )
    {
        SequenceAsHashMap aEncryptionData;
        aEncryptionData[ CREATE_OUSTRING( "AES128EncryptionKey" ) ] <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( &key[ 0 ] ), key.size() );
        aEncryptionData[ CREATE_OUSTRING( "AES128EncryptionSalt" ) ] <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( mpnSalt ), mnSaltSize );
        aEncryptionData[ CREATE_OUSTRING( "AES128EncryptionVerifier" ) ] <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( mpnEncrVerifier ), sizeof( mpnEncrVerifier ) );
        aEncryptionData[ CREATE_OUSTRING( "AES128EncryptionVerifierHash" ) ] <<= Sequence< sal_Int8 >( reinterpret_cast< const sal_Int8* >( mpnEncrVerifierHash ), sizeof( mpnEncrVerifierHash ) );
        encryptionKey = key;
        aResult = aEncryptionData.getAsConstNamedValueList();
    }

    return aResult;
}

bool StandardEncryptionInfo::verifyEncryptionData( const Sequence< NamedValue >& rEncryptionData ) throw ( Exception )
{
    SequenceAsHashMap aHashData( rEncryptionData );
    Sequence< sal_Int8 > aKey = aHashData.getUnpackedValueOrDefault( CREATE_OUSTRING( "AES128EncryptionKey" ), Sequence< sal_Int8 >() );
    Sequence< sal_Int8 > aVerifier = aHashData.getUnpackedValueOrDefault( CREATE_OUSTRING( "AES128EncryptionVerifier" ), Sequence< sal_Int8 >() );
    Sequence< sal_Int8 > aVerifierHash = aHashData.getUnpackedValueOrDefault( CREATE_OUSTRING( "AES128EncryptionVerifierHash" ), Sequence< sal_Int8 >() );
    const sal_uInt8 *pnKey = reinterpret_cast< const sal_uInt8* >( aKey.getConstArray() );
    sal_uInt32 nKeySize = aKey.getLength();
    const sal_uInt8 *pnVerifier = reinterpret_cast< const sal_uInt8* >( aVerifier.getConstArray() );
    sal_uInt32 nVerifierSize = aVerifier.getLength();
    const sal_uInt8 *pnVerifierHash = reinterpret_cast< const sal_uInt8* >( aVerifierHash.getConstArray() );
    sal_uInt32 nVerifierHashSize = aVerifierHash.getLength();
    if( checkEncryptionData( pnKey, nKeySize, pnVerifier, nVerifierSize, pnVerifierHash, nVerifierHashSize ) )
    {
        encryptionKey = vector< sal_uInt8 >( &pnKey[ 0 ], &pnKey[ nKeySize ] );
        return true;
    }
    else
        return false;
}

bool StandardEncryptionInfo::checkEncryptionData( const sal_uInt8* pnKey, sal_uInt32 nKeySize, const sal_uInt8* pnVerifier, sal_uInt32 nVerifierSize, const sal_uInt8* pnVerifierHash, sal_uInt32 nVerifierHashSize ) throw ( Exception )
{
    bool bResult = false;
 
    // the only currently supported algorithm needs key size 128
    if ( nKeySize == 16 && nVerifierSize == 16 && nVerifierHashSize == 32 )
    {
        // check password
        EVP_CIPHER_CTX *aes_ctx;
        aes_ctx = EVP_CIPHER_CTX_new();
        if ( aes_ctx == NULL )
            return false;
        EVP_DecryptInit_ex( aes_ctx, EVP_aes_128_ecb(), 0, pnKey, 0 );
        EVP_CIPHER_CTX_set_padding( aes_ctx, 0 );
        int nOutLen = 0;
        sal_uInt8 pnTmpVerifier[ 16 ];
        (void) memset( pnTmpVerifier, 0, sizeof(pnTmpVerifier) );

        /*int*/ EVP_DecryptUpdate( aes_ctx, pnTmpVerifier, &nOutLen, pnVerifier, nVerifierSize );
        EVP_CIPHER_CTX_free( aes_ctx );

        aes_ctx = EVP_CIPHER_CTX_new();
        if ( aes_ctx == NULL )
            return false;
        EVP_DecryptInit_ex( aes_ctx, EVP_aes_128_ecb(), 0, pnKey, 0 );
        EVP_CIPHER_CTX_set_padding( aes_ctx, 0 );
        sal_uInt8 pnTmpVerifierHash[ 32 ];
        (void) memset( pnTmpVerifierHash, 0, sizeof(pnTmpVerifierHash) );
    
        /*int*/ EVP_DecryptUpdate( aes_ctx, pnTmpVerifierHash, &nOutLen, pnVerifierHash, nVerifierHashSize );
        EVP_CIPHER_CTX_free( aes_ctx );
 
        rtlDigest aDigest = rtl_digest_create( rtl_Digest_AlgorithmSHA1 );
        rtlDigestError aError = rtl_digest_update( aDigest, pnTmpVerifier, sizeof( pnTmpVerifier ) );
        sal_uInt8 pnSha1Hash[ RTL_DIGEST_LENGTH_SHA1 ];
        aError = rtl_digest_get( aDigest, pnSha1Hash, RTL_DIGEST_LENGTH_SHA1 );
        rtl_digest_destroy( aDigest );
 
        bResult = ( memcmp( pnSha1Hash, pnTmpVerifierHash, RTL_DIGEST_LENGTH_SHA1 ) == 0 );
    }
        
    return bResult;
}

void StandardEncryptionInfo::decryptStream( BinaryXInputStream &aEncryptedPackage, BinaryXOutputStream &aDecryptedPackage ) throw ( Exception )
{
    EVP_CIPHER_CTX *aes_ctx;
    aes_ctx = EVP_CIPHER_CTX_new();
    if ( aes_ctx == NULL )
        throw Exception();
    EVP_DecryptInit_ex( aes_ctx, EVP_aes_128_ecb(), 0, &encryptionKey[ 0 ], 0 );
    EVP_CIPHER_CTX_set_padding( aes_ctx, 0 );

    sal_uInt8 pnInBuffer[ 1024 ];
    sal_uInt8 pnOutBuffer[ 1024 ];
    sal_Int32 nInLen;
    int nOutLen;
    aEncryptedPackage.skip( 8 ); // decrypted size
    while( (nInLen = aEncryptedPackage.readMemory( pnInBuffer, sizeof( pnInBuffer ) )) > 0 )
    {
        EVP_DecryptUpdate( aes_ctx, pnOutBuffer, &nOutLen, pnInBuffer, nInLen );
        aDecryptedPackage.writeMemory( pnOutBuffer, nOutLen );
    }
    EVP_DecryptFinal_ex( aes_ctx, pnOutBuffer, &nOutLen );
    aDecryptedPackage.writeMemory( pnOutBuffer, nOutLen );

    EVP_CIPHER_CTX_free( aes_ctx );
    aDecryptedPackage.flush();
}

// ============================================================================
// "Agile" encryption, 2.3.4.10 of MS-OFFCRYPTO
// ============================================================================

struct AgileKeyData
{
    sal_Int32 saltSize;
    sal_Int32 blockSize;
    sal_Int32 keyBits;
    sal_Int32 hashSize;
    OUString cipherAlgorithm;
    OUString cipherChaining;
    OUString hashAlgorithm;
    vector< sal_uInt8 > saltValue;
};

struct AgileDataIntegrity
{
    vector< sal_uInt8 > encryptedHmacKey;
    vector< sal_uInt8 > encryptedHmacValue;
};

struct AgilePasswordKeyEncryptor
{
    sal_Int32 saltSize;
    sal_Int32 blockSize;
    sal_Int32 keyBits;
    sal_Int32 hashSize;
    OUString cipherAlgorithm;
    OUString cipherChaining;
    OUString hashAlgorithm;
    vector< sal_uInt8 > saltValue;
    sal_Int32 spinCount;
    vector< sal_uInt8 > encryptedVerifierHashInput;
    vector< sal_uInt8 > encryptedVerifierHashValue;
    vector< sal_uInt8 > encryptedKeyValue;
};

static bool decodeBase64( OUString& base64, vector< sal_uInt8 >& bytes )
{
    ::rtl::OString base64Ascii = ::rtl::OUStringToOString( base64, RTL_TEXTENCODING_UTF8 );
    const sal_uInt32 len = base64Ascii.getLength();
    bytes.resize( (len + 3) / 4 * 3 );
    int decodedSize = EVP_DecodeBlock( &bytes[ 0 ], reinterpret_cast< sal_uInt8 const * >( base64Ascii.getStr() ), len );
    if ( decodedSize < 0 )
        return false;
    if ( len >= 2 && base64Ascii[ len-1 ] == '=' && base64Ascii[ len-2 ] == '=' )
        decodedSize -= 2;
    else if ( len >= 1 && base64Ascii[ len-1] == '=' )
        decodedSize--;
    bytes.resize( decodedSize );
    return true;
}

class AgileEncryptionInfo : public EncryptionInfo
{
public:
    AgileEncryptionInfo( const Reference< XComponentContext >& context, Reference< XInputStream >& inputStream ) throw ( Exception );
    ~AgileEncryptionInfo() {}
    bool isImplemented() { return true; } // FIXME
    Sequence< NamedValue > verifyPassword( const OUString& rPassword ) throw ( Exception );
    bool verifyEncryptionData( const Sequence< NamedValue >& rEncryptionData ) throw ( Exception );
    void decryptStream( BinaryXInputStream &aEncryptedPackage, BinaryXOutputStream &aDecryptedPackage ) throw ( Exception );

private:
    AgileKeyData keyData;
    AgileDataIntegrity dataIntegrity;
    AgilePasswordKeyEncryptor passwordKeyEncryptor;
    vector< sal_uInt8> encryptionKey;
    vector< sal_uInt8> hmacKey;
    vector< sal_uInt8> hmacValue;
};

// A SAX handler that parses the XML from the "XmlEncryptionDescriptor" in the EncryptionInfo stream.
class AgileEncryptionHandler : public ::cppu::WeakImplHelper1< XFastDocumentHandler >
{
public:
    AgileEncryptionHandler( AgileKeyData &aKeyData, AgileDataIntegrity &aDataIntegrity, AgilePasswordKeyEncryptor &aPasswordKeyEncryptor )
    : keyData( aKeyData ),
      dataIntegrity( aDataIntegrity ),
      passwordKeyEncryptor( aPasswordKeyEncryptor )
    {
    }

    // XFastDocumentHandler
    virtual void SAL_CALL startDocument() throw (SAXException, RuntimeException);
    virtual void SAL_CALL endDocument() throw (SAXException, RuntimeException);
    virtual void SAL_CALL setDocumentLocator( const Reference< XLocator >& xLocator ) throw (SAXException, RuntimeException);

    // XFastContextHandler
    virtual void SAL_CALL startFastElement( sal_Int32 nElement, const Reference< XFastAttributeList >& Attribs ) throw (SAXException, RuntimeException);
    virtual void SAL_CALL startUnknownElement( const OUString& Namespace, const OUString& Name, const Reference< XFastAttributeList >& Attribs ) throw (SAXException, RuntimeException);
    virtual void SAL_CALL endFastElement( sal_Int32 Element ) throw (SAXException, RuntimeException);
    virtual void SAL_CALL endUnknownElement( const OUString& Namespace, const OUString& Name ) throw (SAXException, RuntimeException);
    virtual Reference< XFastContextHandler > SAL_CALL createFastChildContext( sal_Int32 Element, const Reference< XFastAttributeList >& Attribs ) throw (SAXException, RuntimeException);
    virtual Reference< XFastContextHandler > SAL_CALL createUnknownChildContext( const OUString& Namespace, const OUString& Name, const Reference< XFastAttributeList >& Attribs ) throw (SAXException, RuntimeException);
    virtual void SAL_CALL characters( const OUString& aChars ) throw (SAXException, RuntimeException);
    virtual void SAL_CALL ignorableWhitespace( const OUString& aWhitespaces ) throw (SAXException, RuntimeException);
    virtual void SAL_CALL processingInstruction( const OUString& aTarget, const OUString& aData ) throw (SAXException, RuntimeException);

    OUString& getLastError() { return lastError; }

private:
    void parseKeyData( const AttributeList& attribs ) throw (SAXException, RuntimeException);
    void parseDataIntegrity( const AttributeList& attribs ) throw (SAXException, RuntimeException);
    void parseEncryptedKey( const AttributeList& attribs ) throw (SAXException, RuntimeException);

    vector< sal_Int32 > stack;
    OUString lastError;
    AgileKeyData &keyData;
    AgileDataIntegrity &dataIntegrity;
    AgilePasswordKeyEncryptor &passwordKeyEncryptor;
};

void AgileEncryptionHandler::startDocument()
    throw ( SAXException, RuntimeException )
{
}

void AgileEncryptionHandler::endDocument()
    throw ( SAXException, RuntimeException )
{
}

void AgileEncryptionHandler::setDocumentLocator( const Reference< XLocator >& )
    throw ( SAXException, RuntimeException )
{
}

void AgileEncryptionHandler::startFastElement( sal_Int32 nElement, const Reference< XFastAttributeList >& attribs )
    throw( SAXException, RuntimeException )
{
    switch ( nElement )
    {
        case ENCRYPTION_TOKEN( encryption ):
        break;

        case ENCRYPTION_TOKEN( keyData ):
            if ( stack.size() == 1 && (stack[ 0 ] == ENCRYPTION_TOKEN( encryption )) )
                parseKeyData( AttributeList( attribs ) );
        break;

        case ENCRYPTION_TOKEN( dataIntegrity ):
            if ( stack.size() == 1 && (stack[ 0 ] == ENCRYPTION_TOKEN( encryption )) )
                parseDataIntegrity( AttributeList ( attribs ) );
        break;

        case ENCRYPTION_TOKEN( keyEncryptors ):
        break;

        case ENCRYPTION_TOKEN( keyEncryptor ):
        break;

        case KEY_ENCRYPTOR_PASSWORD_TOKEN( encryptedKey ):
            if ( stack.size() == 3
                && (stack[ 0 ] == ENCRYPTION_TOKEN( encryption ))
                && (stack[ 1 ] == ENCRYPTION_TOKEN( keyEncryptors ))
                && (stack[ 2 ] == ENCRYPTION_TOKEN( keyEncryptor )) )
                parseEncryptedKey( AttributeList ( attribs ) );
        break;
    }
    stack.push_back( nElement );
}

void AgileEncryptionHandler::startUnknownElement( const OUString&, const OUString&, const Reference< XFastAttributeList >& )
    throw( SAXException, RuntimeException )
{
    stack.push_back( -1 );
}

void AgileEncryptionHandler::endFastElement( sal_Int32 nElement )
    throw( SAXException, RuntimeException )
{
    stack.pop_back();
}

void AgileEncryptionHandler::endUnknownElement( const OUString&, const OUString& )
    throw( SAXException, RuntimeException )
{
    stack.pop_back();
}

Reference< XFastContextHandler > AgileEncryptionHandler::createFastChildContext( sal_Int32, const Reference< XFastAttributeList >& )
    throw (SAXException, RuntimeException)
{
    return this;
}

Reference< XFastContextHandler > AgileEncryptionHandler::createUnknownChildContext( const OUString&, const OUString&, const Reference< XFastAttributeList >& )
    throw (SAXException, RuntimeException)
{
    return this;
}

void AgileEncryptionHandler::characters( const ::rtl::OUString& rStr )
    throw( SAXException, RuntimeException )
{
}

void AgileEncryptionHandler::ignorableWhitespace( const ::rtl::OUString& str )
    throw( SAXException, RuntimeException )
{
}

void AgileEncryptionHandler::processingInstruction( const ::rtl::OUString& aTarget, const ::rtl::OUString& aData )
    throw( SAXException, RuntimeException )
{
}

void AgileEncryptionHandler::parseKeyData( const AttributeList& attribs )
    throw ( SAXException, RuntimeException )
{
    keyData.saltSize = attribs.getInteger( XML_saltSize, 0 );
    keyData.blockSize = attribs.getInteger( XML_blockSize, 0 );
    keyData.keyBits = attribs.getInteger( XML_keyBits, 0 );
    keyData.hashSize = attribs.getInteger( XML_hashSize, 0 );
    keyData.cipherAlgorithm = attribs.getString( XML_cipherAlgorithm, OUString() );
    keyData.cipherChaining = attribs.getString( XML_cipherChaining, OUString() );
    keyData.hashAlgorithm = attribs.getString( XML_hashAlgorithm, OUString() );

    OUString saltValue = attribs.getString( XML_saltValue, OUString() );
    if( !decodeBase64( saltValue, keyData.saltValue ) )
        lastError = OUString::createFromAscii( "Failed to base64 decode the keyData.saltValue " ) + saltValue;
}

void AgileEncryptionHandler::parseDataIntegrity( const AttributeList& attribs )
    throw ( SAXException, RuntimeException )
{
    OUString encryptedHmacKey = attribs.getString( XML_encryptedHmacKey, OUString() );
    if( !decodeBase64( encryptedHmacKey, dataIntegrity.encryptedHmacKey ) )
        lastError = OUString::createFromAscii( "Failed to base64 decode the dataIntegrity.encryptedHmacKey " ) + encryptedHmacKey;
    OUString encryptedHmacValue = attribs.getString( XML_encryptedHmacValue, OUString() );
    if( !decodeBase64( encryptedHmacValue, dataIntegrity.encryptedHmacValue ) )
        lastError = OUString::createFromAscii( "Failed to base64 decode the dataIntegrity.encryptedHmacValue " ) + encryptedHmacValue;
}

void AgileEncryptionHandler::parseEncryptedKey( const AttributeList& attribs )
    throw ( SAXException, RuntimeException )
{
    passwordKeyEncryptor.spinCount = attribs.getInteger( XML_spinCount, 0 );
    passwordKeyEncryptor.saltSize = attribs.getInteger( XML_saltSize, 0 );
    passwordKeyEncryptor.blockSize = attribs.getInteger( XML_blockSize, 0 );
    passwordKeyEncryptor.keyBits = attribs.getInteger( XML_keyBits, 0 );
    passwordKeyEncryptor.hashSize = attribs.getInteger( XML_hashSize, 0 );
    passwordKeyEncryptor.cipherAlgorithm = attribs.getString( XML_cipherAlgorithm, OUString() );
    passwordKeyEncryptor.cipherChaining = attribs.getString( XML_cipherChaining, OUString() );
    passwordKeyEncryptor.hashAlgorithm = attribs.getString( XML_hashAlgorithm, OUString() );
    OUString saltValue = attribs.getString( XML_saltValue, OUString() );
    if( !decodeBase64( saltValue, passwordKeyEncryptor.saltValue ) )
        lastError = OUString::createFromAscii( "Failed to base64 decode the passwordKeyEncryptor.saltValue " ) + saltValue;
    OUString encryptedVerifierHashInput = attribs.getString( XML_encryptedVerifierHashInput, OUString() );
    if( !decodeBase64( encryptedVerifierHashInput, passwordKeyEncryptor.encryptedVerifierHashInput ) )
        lastError = OUString::createFromAscii( "Failed to base64 decode the passwordKeyEncryptor.encryptedVerifierHashInput " ) + encryptedVerifierHashInput;
    OUString encryptedVerifierHashValue = attribs.getString( XML_encryptedVerifierHashValue, OUString() );
    if( !decodeBase64( encryptedVerifierHashValue, passwordKeyEncryptor.encryptedVerifierHashValue ) )
        lastError = OUString::createFromAscii( "Failed to base64 decode the passwordKeyEncryptor.encryptedVerifierHashValue " ) + encryptedVerifierHashValue;
    OUString encryptedKeyValue = attribs.getString( XML_encryptedKeyValue, OUString() );
    if( !decodeBase64( encryptedKeyValue, passwordKeyEncryptor.encryptedKeyValue ) )
        lastError = OUString::createFromAscii( "Failed to base64 decode the passwordKeyEncryptor.encryptedKeyValue " ) + encryptedKeyValue;
}

static sal_uInt16 readUInt16LE( Reference< XInputStream >& inputStream ) throw ( Exception )
{
    Sequence< sal_Int8 > bytes( 2 );
    sal_Int32 bytesRead = inputStream->readBytes( bytes, 2 );
    if( bytesRead < 2 )
        throw new Exception( OUString::createFromAscii( "EncryptionInfo::readEncryptionInfo() failed, early end of file" ), Reference< XInterface >() );
    return (sal_uInt16) ( bytes[0] | (bytes[1] << 8) );
}

static sal_uInt32 readUInt32LE( Reference< XInputStream >& inputStream ) throw ( Exception )
{
    Sequence< sal_Int8 > bytes( 4 );
    sal_Int32 bytesRead = inputStream->readBytes( bytes, 4 );
    if( bytesRead < 4 )
        throw new Exception( OUString::createFromAscii( "EncryptionInfo::readEncryptionInfo() failed, early end of file" ), Reference< XInterface >() );
    return (sal_uInt32) ( bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24) );
}

AgileEncryptionInfo::AgileEncryptionInfo( const Reference< XComponentContext >& context, Reference< XInputStream >& inputStream ) throw ( Exception )
{
    sal_uInt32 nReserved = readUInt32LE( inputStream );
    if( nReserved != 0x40 )
        throw new Exception( OUString::createFromAscii( "reserved field isn't 0x40" ), Reference< XInterface >() );
    AgileEncryptionHandler *agileEncryptionHandler = new AgileEncryptionHandler( keyData, dataIntegrity, passwordKeyEncryptor );
    Reference< XFastDocumentHandler > documentHandler( agileEncryptionHandler );
    FastParser fastParser( context );
    fastParser.registerNamespace( NMSP_encryption );
    fastParser.registerNamespace( NMSP_keyEncryptorPassword );
    fastParser.setDocumentHandler( documentHandler );
    fastParser.parseStream( inputStream, OUString::createFromAscii( "EncryptionInfo" ), false );
    if( !agileEncryptionHandler->getLastError().isEmpty() )
        throw new Exception( agileEncryptionHandler->getLastError(), Reference< XInterface >() );
}

static const EVP_MD* toOpenSSLDigestAlgorithm( const OUString& hashAlgorithm  ) throw ( Exception )
{
    if( hashAlgorithm.equalsAscii( "SHA-1" ) )
        return EVP_sha1();
    else if( hashAlgorithm.equalsAscii( "SHA1" ) ) // Typical Microsoft. The specification says "SHA-1", but documents use "SHA1".
        return EVP_sha1();
    else if( hashAlgorithm.equalsAscii( "SHA256" ) )
        return EVP_sha256();
    else if( hashAlgorithm.equalsAscii( "SHA384" ) )
        return EVP_sha384();
    else if( hashAlgorithm.equalsAscii( "SHA512" ) )
        return EVP_sha512();
    else if( hashAlgorithm.equalsAscii( "MD5" ) )
        return EVP_md5();
    else if( hashAlgorithm.equalsAscii( "MD4" ) )
        return EVP_md4();
#if !defined(OPENSSL_NO_MD2)
    else if( hashAlgorithm.equalsAscii( "MD2" ) )
        return EVP_md2();
#endif
    else if( hashAlgorithm.equalsAscii( "RIPEMD-160" ) )
        return EVP_ripemd160();
    else if( hashAlgorithm.equalsAscii( "WHIRLPOOL" ) )
        return EVP_whirlpool();
    char buffer[ 256 ];
    ::rtl::OString str = ::rtl::OUStringToOString( hashAlgorithm, RTL_TEXTENCODING_UTF8 );
    snprintf( buffer, sizeof( buffer ), "Unsupported digest algorithm %s", str.getStr() );
    throw Exception( OUString::createFromAscii( buffer ), Reference< XInterface >() );
}

static const EVP_CIPHER* toOpenSSLCipherAlgorithm( const OUString& cipherName, sal_uInt32 keyBits, const OUString &chainingMode ) throw ( Exception )
{
    if( cipherName.equalsAscii( "AES" ) && keyBits == 128 && chainingMode.equalsAscii( "ChainingModeCBC" ) )
        return EVP_aes_128_cbc();
    else if( cipherName.equalsAscii( "AES" ) && keyBits == 128 && chainingMode.equalsAscii( "ChainingModeCFB" ) )
        return EVP_aes_128_cfb();
    else if( cipherName.equalsAscii( "AES" ) && keyBits == 192 && chainingMode.equalsAscii( "ChainingModeCBC" ) )
        return EVP_aes_192_cbc();
    else if( cipherName.equalsAscii( "AES" ) && keyBits == 192 && chainingMode.equalsAscii( "ChainingModeCFB" ) )
        return EVP_aes_192_cfb();
    else if( cipherName.equalsAscii( "AES" ) && keyBits == 256 && chainingMode.equalsAscii( "ChainingModeCBC" ) )
        return EVP_aes_256_cbc();
    else if( cipherName.equalsAscii( "AES" ) && keyBits == 256 && chainingMode.equalsAscii( "ChainingModeCFB" ) )
        return EVP_aes_256_cfb();
#if !defined(OPENSSL_NO_RC2)
    else if( cipherName.equalsAscii( "RC2" ) && keyBits == 128 && chainingMode.equalsAscii( "ChainingModeCBC" ) )
        return EVP_rc2_cbc();
    else if( cipherName.equalsAscii( "RC2" ) && keyBits == 128 && chainingMode.equalsAscii( "ChainingModeCFB" ) )
        return EVP_rc2_cfb();
#endif
#if !defined(OPENSSL_NO_DES)
    else if( cipherName.equalsAscii( "DES" ) && keyBits == 56 && chainingMode.equalsAscii( "ChainingModeCBC" ) )
        return EVP_des_cbc();
    else if( cipherName.equalsAscii( "DES" ) && keyBits == 56 && chainingMode.equalsAscii( "ChainingModeCFB" ) )
        return EVP_des_cfb();
    else if( cipherName.equalsAscii( "DESX" ) && keyBits == 128 && chainingMode.equalsAscii( "ChainingModeCBC" ) )
        return EVP_desx_cbc();
    else if( cipherName.equalsAscii( "3DES" ) && keyBits == 168 && chainingMode.equalsAscii( "ChainingModeCBC" ) )
        return EVP_des_ede3_cbc();
    else if( cipherName.equalsAscii( "3DES" ) && keyBits == 168 && chainingMode.equalsAscii( "ChainingModeCFB" ) )
        return EVP_des_ede3_cfb();
    else if( cipherName.equalsAscii( "3DES_112" ) && keyBits == 112 && chainingMode.equalsAscii( "ChainingModeCBC" ) )
        return EVP_des_ede_cbc();
    else if( cipherName.equalsAscii( "3DES_112" ) && keyBits == 112 && chainingMode.equalsAscii( "ChainingModeCFB" ) )
        return EVP_des_ede_cfb();
#endif
    char buffer[ 256 ];
    ::rtl::OString cipherNameUtf8 = ::rtl::OUStringToOString( cipherName, RTL_TEXTENCODING_UTF8 );
    ::rtl::OString chainingModeUtf8 = ::rtl::OUStringToOString( chainingMode, RTL_TEXTENCODING_UTF8 );
    snprintf( buffer, sizeof( buffer ), "Unsupported cipher with name=%s, keyBits=%u, chainingMode=%s", cipherNameUtf8.getStr(), keyBits, chainingModeUtf8.getStr() );
    throw Exception( OUString::createFromAscii( buffer ), Reference< XInterface >() );
}

// Ported from Apache POI's org.apache.poi.poifs.crypt.CryptoFunctions.hashPassword().
static vector< sal_uInt8 > hashPassword( const OUString& password, const EVP_MD *digestAlgorithm, vector< sal_uInt8 >& salt, sal_uInt32 spinCount ) throw ( Exception )
{
    OpenSSLDigest digest;
    digest.initialize( digestAlgorithm );
    size_t digestSize = digest.digestSize();

    // Convert to little-endian UTF-16
    vector< sal_uInt8 > passwordLE( 2 * password.getLength() );
    for ( int i = 0; i < password.getLength(); i++ )
        ByteOrderConverter::writeLittleEndian( &passwordLE[ 2 * i ], static_cast< sal_uInt16 >( password[ i ] ) );

    vector< sal_uInt8> digestBuffer( digestSize );
    digest.update( &salt[ 0 ], salt.size() );
    digest.update( &passwordLE[ 0 ], passwordLE.size() );
    digest.final( &digestBuffer[ 0 ], NULL );

    char iteratorBuffer[ 4 ];
    for (sal_uInt32 i = 0; i < spinCount; i++)
    {
        digest.initialize( digestAlgorithm );
        ByteOrderConverter::writeLittleEndian( &iteratorBuffer, i );
        digest.update( iteratorBuffer, sizeof( iteratorBuffer ) );
        digest.update( &digestBuffer[ 0 ], digestSize );
        digest.final( &digestBuffer[ 0 ], NULL );
    }
    return digestBuffer;
}

// Ported from Apache POI's org.apache.poi.poifs.crypt.CryptoFunctions.getBlock36().
static void toBlock36( vector< sal_uInt8 >& digest, sal_uInt32 size )
{
    if( digest.size() < size )
    {
        sal_uInt32 i = digest.size();
        digest.resize( size );
        for (; i < size; i++)
            digest[ i ] = 0x36;
    }
    else
        digest.resize( size );
}

// Ported from Apache POI's org.apache.poi.poifs.crypt.CryptoFunctions.getBlock0().
static void toBlock0( vector< sal_uInt8 >& digest, sal_uInt32 size )
{
    if( digest.size() < size )
    {
        sal_uInt32 i = digest.size();
        digest.resize( size );
        for (; i < size; i++)
            digest[ i ] = 0;
    }
    else
        digest.resize( size );
}

// Ported from Apache POI's org.apache.poi.poifs.crypt.CryptoFunctions.generateKey().
static vector< sal_uInt8 > generateKey( const vector< sal_uInt8 >& passwordHash,
                                        const EVP_MD *digestAlgorithm,
                                        const vector< sal_uInt8 >& blockKey,
                                        sal_uInt32 keySize )
    throw ( Exception )
{
    OpenSSLDigest digest;
    digest.initialize( digestAlgorithm );
    digest.update( &passwordHash[ 0 ], passwordHash.size() );
    digest.update( &blockKey[ 0 ], blockKey.size() );
    vector< sal_uInt8> key( digest.digestSize() );
    digest.final( &key[ 0 ], NULL );
    toBlock36( key, keySize );
    return key;
}

// Ported from Apache POI's org.apache.poi.poifs.crypt.CryptoFunctions.generateIv().
static vector< sal_uInt8> generateIv( const vector< sal_uInt8 >& salt,
                                      sal_uInt32 blockSize )
    throw ( Exception )
{
    vector< sal_uInt8> iv( salt );
    toBlock36( iv, blockSize );
    return iv;
}

// Ported from Apache POI's org.apache.poi.poifs.crypt.CryptoFunctions.generateIv().
static vector< sal_uInt8> generateIv( const EVP_MD *digestAlgorithm,
                                      const vector< sal_uInt8 >& salt,
                                      const vector< sal_uInt8 >& blockKey,
                                      sal_uInt32 blockSize )
    throw ( Exception )
{
    OpenSSLDigest digest;
    digest.initialize( digestAlgorithm );
    digest.update( &salt[ 0 ], salt.size() );
    digest.update( &blockKey[ 0 ], blockKey.size() );
    vector< sal_uInt8> iv( digest.digestSize() );
    digest.final( &iv[ 0 ], NULL );
    toBlock36( iv, blockSize );
    return iv;
}

// Ported from Apache POI's org.apache.poi.poifs.crypt.agile.AgileDecryptor.getNextBlockSize().
static sal_uInt32 getNextBlockSize( sal_uInt32 totalSize, sal_uInt32 blockSize )
{
    sal_uInt32 numberOfBlocks = ( totalSize + ( blockSize - 1 ) ) / blockSize;
    return numberOfBlocks * blockSize;
}

static vector< sal_uInt8 > decryptAll( const EVP_CIPHER* cipherAlgorithm,
                                       const sal_uInt8* iv,
                                       const sal_uInt8* key,
                                       const sal_uInt8* encryptedData,
                                       sal_uInt32 encryptedDataLength )
    throw ( Exception )
{
    OpenSSLCipher cipher;
    cipher.initialize( cipherAlgorithm, key, iv, 0 );
    cipher.setPadding( 0 );
    const int blockSize = cipher.blockSize();
    vector< sal_uInt8 > decryptedData( encryptedDataLength + 2*blockSize );

    int decryptedDataLength;
    cipher.update( encryptedData, encryptedDataLength, &decryptedData[ 0 ], &decryptedDataLength );
    int finalDataLength;
    cipher.final( &decryptedData[ decryptedDataLength ], &finalDataLength );
    decryptedDataLength += finalDataLength;
    decryptedData.resize( decryptedDataLength );
    return decryptedData;
}

// Ported from Apache POI's org.apache.poi.poifs.crypt.agile.AgileDecryptor.hashInput().
static vector< sal_uInt8 > hashInput( const vector< sal_uInt8 >& passwordHash,
                                      const vector< sal_uInt8 >& salt,
                                      const EVP_MD *digestAlgorithm,
                                      const vector< sal_uInt8 >& blockKey,
                                      const vector< sal_uInt8 >& inputKey,
                                      const EVP_CIPHER *decryptionAlgorithm,
                                      sal_uInt32 keySize,
                                      sal_uInt32 blockSize )
    throw ( Exception )
{
    vector< sal_uInt8 > intermediateKey = generateKey( passwordHash, digestAlgorithm, blockKey, keySize );
    vector< sal_uInt8> iv = generateIv( salt, blockSize );
    vector< sal_uInt8 > zeroedInput( inputKey.size() );
    zeroedInput = inputKey;
    toBlock0( zeroedInput, getNextBlockSize( zeroedInput.size(), blockSize ) );
    return decryptAll( decryptionAlgorithm, &iv[ 0 ], &intermediateKey[ 0 ], &zeroedInput[ 0 ], zeroedInput.size() );
}

// Ported from Apache POI's org.apache.poi.poifs.crypt.agile.AgileDecryptor.verifyPassword().
Sequence< NamedValue > AgileEncryptionInfo::verifyPassword( const OUString& password )
    throw ( Exception )
{
    const EVP_MD *digestAlgorithm = toOpenSSLDigestAlgorithm( passwordKeyEncryptor.hashAlgorithm );
    vector< sal_uInt8 > passwordHash = hashPassword( password, digestAlgorithm, passwordKeyEncryptor.saltValue, passwordKeyEncryptor.spinCount );

    static const sal_uInt8 verifierInputBlockData[] = { 0xfe, 0xa7, 0xd2, 0x76, 0x3b, 0x4b, 0x9e, 0x79 };
    vector< sal_uInt8 > verifierInputBlock( &verifierInputBlockData[ 0 ], &verifierInputBlockData[ sizeof( verifierInputBlockData ) ] );
    const EVP_CIPHER* cipher = toOpenSSLCipherAlgorithm( passwordKeyEncryptor.cipherAlgorithm, passwordKeyEncryptor.keyBits, passwordKeyEncryptor.cipherChaining );
    vector< sal_uInt8 > encryptedVerifierHash = hashInput( passwordHash, passwordKeyEncryptor.saltValue, digestAlgorithm, verifierInputBlock,
                                                           passwordKeyEncryptor.encryptedVerifierHashInput, cipher, passwordKeyEncryptor.keyBits,
                                                           passwordKeyEncryptor.blockSize );
    const EVP_MD *verifierDigestAlgorithm = toOpenSSLDigestAlgorithm( keyData.hashAlgorithm );
    OpenSSLDigest verifierDigest;
    verifierDigest.initialize( verifierDigestAlgorithm );
    verifierDigest.update( &encryptedVerifierHash[ 0 ], encryptedVerifierHash.size() );
    encryptedVerifierHash.resize( verifierDigest.digestSize() );
    verifierDigest.final( &encryptedVerifierHash[ 0 ], NULL );

    static const sal_uInt8 verifierHashBlockData[] = { 0xd7, 0xaa, 0x0f, 0x6d, 0x30, 0x61, 0x34, 0x4e };
    vector< sal_uInt8 > verifierHashBlock( &verifierHashBlockData[ 0 ], &verifierHashBlockData[ sizeof( verifierHashBlockData ) ] );
    vector< sal_uInt8 > verifierHashDec = hashInput( passwordHash, passwordKeyEncryptor.saltValue, digestAlgorithm, verifierHashBlock,
                                                     passwordKeyEncryptor.encryptedVerifierHashValue, cipher, passwordKeyEncryptor.keyBits,
                                                     passwordKeyEncryptor.blockSize );
    toBlock0( verifierHashDec, verifierDigest.digestSize() );

    if( encryptedVerifierHash != verifierHashDec )
        return Sequence< NamedValue >();

    // Password is correct. Decrypt and store the encryption key.
    static const sal_uInt8 cryptoKeyBlockData[] = { 0x14, 0x6e, 0x0b, 0xe7, 0xab, 0xac, 0xd0, 0xd6 };
    vector< sal_uInt8 > cryptoKeyBlock( &cryptoKeyBlockData[ 0 ], &cryptoKeyBlockData[ sizeof( cryptoKeyBlockData ) ] );
    encryptionKey = hashInput( passwordHash, passwordKeyEncryptor.saltValue, digestAlgorithm, cryptoKeyBlock,
                               passwordKeyEncryptor.encryptedKeyValue, cipher, passwordKeyEncryptor.keyBits,
                               passwordKeyEncryptor.blockSize );
    toBlock0( encryptionKey, passwordKeyEncryptor.keyBits / 8 );

    // Also decrypt the dataIntegrity fields for stream validation. Note that they are optional.
    if( !dataIntegrity.encryptedHmacKey.empty() && !dataIntegrity.encryptedHmacValue.empty() )
    {
        const EVP_MD* keyDataDigestAlgorithm = toOpenSSLDigestAlgorithm( keyData.hashAlgorithm );
        const EVP_CIPHER* keyDataCipher = toOpenSSLCipherAlgorithm( keyData.cipherAlgorithm, keyData.keyBits, keyData.cipherChaining );
        static const sal_uInt8 integrityKeyBlockData[] = { 0x5f, 0xb2, 0xad, 0x01, 0x0c, 0xb9, 0xe1, 0xf6 };
        vector< sal_uInt8 > integrityKeyBlock( &integrityKeyBlockData[ 0 ], &integrityKeyBlockData[ sizeof( integrityKeyBlockData ) ] );
        vector< sal_uInt8 > integrityKeyIv = generateIv( keyDataDigestAlgorithm, keyData.saltValue, integrityKeyBlock, keyData.blockSize );
        hmacKey = decryptAll( keyDataCipher, &integrityKeyIv[ 0 ], &encryptionKey[ 0 ], &dataIntegrity.encryptedHmacKey[ 0 ], dataIntegrity.encryptedHmacKey.size() );
        toBlock0( hmacKey, OpenSSLDigest::digestSize( keyDataDigestAlgorithm ) );

        static const sal_uInt8 integrityValueBlockData[] = { 0xa0, 0x67, 0x7f, 0x02, 0xb2, 0x2c, 0x84, 0x33 };
        vector< sal_uInt8 > integrityValueBlock( &integrityValueBlockData[ 0 ], &integrityValueBlockData[ sizeof( integrityValueBlockData ) ] );
        vector< sal_uInt8 > integrityValueIv = generateIv( keyDataDigestAlgorithm, keyData.saltValue, integrityValueBlock, keyData.blockSize );
        hmacValue = decryptAll( keyDataCipher, &integrityValueIv[ 0 ], &encryptionKey[ 0 ], &dataIntegrity.encryptedHmacValue[ 0 ], dataIntegrity.encryptedHmacValue.size() );
        toBlock0( hmacValue, OpenSSLDigest::digestSize( keyDataDigestAlgorithm ) );
    }

    // On success, MUST populate something into the encryption data, even though we'll never use it.
    SequenceAsHashMap encryptionData;
    encryptionData[ CREATE_OUSTRING( "OOXMLAgileEncryptionPasswordVerified" ) ] <<= sal_True;
    return encryptionData.getAsConstNamedValueList();
}

bool AgileEncryptionInfo::verifyEncryptionData( const Sequence< NamedValue >& rEncryptionData )
    throw ( Exception )
{
    // OpenGrok shows how only main/comphelper/source/misc/docpasswordhelper.cxx calls IDocPasswordVerifier::verifyEncryptionData(),
    // and only when the password is wrong and the rMediaEncData non-empty, which presumably allows other forms of encryption
    // (eg. by certificate) to be used. We only support password for now.
    return false;
}

// Ported from Apache POI's org.apache.poi.poifs.crypt.agile.AgileDecryptor.initCipherForBlock().
void AgileEncryptionInfo::decryptStream( BinaryXInputStream &aEncryptedPackage, BinaryXOutputStream &aDecryptedPackage )
    throw ( Exception )
{
    if( encryptionKey.empty() )
        throw Exception( OUString::createFromAscii( "Encryption key not set, was the password wrong?" ), Reference< XInterface >() );
    const EVP_CIPHER* cipherAlgorithm = toOpenSSLCipherAlgorithm( keyData.cipherAlgorithm, keyData.keyBits, keyData.cipherChaining );
    const EVP_MD* digestAlgorithm = toOpenSSLDigestAlgorithm( keyData.hashAlgorithm );
    OpenSSLCipher cipher;

    const sal_uInt64 decryptedSize = aEncryptedPackage.readuInt64();

    sal_uInt8 inputBuffer[ 4096 ];
    vector< sal_uInt8 > outputBuffer( 4096 + 2*cipher.blockSize() );
    sal_Int32 bytesIn;
    int bytesOut;
    int finalBytesOut;
    sal_uInt64 totalBytesWritten = 0;

    vector< sal_uInt8 > blockBytes( 4 );
    bool done = false;
    for ( sal_uInt32 block = 0; !done; block++ )
    {
        ByteOrderConverter::writeLittleEndian( &blockBytes[ 0 ], block );
        vector< sal_uInt8 > iv = generateIv( digestAlgorithm, keyData.saltValue, blockBytes, keyData.blockSize );
        cipher.initialize( cipherAlgorithm, &encryptionKey[ 0 ], &iv[ 0 ], 0 );
        cipher.setPadding( 0 );

        bytesIn = aEncryptedPackage.readMemory( inputBuffer, sizeof( inputBuffer ) );
        if( bytesIn > 0 )
        {
            cipher.update( inputBuffer, bytesIn, &outputBuffer[ 0 ], &bytesOut );
            cipher.final( &outputBuffer[ bytesOut ], &finalBytesOut );
            bytesOut += finalBytesOut;
            if( decryptedSize < (totalBytesWritten + bytesOut) )
            {
                bytesOut = decryptedSize % sizeof( inputBuffer );
                done = true;
            }
            aDecryptedPackage.writeMemory( &outputBuffer[ 0 ], bytesOut );
            totalBytesWritten += bytesOut;
        } else
            done = true;
    }

    aDecryptedPackage.flush();
}

EncryptionInfo* EncryptionInfo::readEncryptionInfo( const Reference< XComponentContext >& context, Reference< XInputStream >& inputStream )
    throw ( Exception )
{
    sal_uInt16 nVersionMajor = readUInt16LE( inputStream );
    sal_uInt16 nVersionMinor = readUInt16LE( inputStream );
    if( ( nVersionMajor == 2 && nVersionMinor == 2 ) ||
        ( nVersionMajor == 3 && nVersionMinor == 2 ) ||
        ( nVersionMajor == 4 && nVersionMinor == 2 ) )
    {
        // 2.3.4.5 Standard Encryption
        BinaryXInputStream aInfoStrm( inputStream, false );
        return new StandardEncryptionInfo( aInfoStrm );
    }
    else if ( nVersionMajor == 4 && nVersionMajor == 4 )
    {
        // 2.3.4.10 Agile Encryption
        return new AgileEncryptionInfo( context, inputStream );
    }
    else
    {
        char msg[ 1024 ];
        snprintf( msg, sizeof( msg ), "EncryptionInfo::readEncryptionInfo() error: unsupported EncryptionVersionInfo header with major=%hu minor=%hu",
            nVersionMajor, nVersionMinor );
        throw Exception( OUString::createFromAscii( msg ), Reference< XInterface >() );
    }
}

// ============================================================================

} // namespace core
} // namespace oox
