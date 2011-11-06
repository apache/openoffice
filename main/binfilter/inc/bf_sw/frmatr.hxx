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


#ifndef _FRMATR_HXX
#define _FRMATR_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _FORMAT_HXX
#include <format.hxx>		//fuer Implementierung der inlines
#endif
namespace binfilter {

//------------------------ Inlines ---------------------------------


/******************************************************************************
 *	Implementierung der FrameAttribut Methoden vom SwAttrSet
 ******************************************************************************/

#if ! (defined(MACOSX) && ( __GNUC__ < 3 ))
// GrP moved to gcc_outl.cxx; revisit with gcc3
inline const SvxPaperBinItem &SwAttrSet::GetPaperBin(BOOL bInP) const
	{ return (const SvxPaperBinItem&)Get( RES_PAPER_BIN,bInP); }
inline const SvxLRSpaceItem &SwAttrSet::GetLRSpace(BOOL bInP) const
	{ return (const SvxLRSpaceItem&)Get( RES_LR_SPACE,bInP); }
inline const SvxULSpaceItem &SwAttrSet::GetULSpace(BOOL bInP) const
	{ return (const SvxULSpaceItem&)Get( RES_UL_SPACE,bInP); }
inline const SvxPrintItem &SwAttrSet::GetPrint(BOOL bInP) const
	{ return (const SvxPrintItem&)Get( RES_PRINT,bInP); }
inline const SvxOpaqueItem &SwAttrSet::GetOpaque(BOOL bInP) const
	{ return (const SvxOpaqueItem&)Get( RES_OPAQUE,bInP); }
inline const SvxProtectItem &SwAttrSet::GetProtect(BOOL bInP) const
	{ return (const SvxProtectItem&)Get( RES_PROTECT,bInP); }
inline const SvxBoxItem &SwAttrSet::GetBox(BOOL bInP) const
	{ return (const SvxBoxItem&)Get( RES_BOX,bInP); }
inline const SvxFmtKeepItem &SwAttrSet::GetKeep(BOOL bInP) const
	{ return (const SvxFmtKeepItem&)Get( RES_KEEP,bInP); }
inline const SvxBrushItem &SwAttrSet::GetBackground(BOOL bInP) const
	{ return (const SvxBrushItem&)Get( RES_BACKGROUND,bInP); }
inline const SvxShadowItem &SwAttrSet::GetShadow(BOOL bInP) const
	{ return (const SvxShadowItem&)Get( RES_SHADOW,bInP); }
inline const SvxFmtBreakItem &SwAttrSet::GetBreak(BOOL bInP) const
	{ return (const SvxFmtBreakItem&)Get( RES_BREAK,bInP); }
inline const SvxMacroItem &SwAttrSet::GetMacro(BOOL bInP) const
	{ return (const SvxMacroItem&)Get( RES_FRMMACRO,bInP); }
inline const SvxFrameDirectionItem &SwAttrSet::GetFrmDir(BOOL bInP) const
	{ return (const SvxFrameDirectionItem&)Get( RES_FRAMEDIR,bInP); }


/******************************************************************************
 *	Implementierung der FrameAttribut Methoden vom SwFmt
 ******************************************************************************/

inline const SvxPaperBinItem &SwFmt::GetPaperBin(BOOL bInP) const
	{ return aSet.GetPaperBin(bInP); }
inline const SvxLRSpaceItem &SwFmt::GetLRSpace(BOOL bInP) const
	{ return aSet.GetLRSpace(bInP); }
inline const SvxULSpaceItem &SwFmt::GetULSpace(BOOL bInP) const
	{ return aSet.GetULSpace(bInP); }
inline const SvxPrintItem &SwFmt::GetPrint(BOOL bInP) const
	{ return aSet.GetPrint(bInP); }
inline const SvxOpaqueItem &SwFmt::GetOpaque(BOOL bInP) const
	{ return aSet.GetOpaque(bInP); }
inline const SvxProtectItem &SwFmt::GetProtect(BOOL bInP) const
	{ return aSet.GetProtect(bInP); }
inline const SvxBoxItem &SwFmt::GetBox(BOOL bInP) const
	{ return aSet.GetBox(bInP); }
inline const SvxFmtKeepItem &SwFmt::GetKeep(BOOL bInP) const
	{ return aSet.GetKeep(bInP); }
inline const SvxBrushItem &SwFmt::GetBackground(BOOL bInP) const
	{ return aSet.GetBackground(bInP); }
inline const SvxShadowItem &SwFmt::GetShadow(BOOL bInP) const
	{ return aSet.GetShadow(bInP); }
inline const SvxFmtBreakItem &SwFmt::GetBreak(BOOL bInP) const
	{ return aSet.GetBreak(bInP); }
inline const SvxMacroItem &SwFmt::GetMacro(BOOL bInP) const
	{ return aSet.GetMacro(bInP); }
inline const SvxFrameDirectionItem	&SwFmt::GetFrmDir(BOOL bInP) const
	{ return aSet.GetFrmDir(bInP); }
        
#endif

} //namespace binfilter
#endif //_FRMATR_HXX
