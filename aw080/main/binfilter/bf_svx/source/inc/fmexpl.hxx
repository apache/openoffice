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


#ifndef _SVX_FMEXPL_HXX
#define _SVX_FMEXPL_HXX







#ifndef _SFXDOCKWIN_HXX //autogen
#include <bf_sfx2/dockwin.hxx>
#endif












#ifndef _SVX_FMVIEW_HXX
#include "fmview.hxx"
#endif


class SdrObjListIter;
namespace binfilter {

class FmFormShell;
class SdrObject;
class FmFormModel;

//========================================================================

//========================================================================

//========================================================================

//========================================================================

//========================================================================

//========================================================================
class FmNavViewMarksChanged : public SfxHint
{
	FmFormView* pView;
public:
	FmNavViewMarksChanged(FmFormView* pWhichView) { pView = pWhichView; }
	virtual ~FmNavViewMarksChanged() {}

};

//========================================================================

//========================================================================


//========================================================================
// FmNavRequestSelectHint - jemand teilt dem NavigatorTree mit, dass er bestimmte Eintraege selektieren soll



//========================================================================


//========================================================================


//========================================================================
//............................................................................
namespace svxform
{
//............................................................................

	//========================================================================
	// class OFormComponentObserver
	//========================================================================

	//========================================================================
	//= NavigatorTreeModel
	//========================================================================

	//========================================================================


	//========================================================================

	//========================================================================
	class NavigatorFrameManager : public SfxChildWindow
	{
	public:
		NavigatorFrameManager( Window *pParent, sal_uInt16 nId, SfxBindings *pBindings,
						  SfxChildWinInfo *pInfo );
		SFX_DECL_CHILDWINDOW( NavigatorFrameManager );
	};

//............................................................................
}	// namespace svxform
//............................................................................

}//end of namespace binfilter
#endif // _SVX_FMEXPL_HXX

