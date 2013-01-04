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


#ifndef _SFX2_TIMESTAMP_HXX
#define _SFX2_TIMESTAMP_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif

#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif

#ifndef _DATETIME_HXX
#include <tools/datetime.hxx>
#endif

namespace binfilter {

//______________________________________________________________________________________________________________________________
//  defines
//______________________________________________________________________________________________________________________________

#define TIMESTAMP_MAXLENGTH       		31														/// Max. length of valid timestamp in stream.

//______________________________________________________________________________________________________________________________
//  classes, structs
//______________________________________________________________________________________________________________________________

class TimeStamp
{
	//--------------------------------------------------------------------------------
	//  public method
	//--------------------------------------------------------------------------------
	public:

		//--------------------------------------------------------------------------------
		//  construct / destruct
		//--------------------------------------------------------------------------------

		TimeStamp (													) ;
		TimeStamp ( const String& rName								) ;
		TimeStamp ( const DateTime& rDateTime						) ;
		TimeStamp ( const String& rName, const DateTime& rDateTime	) ;

		//--------------------------------------------------------------------------------
		//  operator methods
		//--------------------------------------------------------------------------------

		const TimeStamp&	operator=	( const TimeStamp& rCopy	)	    ;
		BOOL				operator==	( const TimeStamp& rCompare ) const ;
		int					operator!=	( const TimeStamp& rCompare ) const ;

		//--------------------------------------------------------------------------------
		//  other methods
		//--------------------------------------------------------------------------------

		BOOL				IsValid	(							) const ;
		void				SetInvalid();
		BOOL				Load	( SvStream& rStream 		)       ;
		BOOL				Save 	( SvStream& rStream 		) const ;
		void				SetName ( const String& rName 		)       ;
		void				SetTime ( const DateTime& rDateTime )       ;
		const String&		GetName (							) const ;
		const DateTime&		GetTime (							) const ;

	//--------------------------------------------------------------------------------
	//  protected methods
	//--------------------------------------------------------------------------------
	protected:

	//--------------------------------------------------------------------------------
	//  private methods
	//--------------------------------------------------------------------------------
	private:

		void	impl_adjustName		( String& rName ) ;

		//--------------------------------------------------------------------------------
		//  debug methods
		//--------------------------------------------------------------------------------

		#ifdef DBG_UTIL

		BOOL	impl_debug_checkParameter	( const String& rString								) const ;
		BOOL	impl_debug_checkParameter	( const String& rString, const DateTime& rDateTime	) const ;
		BOOL	impl_debug_checkParameter	( const TimeStamp& rTimeStamp						) const ;
		BOOL	impl_debug_checkParameter	( SvStream& rSvStream								) const ;
		BOOL	impl_debug_checkParameter	( const DateTime& rDateTime							) const ;

		#endif // DBG_UTIL

	//--------------------------------------------------------------------------------
	//  private variables
	//--------------------------------------------------------------------------------
	private:

		String		m_sModifiedByName	;	/// Name of stamp
		DateTime	m_aModifiedDateTime	;	/// Time and date of stamp
} ;

}
#endif // _TIMESTAMP_HXX
