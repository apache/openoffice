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



/*
    Windows shell extensions need to be approved in order to be used by the 
    Windows shell for clarification read the following section from the
    Microsoft Developers Network Library (MSDN) see
    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/shell/programmersguide/shell_int/shell_int_extending/extensionhandlers/shell_ext.asp
    
    
    <MSDN>
    Shell extension handlers run in the Shell process. Because it is a system process, 
    the administrator of a Windows NT system can limit Shell extension handlers to 
    those on an approved list by setting the EnforceShellExtensionSecurity value of the 
    HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Policies\Explorer key to 1 
    (one).
    To place a Shell extension handler on the approved list, create a REG_SZ value whose 
    name is the string form of the handler's GUID under 
    HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved. 
    
    The Shell does not use the value that is assigned to the GUID, but it should be set to 
    make inspecting the registry easier.

    Your setup application can add values to the Approved key only if the person installing 
    the application has sufficient privileges. If the attempt to add an extension handler 
    fails, you should inform the user that administrative privileges are required to fully 
    install the application. If the handler is essential to the application, you should fail 
    the setup and notify the user to contact an administrator. 

    While there is no need to add values to the Approved key on Windows 95 or Windows 98 
    systems, there is no harm in doing so. The system will simply ignore them. However, there 
    is no guarantee that the key will exist on these systems. Your setup program must be able 
    to handle this case.
    </MSDN>
    
    We add the following entries to the respective registry key
    "{C52AF81D-F7A0-4AAB-8E87-F80A60CCD396}"="OpenOffice Column Handler"
    "{087B3AE3-E237-4467-B8DB-5A38AB959AC9}"="OpenOffice Infotip Handler"
    "{63542C48-9552-494A-84F7-73AA6A7C99C1}"="OpenOffice Property Sheet Handler"
    "{3B092F0C-7696-40E3-A80F-68D74DA84210}"="OpenOffice Thumbnail Viewer"
    
    These shell extensions are implemented in the 'shell' project. We ignore registration 
    failures because of insufficient privileges. The reason is: On systems which restrict the
    use of shell extensions by applying the aforementioned policy probably only people with
    sufficient privileges are allowed to install applications anyway. On systems where the
    use of shell extensions is not restricted registration failures because of insufficient 
    privileges have no negative effect because the shell extensions will work anyhow. 
*/
 
#ifdef _MSC_VER
#pragma warning(push, 1) /* disable warnings within system headers */
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msiquery.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <malloc.h>

#ifdef UNICODE
#define _UNICODE
#endif
#include <tchar.h>

struct RegistryEntry
{
    TCHAR* Key;
    TCHAR* Value;
};

RegistryEntry ColumnHandler = { TEXT("{C52AF81D-F7A0-4AAB-8E87-F80A60CCD396}"), TEXT("OpenOffice Column Handler") };
RegistryEntry InfotipHandler = { TEXT("{087B3AE3-E237-4467-B8DB-5A38AB959AC9}"), TEXT("OpenOffice Infotip Handler") };
RegistryEntry PropHandler = { TEXT("{63542C48-9552-494A-84F7-73AA6A7C99C1}"), TEXT("OpenOffice Property Sheet Handler") };
RegistryEntry ThumbViewer = { TEXT("{3B092F0C-7696-40E3-A80F-68D74DA84210}"), TEXT("OpenOffice Thumbnail Viewer") };

BOOL GetMsiProp( MSIHANDLE hMSI, const char* pPropName, char** ppValue )
{
    DWORD sz = 0;
   	if ( MsiGetProperty( hMSI, pPropName, 0, &sz ) == ERROR_MORE_DATA )
   	{
       	sz++;
       	DWORD nbytes = sz * sizeof( char );
       	char* buff = reinterpret_cast<char*>( malloc( nbytes ) );
       	ZeroMemory( buff, nbytes );
       	MsiGetProperty( hMSI, pPropName, buff, &sz );
   		*ppValue = buff;

		return TRUE;
	}

	return FALSE;
} 

bool IsVersionNT64( MSIHANDLE hMSI )
{
    char* pVal = NULL;

	if ( GetMsiProp( hMSI, "VersionNT64", &pVal ) && pVal )
	{
		free( pVal );
		return true;
	}

	return false;
}




