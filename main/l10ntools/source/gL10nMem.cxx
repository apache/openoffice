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
using namespace std;
#include "gLang.hxx"



/*****************************************************************************
 ************************   G L 1 0 N M E M . C X X   ************************
 *****************************************************************************
 * This is the translation memory that links between the converts (from source
 * files) and to the language files. The memory contains the actual text info
 *****************************************************************************/



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_entry::l10nMem_entry(const string& srSourceFile, const string& srModuleName,
                             const string& srKey,        const string& srLanguage,
                             const string& srText)
                            : msSourceFile(srSourceFile),
                              msModuleName(srModuleName),
                              msKey(srKey),
                              msLanguage(srLanguage),
                              msText(srText)
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem_entry::~l10nMem_entry()
{}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem::l10nMem()
{
  mcMemory.push_back(l10nMem_entry("file1", "module1",
                                   "key", "da", "hovsa"));
  mcMemory.push_back(l10nMem_entry("file1", "module1",
                                   "key", "ja", "hovsa"));
}



/**********************   I M P L E M E N T A T I O N   **********************/
l10nMem::~l10nMem()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::save(const string& srTargetFile)
{
  int i;

  cout << "--------------- dump of l10n " << srTargetFile << "-----------------" << endl;

  for (i = 0; i < (int)mcMemory.size(); ++i)
    cout << "key: " << mcMemory[i].msKey << "  text: " << mcMemory[i].msText << endl;
  // JIX
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::setFileName(const string& srSourceFile)
{
  msCurrentSourceFileName = srSourceFile;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::setModuleName(const string& srModuleName)
{
  msCurrentModuleName = srModuleName;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void l10nMem::setEnUsKey(const string& srKey, const string& srText)
{
  string baseLanguage = "en-US";
  mcMemory.push_back(l10nMem_entry(msCurrentSourceFileName, msCurrentModuleName,
                                   srKey, baseLanguage, srText));
}



/**********************   I M P L E M E N T A T I O N   **********************/
vector<l10nMem_entry *>&  l10nMem::getLanguagesForKey(const string& srKey)
{
  int nL = mcMemory.size();

  mcCurrentSelection.clear();
  for (int i = 0; i < nL; ++i)
    if (mcMemory[i].msLanguage != "en-US")
      mcCurrentSelection.push_back(&mcMemory[i]);

  return mcCurrentSelection;
}
