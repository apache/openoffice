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



#ifndef SC_COLROWBAR_HXX
#define SC_COLROWBAR_HXX

#ifndef SC_HDRCONT_HXX
#include "hdrcont.hxx"
#endif

#ifndef SC_VIEWDATA_HXX
#include "viewdata.hxx"
#endif
namespace binfilter {

class ScHeaderFunctionSet;
class ScHeaderSelectionEngine;

// ---------------------------------------------------------------------------


class ScColBar : public ScHeaderControl
{
	ScViewData*				 pViewData;
	ScHSplitPos				 eWhich;
	ScHeaderFunctionSet*	 pFuncSet;
	ScHeaderSelectionEngine* pSelEngine;

public:
				ScColBar( Window* pParent, ScViewData* pData, ScHSplitPos eWhichPos,
							ScHeaderFunctionSet* pFunc, ScHeaderSelectionEngine* pEng );
				~ScColBar();

	virtual USHORT		GetPos();
	virtual USHORT		GetEntrySize( USHORT nEntryNo );
	virtual String 		GetEntryText( USHORT nEntryNo ){DBG_BF_ASSERT(0, "STRIP");String aStr; return aStr;} //STRIP001 virtual String 		GetEntryText( USHORT nEntryNo );

	virtual void		SetEntrySize( USHORT nPos, USHORT nNewSize ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void		SetEntrySize( USHORT nPos, USHORT nNewSize );
	virtual void		HideEntries( USHORT nStart, USHORT nEnd ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void		HideEntries( USHORT nStart, USHORT nEnd );

	virtual void		SetMarking( BOOL bSet );
	virtual void		SelectWindow(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void		SelectWindow();
	virtual BOOL		IsDisabled(){DBG_BF_ASSERT(0, "STRIP");return FALSE;} //STRIP001 virtual BOOL		IsDisabled();
	virtual BOOL		ResizeAllowed(){DBG_BF_ASSERT(0, "STRIP");return FALSE;} //STRIP001 virtual BOOL		ResizeAllowed();

	virtual void		DrawInvert( long nDragPos ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void		DrawInvert( long nDragPos );

	virtual String		GetDragHelp( long nVal ){DBG_BF_ASSERT(0, "STRIP"); String aStr; return aStr;}//STRIP001 virtual String		GetDragHelp( long nVal );
};


class ScRowBar : public ScHeaderControl
{
	ScViewData*				 pViewData;
	ScVSplitPos				 eWhich;
	ScHeaderFunctionSet*	 pFuncSet;
	ScHeaderSelectionEngine* pSelEngine;

public:
				ScRowBar( Window* pParent, ScViewData* pData, ScVSplitPos eWhichPos,
							ScHeaderFunctionSet* pFunc, ScHeaderSelectionEngine* pEng );
				~ScRowBar();

	virtual USHORT		GetPos();
	virtual USHORT		GetEntrySize( USHORT nEntryNo );
	virtual String 		GetEntryText( USHORT nEntryNo ){DBG_BF_ASSERT(0, "STRIP"); String s; return s;}//STRIP001 virtual String 		GetEntryText( USHORT nEntryNo );

	virtual USHORT		GetHiddenCount( USHORT nEntryNo ){DBG_BF_ASSERT(0, "STRIP"); return 0;}//STRIP001 virtual USHORT		GetHiddenCount( USHORT nEntryNo );	// nur fuer Zeilen

	virtual void		SetEntrySize( USHORT nPos, USHORT nNewSize ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void		SetEntrySize( USHORT nPos, USHORT nNewSize );
	virtual void		HideEntries( USHORT nStart, USHORT nEnd ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void		HideEntries( USHORT nStart, USHORT nEnd );

	virtual void		SetMarking( BOOL bSet );
	virtual void		SelectWindow(){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void		SelectWindow();
	virtual BOOL		IsDisabled(){DBG_BF_ASSERT(0, "STRIP"); return FALSE;}//STRIP001 virtual BOOL		IsDisabled();
	virtual BOOL		ResizeAllowed(){DBG_BF_ASSERT(0, "STRIP"); return FALSE;}//STRIP001 virtual BOOL		ResizeAllowed();

	virtual void		DrawInvert( long nDragPos ){DBG_BF_ASSERT(0, "STRIP");} //STRIP001 virtual void		DrawInvert( long nDragPos );

	virtual String		GetDragHelp( long nVal ){DBG_BF_ASSERT(0, "STRIP"); String s; return s;}//STRIP001 virtual String		GetDragHelp( long nVal );
};



} //namespace binfilter
#endif


