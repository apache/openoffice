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
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>



/*****************************************************************************
 ***************************   G L A N G . H X X   ***************************
 *****************************************************************************
 * This is the class definition header of the l10n localizer program,
 * all classes and their interrelations is defined here
 *****************************************************************************/



/********************   C L A S S   D E F I N I T I O N   ********************/
class l10nMem_entry
{
  public:
    l10nMem_entry(const std::string& srSourceFile, const std::string& srModuleName, const std::string& srKey,
                  const std::string& srLanguage,   const std::string& srText);
    ~l10nMem_entry();

    std::string msSourceFile;
    std::string msModuleName;
    std::string msKey;
    std::string msLanguage;
    std::string msText;

  private:
};


/********************   C L A S S   D E F I N I T I O N   ********************/
class l10nMem
{
  public:
    l10nMem();
    ~l10nMem();

    void save(const std::string& srTargetFile);
	void clear();
    void setFileName(const std::string& srSourceFile);
    void setModuleName(const std::string& srModuleName);
    void setEnUsKey(const std::string& srKey, const std::string& srText);
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
    convert_gen(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose);
    virtual ~convert_gen();
    
    static convert_gen& getConverter(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose);

    virtual void extract() = 0;
    virtual void insert()  = 0;

    void lexRead (char *sBuf, int *nResult, int nMax_size);
    void lineRead(bool *bEof, std::string& line);
    void writeSourceFile(const std::string& line);
    void trim(std::string& line);


  protected:
    bool               mbMergeMode;
	bool               mbVerbose;
    const std::string& msSourceFile;
    std::string        msSourceBuffer;
    int                mnSourceReadIndex;
    l10nMem&           mcMemory;

  private:
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_po_impl;
class convert_po : public convert_gen
{
  public:
    static convert_po_impl *mcpImpl;


    convert_po(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose);
    ~convert_po();
    
    void extract();
    void insert();

  private:
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_src_impl;
class convert_src : public convert_gen
{
  public:
    static convert_src_impl *mcpImpl;


    convert_src(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose);
    ~convert_src();
    
    void extract();
    void insert();

  private:
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_hrc_impl;
class convert_hrc : public convert_gen
{
  public:
    static convert_hrc_impl *mcpImpl;


    convert_hrc(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose);
    ~convert_hrc();
    
    void extract();
    void insert();

  private:
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_tree : public convert_gen
{
  public:
    convert_tree(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose);
    ~convert_tree();
    
    void extract();
    void insert();

  private:
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_ulf : public convert_gen
{
  public:
    convert_ulf(const std::string& srSourceFile, l10nMem& srMemory, bool brVerbose);
    ~convert_ulf();
    
    void extract();
    void insert();

  private:
    void handleLines();
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_xcu_impl;
class convert_xcu : public convert_gen
{
  public:
    static convert_xcu_impl *mcpImpl;


    convert_xcu(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose);
    virtual ~convert_xcu();
    
    virtual void extract();
    virtual void insert();

  private:
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_xcs_impl;
class convert_xcs : public convert_gen
{
  public:
    static convert_xcs_impl *mcpImpl;


    convert_xcs(const std::string& srSourceFile, l10nMem& srMemory, bool brVerbose);
    ~convert_xcs();
    
    void extract();
    void insert();

  private:
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_xrm_impl;
class convert_xrm : public convert_gen
{
  public:
    static convert_xrm_impl *mcpImpl;


    convert_xrm(const std::string& srSourceFile, l10nMem& srMemory, bool brVerbose);
    ~convert_xrm();
    
    void extract();
    void insert();

  private:
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_xhp_impl;
class convert_xhp : public convert_gen
{
  public:
    static convert_xhp_impl *mcpImpl;


    convert_xhp(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose);
    ~convert_xhp();
    
    void extract();
    void insert();

  private:
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_properties : public convert_gen
{
  public:
    convert_properties(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose);
    ~convert_properties();
    
    void extract();
    void insert();

  private:
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
    enum {DO_NONE, DO_EXTRACT, DO_MERGE, DO_GENERATE, DO_INSERT} meWorkMode;
    l10nMem        mcMemory;
    std::string    msModuleName;
    std::string    msSourceDir;
    std::string    msTargetDir;
    bool           mbVerbose;
    std::vector<std::string> msSourceFiles;

    void runExtract();
    void runMerge();
    void runGenerate();
    void runInsert();
};
#endif
