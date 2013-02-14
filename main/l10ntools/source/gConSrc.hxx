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
#include "gCon.hxx"



/*****************************************************************************
 *************************   G C O N S R C . H X X   *************************
 *****************************************************************************
 * This is the class header for .src conversion
 *****************************************************************************/



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_src : public convert_gen_impl
{
  public:
    convert_src(l10nMem& crMemory);
    ~convert_src();
    
    void pushKey(char *syyText);
    void popKey (char *syyText);
    void pushNoKey(char *syyText);
    void pushPlaceHolder(char *syyText);
    void registerHelpKey(char *syyText);
    void registerIdentKey(char *syyText);
    
    void saveData(char *syyText);
    void saveItemList(char *syyText);
	void startDefine(char *syyText);
	void collectData(char *sCollectedText);

  private:
    std::vector<std::string> mcStack;
    std::string         msCollector;
	bool                mbUseIdentifier;
	bool                mbDoDefine;

    void execute();
};
#endif