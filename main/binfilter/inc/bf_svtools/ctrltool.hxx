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



#ifndef _CTRLTOOL_HXX
#define _CTRLTOOL_HXX

#include <bf_svtools/bf_solar.h>

#include <sal/types.h>
#include <tools/list.hxx>
#include <vcl/metric.hxx>


class OutputDevice;

namespace binfilter {

class ImplFontListNameInfo;
#define FONTLIST_FONTINFO_NOTFOUND	((USHORT)0xFFFF)

#define FONTLIST_FONTNAMETYPE_PRINTER			((USHORT)0x0001)
#define FONTLIST_FONTNAMETYPE_SCREEN			((USHORT)0x0002)
#define FONTLIST_FONTNAMETYPE_SCALABLE			((USHORT)0x0004)

class FontList : private List
{
private:
	XubString				maMapBoth;
	XubString				maMapPrinterOnly;
	XubString				maMapScreenOnly;
	XubString				maMapSizeNotAvailable;
	XubString				maMapStyleNotAvailable;
	XubString				maMapNotAvailable;
	XubString				maLight;
	XubString				maLightItalic;
	XubString				maNormal;
	XubString				maNormalItalic;
	XubString				maBold;
	XubString				maBoldItalic;
	XubString				maBlack;
	XubString				maBlackItalic;
	long*					mpSizeAry;
	OutputDevice*			mpDev;
	OutputDevice*			mpDev2;

#ifdef CTRLTOOL_CXX
	ImplFontListNameInfo*	ImplFind( const XubString& rSearchName, ULONG* pIndex ) const;
	ImplFontListNameInfo*	ImplFindByName( const XubString& rStr ) const;
	void					ImplInsertFonts( OutputDevice* pDev, BOOL bAll,
											 BOOL bInsertData );
#endif

public:
							FontList( OutputDevice* pDevice,
									  OutputDevice* pDevice2 = NULL,
									  BOOL bAll = TRUE );
							~FontList();

    OutputDevice*           GetDevice() const { return mpDev; }
	OutputDevice*			GetDevice2() const { return mpDev2; }

	const XubString&		GetNormalStr() const { return maNormal; }
	const XubString&		GetItalicStr() const { return maNormalItalic; }
	const XubString&		GetBoldStr() const { return maBold; }
	const XubString&		GetBoldItalicStr() const { return maBoldItalic; }
	FontInfo				Get( const XubString& rName,
								 FontWeight eWeight,
								 FontItalic eItalic ) const;

	USHORT					GetFontNameCount() const
								{ return (USHORT)List::Count(); }
private:
							FontList( const FontList& );
	FontList&				operator =( const FontList& );
};

}

#endif	// _CTRLTOOL_HXX
