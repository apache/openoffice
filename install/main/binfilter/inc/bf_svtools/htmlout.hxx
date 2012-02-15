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



#ifndef _HTMLOUT_HXX
#define _HTMLOUT_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#include <bf_svtools/bf_solar.h>

#ifndef _RTL_TEXTENC_H
#include <rtl/textenc.h>
#endif
#ifndef _SFXMACITEM_HXX //autogen
#include <bf_svtools/macitem.hxx>
#endif

class Color;
class String;
class SvStream;
class SvDataObject;

namespace binfilter
{
class ImageMap;

class SvxMacroTableDtor;
class SvNumberFormatter;

struct HTMLOutEvent
{
	const sal_Char *pBasicName;
	const sal_Char *pJavaName;
	USHORT nEvent;
};

struct HTMLOutFuncs
{
#if defined(MAC) || defined(UNX)
	static const sal_Char sNewLine;		// nur \012 oder \015
#else
	static const sal_Char __FAR_DATA sNewLine[];	// \015\012
#endif
};
}

#endif

