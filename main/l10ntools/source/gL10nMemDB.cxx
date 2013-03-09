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
#include "gL10nMem.hxx"

/*****************************************************************************
 **********************   G L 1 0 N M E M D B . C X X   **********************
 *****************************************************************************
 * This is the translation memory that links between the converts (from source
 * files) and to the language files. The memory contains the actual text info
 ***********************d******************************************************/



/*******************   G L O B A L   D E F I N I T I O N   *******************/



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_lang_entry::l10nMem_lang_entry(const std::string& sText, bool bFuzzy)
                                      :
                                       msText(sText),
                                       mbFuzzy(bFuzzy)
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_lang_entry::~l10nMem_lang_entry()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_enus_entry::l10nMem_enus_entry(const std::string& sKey,
                                       const std::string& sText,
                                       int                iLineNo,
                                       int                iFileInx,
                                       ENTRY_STATE        eState)
                                      :
                                       msKey(sKey),
                                       msText(sText),
                                       meState(eState),
                                       miFileInx(iFileInx),
                                       miLineNo(iLineNo)
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_enus_entry::~l10nMem_enus_entry()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_file_entry::l10nMem_file_entry(const std::string& sFileName, int iStart)
                                      :
                                       msFileName(sFileName),
                                       miStart(iStart),
                                       miEnd(iStart)
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_file_entry::~l10nMem_file_entry()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_db::l10nMem_db()
                      :
                       miCurFileInx(0),
                       miCurLangInx(-1),
                       miCurENUSinx(-1),
                       mbNeedWrite(false)
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_db::~l10nMem_db()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_db::loadENUSkey(int                iLineNo,
                             const std::string& sSourceFile,
                             const std::string& sKey,
                             const std::string& sText)
{
  setFileName(iLineNo, sSourceFile, true);

  // add it to vector and update file pointer
  addKey(iLineNo, sKey, sText, ENTRY_DELETED);
  mcFileList[miCurFileInx].miEnd = miCurENUSinx;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_db::loadLangKey(int                iLineNo,
                             const std::string& sSourceFile,
                             const std::string& sKey,
                             const std::string& sOrgText,
                             const std::string& sText,
                             bool               bFuzzy)
{
  locateKey(iLineNo, sKey, sOrgText);

  mcENUSlist[miCurENUSinx].mcLangList.push_back(l10nMem_lang_entry(sText, bFuzzy));
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_db::setLanguage(const std::string& sLanguage,
                             bool               bCreate)
{
  int iSize = mcLangList.size();


  // en-US is loaded as master and cannot be loaded again
  if (sLanguage == "en-US")
    throw l10nMem::showError("en-US is loaded automatically");

  // check if language is already loaded
  for (miCurLangInx = 0; miCurLangInx < iSize && mcLangList[miCurLangInx] != sLanguage; ++miCurLangInx) ;
  if (miCurLangInx < iSize)
  {    
    if (bCreate)
      throw l10nMem::showError("loading " + sLanguage + " twice");
  }
  else
  {
    if (bCreate)
      mcLangList.push_back(sLanguage);
    else
      throw l10nMem::showError("language " + sLanguage + " not loaded");
  }
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_db::setFileName(int                iLineNo,
                             const std::string& sFilename,
                             bool               bCreate)
{
  int iSize = mcFileList.size();

  // Same file as last
  if (mcFileList[miCurFileInx].msFileName == sFilename)
    return;

  // Locate file in index (or add it)
  for (miCurFileInx = 0; miCurFileInx < iSize &&
                         mcFileList[miCurFileInx].msFileName != sFilename; ++miCurFileInx) ;
  if (miCurFileInx == iSize)
  {
    if (bCreate)
      mcFileList.push_back(l10nMem_file_entry(sFilename, mcENUSlist.size()));
    else
      throw l10nMem::showError("file " + sFilename + " not located in po files", iLineNo);
  }
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_db::loadCleanup(bool bMaster)
{
  if (bMaster)
  {
    // JIX
  }
  else
  {
    // Insert dummy entries for missing lang entries
    int i, iSizeENUS = mcENUSlist.size();
    int iSizeLang = mcLangList.size();

    for (i = 0; i < iSizeENUS; ++i)
      if (iSizeLang > (int)mcENUSlist[i].mcLangList.size())
        mcENUSlist[i].mcLangList.push_back(l10nMem_lang_entry("", true));
  }
}



/**********************   I M P L E M E N T A T I O N   **********************/
bool l10nMem_db::locateKey(int                iLineNo,
                           const std::string& sKey,
                           const std::string& sText,
                           bool               bThrow)
{
  // Start from beginning of file and to end
  l10nMem_file_entry& cCur  = mcFileList[miCurFileInx];

  // Find match with key and text
  for (miCurENUSinx = cCur.miStart; miCurENUSinx <= cCur.miEnd; ++miCurENUSinx)
  {
    l10nMem_enus_entry& cEntry = mcENUSlist[miCurENUSinx];

    if (cEntry.msText == sText && cEntry.msKey == sKey)
      return true;
  }

  if (bThrow)
    throw l10nMem::showError("cannot find key(" + sKey +") with text(" + sText + ")", iLineNo);
  return false;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_db::addKey(int                iLineNo,
                        const std::string& sKey,
                        const std::string& sText,
                        ENTRY_STATE        eState)
{
  // add it to vector and update file pointer
  miCurENUSinx = mcENUSlist.size();
  mcENUSlist.push_back(l10nMem_enus_entry(sKey, sText, iLineNo, miCurFileInx, eState));
}