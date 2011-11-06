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



#ifndef _SVTOOLS_CINTITEM_HXX
#define _SVTOOLS_CINTITEM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif

namespace binfilter {

//============================================================================
DBG_NAMEEX_VISIBILITY(CntByteItem, )

class  CntByteItem: public SfxPoolItem
{
	BYTE m_nValue;

public:
	TYPEINFO();

	CntByteItem(USHORT which = 0, BYTE nTheValue = 0):
		SfxPoolItem(which), m_nValue(nTheValue) { DBG_CTOR(CntByteItem, 0); }

	CntByteItem(const CntByteItem & rItem):
		SfxPoolItem(rItem), m_nValue(rItem.m_nValue)
	{ DBG_CTOR(CntByteItem, 0); }

	virtual ~CntByteItem();

	virtual int operator ==(const SfxPoolItem & rItem) const;

    using SfxPoolItem::Compare;
	virtual int Compare(const SfxPoolItem & rWith) const;

	virtual SfxItemPresentation GetPresentation(SfxItemPresentation,
												SfxMapUnit, SfxMapUnit,
												XubString & rText,
                                                const ::IntlWrapper * = 0)
		const;

	virtual	BOOL QueryValue(::com::sun::star::uno::Any& rVal,
							BYTE nMemberId = 0) const;

	virtual	BOOL PutValue(const ::com::sun::star::uno::Any& rVal,
						  BYTE nMemberId = 0);

	virtual SfxPoolItem * Create(SvStream & rStream, USHORT) const;

	virtual SvStream & Store(SvStream & rStream, USHORT) const;

	virtual SfxPoolItem * Clone(SfxItemPool * = 0) const;

	virtual BYTE GetMin() const;

	virtual BYTE GetMax() const;

	virtual SfxFieldUnit GetUnit() const;

	BYTE GetValue() const { return m_nValue; }

	inline void SetValue(BYTE nTheValue);
};

inline void CntByteItem::SetValue(BYTE nTheValue)
{
	DBG_ASSERT(GetRefCount() == 0, "CntByteItem::SetValue(): Pooled item");
	m_nValue = nTheValue;
}

//============================================================================
DBG_NAMEEX_VISIBILITY(CntUInt16Item, )

class  CntUInt16Item: public SfxPoolItem
{
	UINT16 m_nValue;

public:
	TYPEINFO();

	CntUInt16Item(USHORT which = 0, UINT16 nTheValue = 0):
		SfxPoolItem(which), m_nValue(nTheValue)
	{ DBG_CTOR(CntUInt16Item, 0); }

	CntUInt16Item(USHORT which, SvStream & rStream);

	CntUInt16Item(const CntUInt16Item & rItem):
		SfxPoolItem(rItem), m_nValue(rItem.m_nValue)
	{ DBG_CTOR(CntUInt16Item, 0); }

	virtual ~CntUInt16Item();

	virtual int operator ==(const SfxPoolItem & rItem) const;

    using SfxPoolItem::Compare;
	virtual int Compare(const SfxPoolItem & rWith) const;

	virtual SfxItemPresentation GetPresentation(SfxItemPresentation,
												SfxMapUnit, SfxMapUnit,
												XubString & rText,
                                                const ::IntlWrapper * = 0)
		const;

	virtual	BOOL QueryValue(::com::sun::star::uno::Any& rVal,
							BYTE nMemberId = 0) const;

	virtual	BOOL PutValue(const ::com::sun::star::uno::Any& rVal,
						  BYTE nMemberId = 0);

	virtual SfxPoolItem * Create(SvStream & rStream, USHORT) const;

	virtual SvStream & Store(SvStream & rStream, USHORT) const;

	virtual SfxPoolItem * Clone(SfxItemPool * = 0) const;

	virtual UINT16 GetMin() const;

	virtual UINT16 GetMax() const;

	virtual SfxFieldUnit GetUnit() const;

	INT16 GetValue() const { return m_nValue; }

	inline void SetValue(UINT16 nTheValue);
};

inline void CntUInt16Item::SetValue(UINT16 nTheValue)
{
	DBG_ASSERT(GetRefCount() == 0, "CntUInt16Item::SetValue(): Pooled item");
	m_nValue = nTheValue;
}

//============================================================================
DBG_NAMEEX_VISIBILITY(CntInt32Item, )

class  CntInt32Item: public SfxPoolItem
{
	INT32 m_nValue;

public:
	TYPEINFO();

