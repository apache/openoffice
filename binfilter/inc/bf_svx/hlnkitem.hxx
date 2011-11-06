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


#ifndef _SVX_HLNKITEM_HXX
#define _SVX_HLNKITEM_HXX

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _SFXSIDS_HRC
#include <bf_sfx2/sfxsids.hrc>
#endif

#ifndef _SFXMACITEM_HXX
#include <bf_svtools/macitem.hxx>
#endif
namespace binfilter {

#define HYPERDLG_EVENT_MOUSEOVER_OBJECT		0x0001
#define HYPERDLG_EVENT_MOUSECLICK_OBJECT	0x0002
#define HYPERDLG_EVENT_MOUSEOUT_OBJECT		0x0004

enum SvxLinkInsertMode
{
	HLINK_DEFAULT,
	HLINK_FIELD,
	HLINK_BUTTON,
	HLINK_HTMLMODE = 0x0080
};

class SvxHyperlinkItem : public SfxPoolItem
{
public:
	TYPEINFO();
	virtual int 			 operator==( const SfxPoolItem& ) const;
	virtual SfxPoolItem*	 Clone( SfxItemPool *pPool = 0 ) const;
};

}//end of namespace binfilter
#endif

