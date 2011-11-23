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


#ifndef _EETEXT_HXX
#define _EETEXT_HXX
namespace binfilter {

// Zeichenattribute....
#define ITEMID_FONT 			EE_CHAR_FONTINFO
#define ITEMID_POSTURE			EE_CHAR_ITALIC
#define ITEMID_WEIGHT			EE_CHAR_WEIGHT
#define ITEMID_SHADOWED 		EE_CHAR_SHADOW
#define ITEMID_CONTOUR			EE_CHAR_OUTLINE
#define ITEMID_CROSSEDOUT		EE_CHAR_STRIKEOUT
#define ITEMID_UNDERLINE		EE_CHAR_UNDERLINE
#define ITEMID_FONTHEIGHT		EE_CHAR_FONTHEIGHT
#define ITEMID_COLOR			EE_CHAR_COLOR
#define ITEMID_WORDLINEMODE 	0
#define ITEMID_PROPSIZE 		0
#define ITEMID_CHARSETCOLOR 	0
#define ITEMID_KERNING			0
#define ITEMID_AUTOKERN			0
#define ITEMID_CASEMAP			0
#define ITEMID_LANGUAGE			0
#define ITEMID_ESCAPEMENT		0
#define ITEMID_NOLINEBREAK		0
#define ITEMID_NOHYPHENHERE 	0

// Absatzattribute
#define ITEMID_ADJUST			EE_PARA_JUST
#define ITEMID_LINESPACING		EE_PARA_SBL
#define ITEMID_WIDOWS			0
#define ITEMID_ORPHANS			0
#define ITEMID_HYPHENZONE		0

#define ITEMID_TABSTOP			EE_PARA_TABS

#define ITEMID_PAPERBIN 		0
#define ITEMID_LRSPACE			EE_PARA_LRSPACE
#define ITEMID_ULSPACE			EE_PARA_ULSPACE
#define ITEMID_PRINT			0
#define ITEMID_OPAQUE			0
#define ITEMID_PROTECT			0
#define ITEMID_BACKGROUND		0
#define ITEMID_SHADOW			0
#define ITEMID_MACRO			0
#define ITEMID_BOX				0
#define ITEMID_BOXINFO			0


} //namespace binfilter
#endif  // EETEXT_HXX


