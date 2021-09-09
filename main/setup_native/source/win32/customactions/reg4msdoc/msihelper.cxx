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



#include "msihelper.hxx"

#include <malloc.h>
#include <assert.h>

bool GetMsiProp(MSIHANDLE handle, LPCTSTR name, /*out*/std::wstring& value)
{
    DWORD sz = 0;
    LPTSTR dummy = TEXT("");
    if (MsiGetProperty(handle, name, dummy, &sz) == ERROR_MORE_DATA)
    {
        sz++;
        DWORD nbytes = sz * sizeof(TCHAR);
        LPTSTR buff = reinterpret_cast<LPTSTR>(_alloca(nbytes));
        ZeroMemory(buff, nbytes);
        MsiGetProperty(handle, name, buff, &sz);
        value = buff;
        return true;
    }
    return false;
}

void SetMsiProp(MSIHANDLE handle, LPCTSTR name)
{
    MsiSetProperty(handle, name, TEXT("1"));
}

void UnsetMsiProp(MSIHANDLE handle, LPCTSTR name)
{
    MsiSetProperty(handle, name, TEXT(""));
}

bool IsSetMsiProp(MSIHANDLE handle, LPCTSTR name)
{
    std::wstring val;
    GetMsiProp(handle, name, val);
    return (val == TEXT("1"));
}

bool IsMsiPropNotEmpty(MSIHANDLE handle, LPCTSTR name)
{
    std::wstring val;
    GetMsiProp(handle, name, val);
    return (val != TEXT(""));
}

bool IsAllUserInstallation(MSIHANDLE handle)
{
    return IsSetMsiProp(handle, TEXT("ALLUSERS"));
}

std::wstring GetOfficeInstallationPath(MSIHANDLE handle)
{
    std::wstring progpath;
    GetMsiProp(handle, TEXT("INSTALLLOCATION"), progpath);
    return progpath;
}

std::wstring GetOfficeExecutablePath(MSIHANDLE handle)
{
    std::wstring exepath = GetOfficeInstallationPath(handle);
    exepath += TEXT("program\\soffice.exe");
    return exepath;
}

std::wstring GetProductName(MSIHANDLE handle)
{
    std::wstring prodname;
    GetMsiProp(handle, TEXT("ProductName"), prodname);
    return prodname;
}

bool IsModuleInstalled(MSIHANDLE handle, LPCTSTR name)
{
    INSTALLSTATE current_state;
    INSTALLSTATE future_state;
    MsiGetFeatureState(handle, name, &current_state, &future_state);
    return (current_state == INSTALLSTATE_LOCAL);
}

bool IsModuleSelectedForInstallation(MSIHANDLE handle, LPCTSTR name)
{
    INSTALLSTATE current_state;
    INSTALLSTATE future_state;
    MsiGetFeatureState(handle, name, &current_state, &future_state);
    return (future_state == INSTALLSTATE_LOCAL);
}

bool IsModuleSelectedForDeinstallation(MSIHANDLE handle, LPCTSTR name)
{
    INSTALLSTATE current_state;
    INSTALLSTATE future_state;
    MsiGetFeatureState(handle, name, &current_state, &future_state);
    return ((current_state == INSTALLSTATE_LOCAL) && (future_state == INSTALLSTATE_ABSENT));
}

bool IsCompleteDeinstallation(MSIHANDLE handle)
{
    std::wstring rm;
    GetMsiProp(handle, TEXT("REMOVE"), rm);
    return (rm == TEXT("ALL"));
}
