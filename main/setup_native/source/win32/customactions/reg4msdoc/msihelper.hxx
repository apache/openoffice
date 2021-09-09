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



#ifndef INCLUDED_MSIHELPER_HXX
#define INCLUDED_MSIHELPER_HXX

#ifdef _MSC_VER
#pragma warning(push, 1) /* disable warnings within system headers */
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msiquery.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <string>

/**
    Get the value of the named property

    @param handle
    [in] a valid msi handle.

    @param name
    [in] the name of the property.

    @param value
    [out] receives this value of the property.

    @returns
    <TRUE/>if the property was found.
*/
bool GetMsiProp(MSIHANDLE handle, LPCTSTR name, /*out*/std::wstring& value);

/**
    Set the value of a binary property which can only
    have the values "0" or "1" to "1".

    @param handle
    [in] a valid msi handle.

    @param name
    [in] the name of the property.
*/
void SetMsiProp(MSIHANDLE handle, LPCTSTR name);

/**
    Set the value of a binary property which can only
    have the values "0" or "1" to "0".

    @param handle
    [in] a valid msi handle.

    @param name
    [in] the name of the property.
*/
void UnsetMsiProp(MSIHANDLE handle, LPCTSTR name);

/**
    Returns whether a certain property is set meaning
    its value is "1". This method should be used for
    binary properties whose value can be "0" or "1".

    @returns
    <TRUE/>if the value of the specified property is
    "1" else if the property is not defined or its
    value is other than "1" <FALSE/> will be returned.
*/
bool IsSetMsiProp(MSIHANDLE handle, LPCTSTR name);

/**
    Returns whether a certain property is set meaning
    its value is not empty. This method should be used for
    properties, that can have different values.

    @returns
    <TRUE/>if the value of the specified property is
    not empty. If it is empty <FALSE/> will be returned.
*/
bool IsMsiPropNotEmpty(MSIHANDLE handle, LPCTSTR name);

/**
    Query if this is an installation for all user or not.

    @param handle
    [in] a valid msi handle.

    @returns
    <TRUE/>if this is an all user installation
*/
bool IsAllUserInstallation(MSIHANDLE handle);

/**
    Returns the destination folder of the office installation
    as system path. The returned path contains a final '\'.

    @param handle
    [in] a valid msi handle.

    @returns
    the destination path of the office installation finalized
    with a '\'.
*/
std::wstring GetOfficeInstallationPath(MSIHANDLE handle);

/**
    Returns the absolute path of the office executable that
    will be installed as system path.

    @param handle
    [in] a valid msi handle.

    @returns
    the absolute system path of the office executable (e.g.
    "C:\Program Files (x86)\OpenOffice 4\program\soffice.exe").
*/
std::wstring GetOfficeExecutablePath(MSIHANDLE handle);

/**
    Get the name of the office that will be installed

    @param handle
    [in] a valid msi handle.

    @returns
    the name of the office product that will be installed.
*/
std::wstring GetProductName(MSIHANDLE handle);

/**
    Determine if the specified module is installed locally.

    @param handle
    [in] a valid msi handle.

    @param name
    [in] the name of the module.

    @returns
    <TRUE/>if the specified module is installed locally.
*/
bool IsModuleInstalled(MSIHANDLE handle, LPCTSTR name);

/**
    Determine if the specified module is selected to be installed
    locally.

    @param handle
    [in] a valid msi handle.

    @param name
    [in] the name of the module.

    @returns
    <TRUE/>if the specified module is about to be installed locally.
*/
bool IsModuleSelectedForInstallation(MSIHANDLE handle, LPCTSTR name);

/**
    Determine if the specified module which is locally installed is
    selected for deinstallation.

    @param handle
    [in] a valid msi handle.

    @param name
    [in] the name of the module.

    @returns
    <TRUE/>if the specified module is about to be deinstalled.
*/
bool IsModuleSelectedForDeinstallation(MSIHANDLE handle, LPCTSTR name);

/**
    Determine whether this is a complete deinstallation or not.

    @param handle
    [in] a valid msi handle.

    @returns
    <TRUE/>if this is a complete deinstallation.
*/
bool IsCompleteDeinstallation(MSIHANDLE handle);

#endif
