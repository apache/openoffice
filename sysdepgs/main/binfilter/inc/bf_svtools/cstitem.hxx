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



#ifndef _CSTITEM_HXX
#define _CSTITEM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif

#include <bf_svtools/poolitem.hxx>

class SvStream;

namespace binfilter {

enum CrawlStatus
{
	CSTAT_NEVER_UPD			= 0,	    /* noch nie ueberprueft 						*/
	CSTAT_IN_UPD			= 1,		/* Ueberpruefung laeuft 						*/
	CSTAT_UPD_NEWER			= 2,		/* wurde ueberprueft und ist neuer 				*/
	CSTAT_UPD_NOT_NEWER		= 3,		/* wurde ueberprueft und ist nicht neuer 		*/
	CSTAT_UPD_CANCEL		= 4,		/* Ueberpruefung vom Benutzer abgebrochen 		*/
	CSTAT_ERR_GENERAL		= 5,		/* allgemeiner Fehler							*/
	CSTAT_ERR_NOTEXISTS		= 6,		/* Server existiert nicht						*/
	CSTAT_ERR_NOTREACHED	= 7,		/* Server nicht ereicht							*/
	CSTAT_UPD_IMMEDIATELY	= 8,		/* es wird gleich ueberprueftt 					*/
	CSTAT_ERR_OFFLINE		= 9			/* Ueberpruefung nicht m�glich, da Offline		*/
};

DBG_NAMEEX(SfxCrawlStatusItem)

// class SfxDateTimeRangeItem -------------------------------------------------

class SfxCrawlStatusItem : public SfxPoolItem
{
private:
			CrawlStatus		eStatus;
public:
			TYPEINFO();

			SfxCrawlStatusItem( const SfxCrawlStatusItem& rCpy );
			SfxCrawlStatusItem( USHORT nWhich, CrawlStatus eStat );
			~SfxCrawlStatusItem() { DBG_DTOR(SfxCrawlStatusItem, 0); }

	virtual	int				operator==( const SfxPoolItem& )			const;
    using SfxPoolItem::Compare;
	virtual int				Compare( const SfxPoolItem &rWith )			const;
	virtual SfxPoolItem*	Create( SvStream&, USHORT nItemVersion )	const;
	virtual SvStream&		Store( SvStream&, USHORT nItemVersion )		const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 )				const;

	virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
									SfxMapUnit eCoreMetric,
									SfxMapUnit ePresMetric,
									XubString &rText,
                                    const ::IntlWrapper * pIntlWrapper = 0 )
		const;

	CrawlStatus				GetStatus() const { return eStatus; }
	void					SetStatus(CrawlStatus eNew) { eStatus = eNew; }

	virtual	BOOL PutValue  ( const ::com::sun::star::uno::Any& rVal,
						     BYTE nMemberId = 0 );
	virtual	BOOL QueryValue( ::com::sun::star::uno::Any& rVal,
							 BYTE nMemberId = 0 ) const;
};

}

#endif

