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


#ifndef _UIPARAM_HXX
#define _UIPARAM_HXX

#ifndef _SVX_DIALOGS_HRC
#include <bf_svx/dialogs.hrc>
#endif
namespace binfilter {

/*--------------------------------------------------------------------
	Beschreibung:	Mapping fuer UI-Paramter
 --------------------------------------------------------------------*/

#define ITEMID_FONTLIST			SID_ATTR_CHAR_FONTLIST
#define ITEMID_TABPOS       	SID_ATTR_TABSTOP_POS
#define ITEMID_DEFTABS      	SID_ATTR_TABSTOP_DEFAULTS
#define ITEMID_TABOFFSET    	SID_ATTR_TABSTOP_OFFSET
#define ITEMID_SEARCH       	SID_SEARCH_ITEM
#define ITEMID_PARADLGLIMITS	SID_ATTR_PARA_DLGLIMITS
#define ITEMID_INFRAME          SID_ATTR_PARA_INFRAME

#define ITEMID_PAGEMODEL	    SID_ATTR_PARA_MODEL
#define	ITEMID_PAGE				SID_ATTR_PAGE
#define ITEMID_SIZE     		SID_ATTR_PAGE_SIZE
#define ITEMID_SETITEM     		SID_ATTR_PAGE_HEADERSET
#define ITEMID_PTR     			SID_ATTR_PAGE_BSP
#define ITEMID_SPELLCHECK 		SID_ATTR_SPELL
#define ITEMID_HYPHENREGION 	SID_ATTR_HYPHENREGION
#define ITEMID_HYPHENZONE       RES_PARATR_HYPHENZONE
#define ITEMID_LINE             SID_ATTR_LINE_STYLE
#define ITEMID_BOXINFO 			SID_ATTR_BORDER_INNER

#define ITEMID_AUTHOR			SID_ATTR_POSTIT_AUTHOR
#define ITEMID_DATE 			SID_ATTR_POSTIT_DATE
#define ITEMID_TEXT 			SID_ATTR_POSTIT_TEXT

#define ITEMID_NUMBERINFO 		SID_ATTR_NUMBERFORMAT_INFO

} //namespace binfilter
#endif // _UIPARAM_HXX
