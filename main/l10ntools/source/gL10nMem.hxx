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
class l10Mem_lang_entry
{
  public:
    l10Mem_lang_entry(const std::string& sKey,
                             const std::string& sText);
    ~l10Mem_lang_entry();

  private:
    std::string msKey;     // only used in master: key in po file and source file
    std::string msText;    // in master en-US text otherwise translated text
    ENTRY_STATE meState;   // status information
    int         miFileInx; // index of file name
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class l10Men_lang_container
{
  public:
    l10Men_lang_container();
    ~l10Men_lang_container();

  private:
    std::string                    msLanguage; // language code
    std::vector<l10Mem_lang_entry> mcEntries;  // key/text entries
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class l10Mem_file_entry
{
  public:
    l10Mem_file_entry();
    ~l10Mem_file_entry();

  private:
    std::string msFile;  // file Name
    int         miStart; // start index of entries in mcEntries (l10Men_lang_container)
    int         miEnd;   // last index of entries in mcEntries (l10Men_lang_container)
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class l10Mem_db
{
  public:
    l10Mem_db();
    ~l10Mem_db();

  private:
    l10Men_lang_container              mcENUS;
    std::vector<l10Men_lang_container> mcLang;
    std::vector<l10Mem_file_entry>     mcFile;
};




/********************   C L A S S   D E F I N I T I O N   ********************/
class l10nMem_impl
{
  public:
    l10nMem_impl();
    ~l10nMem_impl();

    std::string showError    (const std::string& sText, int iLineNo = 0);
    std::string showWarning  (const std::string& sText, int iLineNo = 0);
    bool        isError      ();

    bool        checkKey     (const std::string& sKey);
    void        setFileName  (const std::string& sSourceFile);
    void        setModuleName(const std::string& sModuleName);
    void        setLanguage  (const std::string& sLanguage, bool bMaster = false);

    void        loadEnUsKey  (const std::string& sKey,
                              const std::string& sText);
    void        loadLangKey  (ENTRY_STATE        eFlag,
                              const std::string& sKey,
                              const std::string& sOrgText,
                              const std::string& sText);
    void        save();

    void        setEnUsKey   (int                iLineNo,
                              const std::string& sKey,
                              const std::string& srText);


  private:
    std::string                  msCurrentModuleName;
    std::string                  msCurrentSourceFileName;
    std::vector<l10nMem_entry *> mcCurrentSelection;
    std::vector<l10nMem_entry>   mcMemory;
    bool                         mbInError;
    int                          miLastUniqResort;
    static l10nMem_impl         *mcImpl;

    friend class l10nMem;
};
#endif
