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


#ifndef _SFXTABDLG_HXX
#define _SFXTABDLG_HXX

#include "sal/config.h"
#include "sfx2/dllapi.h"
#include "sal/types.h"
#include <vcl/tabdlg.hxx>
#include <vcl/button.hxx>
#include <vcl/tabctrl.hxx>
#include <vcl/tabpage.hxx>
#include <svl/itempool.hxx>
#include <svl/itemset.hxx>
#include <com/sun/star/frame/XFrame.hpp>

#if ENABLE_LAYOUT
#include <layout/layout.hxx>
namespace layout { class SfxTabDialog; class SfxTabDialogController; }
#endif /* ENABLE_LAYOUT */

class SfxPoolItem;
class SfxTabDialog;
class SfxViewFrame;
class SfxTabPage;
class SfxBindings;

#ifndef ENABLE_LAYOUT_SFX_TABDIALOG
#define ENABLE_LAYOUT_SFX_TABDIALOG 0
#define NAMESPACE_LAYOUT_SFX_TABDIALOG
#define END_NAMESPACE_LAYOUT_SFX_TABDIALOG
#define LAYOUT_NS_SFX_TABDIALOG
#endif /* !ENABLE_LAYOUT_SFX_TABDIALOG*/

typedef SfxTabPage* (*CreateTabPage)(Window *pParent, const SfxItemSet &rAttrSet);
typedef sal_uInt16*		(*GetTabPageRanges)(); // provide international Which-Value
struct TabPageImpl;
class SfxUs_Impl;

#if ENABLE_LAYOUT_SFX_TABDIALOG
#include <layout/layout-pre.hxx>
#undef SfxTabDialog
#undef SfxTabPage
#endif /* ENABLE_LAYOUT_SFX_TABDIALOG */

NAMESPACE_LAYOUT_SFX_TABDIALOG

struct TabDlg_Impl;

#define ID_TABCONTROL	1
#define RET_USER 		100
#define RET_USER_CANCEL	101

class SFX2_DLLPUBLIC SfxTabDialogItem: public SfxSetItem
{
public:
	TYPEINFO();
                            SfxTabDialogItem( sal_uInt16 nId, const SfxItemSet& rItemSet );
                            SfxTabDialogItem(const SfxTabDialogItem& rAttr, SfxItemPool* pItemPool=NULL);
	virtual SfxPoolItem*    Clone(SfxItemPool* pToPool) const;
	virtual SfxPoolItem*    Create(SvStream& rStream, sal_uInt16 nVersion) const;
};

class SFX2_DLLPUBLIC SfxTabDialog : public TabDialog
{
private:
friend class SfxTabPage;
friend class SfxTabDialogController;

	SfxViewFrame*	pFrame;

	TabControl		aTabCtrl;
	OKButton		aOKBtn;
	PushButton*		pUserBtn;
	CancelButton	aCancelBtn;
	HelpButton		aHelpBtn;
	PushButton		aResetBtn;
	PushButton		aBaseFmtBtn;

	const SfxItemSet*	pSet;
	SfxItemSet*			pOutSet;
	TabDlg_Impl*		pImpl;
	sal_uInt16*				pRanges;
	sal_uInt32			nResId;
	sal_uInt16				nAppPageId;
	sal_Bool				bItemsReset;
	sal_Bool				bFmt;

//#if 0 // _SOLAR__PRIVATE
	DECL_DLLPRIVATE_LINK( ActivatePageHdl, TabControl * );
	DECL_DLLPRIVATE_LINK( DeactivatePageHdl, TabControl * );
	DECL_DLLPRIVATE_LINK( OkHdl, Button * );
	DECL_DLLPRIVATE_LINK( ResetHdl, Button * );
	DECL_DLLPRIVATE_LINK( BaseFmtHdl, Button * );
	DECL_DLLPRIVATE_LINK( UserHdl, Button * );
	DECL_DLLPRIVATE_LINK( CancelHdl, Button * );
	SAL_DLLPRIVATE void Init_Impl(sal_Bool, const String *);
//#endif

protected:
	virtual short				Ok();
	// be deleted in Sfx !
	virtual SfxItemSet*			CreateInputItemSet( sal_uInt16 nId );
	// don't be deleted in Sfx!
	virtual const SfxItemSet*	GetRefreshedSet();
	virtual void				PageCreated( sal_uInt16 nId, SfxTabPage &rPage );
    virtual long                Notify( NotifyEvent& rNEvt );

