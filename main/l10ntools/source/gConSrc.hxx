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
#include <stack>



/*****************************************************************************
 *************************   G C O N S R C . H X X   *************************
 *****************************************************************************
 * This is the class header for .src conversion
 *****************************************************************************/



/********************   C L A S S   D E F I N I T I O N   ********************/
class src_stack_entry
{
  public:
    src_stack_entry(string& sName);
    ~src_stack_entry();

    string   msName;
};




/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_src_impl : public convert_gen
{
  public:
    convert_src_impl(const string& srSourceFile, l10nMem& crMemory);
    ~convert_src_impl();
    
    void pushKey(string &sText);
    void popKey ();
    void pushNoKey();
    void registerPushKey();
    void pushRegistredKey(string &sText);
    
    void saveData(string& sCollectedText);
    void copyData(string& sCollectedText);

  private:
    stack<src_stack_entry> mcStack;
    bool                   mbCollectingData;
    string                 msCollector;

    void extract();
    void insert();
    void runLex();

    friend class convert_src;
};
#endif