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


#ifndef _UNDO_HXX
#define _UNDO_HXX

#include <bf_svtools/bf_solar.h>

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

#ifndef _SVARRAY_HXX
#include <bf_svtools/svarray.hxx>
#endif

namespace binfilter
{

//====================================================================

class  SfxRepeatTarget
{
public:
						TYPEINFO();
	virtual 			~SfxRepeatTarget() = 0;
};

//====================================================================

class  SfxUndoAction
{
	BOOL bLinked;
public:
							TYPEINFO();
							SfxUndoAction();
	virtual 				~SfxUndoAction();

	virtual BOOL            IsLinked();
	virtual void            SetLinked( BOOL bIsLinked = TRUE );
	virtual void			Undo();
	virtual void			Redo();
	virtual void			Repeat(SfxRepeatTarget&);
	virtual BOOL			CanRepeat(SfxRepeatTarget&) const;

	virtual BOOL			Merge( SfxUndoAction *pNextAction );

	virtual UniString			GetComment() const;
	virtual UniString			GetRepeatComment(SfxRepeatTarget&) const;
	virtual USHORT			GetId() const;

private:
	SfxUndoAction&			operator=( const SfxUndoAction& );	  // n.i.!!
};

//========================================================================

SV_DECL_PTRARR( SfxUndoActions, SfxUndoAction*, 20, 8 )

//====================================================================

/** do not make use of this implementation details, unless you
	really really have to! */
struct  SfxUndoArray
{
	SfxUndoActions          aUndoActions;
	USHORT					nMaxUndoActions;
	USHORT					nCurUndoAction;
	SfxUndoArray 			*pFatherUndoArray;
							SfxUndoArray(USHORT nMax=0):
                                nMaxUndoActions(nMax), nCurUndoAction(0),
                                pFatherUndoArray(0) {}
						   ~SfxUndoArray();
};

//=========================================================================

/** do not make use of this implementation details, unless you
	really really have to! */
class  SfxListUndoAction : public SfxUndoAction, public SfxUndoArray

/*	[Beschreibung]

	UndoAction zur Klammerung mehrerer Undos in einer UndoAction.
	Diese Actions werden vom SfxUndoManager verwendet. Dort
	wird mit < SfxUndoManager::EnterListAction > eine Klammerebene
	geoeffnet und mit <SfxUndoManager::LeaveListAction > wieder
	geschlossen. Redo und Undo auf SfxListUndoActions wirken
	Elementweise.

*/
{
	public:
							TYPEINFO();

							SfxListUndoAction( const UniString &rComment,
								const UniString rRepeatComment, USHORT Id, SfxUndoArray *pFather);
	virtual void			Undo();
	virtual void			Redo();
	virtual void			Repeat(SfxRepeatTarget&);
	virtual BOOL			CanRepeat(SfxRepeatTarget&) const;

	virtual BOOL			Merge( SfxUndoAction *pNextAction );

	virtual UniString			GetComment() const;
	virtual UniString			GetRepeatComment(SfxRepeatTarget&) const;
	virtual USHORT			GetId() const;

	void SetComment( const UniString& rComment );

	private:

	USHORT					nId;
	UniString					aComment, aRepeatComment;

};

//=========================================================================

class  SfxUndoManager
{
	friend class SfxLinkUndoAction;

	SfxUndoArray 			*pUndoArray;
	SfxUndoArray			*pActUndoArray;
	SfxUndoArray			*pFatherUndoArray;

public:
							SfxUndoManager( USHORT nMaxUndoActionCount = 20 );
	virtual 				~SfxUndoManager();

	virtual void			SetMaxUndoActionCount( USHORT nMaxUndoActionCount );
	virtual USHORT			GetMaxUndoActionCount() const;
	virtual void			Clear();

	virtual void			AddUndoAction( SfxUndoAction *pAction, BOOL bTryMerg=FALSE );

	virtual USHORT			GetUndoActionCount() const;
	virtual USHORT			GetUndoActionId(USHORT nNo=0) const;
	virtual UniString		GetUndoActionComment( USHORT nNo=0 ) const;

	virtual BOOL			Undo( USHORT nCount=1 );
	virtual void			Undo( SfxUndoAction &rAction );

	virtual USHORT			GetRedoActionCount() const;
	virtual USHORT			GetRedoActionId(USHORT nNo=0) const;
	virtual UniString			GetRedoActionComment( USHORT nNo=0 ) const;

	virtual BOOL			Redo( USHORT nCount=1 );
	virtual void			Redo( SfxUndoAction &rAction );
	virtual	void			ClearRedo();

	virtual USHORT			GetRepeatActionCount() const;
	virtual UniString			GetRepeatActionComment( SfxRepeatTarget &rTarget, USHORT nNo = 0) const;
	virtual BOOL			Repeat( SfxRepeatTarget &rTarget, USHORT nFrom=0, USHORT nCount=1 );
	virtual void			Repeat( SfxRepeatTarget &rTarget, SfxUndoAction &rAction );
	virtual BOOL			CanRepeat( SfxRepeatTarget &rTarget, USHORT nNo = 0 ) const;
	virtual BOOL			CanRepeat( SfxRepeatTarget &rTarget, SfxUndoAction &rAction ) const;

	virtual void            EnterListAction(const UniString &rComment, const UniString& rRepeatComment, USHORT nId=0);
	virtual void 			LeaveListAction();
};

//=========================================================================

class  SfxLinkUndoAction : public SfxUndoAction

/*	[Beschreibung]

	Die SfxLinkUndoAction dient zur Verbindung zweier SfxUndoManager. Die
	im ersten SfxUndoManager eingefuegten SfxUndoAction leiten ihr Undo und Redo
	an den zweiten weiter, so dass ein Undo und Redo am ersten
	SfxUndoManager wie eine am zweiten wirkt.

	Die SfxLinkUndoAction ist nach dem Einfuegen der SfxUndoAction am
	zweiten SfxUndoManager einzufuegen. Waehrend der zweite SfxUndoManager
	vom ersten ferngesteuert wird, duerfen an ihm weder Actions eingefuegt werden,
	noch darf Undo/Redo aufgerufen werden.

*/

{
public:
							TYPEINFO();
							~SfxLinkUndoAction();

	virtual void			Undo();
	virtual void			Redo();
	virtual BOOL			CanRepeat(SfxRepeatTarget& r) const;

	virtual void			Repeat(SfxRepeatTarget&r);

	virtual UniString			GetComment() const;
	virtual UniString			GetRepeatComment(SfxRepeatTarget&r) const;
	virtual USHORT			GetId() const;

	SfxUndoAction*			GetAction() const { return pAction; }

protected:
	SfxUndoManager			*pUndoManager;
	SfxUndoAction			*pAction;

};

}

#endif
