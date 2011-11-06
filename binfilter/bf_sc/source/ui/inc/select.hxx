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



#ifndef SC_SELECT_HXX
#define SC_SELECT_HXX

#ifndef _SELENG_HXX //autogen
#include <vcl/seleng.hxx>
#endif

#include "viewdata.hxx"		// ScSplitPos
namespace binfilter {

// ---------------------------------------------------------------------------

class ScTabView;
class ScViewData;


class ScViewSelectionEngine : public SelectionEngine
{
private:
	ScSplitPos		eWhich;
public:
					ScViewSelectionEngine( Window* pWindow, ScTabView* pView,
													ScSplitPos eSplitPos );

	ScSplitPos		GetWhich() const			{ return eWhich; }
	void			SetWhich(ScSplitPos eNew)	{ eWhich = eNew; }
};


class ScViewFunctionSet : public FunctionSet			// View (Gridwin / Tastatur)
{
private:
	ScViewData*				pViewData;
	ScViewSelectionEngine*	pEngine;

	BOOL			bAnchor;
	BOOL			bStarted;
	ScTripel		aAnchorPos;


public:
					ScViewFunctionSet( ScViewData* pNewViewData );

	void			SetSelectionEngine( ScViewSelectionEngine* pSelEngine );


	virtual void	BeginDrag(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001  	virtual void	BeginDrag();
	virtual void	CreateAnchor(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void	CreateAnchor();
	virtual void	DestroyAnchor(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void	DestroyAnchor();
	virtual BOOL	SetCursorAtPoint( const Point& rPointPixel, BOOL bDontSelectAtCursor = FALSE ){DBG_BF_ASSERT(0, "STRIP");return FALSE;} //STRIP001 virtual BOOL	SetCursorAtPoint( const Point& rPointPixel, BOOL bDontSelectAtCursor = FALSE );
	virtual BOOL	IsSelectionAtPoint( const Point& rPointPixel ){DBG_BF_ASSERT(0, "STRIP"); return FALSE;} //STRIP001 virtual BOOL	IsSelectionAtPoint( const Point& rPointPixel );
	virtual void	DeselectAtPoint( const Point& rPointPixel ){DBG_ASSERT(0, "STRIP")}; //STRIP001 virtual void	DeselectAtPoint( const Point& rPointPixel );
	virtual void	DeselectAll(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void	DeselectAll();

};


// ---------------------------------------------------------------------------


class ScHeaderFunctionSet : public FunctionSet			// Spalten- / Zeilenkoepfe
{
private:
	ScViewData*		pViewData;
	BOOL			bColumn;				// Col- / Rowbar
	ScSplitPos		eWhich;

	BOOL			bAnchor;
	USHORT			nCursorPos;

public:
					ScHeaderFunctionSet( ScViewData* pNewViewData );


	virtual void	BeginDrag(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void	BeginDrag();
	virtual void	CreateAnchor(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void	CreateAnchor();
	virtual void	DestroyAnchor(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void	DestroyAnchor();
	virtual BOOL	SetCursorAtPoint( const Point& rPointPixel, BOOL bDontSelectAtCursor = FALSE ){DBG_BF_ASSERT(0, "STRIP");return FALSE;} //STRIP001 virtual BOOL	SetCursorAtPoint( const Point& rPointPixel, BOOL bDontSelectAtCursor = FALSE );
	virtual BOOL	IsSelectionAtPoint( const Point& rPointPixel ){DBG_BF_ASSERT(0, "STRIP");return FALSE;}//STRIP001 	virtual BOOL	IsSelectionAtPoint( const Point& rPointPixel );
	virtual void	DeselectAtPoint( const Point& rPointPixel ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void	DeselectAtPoint( const Point& rPointPixel );
	virtual void	DeselectAll(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void	DeselectAll();

	void			SetAnchorFlag(BOOL bSet)	{ bAnchor = bSet; }
};


class ScHeaderSelectionEngine : public SelectionEngine
{
public:
					ScHeaderSelectionEngine( Window* pWindow, ScHeaderFunctionSet* pFuncSet );
};



} //namespace binfilter
#endif
