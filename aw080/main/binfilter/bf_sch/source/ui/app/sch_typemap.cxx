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




#ifdef _MSC_VER
#pragma hdrstop
#endif

#include <bf_svx/eeitem.hxx>

#include "eetext.hxx"
#ifndef _SVX_FONTITEM_HXX //autogen
#include <bf_svx/fontitem.hxx>
#endif
#ifndef _SVX_POSTITEM_HXX //autogen
#include <bf_svx/postitem.hxx>
#endif
#ifndef _SVX_WGHTITEM_HXX //autogen
#include <bf_svx/wghtitem.hxx>
#endif
#ifndef _SVX_SHDDITEM_HXX //autogen
#include <bf_svx/shdditem.hxx>
#endif
#ifndef _SVX_ITEM_HXX //autogen
#include <bf_svx/cntritem.hxx>
#endif
#ifndef _SVX_CRSDITEM_HXX //autogen
#include <bf_svx/crsditem.hxx>
#endif
#ifndef _SVX_UDLNITEM_HXX //autogen
#include <bf_svx/udlnitem.hxx>
#endif
#ifndef _SVX_FHGTITEM_HXX //autogen
#include <bf_svx/fhgtitem.hxx>
#endif
#ifndef _SVX_COLRITEM_HXX //autogen
#include <bf_svx/colritem.hxx>
#endif

#include <bf_svx/svxids.hrc>

#define ITEMID_FMTSPLIT		0
#define ITEMID_PAGEMODEL	0
#include "eetext.hxx"

#define ITEMID_LINE			0
#define ITEMID_BRUSH		0
#define ITEMID_FMTBREAK		0
#define ITEMID_FMTKEEP		0
#define ITEMID_SIZE			0
#include <bf_svx/sizeitem.hxx>

#define ITEMID_PAGE			0


#ifndef _SFXSTRITEM_HXX
#include <bf_svtools/stritem.hxx>
#endif

#define ITEMID_AUTHOR		0
#define ITEMID_DATE			0
#define ITEMID_TEXT			0

#define ITEMID_SEARCH		0

#define ITEMID_HORJUSTIFY	0
#define ITEMID_VERJUSTIFY	0
#define ITEMID_ORIENTATION	0

#include <bf_svx/zoomitem.hxx>

#define ITEMID_CHARTSTYLE	0
#ifndef _SVX_CHRTITEM_HXX //autogen
#include <bf_svx/chrtitem.hxx>
#endif

#ifndef _SFXMSG_HXX
#include <bf_sfx2/msg.hxx>
#endif

namespace binfilter {

/*?*/ typedef SfxVoidItem	CharSetItem;
/*?*/ typedef SfxVoidItem	FontFamilyItem;
/*?*/ typedef SfxVoidItem	FontPitchItem;
/*?*/ typedef SfxVoidItem	FontAlignItem;
/*?*/ typedef SfxVoidItem	FontWeightItem;
/*?*/ typedef SfxVoidItem	FontUnderlineItem;
/*?*/ typedef SfxVoidItem	FontStrikeoutItem;
/*?*/ typedef SfxVoidItem	FontItalicItem;
/*?*/ typedef SfxVoidItem	SvxDbTypeItem;
/*?*/ typedef SfxVoidItem SvxLineSpaceItem;
/*?*/ typedef SfxVoidItem	SvxInterLineSpaceItem;
/*?*/ typedef SfxVoidItem	SvxBreakItem;
/*?*/ typedef SfxVoidItem	BrushStyleItem;
/*?*/ typedef SfxVoidItem	SvxNumTypeItem;
/*?*/ typedef SfxVoidItem	SvxShadowLocationItem;
/*?*/ typedef SfxVoidItem	SvxChooseControlEnumItem;
/*?*/ typedef SfxVoidItem	SvxDrawToolEnumItem;
/*?*/ typedef SfxVoidItem	SvxChooseControlItem;
/*?*/ typedef SfxVoidItem	SvxDrawToolItem;
/*?*/ typedef SfxVoidItem	SvxCellHorJustifyEnumItem;
/*?*/ typedef SfxVoidItem	SvxCellVerJustifyEnumItem;
/*?*/ typedef SfxVoidItem	SvxCellOrientationEnumItem;
/*?*/ typedef SfxEnumItem	SvxStyleTypeItem;

#define SFX_TYPEMAP
#include "schslots.hxx"





}
