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


#ifndef _SVX_ITEMTYPE_HXX
#define _SVX_ITEMTYPE_HXX

#include <bf_svtools/bf_solar.h>

// include ---------------------------------------------------------------

#ifndef _SVXITEMS_HRC
#include <bf_svx/svxitems.hrc>
#endif

// forward ---------------------------------------------------------------

#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _TOOLS_RESID_HXX //autogen
#include <tools/resid.hxx>
#endif
#ifndef _BIGINT_HXX //autogen
#include <tools/bigint.hxx>
#endif
#ifndef _SVX_DIALMGR_HXX //autogen
#include <bf_svx/dialmgr.hxx>
#endif
#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _SHL_HXX //autogen
#include <tools/shl.hxx>
#endif
class Color;
class IntlWrapper;
namespace binfilter {

class XColorTable;
// static and prototypes -------------------------------------------------

static const sal_Unicode cDelim = ',';
static const sal_Unicode cpDelim[] = { ',' , ' ', '\0' };

String GetSvxString( USHORT nId );
String GetMetricText( long nVal, SfxMapUnit eSrcUnit, SfxMapUnit eDestUnit, const ::IntlWrapper * pIntl );
#ifndef SVX_LIGHT
String GetColorString( const Color& rCol );
#endif
USHORT GetMetricId( SfxMapUnit eUnit );

// -----------------------------------------------------------------------

inline String GetBoolString( BOOL bVal )
{
	return String( ResId( bVal ? RID_SVXITEMS_TRUE
							   : RID_SVXITEMS_FALSE, DIALOG_MGR() ) );
}

// -----------------------------------------------------------------------

inline long Scale( long nVal, long nMult, long nDiv )
{
	BigInt aVal( nVal );
	aVal *= nMult;
	aVal += nDiv/2;
	aVal /= nDiv;
	return aVal;
}

}//end of namespace binfilter
#endif

