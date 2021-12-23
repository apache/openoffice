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



#ifndef _ENHANCED_CUSTOMSHAPE_ENGINE_HXX
#define _ENHANCED_CUSTOMSHAPE_ENGINE_HXX

#include <tools/debug.hxx>
#include <tools/string.hxx>
#include <tools/stack.hxx>

#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/uno/RuntimeException.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/registry/XRegistryKey.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <cppuhelper/implbase3.hxx>
#ifndef __com_sun_star_awt_Rectangle_hpp_
#include <com/sun/star/awt/Rectangle.hpp>
#endif
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/drawing/PolyPolygonBezierCoords.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/drawing/XCustomShapeEngine.hpp>

// -----------------------------------------------------------------------------

#define NMSP_IO			com::sun::star::io
#define NMSP_UNO		com::sun::star::uno
#define NMSP_BEANS		com::sun::star::beans
#define NMSP_LANG		com::sun::star::lang
#define NMSP_UTIL		com::sun::star::util
#define NMSP_SAX		com::sun::star::xml::sax
#define NMSP_LOGGING	NMSP_UTIL::logging


#define REF( _def_Obj )			NMSP_UNO::Reference< _def_Obj >
#define SEQ( _def_Obj )			NMSP_UNO::Sequence< _def_Obj >
#define B2UCONST( _def_pChar )	(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(_def_pChar )))

// ---------------------------
// - EnhancedCustomShapeEngine -
// ---------------------------
//

class SdrObject;
class SdrObjCustomShape;
class EnhancedCustomShapeEngine : public cppu::WeakImplHelper3
<
	com::sun::star::lang::XInitialization,
	com::sun::star::lang::XServiceInfo,
	com::sun::star::drawing::XCustomShapeEngine
>
{
	REF( NMSP_LANG::XMultiServiceFactory )		mxFact;
	REF( com::sun::star::drawing::XShape )		mxShape;
	sal_Bool									mbForceGroupWithText;

	SdrObject* ImplForceGroupWithText( const SdrObjCustomShape* pCustoObj, SdrObject* pRenderedShape );

public:
							EnhancedCustomShapeEngine( const REF( NMSP_LANG::XMultiServiceFactory )& rxMgr );
	virtual 				~EnhancedCustomShapeEngine();

	// XInterface
	virtual void SAL_CALL	acquire() throw();
	virtual void SAL_CALL	release() throw();

	// XInitialization
	virtual void SAL_CALL initialize( const SEQ( NMSP_UNO::Any )& aArguments )
		throw ( NMSP_UNO::Exception, NMSP_UNO::RuntimeException );

	// XServiceInfo
	virtual rtl::OUString SAL_CALL getImplementationName()
		throw ( NMSP_UNO::RuntimeException );
	virtual sal_Bool SAL_CALL supportsService( const rtl::OUString& rServiceName )
		throw ( NMSP_UNO::RuntimeException );
	virtual SEQ( rtl::OUString ) SAL_CALL getSupportedServiceNames()
		throw ( NMSP_UNO::RuntimeException );

	// XCustomShapeEngine
	virtual REF( com::sun::star::drawing::XShape ) SAL_CALL render()
		throw ( NMSP_UNO::RuntimeException );
	virtual com::sun::star::awt::Rectangle SAL_CALL getTextBounds()
		throw ( NMSP_UNO::RuntimeException );
	virtual com::sun::star::drawing::PolyPolygonBezierCoords SAL_CALL getLineGeometry()
		throw ( NMSP_UNO::RuntimeException );
	virtual SEQ( REF( com::sun::star::drawing::XCustomShapeHandle ) ) SAL_CALL getInteraction()
		throw ( NMSP_UNO::RuntimeException );
};

rtl::OUString EnhancedCustomShapeEngine_getImplementationName()
	throw ( NMSP_UNO::RuntimeException );
sal_Bool SAL_CALL EnhancedCustomShapeEngine_supportsService( const rtl::OUString& rServiceName )
	throw( NMSP_UNO::RuntimeException );
SEQ( rtl::OUString ) SAL_CALL EnhancedCustomShapeEngine_getSupportedServiceNames()
	throw( NMSP_UNO::RuntimeException );

#endif
