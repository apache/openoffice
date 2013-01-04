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


#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _SVX_FMDMOD_HXX
#include "fmdmod.hxx"
#endif

#ifndef _SVX_FMSERVS_HXX
#include "fmservs.hxx"
#endif
#ifndef _SVX_FMOBJ_HXX
#include <fmobj.hxx>
#endif
#ifndef _SVX_UNOSHAPE_HXX
#include <unoshape.hxx>
#endif
#include "fmglob.hxx"
#ifndef _LEGACYBINFILTERMGR_HXX
#include <legacysmgr/legacy_binfilters_smgr.hxx>	//STRIP002 
#endif
namespace binfilter {

using namespace ::binfilter::svxform;//STRIP008 using namespace ::svxform;

//-----------------------------------------------------------------------------
/*N*/ ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >  SAL_CALL SvxFmMSFactory::createInstance(const ::rtl::OUString& ServiceSpecifier) throw( ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException )
/*N*/ {
/*N*/ 	::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >  xRet;
/*N*/ 	if ( ServiceSpecifier.indexOf( ::rtl::OUString::createFromAscii("com.sun.star.form.component.") ) == 0 )
/*N*/ 	{
/*?*/ 		xRet = ::legacy_binfilters::getLegacyProcessServiceFactory()->createInstance(ServiceSpecifier);
/*N*/ 	}
/*N*/ 	else if ( ServiceSpecifier == ::rtl::OUString( ::rtl::OUString::createFromAscii("com.sun.star.drawing.ControlShape") ) )
/*N*/ 	{
/*N*/ 		SdrObject* pObj = new FmFormObj(OBJ_FM_CONTROL);
/*N*/ 		xRet = *new SvxShapeControl(pObj);
/*N*/ 	}
/*N*/ 	if (!xRet.is())
/*N*/ 		xRet = SvxUnoDrawMSFactory::createInstance(ServiceSpecifier);
/*N*/ 	return xRet;
/*N*/ }

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

/*
// XServiceManager
::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >  SvxFmDrawModel::createInstance(const ::rtl::OUString& ServiceName)
			const throw( ::com::sun::star::lang::ServiceNotRegisteredException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException )
{
	::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >  xRet;
	sal_uInt16 nTokenCount = ServiceName.getTokenCount('.');
	if (nTokenCount == 5 &&
		ServiceName.getToken( 0, '.' ) == ::rtl::OUString::createFromAscii("stardiv") &&
		ServiceName.getToken( 1, '.' ) == ::rtl::OUString::createFromAscii("one") &&
		ServiceName.getToken( 2, '.' ) == ::rtl::OUString::createFromAscii("form") &&
		ServiceName.getToken( 3, '.' ) == ::rtl::OUString::createFromAscii("component"))
	{
		xRet = ::legacy_binfilters::getLegacyProcessServiceFactory()->createInstance(ServiceName);
	}
	else
	if (nTokenCount == 4 &&
		ServiceName.getToken( 0, '.' ) == ::rtl::OUString::createFromAscii("stardiv") &&
		ServiceName.getToken( 1, '.' ) == ::rtl::OUString::createFromAscii("one") &&
		ServiceName.getToken( 2, '.' ) == ::rtl::OUString::createFromAscii("drawing") &&
		ServiceName.getToken( 3, '.' ) == ::rtl::OUString::createFromAscii("ControlShape"))
	{
		SdrObject* pObj = new FmFormObj();
		xRet = *new SvxShapeControl(pObj);
	}
	if (!xRet.is())
		xRet = SvxUnoDrawModel::createInstance(ServiceName);
	return xRet;
}
*/


}
