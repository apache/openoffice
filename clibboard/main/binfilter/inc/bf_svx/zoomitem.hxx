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


#ifndef _SVX_ZOOMITEM_HXX
#define _SVX_ZOOMITEM_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SFXINTITEM_HXX //autogen
#include <bf_svtools/intitem.hxx>
#endif
#ifndef _SVX_SVXIDS_HRC
#include <bf_svx/svxids.hrc>
#endif
namespace binfilter {

//-------------------------------------------------------------------------

enum SvxZoomType
{
	SVX_ZOOM_PERCENT,   	// GetValue() ist kein besonderer prozentualer Wert
	SVX_ZOOM_OPTIMAL,		// GetValue() entspricht der optimalen Gr"o\se
	SVX_ZOOM_WHOLEPAGE,		// GetValue() entspricht der ganzen Seite
    SVX_ZOOM_PAGEWIDTH,      // GetValue() entspricht der Seitenbreite
    SVX_ZOOM_PAGEWIDTH_NOBORDER  // GetValue() pagewidth without border
};

//-------------------------------------------------------------------------

class SvxZoomItem: public SfxUInt16Item
{
	USHORT					nValueSet;	// erlaubte Werte (siehe #defines unten)
	SvxZoomType				eType;

public:
	TYPEINFO();

	SvxZoomItem( SvxZoomType eZoomType = SVX_ZOOM_PERCENT,
				 USHORT nVal = 0, USHORT nWhich = SID_ATTR_ZOOM );
	SvxZoomItem( const SvxZoomItem& );
	~SvxZoomItem();

	void					SetValueSet( USHORT nValues ) { nValueSet = nValues; }
	USHORT					GetValueSet() const { return nValueSet; }
	FASTBOOL				IsValueAllowed( USHORT nValue ) const
							{ return nValue == ( nValue & nValueSet ); }

	SvxZoomType 			GetType() const { return eType; }
	void					SetType( SvxZoomType eNewType )
							{ eType = eNewType; }

	virtual SfxPoolItem*	Clone( SfxItemPool *pPool = 0 ) const;
	virtual int				operator==( const SfxPoolItem& ) const;
};

//------------------------------------------------------------------------

#define SVX_ZOOM_ENABLE_50 			0x0001
#define SVX_ZOOM_ENABLE_75 			0x0002
#define SVX_ZOOM_ENABLE_100			0x0004
#define SVX_ZOOM_ENABLE_150			0x0008
#define SVX_ZOOM_ENABLE_200			0x0010
#define SVX_ZOOM_ENABLE_OPTIMAL		0x1000
#define SVX_ZOOM_ENABLE_WHOLEPAGE	0x2000
#define SVX_ZOOM_ENABLE_PAGEWIDTH	0x4000
#define SVX_ZOOM_ENABLE_ALL         0x701F

}//end of namespace binfilter
#endif
