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



#ifndef _XMLOFF_METAEXPORTCOMPONENT_HXX
#define _XMLOFF_METAEXPORTCOMPONENT_HXX

#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif
namespace binfilter {

class XMLMetaExportComponent : public SvXMLExport
{
public:
	// #110680#
	// XMLMetaExportComponent();
	XMLMetaExportComponent(
		const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory
		);

	~XMLMetaExportComponent();

protected:
	// export the events off all autotexts
	virtual sal_uInt32 exportDoc(
        enum ::binfilter::xmloff::token::XMLTokenEnum eClass = xmloff::token::XML_TOKEN_INVALID );

	// methods without content:
	virtual void _ExportAutoStyles();
	virtual void _ExportMasterStyles();
	virtual void _ExportContent();
};

// global functions to support the component

::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
	XMLMetaExportComponent_getSupportedServiceNames()
	throw();

::rtl::OUString SAL_CALL XMLMetaExportComponent_getImplementationName()
	throw();

::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
	XMLMetaExportComponent_createInstance(
		const ::com::sun::star::uno::Reference<
			::com::sun::star::lang::XMultiServiceFactory > & )
	throw( ::com::sun::star::uno::Exception );

}//end of namespace binfilter
#endif

