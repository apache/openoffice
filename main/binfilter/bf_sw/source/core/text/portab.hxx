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


#ifndef _PORTAB_HXX
#define _PORTAB_HXX

#include "porglue.hxx"
namespace binfilter {

/*************************************************************************
 *                      class SwTabPortion
 *************************************************************************/

class SwTabPortion : public SwFixPortion
{
	const KSHORT nTabPos;
	const xub_Unicode cFill;

	// Das Format() verzweigt entweder in Pre- oder PostFormat()
	sal_Bool PreFormat( SwTxtFormatInfo &rInf );
public:
	SwTabPortion( const KSHORT nTabPos, const xub_Unicode cFill = '\0' );
	virtual sal_Bool Format( SwTxtFormatInfo &rInf );
	virtual void FormatEOL( SwTxtFormatInfo &rInf );
	sal_Bool PostFormat( SwTxtFormatInfo &rInf );
	inline  sal_Bool IsFilled() const { return 0 != cFill; }
	inline  KSHORT GetTabPos() const { return nTabPos; }

    // Accessibility: pass information about this portion to the PortionHandler

	OUTPUT_OPERATOR
};

/*************************************************************************
 *                  class SwTabLeftPortion
 *************************************************************************/

class SwTabLeftPortion : public SwTabPortion
{
public:
	inline SwTabLeftPortion( const KSHORT nTabPos, const xub_Unicode cFill='\0' )
		 : SwTabPortion( nTabPos, cFill )
	{ SetWhichPor( POR_TABLEFT ); }
	OUTPUT_OPERATOR
};

/*************************************************************************
 *                  class SwTabRightPortion
 *************************************************************************/

class SwTabRightPortion : public SwTabPortion
{
public:
	inline SwTabRightPortion( const KSHORT nTabPos, const xub_Unicode cFill='\0' )
		 : SwTabPortion( nTabPos, cFill )
	{ SetWhichPor( POR_TABRIGHT ); }
	OUTPUT_OPERATOR
};

/*************************************************************************
 *                  class SwTabCenterPortion
 *************************************************************************/

class SwTabCenterPortion : public SwTabPortion
{
public:
	inline SwTabCenterPortion( const KSHORT nTabPos, const xub_Unicode cFill='\0' )
		 : SwTabPortion( nTabPos, cFill )
	{ SetWhichPor( POR_TABCENTER );	}
	OUTPUT_OPERATOR
};

/*************************************************************************
 *                  class SwTabDecimalPortion
 *************************************************************************/

class SwTabDecimalPortion : public SwTabPortion
{
	const xub_Unicode cTab;
public:
	inline SwTabDecimalPortion( const KSHORT nTabPos, const xub_Unicode cTab,
								const xub_Unicode cFill = '\0' )
		 : SwTabPortion( nTabPos, cFill ), cTab(cTab)
	{ SetWhichPor( POR_TABDECIMAL ); }
	inline xub_Unicode GetTabDecimal() const { return cTab; }
	OUTPUT_OPERATOR
};

CLASSIO( SwTabPortion )
CLASSIO( SwTabLeftPortion )
CLASSIO( SwTabRightPortion )
CLASSIO( SwTabCenterPortion )
CLASSIO( SwTabDecimalPortion )


} //namespace binfilter
#endif
