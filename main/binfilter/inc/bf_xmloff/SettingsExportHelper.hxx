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



#ifndef _XMLOFF_SETTINGSEXPORTHELPER_HXX
#define _XMLOFF_SETTINGSEXPORTHELPER_HXX

#ifndef _COM_SUN_STAR_AWT_RECTANGLE_HPP_
#include <com/sun/star/awt/Rectangle.hpp>
#endif
#ifndef _COM_SUN_STAR_FORMULA_SYMBOLDESCRIPTOR_HPP_
#include <com/sun/star/formula/SymbolDescriptor.hpp>
#endif

#ifndef _XMLOFF_XMLEXP_HXX
#include <bf_xmloff/xmlexp.hxx>
#endif

namespace com
{
	namespace sun { namespace star {
		namespace container { class XNameAccess; class XIndexAccess; }
		namespace util { struct DateTime; }
	} }
}
namespace binfilter {

class XMLSettingsExportHelper
{
	SvXMLExport&	rExport;

	void CallTypeFunction(const ::com::sun::star::uno::Any& rAny,
						const ::rtl::OUString& rName) const;

	void exportBool(const sal_Bool bValue, const ::rtl::OUString& rName) const;
	void exportByte(const sal_Int8 nValue, const ::rtl::OUString& rName) const;
	void exportShort(const sal_Int16 nValue, const ::rtl::OUString& rName) const;
	void exportInt(const sal_Int32 nValue, const ::rtl::OUString& rName) const;
	void exportLong(const sal_Int64 nValue, const ::rtl::OUString& rName) const;
	void exportDouble(const double fValue, const ::rtl::OUString& rName) const;
	void exportString(const ::rtl::OUString& sValue, const ::rtl::OUString& rName) const;
	void exportDateTime(const ::com::sun::star::util::DateTime& aValue, const ::rtl::OUString& rName) const;
	void exportSequencePropertyValue(
		const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue>& aProps,
		const ::rtl::OUString& rName) const;
	void exportbase64Binary(
		const ::com::sun::star::uno::Sequence<sal_Int8>& aProps,
		const ::rtl::OUString& rName) const;
	void exportMapEntry(const ::com::sun::star::uno::Any& rAny,
						const ::rtl::OUString& rName,
						const sal_Bool bNameAccess) const;
	void exportNameAccess(
		const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess>& aNamed,
		const ::rtl::OUString& rName) const;
	void exportIndexAccess(
		const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess> aIndexed,
		const ::rtl::OUString rName) const;

	void exportSymbolDescriptors(
					const ::com::sun::star::uno::Sequence < ::com::sun::star::formula::SymbolDescriptor > &rProps,
					const ::rtl::OUString rName) const;
	void exportForbiddenCharacters(
					const ::com::sun::star::uno::Any &rAny,
					const ::rtl::OUString rName) const;

public:
	XMLSettingsExportHelper(SvXMLExport& rExport);
	~XMLSettingsExportHelper();

	void exportSettings(
		const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue>& aProps,
		const ::rtl::OUString& rName) const;
};

}//end of namespace binfilter
#endif

