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
#include "gCon.hxx"
#include "gConPo.hxx"
#include "gConProp.hxx"
#include "gConSrc.hxx"
#include "gConTree.hxx"
#include "gConUlf.hxx"
#include "gConXcs.hxx"
#include "gConXcu.hxx"
#include "gConXhp.hxx"
#include "gConXrm.hxx"

#include <iostream>
#include <fstream>



/*****************************************************************************
 ****************************   G C O N . C X X   ****************************
 *****************************************************************************
 * This is the generic conversion module, it handles all generic work of the
 * conversion, and offer utility functions to the specific conversion classes
 *****************************************************************************/



/*******************   G L O B A L   D E F I N I T I O N   *******************/
convert_gen_impl * convert_gen_impl::mcImpl = NULL;



/**********************   I M P L E M E N T A T I O N   **********************/
convert_gen::convert_gen(const std::string& srSourceFile, l10nMem& crMemory, const bool bMerge) 
{
  // did the user give a .xxx with the source file ?
  int nInx = srSourceFile.rfind(".");
  if (nInx == (int)std::string::npos)
    throw std::string("source file: ")+srSourceFile+" missing extension";

  // do we have an old object
  if (convert_gen_impl::mcImpl)
	delete convert_gen_impl::mcImpl;

  // find correct conversion class and create correct object
  std::string sExtension = srSourceFile.substr(nInx+1);
  if      (sExtension == "hrc")        convert_gen_impl::mcImpl = new convert_src(crMemory);
  else if (sExtension == "src")        convert_gen_impl::mcImpl = new convert_src(crMemory);
  else if (sExtension == "po")         convert_gen_impl::mcImpl = new convert_po(crMemory);
  else if (sExtension == "tree")       convert_gen_impl::mcImpl = new convert_tree(crMemory);
  else if (sExtension == "ulf")        convert_gen_impl::mcImpl = new convert_ulf(crMemory);
  else if (sExtension == "xcu")        convert_gen_impl::mcImpl = new convert_xcs(crMemory);
  else if (sExtension == "xhp")        convert_gen_impl::mcImpl = new convert_xhp(crMemory);
  else if (sExtension == "properties") convert_gen_impl::mcImpl = new convert_prop(crMemory);
  else throw std::string("unknown extension on source file: ")+srSourceFile;

  // and set environment
  convert_gen_impl::mcImpl->msSourceFile = srSourceFile;
  convert_gen_impl::mcImpl->mbMergeMode  = bMerge;
}



/**********************   I M P L E M E N T A T I O N   **********************/
convert_gen::~convert_gen()
{
  delete convert_gen_impl::mcImpl;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_gen::execute()
{
  // and load file
  convert_gen_impl::mcImpl->prepareFile(); 

  // and execute conversion
  convert_gen_impl::mcImpl->execute();
}



/**********************   I M P L E M E N T A T I O N   **********************/
convert_gen_impl::convert_gen_impl(l10nMem& crMemory)
	                              : mcMemory(crMemory)
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
convert_gen_impl::~convert_gen_impl()
{
  mcImpl = NULL;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_gen_impl::prepareFile()
{
  std::ifstream inputFile(msSourceFile.c_str(), std::ios::binary);

  
  if (!inputFile.is_open())
    throw std::string("Could not open ")+msSourceFile;

  // get length of file:
  mnSourceReadIndex = 0;
  inputFile.seekg (0, std::ios::end);
  msSourceBuffer.resize((unsigned int)inputFile.tellg());
  inputFile.seekg (0, std::ios::beg);

  // get size, prepare std::string and read whole file
  inputFile.read((char *)msSourceBuffer.c_str(), msSourceBuffer.size());
  if ((unsigned int)inputFile.gcount() != msSourceBuffer.size())
    throw std::string("cannot read whole file: "+msSourceFile);
  inputFile.close();
}




/**********************   I M P L E M E N T A T I O N   **********************/
void convert_gen_impl::lexRead(char *sBuf, int *nResult, int nMax_size)
{
  // did we hit eof
  if (mnSourceReadIndex == -1)
  {
    *nResult = 0;
    return;
  }

  // assume we can copy all that are left.
  *nResult = msSourceBuffer.size() - mnSourceReadIndex;

  // space enough for the whole line ?
  if (*nResult <= nMax_size)
  {
    msSourceBuffer.copy(sBuf, *nResult, mnSourceReadIndex);
    mnSourceReadIndex = -1;
  }
  else
  {
    msSourceBuffer.copy(sBuf, nMax_size, mnSourceReadIndex);
    *nResult           = nMax_size;
    mnSourceReadIndex += nMax_size;
  }
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_gen_impl::lineRead(bool *bEof, std::string& line)
{
  // did we hit eof
  if (mnSourceReadIndex == -1 || mnSourceReadIndex >= (int)msSourceBuffer.size())
  {
    *bEof             = true;
    mnSourceReadIndex = -1;
    return;
  }

  // find next newline
  int nNextLF = msSourceBuffer.find('\n', mnSourceReadIndex);
  if (nNextLF == (int)msSourceBuffer.npos)
    nNextLF = msSourceBuffer.size()+1;

  // copy std::string
  line              = msSourceBuffer.substr(mnSourceReadIndex, nNextLF - mnSourceReadIndex);
  mnSourceReadIndex = nNextLF +1;
  *bEof             = false;
  return;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_gen_impl::writeSourceFile(const std::string& line)
{
  if (!line.size())
	return;

  std::cout << line;
  // JIX
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_gen_impl::trim(std::string& sLine)
{
  int nL;

  // remove leading spaces
  nL = sLine.find_first_not_of(" \t");
  if (nL != (int)std::string::npos)
    sLine.erase(0, nL);

  // remove trailing spaces
  nL = sLine.find_last_not_of(" \t");
  if (nL != (int)std::string::npos)
    sLine.erase(nL +1);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_gen_impl::collectData(char *sText)
{
  msCollector += sText;
  if (msCollector[msCollector.size()-1] == '\n')
  {
    if (mbMergeMode)
      writeSourceFile(msCollector);
    msCollector.clear();
  }
}



/**********************   I M P L E M E N T A T I O N   **********************/
std::string& convert_gen_impl::copySource(char *yyText, bool bDoClear)
{
  msCopyText = yyText;

  // write text for merge
  if (mbMergeMode)
    writeSourceFile(msCollector + msCopyText);

  if (bDoClear)
    msCollector.clear();

  return msCopyText;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_gen_impl::isolateText(std::string& sText, int iStart, int *iEnd, std::string& sResult)
{
  int  iCur;


  // Skip initial space
  for (; sText[iStart] == ' ' || sText[iStart] == '\t' || sText[iStart] == '='; ++iStart) ;

  // time to collecting text or word ?
  if (sText[iStart] == '\"')
  {
    for (iCur = ++iStart; sText[iCur] != '\0'; ++iCur)
    {
	  // Escape next char
	  if (sText[iCur] == '\\')
	  {
		++iCur;
		continue;
	  }
	  if (sText[iCur] == '\"')
		break;
	}
	*iEnd = iCur+1;
  }
  else
  {
    for (iCur = iStart; sText[iCur] != '\0'; ++iCur)
    {
	  // Time to stop
	  if (sText[iCur] == '[' || sText[iCur] == ' ' || sText[iCur] == '\t' || sText[iCur] == '\n')
		break;
	}
	*iEnd = iCur;
  }

  // Update result
  sResult = sText.substr(iStart, iCur - iStart);
}
