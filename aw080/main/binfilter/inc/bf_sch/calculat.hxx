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



// header for BOOL
#include <bf_svtools/bf_solar.h>

#include <vector>
#include <utility>
class Point;
class Rectangle;
namespace binfilter {

class Line;
class XPolygon;
class XPolyPolygon;

/**	@descr	This class clips single lines and polygons at a given rectangle.
		All of it's methods are static, therefore no object has to be created.
		
		It uses the Liang-Biarsky parametric line-clipping algorithm as described in:
			Computer Graphics: principles and practice, 2nd ed.,
			James D. Foley et al.,
			Section 3.12.4 on page 117.
*/
class SchCalculationHelper
{
public:
	/**	@descr	The intersection between an open polygon and a rectangle is
			calculated and the resulting lines are placed into the poly-polygon aResult.
		@param	rPolygon	The polygon is required to be open, ie. it's start and end point
			have different coordinates and that it is continuous, ie. has no holes.
		@param	rRectangle	The clipping area.
		@param	aResult	The resulting lines that are the parts of the given polygon lying inside
			the clipping area are stored into aResult whose prior content is deleted first.
	 */
	static void IntersectPolygonWithRectangle( const XPolygon& rPolygon, const Rectangle& rRectangle,
											   XPolyPolygon& aResult );

private:
	/**	@descr	The line given by it's two endpoints rP0 and rP1 is clipped at the rectangle
			rRectangle.  If there is at least a part of it visible then TRUE is returned and
			the endpoints of that part are stored in rP0 and rP1.  The points rP0 and rP1
			may have the same coordinates.
		@param	rP0 Start point of the line to clip.  Modified to contain a start point inside
			the clipping area if possible.
		@param	rP1 End point of the line to clip.  Modified to contain an end point inside
			the clipping area if possible.
		@param	rRectangle Clipping area.
		@return	If the line lies completely or partly inside the clipping area then TRUE
			is returned.  If the line lies completely outside then FALSE is returned and rP0 and
			rP1 are left unmodified.
	*/
	static	BOOL	clip2d	(Point & rP0, Point & rP1, const Rectangle& rRectangle);
	
	/**	@descr	This is a supporting function for clip2d.  It computes a new parametric 
			value for an entering (dTE) or leaving (dTL) intersection point with one
			of the edges bounding the clipping area.
			For explanation of the parameters please refer to the literature reference 
			given above.
	*/
	static	BOOL	CLIPt	(double fDenom, double fNum, double & fTE, double & fTL);
};

} //namespace binfilter
