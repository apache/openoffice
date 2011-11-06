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



#ifndef _XMLOFF_FORMS_CONTROLPROPERTYHDL_HXX_
#define _XMLOFF_FORMS_CONTROLPROPERTYHDL_HXX_

#ifndef _XMLOFF_PROPERTYHANDLERFACTORY_HXX
#include "prhdlfac.hxx"
#endif
#ifndef _VOS_REF_HXX_
#include <vos/ref.hxx>
#endif
#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _XMLOFF_XMLEXPPR_HXX
#include "xmlexppr.hxx"
#endif
#ifndef _XMLOFF_CONTROLBORDERHDL_HXX_
#include "ControlBorderHandler.hxx"
#endif
namespace binfilter {

struct SvXMLEnumMapEntry;
class XMLConstantsPropertyHandler;
//.........................................................................
namespace xmloff
{
//.........................................................................

	class IFormsExportContext;
	class IFormsImportContext;

	//=====================================================================
	//= ORotationAngleHandler
	//=====================================================================
	class ORotationAngleHandler : public XMLPropertyHandler
	{
	public:
		ORotationAngleHandler();

		virtual sal_Bool importXML( const ::rtl::OUString& _rStrImpValue, ::com::sun::star::uno::Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const;
		virtual sal_Bool exportXML( ::rtl::OUString& _rStrExpValue, const ::com::sun::star::uno::Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const;
	};

	//=====================================================================
	//= OFontWidthHandler
	//=====================================================================
	class OFontWidthHandler : public XMLPropertyHandler
	{
	public:
		OFontWidthHandler();

		virtual sal_Bool importXML( const ::rtl::OUString& _rStrImpValue, ::com::sun::star::uno::Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const;
		virtual sal_Bool exportXML( ::rtl::OUString& _rStrExpValue, const ::com::sun::star::uno::Any& _rValue, const SvXMLUnitConverter& _rUnitConverter ) const;
	};

	//=====================================================================
	//= OControlPropertyHandlerFactory
	//=====================================================================
	class OControlPropertyHandlerFactory : public XMLPropertyHandlerFactory
	{
	protected:
		mutable XMLConstantsPropertyHandler*	m_pTextAlignHandler;
		mutable OControlBorderHandler*			m_pControlBorderHandler;
		mutable ORotationAngleHandler*			m_pRotationAngleHandler;
		mutable OFontWidthHandler*				m_pFontWidthHandler;
		mutable XMLConstantsPropertyHandler*	m_pFontEmphasisHandler;
		mutable XMLConstantsPropertyHandler*	m_pFontReliefHandler;

	public:
		OControlPropertyHandlerFactory();
		virtual ~OControlPropertyHandlerFactory();

		virtual const XMLPropertyHandler* GetPropertyHandler(sal_Int32 _nType) const;
	};

//.........................................................................
}	// namespace xmloff
//.........................................................................

}//end of namespace binfilter
#endif // _XMLOFF_FORMS_CONTROLPROPERTYHDL_HXX_

