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


#ifndef _SVX_SCRIPTTYPEITEM_HXX
#define _SVX_SCRIPTTYPEITEM_HXX

// include ---------------------------------------------------------------

#ifndef _SFXINTITEM_HXX
#include <bf_svtools/intitem.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif
#ifndef _SVTOOLS_LANGUAGEOPTIONS_HXX
#include <bf_svtools/languageoptions.hxx>
#endif
namespace binfilter {

// class SvxScriptTypeItem ----------------------------------------------

/* [Description]

		This item describe  the scriptype of the selected text and is only
	used for the user interface.
*/

USHORT GetI18NScriptTypeOfLanguage( USHORT nLang );
USHORT GetItemScriptType( short nI18NType );
short  GetI18NScriptType( USHORT nItemType );


class SvxScriptSetItem : public SfxSetItem
{
public:
	TYPEINFO();

	SvxScriptSetItem( USHORT nSlotId, SfxItemPool& rPool );

	virtual SfxPoolItem* Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem* Create( SvStream &, USHORT nVersion ) const;
};

}//end of namespace binfilter
#endif

