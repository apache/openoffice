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



#ifndef SC_UNDOCELL_HXX
#define SC_UNDOCELL_HXX

#ifndef SC_UNDOBASE_HXX
#include "undobase.hxx"
#endif
namespace binfilter {

class ScDocShell;
class ScBaseCell;
class ScPatternAttr;
class EditTextObject;
class SdrUndoAction;
class ScDetOpList;
class ScDetOpData;
class ScRangeName;

//----------------------------------------------------------------------------







class ScUndoPutCell: public ScSimpleUndo
{
public:
					ScUndoPutCell( ScDocShell* pNewDocShell,
							const ScAddress& rNewPos,
							ScBaseCell* pUndoCell, ScBaseCell* pRedoCell, BOOL bHeight );
	virtual 		~ScUndoPutCell();

	virtual BOOL	CanRepeat(SfxRepeatTarget& rTarget) const;

	virtual String	GetComment() const;

private:
	ScAddress		aPos;
	ScBaseCell*		pOldCell;
	ScBaseCell*		pEnteredCell;
	ULONG			nEndChangeAction;
	BOOL			bNeedHeight;

	void			SetChangeTrack();
};






class ScUndoNote: public ScSimpleUndo
{
public:
					ScUndoNote( ScDocShell* pNewDocShell,
								BOOL bShow, const ScAddress& rNewPos,
								SdrUndoAction* pDraw );
	virtual 		~ScUndoNote();



private:
	BOOL			bIsShow;
	ScAddress		aPos;
	SdrUndoAction*	pDrawUndo;
};






class ScUndoRangeNames: public ScSimpleUndo
{
public:
					ScUndoRangeNames( ScDocShell* pNewDocShell,
										ScRangeName* pOld, ScRangeName* pNew );
	virtual 		~ScUndoRangeNames();

	virtual BOOL	CanRepeat(SfxRepeatTarget& rTarget) const;

	virtual String	GetComment() const;

private:
	ScRangeName*	pOldRanges;
	ScRangeName*	pNewRanges;

};



} //namespace binfilter
#endif

