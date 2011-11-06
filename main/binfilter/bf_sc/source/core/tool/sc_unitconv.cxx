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



#ifdef PCH
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif


#include "unitconv.hxx"
#include "viewopti.hxx"			//! move ScLinkConfigItem to separate header!
namespace binfilter {

using namespace utl;
using namespace rtl;
using namespace ::com::sun::star::uno;

// --------------------------------------------------------------------

/*N*/ const sal_Unicode cDelim = 0x01;		// Delimiter zwischen From und To


// --- ScUnitConverterData --------------------------------------------

/*N*/ ScUnitConverterData::ScUnitConverterData( const String& rFromUnit,
/*N*/ 			const String& rToUnit, double fVal )
/*N*/ 		:
/*N*/ 		StrData( rFromUnit ),
/*N*/ 		fValue( fVal )
/*N*/ {
/*N*/ 	String aTmp;
/*N*/ 	ScUnitConverterData::BuildIndexString( aTmp, rFromUnit, rToUnit );
/*N*/ 	SetString( aTmp );
/*N*/ }


/*N*/ ScUnitConverterData::ScUnitConverterData( const ScUnitConverterData& r )
/*N*/ 		:
/*N*/ 		StrData( r ),
/*N*/ 		fValue( r.fValue )
/*N*/ {
/*N*/ }


/*N*/ DataObject* ScUnitConverterData::Clone() const
/*N*/ {
/*N*/ 	return new ScUnitConverterData( *this );
/*N*/ }


// static
/*N*/ void ScUnitConverterData::BuildIndexString( String& rStr,
/*N*/ 			const String& rFromUnit, const String& rToUnit )
/*N*/ {
/*N*/ #if 1
/*N*/ // case sensitive
/*N*/ 	rStr = rFromUnit;
/*N*/ 	rStr += cDelim;
/*N*/ 	rStr += rToUnit;
/*N*/ #else
/*N*/ // not case sensitive
/*N*/ 	rStr = rFromUnit;
/*N*/ 	String aTo( rToUnit );
/*N*/ 	ScGlobal::pCharClass->toUpper( rStr );
/*N*/ 	ScGlobal::pCharClass->toUpper( aTo );
/*N*/ 	rStr += cDelim;
/*N*/ 	rStr += aTo;
/*N*/ #endif
/*N*/ }


// --- ScUnitConverter ------------------------------------------------

/*N*/ #define CFGPATH_UNIT		"Office.Calc/UnitConversion"
/*N*/ #define CFGSTR_UNIT_FROM	"FromUnit"
/*N*/ #define CFGSTR_UNIT_TO		"ToUnit"
/*N*/ #define CFGSTR_UNIT_FACTOR	"Factor"

/*N*/ ScUnitConverter::ScUnitConverter( USHORT nInit, USHORT nDelta ) :
/*N*/ 		StrCollection( nInit, nDelta, FALSE )
/*N*/ {
/*N*/ 	//	read from configuration - "convert.ini" is no longer used
/*N*/ 	//!	config item as member to allow change of values
/*N*/ 
/*N*/ 	ScLinkConfigItem aConfigItem( OUString::createFromAscii( CFGPATH_UNIT ) );
/*N*/ 
/*N*/ 	// empty node name -> use the config item's path itself
/*N*/     OUString aEmptyString;
/*N*/ 	Sequence<OUString> aNodeNames = aConfigItem.GetNodeNames( aEmptyString );
/*N*/ 
/*N*/ 	long nNodeCount = aNodeNames.getLength();
/*N*/ 	if ( nNodeCount )
/*N*/ 	{
/*N*/ 		const OUString* pNodeArray = aNodeNames.getConstArray();
/*N*/ 		Sequence<OUString> aValNames( nNodeCount * 3 );
/*N*/ 		OUString* pValNameArray = aValNames.getArray();
/*N*/ 		const OUString sSlash('/');
/*N*/ 
/*N*/ 		long nIndex = 0;
/*N*/ 		for (long i=0; i<nNodeCount; i++)
/*N*/ 		{
/*N*/ 			OUString sPrefix = pNodeArray[i];
/*N*/ 			sPrefix += sSlash;
/*N*/ 
/*N*/ 			pValNameArray[nIndex] = sPrefix;
/*N*/ 			pValNameArray[nIndex++] += OUString::createFromAscii( CFGSTR_UNIT_FROM );
/*N*/ 			pValNameArray[nIndex] = sPrefix;
/*N*/ 			pValNameArray[nIndex++] += OUString::createFromAscii( CFGSTR_UNIT_TO );
/*N*/ 			pValNameArray[nIndex] = sPrefix;
/*N*/ 			pValNameArray[nIndex++] += OUString::createFromAscii( CFGSTR_UNIT_FACTOR );
/*N*/ 		}
/*N*/ 
/*N*/ 		Sequence<Any> aProperties = aConfigItem.GetProperties(aValNames);
/*N*/ 
/*N*/ 		if (aProperties.getLength() == aValNames.getLength())
/*N*/ 		{
/*N*/ 			const Any* pProperties = aProperties.getConstArray();
/*N*/ 
/*N*/ 			OUString sFromUnit;
/*N*/ 			OUString sToUnit;
/*N*/ 			double fFactor;
/*N*/ 
/*N*/ 			nIndex = 0;
/*N*/ 			for (long i=0; i<nNodeCount; i++)
/*N*/ 			{
/*N*/ 				pProperties[nIndex++] >>= sFromUnit;
/*N*/ 				pProperties[nIndex++] >>= sToUnit;
/*N*/ 				pProperties[nIndex++] >>= fFactor;
/*N*/ 
/*N*/ 				ScUnitConverterData* pNew = new ScUnitConverterData( sFromUnit, sToUnit, fFactor );
/*N*/ 				if ( !Insert( pNew ) )
/*N*/ 					delete pNew;
/*N*/ 			}
/*N*/ 		}
/*N*/ 	}
/*N*/ }

/*N*/ BOOL ScUnitConverter::GetValue( double& fValue, const String& rFromUnit,
/*N*/ 				const String& rToUnit ) const
/*N*/ {
/*N*/ 	ScUnitConverterData aSearch( rFromUnit, rToUnit );
/*N*/ 	USHORT nIndex;
/*N*/ 	if ( Search( &aSearch, nIndex ) )
/*N*/ 	{
/*N*/ 		fValue = ((const ScUnitConverterData*)(At( nIndex )))->GetValue();
/*N*/ 		return TRUE;
/*N*/ 	}
/*N*/ 	fValue = 1.0;
/*N*/ 	return FALSE;
/*N*/ }


}
