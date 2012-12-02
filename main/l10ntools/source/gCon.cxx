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
 ****************************   G C O N . C X X   ****************************
 *****************************************************************************
 * This is the generic conversion module, it handles all generic work of the
 * conversion, and offer utility functions to the specific conversion classes
 *****************************************************************************/



/**********************   I M P L E M E N T A T I O N   **********************/
convert_gen::convert_gen(const string& srSourceFile, l10nMem& crMemory)
                        : msSourceFile(srSourceFile),
                          mcMemory(crMemory)
{
  ifstream inputFile(msSourceFile.c_str(), ios::binary);

  
  if (!inputFile.is_open())
    throw string("Could not open ")+msSourceFile;

  // get length of file:
  mnSourceReadIndex = 0;
  inputFile.seekg (0, ios::end);
  msSourceBuffer.resize((unsigned int)inputFile.tellg());
  inputFile.seekg (0, ios::beg);

  // get size, prepare string and read whole file
  inputFile.read((char *)msSourceBuffer.c_str(), msSourceBuffer.size());
  if ((unsigned int)inputFile.gcount() != msSourceBuffer.size())
    throw string("cannot read whole file: "+msSourceFile);
  inputFile.close();
}



/**********************   I M P L E M E N T A T I O N   **********************/
convert_gen::~convert_gen()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
convert_gen& convert_gen::getConverter(const string& srSourceFile, l10nMem& crMemory)
{
  // did the user give a .xxx with the source file ?
  int nInx = srSourceFile.find_last_of(".");
  if (nInx <= 0)
    throw string("source file: ")+srSourceFile+" missing extension";

  // find correct conversion class
  string sExtension = srSourceFile.substr(nInx+1);

  // did the user give a .xxx with the source file ?
  if (sExtension == "hrc" || sExtension == "src") return *(new convert_src (srSourceFile, crMemory));
  if (sExtension == "tree")                       return *(new convert_tree(srSourceFile, crMemory));
  if (sExtension == "ulf")                        return *(new convert_ulf (srSourceFile, crMemory));
  if (sExtension == "xcu")                        return *(new convert_xcu (srSourceFile, crMemory));
  if (sExtension == "xcs")                        return *(new convert_xcs (srSourceFile, crMemory));
  if (sExtension == "xrm")                        return *(new convert_xrm (srSourceFile, crMemory));
  if (sExtension == "xhp")                        return *(new convert_xhp (srSourceFile, crMemory));
  if (sExtension == "properties")                 return *(new convert_properties(srSourceFile, crMemory));

  throw string("unknown extension on source file: ")+srSourceFile;
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
void convert_gen::lineRead(bool *bEof, string& line)
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
  if (nNextLF == msSourceBuffer.npos)
    nNextLF = msSourceBuffer.size()+1;

  // copy string
  line              = msSourceBuffer.substr(mnSourceReadIndex, nNextLF - mnSourceReadIndex-1);
  mnSourceReadIndex = nNextLF +1;
  *bEof             = false;
  return;
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_gen::writeSourceFile(const string& line)
{
  cout << line;
  // JIX
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_gen::trim(string& sLine)
{
  int nL;

  // remove leading spaces
  nL = sLine.find_first_not_of(" \t");
  if (nL != string::npos)
    sLine.erase(0, nL);

  // remove trailing spaces
  nL = sLine.find_last_not_of(" \t");
  if (nL != string::npos)
    sLine.erase(nL +1);
}
