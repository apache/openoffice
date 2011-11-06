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



#ifndef _EDITUNDO_HXX
#define _EDITUNDO_HXX

#include <editdoc.hxx>
#include <editund2.hxx>
#include <editdata.hxx>
namespace binfilter {

#define UNDO_NOACTION			0
#define UNDO_NEWUNDO			1
#define UNDO_UNDOSDELETED		2
#define UNDO_EMPTYGROUPDELETED	3
#define UNDO_INVALIDEND			4

class ImpEditEngine;
class EditView;

// -----------------------------------------------------------------------
// EditUndoInsertChars
// ------------------------------------------------------------------------
class EditUndoInsertChars : public EditUndo
{
private:
	EPaM			aEPaM;
	String			aText;

public:
					TYPEINFO();
					EditUndoInsertChars( ImpEditEngine* pImpEE, const EPaM& rEPaM, const String& rStr );

	const EPaM&		GetEPaM() { return aEPaM; }
	String&			GetStr() { return aText; }

	virtual void 	Undo();
	virtual void 	Redo();
	virtual void 	Repeat();

	virtual BOOL	Merge( SfxUndoAction *pNextAction );
};

// -----------------------------------------------------------------------
// EditUndoRemoveChars
// ------------------------------------------------------------------------

// -----------------------------------------------------------------------
// EditUndoInsertFeature
// ------------------------------------------------------------------------
class EditUndoInsertFeature : public EditUndo
{
private:
	EPaM			aEPaM;
	SfxPoolItem*	pFeature;

public:
					EditUndoInsertFeature( ImpEditEngine* pImpEE, const EPaM& rEPaM,
											const SfxPoolItem& rFeature);
					~EditUndoInsertFeature();

	virtual void 	Undo();
	virtual void 	Redo();
	virtual void 	Repeat();
};

// -----------------------------------------------------------------------
// EditUndoMoveParagraphs
// ------------------------------------------------------------------------

// -----------------------------------------------------------------------
// EditUndoSetStyleSheet
// ------------------------------------------------------------------------
class EditUndoSetStyleSheet: public EditUndo
{
private:
	USHORT			nPara;
	XubString		aPrevName;
	XubString		aNewName;
	SfxStyleFamily	ePrevFamily;
	SfxStyleFamily	eNewFamily;
	SfxItemSet		aPrevParaAttribs;

public:
					
					EditUndoSetStyleSheet( ImpEditEngine* pImpEE, USHORT nPara,
						const XubString& rPrevName, SfxStyleFamily ePrevFamily,
						const XubString& rNewName, SfxStyleFamily eNewFamily,
						const SfxItemSet& rPrevParaAttribs );
					~EditUndoSetStyleSheet();

	virtual void	Undo();
	virtual void	Redo();
	virtual void	Repeat();
};

// -----------------------------------------------------------------------
// EditUndoSetParaAttribs
// ------------------------------------------------------------------------
class EditUndoSetParaAttribs: public EditUndo
{
private:
	USHORT			nPara;
	SfxItemSet		aPrevItems;
	SfxItemSet		aNewItems;

public:
					EditUndoSetParaAttribs( ImpEditEngine* pImpEE, USHORT nPara, const SfxItemSet& rPrevItems, const SfxItemSet& rNewItems );
					~EditUndoSetParaAttribs();

	virtual void	Undo();
	virtual void	Redo();
	virtual void	Repeat();
};

// -----------------------------------------------------------------------
// EditUndoSetAttribs
// ------------------------------------------------------------------------

// -----------------------------------------------------------------------
// EditUndoTransliteration
// ------------------------------------------------------------------------

// -----------------------------------------------------------------------
// EditUndoMarkSelection
// ------------------------------------------------------------------------


}//end of namespace binfilter
#endif // _EDITUNDO_HXX