/*
    Called during installation when the module "Windows Explorer Extensions" is
    selected.
*/
extern "C" UINT __stdcall InstallExecSequenceEntry(MSIHANDLE hMSI)
{
    //MessageBox(NULL, TEXT("InstallExecSequenceEntry"), TEXT("Pythonmsi"), MB_OK | MB_ICONINFORMATION);
    HKEY hKey;


// 06.11.2009 tkr: to provide windows xp as build systems for mingw we need to define KEY_WOW64_64KEY
// in mingw 3.13 KEY_WOW64_64KEY isn't available < Win2003 systems.
// Also defined in setup_native\source\win32\customactions\reg64\reg64.cxx,source\win32\customactions\shellextensions\shellextensions.cxx and
// extensions\source\activex\main\so_activex.cpp
#ifndef KEY_WOW64_64KEY
	#define KEY_WOW64_64KEY	(0x0100)
#endif

	if (IsVersionNT64(hMSI))
	{
		// Open Windows 64 Bit Registry
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),0, KEY_WRITE | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS)
		{            
			RegSetValueEx(hKey, ColumnHandler.Key, 0, REG_SZ, reinterpret_cast<const BYTE*>(ColumnHandler.Value), _tcslen(ColumnHandler.Value) + 1);        
			RegSetValueEx(hKey, InfotipHandler.Key, 0, REG_SZ, reinterpret_cast<const BYTE*>(InfotipHandler.Value), _tcslen(InfotipHandler.Value) + 1);            
			RegSetValueEx(hKey, PropHandler.Key, 0, REG_SZ, reinterpret_cast<const BYTE*>(PropHandler.Value), _tcslen(PropHandler.Value) + 1);    
			RegSetValueEx(hKey, ThumbViewer.Key, 0, REG_SZ, reinterpret_cast<const BYTE*>(ThumbViewer.Value), _tcslen(ThumbViewer.Value) + 1);    
	    
			RegCloseKey(hKey);
		}         

		// Open Windows 32 Bit Registry on Win64 machine

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),0, KEY_WRITE, &hKey ) == ERROR_SUCCESS)
		{            
			RegSetValueEx(hKey, ColumnHandler.Key, 0, REG_SZ, reinterpret_cast<const BYTE*>(ColumnHandler.Value), _tcslen(ColumnHandler.Value) + 1);        
			RegSetValueEx(hKey, InfotipHandler.Key, 0, REG_SZ, reinterpret_cast<const BYTE*>(InfotipHandler.Value), _tcslen(InfotipHandler.Value) + 1);            
			RegSetValueEx(hKey, PropHandler.Key, 0, REG_SZ, reinterpret_cast<const BYTE*>(PropHandler.Value), _tcslen(PropHandler.Value) + 1);    
			RegSetValueEx(hKey, ThumbViewer.Key, 0, REG_SZ, reinterpret_cast<const BYTE*>(ThumbViewer.Value), _tcslen(ThumbViewer.Value) + 1);    
	    
			RegCloseKey(hKey);
		}         


	} else
	{
		if (RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"), &hKey) == ERROR_SUCCESS)
		{            
			RegSetValueEx(hKey, ColumnHandler.Key, 0, REG_SZ, reinterpret_cast<const BYTE*>(ColumnHandler.Value), _tcslen(ColumnHandler.Value) + 1);        
			RegSetValueEx(hKey, InfotipHandler.Key, 0, REG_SZ, reinterpret_cast<const BYTE*>(InfotipHandler.Value), _tcslen(InfotipHandler.Value) + 1);            
			RegSetValueEx(hKey, PropHandler.Key, 0, REG_SZ, reinterpret_cast<const BYTE*>(PropHandler.Value), _tcslen(PropHandler.Value) + 1);    
			RegSetValueEx(hKey, ThumbViewer.Key, 0, REG_SZ, reinterpret_cast<const BYTE*>(ThumbViewer.Value), _tcslen(ThumbViewer.Value) + 1);    
	    
			RegCloseKey(hKey);
		}         
	}
    return ERROR_SUCCESS;    
}

/*
    Called during deinstallation when the module "Windows Explorer Extensions" has
    been installed.
*/
extern "C" UINT __stdcall DeinstallExecSequenceEntry(MSIHANDLE)
{
    //MessageBox(NULL, TEXT("DeinstallExecSequenceEntry"), TEXT("Pythonmsi"), MB_OK | MB_ICONINFORMATION);
    HKEY hKey;
    if (RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"), &hKey) == ERROR_SUCCESS)
    {
        RegDeleteValue(hKey, ColumnHandler.Key);
        RegDeleteValue(hKey, InfotipHandler.Key);
        RegDeleteValue(hKey, PropHandler.Key);
        RegDeleteValue(hKey, ThumbViewer.Key);
        
        RegCloseKey(hKey);
    }
    return ERROR_SUCCESS;
}
