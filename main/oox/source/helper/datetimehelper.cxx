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

#include "oox/helper/datetimehelper.hxx"

namespace oox {

// ============================================================================

using namespace ::com::sun::star::util;

using ::rtl::OUString;

// ============================================================================

bool parseISO8601DateTime( OUString &aValue, DateTime &dateTime )
{
    bool bValid = (aValue.getLength() >= 19) && (aValue[ 4 ] == '-') && (aValue[ 7 ] == '-') &&
        (aValue[ 10 ] == 'T') && (aValue[ 13 ] == ':') && (aValue[ 16 ] == ':');
    if( bValid )
    {
        dateTime.Year    = static_cast< sal_uInt16 >( aValue.copy( 0, 4 ).toInt32() );
        dateTime.Month   = static_cast< sal_uInt16 >( aValue.copy( 5, 2 ).toInt32() );
        dateTime.Day     = static_cast< sal_uInt16 >( aValue.copy( 8, 2 ).toInt32() );
        dateTime.Hours   = static_cast< sal_uInt16 >( aValue.copy( 11, 2 ).toInt32() );
        dateTime.Minutes = static_cast< sal_uInt16 >( aValue.copy( 14, 2 ).toInt32() );
        double seconds = aValue.copy( 17 ).toDouble();
        dateTime.Seconds = static_cast< sal_uInt16 >( floor( seconds ) );
        dateTime.HundredthSeconds = static_cast< sal_uInt16 >( round ( 100 * ( seconds - floor( seconds ) ) ) );
    }
    return bValid;
}

// ============================================================================

} // namespace oox
