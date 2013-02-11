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
#ifndef GCON_HXX
#define GCON_HXX
#include "gLang.hxx"



/*****************************************************************************
 ****************************   G C O N . H X X   ****************************
 *****************************************************************************
 * This is the class definition header for all converter classes,
 * all classes and their interrelations is defined here
 *****************************************************************************/



/*******************   G L O B A L   D E F I N I T I O N   *******************/



/********************   C L A S S   D E F I N I T I O N   ********************/
class convert_gen_impl
{
  public:
    static convert_gen_impl *mcImpl;


    convert_gen_impl(l10nMem& crMemory);
	virtual ~convert_gen_impl();

	// all converters MUST implement this function
    virtual void execute() = 0;

	// generic variables
    bool         mbMergeMode;
    std::string  msSourceFile;
    l10nMem&     mcMemory;

	// converter help variables
	bool         mbCollectingData;
    std::string  msCollector;
    std::string  msKey;

	// utility functions for converters
	void lexRead (char *sBuf, int *nResult, int nMax_size);
    void lineRead(bool *bEof, std::string& line);
    void writeSourceFile(const std::string& line);
    void trim(std::string& line);
	void collectData(char *sCollectedText);

  private:
    std::string  msSourceBuffer;
    int          mnSourceReadIndex;

	void prepareFile();


	friend class convert_gen;
};
#endif