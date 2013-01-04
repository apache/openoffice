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


#ifndef _TMPLCTRL_HXX
#define _TMPLCTRL_HXX

// include ---------------------------------------------------------------

#ifndef _SFXSTBITEM_HXX //autogen
#include <bf_sfx2/stbitem.hxx>
#endif

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
namespace binfilter {


// class BookmarkToolBoxControl -------------------------------------------

class SwTemplateControl : public SfxStatusBarControl
{
protected:
public:

	SFX_DECL_STATUSBAR_CONTROL();

	SwTemplateControl( USHORT nId, StatusBar& rStb, SfxBindings& rBind );
	~SwTemplateControl();

private:
	String  sTemplate;
};


} //namespace binfilter
#endif

