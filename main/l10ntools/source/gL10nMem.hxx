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
class l10nMem_entry
{
  public:
    l10nMem_entry(const std::string& srSourceFile, const std::string& srModuleName, const std::string& srKey,
                  const std::string& srObjectType, const std::string& srLanguage,   const std::string& srText,
                  const int          iIndex,
                  const std::string  srOldKey = "");
    ~l10nMem_entry();

    std::string msSourceFile;
    std::string msModuleName;
    std::string msKey;
    std::string msObjectType;
    std::string msLanguage;
    std::string msText;
    std::string msOLDKEY;
    int         miIndex;
    bool        mbDeleted;
    std::vector<l10nMem_entry> mcLang;

  private:
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class l10nMem_impl
{
  public:
    l10nMem_impl();
    ~l10nMem_impl();

    std::string                   showError         (int iLineNo, char *sText, bool bWarning = false);
    bool                          isError           ();
    bool                          checkKey          (const std::string& sKey,
                                                     const std::string& sObjectType);
    void                          save              (const std::string& srTargetFile);
    void                          clear();
    void                          setFileName       (const std::string& srSourceFile);
    void                          setModuleName     (const std::string& srModuleName);
    void                          setEnUsKey        (int        iLineNo,const std::string& srKey,
                                                     const std::string& srObjectType,
                                                     const std::string& srText, int iIndex = 0);
    void        loadEnUsKey  (const std::string& srKey,
                              const std::string& srObjectType,
                              const std::string& srText,
                              const std::string& oldKey);
    void        loadLangKey  (const std::string& srLang,
                              const std::string& srKey,
                              const std::string& srObjectType,
                              const std::string& srText);
    std::vector<l10nMem_entry *>& getLanguagesForKey(const std::string& srKey);

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
