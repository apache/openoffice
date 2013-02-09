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
#include "gLang.hxx"



/*****************************************************************************
 ****************************   G C O N . C X X   ****************************
 *****************************************************************************
 * This is the generic conversion module, it handles all generic work of the
 * conversion, and offer utility functions to the specific conversion classes
 *****************************************************************************/



/**********************   I M P L E M E N T A T I O N   **********************/
convert_gen::convert_gen(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose)
                        : msSourceFile(srSourceFile),
						  mbVerbose(brVerbose),
                          mcMemory(crMemory)
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
convert_gen::~convert_gen()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
convert_gen& convert_gen::getConverter(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose)
{
  // did the user give a .xxx with the source file ?
  int nInx = srSourceFile.find_last_of(".");
  if (nInx <= 0)
    throw std::string("source file: ")+srSourceFile+" missing extension";

  // find correct conversion class
  std::string sExtension = srSourceFile.substr(nInx+1);

  // did the user give a .xxx with the source file ?
  if (sExtension == "hrc")        return *(new convert_hrc       (srSourceFile, crMemory, brVerbose));
  if (sExtension == "src")        return *(new convert_src       (srSourceFile, crMemory, brVerbose));
  if (sExtension == "po")         return *(new convert_po        (srSourceFile, crMemory, brVerbose));
  if (sExtension == "tree")       return *(new convert_tree      (srSourceFile, crMemory, brVerbose));
  if (sExtension == "ulf")        return *(new convert_ulf       (srSourceFile, crMemory, brVerbose));
  if (sExtension == "xcu")        return *(new convert_xcu       (srSourceFile, crMemory, brVerbose));
  if (sExtension == "xcs")        return *(new convert_xcs       (srSourceFile, crMemory, brVerbose));
  if (sExtension == "xrm")        return *(new convert_xrm       (srSourceFile, crMemory, brVerbose));
  if (sExtension == "xhp")        return *(new convert_xhp       (srSourceFile, crMemory, brVerbose));
  if (sExtension == "properties") return *(new convert_properties(srSourceFile, crMemory, brVerbose));

  throw std::string("unknown extension on source file: ")+srSourceFile;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_gen::lexRead(char *sBuf, int *nResult, int nMax_size)
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
void convert_gen::lineRead(bool *bEof, std::string& line)
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
void convert_gen::writeSourceFile(const std::string& line)
{
  if (!line.size())
	return;

  std::cout << line;
  // JIX
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_gen::trim(std::string& sLine)
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
