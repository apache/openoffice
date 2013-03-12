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
#include <iostream>
#include <fstream>
#include <sstream>


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
                                       int                  iLangSize,
                                       l10nMem::ENTRY_STATE eState)
                                      :
                                       msKey(sKey),
                                       msText(sText),
                                       meState(eState),
                                       miFileInx(iFileInx),
                                       miLineNo(iLineNo)
{
  int i;

  for (i = 0; i < iLangSize; ++i)
    mcLangList.push_back(l10nMem_lang_entry("", false));
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
  // Store fileName without relative path
  int i = msFileName.rfind("/");
  if (i == (int)std::string::npos)
    msPureName = msFileName;
  else
    msPureName = msFileName.substr(i+1);
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
                       mbNeedWrite(false),
                       mbConvertMode(false)
{
  mcFileList.push_back(l10nMem_file_entry("-genLang-", 0));
  mcLangList.push_back("-genLang-");
  mcENUSlist.push_back(l10nMem_enus_entry("-genLang-", "-genLang-", 0, 0, 0, l10nMem::ENTRY_DELETED));
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
  // add it to vector and update file pointer
  addKey(iLineNo, sSourceFile, sKey, sText, l10nMem::ENTRY_DELETED);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_db::setLanguage(const std::string& sLanguage,
                             bool               bCreate,
                             bool               bConvert)
{
  int i, iSize;

  // regular load or convert of old po files
  if (bConvert)
  {
    iSize = mcENUSlist.size();
    for (i = 0; i < iSize; ++i)
      mcENUSlist[i].meState = l10nMem::ENTRY_NORMAL;
  }
  mbConvertMode = bConvert;
  miCurFileInx  = 0;

  // With no languages selected only en_US is generated
  if (!sLanguage.size())
  {
    miCurLangInx = 0;
    return;
  }

  // en_US is loaded as master and cannot be loaded again
  if (sLanguage == "en_US")
    throw l10nMem::showError("en_US is loaded automatically");

  // check if language is already loaded
  iSize = mcLangList.size();
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
  for (i = 0; i < iSize; ++i)
    mcENUSlist[i].mcLangList.push_back(l10nMem_lang_entry("", false));
}



/**********************   I M P L E M E N T A T I O N   **********************/
bool l10nMem_db::findFileName(const std::string& sSourceFile)
{
  int iSize = mcFileList.size();

  // Check this or next file
  if (mcFileList[miCurFileInx].msFileName == sSourceFile)
    return true;
  if (++miCurFileInx < iSize && mcFileList[miCurFileInx].msFileName == sSourceFile)
    return true;

  for (miCurFileInx = 1;
       miCurFileInx < iSize && mcFileList[miCurFileInx].msFileName != sSourceFile;
       ++miCurFileInx) ;
  if (miCurFileInx == iSize)
  {
    miCurFileInx = 0;
    return false;
  }
  else
    return true;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_db::loadLangKey(int                iLineNo,
                             const std::string& sSourceFile,
                             const std::string& sKey,
                             const std::string& sOrgText,
                             const std::string& sText,
                             bool               bFuzzy)
{
  if (!locateKey(iLineNo, sSourceFile, sKey, sOrgText, true))
    throw l10nMem::showError(".po file contains unknown filename: " + sSourceFile + " or key: " + sKey);

  l10nMem_lang_entry& xCur = mcENUSlist[miCurENUSinx].mcLangList[miCurLangInx];
  xCur.msText  = sText;
  xCur.mbFuzzy = bFuzzy;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_db::reorganize()
{
  int iE, iEsize  = mcENUSlist.size();
  int iD, iDsize;
  std::vector<int> listDel, listAdd;


  // Check number of changes
  for (iE = 1; iE < iEsize; ++iE)
  {
    l10nMem_enus_entry& cur = mcENUSlist[iE];
    if (cur.meState == l10nMem::ENTRY_ADDED)
      listAdd.push_back(iE);
    if (cur.meState == l10nMem::ENTRY_DELETED)
      listDel.push_back(iE);
  }

  if (!listDel.size() || !listAdd.size())
    return;

  // loop through added text and see if key match deleted text
  iEsize = listAdd.size();
  iDsize = listDel.size();
  for (iE = 0; iE < iEsize; ++iE)
  {
    l10nMem_enus_entry& curAdd = mcENUSlist[listAdd[iE]];
    for (iD = 0; iD < iDsize; ++iD)
    {
      l10nMem_enus_entry& curE = mcENUSlist[listDel[iD]];

      if (curE.miFileInx != curAdd.miFileInx)
        continue;
      if (curE.msKey == curAdd.msKey)
        break;
      if (curE.msText == curAdd.msText)
        break;
    }
    if (iD == iDsize)
      continue;

    // Update deleted entry (original), because lang is connected here
    l10nMem_enus_entry& curDel = mcENUSlist[listDel[iD]];
    curDel.msText    = curAdd.msText;
    curDel.msKey     = curAdd.msKey;
    curDel.meState   = l10nMem::ENTRY_CHANGED;
    curAdd.meState   = l10nMem::ENTRY_DELETED;
  }
}



/**********************   I M P L E M E N T A T I O N   **********************/
bool l10nMem_db::locateKey(int                iLineNo,
                           const std::string& sSourceFile,
                           const std::string& sKey,
                           const std::string& sText,
                           bool               bThrow)
{
  // Position file pointer
  if (!findFileName(sSourceFile))
    return false;

  // Fast check, to see if next key is the one (normal with load and source without change)
  if (++miCurENUSinx < (int)mcENUSlist.size())
  {
    l10nMem_enus_entry& nowEntry = mcENUSlist[miCurENUSinx];
    if (nowEntry.msText == sText && nowEntry.msKey == sKey)
      return true;
  }

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
void l10nMem_db::addKey(int                  iLineNo,
                        const std::string&   sSourceFile,
                        const std::string&   sKey,
                        const std::string&   sText,
                        l10nMem::ENTRY_STATE eStat)
{
  // check file
  if (!findFileName(sSourceFile))
  {
    // prepare for new entry
    miCurENUSinx = mcENUSlist.size();
    miCurFileInx = mcFileList.size();

    // Create file
    mcFileList.push_back(l10nMem_file_entry(sSourceFile, miCurENUSinx));
   
    // and add entry at the back (no problem since it is a new file)
    mcENUSlist.push_back(l10nMem_enus_entry(sKey, sText, iLineNo, miCurFileInx,
                                            mcLangList.size(), eStat));
    mcFileList[miCurFileInx].miEnd = miCurENUSinx;
  }
  else
  {
    int iFsize                                   = mcFileList.size();
    l10nMem_file_entry& curF                     = mcFileList[miCurFileInx];
    std::vector<l10nMem_enus_entry>::iterator it = mcENUSlist.begin();

    // file is registred, so we need to add the entry at the end of the file range
    curF.miEnd++;
    mcENUSlist.insert(it + curF.miEnd,
                      l10nMem_enus_entry(sKey, sText, iLineNo, miCurFileInx,
                                         mcLangList.size(), eStat));
    for (int i = miCurFileInx+1; i < iFsize; ++i)
    {
      l10nMem_file_entry& curF2 = mcFileList[i];
      if (curF2.miStart >= curF.miEnd)
        curF2.miStart++;
      if (curF2.miEnd >= curF.miEnd)
        curF2.miEnd++;
    }
  }
}
