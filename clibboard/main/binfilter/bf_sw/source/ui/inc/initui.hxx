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



#ifndef _INITUI_HXX
#define _INITUI_HXX

#include <bf_svtools/bf_solar.h>
class String; 
namespace binfilter {
class SvStringsDtor; 

/*
 * Forward Declarations
 */

class SwThesaurus;
class SpellCheck;


/*
 * Extern Definitions
 */
extern  SwThesaurus* 	pThes;
extern  String		 	GetSWGVersion();

extern  String* 		pOldGrfCat;
extern  String* 		pOldTabCat;
extern  String* 		pOldFrmCat;

extern  String* 		pCurrGlosGroup;

extern SvStringsDtor* 	pDBNameList;

extern SvStringsDtor* 	pAuthFieldNameList;
extern SvStringsDtor* 	pAuthFieldTypeList;

// stellt die Textbausteinverwaltung zur Verfuegung
class SwGlossaries;
SwGlossaries* GetGlossaries();

class SwGlossaryList;

BOOL HasGlossaryList();
SwGlossaryList* GetGlossaryList();

extern	void _InitUI();
extern	void _FinitUI();
extern  void _InitSpell();
extern  void _FinitSpell();


} //namespace binfilter
#endif
