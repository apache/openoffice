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



#ifndef _POLYPATH_HXX
#define _POLYPATH_HXX

#include <bf_svtools/bf_solar.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace binfilter {

class XPolygon; // Dummy zum Uebersetzen

enum PathKind {PATH_POLYGON,
			   PATH_POLYLINE,
			   PATH_BEZIER,
			   PATH_BEZIERLINE};

class PolyPath {
protected:
	// ...
public:
	PolyPath();
	PolyPath(const PolyPath& rPath);
	~PolyPath();
	void Clear();

		  USHORT    GetPathCount()        const;
		  PathKind  GetPathKind(USHORT i) const;
	const Polygon&  GetPolygon (USHORT i) const;
	const XPolygon& GetXPolygon(USHORT i) const;
		  Polygon&  GetPolygon (USHORT i);
		  XPolygon& GetXPolygon(USHORT i);
	const Polygon&  operator[] (USHORT i); // eigentlich identisch mit Get

	void InsertPolyPath  (const PolyPath&   , USHORT nPos=POLYPOLY_APPEND);
	void InsertPolygon   (const Polygon&    , USHORT nPos=POLYPOLY_APPEND);
	void InsertPolygon   (const PolyPolygon&, USHORT nPos=POLYPOLY_APPEND);
	void InsertPolyLine  (const Polygon&    , USHORT nPos=POLYPOLY_APPEND);
	void InsertBezier    (const XPolygon&   , USHORT nPos=POLYPOLY_APPEND);
	void InsertBezierLine(const XPolygon&   , USHORT nPos=POLYPOLY_APPEND);

	BOOL operator==(const PolyPath& rPath);
	void operator!=(const PolyPath& rPath);
	void operator= (const PolyPath& rPath);
	void operator+=(const Point&);
	void operator-=(const Point&);

	Rectangle GetBoundRect() const;
	BOOL IsInside(const Point&) const;
	BOOL IsTouch(const Rectangle&) const;
};

// ExtOutputDevice::DrawPolyPath(const PolyPath&)
// Alle Polygone und Beziers werden zu einem einzigen PolyPolygon gemacht das
// dann mit DrawPolyPolygon() ausgegeben wird. Alle PolyLines und BezierLines
// werden dagegen mit DrawPolyLine() (oder DrawPolyPolyLine()?) gamalt.

////////////////////////////////////////////////////////////////////////////////////////////////////

}//end of namespace binfilter
#endif //_POLYPATH_HXX

