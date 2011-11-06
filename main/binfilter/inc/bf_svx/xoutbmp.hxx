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



#ifndef _XOUTBMP_HXX
#define _XOUTBMP_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _SV_GRAPH_HXX
#include <vcl/graph.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
class VirtualDevice;
class INetURLObject;
class Polygon;
namespace binfilter {
class GraphicFilter;

// -----------
// - Defines -
// -----------

#define XOUTBMP_MIRROR_HORZ				0x00000001L
#define XOUTBMP_MIRROR_VERT				0x00000010L

#define XOUTBMP_CONTOUR_HORZ			0x00000001L
#define XOUTBMP_CONTOUR_VERT			0x00000002L
#define XOUTBMP_CONTOUR_EDGEDETECT		0x00000004L

#define XOUTBMP_DONT_EXPAND_FILENAME	0x10000000L
#define XOUTBMP_USE_GIF_IF_POSSIBLE		0x20000000L
#define XOUTBMP_USE_GIF_IF_SENSIBLE		0x40000000L
#define XOUTBMP_USE_NATIVE_IF_POSSIBLE	0x80000000L

// --------------
// - XOutBitmap -
// --------------


class XOutBitmap
{
public:

	static GraphicFilter* pGrfFilter;
};

// ----------------
// - DitherBitmap -
// ----------------

BOOL DitherBitmap( Bitmap& rBitmap );

}//end of namespace binfilter
#endif // _XOUTBMP_HXX
