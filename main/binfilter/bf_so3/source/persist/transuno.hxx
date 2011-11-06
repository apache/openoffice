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



#ifndef _TRANSUNO_HXX
#define _TRANSUNO_HXX "$Revision: 1.4 $"

#ifndef _RTL_USTRING_
#include <rtl/ustring.h>
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_XINTERFACE_HPP_
#include <com/sun/star/uno/XInterface.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTY_HPP_
#include <com/sun/star/beans/Property.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_PROPERTYVALUEINFO_HPP_
#include <com/sun/star/ucb/PropertyValueInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCONTENT_HPP_
#include <com/sun/star/ucb/XContent.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XCOMMANDPROCESSOR_HPP_
#include <com/sun/star/ucb/XCommandProcessor.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XROW_HPP_
#include <com/sun/star/sdbc/XRow.hpp>
#endif

using com::sun::star::beans::PropertyValue;
using com::sun::star::beans::Property;
using com::sun::star::ucb::XContent;
using com::sun::star::ucb::XCommandProcessor;

namespace binfilter {

/*========================================================================
 *
 * SvBindingTransport_Impl interface.
 *
 *======================================================================*/
class SvBindingTransport_Impl
{
public:
	/** getProcessServiceFactory.
	 */
	static com::sun::star::uno::Reference<com::sun::star::uno::XInterface>
	getProcessServiceFactory (void);

	/** createContent.
	 */
	static com::sun::star::uno::Reference<XContent>
	createContent (
		const rtl::OUString &rUrl);

	/** getProperties.
	 */
    static com::sun::star::uno::Any
	getProperties (
        const com::sun::star::uno::Reference<XCommandProcessor> &rxProc,
        const com::sun::star::uno::Sequence<Property>           &rxProp);

	/** setProperties.
	 */
	static void setProperties (
        const com::sun::star::uno::Reference<XCommandProcessor> &rxProc,
        const com::sun::star::uno::Sequence<PropertyValue>           &rxProp);
};

/*========================================================================
 *
 * The End.
 *
 *======================================================================*/

}

#endif /* !_TRANSUNO_HXX */
