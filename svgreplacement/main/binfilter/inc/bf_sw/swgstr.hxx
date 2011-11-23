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



#ifndef _SWGSTR_HXX
#define _SWGSTR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif
namespace binfilter {

typedef long long3;						// Zur Dokumentation: 3-byte-Longs

#define MAX_BEGIN 64					// Maximale Blockschachtelung
#define PASSWDLEN 16					// Maximale Passwortlaenge

// Neue Version mit SvStreams

// Passwort- und Codierungs-Funktionalitaet

class swcrypter {
protected:
    sal_Char   cPasswd[ PASSWDLEN ];    // Passwort-Puffer
    BOOL   bPasswd;                     // TRUE wenn mit Passwort
    void   encode( sal_Char*, USHORT ); // Puffer codieren/decodieren
public:
    swcrypter();
    BOOL setpasswd( const String& );    // Passwort setzen
    void copypasswd( const sal_Char* ); // Passwort direkt setzen
    const sal_Char* getpasswd() { return cPasswd; }
};

// Reader/Writer-Stream-Basisklasse mit Pufferverwaltung fuer Texte
// und Spezial-I/O fuer 3-Byte-Longs

class swstreambase : public swcrypter {
protected:
    SvStream* pStrm;                    // eigentlicher Stream
    sal_Char*  pBuf;                        // Zwischenpuffer
    USHORT nBuflen;                     // Laenge des Zwischenpuffers
    short  nLong;                       // Long-Laenge (3 oder 4)
    BOOL   bTempStrm;                   // TRUE: Stream loeschen
    void   checkbuf( USHORT );          // Testen der Pufferlaenge

    swstreambase( SvStream& );

    swstreambase( const swstreambase& );
    int operator=( const swstreambase& );
public:
    ~swstreambase();
    SvStream& Strm()                    { return *pStrm; }
    sal_Char* GetBuf()                  { return pBuf; }

    // Zusatzfunktionen zur I/O von LONGs als 3-Byte-Zahlen

    void long3()                        { nLong = 3; }
    void long4()                        { nLong = 4; }

    void sync( swstreambase& r )        { pStrm->Seek( r.tell() ); }

    // Alias- und Hilfsfunktionen

    void seek( long nPos )              { pStrm->Seek( nPos );  }
    long tell()                         { return pStrm->Tell(); }
    long filesize();                    // Dateigroesse
    void flush()                        { pStrm->Flush(); }

    void setbad();
    int good()                          { return ( pStrm->GetError() == SVSTREAM_OK ); }
    int bad()                           { return ( pStrm->GetError() != SVSTREAM_OK ); }
    int operator!()                     { return ( pStrm->GetError() != SVSTREAM_OK ); }
    int eof()                           { return pStrm->IsEof(); }

    BYTE get();
    void get( void* p, USHORT n )       { pStrm->Read( (sal_Char*) p, n ); }

    inline swstreambase& operator>>( sal_Char& );
    inline swstreambase& operator>>( BYTE& );
    inline swstreambase& operator>>( short& );
    inline swstreambase& operator>>( USHORT& );
           swstreambase& operator>>( long& );
    inline swstreambase& operator>>( ULONG& );
};

inline swstreambase& swstreambase::operator>>( sal_Char& c )
{
    *pStrm >> c; return *this;
}

inline swstreambase& swstreambase::operator>>( BYTE& c )
{
    *pStrm >> c; return *this;
}

inline swstreambase& swstreambase::operator>>( short& c )
{
    *pStrm >> c; return *this;
}

inline swstreambase& swstreambase::operator>>( USHORT& c )
{
    *pStrm >> c; return *this;
}

inline swstreambase& swstreambase::operator>>( ULONG& c )
{
    return *this >> (long&) c;
}

class swistream : public swstreambase {
    BYTE   cType;                       // Record-Typ
    ULONG  nOffset;                     // Record-Offset-Portion
public:
    swistream( SvStream& );

    BYTE peek();                        // 1 Byte testen
    BYTE next();                        // Blockstart
    BYTE cur() { return cType; }        // aktueller Block
    BYTE skipnext();                    // Record ueberspringen
    void undonext();                    // next() rueckgaengig machen
    long getskip()                      { return nOffset; }
    void skip( long = -1L );            // Block ueberspringen
    sal_Char* text();                   // Textstring lesen (nach BEGIN)
    long size();                        // aktuelle Record-Laenge

private:
    swistream( const swistream& );
    int operator=( const swistream& );
};


} //namespace binfilter
#endif
