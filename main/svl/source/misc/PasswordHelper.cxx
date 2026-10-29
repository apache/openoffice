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
#include "precompiled_svl.hxx"

#include <svl/PasswordHelper.hxx>
#include <rtl/digest.h>
#include <tools/string.hxx>

using namespace com::sun::star;

namespace
{
const sal_uInt32 SHA256_K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

inline sal_uInt32 lcl_Rotr( sal_uInt32 n, int b ) { return ( n >> b ) | ( n << ( 32 - b ) ); }

void lcl_Sha256Block( sal_uInt32* pState, const sal_uInt8* pBlock )
{
    sal_uInt32 w[64];
    for( int i = 0; i < 16; ++i )
        w[i] = ( sal_uInt32( pBlock[4*i] ) << 24 ) | ( sal_uInt32( pBlock[4*i+1] ) << 16 ) |
               ( sal_uInt32( pBlock[4*i+2] ) << 8 ) | sal_uInt32( pBlock[4*i+3] );
    for( int i = 16; i < 64; ++i )
    {
        const sal_uInt32 s0 = lcl_Rotr( w[i-15], 7 ) ^ lcl_Rotr( w[i-15], 18 ) ^ ( w[i-15] >> 3 );
        const sal_uInt32 s1 = lcl_Rotr( w[i-2], 17 ) ^ lcl_Rotr( w[i-2], 19 ) ^ ( w[i-2] >> 10 );
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    sal_uInt32 a = pState[0], b = pState[1], c = pState[2], d = pState[3];
    sal_uInt32 e = pState[4], f = pState[5], g = pState[6], h = pState[7];
    for( int i = 0; i < 64; ++i )
    {
        const sal_uInt32 t1 = h + ( lcl_Rotr( e, 6 ) ^ lcl_Rotr( e, 11 ) ^ lcl_Rotr( e, 25 ) ) +
                              ( ( e & f ) ^ ( ~e & g ) ) + SHA256_K[i] + w[i];
        const sal_uInt32 t2 = ( lcl_Rotr( a, 2 ) ^ lcl_Rotr( a, 13 ) ^ lcl_Rotr( a, 22 ) ) +
                              ( ( a & b ) ^ ( a & c ) ^ ( b & c ) );
        h = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
    }
    pState[0] += a; pState[1] += b; pState[2] += c; pState[3] += d;
    pState[4] += e; pState[5] += f; pState[6] += g; pState[7] += h;
}

// sal has no SHA-256; FIPS 180-4
void lcl_Sha256( const sal_uInt8* pData, sal_uInt32 nLen, sal_uInt8* pOut )
{
    sal_uInt32 aState[8] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                             0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
    sal_uInt32 nDone = 0;
    for( ; nLen - nDone >= 64; nDone += 64 )
        lcl_Sha256Block( aState, pData + nDone );

    sal_uInt8 aTail[128] = { 0 };
    const sal_uInt32 nRest = nLen - nDone;
    for( sal_uInt32 i = 0; i < nRest; ++i )
        aTail[i] = pData[nDone + i];
    aTail[nRest] = 0x80;
    const sal_uInt32 nTailLen = nRest < 56 ? 64 : 128;
    const sal_uInt64 nBits = sal_uInt64( nLen ) * 8;
    for( int i = 0; i < 8; ++i )
        aTail[nTailLen - 1 - i] = sal_uInt8( nBits >> ( 8 * i ) );
    for( sal_uInt32 i = 0; i < nTailLen; i += 64 )
        lcl_Sha256Block( aState, aTail + i );

    for( int i = 0; i < 8; ++i )
        for( int j = 0; j < 4; ++j )
            pOut[4*i+j] = sal_uInt8( aState[i] >> ( 24 - 8 * j ) );
}
}

void SvPasswordHelper::GetHashPassword(uno::Sequence<sal_Int8>& rPassHash, const sal_Char* pPass, sal_uInt32 nLen)
{
    rPassHash.realloc(RTL_DIGEST_LENGTH_SHA1);

	rtlDigestError aError = rtl_digest_SHA1 (pPass, nLen, reinterpret_cast<sal_uInt8*>(rPassHash.getArray()), rPassHash.getLength());
	if (aError != rtl_Digest_E_None)
	{
	    rPassHash.realloc(0);
	}
}

void SvPasswordHelper::GetHashPasswordLittleEndian(uno::Sequence<sal_Int8>& rPassHash, const String& sPass)
{
    xub_StrLen nSize(sPass.Len());
    sal_Char* pCharBuffer = new sal_Char[nSize * sizeof(sal_Unicode)];

    for (xub_StrLen i = 0; i < nSize; ++i)
    {
        sal_Unicode ch(sPass.GetChar(i));
        pCharBuffer[2 * i] = static_cast< sal_Char >(ch & 0xFF);
        pCharBuffer[2 * i + 1] = static_cast< sal_Char >(ch >> 8);
    }

    GetHashPassword(rPassHash, pCharBuffer, nSize * sizeof(sal_Unicode));

    delete[] pCharBuffer;
}

void SvPasswordHelper::GetHashPasswordBigEndian(uno::Sequence<sal_Int8>& rPassHash, const String& sPass)
{
    xub_StrLen nSize(sPass.Len());
    sal_Char* pCharBuffer = new sal_Char[nSize * sizeof(sal_Unicode)];

    for (xub_StrLen i = 0; i < nSize; ++i)
    {
        sal_Unicode ch(sPass.GetChar(i));
        pCharBuffer[2 * i] = static_cast< sal_Char >(ch >> 8);
        pCharBuffer[2 * i + 1] = static_cast< sal_Char >(ch & 0xFF);
    }

    GetHashPassword(rPassHash, pCharBuffer, nSize * sizeof(sal_Unicode));

    delete[] pCharBuffer;
}

void SvPasswordHelper::GetHashPasswordUTF8(uno::Sequence<sal_Int8>& rPassHash, const String& sPass)
{
    const ::rtl::OString aUtf8( ::rtl::OUStringToOString( ::rtl::OUString( sPass ), RTL_TEXTENCODING_UTF8 ) );
    GetHashPassword(rPassHash, aUtf8.getStr(), aUtf8.getLength());
}

void SvPasswordHelper::GetHashPassword(uno::Sequence<sal_Int8>& rPassHash, const String& sPass)
{
    GetHashPasswordLittleEndian(rPassHash, sPass);
}

void SvPasswordHelper::GetHashPasswordSHA256(uno::Sequence<sal_Int8>& rPassHash, const String& sPass)
{
    const ::rtl::OString aUtf8( ::rtl::OUStringToOString( ::rtl::OUString( sPass ), RTL_TEXTENCODING_UTF8 ) );
    rPassHash.realloc(32);
    lcl_Sha256( reinterpret_cast<const sal_uInt8*>(aUtf8.getStr()), aUtf8.getLength(),
                reinterpret_cast<sal_uInt8*>(rPassHash.getArray()) );
}

::rtl::OUString SvPasswordHelper::GetDigestAlgorithmIRI(const uno::Sequence<sal_Int8>& rPassHash)
{
    if (rPassHash.getLength() == 32)
        return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "http://www.w3.org/2000/09/xmldsig#sha256" ) );
    return ::rtl::OUString();
}

bool SvPasswordHelper::CompareHashPassword(const uno::Sequence<sal_Int8>& rOldPassHash, const String& sNewPass)
{
    uno::Sequence<sal_Int8> aNewPass;
    if (rOldPassHash.getLength() == 32)
    {
        GetHashPasswordSHA256(aNewPass, sNewPass);
        return aNewPass == rOldPassHash;
    }
    if (rOldPassHash.getLength() != RTL_DIGEST_LENGTH_SHA1)
        return false;

    GetHashPasswordLittleEndian(aNewPass, sNewPass);
    if (aNewPass == rOldPassHash)
        return true;
    GetHashPasswordBigEndian(aNewPass, sNewPass);
    if (aNewPass == rOldPassHash)
        return true;
    GetHashPasswordUTF8(aNewPass, sNewPass);
    return aNewPass == rOldPassHash;
}
