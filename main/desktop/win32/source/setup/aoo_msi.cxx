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

#define  UNICODE    1
#define _UNICODE    1

#include "aoo_msi.hxx"

#include <strsafe.h>

/// Handle to MSI.DLL
static HMODULE hMsi = NULL;


TCHAR *getInstallerLocation( void )
{
    static const TCHAR sInstKey[] = TEXT( "Software\\Microsoft\\Windows\\CurrentVersion\\Installer" );
    static const TCHAR sInstLocValue[] = TEXT( "InstallerLocation" );

    HKEY    hInstKey = NULL;
    TCHAR  *sMsiFolder = new TCHAR[ MAX_PATH + 1 ];
    sMsiFolder[0] = '\0';

    // find registered location of Msi.dll
    if ( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, sInstKey, 0, KEY_READ, &hInstKey ) )
    {
	long    nRet = ERROR_SUCCESS;
	DWORD   dwMsiFolderSize = MAX_PATH + 1;
	DWORD   dwType = 0;

	if ( ERROR_MORE_DATA == ( nRet = RegQueryValueEx( hInstKey, sInstLocValue, NULL,
							  &dwType, (BYTE*)sMsiFolder, &dwMsiFolderSize ) ) )
        {
	    // try again with larger buffer
	    delete [] sMsiFolder;
	    sMsiFolder = new TCHAR[ dwMsiFolderSize ];
	    nRet = RegQueryValueEx( hInstKey, sInstLocValue, NULL, &dwType,
				    (BYTE*)sMsiFolder, &dwMsiFolderSize );
	}

	if ( ( ERROR_SUCCESS == nRet) &&
             ( dwType == REG_SZ ) && ( dwMsiFolderSize ) ) {
            // Make sure the string is null-terminated
            sMsiFolder[dwMsiFolderSize / sizeof ( TCHAR ) + 1] = 0;
        } else {
            sMsiFolder[0] = '\0';
        }
    }
    return sMsiFolder;
}

/** Load MSI.DLL and a symbol from it
 *
 * @return the requested symbol, or NULL in case of error.
 */
static FARPROC aoo_msi_load( LPCSTR lpProcName )
{
    static const TCHAR sMsiDll[] = TEXT( "\\msi.dll" );
    if ( !hMsi ) // use the default location
    {
	TCHAR *sInstallerLocation = getInstallerLocation();
        if ( sInstallerLocation[0] )
        {
            // load Msi.dll from registered location
            int nLength = lstrlen( sMsiDll ) + lstrlen( sInstallerLocation ) + 1; // use StringCchLength ?
            TCHAR *pMsiLocation = new TCHAR[ nLength ];

            if ( SUCCEEDED( StringCchCopy( pMsiLocation, nLength, sInstallerLocation ) ) &&
                 SUCCEEDED( StringCchCat( pMsiLocation, nLength, sMsiDll ) ) )
            {
                hMsi = LoadLibrary( pMsiLocation );
            }
            delete[] pMsiLocation;
        }
        delete[] sInstallerLocation;
    }
    if ( !hMsi )
    {
	return NULL;
    }
    return GetProcAddress( hMsi, lpProcName );
}


typedef HRESULT (CALLBACK* PFnDllGetVersion)( DLLVERSIONINFO *pdvi );
HRESULT aoo_MsiDllGetVersion( DLLVERSIONINFO *pdvi ) {
    static PFnDllGetVersion f = NULL;
    if ( f == NULL )
	f = (PFnDllGetVersion) aoo_msi_load( "DllGetVersion" );
    if ( f == NULL )
	return HRESULT_FROM_WIN32( GetLastError() );
    return f( pdvi );
}

typedef UINT (WINAPI* PFnMsiGetPatchInfo)( LPCTSTR  szPatch,
					   LPCTSTR  szAttribute,
					   LPTSTR   lpValueBuf,
					   LPDWORD pcchValueBuf );
UINT WINAPI aoo_MsiGetPatchInfo( LPCTSTR  szPatch,
				 LPCTSTR  szAttribute,
				 LPTSTR   lpValueBuf,
				 LPDWORD pcchValueBuf )
{
    static PFnMsiGetPatchInfo f = NULL;
    if ( f == NULL )
    {
#ifdef UNICODE
        f = (PFnMsiGetPatchInfo) aoo_msi_load( "MsiGetPatchInfoW" );
#else
        f = (PFnMsiGetPatchInfo) aoo_msi_load( "MsiGetPatchInfoA" );
#endif
    }
    if ( !f )
    {
        return ERROR_BAD_CONFIGURATION;
    }
    return f( szPatch, szAttribute, lpValueBuf, pcchValueBuf );
}