	CntInt32Item(USHORT which = 0, INT32 nTheValue = 0):
		SfxPoolItem(which), m_nValue(nTheValue)
	{ DBG_CTOR(CntInt32Item, 0); }

	CntInt32Item(USHORT which, SvStream & rStream);

	CntInt32Item(const CntInt32Item & rItem):
		SfxPoolItem(rItem), m_nValue(rItem.m_nValue)
	{ DBG_CTOR(CntInt32Item, 0); }

	virtual ~CntInt32Item();

	virtual int operator ==(const SfxPoolItem & rItem) const;

    using SfxPoolItem::Compare;
	virtual int Compare(const SfxPoolItem & rWith) const;

	virtual SfxItemPresentation GetPresentation(SfxItemPresentation,
												SfxMapUnit, SfxMapUnit,
												XubString & rText,
                                                const ::IntlWrapper * = 0)
		const;

	virtual	BOOL QueryValue(::com::sun::star::uno::Any& rVal,
							BYTE nMemberId = 0) const;

	virtual	BOOL PutValue(const ::com::sun::star::uno::Any& rVal,
						  BYTE nMemberId = 0);

	virtual SfxPoolItem * Create(SvStream & rStream, USHORT) const;

	virtual SvStream & Store(SvStream &, USHORT) const;

	virtual SfxPoolItem * Clone(SfxItemPool * = 0) const;

	virtual INT32 GetMin() const;

	virtual INT32 GetMax() const;

	virtual SfxFieldUnit GetUnit() const;

	INT32 GetValue() const { return m_nValue; }

	inline void SetValue(INT32 nTheValue);
};

inline void CntInt32Item::SetValue(INT32 nTheValue)
{
	DBG_ASSERT(GetRefCount() == 0, "CntInt32Item::SetValue(): Pooled item");
	m_nValue = nTheValue;
}

//============================================================================
DBG_NAMEEX_VISIBILITY(CntUInt32Item, )

class  CntUInt32Item: public SfxPoolItem
{
	UINT32 m_nValue;

public:
	TYPEINFO();

	CntUInt32Item(USHORT which = 0, UINT32 nTheValue = 0):
		SfxPoolItem(which), m_nValue(nTheValue)
	{ DBG_CTOR(CntUInt32Item, 0); }

	CntUInt32Item(USHORT nWhich, SvStream & rStream);

	CntUInt32Item(const CntUInt32Item & rItem):
		SfxPoolItem(rItem), m_nValue(rItem.m_nValue)
	{ DBG_CTOR(CntUInt32Item, 0); }

	virtual ~CntUInt32Item();

	virtual int operator ==(const SfxPoolItem & rItem) const;

    using SfxPoolItem::Compare;
	virtual int Compare(const SfxPoolItem & rWith) const;

	virtual SfxItemPresentation GetPresentation(SfxItemPresentation,
												SfxMapUnit, SfxMapUnit,
												XubString & rText,
                                                const ::IntlWrapper * = 0)
		const;

	virtual	BOOL QueryValue(::com::sun::star::uno::Any& rVal,
							BYTE nMemberId = 0) const;

	virtual	BOOL PutValue(const ::com::sun::star::uno::Any& rVal,
						  BYTE nMemberId = 0);

	virtual SfxPoolItem * Create(SvStream & rStream, USHORT) const;

	virtual SvStream & Store(SvStream & rStream, USHORT) const;

	virtual SfxPoolItem * Clone(SfxItemPool * = 0) const;

	virtual UINT32 GetMin() const;

	virtual UINT32 GetMax() const;

	virtual SfxFieldUnit GetUnit() const;

	UINT32 GetValue() const { return m_nValue; }

	inline void SetValue(UINT32 nTheValue);
};

inline void CntUInt32Item::SetValue(UINT32 nTheValue)
{
	DBG_ASSERT(GetRefCount() == 0, "CntUInt32Item::SetValue(): Pooled item");
	m_nValue = nTheValue;
}

}

#endif // _SVTOOLS_CINTITEM_HXX