	SfxItemSet*		pExampleSet;
	SfxItemSet*		GetInputSetImpl();
	SfxTabPage*		GetTabPage( sal_uInt16 nPageId ) const;

	sal_Bool			IsInOK() const;
	/**	prepare to leace the current page. Calls the DeactivatePage method of the current page, (if necessary),
		handles the item sets to copy.
		@return sal_True if it is allowed to leave the current page, sal_False otherwise
	*/
	bool PrepareLeaveCurrentPage();

public:
    SfxTabDialog( Window* pParent, const ResId &rResId, sal_uInt16 nSetId, SfxBindings& rBindings,
				  sal_Bool bEditFmt = sal_False, const String *pUserButtonText = 0 );
	SfxTabDialog( Window* pParent, const ResId &rResId, const SfxItemSet * = 0,
				  sal_Bool bEditFmt = sal_False, const String *pUserButtonText = 0 );
	SfxTabDialog( SfxViewFrame *pViewFrame, Window* pParent, const ResId &rResId,
				  const SfxItemSet * = 0, sal_Bool bEditFmt = sal_False,
				  const String *pUserButtonText = 0 );
	~SfxTabDialog();

	void				AddTabPage( sal_uInt16 nId,
									CreateTabPage pCreateFunc,		// != 0
									GetTabPageRanges pRangesFunc,	// can be 0
									sal_Bool bItemsOnDemand = sal_False);
	void				AddTabPage( sal_uInt16 nId,
									const String &rRiderText,
									CreateTabPage pCreateFunc,		// != 0
									GetTabPageRanges pRangesFunc,	// can be 0
									sal_Bool bItemsOnDemand = sal_False,
									sal_uInt16 nPos = TAB_APPEND);
	void				AddTabPage( sal_uInt16 nId,
									const Bitmap &rRiderBitmap,
									CreateTabPage pCreateFunc,		// != 0
									GetTabPageRanges pRangesFunc,	// can be 0
									sal_Bool bItemsOnDemand = sal_False,
									sal_uInt16 nPos = TAB_APPEND);

	void				AddTabPage( sal_uInt16 nId,
									sal_Bool bItemsOnDemand = sal_False);
	void				AddTabPage( sal_uInt16 nId,
									const String &rRiderText,
									sal_Bool bItemsOnDemand = sal_False,
									sal_uInt16 nPos = TAB_APPEND);
	void				AddTabPage( sal_uInt16 nId,
									const Bitmap &rRiderBitmap,
									sal_Bool bItemsOnDemand = sal_False,
									sal_uInt16 nPos = TAB_APPEND);

	void				RemoveTabPage( sal_uInt16 nId );

	void				SetCurPageId( sal_uInt16 nId ) { nAppPageId = nId; }
	sal_uInt16				GetCurPageId() const
							{ return aTabCtrl.GetCurPageId(); }
	void				ShowPage( sal_uInt16 nId );

						// liefert ggf. per Map konvertierte lokale Slots
	const sal_uInt16*		GetInputRanges( const SfxItemPool& );
	void				SetInputSet( const SfxItemSet* pInSet );
	const SfxItemSet*	GetOutputItemSet() const { return pOutSet; }
	const SfxItemSet*	GetOutputItemSet( sal_uInt16 nId ) const;
	int					FillOutputItemSet();
	sal_Bool IsFormat() const { return bFmt; }

	const OKButton& 	GetOKButton() const { return aOKBtn; }
	OKButton&			GetOKButton() { return aOKBtn; }
	const CancelButton& GetCancelButton() const { return aCancelBtn; }
	CancelButton&		GetCancelButton() { return aCancelBtn; }
	const HelpButton&	GetHelpButton() const { return aHelpBtn; }
	HelpButton&			GetHelpButton() { return aHelpBtn; }
	const PushButton& 	GetResetButton() const { return aResetBtn; }
	PushButton&			GetResetButton() { return aResetBtn; }

	const PushButton*	GetApplyButton() const;
	PushButton*			GetApplyButton();

	const PushButton*	GetUserButton() const { return pUserBtn; }
	PushButton*			GetUserButton() { return pUserBtn; }
	void				RemoveResetButton();

