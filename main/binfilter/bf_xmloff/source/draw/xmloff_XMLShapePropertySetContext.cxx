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



#ifdef _MSC_VER
#pragma hdrstop
#endif

#ifndef _XMLOFF_XMLSHAPEPROPERTYSETCONTEXT_HXX_
#include "XMLShapePropertySetContext.hxx"
#endif
#ifndef _XMLOFF_XMLIMP_HXX
#include "xmlimp.hxx"
#endif
#ifndef _XMLOFF_XMLNUMI_HXX 
#include "xmlnumi.hxx"
#endif
#ifndef _XMLOFF_XMLTABI_HXX 
#include "xmltabi.hxx"
#endif
#ifndef _XMLOFF_TEXTPRMAP_HXX_ 
#include "txtprmap.hxx"
#endif

#include "sdpropls.hxx"
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

//////////////////////////////////////////////////////////////////////////////

TYPEINIT1( XMLShapePropertySetContext, SvXMLPropertySetContext );

XMLShapePropertySetContext::XMLShapePropertySetContext(
				 SvXMLImport& rImport, sal_uInt16 nPrfx,
				 const OUString& rLName,
				 const Reference< xml::sax::XAttributeList > & xAttrList,
				 ::std::vector< XMLPropertyState > &rProps,
				 const UniReference < SvXMLImportPropertyMapper > &rMap ) :
	SvXMLPropertySetContext( rImport, nPrfx, rLName, xAttrList, rProps, rMap ),
	mnBulletIndex(-1)
{
}

XMLShapePropertySetContext::~XMLShapePropertySetContext()
{
}

void XMLShapePropertySetContext::EndElement()
{
	Reference< container::XIndexReplace > xNumRule;
	if( mxBulletStyle.Is() )
	{
		SvxXMLListStyleContext* pBulletStyle = (SvxXMLListStyleContext*)&mxBulletStyle;
		xNumRule = pBulletStyle->CreateNumRule( GetImport().GetModel() );
		pBulletStyle->FillUnoNumRule(xNumRule, NULL /* const SvI18NMap * ??? */ );
	}

	Any aAny;
	aAny <<= xNumRule;

	XMLPropertyState aPropState( mnBulletIndex, aAny );
	rProperties.push_back( aPropState );

	SvXMLPropertySetContext::EndElement();
}
	
SvXMLImportContext *XMLShapePropertySetContext::CreateChildContext(
				   sal_uInt16 nPrefix,
				   const OUString& rLocalName,
				   const Reference< xml::sax::XAttributeList > & xAttrList,
				   ::std::vector< XMLPropertyState > &rProperties,
				   const XMLPropertyState& rProp )
{
	SvXMLImportContext *pContext = 0;

	switch( xMapper->getPropertySetMapper()->GetEntryContextId( rProp.mnIndex ) )
	{
	case CTF_NUMBERINGRULES:
		mnBulletIndex = rProp.mnIndex;
		mxBulletStyle = pContext = new SvxXMLListStyleContext( GetImport(), nPrefix, rLocalName, xAttrList );
		break;
	case CTF_TABSTOP:
		pContext = new SvxXMLTabStopImportContext( GetImport(), nPrefix,
												   rLocalName, rProp,
												   rProperties );
		break;
	}
	
	if( !pContext )
		pContext = SvXMLPropertySetContext::CreateChildContext( nPrefix, rLocalName,
															xAttrList, 
															rProperties, rProp );
	
	return pContext;
}
}//end of namespace binfilter
