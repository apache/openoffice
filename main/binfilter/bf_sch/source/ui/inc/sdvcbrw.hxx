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



#ifndef _SDVCBRW_HXX
#define _SDVCBRW_HXX
/*
#ifndef _SVX_VCBRW_HXX //autogen
#include <bf_svx/vcbrw.hxx>
#endif
*/
#ifndef _SFXBRDCST_HXX //autogen
#include <bf_svtools/brdcst.hxx>
#endif

#ifndef _SFXLSTNER_HXX //autogen
#include <bf_svtools/lstner.hxx>
#endif
namespace binfilter {

class SdrView;

/*************************************************************************
|*
|* Klasse fuer Browser
|*
\************************************************************************/
class SdVCBrowser : public VCBrowser //, public SfxListener, public SfxBroadcaster
{
protected:
	SdrView*	pView;
	BOOL		bUpdate;

	BOOL	Close();

	virtual void SFX_NOTIFY( SfxBroadcaster& rBC, const TypeId& rBCType,
						 const SfxHint& rHint, const TypeId& rHintType );
	void		 Modify();


public:
	SdVCBrowser( Window* pParent );

	void Update( SdrView* pView );
};




} //namespace binfilter
#endif		// _SDVCBRW_HXX
