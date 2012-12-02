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
#include <iostream>
#include "gLang.hxx"



/*****************************************************************************
 ************************   G H A N D L E R . C X X   ************************
 *****************************************************************************
 * This is the control module, that interpret the command line and implement
 * the different work types 
 *     extract / merge / generate / insert
 *****************************************************************************/



/**********************   I M P L E M E N T A T I O N   **********************/
handler::handler()
                : meWorkMode(DO_NONE),
                  mbDoNotCopy(false)
{
  // clear translation memory
}



/**********************   I M P L E M E N T A T I O N   **********************/
handler::~handler()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
void handler::checkCommandLine(int argc, char *argv[])
{
  // make internal throw test (to avoid if cascades
  try
  {
    // check for parameter count
    if (argc <= 1)
      throw string("");

    // check for working mode
    string meWorkText(argv[1]);

    if      (meWorkText == "extract")  meWorkMode = DO_EXTRACT;
    else if (meWorkText == "merge")    meWorkMode = DO_MERGE;
    else if (meWorkText == "generate") meWorkMode = DO_GENERATE;
    else if (meWorkText == "insert")   meWorkMode = DO_INSERT;
    else if (meWorkText == "--help")   throw string("");
    else                               throw string("unknown mode (1 argument)");

    // decode parameters and translate to variables
    for (int n = 2; n < argc; ++n)
    {
      string sArg(argv[n]);

      // all -x is followed by a directory
      if (sArg.at(0) == '-')
      {
        if (n == argc)
          throw string("missing directory after ")+sArg;

        // find directory type, and set it
        if      (sArg == "-m") msModuleName = argv[++n];      
        else if (sArg == "-t") msTargetDir  = argv[++n];      
        else if (sArg == "-s") msSourceDir  = argv[++n];      
        else                  throw string("unknown parameter: ")+sArg;
      }
      else
        msSourceFiles.push_back(sArg);
    }

    // check parameters according to function
    switch (meWorkMode)
    {
      case DO_EXTRACT:
           // required parameters
           if (!msModuleName.size())  throw string("missing -m <module name>");
           if (!msTargetDir.size())   throw string("missing -t <target dir>");
           if (!msSourceFiles.size()) throw string("missing source files");
           break;

      case DO_MERGE:
           // required parameters
           if (!msSourceDir.size())   throw string("missing -s <source dir>");
           if (!msTargetDir.size())   throw string("missing -t <target dir>");

           // not allowed parameters
           if (msModuleName.size())   throw string("-m is invalid with merge");
           if (msSourceFiles.size())  throw string("<source> is invalid with merge");
           break;

      case DO_GENERATE:
           // required parameters
           if (!msSourceDir.size())   throw string("missing -s <source dir>");
           if (!msTargetDir.size())   throw string("missing -t <target dir>");

           // not allowed parameters
           if (msModuleName.size())   throw string("-m is invalid with generate");
           if (!msSourceFiles.size()) throw string("<source> is invalid with generate");
           break;

      case DO_INSERT:
           // required parameters
           if (!msModuleName.size())  throw string("missing -m <module name>");
           if (!msSourceDir.size())   throw string("missing -s <source dir>");

           // not allowed parameters
           if (!msSourceFiles.size()) throw string("<source> is invalid with generate");
           break;

      case DO_NONE:
           throw string("unknown mode (1 argument)");
    }
  }
  catch(string sErr)
  {
    // do we have an error text ?
    if (sErr.size())
      cerr << "commandline error:" << sErr << endl;

    // give the correct usage
    cout << "genLang (c)2012 by Apache Software Foundation\n"
            "====================================\n"
            "As part of the L10N framework, genLang extracts and merges translations\n"
            "out of and into the whole source tree.\n\n"
            "Syntax:\n"
            "  genLang extract -m <name> [-s <source dir>] -t <target dir> <sources>\n"
            "  genLang merge   -s <source dir> -t <target dir>\n"
            "  genLang generate -s <source dir> -t <target dir>\n"
            "  genLang insert  -s <source dir> -m <name> [-t <target dir>]\n"
            "  genLang --help\n"
            "Parameters:\n"
            "  extract,  causes a scan of the <sources> for en-US text, and creates\n"
            "            a module file:  <target dir>/<module name>\n"
            "            -s can optionally be used to specify the root of <source>\n"
            "  merge,    causes a merge of the en-US files in <source dir> with the\n"
            "            native language files in <target dir>/*\n"
            "  generate, uses the native language files in <source dir> to build\n"
            "            module files in <target dir> with all language\n"
            "  insert,   uses the module file <source dir>/<module name> to\n"
            "            update the source files in the module"
            "\n"
            "  -m <modulename>   name of the module, used in file naming\n"
            "  -s <source dir>   extract:  source file directory\n"
            "                    merge:    language staging input directory\n"
            "                    generate: native language directory\n"
            "                    insert:   language staging output directory\n"
            "  -t <target dir>   extract:  language staging input directory\n"
            "                    merge:    native language directory\n"
            "                    generate: language output staging directory\n"
            "  <sources>         list of files containing messages to be extracted\n";
    exit(-1);
  }

  // update directories to include final /
  {
    int nLen;

    nLen = msSourceDir.size();
    if (nLen && msSourceDir.at(nLen-1) != '/')
      msSourceDir.append("/");
    nLen = msTargetDir.size();
    if (msTargetDir.at(nLen-1) != '/')
      msTargetDir.append("/");
  }
}



/**********************   I M P L E M E N T A T I O N   **********************/
void handler::run()
{
  try
  {
    // use workMode to start correct control part
    switch (meWorkMode)
    {
      case DO_EXTRACT:  runExtract();  break;
      case DO_MERGE:    runMerge();    break;
      case DO_GENERATE: runGenerate(); break;
      case DO_INSERT:   runInsert();   break;
      case DO_NONE:     throw string("INTERNAL ERROR, checkCommandLine not called!!!");
    }
  }
  catch(string sErr)
  {
    cerr << "runtime error: " << sErr << endl;
    exit(-1);
  }
}



/**********************   I M P L E M E N T A T I O N   **********************/
void handler::runExtract()
{
  // prepare translation memory to module type
  mcMemory.setModuleName(msModuleName);

  // loop through all source files, and extract messages from each file
  for (vector<string>::iterator siSource = msSourceFiles.begin(); siSource != msSourceFiles.end(); ++siSource)
  {
    // prepare translation memory
    mcMemory.setFileName(*siSource);

    // get converter and extract files
    convert_gen::getConverter(msSourceDir + *siSource, mcMemory).extract();
  }

  // and generate language file
  mcMemory.save(msTargetDir + msModuleName);
}



/**********************   I M P L E M E N T A T I O N   **********************/
void handler::runMerge()
{
  throw string("handler::runMerge not implemented");
}



/**********************   I M P L E M E N T A T I O N   **********************/
void handler::runGenerate()
{
  throw string("handler::runGenerate not implemented");
}



/**********************   I M P L E M E N T A T I O N   **********************/
void handler::runInsert()
{
  throw string("handler::runInsert not implemented");
}
