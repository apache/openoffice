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
#include "gLang.hxx"
#include <fstream>



/*****************************************************************************
 ************************   G L 1 0 N M E M . C X X   ************************
 *****************************************************************************
 * This is the translation memory that links between the converts (from source
 * files) and to the language files. The memory contains the actual text info
 ***********************d******************************************************/



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_entry::l10nMem_entry(const std::string& srSourceFile, const std::string& srModuleName,
                             const std::string& srKey,        const std::string& srLanguage,
                             const std::string& srText,       const int iIndex)
                            : msSourceFile(srSourceFile),
                              msModuleName(srModuleName),
                              msKey(srKey),
                              msLanguage(srLanguage),
                              msText(srText),
							  miIndex(iIndex)
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_entry::~l10nMem_entry()
{}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem::l10nMem()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem::~l10nMem()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::save(const std::string& srTargetFile)
{
  int           i;
  std::string   sFile = srTargetFile + ".cnv";
  std::ofstream outputFile(sFile.c_str(), std::ios::binary);

  if (!outputFile.is_open())
    throw std::string("Could not open ")+srTargetFile;

	
  for (i = 0; i < (int)mcMemory.size(); ++i)
  {
	outputFile << mcMemory[i].msModuleName << "\t" << mcMemory[i].msSourceFile << "\t"
	           << mcMemory[i].msKey;
	if (mcMemory[i].miIndex)
	  outputFile << "." << mcMemory[i].miIndex;
	outputFile << "\t" << mcMemory[i].msLanguage  << "\t"
               << mcMemory[i].msText << std::endl;
  }
  // JIX
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::clear()
{
  mcMemory.clear();
  // JIX
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::setFileName(const std::string& srSourceFile)
{
  msCurrentSourceFileName = srSourceFile;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::setModuleName(const std::string& srModuleName)
{
  msCurrentModuleName = srModuleName;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::setEnUsKey(const std::string& srKey, const std::string& srText, int iIndex)
{
  std::string baseLanguage = "en-US";
  mcMemory.push_back(l10nMem_entry(msCurrentSourceFileName, msCurrentModuleName,
                                   srKey, baseLanguage, srText, iIndex));
}



/**********************   I M P L E M E N T A T I O N   **********************/
std::vector<l10nMem_entry *>&  l10nMem::getLanguagesForKey(const std::string& srKey)
{
  int nL = mcMemory.size();

  mcCurrentSelection.clear();
  for (int i = 0; i < nL; ++i)
    if (mcMemory[i].msLanguage != "en-US")
      mcCurrentSelection.push_back(&mcMemory[i]);

  return mcCurrentSelection;
}
