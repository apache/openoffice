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
 ***************************   G L A N G . C X X   ***************************
 *****************************************************************************
 * This is the main of the l10n localizer program, it is C based and call
 * down to classes for handling.
 *****************************************************************************/




/**********************   I M P L E M E N T A T I O N   **********************/
#if defined(UNX) || defined(OS2)
int main(int argc, char *argv[])
#else
int _cdecl main(int argc, char *argv[])
#endif
{
  handler cHandler;

  // check command line (exit if problems)
  cHandler.checkCommandLine(argc, argv);

  // command line is ok, so execute it
  cHandler.run();
}
