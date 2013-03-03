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
class l10nMem_impl;
class l10nMem
{
  public:
    l10nMem();
    ~l10nMem();

    std::string showError    (int iLineNo, char *sText, bool bWarning = false);
    bool        isError      ();
    bool        checkKey     (const std::string& sKey,
                              const std::string& sObjectType);
    void        setFileName  (const std::string& srSourceFile);
    void        setModuleName(const std::string& srModuleName);
    void        setEnUsKey   (int                iLineNo,
                              const std::string& srKey,
                              const std::string& srObjectType,
                              const std::string& srText,
                              int                iIndex = 0);
    void        loadEnUsKey  (const std::string& srKey,
                              const std::string& srObjectType,
                              const std::string& srText,
                              const std::string& oldKey);
    void        loadLangKey  (const std::string& srLang,
                              const std::string& srKey,
                              const std::string& srObjectType,
                              const std::string& srText);
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_gen
{
  public:
    convert_gen(l10nMem& crMemory, const std::string& srSourceFile);
    ~convert_gen();

    // do extract/merge
    void execute(const bool bMerge);
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
    enum {DO_CONVERT, DO_EXTRACT, DO_MERGE} meWorkMode;
    l10nMem                  mcMemory;
    std::string              msModuleName;
    std::string              msPoOutDir;
    std::string              msPoDir;
    std::string              msSourceDir;
    std::string              msTargetDir;
    std::vector<std::string> mvSourceFiles;
    std::vector<std::string> mvLanguages;
    bool                     mbVerbose;

    void runConvert();
    void runExtractMerge(bool bMerge);

    void showUsage(std::string& sErr);
    void showManual();
    void loadL10MEM();
};
#endif
