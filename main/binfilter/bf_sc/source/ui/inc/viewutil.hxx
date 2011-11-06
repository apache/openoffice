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



#ifndef SC_VIEWUTIL_HXX
#define SC_VIEWUTIL_HXX

#include <bf_svtools/bf_solar.h>
class String;
namespace binfilter {

class SfxItemSet;
class SvxFontItem;

class ScChangeAction;
class ScChangeViewSettings;
class ScDocument;
class ScAddress;


enum ScUpdateMode { SC_UPDATE_ALL, SC_UPDATE_CHANGED, SC_UPDATE_MARKS };


// ---------------------------------------------------------------------------

class ScViewUtil								// static Methoden
{
public:


	static void PutItemScript( SfxItemSet& rShellSet, const SfxItemSet& rCoreSet,
								USHORT nWhichId, USHORT nScript );


};

// ---------------------------------------------------------------------------





} //namespace binfilter
#endif

