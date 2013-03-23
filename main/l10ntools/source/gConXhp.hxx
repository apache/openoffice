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
#ifndef GCONXHP_HXX
#define GCONXHP_HXX
#include "gCon.hxx"



/*****************************************************************************
 *************************   G C O N X H P . H X X   *************************
 *****************************************************************************
 * This is the class definition header xhp converter
 *****************************************************************************/



/********************   C L A S S   D E F I N I T I O N   ********************/
class lang_container
{
  public:
    lang_container(const std::string& sLang, const std::string& sFileName);
    lang_container();
    ~lang_container();

    std::string   msLang;
//    std::ofstream msOutFile;
};



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_xhp : public convert_gen_impl
{
  public:
    convert_xhp(l10nMem& crMemory);
    ~convert_xhp();

    void setString(char *yytext);
    void openTag(char *yytext);
    void closeTag(char *yytext);
    void setId(char *yytext);
    void setLang(char *yytext);
    void setRef(char *yytext);
    void openTransTag(char *yytext);
    void closeTransTag(char *yytext);
    void stopTransTag(char *yytext);
    void startComment(char *yytext);
    void stopComment(char *yytext);
    void handleSpecial(char *yytext);
    void handleDataEnd(char *yytext);
    void duplicate(char *yytext);
    std::string& copySourceSpecial(char *yytext, int iType = 0);

  private:
    typedef enum
    {
      VALUE_NOT_USED,
      VALUE_IS_TAG,
      VALUE_IS_TAG_TRANS,
      VALUE_IS_VALUE,
      VALUE_IS_VALUE_TAG
    } STATE;
    STATE       meExpectValue, mePushValue;
    std::string msKey, msPushCollect;

    std::vector<lang_container> mbLanguages;
    void                        execute();
};
#endif
