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



#ifndef SC_DRAWVIEW_HXX
#define SC_DRAWVIEW_HXX

#ifndef _SVX_FMVIEW_HXX //autogen
#include <bf_svx/fmview.hxx>
#endif

#include "global.hxx"
namespace binfilter {

class ScDocument;
class ScViewData;
class SdrViewUserMarker;

class ScDrawView: public FmFormView
{
	ScViewData*				pViewData;
	OutputDevice*			pDev;					//! noetig ?
	ScDocument*				pDoc;
	USHORT					nTab;
	Fraction				aScaleX;				// Faktor fuer Drawing-MapMode
	Fraction				aScaleY;
	SdrViewUserMarker*		pDropMarker;
	SdrObject*				pDropMarkObj;
	BOOL					bInConstruct;
	BOOL					bDisableHdl;

	void			Construct();
	void			UpdateBrowser();

protected:
	virtual void	ModelHasChanged();

	// add custom handles (used by other apps, e.g. AnchorPos)
    virtual void AddCustomHdl();

public:
					ScDrawView( OutputDevice* pOut, ScViewData* pData );
	virtual			~ScDrawView();

	virtual void	MarkListHasChanged();
	virtual void	SFX_NOTIFY( SfxBroadcaster& rBC, const TypeId& rBCType,
						 const SfxHint& rHint, const TypeId& rHintType );

	void			DrawMarks( OutputDevice* pOut ) const;


	BOOL			IsDisableHdl() const 	{ return bDisableHdl; }


	void			InvalidateAttribs();
	void			InvalidateDrawTextAttrs();


	void			GetScale( Fraction& rFractX, Fraction& rFractY ) const;
	void			RecalcScale();
	void			UpdateWorkArea();
	USHORT			GetTab() const		{ return nTab; }




	void			VCAddWin( Window* pWin );
	void			VCRemoveWin( Window* pWin );


	USHORT			GetPopupMenuId();
	void			UpdateUserViewOptions();


	String			GetSelectedChartName() const;


	SdrEndTextEditKind	ScEndTextEdit();	// ruft SetDrawTextUndo(0)
};




} //namespace binfilter
#endif

