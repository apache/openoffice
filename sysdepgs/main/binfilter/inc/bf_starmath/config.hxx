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


#ifndef CONFIG_HXX
#define CONFIG_HXX

#ifndef _SFXBRDCST_HXX //autogen
#include <bf_svtools/brdcst.hxx>
#endif
#ifndef _SFXLSTNER_HXX //autogen
#include <bf_svtools/lstner.hxx>
#endif
#ifndef _SFXCFGITEM_HXX //autogen
#include <bf_sfx2/cfgitem.hxx>
#endif

#ifndef _FORMAT_HXX
#include "format.hxx"
#endif

#include "cfgitem.hxx"
namespace binfilter {

class SfxItemSet; 

class SmPreferenceDialog;
class SmPrintDialog;
class SmPrintOptionDialog;


class SmConfig : public SmMathConfig, public SfxBroadcaster
{
	SmFontPickList	vFontPickList[7];

public:
	SmConfig();
	virtual ~SmConfig();

	SmFontPickList & GetFontPickList(USHORT nIdent) { return vFontPickList[nIdent]; }
    
	void ConfigToItemSet(SfxItemSet &rSet) const;
};

} //namespace binfilter
#endif


