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



#ifndef _SDASAITM_HXX
#define _SDASAITM_HXX

#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _LIST_HXX
#include <tools/list.hxx>
#endif
namespace binfilter {

class SdrAutoShapeAdjustmentValue
{
	sal_uInt32	nValue;

	friend class SdrAutoShapeAdjustmentItem;

	public :

		void		SetValue( sal_Int32 nVal ) { nValue = nVal; };
		sal_Int32	GetValue() const { return nValue; };

};

class SdrAutoShapeAdjustmentItem : public SfxPoolItem
{
			List	aAdjustmentValueList;

	public :

			TYPEINFO();
			SdrAutoShapeAdjustmentItem();
			SdrAutoShapeAdjustmentItem( SvStream& rIn, sal_uInt16 nVersion );
			~SdrAutoShapeAdjustmentItem();

			virtual int					operator==( const SfxPoolItem& ) const;
			virtual SfxPoolItem*		Create( SvStream&, sal_uInt16 nItem ) const;
			virtual SvStream&			Store( SvStream&, sal_uInt16 nVersion ) const;
			virtual SfxPoolItem*		Clone( SfxItemPool* pPool = NULL ) const;
			virtual	sal_uInt16			GetVersion( sal_uInt16 nFileFormatVersion ) const;


#ifdef SDR_ISPOOLABLE
			virtual int IsPoolable() const;
#endif

			sal_uInt32							GetCount() const { return aAdjustmentValueList.Count(); };
};

}//end of namespace binfilter
#endif

