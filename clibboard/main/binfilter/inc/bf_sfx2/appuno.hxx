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


#ifndef _SFX_APPUNO_HXX
#define _SFX_APPUNO_HXX

//____________________________________________________________________________________________________________________________________
//	generated header
//____________________________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XINITIALIZATION_HPP_
#include <com/sun/star/lang/XInitialization.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XTYPEPROVIDER_HPP_
#include <com/sun/star/lang/XTypeProvider.hpp>
#endif

#ifndef _COM_SUN_STAR_REGISTRY_XSIMPLEREGISTRY_HPP_
#include <com/sun/star/registry/XSimpleRegistry.hpp>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_URL_HPP_
#include <com/sun/star/util/URL.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_EXCEPTION_HPP_
#include <com/sun/star/uno/Exception.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XDISPATCHPROVIDER_HPP_
#include <com/sun/star/frame/XDispatchProvider.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XDISPATCH_HPP_
#include <com/sun/star/frame/XDispatch.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XSYNCHRONOUSDISPATCH_HPP_
#include <com/sun/star/frame/XSynchronousDispatch.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XNOTIFYINGDISPATCH_HPP_
#include <com/sun/star/frame/XNotifyingDispatch.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XDISPATCHRESULTLISTENER_HPP_
#include <com/sun/star/frame/XDispatchResultListener.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_DISPATCHDESCRIPTOR_HPP_
#include <com/sun/star/frame/DispatchDescriptor.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

//____________________________________________________________________________________________________________________________________
//	fix uno header
//____________________________________________________________________________________________________________________________________

#ifndef _COM_SUN_STAR_UNO_ANY_H_
#include <com/sun/star/uno/Any.h>
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

#ifndef _CPPUHELPER_IMPLBASE3_HXX_
#include <cppuhelper/implbase3.hxx>
#endif

//____________________________________________________________________________________________________________________________________
//	something else header
//____________________________________________________________________________________________________________________________________

#ifndef _ERRCODE_HXX
#include <tools/errcode.hxx>
#endif
#include <bf_svtools/svarray.hxx>
#define _SVSTDARR_STRINGSDTOR
#include <bf_svtools/svstdarr.hxx>
#include <bf_sfx2/sfxuno.hxx>

#include <bf_svtools/poolitem.hxx>

namespace binfilter {
class SfxAllItemSet;
class SfxItemSet;

class SfxUsrAnyItem : public SfxPoolItem
{
    ::com::sun::star::uno::Any  aValue;
public:
                                TYPEINFO();
                                SfxUsrAnyItem( sal_uInt16 nWhich, const ::com::sun::star::uno::Any& rAny );
    ::com::sun::star::uno::Any  GetValue() const
                                { return aValue; }
    virtual int                 operator==( const SfxPoolItem& ) const;
    virtual String              GetValueText() const{DBG_BF_ASSERT(0, "STRIP"); return String();} //STRIP001 virtual String              GetValueText() const;
    virtual SfxPoolItem*        Clone( SfxItemPool *pPool = 0 ) const;
};

typedef SfxUsrAnyItem SfxUnoAnyItem;

void TransformParameters(           sal_uInt16                          nSlotId     ,
							const	UNOSEQUENCE< UNOPROPERTYVALUE >&	seqArgs		,
									SfxAllItemSet&						aSet	);

void TransformItems(        sal_uInt16                          nSlotId     ,
							const	SfxItemSet&							aSet		,
									UNOSEQUENCE< UNOPROPERTYVALUE >&	seqArgs	 );


}//end of namespace binfilter
#endif
