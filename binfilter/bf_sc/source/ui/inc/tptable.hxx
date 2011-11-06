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



#ifndef SC_TPTABLE_HXX
#define SC_TPTABLE_HXX

#ifndef _SV_FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _SV_FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
namespace binfilter {

//===================================================================

class ScTablePage : public SfxTabPage
{
public:
	static	SfxTabPage*	Create			( Window*		 	pParent,
										  const SfxItemSet&	rCoreSet );
	static	USHORT*		GetRanges		();
	virtual	BOOL		FillItemSet		( SfxItemSet& rCoreSet );
	virtual	void		Reset			( const SfxItemSet& rCoreSet );
	virtual int			DeactivatePage	( SfxItemSet* pSet = NULL );
    virtual void        DataChanged     ( const DataChangedEvent& rDCEvt );

private:
		   ScTablePage( Window*			pParent,
						 const SfxItemSet&	rCoreSet );
			~ScTablePage();

    void            ShowImage();

private:
    FixedLine       aFlPageDir;
	RadioButton		aBtnTopDown;
	RadioButton		aBtnLeftRight;
    FixedImage      aBmpPageDir;
    Image           aImgLeftRight;
    Image           aImgTopDown;
    Image           aImgLeftRightHC;
    Image           aImgTopDownHC;
	CheckBox		aBtnPageNo;
	NumericField	aEdPageNo;

    FixedLine       aFlPrint;
	CheckBox		aBtnHeaders;
	CheckBox		aBtnGrid;
	CheckBox		aBtnNotes;
	CheckBox		aBtnObjects;
	CheckBox		aBtnCharts;
	CheckBox		aBtnDrawings;
	CheckBox		aBtnFormulas;
	CheckBox		aBtnNullVals;

    FixedLine       aFlScale;
	RadioButton		aBtnScaleAll;
	RadioButton		aBtnScalePageNum;
    MetricField     aEdScaleAll;
	NumericField	aEdScalePageNum;

#ifdef _TPTABLE_CXX
private:
	//------------------------------------
	// Handler:
	DECL_LINK( ScaleHdl,   RadioButton* );
	DECL_LINK( PageDirHdl, RadioButton* );
	DECL_LINK( PageNoHdl,  CheckBox* );
#endif
};



} //namespace binfilter
#endif // SC_TPTABLE_HXX
