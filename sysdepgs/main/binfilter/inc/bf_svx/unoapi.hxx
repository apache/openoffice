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



#ifndef _SVX_UNOAPI_HXX_
#define _SVX_UNOAPI_HXX_


#ifndef _COM_SUN_STAR_DRAWING_XSHAPE_HPP_
#include <com/sun/star/drawing/XShape.hpp>
#endif

#ifndef _SAL_TYPES_H_ 
#include <sal/types.h>
#endif

#ifndef _BF_GOODIES_GRAPHICOBJECT_HXX
#include <bf_goodies/graphicobject.hxx>
#endif

#ifndef _SFXPOOLITEM_HXX
#include <bf_svtools/poolitem.hxx>
#endif
class String;
namespace binfilter {

class SfxItemPool;

class SvxShape;
class SdrObject;
class SvxNumBulletItem;

/** creates a StarOffice API wrapper with the given type and inventor
	Deprecated: This will be replaced with a function returning XShape.
*/
SvxShape* CreateSvxShapeByTypeAndInventor( sal_uInt16 nType, sal_uInt32 nInventor ) throw();

/** returns a StarOffice API wrapper for the given SdrObject */
::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > GetXShapeForSdrObject( SdrObject* pObj ) throw ();

/** returns the SdrObject from the given StarOffice API wrapper */
SdrObject* GetSdrObjectFromXShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape ) throw() ;

/** returns a GraphicObject for this URL */
BfGraphicObject CreateGraphicObjectFromURL( const ::rtl::OUString &rURL ) throw() ;

/** returns the SvxNumBulletItem with the given name from the pool or a null if there is no item
    with that name
*/
SvxNumBulletItem* SvxGetNumBulletItemByName( SfxItemPool* pPool, const ::rtl::OUString& aName ) throw();

/** maps the vcl MapUnit enum to a API constant MeasureUnit.
	Returns false if conversion is not supported.
*/
sal_Bool SvxMapUnitToMeasureUnit( const short nVcl, short& eApi ) throw();

/** maps the API constant MeasureUnit to a vcl MapUnit enum.
	Returns false if conversion is not supported.
*/
sal_Bool SvxMeasureUnitToFieldUnit( const short eApi, short& nVcl ) throw();

/** maps the vcl MapUnit enum to a API constant MeasureUnit.
	Returns false if conversion is not supported.
*/
sal_Bool SvxFieldUnitToMeasureUnit( const short nVcl, short& eApi ) throw();

/** if the given name is a predefined name for the current language it is replaced by
	the corresponding api name.
*/
void SvxUnogetApiNameForItem( const sal_Int16 nWhich, const String& rInternalName, ::rtl::OUString& rApiName ) throw();

/** if the given name is a predefined api name it is replaced by the predefined name
	for the current	language.
*/
void SvxUnogetInternalNameForItem( const sal_Int16 nWhich, const ::rtl::OUString& rApiName, String& rInternalName ) throw();

/** converts the given any with a metric to 100th/mm if needed */
void SvxUnoConvertToMM( const SfxMapUnit eSourceMapUnit, ::com::sun::star::uno::Any & rMetric ) throw();

/** converts the given any with a metric from 100th/mm to the given metric if needed */
void SvxUnoConvertFromMM( const SfxMapUnit eDestinationMapUnit, ::com::sun::star::uno::Any & rMetric ) throw();

}//end of namespace binfilter
#endif // _SVX_UNOAPI_HXX_
