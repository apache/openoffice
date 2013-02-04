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


#ifndef _SVX_SHAPEIMPL_HXX
#define _SVX_SHAPEIMPL_HXX

#include "unoshape.hxx"
class SvGlobalName;
namespace binfilter {


/***********************************************************************
*                                                                      *
***********************************************************************/

class SvxShapeCaption : public SvxShapeText
{
public:
	SvxShapeCaption( SdrObject* pObj ) throw();
	virtual ~SvxShapeCaption() throw();
};

/***********************************************************************
*                                                                      *
***********************************************************************/

class SvxOle2Shape : public SvxShape
{
public:
	SvxOle2Shape( SdrObject* pObject, const SfxItemPropertyMap* pPropertySet ) throw ();
	virtual ~SvxOle2Shape() throw();

	sal_Bool createObject( const SvGlobalName &aClassName );

	// XInterface
	virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);

	//XPropertySet
    virtual void SAL_CALL 	setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) 	throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException); 
};

/***********************************************************************
*                                                                      *
***********************************************************************/
}//end of namespace binfilter
#ifndef SVX_LIGHT

#ifndef _PLUGIN_HXX 
#include <bf_so3/plugin.hxx>
#endif
namespace binfilter {
class SvxPluginShape : public SvxOle2Shape
{
private:
	SvPlugInObjectRef mxPlugin;

public:
	SvxPluginShape( SdrObject* pObj ) throw();
	virtual ~SvxPluginShape() throw();

	virtual void Create( SdrObject* pNewOpj, SvxDrawPage* pNewPage = NULL ) throw ();

	//XPropertySet
    virtual void SAL_CALL 	setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) 	throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException); 
};
}//end of namespace binfilter
#endif

/***********************************************************************
*                                                                      *
***********************************************************************/

#ifndef SVX_LIGHT

#ifndef _APPLET_HXX 
#include <bf_so3/applet.hxx>
#endif
namespace binfilter {
class SvxAppletShape : public SvxOle2Shape
{
private:
	SvAppletObjectRef mxApplet;

public:
	SvxAppletShape( SdrObject* pObj ) throw();
	virtual ~SvxAppletShape() throw();

	virtual void Create( SdrObject* pNewOpj, SvxDrawPage* pNewPage = NULL ) throw ();

	//XPropertySet
    virtual void SAL_CALL 	setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) 	throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException); 
};
}//end of namespace binfilter
#endif

/***********************************************************************
*                                                                      *
***********************************************************************/

#ifndef SVX_LIGHT

namespace binfilter {
class SvxFrameShape : public SvxOle2Shape
{
public:
	SvxFrameShape( SdrObject* pObj ) throw();
	virtual ~SvxFrameShape() throw();

	virtual void Create( SdrObject* pNewOpj, SvxDrawPage* pNewPage = NULL ) throw ();

	//XPropertySet
    virtual void SAL_CALL 	setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) 	throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException); 
};
}//end of namespace binfilter
#endif

#endif


