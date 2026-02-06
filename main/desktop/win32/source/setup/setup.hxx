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

#include <stdio.h>

//--------------------------------------------------------------------------

struct LanguageData
{
    long    m_nLanguageID;
    LPTSTR  m_pTransform;

     LanguageData( LPTSTR pData );
    ~LanguageData();
};

//--------------------------------------------------------------------------

class SetupApp
{
    DWORD           m_nOSVersion;
    DWORD           m_nMinorVersion;
    boolean         m_bIsWin9x      : 1;
    boolean         m_bNeedReboot   : 1;
    boolean         m_bAdministrative : 1;

    HINSTANCE   m_hInst;
    HANDLE      m_hMapFile;
    LPTSTR      m_pAppTitle;
    LPTSTR      m_pCmdLine;
    LPTSTR      m_pDatabase;
    LPTSTR      m_pReqVersion;
    LPTSTR      m_pProductName;
    LPTSTR      m_pAdvertise;
    LPTSTR      m_pTmpName;
    LPTSTR      m_pErrorText;
    LPTSTR      m_pModuleFile;
    LPTSTR      m_pPatchFiles;
    LPCTSTR     m_pUpgradeKey;
    LPCTSTR     m_pProductVersion;
    int        *m_pMSIErrorCode;

    boolean     m_bQuiet            : 1;
    boolean     m_bIgnoreAlreadyRunning : 1;
    boolean     m_bRegNoMsoTypes :1;
    boolean     m_bRegAllMsoTypes :1;
    boolean     m_bIsMinorUpgrade :1;
    boolean     m_bSupportsPatch :1;

    FILE       *m_pLogFile;

    long            m_nLanguageID;
    long            m_nLanguageCount;
    LanguageData** m_ppLanguageList;

private:

    boolean     GetPathToFile( TCHAR* pFileName, TCHAR **pPath );
    LPCTSTR     GetPathToMSI();

    int         GetNameValue( TCHAR* pLine, TCHAR **pName, TCHAR **pValue );
    boolean     GetProfileSection( LPCTSTR pFileName, LPCTSTR pSection,
                                   DWORD& rSize, LPTSTR *pRetBuf );
    LPTSTR      CopyIniFile( LPCTSTR pIniFile );
    void        ConvertNewline( LPTSTR pText ) const;

    boolean     LaunchInstaller( LPCTSTR pParam );
    HMODULE     LoadMsiLibrary();
    DWORD       WaitForProcess( HANDLE hHandle );

    boolean     GetCmdLineParameters( LPTSTR *pCmdLine );
    DWORD       GetNextArgument( LPCTSTR pStr, LPTSTR *pArg,
                                 LPTSTR *pNext, boolean bStripQuotes = false );
    boolean     IsAdmin();

    boolean     GetCommandLine();

    boolean     IsTerminalServerInstalled() const;
    void        AddFileToPatchList( TCHAR* pPath, TCHAR* pFile );
    boolean     IsPatchInstalled( TCHAR* pBaseDir, TCHAR* pFileName );
    boolean     InstallRuntimes( TCHAR* pProductCode, TCHAR* pFileName );

public:
    UINT            m_uiRet;
                    SetupApp();
    virtual        ~SetupApp();

    virtual boolean Initialize( HINSTANCE hInst );
    virtual boolean AlreadyRunning() const;
    virtual boolean ReadProfile();
    virtual boolean GetPatches();
    virtual boolean ChooseLanguage( long& rLanguage );
    virtual boolean CheckVersion();
    virtual boolean CheckForUpgrade();
    virtual boolean InstallRuntimes();
    virtual boolean Install( long nLanguage );

    virtual UINT    GetError() const;
    virtual void    DisplayError( UINT nErr ) const;

    void            SetError( UINT nErr ) { m_uiRet = nErr; }
    boolean         IsWin9x() const { return m_bIsWin9x; }
    DWORD           GetOSVersion() const { return m_nOSVersion; }
    DWORD           GetMinorVersion() const { return m_nMinorVersion; }

    boolean         IsAdminInstall() { return m_bAdministrative; }
    void            SetAdminInstall( boolean bValue ) { m_bAdministrative = bValue; }

    void            SetRebootNeeded( boolean bNeedReboot ) { m_bNeedReboot = bNeedReboot; }
    boolean         NeedReboot() const { return m_bNeedReboot; }
    void            Log( LPCTSTR pMessage, LPCTSTR pText = NULL ) const;

    long            GetLanguageCount() const { return m_nLanguageCount; }
    long            GetLanguageID( long nIndex ) const;
    void            GetLanguageName( long nLanguage, LPTSTR sName ) const;

    LPCTSTR         GetAppTitle() const { return m_pAppTitle; }
    LPTSTR          SetProdToAppTitle( LPCTSTR pProdName );
    HINSTANCE       GetHInst() const { return m_hInst; }
};

//--------------------------------------------------------------------------
