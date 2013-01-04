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




#ifndef __SBX_SBX_DEC_HXX
#define __SBX_SBX_DEC_HXX

#ifdef WIN32

#include <bf_svtools/bf_prewin.h>
}	// close extern "C" {

#ifndef __MINGW32__
#include <comutil.h>
#endif
#include <oleauto.h>

extern "C" {	// reopen extern "C" {
#include <bf_svtools/bf_postwin.h>

#endif
#endif

#ifndef _SBXCLASS_HXX
#include "sbx.hxx"
#endif

#include <com/sun/star/bridge/oleautomation/Decimal.hpp>

namespace binfilter {

// Decimal support
// Implementation only for windows

class SbxDecimal
{
	friend void releaseDecimalPtr( SbxDecimal*& rpDecimal );

#ifdef WIN32
	DECIMAL		maDec;
#endif
	INT32		mnRefCount;

public:
#ifdef WIN32
    SbxDecimal();
#endif
	SbxDecimal( const SbxDecimal& rDec );

	~SbxDecimal();

	void addRef( void )
		{ mnRefCount++; }

	void setChar( sal_Unicode val );
	void setByte( BYTE val );
#ifdef WIN32
	void setShort( INT16 val );
	bool setString( String* pString );
	void setUShort( UINT16 val );
#endif
	void setLong( INT32 val );
	void setULong( UINT32 val );
	bool setSingle( float val );
	bool setDouble( double val );
	void setInt( int val );
	void setUInt( unsigned int val );
	void setDecimal( SbxDecimal* pDecimal )
	{
#ifdef WIN32
		if( pDecimal )
			maDec = pDecimal->maDec;
#else
        (void)pDecimal;
#endif
	}

#ifdef WIN32
	bool getShort( INT16& rVal );
	bool getChar( sal_Unicode& rVal );
	bool getLong( INT32& rVal );
	bool getString( String& rString );
	bool getUInt( unsigned int& rVal );
	bool getULong( UINT32& rVal );
	bool getUShort( UINT16& rVal );
#endif
	bool getSingle( float& rVal );
	bool getDouble( double& rVal );

	bool operator -= ( const SbxDecimal &r );
	bool operator += ( const SbxDecimal &r );
	bool operator /= ( const SbxDecimal &r );
	bool operator *= ( const SbxDecimal &r );
	bool neg( void );

	bool isZero( void );

	enum CmpResult { LT, EQ, GT };
	friend CmpResult compare( const SbxDecimal &rLeft, const SbxDecimal &rRight );
};

}
