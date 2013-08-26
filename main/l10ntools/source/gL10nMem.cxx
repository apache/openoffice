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
//#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>



/*****************************************************************************
 ************************   G L 1 0 N M E M . C X X   ************************
 *****************************************************************************
 * This is the interface to translation memory that links between the converts
 * and to the language files. The memory contains the actual text info
 ***********************d******************************************************/



/*******************   G L O B A L   D E F I N I T I O N   *******************/
l10nMem_impl * l10nMem_impl::mcImpl    = NULL;
bool           l10nMem_impl::mbVerbose = false;
bool           l10nMem_impl::mbDebug   = false;



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem::l10nMem()
{
  l10nMem_impl::mcImpl = new l10nMem_impl();
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem::~l10nMem()
{
  delete l10nMem_impl::mcImpl;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::setShowVerbose()
{
  l10nMem_impl::mbVerbose = true;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::setShowDebug()
{
  l10nMem_impl::mbDebug = true;
}



/**********************   I M P L E M E N T A T I O N   **********************/
int l10nMem::showError(const std::string& sText, int iLineNo)
     { return l10nMem_impl::mcImpl->showError(sText, iLineNo); }
int  l10nMem::showWarning(const std::string& sText, int iLineNo)
     { return l10nMem_impl::mcImpl->showWarning(sText, iLineNo); }
void l10nMem::showDebug(const std::string& sText, int iLineNo)
     { l10nMem_impl::mcImpl->showDebug(sText, iLineNo); }
void l10nMem::showVerbose(const std::string& sText, int iLineNo)
     { l10nMem_impl::mcImpl->showVerbose(sText, iLineNo); }
bool l10nMem::isError()
     { return l10nMem_impl::mcImpl->mbInError; }
void l10nMem::setModuleName(const std::string& sM)
     { l10nMem_impl::mcImpl->setModuleName(sM); }
void l10nMem::setLanguage(const std::string& sL, bool bC)
     { l10nMem_impl::mcImpl->mcDb.setLanguage(sL, bC); }
void l10nMem::setConvert(bool bC, bool bS)
     { l10nMem_impl::mcImpl->mcDb.setConvert(bC, bS); }
void l10nMem::loadEntryKey(int iL, const std::string& sS, const std::string& sK, const std::string& sO, const std::string& sT, bool               bI)
     { l10nMem_impl::mcImpl->loadEntryKey(iL, sS, sK, sO, sT, bI); }
void l10nMem::setSourceKey(int iL, const std::string& sF, const std::string& sK, const std::string& sT)
     { l10nMem_impl::mcImpl->setSourceKey(iL, sF, sK, sT); }
void l10nMem::save(const std::string& sT, bool bK, bool bF)
     { l10nMem_impl::mcImpl->save(*this, sT, bK, bF); }
void l10nMem::showNOconvert ()
     { l10nMem_impl::mcImpl->showNOconvert(); }

int  l10nMem::prepareMerge()
     { return l10nMem_impl::mcImpl->mcDb.prepareMerge(); }
void l10nMem::dumpMem(const std::string& sT)
     { l10nMem_impl::mcImpl->dumpMem(sT); }
bool l10nMem::getMergeLang(std::string& sL, std::string& sT)
     { return l10nMem_impl::mcImpl->mcDb.getMergeLang(sL, sT); }



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_impl::l10nMem_impl()
                          : 
                            mbInError(false)
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_impl::~l10nMem_impl()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
int l10nMem_impl::showError(const std::string& sText, int iLineNo)
{
  mbInError = true;
  formatAndShowText("ERROR", iLineNo, sText);
  return 1;
}



/**********************   I M P L E M E N T A T I O N   **********************/
int l10nMem_impl::showWarning(const std::string& sText, int iLineNo)
{
  formatAndShowText("WARNING", iLineNo, sText);
  return 2;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::showDebug(const std::string& sText, int iLineNo)
{
  if (mbDebug)
    formatAndShowText("DEBUG", iLineNo, sText);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::showVerbose(const std::string& sText, int iLineNo)
{
  if (mbVerbose)
    formatAndShowText("INFO", iLineNo, sText);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::setModuleName(const std::string& sModuleName)
{
  msModuleName = sModuleName;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::loadEntryKey(int                iLineNo,
                               const std::string& sSourceFile,
                               const std::string& sKey,
                               const std::string& sMsgId,
                               const std::string& sMsgStr,
                               bool               bIsFuzzy)
{
  if (mcDb.mbConvertMode)
    convEntryKey(iLineNo, sSourceFile, sKey, sMsgId, sMsgStr, bIsFuzzy);
  else if (!mcDb.miCurLangInx)
    mcDb.loadENUSkey(iLineNo, sSourceFile, sKey, sMsgId);
  else
    mcDb.loadLangKey(iLineNo, sSourceFile, sKey, sMsgId, sMsgStr, bIsFuzzy);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::setSourceKey(int                iLineNo,
                                const std::string& sSourceFile,
                                const std::string& sKey,
                                const std::string& sMsgId)
{
  std::string newText(sMsgId);
  int         i;


  // time to escape " and \ if contained in text or key
  for (i = 0; (i = newText.find("\\", i)) != (int)std::string::npos;)
  {
    ++i;
    if (i < (int)newText.size() && (newText[i] == '<' || newText[i] == '>'))
      ++i;
    else
    {
      newText.insert(i-1, "\\");
      ++i;
    }
  }
  for (i = 0; (i = newText.find("\"", i)) != (int)std::string::npos;)
  {
    newText.insert(i, "\\");
    i += 2;
  }

  // if key exist update state
  if (mcDb.locateKey(iLineNo, sSourceFile, sKey, newText, false))
  {
    mcDb.mcENUSlist[mcDb.miCurENUSinx].meState = l10nMem::ENTRY_NORMAL;
  }
  else
  {
    // add key, if changed text, this is wrong but handled in reorganize
    mcDb.addKey(iLineNo, sSourceFile, sKey, newText, l10nMem::ENTRY_ADDED);
  }
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::save(l10nMem& cMem, const std::string& sTargetDir, bool bKid, bool bForce)
{
  int iE, iEsize  = mcDb.mcENUSlist.size();
  int iL, iLsize  = mcDb.mcLangList.size();
  int iCntDeleted = 0, iCntChanged = 0, iCntAdded = 0;
  std::string sFileName = msModuleName + ".pot";
 

  // and reorganize db if needed
   mcDb.miCurFileInx = 0;
   mcDb.reorganize();

  // no save if there has been errors
  if(!needWrite(sFileName, bForce))
    return;

  //JIX save HANDLE KID

  // Save en-US
  {
    convert_gen savePo(cMem, sTargetDir, sTargetDir, sFileName);

    savePo.startSave("templates/", sFileName);
    for (iE = 1; iE < iEsize; ++iE)
    {
      l10nMem_enus_entry& cE = mcDb.mcENUSlist[iE];

      // remove deleted entries
      if (cE.meState == l10nMem::ENTRY_DELETED)
        continue;

      savePo.save(mcDb.mcFileList[cE.miFileInx].msFileName, cE.msKey, cE.msMsgId, "", false);
    }
    savePo.endSave();
  }

  // save all languages
  sFileName = msModuleName + ".po";
  for (iL = 1; iL < iLsize; ++iL)
  {
    convert_gen savePo(cMem, sTargetDir, sTargetDir, sFileName);

    savePo.startSave(mcDb.mcLangList[iL] + "/", sFileName);
    for (iE = 1; iE < iEsize; ++iE)
    {
      l10nMem_enus_entry& cE = mcDb.mcENUSlist[iE];
      l10nMem_lang_entry& cL = cE.mcLangText[iL];
      bool                bF = cL.mbFuzzy;

      // remove deleted entries
      if (cE.meState == l10nMem::ENTRY_DELETED)
        continue;

      savePo.save(mcDb.mcFileList[cE.miFileInx].msFileName, cE.msKey, cE.msMsgId, cL.msMsgStr, bF);
    }
    savePo.endSave();
  }
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::showNOconvert ()
{
  int iE, iEsize  = mcDb.mcENUSlist.size();

  for (iE = 1; iE < iEsize; ++iE)
  {
    l10nMem_enus_entry& cE = mcDb.mcENUSlist[iE];  

    if (cE.meState == l10nMem::ENTRY_DELETED)
    {
      showError("template key(" + cE.msKey + ") msgId(" + cE.msMsgId + ") not in pot file", 0);
    }
  }
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::dumpMem(const std::string& sFileName)
{
  // and reorganize db if needed
  mcDb.reorganize();

  // no save if there has been errors
  if(!needWrite(sFileName, true))
    return;

  // JIX (dumpMem)
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::formatAndShowText(const std::string& sType, int iLineNo, const std::string& sText)
{
  std::string& cFile = mcDb.mcFileList[mcDb.miCurFileInx].msFileName;

  std::cout << sType;
  if (mcDb.miCurFileInx > 0)
    std::cout << " in " << mcDb.mcFileList[mcDb.miCurFileInx].msFileName;
  if (iLineNo)
    std::cout << "(" << iLineNo << ")";
  std::cout << ":  " << sText << std::endl;
}



/**********************   I M P L E M E N T A T I O N   **********************/
bool l10nMem_impl::needWrite(const std::string sFileName, bool bForce)
{
  int iE, iEsize  = mcDb.mcENUSlist.size();
  int iCntDeleted = 0, iCntChanged = 0, iCntAdded = 0;

  // no save if there has been errors
  if (mbInError)
    throw l10nMem::showError("Cannot save due to previous errors");

  // Check number of changes
  for (iE = 1; iE < iEsize; ++iE)
  {
    l10nMem_enus_entry& cur = mcDb.mcENUSlist[iE];
    if (cur.meState == l10nMem::ENTRY_ADDED)
      ++iCntAdded;
    if (cur.meState == l10nMem::ENTRY_CHANGED)
    {
      ++iCntChanged;
      if (mcDb.mbConvertMode)
        cur.meState = l10nMem::ENTRY_NORMAL;
    }
    if (cur.meState == l10nMem::ENTRY_DELETED)
      ++iCntDeleted;
  }
  if (!mcDb.mbConvertMode)
    iCntDeleted -= iCntChanged;
  if (!iCntAdded && !iCntChanged && !iCntDeleted)
  {
    std::cout << "genLang: No changes in " <<   sFileName;
    if (bForce)
      std::cout << ", -o switch used, so files are saved" << std::endl;
    else
      std::cout << " skipping \"save\"" << std::endl;
    return bForce;
  }

  std::cout << "genLang statistics: " << iCntDeleted << " deleted, "
                                      << iCntChanged << " changed, "
                                      << iCntAdded   << " added entries in "
                                      << sFileName   << std::endl;
  return true;
}



/**********************   I M P L E M E N T A T I O N   **********************/
bool l10nMem_impl::convFilterWarning(const std::string& sSourceFile,
                                     const std::string& sKey,
                                     const std::string& sMsgId)
{
  if (sMsgId == "-")
    return true;

  if (msModuleName == "basic")
  {
    if (sSourceFile == "basic.src"    ||
        sSourceFile == "basmsg.src"   ||
        sSourceFile == "svtmsg.src"   ||
        sSourceFile == "testtool.src" ||
        sSourceFile == "ttmsg.src"    )
      return true;
  }

  return false;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::convEntryKey(int                iLineNo,
                               const std::string& sSourceFile,
                               const std::string& sKey,
                               const std::string& sMsgId,
                               const std::string& sMsgStr,
                               bool               bIsFuzzy)
{
  std::vector<int> ivEntryList;
  int              i, iSize;


  // Find all matching file names (old system does not have directory.
  // build list of potential entries
  iSize = mcDb.mcFileList.size();
  for (i = 1; i < iSize; ++i)
    if (sSourceFile == mcDb.mcFileList[i].msPureName)
    {
      int j    = mcDb.mcFileList[i].miStart;
      int iEnd = mcDb.mcFileList[i].miEnd;

      for (; j <= iEnd; ++j)
        ivEntryList.push_back(j);
    }

  // Did we find one or more files ?
  iSize = ivEntryList.size();
  if (!iSize)
  {
    showWarning("source file(" + sSourceFile + ") not in template file", iLineNo);
    return;
  }

  // Loop through all potential en-US entries
  for (i = 0; i < iSize; ++i)
  {
    l10nMem_enus_entry& curE = mcDb.mcENUSlist[ivEntryList[i]];

    // The entry must be unconverted and msgId must match
    if (curE.meState == l10nMem::ENTRY_DELETED && sMsgId == curE.msMsgId)
    {
      curE.meState = l10nMem::ENTRY_NORMAL;
      break;
    }
  }
  if (i == iSize)
  {
    showWarning("file(" + sSourceFile + ") key(" + sKey + ") msgId(" + sMsgId + ") not found in templates", iLineNo);
  }

  if (mcDb.mbStrictMode)
    return;


{
  std::string      sKeyUpper;


  // filter out dummy messages, silently
  if (convFilterWarning(sSourceFile, sKey, sMsgId))
    return;

  // make copy of key in upper case
  sKeyUpper = sKey;
  l10nMem_db::keyToUpper(sKeyUpper);



  // Loop through all potential en-US entries
  int iCandidate;
  for (iCandidate = -1, i = 0; i < iSize; ++i)
  {
    l10nMem_enus_entry& curE = mcDb.mcENUSlist[ivEntryList[i]];

    // The entry cannot be converted twice
    if (curE.meState != l10nMem::ENTRY_NORMAL)
      continue;

    // The msgId must match
    if (sMsgId == curE.msMsgId)
      break;

    // compare keys, but be aware of different length
    if (sKeyUpper.find(curE.msKey) != std::string::npos)
    {
      // is this the second (or more candidate ? then we cannot use key
      if (iCandidate > 0)
        iCandidate = -1;
      else
        iCandidate = i;
    }
  }
  sKeyUpper.clear();
  if (i == iSize && iCandidate < 0)
    sKeyUpper = "cannot be matched";
  else
  {
    // Primarely use text match, alternatively use key candidate 
    if (i == iSize)
    {
      i = iCandidate;
      sKeyUpper = "could only be matched through KEY";
    }

    // update language text
    l10nMem_enus_entry& curE = mcDb.mcENUSlist[ivEntryList[i]];
    l10nMem_lang_entry& curL = curE.mcLangText[mcDb.miCurLangInx];

    if (sMsgStr != curL.msMsgStr)
    {
      curL.msMsgStr = sMsgStr;
      curL.mbFuzzy  = bIsFuzzy;
      curE.meState  = l10nMem::ENTRY_CHANGED;
    }
    if (sKeyUpper.size())
    {
      showWarning("file(" + sSourceFile + ") key(" + sKey + ")  lang(" + mcDb.mcLangList[mcDb.miCurLangInx] +
                  ") with msgId(" + sMsgId + ") " + sKeyUpper, iLineNo);
    }
  }
  }
}
