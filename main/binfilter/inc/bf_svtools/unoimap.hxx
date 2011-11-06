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



#ifndef _SVTOOLS_UNOIMAP_HXX
#define _SVTOOLS_UNOIMAP_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _COM_SUN_STAR_UNO_XINTERFACE_HPP_ 
#include <com/sun/star/uno/XInterface.hpp>
#endif

namespace binfilter
{

class ImageMap;
struct SvEventDescription;

 com::sun::star::uno::Reference< com::sun::star::uno::XInterface > SvUnoImageMapRectangleObject_createInstance( const SvEventDescription* pSupportedMacroItems );
 com::sun::star::uno::Reference< com::sun::star::uno::XInterface > SvUnoImageMapCircleObject_createInstance( const SvEventDescription* pSupportedMacroItems );
 com::sun::star::uno::Reference< com::sun::star::uno::XInterface > SvUnoImageMapPolygonObject_createInstance( const SvEventDescription* pSupportedMacroItems );

 com::sun::star::uno::Reference< com::sun::star::uno::XInterface > SvUnoImageMap_createInstance( const SvEventDescription* pSupportedMacroItems );
 com::sun::star::uno::Reference< com::sun::star::uno::XInterface > SvUnoImageMap_createInstance( const ImageMap& rMap, const SvEventDescription* pSupportedMacroItems );
 sal_Bool SvUnoImageMap_fillImageMap( com::sun::star::uno::Reference< com::sun::star::uno::XInterface > xImageMap, ImageMap& rMap );

}

#endif
