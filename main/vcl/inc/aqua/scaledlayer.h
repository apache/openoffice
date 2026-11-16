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



#ifndef _SV_SCALEDLAYER_H
#define _SV_SCALEDLAYER_H

#include "premac.h"
#include <ApplicationServices/ApplicationServices.h>
#include "postmac.h"

// A CGLayer sized in pixels and the number of its pixels per point.
// It does not retain or release the layer.
class ScaledLayer
{
public:
	ScaledLayer() : mxLayer( NULL ), mfScale( 1.0f ) {}
	ScaledLayer( CGLayerRef xLayer, float fScale ) : mxLayer( xLayer ), mfScale( fScale ) {}

	CGLayerRef	get() const { return mxLayer; }
	float		getScale() const { return mfScale; }

	CGSize		getPixelSize() const { return mxLayer ? CGLayerGetSize( mxLayer ) : CGSizeZero; }
	CGSize		getPointSize() const
	{
		const CGSize aPixels = getPixelSize();
		return CGSizeMake( aPixels.width / mfScale, aPixels.height / mfScale );
	}

	static CGSize toPixels( CGFloat fWidth, CGFloat fHeight, float fScale )
	{
		return CGSizeMake( fWidth * fScale, fHeight * fScale );
	}
	CGSize		toPixels( CGFloat fWidth, CGFloat fHeight ) const { return toPixels( fWidth, fHeight, mfScale ); }

	// lets a layer context take VCL coordinates in points
	void		scaleContext( CGContextRef xContext ) const { CGContextScaleCTM( xContext, mfScale, mfScale ); }

private:
	CGLayerRef	mxLayer;
	float		mfScale;
};

#endif // _SV_SCALEDLAYER_H
