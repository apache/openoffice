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


#ifndef _EXTINPUT_HXX
#define _EXTINPUT_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SVSTDARR_HXX
#define _SVSTDARR_USHORTS
#include <bf_svtools/svstdarr.hxx>
#endif
#ifndef _PAM_HXX
#include <pam.hxx>
#endif
class CommandExtTextInputData; 
class Font; 

namespace binfilter {


class SwExtTextInput : public SwPaM
{
	SvUShorts aAttrs;
	String sOverwriteText;
	BOOL bInsText : 1;
	BOOL bIsOverwriteCursor : 1;
public:
			SwExtTextInput(){};

	const SvUShorts& GetAttrs() const  	{ return aAttrs; }
	BOOL IsInsText() const 				{ return bInsText; }
	void SetInsText( BOOL bFlag ) 		{ bInsText = bFlag; }
	BOOL IsOverwriteCursor() const 		{ return bIsOverwriteCursor; }
};

} //namespace binfilter
#endif	//_EXTINPUT_HXX

