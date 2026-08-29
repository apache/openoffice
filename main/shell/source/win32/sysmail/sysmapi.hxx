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

#ifndef INCLUDED_SIMPLEMAPI_HXX
#define INCLUDED_SIMPLEMAPI_HXX

#define WIN32_LEAN_AND_MEAN
#if defined _MSC_VER
#pragma warning(push, 1)
#endif
#include <windows.h>
#include <mapi.h>
// Extended MAPI left the Windows SDK after Platform SDK v7.0 -- Microsoft moved
// it to Outlook -- so a modern SDK has no mapix.h, and none of its four
// companions either.  Nothing here needs them: this module uses only simple
// MAPI (MAPILogon, MAPISendMail, MAPILogoff, and the Mapi* structures and
// MAPI_* flags), all of which mapi.h still declares.
//
// Dropped rather than sourced from the old SDK on purpose.  Putting v7.0's
// include directory on the path pulls in its wtypes.h behind mapix.h, and that
// then collides with the modern SDK's -- C2011 on every COM type in it.  The
// two header trees do not mix inside one translation unit.
#if !defined __MINGW32__ && !(defined _MSC_VER && _MSC_VER >= 1900)
#include <mapix.h>
#endif
#if defined _MSC_VER
#pragma warning(pop)
#endif

namespace shell
{
    class WinSysMapi
    {
    public:
        /**
            @throws std::runtime_error
            if either the mapi32.dll could not be loaded at all
            or necessary function exports are missing
        */
        WinSysMapi(); // throws std::runtime_error;

        ~WinSysMapi();

        ULONG MAPILogon(
            ULONG ulUIParam,
            LPTSTR lpszProfileName,
            LPTSTR lpszPassword,
            FLAGS flFlags,
            ULONG ulReserved,
            LPLHANDLE lplhSession );

        ULONG MAPILogoff(
            LHANDLE lhSession,
            ULONG ulUIParam,
            FLAGS flFlags,
            ULONG ulReserved );

        ULONG MAPISendMail(
            LHANDLE lhSession,
            ULONG ulUIParam,
            lpMapiMessage lpMessage,
            FLAGS flFlags,
            ULONG ulReserved );

    private:
        HMODULE         m_hMapiDll;
        LPMAPILOGON     m_lpfnMapiLogon;
        LPMAPILOGOFF    m_lpfnMapiLogoff;
        LPMAPISENDMAIL  m_lpfnMapiSendMail;
    };
}
#endif
