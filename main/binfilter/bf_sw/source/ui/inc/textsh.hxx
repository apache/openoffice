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



#ifndef _SWTEXTSH_HXX
#define _SWTEXTSH_HXX

#ifndef _SWBASESH_HXX
#include <basesh.hxx>
#endif
class Button; 
namespace binfilter {


class SvxHyperlinkItem;

class SwTextShell: public SwBaseShell
{


public:
	SFX_DECL_INTERFACE(SW_TEXTSHELL);
	TYPEINFO();


	void	GetState(SfxItemSet &);

	void	ExecInsert(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	ExecInsert(SfxRequest &);
	void	StateInsert(SfxItemSet&);
	void	ExecEnterNum(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	ExecEnterNum(SfxRequest &);
	void	ExecBasicMove(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	ExecBasicMove(SfxRequest &);
	void	ExecMove(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	ExecMove(SfxRequest &);
	void	ExecMovePage(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	ExecMovePage(SfxRequest &);
	void	ExecMoveCol(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	ExecMoveCol(SfxRequest &);
	void	ExecMoveLingu(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	ExecMoveLingu(SfxRequest &);
	void	ExecMoveMisc(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	ExecMoveMisc(SfxRequest &);
	void	StateField(SfxItemSet &);
	void	ExecIdx(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");}  ;//STRIP001 	void	ExecIdx(SfxRequest &);
	void	GetIdxState(SfxItemSet &);
	void	ExecGlossary(SfxRequest &);//STRIP001 	void	ExecGlossary(SfxRequest &);

	void	ExecCharAttr(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	ExecCharAttr(SfxRequest &);
	void	ExecCharAttrArgs(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	ExecCharAttrArgs(SfxRequest &);
	void	ExecParaAttr(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	ExecParaAttr(SfxRequest &);
	void	ExecParaAttrArgs(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	ExecParaAttrArgs(SfxRequest &);
	void	ExecAttr(SfxRequest &);
	void	ExecDB(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void	ExecDB(SfxRequest &);
	void 	ExecTransliteration(SfxRequest &){DBG_BF_ASSERT(0, "STRIP");} ;//STRIP001 	void 	ExecTransliteration(SfxRequest &);

	void	GetAttrState(SfxItemSet &);

			 SwTextShell(SwView &rView);
	virtual ~SwTextShell();
};

} //namespace binfilter
#endif
