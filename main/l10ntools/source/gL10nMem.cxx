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
 * This is the translation memory that links between the converts (from source
 * files) and to the language files. The memory contains the actual text info
 ***********************d******************************************************/



/*******************   G L O B A L   D E F I N I T I O N   *******************/
l10nMem_impl * l10nMem_impl::mcImpl = NULL;



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
std::string l10nMem::showError(int iLineNo, char *sText, bool bWarning)
{
  return l10nMem_impl::mcImpl->showError(iLineNo, sText, bWarning);
}



/**********************   I M P L E M E N T A T I O N   **********************/
bool l10nMem::isError()
{
  return l10nMem_impl::mcImpl->isError();
}



/**********************   I M P L E M E N T A T I O N   **********************/
bool l10nMem::checkKey(const std::string& sKey, const std::string& sObjectType)
{
  return l10nMem_impl::mcImpl->checkKey(sKey, sObjectType);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::setFileName(const std::string& srSourceFile)
{
  return l10nMem_impl::mcImpl->setFileName(srSourceFile);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::setModuleName(const std::string& srModuleName)
{
  return l10nMem_impl::mcImpl->setModuleName(srModuleName);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::setEnUsKey(int iLineNo, const std::string& srKey, const std::string& srObjectType,
                         const std::string& srText, int iIndex)
{
  return l10nMem_impl::mcImpl->setEnUsKey(iLineNo, srKey, srObjectType, srText, iIndex);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::loadEnUsKey(const std::string& srKey, const std::string& srObjectType,
                          const std::string& srText, const std::string& oldKey)
{
  l10nMem_impl::mcImpl->loadEnUsKey(srKey, srObjectType, srText, oldKey);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::loadLangKey(const std::string& srLang, const std::string& srKey, const std::string& srObjectType,
                          const std::string& srText)
{
  l10nMem_impl::mcImpl->loadLangKey(srLang, srKey, srObjectType, srText);
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_entry::l10nMem_entry(const std::string& srSourceFile, const std::string& srModuleName,
                             const std::string& srKey,        const std::string& srObjectType, 
                             const std::string& srLanguage,   const std::string& srText,
                             const int iIndex,
                             const std::string srOldKey)
                            : msSourceFile(srSourceFile),
                              msModuleName(srModuleName),
                              msKey(srKey),
                              msObjectType(srObjectType),
                              msLanguage(srLanguage),
                              msText(srText),
                              miIndex(iIndex),
                              msOLDKEY(srOldKey),
                              mbDeleted(true)
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_entry::~l10nMem_entry()
{}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_impl::l10nMem_impl()
                : mbInError(false),
                  miLastUniqResort(0)
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_impl::~l10nMem_impl()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
std::string l10nMem_impl::showError(int iLineNo, char *sText, bool bWarning)
{
  if (!bWarning)
    mbInError = true;

  std::cerr << "ERROR in " << msCurrentSourceFileName << ":" << iLineNo << ":  " << sText << std::endl;
  return "ERROR";
}



/**********************   I M P L E M E N T A T I O N   **********************/
bool l10nMem_impl::isError()
{
  return mbInError; 
}



/**********************   I M P L E M E N T A T I O N   **********************/
bool l10nMem_impl::checkKey(const std::string& sKey, const std::string& sObjectType)
{
  int i;

  for (i = 0; i < (int)mcMemory.size(); ++i)
    if (mcMemory[i].msModuleName == msCurrentModuleName     &&
        mcMemory[i].msSourceFile == msCurrentSourceFileName &&
        mcMemory[i].msObjectType == sObjectType             &&
        mcMemory[i].msKey        == sKey                    )
      return false;
  return true;
}



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
    throw std::string("Could not open ")+srTargetFile;


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
void l10nMem_impl::clear()
{
  mcMemory.clear();
  // JIX
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::setFileName(const std::string& srSourceFile)
{
  msCurrentSourceFileName = srSourceFile;

  // modify to please current sdf format
  std::replace(msCurrentSourceFileName.begin(), msCurrentSourceFileName.end(), '/', '\\' );
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::setModuleName(const std::string& srModuleName)
{
  msCurrentModuleName = srModuleName;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::setEnUsKey(int iLineNo, const std::string& srKey, const std::string& srObjectType, const std::string& srText, int iIndex)
{
  std::string baseLanguage = "en-US";
  std::string myKey = srKey;

  iLineNo;

  if (!checkKey(srKey, srObjectType))
  {
    std::stringstream ssBuf;

    ++miLastUniqResort;
    ssBuf  << miLastUniqResort;
    myKey +=  ".globUniq" + ssBuf.str();
  }
  mcMemory.push_back(l10nMem_entry(msCurrentSourceFileName, msCurrentModuleName,
                                   myKey, srObjectType, baseLanguage, srText, iIndex));
}




/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::loadEnUsKey(const std::string& srKey, const std::string& srObjectType,
                          const std::string& srText, const std::string& oldKey)
{
  std::string baseLanguage = "en-US";


  if (!checkKey(srKey, srObjectType))
  {
    showError(0, "en-US key is not uniq");
  }
  mcMemory.push_back(l10nMem_entry(msCurrentSourceFileName, msCurrentModuleName,
                                   srKey, srObjectType, baseLanguage, srText, 0, oldKey));
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem_impl::loadLangKey(const std::string& srLang, const std::string& srKey, const std::string& srObjectType,
                               const std::string& srText)
{
  int i;


  for (i = 0; i < (int)mcMemory.size(); ++i)
  {
    if (mcMemory[i].msSourceFile != msCurrentSourceFileName)
      continue;      
    if (mcMemory[i].msKey == srKey  || mcMemory[i].msOLDKEY == srKey)
      break;
  }
  if (i < (int)mcMemory.size())
  {
    mcMemory[i].mcLang.push_back(l10nMem_entry(msCurrentSourceFileName, msCurrentModuleName,
                                               srKey, srObjectType, srLang, srText, 0, std::string("")));
    return;
  }
    
  showError(0, (char *)(("cannot find key: " + srKey + " for lang " + srLang).c_str()));
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
