/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 * 
 * Copyright IBM Corporation 2010.
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
 
#ifndef __UNOMSAAEVENT_HXX
#define __UNOMSAAEVENT_HXX

//STATE_CHANGED with get FOCUSED -> EVENT_OBJECT_FOCUS
const short UM_EVENT_STATE_FOCUSED = 0 ;

//STATE_CHANGED --> EVENT_OBJECT_STATECHANGE
const short UM_EVENT_STATE_CHECKED  = 1 ;
const short UM_EVENT_STATE_ARMED    = 2 ;
const short UM_EVENT_STATE_PRESSED  = 3 ;
const short UM_EVENT_STATE_SELECTED = 4 ;
const short UM_EVENT_STATE_SHOWING  = 5 ;

//if acc role is MENU_BAR, STATE_CHANGED with get FOCUSED -> EVENT_SYSTEM_MENUSTART
const short UM_EVENT_MENU_START = 6 ;

//if acc role is MENU_BAR, STATE_CHANGED with lose FOCUSED -> EVENT_SYSTEM_MENUEND
const short UM_EVENT_MENU_END = 7 ;

//if acc role is POPUP_MENU, STATE_CHANGED with get FOCUSED -> EVENT_SYSTEM_MENUPOPUPSTART
const short UM_EVENT_MENUPOPUPSTART = 8 ;

//if acc role is POPUP_MENU, STATE_CHANGED with lose FOCUSED -> EVENT_SYSTEM_MENUPOPUPEND
const short UM_EVENT_MENUPOPUPEND = 9 ;

//SELECTION_CHANGED -> EVENT_OBJECT_SELECTION
const short UM_EVENT_SELECTION_CHANGED = 10 ;

//INVALIDATE_ALL_CHILDREN --> EVENT_OBJECT_SHOW
const short UM_EVENT_INVALIDATE_ALL_CHILDREN = 11 ;

//VALUE_CHANGED --> EVENT_OBJECT_VALUECHANGE
const short UM_EVENT_OBJECT_VALUECHANGE = 12 ;

//NAME_CHANGED --> EVENT_OBJECT_NAMECHANGE
const short UM_EVENT_OBJECT_NAMECHANGE = 13 ;

//DESCRIPTION_CHANGED --> EVENT_OBJECT_DESCRIPTIONCHANGE
const short UM_EVENT_OBJECT_DESCRIPTIONCHANGE = 14 ;

//ACTION_CHANGED --> EVENT_OBJECT_DEFACTIONCHANGE
const short UM_EVENT_OBJECT_DEFACTIONCHANGE = 15 ;

//CARET_CHANGED --> EVENT_OBJECT_LOCATIONCHANGED
const short UM_EVENT_OBJECT_CARETCHANGE = 16 ;

//TEXT_CHANGED --> EVENT_OBJECT_VALUECHANGE
const short UM_EVENT_OBJECT_TEXTCHANGE = 17 ;

//ACTIVE_DESCENDANT_CHANGED --> EVENT_OBJECT_FOCUS
const short UM_EVENT_ACTIVE_DESCENDANT_CHANGED = 18 ;

//BOUNDRECT_CHANGED --> EVENT_OBJECT_LOCATIONCHANGE
const short UM_EVENT_BOUNDRECT_CHANGED = 19 ;

//VISIBLE_DATA_CHANGED --> EVENT_OBJECT_VALUECHANGE
const short UM_EVENT_VISIBLE_DATA_CHANGED = 20 ;

//to enable SHOW on dialogs, windows, frames
const short UM_EVENT_SHOW = 21 ;

const short UM_EVENT_STATE_BUSY = 22 ;

const short UM_EVENT_TABLE_CAPTION_CHANGED = 24;
const short UM_EVENT_TABLE_COLUMN_DESCRIPTION_CHANGED = 25;
const short UM_EVENT_TABLE_COLUMN_HEADER_CHANGED = 26;
const short UM_EVENT_TABLE_MODEL_CHANGED = 27;
const short UM_EVENT_TABLE_ROW_HEADER_CHANGED = 28;
const short UM_EVENT_TABLE_SUMMARY_CHANGED = 29;
const short UM_EVENT_OBJECT_REORDER = 30;
const short UM_EVENT_PAGE_CHANGED =31;
const short UM_EVENT_CHILD_ADDED =32;
const short UM_EVENT_CHILD_REMOVED =33;
const short UM_EVENT_TABLE_ROW_DESCRIPTION_CHANGED =    34;
const short UM_EVENT_SELECTION_CHANGED_ADD = 35 ;
const short UM_EVENT_SELECTION_CHANGED_REMOVE = 36 ;
const short UM_EVENT_SELECTION_CHANGED_WITHIN = 37 ;

//support for PAGE_CHANGED event
const short UM_EVENT_OBJECT_PAGECHANGED = 38;

//to add TEXT_SELECTION_CHANGED event
const short UM_EVENT_TEXT_SELECTION_CHANGED = 39;

//for section change and column chang event
const short UM_EVENT_SECTION_CHANGED = 40;
const short UM_EVENT_COLUMN_CHANGED = 41;

#endif
