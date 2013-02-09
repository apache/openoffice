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
#include "gConUlf.hxx"



/*****************************************************************************
 *************************   G C O N U L F . C X X   *************************
 *****************************************************************************
 * This is the conversion for .ulf files
 *****************************************************************************/



/*****************************   G L O B A L S   *****************************/
convert_ulf_impl * convert_ulf::mcpImpl;



/**********************   I M P L E M E N T A T I O N   **********************/
convert_ulf::convert_ulf(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose)
                        : convert_gen(srSourceFile, crMemory, brVerbose) 
                          {mcpImpl = new convert_ulf_impl(srSourceFile, crMemory, brVerbose);}
convert_ulf::~convert_ulf() {delete mcpImpl;}
void convert_ulf::extract() {mcpImpl->extract();}
void convert_ulf::insert()  {mcpImpl->insert();}



/**********************   I M P L E M E N T A T I O N   **********************/
convert_ulf_impl::convert_ulf_impl(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose)
                                  : convert_gen (srSourceFile, crMemory, brVerbose)
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
convert_ulf_impl::~convert_ulf_impl()
{
}



/*****************************************************************************/
void convert_ulf_impl::extract()
{
  // generate l10mMem
  mbMergeMode = false;

  // run lex parser and build token tree
  runLex();
}



/*****************************************************************************/
void convert_ulf_impl::insert()
{
  // generate l10mMem
  mbMergeMode = true;

  // run lex parser and build token tree
  runLex();
}



/*****************************************************************************/
void convert_ulf::handleLines()
{
  bool   bEof, bMultiLineComment = false;
  int    nL;
  std::string sWorkLine, sKey, sText;


  // loop through all lines
  for (;;)
  {
    lineRead(&bEof, sWorkLine);
    if (bEof)
      break;

	// write back original line (add languages are added later)
	if (mbMergeMode)
      writeSourceFile(sWorkLine + "\n");

    // check for end of multiLineComment
    if (bMultiLineComment)
    {
      nL = sWorkLine.find("*/");
      if (nL == (int)std::string::npos)
		continue;

      bMultiLineComment = false;
      sWorkLine.erase(0,nL+2);
    }

    // check for start of comment
    nL = sWorkLine.find("/*");
    if (nL != (int)std::string::npos)
    {
      int nE = sWorkLine.find("*/");
      if (nE == (int)std::string::npos)
      {
        bMultiLineComment = true;
        continue;
      }

	  // remove comment from line
      sWorkLine.erase(nL, nE+2-nL);
    }

    // remove leading/trailing blanks and handle empty lines
    trim(sWorkLine);
    if (!sWorkLine.size() ||
		((int)sWorkLine.size() == 1 &&
		   (sWorkLine[0] == ' ' || sWorkLine[0] == '\t')))
      continue;

    // the format is:
    // [key]
    // language = "text"

    // check for key
    if (sWorkLine[0] == '[')
	{
      sKey = sWorkLine.substr(1, sWorkLine.size()-2);
	  continue;
	}

    // must be language line
    nL = sWorkLine.find_first_of("=");
    if (nL == (int)std::string::npos)
      throw "unknown format in " + msSourceFile + " missing = in line: " + sWorkLine;

    nL = sWorkLine.find_first_of("\"");
    if (nL == (int)std::string::npos)
      throw "unknown format: <<" + sWorkLine + ">> missing '='";

	if (!mbMergeMode)
	{
	  sText = sWorkLine.substr(nL+1, sWorkLine.size()-nL-2);
      trim(sText);
      mcMemory.setEnUsKey(sKey, sText);
	  continue;
	}

    // copy line if merging or add to translation
    {
      // get all languages (includes en-US)
      std::vector<l10nMem_entry *>& cExtraLangauges = mcMemory.getLanguagesForKey(sKey);
      std::string                   sNewLine;
      nL = cExtraLangauges.size();


      for (int i = 0; i < nL; ++i)
      {
        sNewLine = cExtraLangauges[i]->msLanguage +
                   " = \"" +
                   cExtraLangauges[i]->msText +
                   "\"\n";

        writeSourceFile(sNewLine);
      }
    }
  }
}
