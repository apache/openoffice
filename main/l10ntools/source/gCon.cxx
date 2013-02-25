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
  else if (sExtension == "xcu")        convert_gen_impl::mcImpl = new convert_xcu(crMemory);
  else if (sExtension == "xhp")        convert_gen_impl::mcImpl = new convert_xhp(crMemory);
  else if (sExtension == "xrm")        convert_gen_impl::mcImpl = new convert_xrm(crMemory);
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
bool convert_gen::isError()
{
  return convert_gen_impl::mcImpl->mbInError; 
}



/**********************   I M P L E M E N T A T I O N   **********************/
convert_gen_impl::convert_gen_impl(l10nMem& crMemory)
                                : mcMemory(crMemory),
                                  mbInError(false),
                                  miLineNo(1)
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
    throw showError("Cannot open file");

  // get length of file:
  mnSourceReadIndex = 0;
  inputFile.seekg (0, std::ios::end);
  msSourceBuffer.resize((unsigned int)inputFile.tellg());
  inputFile.seekg (0, std::ios::beg);

  // get size, prepare std::string and read whole file
  inputFile.read((char *)msSourceBuffer.c_str(), msSourceBuffer.size());
  if ((unsigned int)inputFile.gcount() != msSourceBuffer.size())
    throw showError("cannot read whole file");
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
void convert_gen_impl::writeSourceFile(const std::string& line)
{
  if (!line.size())
    return;

  std::cout << line;
  // JIX
}



/**********************   I M P L E M E N T A T I O N   **********************/
std::string& convert_gen_impl::copySource(char *yyText, bool bDoClear)
{
  int nL;


  msCopyText = yyText;

  // write text for merge
  if (mbMergeMode)
    writeSourceFile(msCollector + msCopyText);

  if (bDoClear)
    msCollector.clear();
  else
    msCollector += msCopyText;

  // remove any CR
  for (nL = 0; nL < (int)msCopyText.size(); ++nL)
  {
  if (msCopyText[nL] == '\r')
  {
    msCopyText.erase(nL, 1);
    --nL;
    continue;
  }
  if (msCopyText[nL] == '\n')
      ++miLineNo;
  }

  return msCopyText;
}



/**********************   I M P L E M E N T A T I O N   **********************/
std::string convert_gen_impl::showError(char *sText)
{
  mbInError = true;
  std::cerr << "ERROR in " << msSourceFile << ":" << miLineNo << ":  " << sText << std::endl;
  return "ERROR";
}