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


#include "globopt.hxx"

#include <stdio.h>

#ifndef INCLUDED_SVTOOLS_SYSLOCALE_HXX
#include <bf_svtools/syslocale.hxx>
#endif
#ifndef _UTL_CONFIGITEM_HXX_
#include <unotools/configitem.hxx>
#endif
namespace binfilter {

// ================================================================================

/*N*/ namespace
/*N*/ {
/*N*/ 
/*N*/ bool IsMetric()
/*N*/ {
/*N*/     SvtSysLocale aSysLocale;
/*N*/     const LocaleDataWrapper* pLocWrapper = aSysLocale.GetLocaleDataPtr();
/*N*/     MeasurementSystem eSys = pLocWrapper->getMeasurementSystemEnum();
/*N*/ 
/*N*/     return ( eSys == MEASURE_METRIC );
/*N*/ }

// ----------------------------------------

/*N*/ class CalcConfigItem : public ::utl::ConfigItem
/*N*/ {
/*N*/ public:
/*N*/     CalcConfigItem() :
/*N*/             ConfigItem( ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Office.Calc/Layout" )))
/*N*/     {}
/*N*/ 
/*N*/     FieldUnit GetMeasureUnit();
		void Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames );
		void Commit();
/*N*/ };

/*N*/ FieldUnit CalcConfigItem::GetMeasureUnit()
/*N*/ {
/*N*/     FieldUnit eResult( FUNIT_CM );
/*N*/ 
/*N*/     ::com::sun::star::uno::Sequence< ::rtl::OUString > aNames( 1 );
/*N*/     if( IsMetric() )
/*N*/         aNames[ 0 ] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Other/MeasureUnit/Metric" ));
/*N*/     else
/*N*/         aNames[ 0 ] = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Other/MeasureUnit/NonMetric" ));
/*N*/ 
/*N*/     ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > aResult(
/*N*/         GetProperties( aNames ));
/*N*/ 
/*N*/     if( aResult[ 0 ].hasValue() )
/*N*/     {
/*N*/         sal_Int32 nValue;
/*N*/         aResult[ 0 ] >>= nValue;
/*N*/         eResult = static_cast< FieldUnit >( nValue );
/*N*/     }
/*N*/ 
/*N*/     return eResult;
/*N*/ }

void CalcConfigItem::Notify( const ::com::sun::star::uno::Sequence< rtl::OUString >& aPropertyNames ) {}
void CalcConfigItem::Commit() {}


}  // anonymous namespace

// ================================================================================

namespace sch
{

namespace util
{

/*N*/ FieldUnit GetMeasureUnit()
/*N*/ {
/*N*/     static CalcConfigItem aCfgItem;
/*N*/     FieldUnit aUnit( aCfgItem.GetMeasureUnit() );
/*N*/ 
/*N*/     return aCfgItem.GetMeasureUnit();
/*N*/ }
 
}  // namespace util
}  // namespace sch
}
