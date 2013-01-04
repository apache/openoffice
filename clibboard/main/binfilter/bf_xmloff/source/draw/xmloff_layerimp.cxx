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



#include <tools/debug.hxx>

#ifndef _COM_SUN_STAR_DRAWING_XLAYERMANAGER_HPP_
#include <com/sun/star/drawing/XLayerManager.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XLAYERSUPPLIER_HPP_
#include <com/sun/star/drawing/XLayerSupplier.hpp>
#endif



#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmlnmspe.hxx"
#endif


#ifndef _XMLOFF_NMSPMAP_HXX
#include "nmspmap.hxx"
#endif

#ifndef _XMLOFF_LAYERIMP_HXX
#include "layerimp.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::std;
using namespace ::cppu;
using namespace ::com::sun::star;
using namespace ::com::sun::star::xml;
using namespace ::com::sun::star::xml::sax;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::drawing;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::container;
using ::binfilter::xmloff::token::IsXMLToken;
using ::binfilter::xmloff::token::XML_NAME;


TYPEINIT1( SdXMLLayerSetContext, SvXMLImportContext );

SdXMLLayerSetContext::SdXMLLayerSetContext( SvXMLImport& rImport, sal_uInt16 nPrfx,	const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList)
: SvXMLImportContext(rImport, nPrfx, rLocalName)
{
	Reference< XLayerSupplier > xLayerSupplier( rImport.GetModel(), UNO_QUERY );
	DBG_ASSERT( xLayerSupplier.is(), "XModel is not supporting XLayerSupplier!" );
	if( xLayerSupplier.is() )
		mxLayerManager = xLayerSupplier->getLayerManager();
}

SdXMLLayerSetContext::~SdXMLLayerSetContext()
{
}

SvXMLImportContext * SdXMLLayerSetContext::CreateChildContext( USHORT nPrefix, const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
	if( mxLayerManager.is() )
	{
		const OUString strName( RTL_CONSTASCII_USTRINGPARAM( "Name" ) );

		OUString aName;

		const sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
		for(sal_Int16 i=0; i < nAttrCount; i++)
		{
			OUString aLocalName;
			if( GetImport().GetNamespaceMap().GetKeyByAttrName( xAttrList->getNameByIndex( i ), &aLocalName ) == XML_NAMESPACE_DRAW )
			{
				const OUString sValue( xAttrList->getValueByIndex( i ) );

				if( IsXMLToken( aLocalName, XML_NAME ) )
				{
					aName = sValue;
				}
			}
		}

		DBG_ASSERT( aName.getLength(), "draw:layer element without draw:name!" );
		if( aName.getLength() )
		{
			Reference< XPropertySet > xLayer;

			if( mxLayerManager->hasByName( aName ) )
			{
				mxLayerManager->getByName( aName ) >>= xLayer;
				DBG_ASSERT( xLayer.is(), "failed to get existing XLayer!" );
			}
			else
			{
				Reference< XLayerManager > xLayerManager( mxLayerManager, UNO_QUERY );
				if( xLayerManager.is() )
					xLayer = Reference< XPropertySet >::query( xLayerManager->insertNewByIndex( xLayerManager->getCount() ) );
				DBG_ASSERT( xLayer.is(), "failed to create new XLayer!" );

				if( xLayer.is() )
				{
					Any aAny;
					aAny <<= aName;
					xLayer->setPropertyValue( strName, aAny );
				}
			}
		}
	}

	return new SvXMLImportContext( GetImport(), nPrefix, rLocalName );
}
}//end of namespace binfilter
