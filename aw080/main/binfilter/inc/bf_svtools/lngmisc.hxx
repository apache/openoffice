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



#ifndef _SVTOOLS_LNGMISC_HXX_
#define _SVTOOLS_LNGMISC_HXX_

#include <bf_svtools/bf_solar.h>
#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif

///////////////////////////////////////////////////////////////////////////

#define SVT_SOFT_HYPHEN	((sal_Unicode) 0x00AD)
#define SVT_HARD_HYPHEN	((sal_Unicode) 0x2011)

// the non-breaking space
#define SVT_HARD_SPACE  ((sal_Unicode) 0x00A0)

namespace binfilter
{

inline BOOL IsHyphen( sal_Unicode cChar )
{
	return cChar == SVT_SOFT_HYPHEN  ||  cChar == SVT_HARD_HYPHEN;
}


inline BOOL IsControlChar( sal_Unicode cChar )
{
	return cChar < (sal_Unicode) ' ';
}


inline BOOL HasHyphens( const rtl::OUString &rTxt )
{
	return	rTxt.indexOf( SVT_SOFT_HYPHEN ) != -1  ||
			rTxt.indexOf( SVT_HARD_HYPHEN ) != -1;
}

}

#endif
