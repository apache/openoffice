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



/*****************************************************************************
 *************************   G C O N S R C . H X X   *************************
 *****************************************************************************
 * This is the class header for .src and .hrc conversion
 *****************************************************************************/



/*****************   T E M P L A T E   D E F I N I T I O N   *****************/
#if 0
template<typename T>
  void RemoveChars(std::basic_string<T> & Str, const T *CharsToRemove)
  {
    std::basic_string<T>::size_type pos = 0;
    while ( (pos = Str.find_first_of(CharsToRemove, pos)) != std::basic_string<T>::npos)
        Str.erase(pos, 1); 
  }
#endif



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_src_impl : public convert_gen
{
  public:
    typedef enum
    {
      IGNORED      = 400,
      COMMEND,        DEFINEDRES, ANYTOKEN,  UNKNOWNTOKEN,  UNKNOWNCONSTRUCTION, UNKNOWNCHAR,
      FILTER_LEVEL = 500,
      CONDITION,      EMPTYLINE,  RESSOURCE, RESSOURCEEXPR, SMALRESSOURCE,       TEXTLINE,
      LONGTEXTLINE,   TEXT,       LEVELUP,   LEVELDOWN,     APPFONTMAPPING,      ASSIGNMENT, 
      LISTASSIGNMENT, LISTTEXT,   RSCDEFINE, RSCDEFINELEND, NEWTEXTINRES,        UIENTRIES, 
      PRAGMA,         _LISTTEXT,  TEXTREFID, LISTRESID,     _LISTRESID,          NORMDEFINE
    } LEX_TOKENS;

    convert_src_impl(const string& srSourceFile, l10nMem& crMemory);
    ~convert_src_impl();
    
    void addTokenToSet(LEX_TOKENS nToken, string srYYtext);
    void addCommentToSet(LEX_TOKENS nToken, string srYYtext);

  private:
    class tokenStorageEntry
    {
      public:
        tokenStorageEntry(LEX_TOKENS nToken, string& sYYtext): mnToken(nToken), msYYtext(sYYtext){};
        LEX_TOKENS mnToken;
        string     msYYtext;
    };

    void runLex();
    void extract();
    void insert();

    friend class convert_src;
};
#endif
