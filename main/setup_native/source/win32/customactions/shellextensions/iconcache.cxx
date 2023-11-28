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
#pragma warning(push, 1) /* disable warnings within system headers */
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msiquery.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <stdlib.h>

extern "C" UINT __stdcall RebuildShellIconCache(MSIHANDLE)
{
	// Rebuild icon cache on windows OS prior XP

	OSVERSIONINFO	osverinfo;

	osverinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if ( 
		GetVersionEx( &osverinfo ) && 
		VER_PLATFORM_WIN32_NT == osverinfo.dwPlatformId &&
		(
		5 < osverinfo.dwMajorVersion ||
		5 == osverinfo.dwMajorVersion && 0 < osverinfo.dwMinorVersion
		)
		)
	{
		return ERROR_SUCCESS;
	}

	HKEY	hKey;
	DWORD	dwDisposition;
	LONG	lError = RegCreateKeyEx( HKEY_CURRENT_USER, TEXT("Control Panel\\Desktop\\WindowMetrics"), 0, NULL, REG_OPTION_VOLATILE, KEY_SET_VALUE | KEY_QUERY_VALUE, NULL, &hKey, &dwDisposition );

	if ( ERROR_SUCCESS == lError )
	{
		TCHAR	szValue[256];
		TCHAR	szTempValue[256];
		DWORD	cbValue = sizeof(szValue);
		DWORD	dwType;
		int		iSize = 0;

		lError = RegQueryValueEx( hKey, TEXT("Shell Icon Size"), 0, &dwType, (LPBYTE)szValue, &cbValue );

		if ( ERROR_SUCCESS == lError )
			iSize = atoi( szValue );

		if ( !iSize )
		{
			iSize = GetSystemMetrics( SM_CXICON );
			itoa( iSize, szValue, 10 );
			cbValue = strlen( szValue ) + 1;
			dwType = REG_SZ;
		}

		itoa( iSize + 1, szTempValue, 10 );
		lError = RegSetValueEx( hKey, TEXT("Shell Icon Size"), 0, dwType, (LPBYTE)szTempValue, strlen( szTempValue ) + 1 );

		LRESULT	lResult = SendMessageTimeout(
			 HWND_BROADCAST,
			 WM_SETTINGCHANGE,
			 SPI_SETNONCLIENTMETRICS,
			 (LPARAM)TEXT("WindowMetrics"),
			 SMTO_NORMAL|SMTO_ABORTIFHUNG,
			 0, NULL);

		lError = RegSetValueEx( hKey, TEXT("Shell Icon Size"), 0, dwType, (LPBYTE)szValue, cbValue );

		lResult = SendMessageTimeout(
			 HWND_BROADCAST,
			 WM_SETTINGCHANGE,
			 SPI_SETNONCLIENTMETRICS,
			 (LPARAM)TEXT("WindowMetrics"),
			 SMTO_NORMAL|SMTO_ABORTIFHUNG,
			 0, NULL);

		lError = RegCloseKey( hKey );
	}

    return ERROR_SUCCESS;
}
