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


#ifndef _SWBLOCKS_HXX
#define _SWBLOCKS_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _PERSIST_HXX //autogen
#include <bf_so3/persist.hxx>
#endif
/*N*/#include <tools/debug.hxx> //addded for stripping 
class SvxMacroTableDtor; 
namespace binfilter {

class SfxMedium;
class SwPaM;
class SwDoc;

class SvXMLTextBlocks;
// Name eines Textblocks:




class SwImpBlocks
{
	friend class SwTextBlocks;
protected:
	String aFile;						// physikalischer Dateiname
	String aName;						// logischer Name der Datei
	SwImpBlocks( const String&, BOOL = FALSE ){DBG_ASSERT(0, "STRIP")}; //STRIP001 ;
public:
	const String& GetFileName() const {return aFile;} 	// phys. Dateinamen liefern
};




class Sw3Io;
class Sw3IoImp;

class Sw3TextBlocks : public SwImpBlocks
{

public:
	Sw3TextBlocks( const String& rFile): SwImpBlocks( rFile ){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	Sw3TextBlocks( const String& );



	// Methoden fuer die neue Autokorrektur



};

} //namespace binfilter
#endif
