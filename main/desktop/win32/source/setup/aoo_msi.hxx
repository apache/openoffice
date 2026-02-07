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

#ifndef AOO_MSI_H
#define AOO_MSI_H

#if defined _MSC_VER
#pragma warning(push, 1)
#endif
#include <windows.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif
#include <shlwapi.h>
#include <msi.h>

/** Load the "Installer location" from the Windows registry.
 *
 * @return an array allocated with new[] of at least MAX_PATH + 1
 * elements. The string length will be zero in case of error.
 */
TCHAR *getInstallerLocation( void );

HRESULT aoo_MsiDllGetVersion( DLLVERSIONINFO *pdvi );

UINT WINAPI aoo_MsiGetPatchInfo( LPCTSTR  szPatch,
				 LPCTSTR  szAttribute,
				 LPTSTR   lpValueBuf,
				 LPDWORD pcchValueBuf );

UINT WINAPI aoo_MsiGetSummaryInformation( MSIHANDLE hDatabase,
                                          LPCTSTR szDatabasePath,
                                          UINT  uiUpdateCount,
                                          MSIHANDLE *phSummaryInfo );

INSTALLSTATE WINAPI aoo_MsiQueryProductState( LPCTSTR szProduct );

UINT WINAPI aoo_MsiSummaryInfoGetProperty( MSIHANDLE hSummaryInfo,
                                           UINT      uiProperty,
                                           PUINT     puiDataType,
                                           LPINT     piValue,
                                           FILETIME  *pftValue,
                                           LPTSTR     szValueBuf,
                                           LPDWORD   pcchValueBuf );

#endif // defined AOO_MSI_H
