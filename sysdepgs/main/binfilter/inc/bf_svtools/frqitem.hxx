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



#ifndef _FRQITEM_HXX
#define _FRQITEM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _RTTI_HXX
#include <tools/rtti.hxx>
#endif

#ifndef _TOOLS_TIME_HXX //autogen
#include <tools/time.hxx>
#endif

#ifndef _DATETIME_HXX //autogen
#include <tools/datetime.hxx>
#endif

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

#include <bf_svtools/poolitem.hxx>

class SvStream;

namespace binfilter
{

DBG_NAMEEX(SfxFrequencyItem)

// class SfxFrequencyItem -------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/*
FRQ_DAILY
	* jeden X'ten Tag
	nInterval1 = 1 .. 999

	* jeden Wochentag
	nInterval1 = USHRT_MAX

FRQ_WEEKLY
	+ wiederhole jede X'te Woche
	nInterval1 = 1 .. 99

	+ an SU, MO, TU, WE, TH, FR, SA
	nInterval2 = WD_SUNDAY | WD_MONDAY | WD_TUESDAY | WD_WEDNESDAY |
				 WD_THURSDAY | WD_FRIDAY | WD_SATURDAY

FRQ_MONTHLY_DAILY
	* jeden X'ten Tag von jedem X'ten Monat
	nInterval1 = 1 .. 31
	nInterval2 = 1 .. 6

FRQ_MONTHLY_LOGIC
	* jeden ersten, zweiten, dritten, vierten oder letzten Wochentag jeden X'ten Monats
	nInterval1 = 0 .. 4
	nInterval2 = WD_SUNDAY | WD_MONDAY | WD_TUESDAY | WD_WEDNESDAY |
				 WD_THURSDAY | WD_FRIDAY | WD_SATURDAY
	nInterval3 = 1 .. 6

===============================================================================

FRQ_TIME_AT
	* Update um Uhrzeit
	nTime1 = 00:00:00 - 24:00:00

FRQ_TIME_REPEAT
	* Wiederhole alle X Stunden
	nTInterval1 = 1 .. 8
		+ zwischen Uhrzeit 1 und 2
		nTime1 = 00:00:00 - 24:00:00
		nTime2 = 00:00:00 - 24:00:00

*/
///////////////////////////////////////////////////////////////////////////////

enum FrequencyMode
{
	FRQ_DAILY				= 1,
	FRQ_WEEKLY				= 2,
	FRQ_MONTHLY_DAILY		= 3,
	FRQ_MONTHLY_LOGIC		= 4
};

enum FrequencyTimeMode
{
	FRQ_TIME_AT				= 1,
	FRQ_TIME_REPEAT			= 2,
	FRQ_TIME_REPEAT_RANGE	= 3
};

#define WD_SUNDAY		0x0001
#define WD_MONDAY		0x0002
#define WD_TUESDAY		0x0004
#define WD_WEDNESDAY	0x0008
#define WD_THURSDAY		0x0010
#define WD_FRIDAY		0x0020
#define WD_SATURDAY		0x0040

class SfxFrequencyItem : public SfxPoolItem
{
private:
			FrequencyMode		eFrqMode;
			FrequencyTimeMode	eFrqTimeMode;

			USHORT				nDInterval1;
			USHORT				nDInterval2;
			USHORT				nDInterval3;

			USHORT				nTInterval1;
			Time				aTime1;
			Time				aTime2;

			BOOL				bMissingDate;
			DateTime			aMissingDate;

public:
			TYPEINFO();

			SfxFrequencyItem( const SfxFrequencyItem& rCpy );
			SfxFrequencyItem( USHORT nWhich, FrequencyMode eMode, FrequencyTimeMode eTMode,
							  USHORT nDI1, USHORT nDI2, USHORT nDI3, USHORT nTI1,
							  const Time& rT1, const Time& rT2 );
			~SfxFrequencyItem() { DBG_DTOR(SfxFrequencyItem, 0); }

	virtual	int				operator==( const SfxPoolItem& )			const;
    using SfxPoolItem::Compare;
	virtual int				Compare( const SfxPoolItem &rWith )			const;
	virtual SfxPoolItem*	Create( SvStream&, USHORT nItemVersion )	const;
	virtual SvStream&		Store( SvStream&, USHORT nItemVersion )		const;
	virtual SfxPoolItem*	Clone( SfxItemPool* pPool = 0 )				const;
	virtual SfxItemPresentation GetPresentation( SfxItemPresentation ePres,
								SfxMapUnit eCoreMetric, SfxMapUnit ePresMetric, XubString &rText,
                                const ::IntlWrapper * = 0 ) const;

	BOOL					HasMissingDate() const 				{ return bMissingDate; }
	DateTime				GetMissingDate() const 				{ return aMissingDate; }

	FrequencyMode			GetFrequencyMode() const 			{ return eFrqMode; }
	FrequencyTimeMode		GetFrequencyTimeMode() const 		{ return eFrqTimeMode; }
	USHORT					GetDInterval_1() const				{ return nDInterval1; }
	USHORT					GetDInterval_2() const				{ return nDInterval2; }
	USHORT					GetDInterval_3() const				{ return nDInterval3; }
	USHORT					GetTInterval_1() const				{ return nTInterval1; }
	const Time&				GetTime_1() const 					{ return aTime1; }
	const Time&				GetTime_2() const 					{ return aTime2; }

	void					SetFrequencyMode(FrequencyMode eNew)	{ eFrqMode = eNew; }
	void					SetFrequencyTimeMode(FrequencyTimeMode eNew){ eFrqTimeMode = eNew; }
	void					SetDInterval_1(USHORT nNew)			 	{ nDInterval1 = nNew; }
	void					SetDInterval_2(USHORT nNew)			 	{ nDInterval2 = nNew; }
	void					SetDInterval_3(USHORT nNew)			 	{ nDInterval3 = nNew; }
	void					SetTInterval_1(USHORT nNew)			 	{ nTInterval1 = nNew; }
	void					SetTime_1(const Time& rNew)				{ aTime1 = rNew; }
	void					SetTime_2(const Time& rNew)				{ aTime2 = rNew; }
};

}

#endif

