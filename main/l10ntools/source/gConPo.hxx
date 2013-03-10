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
#ifndef GCONPO_HXX
#define GCONPO_HXX
#include "gCon.hxx"
#include <iostream>
#include <fstream>



/*****************************************************************************
 **************************   G C O N P O . H X X   **************************
 *****************************************************************************
 * This is the class header for .po conversion
 *****************************************************************************/



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_po : public convert_gen_impl
{
  public:
    bool mbExpectId;
    bool mbExpectStr;


    convert_po(l10nMem& crMemory);
    ~convert_po();

    void startLook (char *syyText);
    void setValue  (char *syyText);
    void setFuzzy  (char *syyText);
    void setKey    (char *syyText);
    void setMsgId  (char *syyText);
    void setMsgStr (char *syyText);

  private:
    std::string  msValue;
    std::string  msKey;
    bool         mbFuzzy;
    std::filebuf outBuffer;

    void execute();

    void startSave(const std::string& sTargetDir,
                   const std::string& sLanguage,
                   const std::string& sFile);
    void save(const std::string& sKey,
              const std::string& sENUStext,
              const std::string& sText,
              bool               bFuzzy);
    void endSave();
};
#endif