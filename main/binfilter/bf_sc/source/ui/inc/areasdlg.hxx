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



#ifndef SC_AREASDLG_HXX
#define SC_AREASDLG_HXX

#ifndef _LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef SC_ANYREFDG_HXX
#include <anyrefdg.hxx>
#endif
namespace binfilter {

class ScDocument;
class ScViewData;
class ScRangeUtil;
class ScRangeItem;


//============================================================================

class ScPrintAreasDlg : public ScAnyRefDlg
{
public:
					ScPrintAreasDlg( SfxBindings* pB, SfxChildWindow* pCW, Window* pParent );
					~ScPrintAreasDlg();

	virtual void	SetReference( const ScRange& rRef, ScDocument* pDoc );
	virtual void	AddRefEntry();

	virtual BOOL	IsTableLocked() const;

	virtual void	SetActive();
	virtual void	Deactivate();
	virtual BOOL	Close();

private:
	ListBox			aLbPrintArea;
    FixedLine       aFlPrintArea;
	ScRefEdit		aEdPrintArea;
	ScRefButton		aRbPrintArea;

	ListBox			aLbRepeatRow;
    FixedLine       aFlRepeatRow;
	ScRefEdit		aEdRepeatRow;
	ScRefButton		aRbRepeatRow;

	ListBox			aLbRepeatCol;
    FixedLine       aFlRepeatCol;
	ScRefEdit		aEdRepeatCol;
	ScRefButton		aRbRepeatCol;

	OKButton		aBtnOk;
	CancelButton	aBtnCancel;
	HelpButton		aBtnHelp;

	BOOL			bDlgLostFocus;
	ScRefEdit*		pRefInputEdit;
	ScDocument*		pDoc;
	ScViewData*		pViewData;
	USHORT			nCurTab;

#ifdef _AREASDLG_CXX
private:
	void Impl_Reset();
	BOOL Impl_CheckRefStrings();
	void Impl_FillLists();
	BOOL Impl_GetItem( Edit* pEd, SfxStringItem& rItem );

	// Handler:
	DECL_LINK( Impl_SelectHdl,		ListBox*    );
	DECL_LINK( Impl_ModifyHdl,		ScRefEdit*  );
	DECL_LINK( Impl_BtnHdl,    		PushButton* );
	DECL_LINK( Impl_GetFocusHdl,	Control*	);
#endif
};



} //namespace binfilter
#endif

