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



#ifndef SC_TPHFEDIT_HXX
#define SC_TPHFEDIT_HXX

#ifndef _SVX_PAGEITEM_HXX //autogen
#include <bf_svx/pageitem.hxx>
#endif

#ifndef _STDCTRL_HXX //autogen
#include <bf_svtools/stdctrl.hxx>
#endif

#ifndef _SV_GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif

#ifndef SC_ITEMS_HXX
#include "scitems.hxx"          // wegen enum SvxNumType
#endif

#ifndef _SV_TIMER_HXX //autogen
#include <vcl/timer.hxx>
#endif

#ifndef _SV_VIRDEV_HXX //autogen
#include <vcl/virdev.hxx>
#endif

#ifndef SC_POPMENU_HXX
#include <popmenu.hxx>
#endif

#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLE_HPP_
#include <com/sun/star/accessibility/XAccessible.hpp>
#endif
#ifndef _CPPUHELPER_WEAKREF_HXX_ 
#include <cppuhelper/weakref.hxx>
#endif
namespace binfilter {

//===================================================================

class ScHeaderEditEngine;
class ScPatternAttr;
class EditView;
class EditTextObject;
class SvxFieldItem;
class ScAccessibleEditObject;

enum ScEditWindowLocation
{
    Left,
    Center,
    Right
};

class ScEditWindow : public Control
{
public:
			ScEditWindow( Window* pParent, const ResId& rResId, ScEditWindowLocation eLoc );
			~ScEditWindow();

	void 			SetFont( const ScPatternAttr& rPattern );
	void 			SetText( const EditTextObject& rTextObject );
	EditTextObject*	CreateTextObject();
	void			SetCharAttriutes();

	void			InsertField( const SvxFieldItem& rFld );

	void			SetNumType(SvxNumType eNumType);

	virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible > CreateAccessible();

protected:
	virtual void	Paint( const Rectangle& rRec );
	virtual void	MouseMove( const MouseEvent& rMEvt );
	virtual void	MouseButtonDown( const MouseEvent& rMEvt );
	virtual void	MouseButtonUp( const MouseEvent& rMEvt );
	virtual void	KeyInput( const KeyEvent& rKEvt );
	virtual void	Command( const CommandEvent& rCEvt );
	virtual void	GetFocus();
    virtual void    LoseFocus();

private:
	ScHeaderEditEngine*	pEdEngine;
	EditView*			pEdView;
    ScEditWindowLocation eLocation;

    ::com::sun::star::uno::WeakReference< ::com::sun::star::accessibility::XAccessible > xAcc;
    ScAccessibleEditObject* pAcc;
};

//===================================================================
class ScExtIButton : public ImageButton
{
private:

	Timer			aTimer;
	ScPopupMenu*	pPopupMenu;
	Link			aMLink;
	USHORT			nSelected;

	DECL_LINK( TimerHdl, Timer*);

//	void			DrawArrow();

protected:

	virtual void	MouseButtonDown( const MouseEvent& rMEvt );
	virtual void	MouseButtonUp( const MouseEvent& rMEvt);
	virtual void	Click();

	virtual void	StartPopup();

public:

	ScExtIButton(Window* pParent, const ResId& rResId );

	void			SetPopupMenu(ScPopupMenu* pPopUp);

	USHORT			GetSelected();

	void            SetMenuHdl( const Link& rLink ) { aMLink = rLink; }
	const Link&     GetMenuHdl() const { return aMLink; }

	virtual long	PreNotify( NotifyEvent& rNEvt );
};


//===================================================================

class ScHFEditPage : public SfxTabPage
{
public:
	virtual	BOOL	FillItemSet	( SfxItemSet& rCoreSet );
	virtual	void	Reset		( const SfxItemSet& rCoreSet );

	void			SetNumType(SvxNumType eNumType);

protected:
				ScHFEditPage( Window*			pParent,
							  USHORT			nResId,
							  const SfxItemSet&	rCoreSet,
							  USHORT			nWhich );
	virtual		~ScHFEditPage();

private:
	FixedText		aFtLeft;
	ScEditWindow	aWndLeft;
	FixedText		aFtCenter;
	ScEditWindow	aWndCenter;
	FixedText		aFtRight;
	ScEditWindow	aWndRight;
	ImageButton		aBtnText;
	ScExtIButton	aBtnFile;
	ImageButton		aBtnTable;
	ImageButton		aBtnPage;
	ImageButton		aBtnLastPage;
	ImageButton		aBtnDate;
	ImageButton		aBtnTime;
	FixedLine		aFlInfo;
	FixedInfo		aFtInfo;
	ScPopupMenu		aPopUpFile;

	USHORT			nWhich;
	String			aCmdArr[6];

private:
#ifdef _TPHFEDIT_CXX
	void FillCmdArr();
	DECL_LINK( ClickHdl, ImageButton* );
	DECL_LINK( MenuHdl, ScExtIButton* );
#endif
};

//===================================================================

class ScRightHeaderEditPage : public ScHFEditPage
{
public:
	static SfxTabPage*	Create( Window* pParent, const SfxItemSet& rCoreSet );
	static USHORT*		GetRanges();

private:
	ScRightHeaderEditPage( Window* pParent, const SfxItemSet& rSet );
};

//===================================================================

class ScLeftHeaderEditPage : public ScHFEditPage
{
public:
	static SfxTabPage*	Create( Window* pParent, const SfxItemSet& rCoreSet );
	static USHORT*		GetRanges();

private:
	ScLeftHeaderEditPage( Window* pParent, const SfxItemSet& rSet );
};

//===================================================================

class ScRightFooterEditPage : public ScHFEditPage
{
public:
	static SfxTabPage*	Create( Window* pParent, const SfxItemSet& rCoreSet );
	static USHORT*		GetRanges();

private:
	ScRightFooterEditPage( Window* pParent, const SfxItemSet& rSet );
};

//===================================================================

class ScLeftFooterEditPage : public ScHFEditPage
{
public:
	static SfxTabPage*	Create( Window* pParent, const SfxItemSet& rCoreSet );
	static USHORT*		GetRanges();

private:
	ScLeftFooterEditPage( Window* pParent, const SfxItemSet& rSet );
};



} //namespace binfilter
#endif // SC_TPHFEDIT_HXX

