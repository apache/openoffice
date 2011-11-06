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


#ifndef _DOCINF_HXX
#define _DOCINF_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _LIST_HXX //autogen
#include <tools/list.hxx>
#endif

#ifndef _DATETIME_HXX //autogen
#include <tools/datetime.hxx>
#endif

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
class Date; 
class Time; 
class DateTime; 
class SvStream; 

namespace binfilter {


// EXTERN ----------------------------------------------------------------
struct DataTupel;

// ENUM -----------------------------------------------------------------
enum InfoType
{
	INFOTYPE_DATE, INFOTYPE_TIME, INFOTYPE_DATETIME,
	INFOTYPE_MODIFYINFO,
	INFOTYPE_STRING, INFOTYPE_STRINGLIST,
	INFOTYPE_BOOL, INFOTYPE_LONG,
	INFOTYPE_TEMPLATE
};


// CLASS -----------------------------------------------------------------
DECLARE_LIST( DataTupelList, DataTupel* )

// CLASS -----------------------------------------------------------------
class VarDocInfos
{
public:
						VarDocInfos();
						~VarDocInfos();

			void		AddDate( String& rName, Date& rDate );
			void		AddTime( String& rName, Time& rTime );
			void		AddDateTime( String& rName, DateTime& rDateTime );

			void		AddString( String& rName, String& rString );
			void		AddStringList( String& rName, String& rStringList );
			void		AddBool( String& rName, BOOL bBool );
			void		AddLong( String& rName, long lLong );

			USHORT		GetCount();
			InfoType	GetType( USHORT nPos );
	const	String&		GetName( USHORT nPos );

	const	Date&		GetDate( USHORT nPos );
	const	Time&		GetTime( USHORT nPos );
	const	DateTime&	GetDateTime( USHORT nPos );

	const	String&		GetString( USHORT nPos );
	const	String&		GetStringList( USHORT nPos );
			BOOL		GetBool( USHORT nPos );
			long		GetLong( USHORT nPos );

friend      SvStream&    operator<<( SvStream&, VarDocInfos& ); //$ ostream
friend      SvStream&    operator>>( SvStream&, VarDocInfos& ); //$ istream

private:
	DataTupelList*		pInfoList;
};

// CLASS -----------------------------------------------------------------
struct ModifyInfo {
	DateTime	aModifiedDateTime;
	String		aModifiedById;
	String		aModifiedByName;

	friend      SvStream&    operator<<( SvStream&, ModifyInfo& ); //$ ostream
	friend      SvStream&    operator>>( SvStream&, ModifyInfo& ); //$ istream
};

// CLASS -----------------------------------------------------------------
class FixDocInfos
{
public:
						FixDocInfos();
						~FixDocInfos();

			void		SetTitle( String& rTitle );
	const	String&		GetTitle() const;

	void				SetCreated( ModifyInfo& rInfo );
	const	ModifyInfo&	GetCreated() const;

			void		SetLastChanged( ModifyInfo& rInfo );
	const	ModifyInfo&	GetLastChanged() const;

			void		SetLastPrinted( ModifyInfo& rInfo );
	const	ModifyInfo&	GetLastPrinted() const;

			void		SetTotalEditingTime( Time& rTime );
	const	Time&		GetTotalEditingTime() const;

friend      SvStream&    operator<<( SvStream&, FixDocInfos& ); //$ ostream
friend      SvStream&    operator>>( SvStream&, FixDocInfos& ); //$ istream

private:
	String			aTitle;

	ModifyInfo 		aCreated;
	ModifyInfo 		aLastChanged;
	ModifyInfo 		aLastPrinted;
	Time			aTotalEditingTime;
};

// CLASS -----------------------------------------------------------------
struct TemplateInfo {
	String		aTitle;
	ULONG		lTemplateId;

				TemplateInfo() {}
				TemplateInfo( String& rName, ULONG lId ) :
					aTitle( rName ),
					lTemplateId( lId ) {}

	friend      SvStream&    operator<<( SvStream&, TemplateInfo& ); //$ ostream
	friend      SvStream&    operator>>( SvStream&, TemplateInfo& ); //$ istream
};

// CLASS -----------------------------------------------------------------
DECLARE_LIST( TemplateInfos, TemplateInfo* )

SvStream&    operator<<( SvStream&, TemplateInfos& ); //$ ostream
SvStream&    operator>>( SvStream&, TemplateInfos& ); //$ istream

} //namespace binfilter
#endif // _DOCINF_HXX
