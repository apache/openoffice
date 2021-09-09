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



#ifndef _REGISTER_HXX_
#define _REGISTER_HXX_

#ifndef _CONSTANTS_HXX_
#include "constants.hxx"
#endif

#ifdef _MSC_VER
#pragma warning(push, 1) /* disable warnings within system headers */
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <msi.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <string>

const int MOD_WRITER  = 0x1;
const int MOD_CALC    = 0x2;
const int MOD_IMPRESS = 0x4;
const int MOD_ACTIVEX = 0x40;

/*  registers OpenOffice for MS document
    types and as default HTML editor if
    specified */
void Register4MsDoc(MSIHANDLE handle, int Register);

void Unregister4MsDoc(MSIHANDLE handle, int Unregister);

/* restores the entries for the selected
   registry entries */
void Unregister4MsDocAll(MSIHANDLE handle);

/* restores lost settings formerly made
   with Register4MsDoc */
void RepairRegister4MsDocSettings(MSIHANDLE handle);

/** Returns whether we are registered for */
bool IsRegisteredFor(MSIHANDLE handle, int State);

/** Returns whether we should preselect the
    registration checkbox for a certain
    application type or not */
bool query_preselect_registration_for_ms_application(
    MSIHANDLE handle, int Register);

// StarOffice 6.0 saved the registration
// state in HKEY_LOCAL_MACHINE\Software\
// Microsoft\Windows\CurrentVersion\Uninstall\
// StarOffice 6.0\Reg4MsdocState we move this
// value if available to the new registry
// location
int FixReturnRegistrationState(MSIHANDLE handle);

#endif
