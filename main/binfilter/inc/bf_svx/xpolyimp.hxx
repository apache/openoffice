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



#ifndef _XPOLYIMP_HXX
#define _XPOLYIMP_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _GEN_HXX
#include <tools/gen.hxx>
#endif
#ifndef _XPOLY_HXX
#include <bf_svx/xpoly.hxx>
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif
namespace binfilter {


/*************************************************************************
|*
|*    class ImpXPolygon
|*
|*    Beschreibung
|*    Ersterstellung       08.11.94
|*    Letzte Aenderung Joe 26.09.95
|*
*************************************************************************/
class ImpXPolygon
{
public:
	Point*          pPointAry;
	BYTE*           pFlagAry;
	Point*          pOldPointAry;
	BOOL            bDeleteOldPoints;
	USHORT          nSize;
	USHORT          nResize;
	USHORT          nPoints;
	USHORT          nRefCount;

	ImpXPolygon( USHORT nInitSize = 16, USHORT nResize=16 );
	ImpXPolygon( const ImpXPolygon& rImpXPoly );
	~ImpXPolygon();

	FASTBOOL operator==(const ImpXPolygon& rImpXPoly) const;
	FASTBOOL operator!=(const ImpXPolygon& rImpXPoly) const { return !operator==(rImpXPoly); }

	void CheckPointDelete()
	{
		if ( bDeleteOldPoints )
		{
			delete[] (char*)pOldPointAry;
			bDeleteOldPoints = FALSE;
		}
	}

	void Resize( USHORT nNewSize, BOOL bDeletePoints = TRUE );
	void InsertSpace( USHORT nPos, USHORT nCount );
	void Remove( USHORT nPos, USHORT nCount );
};


/*************************************************************************
|*
|*    class ImpXPolyPolygon
|*
|*    Beschreibung
|*    Ersterstellung          08.11.94
|*    Letzte Aenderung  Joe 26-09-1995
|*
*************************************************************************/
DECLARE_LIST( XPolygonList, XPolygon* )//STRIP008 DECLARE_LIST( XPolygonList, XPolygon* );

class ImpXPolyPolygon
{
public:
	XPolygonList aXPolyList;
	USHORT       nRefCount;

				 ImpXPolyPolygon( USHORT nInitSize = 16, USHORT nResize = 16 ) :
					aXPolyList( 1024, nInitSize, nResize )
					{ nRefCount = 1; }
				ImpXPolyPolygon( const ImpXPolyPolygon& rImpXPolyPoly );
				~ImpXPolyPolygon();

};



}//end of namespace binfilter
#endif      // _XPOLYIMP_HXX
