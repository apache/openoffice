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
using namespace std;
#include "gLang.hxx"



/*****************************************************************************
 *************************   G C O N U L F . C X X   *************************
 *****************************************************************************
 * This is the conversion for .ulf files
 *****************************************************************************/



/**********************   I M P L E M E N T A T I O N   **********************/
convert_ulf::convert_ulf(const string& srSourceFile, l10nMem& crMemory)
                        : convert_gen(srSourceFile, crMemory)
{
}



/*****************************************************************************/
convert_ulf::~convert_ulf()
{
}



/*****************************************************************************/
void convert_ulf::extract()
{
  handleLines(false);
}



/*****************************************************************************/
void convert_ulf::insert()
{
  handleLines(true);
}



/*****************************************************************************/
void convert_ulf::handleLines(bool bMergeMode)
{
  bool   bEof, bMultiLineComment = false;
  int    nL;
  string sWorkLine, sOrgLine, sLanguage, sKey, sText;


  // loop through all lines
  for (;;)
  {
    lineRead(&bEof, sOrgLine);
    if (bEof)
      break;

    // copy line so the original can be copied
    sWorkLine  = sOrgLine;
    sOrgLine  += "\n";

    // check for end of multiLineComment
    if (bMultiLineComment)
    {
      nL = sWorkLine.find("*/");
      if (nL == string::npos)
        sWorkLine.erase();
      else
      {
        bMultiLineComment = false;
        sWorkLine.erase(0,nL+2);
      }
    }

    // check for start of comment
    nL = sWorkLine.find("/*");
    if (nL != string::npos)
    {
      int nL2 = sWorkLine.find("*/");
      if (nL2 != string::npos)
        sWorkLine.erase(nL, nL2+2-nL);
      else
      {
        bMultiLineComment = true;
        sWorkLine.erase(nL);
      }
    }

    // remove leading/trailing blanks and handle empty lines
    trim(sWorkLine);
    if (!sWorkLine.size())
    {
      if (bMergeMode)
        writeSourceFile(sOrgLine);
      continue;
    }

    // the format is:
    // [key]
    // language = "text"

    // check for key
    if (sWorkLine[0] == '[')
    {
      sKey = sWorkLine.substr(1, sWorkLine.size()-2);

      // copy line if merging
      if (bMergeMode)
        writeSourceFile(sOrgLine);
    }
    else
    {
      // must be language line
      nL = sWorkLine.find_first_of("=");
      if (nL == string::npos)
        throw "unknown format in " + msSourceFile + " missing = in line: " + sOrgLine;
      sLanguage = sWorkLine.substr(0, nL);
      trim(sLanguage);

      nL = sWorkLine.find_first_of("\"");
      if (nL == string::npos)
        throw "unknown format: <<" + sOrgLine + ">> missing '='";
      sText = sWorkLine.substr(nL+1, sWorkLine.size()-2);
      trim(sText);

      // handle only en-US
      if (sLanguage != "en-US")
        continue;

      // copy line if merging or add to translation
      if (bMergeMode)
      {
        // get all languages (includes en-US)
        vector<l10nMem_entry *>& cExtraLangauges = mcMemory.getLanguagesForKey(sKey);
        string                   sNewLine;
        int                      nL = cExtraLangauges.size();


         writeSourceFile(sOrgLine);
        for (int i = 0; i < nL; ++i)
        {
          sNewLine = cExtraLangauges[i]->msLanguage +
                     " = \"" +
                     cExtraLangauges[i]->msText +
                     "\"\n";

          writeSourceFile(sNewLine);
        }
      }
      else
        mcMemory.setEnUsKey(sKey, sText);
    }
  }
}
