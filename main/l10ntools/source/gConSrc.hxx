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
#ifndef GCONSRCHXX
#define GCONSRCHXX
#include "gLang.hxx"
#include <vector>



/*****************************************************************************
 *************************   G C O N S R C . H X X   *************************
 *****************************************************************************
 * This is the class header for .src conversion
 *****************************************************************************/



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_src_impl : public convert_gen
{
  public:
    convert_src_impl(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose);
    ~convert_src_impl();
    
    void pushKey(std::string &sText);
    void popKey (std::string &sText);
    void pushNoKey(std::string &sText);
    void registerKey(std::string &sText);
    
    void saveData(std::string& sText);
    void saveItemList(std::string& sText);
    void copyData(std::string& sText);

  private:
    std::vector<std::string> mcStack;
    std::string         msCollector;
	bool                mbNoKey;

    void extract();
    void insert();
    void runLex();

    friend class convert_src;
};
#endif