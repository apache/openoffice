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



#ifndef SC_CONTENT_HXX
#define SC_CONTENT_HXX

#include <svtools/svtreebx.hxx>
#include "global.hxx"
#include "address.hxx"
#include <tools/solar.h>

class ScNavigatorDlg;
class ScNavigatorSettings;
class ScDocument;
class ScDocShell;
class ScAreaLink;
class SdrPage;

#define SC_CONTENT_ROOT			0
#define SC_CONTENT_TABLE		1
#define SC_CONTENT_RANGENAME	2
#define SC_CONTENT_DBAREA		3
#define SC_CONTENT_GRAPHIC		4
#define SC_CONTENT_OLEOBJECT	5
#define SC_CONTENT_NOTE			6
#define SC_CONTENT_AREALINK		7
#define SC_CONTENT_DRAWING		8
#define SC_CONTENT_COUNT		9

const sal_uLong SC_CONTENT_NOCHILD = ~0UL;

//
//	TreeListBox for contents
//

class ScContentTree : public SvTreeListBox
{
	ScNavigatorDlg*		pParentWindow;
	ImageList			aEntryImages;
	ImageList			aHCEntryImages;
	SvLBoxEntry*		pRootNodes[SC_CONTENT_COUNT];
	sal_uInt16			nRootType;			// als Root eingestellt
	String				aManualDoc;			// im Navigator umgeschaltet (Title)
	sal_Bool			bHiddenDoc;			// verstecktes aktiv?
	String				aHiddenName;		// URL zum Laden
	String				aHiddenTitle;		// fuer Anzeige
	ScDocument*			pHiddenDocument;	// temporary
	sal_Bool			bisInNavigatorDlg;
	String 				sKeyString;

	sal_uInt16				pPosList[SC_CONTENT_COUNT];		// fuer die Reihenfolge

	static sal_Bool	bIsInDrag;		// static, falls der Navigator im ExecuteDrag geloescht wird

	ScDocShell* GetManualOrCurrent();

	void	InitRoot(sal_uInt16 nType);
	void	ClearType(sal_uInt16 nType);
	void	ClearAll();
	void	InsertContent( sal_uInt16 nType, const String& rValue );
	void	GetDrawNames( sal_uInt16 nType );

	void	GetTableNames();
	void	GetAreaNames();
	void	GetDbNames();
	void	GetLinkNames();
	void	GetGraphicNames();
	void	GetOleNames();
	void	GetDrawingNames();
	void	GetNoteStrings();

	static bool IsPartOfType( sal_uInt16 nContentType, sal_uInt16 nObjIdentifier );

	sal_Bool	DrawNamesChanged( sal_uInt16 nType );
	sal_Bool	NoteStringsChanged();

	ScAddress GetNotePos( sal_uLong nIndex );
	const ScAreaLink* GetLink( sal_uLong nIndex );

    /** Returns the indexes of the specified listbox entry.
        @param rnRootIndex  Root index of specified entry is returned.
        @param rnChildIndex  Index of the entry inside its root is returned (or SC_CONTENT_NOCHILD if entry is root).
        @param pEntry  The entry to examine. */
    void    GetEntryIndexes( sal_uInt16& rnRootIndex, sal_uLong& rnChildIndex, SvLBoxEntry* pEntry ) const;

    /** Returns the child index of the specified listbox entry.
        @param pEntry  The entry to examine or NULL for the selected entry.
        @return  Index of the entry inside its root or SC_CONTENT_NOCHILD if entry is root. */
    sal_uLong   GetChildIndex( SvLBoxEntry* pEntry ) const;

	void	DoDrag();
//UNUSED2008-05  void	AdjustTitle();

	ScDocument* GetSourceDocument();

	DECL_LINK( ContentDoubleClickHdl, ScContentTree* );
	DECL_STATIC_LINK( ScContentTree, ExecDragHdl, void* );
public:
	SvLBoxEntry* pTmpEntry;

	bool m_bFirstPaint;
protected:
//	virtual sal_Bool	Drop( const DropEvent& rEvt );
//	virtual sal_Bool	QueryDrop( DropEvent& rEvt );

	using SvTreeListBox::ExecuteDrop;

	virtual sal_Int8	AcceptDrop( const AcceptDropEvent& rEvt );
	virtual sal_Int8	ExecuteDrop( const ExecuteDropEvent& rEvt );
	virtual void		StartDrag( sal_Int8 nAction, const Point& rPosPixel );
	virtual void		DragFinished( sal_Int8 nAction );

	virtual void	Command( const CommandEvent& rCEvt );
	virtual void	RequestHelp( const HelpEvent& rHEvt );
	virtual void 	InitEntry(SvLBoxEntry*,const XubString&,const Image&,const Image&, SvLBoxButtonKind);
public:
			ScContentTree( Window* pParent, const ResId& rResId );
			~ScContentTree();

	String			getAltLongDescText( SvLBoxEntry* pEntry , sal_Bool isAltText) const;
	String			GetEntryAltText( SvLBoxEntry* pEntry ) const;
	String			GetEntryLongDescription( SvLBoxEntry* pEntry ) const;

	void			ObjectFresh( sal_uInt16 nType,SvLBoxEntry* pEntry = NULL);
	sal_Bool		SetNavigatorDlgFlag(sal_Bool isInNavigatorDlg){ return bisInNavigatorDlg=isInNavigatorDlg;};
	virtual void	MouseButtonDown( const MouseEvent& rMEvt );
	virtual void	KeyInput( const KeyEvent& rKEvt );

	void	InitWindowBits( sal_Bool bButtons );

	void	Refresh( sal_uInt16 nType = 0 );

	void	ToggleRoot();
	void	SetRootType( sal_uInt16 nNew );
	sal_uInt16	GetRootType() const				{ return nRootType; }

	void	ActiveDocChanged();
	void	ResetManualDoc();
	void	SetManualDoc(const String& rName);
	sal_Bool	LoadFile(const String& rUrl);
	void	SelectDoc(const String& rName);

	const String& GetHiddenTitle() const	{ return aHiddenTitle; }

    /** Applies the navigator settings to the listbox. */
    void                        ApplySettings();
    /** Stores the current listbox state in the navigator settings. */
    void                        StoreSettings() const;

	static sal_Bool	IsInDrag()	{ return bIsInDrag; }
};

#endif // SC_NAVIPI_HXX

