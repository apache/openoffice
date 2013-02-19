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
#ifndef GLANG_HXX
#define GLANG_HXX
#include <string>
#include <vector>



/*****************************************************************************
 ***************************   G L A N G . H X X   ***************************
 *****************************************************************************
 * This is the class definition header of the l10n localizer program,
 * all global classes and their interrelations is defined here
 *****************************************************************************/



/*******************   G L O B A L   D E F I N I T I O N   *******************/
extern bool gbVerbose;



/********************   C L A S S   D E F I N I T I O N   ********************/
class l10nMem_entry
{
  public:
    l10nMem_entry(const std::string& srSourceFile, const std::string& srModuleName, const std::string& srKey,
                  const std::string& srObjectType, const std::string& srLanguage,   const std::string& srText,
          const int          iIndex);
    ~l10nMem_entry();

    std::string msSourceFile;
    std::string msModuleName;
    std::string msKey;
  std::string msObjectType;
    std::string msLanguage;
    std::string msText;
  int         miIndex;

  private:
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class l10nMem
{
  public:
    l10nMem();
    ~l10nMem();

    void save         (const std::string& srTargetFile);
  void clear();
    void setFileName  (const std::string& srSourceFile);
    void setModuleName(const std::string& srModuleName);
    void setEnUsKey   (const std::string& srKey, const std::string& srObjectType, const std::string& srText, int iIndex = 0);
    std::vector<l10nMem_entry *>& getLanguagesForKey(const std::string& srKey);

  private:
    std::string                  msCurrentModuleName;
    std::string                  msCurrentSourceFileName;
    std::vector<l10nMem_entry *> mcCurrentSelection;
    std::vector<l10nMem_entry>   mcMemory;
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_gen
{
  public:
  convert_gen(const std::string& srSourceFile, l10nMem& crMemory, const bool bMerge);
  ~convert_gen();

  // do extract/merge
    void execute();
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class handler
{
  public:
    handler();
    ~handler();

    void checkCommandLine(int argc, char *argv[]);
    void run();

  private:
    enum {DO_EXTRACT, DO_EXTRACTMERGE, DO_GENERATE, DO_INSERT} meWorkMode;
    l10nMem                  mcMemory;
    std::string              msModuleName;
    std::string              msSourceDir;
    std::string              msTargetDir;
    std::vector<std::string> msSourceFiles;
  bool                      mbVerbose;

    void runExtractMerge(bool bMerge);
    void runGenerate();
    void runInsert();
};
#endif
