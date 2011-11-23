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




#ifndef _FILTER_CONFIG_ITEM_HXX_
#define _FILTER_CONFIG_ITEM_HXX_

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _COM_SUN_STAR_AWT_SIZE_HPP_
#include <com/sun/star/awt/Size.hpp>
#endif

#ifndef _COM_SUN_STAR_UNO_ANY_H_
#include <com/sun/star/uno/Any.h>
#endif
#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_UNO_XINTERFACE_HPP_
#include <com/sun/star/uno/XInterface.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYACCESS_HPP_
#include <com/sun/star/beans/XPropertyAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_TASK_XSTATUSINDICATOR_HPP_
#include <com/sun/star/task/XStatusIndicator.hpp>
#endif

namespace binfilter
{

class  FilterConfigItem
{
		::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface	> xUpdatableView;
		::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > xPropSet;
		::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > aFilterData;

		sal_Bool	bModified;

		sal_Bool	ImplGetPropertyValue( ::com::sun::star::uno::Any& rAny,
						const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& rXPropSet,
							const ::rtl::OUString& rPropName,
								sal_Bool bTestPropertyAvailability );

		void		ImpInitTree( const String& rTree );


		static ::com::sun::star::beans::PropertyValue* GetPropertyValue(
					::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& rPropSeq,
						const ::rtl::OUString& rName );
		static  sal_Bool WritePropertyValue(
					::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& rPropSeq,
						const ::com::sun::star::beans::PropertyValue& rPropValue );

	public :

		FilterConfigItem( const ::rtl::OUString& rSubTree );
		FilterConfigItem( ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >* pFilterData );
		~FilterConfigItem();

		// all read methods are trying to return values in following order:
		// 1. FilterData PropertySequence
		// 2. configuration
		// 3. given default
		sal_Bool	ReadBool( const ::rtl::OUString& rKey, sal_Bool bDefault );
		sal_Int32	ReadInt32( const ::rtl::OUString& rKey, sal_Int32 nDefault );

		// try to store to configuration
		// and always stores into the FilterData sequence
		void		WriteInt32( const ::rtl::OUString& rKey, sal_Int32 nValue );

		// GetStatusIndicator is returning the "StatusIndicator" property of the FilterData sequence
		::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator > GetStatusIndicator() const;
};

}

#endif	// _FILTER_CONFIG_ITEM_HXX_

