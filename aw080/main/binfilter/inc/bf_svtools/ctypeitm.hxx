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


#ifndef _SVTOOLS_CTYPEITM_HXX
#define _SVTOOLS_CTYPEITM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _INETTYPE_HXX
#include <bf_svtools/inettype.hxx>
#endif
#ifndef _SVTOOLS_CUSTRITM_HXX
#include <bf_svtools/custritm.hxx>
#endif

namespace binfilter {

//=========================================================================

class CntContentTypeItem : public CntUnencodedStringItem
{
private:
	INetContentType _eType;
	XubString		_aPresentation;

public:
	TYPEINFO();

	CntContentTypeItem();
	CntContentTypeItem(	USHORT nWhich, const XubString& rType );
	CntContentTypeItem( const CntContentTypeItem& rOrig );

	virtual SfxPoolItem* Create( SvStream& rStream,
								 USHORT nItemVersion ) const;
	virtual SvStream & Store(SvStream & rStream, USHORT) const;

	virtual int          operator==( const SfxPoolItem& rOrig ) const;

	virtual USHORT GetVersion(USHORT) const;

	virtual SfxPoolItem* Clone( SfxItemPool *pPool = NULL ) const;

	void SetValue( const XubString& rNewVal );

    using SfxPoolItem::Compare;
    virtual int Compare( const SfxPoolItem &rWith, const ::IntlWrapper& rIntlWrapper ) const;

	virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
												 SfxMapUnit eCoreMetric,
												 SfxMapUnit ePresMetric,
												 XubString &rText,
                                                 const ::IntlWrapper* pIntlWrapper = 0 ) const;

	virtual	BOOL QueryValue( ::com::sun::star::uno::Any& rVal,
							 BYTE nMemberId = 0 ) const;
	virtual	BOOL PutValue  ( const ::com::sun::star::uno::Any& rVal,
						     BYTE nMemberId = 0);

	INetContentType GetEnumValue() const;

	void  			  	  SetValue( const INetContentType eType );
};

}

#endif /* !_SVTOOLS_CTYPEITM_HXX */

