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



#ifndef SD_SLIDESORTER_VIEW_FRAME_PAINTER_HXX
#define SD_SLIDESORTER_VIEW_FRAME_PAINTER_HXX

#include <vcl/bitmapex.hxx>


namespace sd { namespace slidesorter { namespace view {

class FramePainter
{
public:
	FramePainter (const BitmapEx& rBitmap);
	~FramePainter (void);

	/** Paint a border around the given box by using a set of bitmaps for
		the corners and sides.
	*/
	void PaintFrame (OutputDevice&rDevice, const Rectangle aBox) const;

	/** Special functionality that takes the color from the center
		bitmap and replaces that color in all bitmaps by the given new
		color. Alpha values are not modified.
		@param bClearCenterBitmap
			When <TRUE/> then the center bitmap is erased.
	*/
	void AdaptColor (const Color aNewColor, const bool bClearCenterBitmap);

private:
	/** Bitmap with offset that is used when the bitmap is painted. The bitmap
	*/
	class OffsetBitmap {
	public:
		BitmapEx maBitmap;
		Point maOffset;

		/** Create one of the eight shadow bitmaps from one that combines
			them all. This larger bitmap is expected to have dimension NxN
			with N=1+2*M. Of this larger bitmap there are created four
			corner bitmaps of size 2*M x 2*M and four side bitmaps of sizes
			1xM (top and bottom) and Mx1 (left and right). The corner
			bitmaps have each one quadrant of size MxM that is painted under
			the interior of the frame.
			@param rBitmap
				The larger bitmap of which the eight shadow bitmaps are cut
				out from.
			@param nHorizontalPosition
				Valid values are -1 (left), 0 (center), and +1 (right).
			@param nVerticalPosition
				Valid values are -1 (top), 0 (center), and +1 (bottom).
		*/
		OffsetBitmap (
			const BitmapEx& rBitmap,
			const sal_Int32 nHorizontalPosition,
			const sal_Int32 nVerticalPosition);

		/** Use the given device to paint the bitmap at the location that is
			the sum of the given anchor and the internal offset.
		*/
		void PaintCorner (OutputDevice& rDevice, const Point& rAnchor) const;

		/** Use the given device to paint the bitmap stretched between the
			two given locations. Offsets of the adjacent corner bitmaps and
			the offset of the side bitmap are used to determine the area
			that is to be filled with the side bitmap.
		*/
		void PaintSide (
			OutputDevice& rDevice,
			const Point& rAnchor1,
			const Point& rAnchor2,
			const OffsetBitmap& rCornerBitmap1,
			const OffsetBitmap& rCornerBitmap2) const;

		/** Fill the given rectangle with the bitmap.
		*/
		void PaintCenter (
			OutputDevice& rDevice,
			const Rectangle& rBox) const;
	};
	OffsetBitmap maTopLeft;
	OffsetBitmap maTop;
	OffsetBitmap maTopRight;
	OffsetBitmap maLeft;
	OffsetBitmap maRight;
	OffsetBitmap maBottomLeft;
	OffsetBitmap maBottom;
	OffsetBitmap maBottomRight;
	OffsetBitmap maCenter;
	bool mbIsValid;
};


} } } // end of namespace sd::slidesorter::view

#endif
