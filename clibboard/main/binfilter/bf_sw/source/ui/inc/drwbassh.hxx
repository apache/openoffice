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


#ifndef _SWDRWBASSH_HXX
#define _SWDRWBASSH_HXX

#include "basesh.hxx"
namespace binfilter {

class SfxItemSet; 
class SwWrtShell;
class SwView;

class SwDrawBase;
class SvxNameDialog;

class SwDrawBaseShell: public SwBaseShell
{

public:
	SwDrawBaseShell(SwView &rShell):SwBaseShell( rShell ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 SwDrawBaseShell(SwView &rShell);

	SFX_DECL_INTERFACE(SW_DRAWBASESHELL);

	void		DisableState(SfxItemSet &rSet){DBG_BF_ASSERT(0, "STRIP");}; //STRIP001 void		DisableState(SfxItemSet &rSet)				 { Disable(rSet);}


};


} //namespace binfilter
#endif
