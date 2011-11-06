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


#ifndef _SVX_TBXFORM_HXX
#define _SVX_TBXFORM_HXX

#ifndef _SFXTBXCTRL_HXX //autogen
#include <bf_sfx2/tbxctrl.hxx>
#endif




class FixedText;
namespace binfilter {

//========================================================================


//========================================================================


//========================================================================


//========================================================================
class SvxFmTbxCtlConfig : public SfxToolBoxControl
{
private:
	UINT16 nLastSlot;

public:
	SFX_DECL_TOOLBOX_CONTROL();

	SvxFmTbxCtlConfig( USHORT nId, ToolBox& rTbx, SfxBindings& );
	~SvxFmTbxCtlConfig() {}

	virtual void				StateChanged( USHORT nSID, SfxItemState eState,
											  const SfxPoolItem* pState );
};

//========================================================================
class SvxFmTbxCtlAbsRec : public SfxToolBoxControl
{
public:
	SFX_DECL_TOOLBOX_CONTROL();

	SvxFmTbxCtlAbsRec( USHORT nId, ToolBox& rTbx, SfxBindings& rBindings );


};

//========================================================================
class SvxFmTbxCtlRecText : public SfxToolBoxControl
{
public:
	SFX_DECL_TOOLBOX_CONTROL();

	SvxFmTbxCtlRecText( USHORT nId, ToolBox& rTbx, SfxBindings& rBindings );
};

//========================================================================
class SvxFmTbxCtlRecFromText : public SfxToolBoxControl
{
public:
	SFX_DECL_TOOLBOX_CONTROL();

	SvxFmTbxCtlRecFromText( USHORT nId, ToolBox& rTbx, SfxBindings& rBindings );
};

//========================================================================
class SvxFmTbxCtlRecTotal : public SfxToolBoxControl
{
	FixedText* pFixedText;

public:
	SFX_DECL_TOOLBOX_CONTROL();

	SvxFmTbxCtlRecTotal( USHORT nId, ToolBox& rTbx, SfxBindings& rBindings );
};

/*
//========================================================================
class SvxFmTbxCtlFilterText : public SfxToolBoxControl
{
	FixedText* pFixedText;
	XubString aText;
public:
	SFX_DECL_TOOLBOX_CONTROL();

	SvxFmTbxCtlFilterText( USHORT nId, ToolBox& rTbx, SfxBindings& rBindings );
	~SvxFmTbxCtlFilterText();

	virtual Window*	CreateItemWindow( Window* pParent );
	virtual void	StateChanged( USHORT nSID, SfxItemState eState,
								  const SfxPoolItem* pState );
};
*/

//========================================================================
class SvxFmTbxNextRec : public SfxToolBoxControl
{
public:
	SFX_DECL_TOOLBOX_CONTROL();											 
	SvxFmTbxNextRec( USHORT nId, ToolBox& rTbx, SfxBindings& rBindings );	
};

//========================================================================
class SvxFmTbxPrevRec : public SfxToolBoxControl
{
public:
	SFX_DECL_TOOLBOX_CONTROL();											 
	SvxFmTbxPrevRec( USHORT nId, ToolBox& rTbx, SfxBindings& rBindings );	
};					


}//end of namespace binfilter
#endif

