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
#ifndef GCONXCU_HXX
#define GCONXCU_HXX
#include "gCon.hxx"



/*****************************************************************************
 *************************   G C O N X C U . H X X   *************************
 *****************************************************************************
 * This is the class header for .xcu conversion
 *****************************************************************************/



/********************   C L A S S   D E F I N I T I O N   ********************/
class xcu_stack_entry;
class convert_xcu : public convert_gen_impl
{
  public:
    typedef enum {TAG_COMPONENT, TAG_PROP, TAG_NODE } TAG_TYPE;
    convert_xcu(l10nMem& crMemory);
    ~convert_xcu();

    void pushKeyPart(TAG_TYPE eIsNode, char *syyText);
    void popKeyPart (TAG_TYPE eIsNode, char *syyText);

    void startCollectData(char *syyText);
    void stopCollectData(char *syyText);

  private:
    std::vector<std::string> mcStack;
  std::string              msObj;
  bool                     mbCollectingData;


    void execute();
};
#endif