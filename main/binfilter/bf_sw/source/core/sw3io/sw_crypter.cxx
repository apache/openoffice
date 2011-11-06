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


#ifdef _MSC_VER
#pragma hdrstop
#endif

#include <string.h>
#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif

#ifndef _CRYPTER_HXX
#include <crypter.hxx>
#endif

namespace binfilter {




Crypter::Crypter( const ByteString& r )
{
    // Dies sind Randomwerte, die konstant zur Verschluesselung
    // des Passworts verwendet werden. Durch die Verwendung eines
    // verschluesselten Passworts wird vermieden, dass das Passwort
    // im RAM gehalten wird.
    static const BYTE cEncode[] =
    { 0xAB, 0x9E, 0x43, 0x05, 0x38, 0x12, 0x4d, 0x44,
      0xD5, 0x7e, 0xe3, 0x84, 0x98, 0x23, 0x3f, 0xba };

    xub_StrLen nLen = r.Len();
    if( nLen > PASSWDLEN ) nLen = PASSWDLEN;
    ByteString aPasswd( r );
    if( nLen > PASSWDLEN )
        aPasswd.Erase( nLen );
    else
        aPasswd.Expand( PASSWDLEN, ' ' );
    memcpy( cPasswd, cEncode, PASSWDLEN );
    Encrypt( aPasswd );
    memcpy( cPasswd, aPasswd.GetBuffer(), PASSWDLEN );
}



void Crypter::Encrypt( ByteString& r ) const
{
    xub_StrLen nLen = r.Len();
    if( !nLen )
        return ;

    xub_StrLen nCryptPtr = 0;
    BYTE cBuf[ PASSWDLEN ];
    memcpy( cBuf, cPasswd, PASSWDLEN );
    BYTE* pSrc = (BYTE*)r.GetBufferAccess();
    BYTE* p = cBuf;

    while( nLen-- )
    {
        *pSrc = *pSrc ^ ( *p ^ (BYTE) ( cBuf[ 0 ] * nCryptPtr ) );
        *p += ( nCryptPtr < (PASSWDLEN-1) ) ? *(p+1) : cBuf[ 0 ];
        if( !*p ) *p += 1;
        p++;
        if( ++nCryptPtr >= PASSWDLEN ) nCryptPtr = 0, p = cBuf;
        pSrc++;
    }
}



void Crypter::Decrypt( ByteString& r ) const
{
    Encrypt( r );
}



}
