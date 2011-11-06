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


#ifndef _SVX_WRITINGMODEITEM_HXX
#define _SVX_WRITINGMODEITEM_HXX

// include ---------------------------------------------------------------

#ifndef _COM_SUN_STAR_TEXT_WRITINGMODE_HPP_ 
#include <com/sun/star/text/WritingMode.hpp>
#endif

#ifndef _SFXINTITEM_HXX
#include <bf_svtools/intitem.hxx>
#endif

#ifndef _SVDDEF_HXX
#include <bf_svx/svddef.hxx>
#endif
namespace binfilter {

// class SvxWritingModeItem ----------------------------------------------

class SvxWritingModeItem : public SfxUInt16Item
{
public:
	TYPEINFO();

	SvxWritingModeItem( ::com::sun::star::text::WritingMode eValue = ::com::sun::star::text::WritingMode_LR_TB, USHORT nWhich = SDRATTR_TEXTDIRECTION );
	virtual ~SvxWritingModeItem();

	SvxWritingModeItem& operator=( const SvxWritingModeItem& rItem );

	virtual SfxPoolItem*	Clone( SfxItemPool *pPool = 0 ) const;
	virtual USHORT			GetVersion( USHORT nFileVersion ) const;
	virtual int 			operator==( const SfxPoolItem& ) const;


	virtual sal_Bool QueryValue( ::com::sun::star::uno::Any& rVal,
								BYTE nMemberId ) const;
};

}//end of namespace binfilter
#endif // #ifndef _SVX_WRITINGMODEITEM_HXX

