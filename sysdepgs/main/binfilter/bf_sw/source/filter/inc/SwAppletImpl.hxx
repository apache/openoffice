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



#ifndef _SW_APPLET_IMPL_HXX
#define _SW_APPLET_IMPL_HXX

#include <bf_svtools/bf_solar.h>

#define SWHTML_OPTTYPE_IGNORE 0
#define SWHTML_OPTTYPE_TAG 1
#define SWHTML_OPTTYPE_PARAM 2
#define SWHTML_OPTTYPE_SIZE 3

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif

#ifndef _HTMLKYWD_HXX
#include <bf_svtools/htmlkywd.hxx>
#endif
#ifndef _FRAMEOBJ_HXX //autogen
#include <bf_sfx2/frameobj.hxx>
#endif
#ifndef _WRKWIN_HXX //autogen
#include <vcl/wrkwin.hxx>
#endif
#ifndef _SVSTOR_HXX //autogen
#include <bf_so3/svstor.hxx>
#endif
#ifndef _APPLET_HXX //autogen
#include <bf_so3/applet.hxx>
#endif
#ifndef _PLUGIN_HXX //autogen
#include <bf_so3/plugin.hxx>
#endif
#ifndef _SFXITEMSET_HXX
#include <bf_svtools/itemset.hxx>
#endif
namespace binfilter {

class SfxItemSet; 
extern sal_Char const SVTOOLS_CONSTASCII_DECL( sHTML_O_hidden, "HIDDEN" );
extern sal_Char const SVTOOLS_CONSTASCII_DECL( sHTML_HIDDEN_false, "FALSE" );
extern sal_Char const SVTOOLS_CONSTASCII_DECL( sHTML_O_archive, "ARCHIVE" );
extern sal_Char const SVTOOLS_CONSTASCII_DECL( sHTML_O_archives, "ARCHIVES" );
extern sal_Char const SVTOOLS_CONSTASCII_DECL( sHTML_O_object, "OBJECT" );

class SwApplet_Impl
{
	SvAppletObjectRef xApplet;		// das aktuelle Applet
	SfxItemSet 		  aItemSet;

public:
	static USHORT GetOptionType( const String& rName, BOOL bApplet ){DBG_BF_ASSERT(0, "STRIP"); return 0;} //STRIP001 static USHORT GetOptionType( const String& rName, BOOL bApplet );
	SwApplet_Impl( SfxItemSet& rSet ): aItemSet ( rSet) {}
	~SwApplet_Impl(){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 ~SwApplet_Impl();

	void CreateApplet( const String& rCode, const String& rName,//STRIP001 	void CreateApplet( const String& rCode, const String& rName,
		BOOL bMayScript, const String& rCodeBase ){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 //STRIP001 					   BOOL bMayScript, const String& rCodeBase );
		sal_Bool CreateApplet();
	SvAppletObject* GetApplet() { return &xApplet; }
	SfxItemSet& GetItemSet() { return aItemSet; }
};
} //namespace binfilter
#endif
