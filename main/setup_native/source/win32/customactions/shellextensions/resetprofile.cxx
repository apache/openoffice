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

// ResetUserProfile: move the installing user's profile aside, so that the
// next start of the office creates a fresh one.  The old profile is renamed,
// never deleted, so the user can get documents, macros or settings back.
//
// The profile location is not hard-coded: it is read from the
// UserInstallation entry of the bootstrap.ini that was just installed,
// e.g. "$SYSUSERCONFIG/OpenOffice/5", and $SYSUSERCONFIG is the user's
// application data folder (AppDataFolder in Windows Installer terms).

#ifdef _MSC_VER
#pragma warning(push, 1) /* disable warnings within system headers */
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <msiquery.h>
#include <strsafe.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <malloc.h>
#include <string>

namespace {

std::wstring GetMsiPropertyW( MSIHANDLE handle, const wchar_t* pProperty )
{
    std::wstring result;
    wchar_t szDummy[1] = L"";
    DWORD nChars = 0;

    if ( MsiGetPropertyW( handle, pProperty, szDummy, &nChars ) == ERROR_MORE_DATA )
    {
        DWORD nBytes = ++nChars * sizeof(wchar_t);
        wchar_t* buffer = static_cast<wchar_t*>( _alloca( nBytes ) );
        ZeroMemory( buffer, nBytes );
        MsiGetPropertyW( handle, pProperty, buffer, &nChars );
        result = buffer;
    }

    return result;
}

// Write one line to the Windows Installer log (msiexec /l*v).
void Log( MSIHANDLE handle, const std::wstring& rText )
{
    PMSIHANDLE hRecord = MsiCreateRecord( 0 );
    if ( hRecord )
    {
        std::wstring aText( L"ResetUserProfile: " );
        aText += rText;
        MsiRecordSetStringW( hRecord, 0, aText.c_str() );
        MsiProcessMessage( handle, INSTALLMESSAGE_INFO, hRecord );
    }
}

void StripTrailingBackslash( std::wstring& rPath )
{
    while ( !rPath.empty() && rPath[ rPath.size() - 1 ] == L'\\' )
        rPath.erase( rPath.size() - 1 );
}

} // namespace

extern "C" UINT __stdcall ResetUserProfile( MSIHANDLE handle )
{
    std::wstring aInstallLocation = GetMsiPropertyW( handle, L"INSTALLLOCATION" );
    std::wstring aAppData = GetMsiPropertyW( handle, L"AppDataFolder" );
    if ( aInstallLocation.empty() || aAppData.empty() )
    {
        Log( handle, L"INSTALLLOCATION or AppDataFolder not set, nothing done." );
        return ERROR_SUCCESS;
    }
    StripTrailingBackslash( aInstallLocation );
    StripTrailingBackslash( aAppData );

    const std::wstring aIni = aInstallLocation + L"\\program\\bootstrap.ini";
    wchar_t aValue[ MAX_PATH ] = L"";
    GetPrivateProfileStringW( L"Bootstrap", L"UserInstallation", L"",
                              aValue, MAX_PATH, aIni.c_str() );

    // Only the shipped form is understood; anything else was edited by an
    // administrator, and a guessed path could move the wrong directory.
    const std::wstring aPrefix( L"$SYSUSERCONFIG" );
    std::wstring aUserInstallation( aValue );
    if ( aUserInstallation.compare( 0, aPrefix.size(), aPrefix ) != 0 )
    {
        Log( handle, L"UserInstallation in " + aIni + L" is \"" + aUserInstallation
                     + L"\", not $SYSUSERCONFIG/...; nothing done." );
        return ERROR_SUCCESS;
    }

    std::wstring aProfile = aAppData + aUserInstallation.substr( aPrefix.size() );
    for ( std::wstring::size_type i = 0; i < aProfile.size(); ++i )
        if ( aProfile[ i ] == L'/' )
            aProfile[ i ] = L'\\';
    StripTrailingBackslash( aProfile );

    DWORD nAttributes = GetFileAttributesW( aProfile.c_str() );
    if ( nAttributes == INVALID_FILE_ATTRIBUTES || !( nAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
    {
        Log( handle, L"no profile at " + aProfile + L", nothing to reset." );
        return ERROR_SUCCESS;
    }

    SYSTEMTIME aNow;
    GetLocalTime( &aNow );
    wchar_t aStamp[ 32 ];
    StringCchPrintfW( aStamp, sizeof( aStamp ) / sizeof( aStamp[ 0 ] ),
                      L".backup-%04u%02u%02u-%02u%02u%02u",
                      aNow.wYear, aNow.wMonth, aNow.wDay,
                      aNow.wHour, aNow.wMinute, aNow.wSecond );
    const std::wstring aBackup = aProfile + aStamp;

    if ( MoveFileW( aProfile.c_str(), aBackup.c_str() ) )
    {
        Log( handle, L"moved " + aProfile + L" to " + aBackup );
    }
    else
    {
        const DWORD nError = GetLastError();
        wchar_t aError[ 16 ];
        StringCchPrintfW( aError, sizeof( aError ) / sizeof( aError[ 0 ] ), L"%lu", nError );
        Log( handle, L"could not move " + aProfile + L" (error " + aError
                     + L"); is the office still running?" );
    }

    return ERROR_SUCCESS;
}
