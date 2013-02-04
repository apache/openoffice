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



#ifndef _SC_XMLCALCULATIONSETTINGSCONTEXT_HXX
#define _SC_XMLCALCULATIONSETTINGSCONTEXT_HXX

#ifndef _XMLOFF_XMLICTXT_HXX
#include <bf_xmloff/xmlictxt.hxx>
#endif

#ifndef _COM_SUN_STAR_UTIL_DATE_HPP_
#include <com/sun/star/util/Date.hpp>
#endif
namespace binfilter {

class ScXMLImport;

class ScXMLCalculationSettingsContext : public SvXMLImportContext
{
	::com::sun::star::util::Date aNullDate;
	double fIterationEpsilon;
	sal_Int32 nIterationCount;
	sal_uInt16 nYear2000;
	sal_Bool bIsIterationEnabled : 1;
	sal_Bool bCalcAsShown : 1;
	sal_Bool bIgnoreCase : 1;
	sal_Bool bLookUpLabels : 1;
	sal_Bool bMatchWholeCell : 1;
	sal_Bool bUseRegularExpressions : 1;

	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }
public:
	ScXMLCalculationSettingsContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList);

	virtual ~ScXMLCalculationSettingsContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	void SetNullDate(const ::com::sun::star::util::Date& aDate) { aNullDate = aDate; }
	void SetIterationStatus(const sal_Bool bValue) { bIsIterationEnabled = bValue; }
	void SetIterationCount(const sal_Int32 nValue) { nIterationCount = nValue; }
	void SetIterationEpsilon(const double fValue) { fIterationEpsilon = fValue; }
	virtual void EndElement();
};

class ScXMLNullDateContext : public SvXMLImportContext
{
	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }
public:
	ScXMLNullDateContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList, ScXMLCalculationSettingsContext* pCalcSet);

	virtual ~ScXMLNullDateContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

class ScXMLIterationContext : public SvXMLImportContext
{
	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }
public:
	ScXMLIterationContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList, ScXMLCalculationSettingsContext* pCalcSet);

	virtual ~ScXMLIterationContext();

	virtual SvXMLImportContext *CreateChildContext( USHORT nPrefix,
									 const ::rtl::OUString& rLocalName,
									 const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	virtual void EndElement();
};

} //namespace binfilter
#endif
