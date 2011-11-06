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


#ifndef _AEITEM_HXX
#define _AEITEM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _SFXENUMITEM_HXX
#include <bf_svtools/eitem.hxx>
#endif

namespace binfilter {

class SfxAllEnumValueArr;
class SvUShorts;

class  SfxAllEnumItem: public SfxEnumItem
{
	SfxAllEnumValueArr*		pValues;
	SvUShorts*				pDisabledValues;

protected:
	USHORT					_GetPosByValue( USHORT nValue ) const;

public:
	TYPEINFO();
							SfxAllEnumItem();
                            SfxAllEnumItem( USHORT nWhich);
							SfxAllEnumItem( USHORT nWhich, SvStream &rStream );
							SfxAllEnumItem( const SfxAllEnumItem & );
							~SfxAllEnumItem();

	void                    InsertValue( USHORT nValue );

	USHORT					GetPosByValue( USHORT nValue ) const;

	virtual USHORT			GetValueCount() const;
	virtual USHORT			GetValueByPos( USHORT nPos ) const;
	virtual XubString		GetValueTextByPos( USHORT nPos ) const;
	virtual SfxPoolItem*	Clone( SfxItemPool *pPool = 0 ) const;
	virtual SfxPoolItem*	Create(SvStream &, USHORT nVersion) const;
	virtual BOOL			IsEnabled( USHORT ) const;
};

}

#endif
