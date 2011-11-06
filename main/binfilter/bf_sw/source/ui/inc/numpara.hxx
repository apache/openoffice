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


#ifndef _NUMPARA_HXX
#define _NUMPARA_HXX

#include <bf_svtools/bf_solar.h>


#ifndef _SV_LSTBOX_HXX //autogen
#include <vcl/lstbox.hxx>
#endif
#ifndef _SV_FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _SV_FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _SV_BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
namespace binfilter {


/* --- class SwParagraphNumTabPage ----------------------------------------
	Mit dieser TabPage werden Numerierungseinstellungen am Absatz bzw. der
	Absatzvorlage vorgenommen.

 ---------------------------------------------------------------------------*/
class SwParagraphNumTabPage : public SfxTabPage
{
    FixedText               aNumberStyleFT;
	ListBox					aNumberStyleLB;

    FixedLine               aNewStartFL;
	TriStateBox				aNewStartCB;
    TriStateBox             aNewStartNumberCB;
	NumericField			aNewStartNF;

    FixedLine               aCountParaFL;
	TriStateBox				aCountParaCB;
	TriStateBox				aRestartParaCountCB;
	FixedText				aRestartFT;
	NumericField			aRestartNF;

	BOOL					bModified : 1;
	BOOL					bCurNumrule : 1;

	DECL_LINK( NewStartHdl_Impl, CheckBox* );
	DECL_LINK( StyleHdl_Impl, ListBox* );
	DECL_LINK( LineCountHdl_Impl, CheckBox* );

protected:
		SwParagraphNumTabPage(Window* pParent, const SfxItemSet& rSet );

public:
		~SwParagraphNumTabPage();

	static SfxTabPage*	Create(	Window* pParent,
								const SfxItemSet& rSet );
	static USHORT*		GetRanges();

	virtual BOOL		FillItemSet( SfxItemSet& rSet );
	virtual void		Reset( const SfxItemSet& rSet );

	void				EnableNewStart();

	ListBox&			GetStyleBox() {return aNumberStyleLB;};

};


} //namespace binfilter
#endif

