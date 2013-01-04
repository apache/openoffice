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



#ifndef _XMLOFF_XMLINDEXSPANENTRYCONTEXT_HXX_
#define _XMLOFF_XMLINDEXSPANENTRYCONTEXT_HXX_

#ifndef _XMLOFF_XMLINDEXSIMPLEENTRYCONTEXT_HXX_
#include "XMLIndexSimpleEntryContext.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_ 
#include <com/sun/star/uno/Reference.h>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_H_
#include <com/sun/star/uno/Sequence.h>
#endif

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

namespace com { namespace sun { namespace star {
	namespace xml { namespace sax { class XAttributeList; } }
} } }
namespace rtl {	class OUString; }
namespace binfilter {

/**
 * Import index entry templates
 */
class XMLIndexSpanEntryContext : public XMLIndexSimpleEntryContext
{
	::rtl::OUStringBuffer sContent;

public:

	TYPEINFO();

	XMLIndexSpanEntryContext(
		SvXMLImport& rImport, 
		XMLIndexTemplateContext& rTemplate,
		sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName );

	~XMLIndexSpanEntryContext();

protected:

	/// Collect element contents
	virtual void Characters(const ::rtl::OUString& sString);

	/// add Text PropertyValue
	virtual void FillPropertyValues(
		::com::sun::star::uno::Sequence<
			::com::sun::star::beans::PropertyValue> & rValues);
};

}//end of namespace binfilter
#endif
