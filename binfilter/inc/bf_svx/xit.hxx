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



#ifndef _SVX_XIT_HXX
#define _SVX_XIT_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXSTRITEM_HXX //autogen
#include <bf_svtools/stritem.hxx>
#endif
namespace binfilter {

	class SfxItemPool;

/************************************************************************/

extern String aNameOrIndexEmptyString;

/************************************************************************/

class XColorTable;
class XDashTable;
class XLineEndTable;
class XHatchTable;
class XBitmapTable;
class XGradientTable;
class NameOrIndex;
class XPropertyList;

typedef BOOL (*SvxCompareValueFunc)( const NameOrIndex* p1, const NameOrIndex* p2 );

//-------------------
// class NameOrIndex
//-------------------
class NameOrIndex : public SfxStringItem
{
	long    nPalIndex;

protected:
	void    Detach()    { nPalIndex = -1; }

public:
			TYPEINFO();
			NameOrIndex() { nPalIndex = -1; }
			NameOrIndex(USHORT nWhich, long nIndex);
			NameOrIndex(USHORT nWhich,
						const String& rName= aNameOrIndexEmptyString);
			NameOrIndex(USHORT nWhich, SvStream& rIn);
			NameOrIndex(const NameOrIndex& rNameOrIndex);
		   ~NameOrIndex() {};

	virtual int          operator==(const SfxPoolItem& rItem) const;
	virtual SvStream&    Store(SvStream& rOut, USHORT nItemVersion ) const;

			String       GetName() const              { return GetValue();   }
			void         SetName(const String& rName) { SetValue(rName);     }
			long         GetIndex() const             { return nPalIndex;    }
			void         SetIndex(long nIndex)        { nPalIndex = nIndex;  }
			BOOL         IsIndex() const          { return (nPalIndex >= 0); }

	/** this static checks if the given NameOrIndex item has a unique name for its value.
		The returned String is a unique name for an item with this value in both given pools.
		Argument pPool2 can be null.
		If returned string equals NameOrIndex->GetName(), the name was already unique.
	*/
	static String CheckNamedItem( const NameOrIndex* pCheckItem, const sal_uInt16 nWhich, const SfxItemPool* pPool1, const SfxItemPool* pPool2, SvxCompareValueFunc pCompareValueFunc, USHORT nPrefixResId, XPropertyList* pDefaults = NULL );
};

}//end of namespace binfilter
#endif
