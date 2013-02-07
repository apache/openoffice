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
 ************************   G C O N T R E E . C X X   ************************
 *****************************************************************************
 * This is the conversion for .tree files
 *****************************************************************************/



/**********************   I M P L E M E N T A T I O N   **********************/
convert_tree::convert_tree(const std::string& srSourceFile, l10nMem& crMemory)
                          : convert_gen(srSourceFile, crMemory)
{
// extension,     program,     parameter,          collectMode, xxx
// ".tree",       "xhtex",     "",                 "negative",  "noiso"
  throw std::string("convert_tree not implemented");
}



/**********************   I M P L E M E N T A T I O N   **********************/
convert_tree::~convert_tree()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_tree::extract()
{
  throw std::string("convert_tree::extract not implemented");
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_tree::insert()
{
  throw std::string("convert_tree::insert not implemented");
}
