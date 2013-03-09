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
#include <algorithm>
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
     { l10nMem_impl::mcImpl->showWarning(sText, iLineNo); }



/**********************   I M P L E M E N T A T I O N   **********************/
bool l10nMem::isError()
{
  return l10nMem_impl::mcImpl->mbInError;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::setModuleName(const std::string& setModuleName)
     { l10nMem_impl::mcImpl->setModuleName(setModuleName); }
void l10nMem::loadENUSkey(int iL, const std::string& sS, const std::string& sK, const std::string& sT)
     { l10nMem_impl::mcImpl->mcDb.loadENUSkey(iL, sS, sK, sT); }
void l10nMem::setLanguage(const std::string& sL, bool bC)
     { l10nMem_impl::mcImpl->mcDb.setLanguage(sL, bC); }
void l10nMem::loadLangKey(int iL, const std::string& sS, const std::string& sK, const std::string& sO, const std::string& sT, bool               bI)
     { l10nMem_impl::mcImpl->mcDb.loadLangKey(iL, sS, sK, sO, sT, bI); }
void l10nMem::convLangKey(int iL, const std::string& sS, const std::string& sK, const std::string& sO, const std::string& sT, bool               bI)
     { l10nMem_impl::mcImpl->convLangKey(iL, sS, sK, sO, sT, bI); }
void l10nMem::setFileName(int iL, const std::string& sS, bool bC)
     { l10nMem_impl::mcImpl->mcDb.setFileName(iL, sS, bC); }
bool l10nMem::checkKey(const std::string& sKey, const std::string& sText)
     { return l10nMem_impl::mcImpl->mcDb.locateKey(0, sKey, sText, false); }
void l10nMem::setSourceKey(int iL, const std::string& sK, const std::string& sT)
     { l10nMem_impl::mcImpl->setSourceKey(iL, sK, sT); }
void l10nMem::reorganize()
     { l10nMem_impl::mcImpl->mcDb.reorganize(); }
void l10nMem::save(const std::string& sTargetDir)
     { l10nMem_impl::mcImpl->save(sTargetDir); }
void l10nMem::dumpMem(const std::string& sTargetDir)
     { l10nMem_impl::mcImpl->dumpMem(sTargetDir); }



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
void l10nMem_impl::setSourceKey(int                iLineNo,
                                const std::string& sKey,
                                const std::string& sText)
{
  // if key exist update state
  if (mcDb.locateKey(iLineNo, sKey, sText, false))
  {
    mcDb.mcENUSlist[mcDb.miCurENUSinx].meState = ENTRY_NORMAL;
  }
  else
  {
    // add key, if changed text this is wrong but handled in loadCleanup
    mcDb.addKey(iLineNo, sKey, sText, ENTRY_ADDED);
  }
}









#if 0
    void convLangKey   (const std::string& sSourceFile,
                        const std::string& sKey,
                        const std::string& sOrgText,
                        const std::string& sText,
                        bool               bIsFuzzy);

    void extractedKey (int                iLineNo,
                       const std::string& sKey,
                       const std::string& sText);

    void save         (const std::string& sTargetDir);
    void dumpMem      (const std::string& sTargetDir);

    void formatAndShowText(std::string& sType, int iLineNo, std::string& sText);
  std::cerr << "ERROR in " << msCurrentSourceFileName << ":" << iLineNo << ":  " << sText << std::endl;



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::save(const std::string& srTargetFile)
{
  int           i;
  std::string   sFile = srTargetFile + ".cnv";

  if (mbInError)
  {
    showError(0, (char *)"Cannot save file due to preceding errors");
    return;
  }
 
  std::ofstream outputFile(sFile.c_str(), std::ios::binary);

  if (!outputFile.is_open())
    throw showError(std::string("Could not open ")+srTargetFile);


  for (i = 0; i < (int)mcMemory.size(); ++i)
  {
    outputFile << mcMemory[i].msModuleName << "\t" << mcMemory[i].msSourceFile << "\t"
               << mcMemory[i].msKey;
    //if (mcMemory[i].miIndex)
    //  outputFile << "." << mcMemory[i].miIndex;
    outputFile << "\t" << mcMemory[i].msLanguage  << "\t" << mcMemory[i].msText << std::endl;
  }
  // JIX
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::loadLangKey(const std::string& srLang, const std::string& srKey, const std::string& srObjectType,
                               const std::string& srText)
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
std::vector<l10nMem_entry *>&  l10nMem_impl::getLanguagesForKey(const std::string& srKey)
{
  int nL = mcMemory.size();

  mcCurrentSelection.clear();
  if (srKey.size())
    for (int i = 0; i < nL; ++i)
      if (mcMemory[i].msLanguage != "en-US")
        mcCurrentSelection.push_back(&mcMemory[i]);

  return mcCurrentSelection;
}
#endif