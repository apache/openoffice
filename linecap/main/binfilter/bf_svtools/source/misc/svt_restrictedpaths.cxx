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



#include <tools/urlobj.hxx>
#include <algorithm>
#include <vector>
#include <osl/process.h>
#include <tools/urlobj.hxx>
#include <unotools/localfilehelper.hxx>
#include <bf_svtools/syslocale.hxx>

namespace binfilter
{
    //=====================================================================
    //= CheckURLAllowed
    //=====================================================================
    struct CheckURLAllowed
    {
    protected:
    #ifdef WNT
        SvtSysLocale    m_aSysLocale;
    #endif
        String          m_sCheckURL;    // the URL to check
        bool            m_bAllowParent;
    public:
        inline CheckURLAllowed( const String& _rCheckURL, bool bAllowParent = true )
            :m_sCheckURL( _rCheckURL ), m_bAllowParent( bAllowParent )
        {
    #ifdef WNT
            // on windows, assume that the relevant file systems are case insensitive,
            // thus normalize the URL
            m_sCheckURL = m_aSysLocale.GetCharClass().toLower( m_sCheckURL, 0, m_sCheckURL.Len() );
    #endif
        }

        bool operator()( const String& _rApprovedURL )
        {
    #ifdef WNT
            // on windows, assume that the relevant file systems are case insensitive,
            // thus normalize the URL
            String sApprovedURL( m_aSysLocale.GetCharClass().toLower( _rApprovedURL, 0, _rApprovedURL.Len() ) );
    #else
            String sApprovedURL( _rApprovedURL );
    #endif

            xub_StrLen nLenApproved = sApprovedURL.Len();
            xub_StrLen nLenChecked  = m_sCheckURL.Len();
            
            if ( nLenApproved > nLenChecked )
            {
                if ( m_bAllowParent )
                {
                    if ( sApprovedURL.Search( m_sCheckURL ) == 0 )
                    {
                        if ( ( m_sCheckURL.GetChar( nLenChecked - 1 ) == '/' )
                            || ( sApprovedURL.GetChar( nLenChecked ) == '/' ) )
                            return true;
                    }
                }
                else
                {
                    // just a difference in final slash?
                    if ( ( nLenApproved == ( nLenChecked + 1 ) ) &&
                        ( sApprovedURL.GetChar( nLenApproved - 1 ) == '/' ) )
                        return true;
                }
                return false;
            }
            else if ( nLenApproved < nLenChecked )
            {
                if ( m_sCheckURL.Search( sApprovedURL ) == 0 )
                {
                    if ( ( sApprovedURL.GetChar( nLenApproved - 1 ) == '/' )
                        || ( m_sCheckURL.GetChar( nLenApproved ) == '/' ) )
                        return true;
                }
                return false;
            }
            else
            {
                // strings have equal length
                return ( sApprovedURL == m_sCheckURL );
            }
        }
    };
}