	short				Execute();
    void                StartExecuteModal( const Link& rEndDialogHdl );
	void				Start( sal_Bool bShow = sal_True );

#if !ENABLE_LAYOUT_SFX_TABDIALOG
	const SfxItemSet*	GetExampleSet() const { return pExampleSet; }
#else /* ENABLE_LAYOUT_SFX_TABDIALOG */
    SfxItemSet*	GetExampleSet() const { return 0; }
#endif /* ENABLE_LAYOUT_SFX_TABDIALOG */
	SfxViewFrame*		GetViewFrame() const { return pFrame; }

	void				EnableApplyButton(sal_Bool bEnable = sal_True);
	sal_Bool				IsApplyButtonEnabled() const;
	void				SetApplyHandler(const Link& _rHdl);
	Link				GetApplyHandler() const;

//#if 0 // _SOLAR__PRIVATE
	SAL_DLLPRIVATE void Start_Impl();
	SAL_DLLPRIVATE sal_Bool OK_Impl() { return PrepareLeaveCurrentPage(); }
//#endif
};

END_NAMESPACE_LAYOUT_SFX_TABDIALOG

#if ENABLE_LAYOUT_SFX_TABDIALOG
#include <layout/layout-post.hxx>
#endif /* ENABLE_LAYOUT_SFX_TABDIALOG */

#if !ENABLE_LAYOUT_SFX_TABDIALOG

namespace sfx { class ItemConnectionBase; }

class SFX2_DLLPUBLIC SfxTabPage: public TabPage
{
friend class SfxTabDialog;
    #if ENABLE_LAYOUT
     friend class layout::SfxTabDialog;
    #endif

private:
	const SfxItemSet*	pSet;
	String				aUserString;
	sal_Bool				bHasExchangeSupport;
	SfxTabDialog*		pTabDlg;
	TabPageImpl*		pImpl;

	SAL_DLLPRIVATE void SetTabDialog( SfxTabDialog* pNew ) { pTabDlg = pNew; }
	SAL_DLLPRIVATE void SetInputSet( const SfxItemSet* pNew ) { pSet = pNew; }

protected:
	SfxTabPage( Window *pParent, const ResId &, const SfxItemSet &rAttrSet );
	SfxTabPage( Window *pParent, WinBits nStyle, const SfxItemSet &rAttrSet );

	sal_uInt16				GetSlot( sal_uInt16 nWhich ) const
							{ return pSet->GetPool()->GetSlotId( nWhich ); }
	sal_uInt16				GetWhich( sal_uInt16 nSlot, sal_Bool bDeep = sal_True ) const
							{ return pSet->GetPool()->GetWhich( nSlot, bDeep ); }
	const SfxPoolItem* 	GetOldItem( const SfxItemSet& rSet, sal_uInt16 nSlot, sal_Bool bDeep = sal_True );
	const SfxPoolItem* 	GetExchangeItem( const SfxItemSet& rSet, sal_uInt16 nSlot );
	SfxTabDialog*		GetTabDialog() const { return pTabDlg; }

    void                AddItemConnection( sfx::ItemConnectionBase* pConnection );

public:
	virtual             ~SfxTabPage();

	const SfxItemSet&	GetItemSet() const { return *pSet; }

    virtual sal_Bool        FillItemSet( SfxItemSet& );
    virtual void        Reset( const SfxItemSet& );

	sal_Bool				HasExchangeSupport() const
							{ return bHasExchangeSupport; }
	void				SetExchangeSupport( sal_Bool bNew = sal_True )
							{ bHasExchangeSupport = bNew; }

	enum sfxpg {
		KEEP_PAGE = 0x0000,		// Error handling; do not change page
			// 2. Filling an ItemSet for updating higher-level examples;
			// this pointer can always
			// be NULL!!
		LEAVE_PAGE = 0x0001,
			// Update set and other page
		REFRESH_SET = 0x0002
	};

        using TabPage::ActivatePage;
        using TabPage::DeactivatePage;
	virtual void            ActivatePage( const SfxItemSet& );
	virtual int             DeactivatePage( SfxItemSet* pSet = 0 );
	void                    SetUserData(const String& rString)
							{ aUserString = rString; }
	String				GetUserData() { return aUserString; }
	virtual void		FillUserData();
	virtual sal_Bool		IsReadOnly() const;
	virtual void PageCreated (SfxAllItemSet aSet); //add CHINA001
	static const SfxPoolItem* GetItem( const SfxItemSet& rSet, sal_uInt16 nSlot, sal_Bool bDeep = sal_True );

    void SetFrame(const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& xFrame);
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > GetFrame();
};

#endif /* !ENABLE_LAYOUT_SFX_TABDIALOG */

#endif

