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



#ifndef _SV_LSTBOX_H
#define _SV_LSTBOX_H

#define LISTBOX_APPEND			  	((sal_uInt16)0xFFFF)
#define LISTBOX_ENTRY_NOTFOUND	  	((sal_uInt16)0xFFFF)
#define LISTBOX_ERROR			  	((sal_uInt16)0xFFFF)

#define LISTBOX_USERDRAW_SELECTED	((sal_uInt16)0x0001)

// --------------------------------------------------------------------

// the following defines can be used for the SetEntryFlags()
// and GetEntryFlags() methods

// !! Do not use these flags for user data as they are reserved      !!
// !! to change the internal behavior of the ListBox implementation !!
// !! for specific entries.											 !!

/** this flag disables a selection of an entry completely. It is not
	possible to select such entries either from the user interface
	nor from the ListBox methods. Cursor traveling is handled correctly.
	This flag can be used to add titles to a ListBox.
*/
#define LISTBOX_ENTRY_FLAG_DISABLE_SELECTION		0x0000001

/** this flag can be used to make an entry multine capable
    A normal entry is single line and will therefore be clipped
    at the right listbox border. Setting this flag enables
    word breaks for the entry text.
*/
#define LISTBOX_ENTRY_FLAG_MULTILINE                0x0000002

/** this flags lets the item be drawn disabled (e.g. in grey text)
    usage only guaranteed with LISTBOX_ENTRY_FLAG_DISABLE_SELECTION
*/
#define LISTBOX_ENTRY_FLAG_DRAW_DISABLED            0x0000004

#endif	// _SV_LSTBOX_H
