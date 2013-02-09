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
#include "gConXrm.hxx"



/*****************************************************************************
 *************************   G C O N X R M . C X X   *************************
 *****************************************************************************
 * This is the conversion for .xrm files
 *****************************************************************************/



/*****************************   G L O B A L S   *****************************/
convert_xrm_impl * convert_xrm::mcpImpl;



/************   I N T E R F A C E   I M P L E M E N T A T I O N   ************/
convert_xrm::convert_xrm(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose)
                        : convert_gen(srSourceFile, crMemory, brVerbose) 
                          {mcpImpl = new convert_xrm_impl(srSourceFile, crMemory, brVerbose);}
convert_xrm::~convert_xrm() {delete mcpImpl;}
void convert_xrm::extract() {mcpImpl->extract();}
void convert_xrm::insert()  {mcpImpl->insert();}



/**********************   I M P L E M E N T A T I O N   **********************/
convert_xrm_impl::convert_xrm_impl(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose)
                                  : convert_gen (srSourceFile, crMemory, brVerbose),
								    mbCollectingData(false)
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
convert_xrm_impl::~convert_xrm_impl()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_xrm_impl::extract()
{
  // generate l10mMem
  mbMergeMode = false;

  // run lex parser and build token tree
  runLex();
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_xrm_impl::insert()
{
  // generate source files.
  mbMergeMode = true;

  // run lex parser and build token tree
  runLex();
}


