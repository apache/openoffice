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



#include <bf_svtools/undo.hxx>

namespace binfilter
{

TYPEINIT0(SfxUndoAction);
TYPEINIT0(SfxListUndoAction);
TYPEINIT0(SfxLinkUndoAction);
TYPEINIT0(SfxRepeatTarget);

SfxRepeatTarget::~SfxRepeatTarget()
{
}

BOOL SfxUndoAction::IsLinked()
{
	return bLinked;
}

void SfxUndoAction::SetLinked( BOOL bIsLinked )
{
	bLinked = bIsLinked;
}

SfxUndoAction::~SfxUndoAction()
{
}

SfxUndoAction::SfxUndoAction()
{
	SetLinked( FALSE );
}

BOOL SfxUndoAction::Merge( SfxUndoAction * )
{
	return FALSE;
}

XubString SfxUndoAction::GetComment() const
{
	return XubString();
}

USHORT SfxUndoAction::GetId() const
{
	return 0;
}

XubString SfxUndoAction::GetRepeatComment(SfxRepeatTarget&) const
{
	return GetComment();
}

void SfxUndoAction::Undo()
{
}

void SfxUndoAction::Redo()
{
}

void SfxUndoAction::Repeat(SfxRepeatTarget&)
{
}

BOOL SfxUndoAction::CanRepeat(SfxRepeatTarget&) const
{
	return TRUE;
}

SfxUndoManager::SfxUndoManager( USHORT nMaxUndoActionCount ) : pFatherUndoArray(0)
{
	pUndoArray=new SfxUndoArray(nMaxUndoActionCount);
	pActUndoArray=pUndoArray;

}

SfxUndoManager::~SfxUndoManager()
{
	delete pUndoArray;
}

void SfxUndoManager::SetMaxUndoActionCount( USHORT nMaxUndoActionCount )
{
	// Remove entries from the pActUndoArray when we have to reduce
	// the number of entries due to a lower nMaxUndoActionCount.
	// Both redo and undo action entries will be removed until we reached the
	// new nMaxUndoActionCount.
	
	long nNumToDelete = pActUndoArray->aUndoActions.Count() - nMaxUndoActionCount;
	if ( nNumToDelete > 0 )
	{
		while ( nNumToDelete > 0 )
		{
			USHORT nPos = pActUndoArray->aUndoActions.Count();
			if ( nPos > pActUndoArray->nCurUndoAction )
			{
				if ( !pActUndoArray->aUndoActions[nPos-1]->IsLinked() )
				{
					delete pActUndoArray->aUndoActions[nPos-1];
					pActUndoArray->aUndoActions.Remove( nPos-1 );
					--nNumToDelete;
				}
			}

			if ( nNumToDelete > 0 && pActUndoArray->nCurUndoAction > 0 )
			{
				if ( !pActUndoArray->aUndoActions[0]->IsLinked() )
				{
					delete pActUndoArray->aUndoActions[0];
					pActUndoArray->aUndoActions.Remove(0);
					--pActUndoArray->nCurUndoAction;
					--nNumToDelete;
				}
			}
			
			if ( nPos == pActUndoArray->aUndoActions.Count() )
				break; // Cannot delete more entries
		}
	}

	pActUndoArray->nMaxUndoActions = nMaxUndoActionCount;
}

//------------------------------------------------------------------------

USHORT SfxUndoManager::GetMaxUndoActionCount() const
{
	return pActUndoArray->nMaxUndoActions;
}

//------------------------------------------------------------------------

void SfxUndoManager::Clear()
{
	while ( pActUndoArray->aUndoActions.Count() )
	{
		SfxUndoAction *pAction=
			pActUndoArray->aUndoActions[pActUndoArray->aUndoActions.Count() - 1];
		pActUndoArray->aUndoActions.Remove( pActUndoArray->aUndoActions.Count() - 1 );
		delete pAction;
	}

	pActUndoArray->nCurUndoAction = 0;
}

//------------------------------------------------------------------------

void SfxUndoManager::ClearRedo()
{
	while ( pActUndoArray->aUndoActions.Count() > pActUndoArray->nCurUndoAction )
	{
		SfxUndoAction *pAction=
			pActUndoArray->aUndoActions[pActUndoArray->aUndoActions.Count() - 1];
		pActUndoArray->aUndoActions.Remove( pActUndoArray->aUndoActions.Count() - 1 );
		delete pAction;
	}
}

//------------------------------------------------------------------------

void SfxUndoManager::AddUndoAction( SfxUndoAction *pAction, BOOL bTryMerge )
{
	// Redo-Actions loeschen
	for ( USHORT nPos = pActUndoArray->aUndoActions.Count();
		  nPos > pActUndoArray->nCurUndoAction; --nPos )
		delete pActUndoArray->aUndoActions[nPos-1];

	pActUndoArray->aUndoActions.Remove(
		pActUndoArray->nCurUndoAction,
		pActUndoArray->aUndoActions.Count() - pActUndoArray->nCurUndoAction );

	if ( pActUndoArray->nMaxUndoActions )
	{
		SfxUndoAction *pTmpAction = pActUndoArray->nCurUndoAction ?
			pActUndoArray->aUndoActions[pActUndoArray->nCurUndoAction-1] : 0;

		if ( !bTryMerge || !(pTmpAction && pTmpAction->Merge(pAction)) )
		{
			// auf Max-Anzahl anpassen
			if( pActUndoArray == pUndoArray )
				while( pActUndoArray->aUndoActions.Count() >=
					   pActUndoArray->nMaxUndoActions &&
					   !pActUndoArray->aUndoActions[0]->IsLinked() )
				{
					delete pActUndoArray->aUndoActions[0];
					pActUndoArray->aUndoActions.Remove(0);
					--pActUndoArray->nCurUndoAction;
				}

			// neue Action anh"angen
			const SfxUndoAction* pTemp = pAction;
			pActUndoArray->aUndoActions.Insert(
				pTemp, pActUndoArray->nCurUndoAction++ );
			return;
		}
	}

	delete pAction;
}

//------------------------------------------------------------------------

USHORT SfxUndoManager::GetUndoActionCount() const
{
	return pActUndoArray->nCurUndoAction;
}

//------------------------------------------------------------------------

XubString SfxUndoManager::GetUndoActionComment( USHORT nNo ) const
{
	if( nNo < pActUndoArray->nCurUndoAction )
	{
		return pActUndoArray->aUndoActions[pActUndoArray->nCurUndoAction-1-nNo]->GetComment(); //!
	}
	else
	{
		XubString aEmpty;
		return aEmpty;
	}
}

//------------------------------------------------------------------------

USHORT SfxUndoManager::GetUndoActionId( USHORT nNo ) const
{
	if( nNo < pActUndoArray->nCurUndoAction )
	{
		return pActUndoArray->aUndoActions[pActUndoArray->nCurUndoAction-1-nNo]->GetId(); //!
	}
	else
	{
		return 0;
	}
}

//------------------------------------------------------------------------

BOOL SfxUndoManager::Undo( USHORT )
{
	if ( pActUndoArray->nCurUndoAction )
	{
		Undo( *pActUndoArray->aUndoActions[ --pActUndoArray->nCurUndoAction ] );
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------

void SfxUndoManager::Undo( SfxUndoAction &rAction )
{
	rAction.Undo();
}

//------------------------------------------------------------------------

USHORT SfxUndoManager::GetRedoActionCount() const
{
	return pActUndoArray->aUndoActions.Count() - pActUndoArray->nCurUndoAction; //!
}

//------------------------------------------------------------------------

XubString SfxUndoManager::GetRedoActionComment( USHORT nNo ) const
{
	return pActUndoArray->aUndoActions[pActUndoArray->nCurUndoAction+nNo]->GetComment(); //!
}

//------------------------------------------------------------------------

USHORT SfxUndoManager::GetRedoActionId( USHORT nNo ) const
{
	return pActUndoArray->aUndoActions[pActUndoArray->nCurUndoAction+nNo]->GetId(); //!
}

//------------------------------------------------------------------------

BOOL SfxUndoManager::Redo( USHORT )
{
	if ( pActUndoArray->aUndoActions.Count() > pActUndoArray->nCurUndoAction )
	{
		Redo( *pActUndoArray->aUndoActions[pActUndoArray->nCurUndoAction++] );
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------

void SfxUndoManager::Redo( SfxUndoAction &rAction )
{
	rAction.Redo();
}

//------------------------------------------------------------------------

USHORT SfxUndoManager::GetRepeatActionCount() const
{
	return pActUndoArray->aUndoActions.Count();
}

//------------------------------------------------------------------------

XubString SfxUndoManager::GetRepeatActionComment( SfxRepeatTarget &rTarget, USHORT nNo ) const
{
	return pActUndoArray->aUndoActions[ pActUndoArray->aUndoActions.Count() - 1 - nNo ]
		->GetRepeatComment(rTarget);
}

//------------------------------------------------------------------------

BOOL SfxUndoManager::Repeat( SfxRepeatTarget &rTarget, USHORT, USHORT )
{
	if ( pActUndoArray->aUndoActions.Count() )
	{
		Repeat( rTarget, *pActUndoArray->aUndoActions[ pActUndoArray->aUndoActions.Count() - 1 ] );
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------

void SfxUndoManager::Repeat( SfxRepeatTarget &rTarget, SfxUndoAction &rAction )
{
	if ( rAction.CanRepeat(rTarget) )
		rAction.Repeat(rTarget);
}

//------------------------------------------------------------------------

BOOL SfxUndoManager::CanRepeat( SfxRepeatTarget &rTarget, SfxUndoAction &rAction ) const
{
	return rAction.CanRepeat(rTarget);
}

//------------------------------------------------------------------------

BOOL SfxUndoManager::CanRepeat( SfxRepeatTarget &rTarget, USHORT nNo ) const
{
	if ( pActUndoArray->aUndoActions.Count() > nNo )
	{
		USHORT nActionNo = pActUndoArray->aUndoActions.Count() - 1 - nNo;
		return pActUndoArray->aUndoActions[nActionNo]->CanRepeat(rTarget);
	}

	return FALSE;
}

//------------------------------------------------------------------------

void SfxUndoManager::EnterListAction(const XubString& rComment, const XubString &rRepeatComment, USHORT nId )
{
	if ( !pUndoArray->nMaxUndoActions )
		return;

	pFatherUndoArray=pActUndoArray;
	SfxListUndoAction *pAction=new SfxListUndoAction(
		rComment, rRepeatComment, nId, pActUndoArray);
	AddUndoAction( pAction );
	pActUndoArray=pAction;
}

//------------------------------------------------------------------------

void SfxUndoManager::LeaveListAction()
{
	if ( !pUndoArray->nMaxUndoActions )
		return;

	if( pActUndoArray == pUndoArray )
	{
		return;
	}

	SfxUndoArray* pTmp=pActUndoArray;
	pActUndoArray=pActUndoArray->pFatherUndoArray;

	// If no undo action where added, delete the undo list action
	SfxUndoAction *pTmpAction= pActUndoArray->aUndoActions[pActUndoArray->nCurUndoAction-1];
	if(!pTmp->nCurUndoAction)
	{
		pActUndoArray->aUndoActions.Remove( --pActUndoArray->nCurUndoAction);
		delete pTmpAction;
	}
	else
	{
		// if the undo array has no comment, try to get it from its children
		SfxListUndoAction* pList = dynamic_cast< SfxListUndoAction * >( pTmpAction );
		if( pList && pList->GetComment().Len() == 0 )
		{
			USHORT n;
			for( n = 0; n < pList->aUndoActions.Count(); n++ )
			{
				if( pList->aUndoActions[n]->GetComment().Len() )
				{
					pList->SetComment( pList->aUndoActions[n]->GetComment() );
					break;
				}
			}
		}
	}
}

//------------------------------------------------------------------------

USHORT SfxListUndoAction::GetId() const
{
	return nId;
}

//------------------------------------------------------------------------

XubString SfxListUndoAction::GetComment() const
{
	return aComment;
}

//------------------------------------------------------------------------

void SfxListUndoAction::SetComment( const UniString& rComment )
{
	aComment = rComment;
}

//------------------------------------------------------------------------

XubString SfxListUndoAction::GetRepeatComment(SfxRepeatTarget &) const
{
	return aRepeatComment;
}


//------------------------------------------------------------------------

SfxListUndoAction::SfxListUndoAction
(
	const XubString &rComment,
	const XubString rRepeatComment,
	USHORT Id,
	SfxUndoArray *pFather
)
: nId(Id), aComment(rComment), aRepeatComment(rRepeatComment) 
{
	pFatherUndoArray = pFather;
	nMaxUndoActions = USHRT_MAX;
}

//------------------------------------------------------------------------

void SfxListUndoAction::Undo()
{
	for(INT16 i=nCurUndoAction-1;i>=0;i--)
		aUndoActions[i]->Undo();
	nCurUndoAction=0;
}

//------------------------------------------------------------------------

void SfxListUndoAction::Redo()
{
	for(USHORT i=nCurUndoAction;i<aUndoActions.Count();i++)
		aUndoActions[i]->Redo();
	nCurUndoAction = aUndoActions.Count();
}

//------------------------------------------------------------------------

void SfxListUndoAction::Repeat(SfxRepeatTarget&rTarget)
{
	for(USHORT i=0;i<nCurUndoAction;i++)
		aUndoActions[i]->Repeat(rTarget);
}

//------------------------------------------------------------------------

BOOL SfxListUndoAction::CanRepeat(SfxRepeatTarget&r)  const
{
	for(USHORT i=0;i<nCurUndoAction;i++)
		if(!aUndoActions[i]->CanRepeat(r))
			return FALSE;
	return TRUE;
}

//------------------------------------------------------------------------

BOOL SfxListUndoAction::Merge( SfxUndoAction *pNextAction )
{
	return aUndoActions.Count() && aUndoActions[aUndoActions.Count()-1]->Merge( pNextAction );
}

//------------------------------------------------------------------------

void SfxLinkUndoAction::Undo()
{
	if ( pAction )
		pUndoManager->Undo(1);
}

//------------------------------------------------------------------------

void SfxLinkUndoAction::Redo()
{
	if ( pAction )
		pUndoManager->Redo(1);
}

//------------------------------------------------------------------------


BOOL SfxLinkUndoAction::CanRepeat(SfxRepeatTarget& r) const
{
	return pAction && pUndoManager->CanRepeat(r,*pAction);
}


//------------------------------------------------------------------------


void SfxLinkUndoAction::Repeat(SfxRepeatTarget&r)
{
	if ( pAction )
		pUndoManager->Repeat(r,*pAction);
}


//------------------------------------------------------------------------

XubString SfxLinkUndoAction::GetComment() const
{
	if ( pAction )
		return pAction->GetComment();
	else
		return XubString();
}


//------------------------------------------------------------------------

XubString SfxLinkUndoAction::GetRepeatComment(SfxRepeatTarget&r) const
{
	if ( pAction )
		return pAction->GetRepeatComment(r);
	else
		return XubString();
}

//------------------------------------------------------------------------

SfxLinkUndoAction::~SfxLinkUndoAction()
{
	if( pAction )
		pAction->SetLinked( FALSE );
}


//------------------------------------------------------------------------

SfxUndoArray::~SfxUndoArray()
{
	while ( aUndoActions.Count() )
	{
		SfxUndoAction *pAction =
			aUndoActions[ aUndoActions.Count() - 1 ];
		aUndoActions.Remove( aUndoActions.Count() - 1 );
		delete pAction;
	}
}


USHORT SfxLinkUndoAction::GetId() const
{
      return pAction ? pAction->GetId() : 0;
}


}
