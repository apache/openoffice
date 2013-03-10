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
#ifndef GL10NMEM_HXX
#define GL10NMEM_HXX
#include "gLang.hxx"



/*****************************************************************************
 ************************   G L 1 0 N M E M . H X X   ************************
 *****************************************************************************
 * This is the class definition header of the l10n localizer program,
 * all global classes and their interrelations is defined here
 *****************************************************************************/



/********************   C L A S S   D E F I N I T I O N   ********************/
class l10nMem_lang_entry
{
  public:
    l10nMem_lang_entry(const std::string& sText, bool bFuzzy);
    ~l10nMem_lang_entry();

    std::string msText;     // translated text from po file
    bool        mbFuzzy;    // fuzzy flag
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class l10nMem_enus_entry
{
  public:
    l10nMem_enus_entry(const std::string&   sKey,
                       const std::string&   sText,
                       int                  iLineNo,
                       int                  iFileInx,
                       l10nMem::ENTRY_STATE eState);
    ~l10nMem_enus_entry();

    std::string                     msKey;      // key in po file and source file
    std::string                     msText;     // en_US text from source file
    l10nMem::ENTRY_STATE            meState;    // status information
    int                             miFileInx;  // index of file name
    int                             miLineNo;   // line number
    std::vector<l10nMem_lang_entry> mcLangList; // language texts (index is languageId)
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class l10nMem_file_entry
{
  public:
    l10nMem_file_entry(const std::string& sFileName, int iStart);
    ~l10nMem_file_entry();

    std::string msFileName;  // file Name
    int         miStart;     // start index of entries in mcMasterEntries (l10Mem_db::mcENUS)
    int         miEnd;       // last index of entries in mcMasterEntries (l10Mem_db::mcENUS)
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class l10nMem_db
{
  public:
    l10nMem_db();
    ~l10nMem_db();

    int                             miCurFileInx;
    int                             miCurLangInx;
    int                             miCurENUSinx;
    int                             miCurLastENUSinx;
    bool                            mbNeedWrite;
    bool                            mbReorganizeNeeded;
    bool                            mbConvertMode;
    std::vector<l10nMem_enus_entry> mcENUSlist;
    std::vector<l10nMem_file_entry> mcFileList;
    std::vector<std::string>        mcLangList;


    void loadENUSkey    (int                iLineNo,
                         const std::string& sSourceFile,
                         const std::string& sKey,
                         const std::string& sText);
    void setLanguage    (const std::string& sLanguage,
                         bool               bCreate,
                        bool                bConvert);
    bool findFileName   (const std::string& sSourceFile, int iStart, bool bCreate);                         
    void loadLangKey    (int                iLineNo,
                         const std::string& sSourceFile,
                         const std::string& sKey,
                         const std::string& sOrgText,
                         const std::string& sText,
                         bool               bFuzzy);


    bool locateKey      (int                iLineNo,
                         const std::string& sKey,
                         const std::string& sText,
                         bool               bThrow = true);
    void reorganize();
    void addKey         (int                  iLineNo,
                         const std::string&   sKey,
                         const std::string&   sText,
                         l10nMem::ENTRY_STATE eStat);
};




/********************   C L A S S   D E F I N I T I O N   ********************/
class l10nMem_impl
{
  public:
    l10nMem_impl();
    ~l10nMem_impl();

    int  showError     (const std::string& sText, int iLineNo);
    int  showWarning   (const std::string& sText, int iLineNo);
    void showDebug     (const std::string& sText, int iLineNo);
    void showVerbose   (const std::string& sText, int iLineNo);

    void setModuleName (const std::string& sModuleName);
    void loadEntryKey  (int                iLineNo,
                        const std::string& sSourceFile,
                        const std::string& sKey,
                        const std::string& sOrgText,
                        const std::string& sText,
                        bool               bIsFuzzy);
    void setSourceKey  (int                iLineNo,
                        const std::string& sFilename,
                        const std::string& sKey,
                        const std::string& sText);

    void save         (l10nMem& cMem,
                       const std::string& sTargetDir,
                       bool               bKid);
    void dumpMem      (const std::string& sTargetDir);

  private:
    static bool                         mbVerbose;
    static bool                         mbDebug;
    static l10nMem_impl                *mcImpl;
    l10nMem_db                          mcDb;
    std::string                         msModuleName;
    bool                                mbInError;

    void formatAndShowText(const std::string& sType, int iLineNo, const std::string& sText);
    void convEntryKey     (int                iLineNo,
                           const std::string& sSourceFile,
                           const std::string& sKey,
                           const std::string& sOrgText,
                           const std::string& sText,
                           bool               bIsFuzzy);

    friend class l10nMem;
};
#endif
