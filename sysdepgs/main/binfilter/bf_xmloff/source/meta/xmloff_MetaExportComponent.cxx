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
#include "MetaExportComponent.hxx"
#endif







// #110680#
//#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
//#include <comphelper/processfactory.hxx>
//#endif


#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif

#ifndef _XMLOFF_NMSPMAP_HXX
#include "nmspmap.hxx"
#endif


#ifndef _XMLOFF_XMLMETAE_HXX
#include "xmlmetae.hxx"
#endif

namespace binfilter {


using namespace ::com::sun::star;
using namespace ::binfilter::xmloff::token;

// #110680#
XMLMetaExportComponent::XMLMetaExportComponent(
	const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory)
:	SvXMLExport( xServiceFactory, MAP_INCH, XML_META )
{
}

XMLMetaExportComponent::~XMLMetaExportComponent()
{
}

sal_uInt32 XMLMetaExportComponent::exportDoc( enum XMLTokenEnum eClass )
{
	GetDocHandler()->startDocument();
	{
		GetAttrList().AddAttribute(
			GetNamespaceMap().GetAttrNameByKey( XML_NAMESPACE_DC ),
			GetNamespaceMap().GetNameByKey( XML_NAMESPACE_DC ) );
		GetAttrList().AddAttribute(
			GetNamespaceMap().GetAttrNameByKey( XML_NAMESPACE_META ),
			GetNamespaceMap().GetNameByKey( XML_NAMESPACE_META ) );
		GetAttrList().AddAttribute(
			GetNamespaceMap().GetAttrNameByKey( XML_NAMESPACE_OFFICE ),
			GetNamespaceMap().GetNameByKey( XML_NAMESPACE_OFFICE ) );

		SvXMLElementExport aDocElem( *this, XML_NAMESPACE_OFFICE, XML_DOCUMENT_META,
						sal_True, sal_True );
		{

			SvXMLElementExport aElem( *this, XML_NAMESPACE_OFFICE, XML_META,
							sal_True, sal_True );
			SfxXMLMetaExport aMeta( *this, GetModel() );
			aMeta.Export();
		}
	}
	GetDocHandler()->endDocument();
	return 0;
}

// methods without content:
void XMLMetaExportComponent::_ExportAutoStyles() {}
void XMLMetaExportComponent::_ExportMasterStyles() {}
void XMLMetaExportComponent::_ExportContent() {}


uno::Sequence< ::rtl::OUString > SAL_CALL XMLMetaExportComponent_getSupportedServiceNames()
	throw()
{
	const ::rtl::OUString aServiceName(
		RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.document.XMLMetaExporter" ) );
	const uno::Sequence< ::rtl::OUString > aSeq( &aServiceName, 1 );
	return aSeq;
}

rtl::OUString SAL_CALL XMLMetaExportComponent_getImplementationName() throw()
{
	return ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "XMLMetaExportComponent" ) );
}

uno::Reference< uno::XInterface > SAL_CALL XMLMetaExportComponent_createInstance(
		const uno::Reference< lang::XMultiServiceFactory > & rSMgr)
	throw( uno::Exception )
{
	// #110680#
	// return (cppu::OWeakObject*)new XMLMetaExportComponent;
	return (cppu::OWeakObject*)new XMLMetaExportComponent(rSMgr);
}

}//end of namespace binfilter
