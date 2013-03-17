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
#ifndef GCON_HXX
#define GCON_HXX
#include "gLang.hxx"

#include <iostream>
#include <fstream>



/*****************************************************************************
 ****************************   G C O N . H X X   ****************************
 *****************************************************************************
 * This is the class definition header for all converter classes,
 * all classes and their interrelations is defined here
 *****************************************************************************/



/*******************   G L O B A L   D E F I N I T I O N   *******************/



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_gen_impl
{
  public:
    static convert_gen_impl *mcImpl;


    convert_gen_impl(l10nMem& crMemory);
    virtual ~convert_gen_impl();

    // all converters MUST implement this function
    virtual void execute() = 0;

    // ONLY po should implement these functions
    virtual void startSave(const std::string& sLanguage,
                           const std::string& sFile);
    virtual void save(const std::string& sFileName,
                      const std::string& sKey,
                      const std::string& sENUStext,
                      const std::string& sText,
                      bool               bFuzzy);
    virtual void endSave();

    // generic variables
    bool         mbMergeMode;
    std::string  msSourcePath;
    std::string  msTargetPath;
    std::string  msSourceFile;
    l10nMem&     mcMemory;
    std::string  msCollector;
    int          miLineNo;


    // utility functions for converters
    void         lexRead (char *sBuf, int *nResult, int nMax_size);
    void         writeSourceFile(const std::string& line);
    std::string& copySource(char *yyText, bool bDoClear = true);

  protected:
    std::string  msSourceBuffer, msCopyText;
    int          miSourceReadIndex;

    bool prepareFile(bool bLoadMode, bool bAllowNoFile);

  private:
    std::ofstream outputFile;

    friend class convert_gen;
};
#endif