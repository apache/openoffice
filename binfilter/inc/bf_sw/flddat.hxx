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



#ifndef _FLDDAT_HXX
#define _FLDDAT_HXX

#include <bf_svtools/bf_solar.h>


#include "fldbas.hxx"
class DateTime; 
namespace binfilter {



/*--------------------------------------------------------------------
	Beschreibung: Formate
 --------------------------------------------------------------------*/

enum SwDateFormat
{
	DF_BEGIN,
	//neue Formate:
	DFF_SSYS = DF_BEGIN,
	DFF_LSYS ,
	DFF_DMY ,
	DFF_DMYY ,
	DFF_DMMY ,
	DFF_DMMYY,
	DFF_DMMMY ,
	DFF_DMMMYY,
	DFF_DDMMY ,
	DFF_DDMMMY ,
	DFF_DDMMMYY ,
	DFF_DDDMMMY ,
	DFF_DDDMMMYY ,
	DFF_MY ,
	DFF_MD ,
	DFF_YMD ,
	DFF_YYMD ,
	DF_END ,
//	Kompatibilitaet:
	DF_SSYS 	= DFF_SSYS,
	DF_LSYS 	= DFF_LSYS,
	DF_SHORT 	= DFF_DMY,
	DF_SCENT 	= DFF_DMYY,
	DF_LMON		= DFF_DMMYY,
	DF_LMONTH	= DFF_DMMMYY,
	DF_LDAYMON	= DFF_DDMMMYY,
	DF_LDAYMONTH= DFF_DDDMMMYY
};

enum SwTimeFormat
{
	TF_BEGIN,
	TF_SYSTEM=TF_BEGIN,
	TF_SSMM_24,
	TF_SSMM_12,
	TF_END
};

enum SwTimeSubFormat
{
	TIME_FIX,
	TIME_VAR
};

enum SwDateSubFormat
{
	DATE_FIX,
	DATE_VAR
};

/*--------------------------------------------------------------------
	Beschreibung: Datum/Uhrzeitfeld
 --------------------------------------------------------------------*/

class SwDateTimeFieldType : public SwValueFieldType
{
public:
		SwDateTimeFieldType(SwDoc* pDoc);

		virtual SwFieldType*    Copy() const;
};

/*--------------------------------------------------------------------
	Beschreibung: Datum/Uhrzeitfeld
 --------------------------------------------------------------------*/

class SwDateTimeField : public SwValueField
{
		USHORT				nSubType;
		long				nOffset;	// Offset in Minuten

public:
		SwDateTimeField(SwDateTimeFieldType* pType, USHORT nSubType = DATEFLD,
					ULONG nFmt = 0, USHORT nLng = 0);

		virtual USHORT			GetSubType() const;
/*N*/ 		virtual void			SetSubType(USHORT nSub); //SW40.SDW 

		virtual double			GetValue() const;


		inline BOOL				IsDate() const
								{ return (nSubType & DATEFLD) != 0; }

		inline void				SetOffset(long nMinutes)	{ nOffset = nMinutes; }
		inline long				GetOffset() const			{ return nOffset; }

        Date                    GetDate(BOOL bUseOffset = FALSE) const;
        Time                    GetTime(BOOL bUseOffset = FALSE) const;
        void                    SetDateTime(const DateTime& rDT);
        static double           GetDateTime(SwDoc* pDoc, const DateTime& rDT);

		virtual String 			Expand() const;
		virtual SwField* 		Copy() const;

	virtual	BOOL       	QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMId ) const;
	virtual	BOOL		PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMId );
};

} //namespace binfilter
#endif // _FLDDAT_HXX
