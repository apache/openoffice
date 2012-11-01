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



// header for Point, Rectangle
// header for Polygon
// header for DBG_ASSERT
// header for XPolygon, XPolyPolygon
#ifndef _XPOLY_HXX
#include <bf_svx/xpoly.hxx>
#endif
// header for Line
#ifndef _SV_LINE_HXX
#include <tools/line.hxx>
#endif
// for performance measurement
#ifndef	_RTL_LOGFILE_HXX_
#include <rtl/logfile.hxx>
#endif

#include "calculat.hxx"

#include <algorithm>
#include <functional>
namespace binfilter {

using namespace ::std;

    
/*N*/ void SchCalculationHelper::IntersectPolygonWithRectangle( const XPolygon& rPolygon, const Rectangle& rRectangle, XPolyPolygon& aResult )
/*N*/ {
/*N*/ 	RTL_LOGFILE_CONTEXT_AUTHOR( context, "sch", "bm93744", "SchCalculationHelper::IntersectPolygonWithRectangle");
/*N*/ 
/*N*/ 	aResult.Clear();
/*N*/ 
/*N*/  	if( rRectangle.IsInside( rPolygon.GetBoundRect() ) )
/*N*/ 	{
/*N*/ 		aResult.Insert( rPolygon );
/*N*/         OSL_TRACE( "IntersectPolygonWithRectangle: result has %d polygons", aResult.Count() );
/*N*/ 		return;
/*N*/ 	}
/*N*/ 
/*N*/     Point aFrom;
/*N*/     Point aTo;
/*N*/ 	USHORT nCount = rPolygon.GetPointCount();
/*N*/ 
/*N*/     // set last point to a position outside the rectangle, such that the first
/*N*/     // time clip2d returns true, the comparison to last will always yield false
/*N*/     Point aLast (rRectangle.TopLeft());
/*N*/     aLast.Move (-1, -1);
/*N*/     XPolygon aCurrentPoly;
/*N*/     USHORT nIdx = 0;
/*N*/ 
/*N*/ 	for (USHORT i=1; i<nCount; i++)
/*N*/ 	{
/*N*/ 		aFrom = rPolygon[i-1];
/*N*/ 		aTo = rPolygon[i];
/*N*/ 		if (clip2d (aFrom, aTo, rRectangle))
/*N*/ 		{
/*N*/             // compose an XPolygon of as many consecutive points as possible
/*N*/             if (aFrom == aLast)
/*N*/             {
/*N*/                 if (aTo != aFrom)
/*N*/                     aCurrentPoly.Insert (nIdx++, aTo, XPOLY_NORMAL);
/*N*/             }
/*N*/             else
/*N*/             {
/*N*/                 // create an XPolygon and put it into the XPolyPolygon
/*N*/                 if (aCurrentPoly.GetPointCount() > 0)
/*N*/                     aResult.Insert (aCurrentPoly, XPOLYPOLY_APPEND);
/*N*/ 
/*N*/                 // start new sequence
/*N*/ 				aCurrentPoly.SetPointCount (0);
/*N*/                 aCurrentPoly.Insert (0, aFrom, XPOLY_NORMAL);
/*N*/                 nIdx = 1;
/*N*/                 if (aTo != aFrom)
/*N*/                     aCurrentPoly.Insert (nIdx++, aTo, XPOLY_NORMAL);
/*N*/             }
/*N*/ 
/*N*/             aLast = aTo;
/*N*/         }
/*N*/     }
/*N*/     if (aCurrentPoly.GetPointCount() > 0)
/*N*/         aResult.Insert (aCurrentPoly, XPOLYPOLY_APPEND);
/*N*/ 
/*N*/     OSL_TRACE( "IntersectPolygonWithRectangle: result has %d polygons", aResult.Count() );
/*N*/ }





/*N*/ BOOL	SchCalculationHelper::clip2d	(Point & rPoint0, 
/*N*/ 										Point & rPoint1,
/*N*/ 										const Rectangle & rRectangle)
/*N*/ {
/*N*/ 	//	Direction vector of the line.						
/*N*/ 	Point	aD = rPoint1 - rPoint0;
/*N*/ 
/*N*/ 	if (aD.X()==0 && aD.Y()==0 && rRectangle.IsInside (rPoint0))
/*N*/ 	{
/*N*/ 		//	Degenerate case of a zero length line.
/*N*/ 		return TRUE;
/*N*/ 	}
/*N*/ 	else
/*N*/ 	{
/*N*/ 		//	Values of the line parameter where the line enters resp. leaves the rectangle.
/*N*/ 		double	fTE = 0,
/*N*/ 				fTL = 1;
/*N*/ 				
/*N*/ 		//	Test wether at least a part lies in the four half-planes with respect to 
/*N*/ 		//	the rectangles four edges.
/*N*/ 		if (CLIPt (aD.X(), rRectangle.Left() - rPoint0.X(), fTE, fTL))
/*N*/ 			if (CLIPt (-aD.X(), rPoint0.X() - rRectangle.Right(), fTE, fTL))
/*N*/ 				if (CLIPt (aD.Y(), rRectangle.Top() - rPoint0.Y(), fTE, fTL))
/*N*/ 					if (CLIPt (-aD.Y(), rPoint0.Y() - rRectangle.Bottom(), fTE, fTL))
/*N*/ 					{
/*N*/ 						//	At least a part is visible.
/*N*/ 						if (fTL < 1)
/*N*/ 						{
/*N*/ 							//	::com::pute the new end point.
/*N*/ 							rPoint1.X() = (long)(rPoint0.X() + fTL * aD.X() + 0.5);
/*N*/ 							rPoint1.Y() = (long)(rPoint0.Y() + fTL * aD.Y() + 0.5);
/*N*/ 						}
/*N*/ 						if (fTE > 0)
/*N*/ 						{
/*N*/ 							//	::com::pute the new starting point.
/*N*/ 							rPoint0.X() = (long)(rPoint0.X() + fTE * aD.X() + 0.5);
/*N*/ 							rPoint0.Y() = (long)(rPoint0.Y() + fTE * aD.Y() + 0.5);
/*N*/ 						}
/*N*/ 						return TRUE;
/*N*/ 					}
/*N*/ 					
/*N*/ 		//	Line is not visible.
/*N*/ 		return FALSE;
/*N*/ 	}
/*N*/ }




/*N*/ BOOL	SchCalculationHelper::CLIPt	(double fDenom, 
/*N*/ 									double fNum, 
/*N*/ 									double & fTE, 
/*N*/ 									double & fTL)
/*N*/ {
/*N*/ 	double	fT;
/*N*/ 	
/*N*/ 	if (fDenom > 0)				//	Intersection enters: PE
/*N*/ 	{
/*N*/ 		fT = fNum / fDenom;		//	Parametric value at the intersection.
/*N*/ 		if (fT > fTL)			//	fTE and fTL crossover
/*N*/ 			return FALSE;		//	  therefore reject the line.
/*N*/ 		else if (fT > fTE)		//	A new fTE has been found.
/*N*/ 			fTE = fT;
/*N*/ 	}
/*N*/ 	else if (fDenom < 0)		//	Intersection leaves: PL
/*N*/ 	{
/*N*/ 		fT = fNum / fDenom;		//	Parametric Value at the intersection.
/*N*/ 		if (fT < fTE)			//	fTE and fTL crossover
/*N*/ 			return FALSE;		//	  therefore reject the line.
/*N*/ 		else if (fT < fTL)		//	A new fTL has been found.
/*N*/ 			fTL = fT;
/*N*/ 	}
/*N*/ 	else if (fNum > 0)
/*N*/ 		return FALSE;			//	Line lies on the outside of the edge.
/*N*/ 	
/*N*/ 	return TRUE;
/*N*/ }


// --------------------------------------------------------------------------------

}
