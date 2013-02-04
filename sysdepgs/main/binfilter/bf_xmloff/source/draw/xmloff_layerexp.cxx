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




#ifndef _COM_SUN_STAR_DRAWING_XLAYERSUPPLIER_HPP_ 
#include <com/sun/star/drawing/XLayerSupplier.hpp>
#endif




#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif

#ifndef _XMLOFF_XMLEXP_HXX
#include "xmlexp.hxx"
#endif



#ifndef _XMLOFF_LAYEREXP_HXX
#include "layerexp.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::cppu;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::drawing;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::container;
using namespace ::binfilter::xmloff::token;

void SdXMLayerExporter::exportLayer( SvXMLExport& rExport )
{
	Reference< XLayerSupplier > xLayerSupplier( rExport.GetModel(), UNO_QUERY );
	if( !xLayerSupplier.is() )
		return;

	Reference< XIndexAccess > xLayerManager( xLayerSupplier->getLayerManager(), UNO_QUERY );
	if( !xLayerManager.is() )
		return;

	const sal_Int32 nCount = xLayerManager->getCount();
	if( nCount == 0 )
		return;

	Reference< XPropertySet> xLayer;
	const OUString strName( RTL_CONSTASCII_USTRINGPARAM( "Name" ) );

	OUStringBuffer sTmp;
	OUString aName;

	SvXMLElementExport aElem( rExport, XML_NAMESPACE_DRAW, XML_LAYER_SET, sal_True, sal_True );

	for( sal_Int32 nIndex = 0; nIndex < nCount; nIndex++ )
	{
		xLayerManager->getByIndex( nIndex ) >>= xLayer;

		if( xLayer.is() )
		{
			if( xLayer->getPropertyValue( strName ) >>= aName )
			{
				rExport.AddAttribute( XML_NAMESPACE_DRAW, XML_NAME, aName );
			}

			SvXMLElementExport aElem( rExport, XML_NAMESPACE_DRAW, XML_LAYER, sal_True, sal_True );
		}	  
	}
}
}//end of namespace binfilter
