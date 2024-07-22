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
#include "precompiled_connectivity.hxx"
#include "file/quotedstring.hxx"
#include <rtl/logfile.hxx>
#include <rtl/ustrbuf.hxx>

namespace connectivity
{
    //==================================================================
    //= QuotedTokenizedString
    //==================================================================
    //------------------------------------------------------------------
    sal_Int32 QuotedTokenizedString::GetTokenCount( sal_Unicode cTok, sal_Unicode cStrDel ) const
    {
        RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "file", "Ocke.Janssen@sun.com", "QuotedTokenizedString::GetTokenCount" );
        const sal_Int32 nLen = m_sString.getLength();
	    if ( !nLen )
		    return 0;

	    sal_Int32 nTokCount = 1;
	    sal_Bool bStart = sal_True;		// Stehen wir auf dem ersten Zeichen im Token?
	    sal_Bool bInString = sal_False;	// Befinden wir uns INNERHALB eines (cStrDel delimited) String?

	    // Suche bis Stringende nach dem ersten nicht uebereinstimmenden Zeichen
	    for( sal_Int32 i = 0; i < nLen; ++i )
	    {
            const sal_Unicode cChar = m_sString[ i ];
		    if (bStart)
		    {
			    bStart = sal_False;
			    // Erstes Zeichen ein String-Delimiter?
			    if ( cChar == cStrDel )
			    {
				    bInString = sal_True;	// dann sind wir jetzt INNERHALB des Strings!
				    continue;			// dieses Zeichen ueberlesen!
			    }
		    }

		    if (bInString) 
            {
			    // Wenn jetzt das String-Delimiter-Zeichen auftritt ...
			    if ( cChar == cStrDel )
			    {
				    if ((i+1 < nLen) && (m_sString[ i+1 ] == cStrDel))
				    {
					    // Verdoppeltes String-Delimiter-Zeichen:
					    ++i;	// kein String-Ende, naechstes Zeichen ueberlesen.
				    }
				    else
				    {
					    // String-Ende
					    bInString = sal_False;
				    }
			    }
		    } // if (bInString) 
            else 
            {
			    // Stimmt das Tokenzeichen ueberein, dann erhoehe TokCount
			    if ( cChar == cTok )
			    {
				    ++nTokCount;
				    bStart = sal_True;
			    }
		    }
	    }
	    //OSL_TRACE("QuotedTokenizedString::nTokCount = %d\n", ((OUtoCStr(::rtl::OUString(nTokCount))) ? (OUtoCStr(::rtl::OUString(nTokCount))):("NULL")) );

	    return nTokCount;
    }

    //------------------------------------------------------------------
    void QuotedTokenizedString::GetTokenSpecial( ::rtl::OUString* _rStr,sal_Int32& nStartPos, sal_Unicode cTok, sal_Unicode cStrDel ) const
    {
        RTL_LOGFILE_CONTEXT_AUTHOR( aLogger, "file", "Ocke.Janssen@sun.com", "QuotedTokenizedString::GetTokenCount" );
	    *_rStr = ::rtl::OUString();
	    const sal_Int32 nLen = m_sString.getLength();
	    if ( nLen )
	    {
		    sal_Bool bInString = (nStartPos < nLen) && (m_sString[ nStartPos ] == cStrDel);	// Are we inside a (cStrDel delimited) String?

		    // Is the first character a string delimiter?
		    if (bInString )
			    ++nStartPos;			// ignore this character!
            if ( nStartPos >= nLen )
                return;

            ::rtl::OUStringBuffer buffer( nLen - nStartPos);
		    // Search until the end of string for the first non-matching character
		    for( sal_Int32 i = nStartPos; i < nLen; ++i )
		    {
                const sal_Unicode cChar = m_sString[ i ];
			    if (bInString)
			    {
				    // Wenn jetzt das String-Delimiter-Zeichen auftritt ...
				    if ( cChar == cStrDel )
				    {
					    if ((i+1 < nLen) && (m_sString[ i+1 ] == cStrDel))
					    {
						    // Verdoppeltes String-Delimiter-Zeichen:
						    // kein String-Ende, naechstes Zeichen ueberlesen.
                            ++i;
						    buffer.append( m_sString[ i ] );	// Zeichen gehoert zum Resultat-String
					    }
					    else
					    {
						    // String-Ende
						    bInString = sal_False;
					    }
				    }
				    else
				    {
					    buffer.append( cChar );	// Zeichen gehoert zum Resultat-String
				    }

			    }
			    else
			    {
				    // Stimmt das Tokenzeichen ueberein, dann erhoehe nTok
				    if ( cChar == cTok )
				    {
					    // Vorzeitiger Abbruch der Schleife moeglich, denn
					    // wir haben, was wir wollten.
					    nStartPos = i+1;
					    break;
				    }
				    else
				    {
					    buffer.append( cChar );	// Zeichen gehoert zum Resultat-String
				    }
			    }
		    } // for( sal_Int32 i = nStartPos; i < nLen; ++i )
            *_rStr = buffer.makeStringAndClear();
	    }
    }
}
