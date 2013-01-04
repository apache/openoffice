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



#ifndef SC_UNDOTAB_HXX
#define SC_UNDOTAB_HXX

#ifndef SC_UNDOBASE_HXX
#include "undobase.hxx"
#endif
#ifndef SC_MARKDATA_HXX
#include "markdata.hxx"
#endif

#ifndef _SVSTDARR_USHORTS

#define _SVSTDARR_USHORTS
#include <bf_svtools/svstdarr.hxx>

#endif

#ifndef _SVSTDARR_STRINGS

#define _SVSTDARR_STRINGS
#include <bf_svtools/svstdarr.hxx>

#endif
namespace binfilter {
class ScDocShell;
class ScDocument;
class SdrUndoAction;
class ScPrintRangeSaver;
class SdrObject;

//----------------------------------------------------------------------------

class ScUndoInsertTab : public ScSimpleUndo
{
public:
					ScUndoInsertTab(
							ScDocShell* pNewDocShell,
							USHORT nTabNum,
							BOOL bApp,
							const String& rNewName);
	virtual			~ScUndoInsertTab();

	virtual BOOL	CanRepeat(SfxRepeatTarget& rTarget) const;

	virtual String	GetComment() const;

private:
	String			sNewName;
	SdrUndoAction*	pDrawUndo;
	ULONG			nEndChangeAction;
	USHORT			nTab;
	BOOL			bAppend;

	void			SetChangeTrack();
};





class ScUndoRenameTab: public ScSimpleUndo
{
public:
					ScUndoRenameTab(
							ScDocShell* pNewDocShell,
							USHORT nT,
							const String& rOldName,
							const String& rNewName);
	virtual			~ScUndoRenameTab();

	virtual BOOL	CanRepeat(SfxRepeatTarget& rTarget) const;

	virtual String	GetComment() const;

private:
	USHORT	nTab;
	String	sOldName;
	String	sNewName;

};












class ScUndoShowHideTab : public ScSimpleUndo
{
public:
					ScUndoShowHideTab(
							ScDocShell* pShell,
							USHORT nNewTab, BOOL bNewShow );
	virtual			~ScUndoShowHideTab();

	virtual BOOL	CanRepeat(SfxRepeatTarget& rTarget) const;

	virtual String	GetComment() const;

private:
	USHORT	nTab;
	BOOL	bShow;

};




class ScUndoPrintRange : public ScSimpleUndo
{
public:
					ScUndoPrintRange( ScDocShell* pShell, USHORT nNewTab,
										ScPrintRangeSaver* pOld, ScPrintRangeSaver* pNew );
	virtual			~ScUndoPrintRange();

private:
	USHORT				nTab;
	ScPrintRangeSaver*	pOldRanges;
	ScPrintRangeSaver*	pNewRanges;

};







} //namespace binfilter
#endif

