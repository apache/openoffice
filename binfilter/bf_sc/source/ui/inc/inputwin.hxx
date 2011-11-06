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



#ifndef SC_INPUTWIN_HXX
#define SC_INPUTWIN_HXX


#ifndef _TOOLBOX_HXX //autogen
#include <vcl/toolbox.hxx>
#endif
#ifndef _SFX_CHILDWIN_HXX //autogen
#include <bf_sfx2/childwin.hxx>
#endif
#ifndef _SFXLSTNER_HXX //autogen
#include <bf_svtools/lstner.hxx>
#endif
#ifndef _COMBOBOX_HXX //autogen
#include <vcl/combobox.hxx>
#endif
#ifndef _WINDOW_HXX //autogen
#include <vcl/window.hxx>
#endif
#ifndef _TRANSFER_HXX
#include <bf_svtools/transfer.hxx>
#endif
namespace binfilter {

class ScEditEngineDefaulter;
class EditView;
struct ESelection;
class ScInputHandler;
class ScAccessibleEditLineTextData;
struct EENotify;
class ScRangeList;

//========================================================================

class ScTextWnd : public Window, public DragSourceHelper		// edit window
{
public:
					ScTextWnd( Window* pParent );
	virtual			~ScTextWnd();



						// fuer FunktionsAutopiloten






	DECL_LINK( NotifyHdl, EENotify* );

protected:
	virtual void	Resize();




private:
	void			ImplInitSettings();
	void			UpdateAutoCorrFlag();

private:
	String		aString;
	Font		aTextFont;
	ScEditEngineDefaulter*	pEditEngine;			// erst bei Bedarf angelegt
	EditView*	pEditView;
    ScAccessibleEditLineTextData* pAccTextData;
    BOOL		bIsRTL;
	BOOL		bIsInsertMode;
	BOOL		bFormulaMode;

    // #102710#; this flag should be true if a key input or a command is handled
    // it prevents the call of InputChanged in the ModifyHandler of the EditEngine
    BOOL        bInputMode;
};

//========================================================================

class ScPosWnd : public ComboBox, public SfxListener		// Positionsanzeige
{
private:
	String			aPosStr;
	Accelerator*	pAccel;
	BOOL			bFormulaMode;
	BOOL			bTopHadFocus;

public:
					ScPosWnd( Window* pParent );
	virtual			~ScPosWnd();


protected:


	virtual void 	SFX_NOTIFY( SfxBroadcaster& rBC, const TypeId& rBCType,
							const SfxHint& rHint, const TypeId& rHintType );

private:
	void			FillRangeNames();

};

//========================================================================

class ScInputWindow : public ToolBox						// Parent-Toolbox
{
public:
                    ScInputWindow( Window* pParent, SfxBindings* pBind );
	virtual			~ScInputWindow();

	virtual void 	Resize();






	// Fuer FunktionsAutopiloten



	ScInputHandler*	GetInputHandler(){ return pInputHdl;}

	void			StateChanged( StateChangedType nType );


protected:

    sal_Bool        UseSubTotal( ScRangeList* pRangeList ) const;

private:
	ScPosWnd		aWndPos;
	ScTextWnd		aTextWindow;
	ScInputHandler*	pInputHdl;
    SfxBindings*    pBindings;
	String			aTextOk;
	String			aTextCancel;
	String			aTextSum;
	String			aTextEqual;
	BOOL			bIsOkCancelMode;
};

//==================================================================

class ScInputWindowWrapper : public SfxChildWindow
{
public:
			ScInputWindowWrapper( Window*			pParent,
								  USHORT			nId,
								  SfxBindings*		pBindings,
								  SfxChildWinInfo*	pInfo );

	SFX_DECL_CHILDWINDOW(ScInputWindowWrapper);
};


} //namespace binfilter
#endif

