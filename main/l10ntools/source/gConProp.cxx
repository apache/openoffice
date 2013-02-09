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
 ************************   G C O N P R O P . C X X   ************************
 *****************************************************************************
 * This is the conversion for .properties files
 *****************************************************************************/



/**********************   I M P L E M E N T A T I O N   **********************/
convert_properties::convert_properties(const std::string& srSourceFile, l10nMem& crMemory, bool brVerbose)
                                      : convert_gen(srSourceFile, crMemory, brVerbose)
{
  throw std::string("convert_properties not implemented");
}



/**********************   I M P L E M E N T A T I O N   **********************/
convert_properties::~convert_properties()
{
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_properties::extract()
{
  throw std::string("convert_properties::extract not implemented");
}



/**********************   I M P L E M E N T A T I O N   **********************/
void convert_properties::insert()
{
  throw std::string("convert_properties::insert not implemented");
}

