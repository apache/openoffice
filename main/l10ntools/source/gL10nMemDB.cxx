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
l10nMem_enus_entry::l10nMem_enus_entry(const std::string&   sKey,
                                       const std::string&   sText,
                                       int                  iLineNo,
                                       int                  iFileInx,
                                       l10nMem::ENTRY_STATE eState)
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
                       miCurLangInx(0),
                       miCurENUSinx(0),
                       miCurLastENUSinx(0),
                       mbNeedWrite(false),
                       mbReorganizeNeeded(false),
                       mbConvertMode(false)
{
  mcFileList.push_back(l10nMem_file_entry("", 0));
  mcLangList.push_back("");
  mcENUSlist.push_back(l10nMem_enus_entry("", "", 0, 0, l10nMem::ENTRY_DELETED));
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
  // create file name if needed
  if (mcFileList[miCurFileInx].msFileName != sSourceFile)
  {
    miCurFileInx = mcFileList.size();
    mcFileList.push_back(l10nMem_file_entry(sSourceFile, miCurENUSinx+1));
  }

  // add it to vector and update file pointer
  addKey(iLineNo, sKey, sText, l10nMem::ENTRY_DELETED);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_db::setLanguage(const std::string& sLanguage,
                             bool               bCreate,
                             bool               bConvert)
{
  int iSize = mcLangList.size();

  // regular load or convert of old po files
  mbConvertMode = bConvert;

  // With no languages selected only en_US is generated
  if (!sLanguage.size())
    miCurLangInx = 0;

  // en_US is loaded as master and cannot be loaded again
  if (sLanguage == "en_US")
    throw l10nMem::showError("en_US is loaded automatically");

  // check if language is already loaded
  for (miCurLangInx = 0; miCurLangInx < iSize && mcLangList[miCurLangInx] != sLanguage; ++miCurLangInx) ;
  if (miCurLangInx < iSize)
  {    
    if (bCreate)
      throw l10nMem::showError("loading " + sLanguage + " twice");
    return;
  }

  // language does not exist in db
  if (!bCreate)
    throw l10nMem::showError("language " + sLanguage + " not loaded");

  // create language
  mcLangList.push_back(sLanguage);

  // add language to all ENUS entries
  iSize = mcENUSlist.size();
  for (int i = 0; i < iSize; ++i)
    mcENUSlist[i].mcLangList.push_back(l10nMem_lang_entry("", false));
}



/**********************   I M P L E M E N T A T I O N   **********************/
bool l10nMem_db::findFileName(const std::string& sSourceFile, int iStart, bool bCreate)
{
  int iSize = mcFileList.size();

  // Check this or next file
  if (mcFileList[miCurFileInx].msFileName == sSourceFile)
    return true;
  if (++miCurFileInx < iSize && mcFileList[miCurFileInx].msFileName == sSourceFile)
    return true;

  for (miCurFileInx = 0;
       miCurFileInx < iSize && mcFileList[miCurFileInx].msFileName == sSourceFile;
       ++miCurFileInx) ;

  if (bCreate && miCurFileInx < iSize)
  {
    mcFileList.push_back(l10nMem_file_entry(sSourceFile, iStart));
    miCurFileInx = iSize;
  }
  return (miCurFileInx < iSize);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_db::loadLangKey(int                iLineNo,
                             const std::string& sSourceFile,
                             const std::string& sKey,
                             const std::string& sOrgText,
                             const std::string& sText,
                             bool               bFuzzy)
{
  if (!findFileName(sSourceFile, false, 0))
    throw l10nMem::showError(".po file contains unknown filename: " + sSourceFile);

  locateKey(iLineNo, sKey, sOrgText);

  l10nMem_lang_entry& xCur = mcENUSlist[miCurENUSinx].mcLangList[miCurLangInx];
  xCur.msText  = sText;
  xCur.mbFuzzy = bFuzzy;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_db::reorganize()
{
  // JIX (reorganize)
}



/**********************   I M P L E M E N T A T I O N   **********************/
bool l10nMem_db::locateKey(int                iLineNo,
                           const std::string& sKey,
                           const std::string& sText,
                           bool               bThrow)
{
  // Start from beginning of file and to end
  l10nMem_file_entry& cCur  = mcFileList[miCurFileInx];

  // Fast check first
  if (miCurENUSinx < (int)mcENUSlist.size() -1)
  {
    l10nMem_enus_entry& nowEntry = mcENUSlist[++miCurENUSinx];
    if (nowEntry.msText == sText && nowEntry.msKey == sKey)
      return true;
  }

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
void l10nMem_db::addKey(int                  iLineNo,
                        const std::string&   sKey,
                        const std::string&   sText,
                        l10nMem::ENTRY_STATE eStat)
{
  // JIX. addKey, handle fuzzy bit in lang (if changed).
  miCurENUSinx = mcENUSlist.size();
  mcENUSlist.push_back(l10nMem_enus_entry(sKey, sText, iLineNo, miCurFileInx, eStat));
  miCurLastENUSinx = mcFileList[miCurFileInx].miEnd = miCurENUSinx;
}