typedef UINT (WINAPI* PFnMsiGetSummaryInformation)( MSIHANDLE hDatabase,
                                                    LPCTSTR szDatabasePath,
                                                    UINT  uiUpdateCount,
                                                    MSIHANDLE *phSummaryInfo );
UINT WINAPI aoo_MsiGetSummaryInformation( MSIHANDLE hDatabase,
                                          LPCTSTR szDatabasePath,
                                          UINT  uiUpdateCount,
                                          MSIHANDLE *phSummaryInfo )
{
    static PFnMsiGetSummaryInformation f = NULL;
    if ( f == NULL )
    {
#ifdef UNICODE
        f = (PFnMsiGetSummaryInformation) aoo_msi_load( "MsiGetSummaryInformationW" );
#else
        f = (PFnMsiGetSummaryInformation) aoo_msi_load( "MsiGetSummaryInformationA" );
#endif
    }
    if ( f == NULL )
    {
        return ERROR_BAD_CONFIGURATION;
    }
    return f( hDatabase, szDatabasePath, uiUpdateCount, phSummaryInfo );
}

typedef INSTALLSTATE (WINAPI* PFnMsiQueryProductState)( LPCTSTR szProduct );
INSTALLSTATE WINAPI aoo_MsiQueryProductState( LPCTSTR szProduct )
{
    static PFnMsiQueryProductState f = NULL;
    if ( f == NULL )
    {
#ifdef UNICODE
        f = (PFnMsiQueryProductState) aoo_msi_load( "MsiQueryProductStateW" );
#else
        f = (PFnMsiQueryProductState) aoo_msi_load( "MsiQueryProductStateA" );
#endif
    }
    if ( f == NULL )
    {
        return INSTALLSTATE_INVALIDARG;
    }
    return f( szProduct );
}

typedef UINT (WINAPI* PFnMsiSummaryInfoGetProperty)( MSIHANDLE hSummaryInfo,
                                                     UINT      uiProperty,
                                                     PUINT     puiDataType,
                                                     LPINT     piValue,
                                                     FILETIME  *pftValue,
                                                     LPTSTR     szValueBuf,
                                                     LPDWORD   pcchValueBuf );
UINT WINAPI aoo_MsiSummaryInfoGetProperty( MSIHANDLE hSummaryInfo,
                                           UINT      uiProperty,
                                           PUINT     puiDataType,
                                           LPINT     piValue,
                                           FILETIME  *pftValue,
                                           LPTSTR     szValueBuf,
                                           LPDWORD   pcchValueBuf )
{
    static PFnMsiSummaryInfoGetProperty f = NULL;
    if ( f == NULL )
    {
#ifdef UNICODE
        f = (PFnMsiSummaryInfoGetProperty) aoo_msi_load( "MsSummaryInfoGetPropertyW" );
#else
        f = (PFnMsiSummaryInfoGetProperty) aoo_msi_load( "MsSummaryInfoGetPropertyA" );
#endif
    }
    if ( f == NULL )
    {
        return ERROR_INVALID_FUNCTION;
    }
    return f( hSummaryInfo, uiProperty, puiDataType, piValue, pftValue,
              szValueBuf, pcchValueBuf );
}


typedef UINT (WINAPI *PFnMsiCloseHandle)( MSIHANDLE hAny );
UINT WINAPI aoo_MsiCloseHandle( MSIHANDLE hAny )
{
    static PFnMsiCloseHandle f = NULL;
    if ( f == NULL )
    {
        f = (PFnMsiCloseHandle) aoo_msi_load( "MsiCloseHandle" );
    }
    if ( f == NULL )
    {
        return ERROR_INVALID_FUNCTION;
    }
    return f( hAny );
}

/*
 * MsiCloseHandle() must be defined because it is required by the
 * destructor of class PMSIHANDLE, defined in Msi.h.
 */
extern "C" {
    UINT WINAPI MsiCloseHandle( MSIHANDLE hAny )
    {
        return aoo_MsiCloseHandle( hAny );
    }
}
