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



#ifdef PCH
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif

//___________________________________________________________________

#ifndef _SC_XMLCONSOLIDATIONCONTEXT_HXX
#include "XMLConsolidationContext.hxx"
#endif

#ifndef SC_DOCUMENT_HXX
#include "document.hxx"
#endif
#ifndef SC_RANGEUTL_HXX
#include "rangeutl.hxx"
#endif
#ifndef SC_XMLIMPRT_HXX
#include "xmlimprt.hxx"
#endif
#ifndef _SC_XMLCONVERTER_HXX
#include "XMLConverter.hxx"
#endif

#ifndef _XMLOFF_NMSPMAP_HXX
#include <bf_xmloff/nmspmap.hxx>
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace xmloff::token;


//___________________________________________________________________

ScXMLConsolidationContext::ScXMLConsolidationContext(
		ScXMLImport& rImport,
		USHORT nPrfx,
		const OUString& rLName,
		const uno::Reference< xml::sax::XAttributeList >& xAttrList ) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	eFunction( SUBTOTAL_FUNC_NONE ),
	bLinkToSource( sal_False ),
	bTargetAddr(sal_False)
{
	rImport.LockSolarMutex();
	if( !xAttrList.is() ) return;

	sal_Int16				nAttrCount		= xAttrList->getLength();
	const SvXMLTokenMap&	rAttrTokenMap	= GetScImport().GetConsolidationAttrTokenMap();

	for( sal_Int16 nIndex = 0; nIndex < nAttrCount; nIndex++ )
	{
		OUString sAttrName	= xAttrList->getNameByIndex( nIndex );
		OUString sValue		= xAttrList->getValueByIndex( nIndex );
		OUString aLocalName;
		USHORT nPrefix		= GetScImport().GetNamespaceMap().GetKeyByAttrName( sAttrName, &aLocalName );

		switch( rAttrTokenMap.Get( nPrefix, aLocalName ) )
		{
			case XML_TOK_CONSOLIDATION_ATTR_FUNCTION:
				eFunction = ScXMLConverter::GetSubTotalFuncFromString( sValue );
			break;
			case XML_TOK_CONSOLIDATION_ATTR_SOURCE_RANGES:
				sSourceList = sValue;
			break;
			case XML_TOK_CONSOLIDATION_ATTR_TARGET_ADDRESS:
				{
					sal_Int32 nOffset(0);
					bTargetAddr = ScXMLConverter::GetAddressFromString(
						aTargetAddr, sValue, GetScImport().GetDocument(), nOffset );
				}
				break;
			case XML_TOK_CONSOLIDATION_ATTR_USE_LABEL:
				sUseLabel = sValue;
			break;
			case XML_TOK_CONSOLIDATION_ATTR_LINK_TO_SOURCE:
				bLinkToSource = IsXMLToken(sValue, XML_TRUE);
			break;
		}
	}
}

ScXMLConsolidationContext::~ScXMLConsolidationContext()
{
}

SvXMLImportContext *ScXMLConsolidationContext::CreateChildContext(
		USHORT nPrefix,
		const OUString& rLName,
		const uno::Reference< xml::sax::XAttributeList>& xAttrList )
{
	return new SvXMLImportContext( GetImport(), nPrefix, rLName );
}

void ScXMLConsolidationContext::EndElement()
{
	if (bTargetAddr)
	{
		ScConsolidateParam aConsParam;
		aConsParam.nCol = aTargetAddr.Col();
		aConsParam.nRow = aTargetAddr.Row();
		aConsParam.nTab = aTargetAddr.Tab();
		aConsParam.eFunction = eFunction;

		sal_Bool bError = sal_False;
		USHORT nCount = (USHORT) Min( ScXMLConverter::GetTokenCount( sSourceList ), (sal_Int32)0xFFFF );
		ScArea** ppAreas = nCount ? new ScArea*[ nCount ] : NULL;
		if( ppAreas )
		{
			sal_Int32 nOffset = 0;
			USHORT nIndex;
			for( nIndex = 0; nIndex < nCount; nIndex++ )
			{
				ppAreas[ nIndex ] = new ScArea;
				if ( !ScXMLConverter::GetAreaFromString(
					*ppAreas[ nIndex ], sSourceList, GetScImport().GetDocument(), nOffset ) )
				{
					bError = sal_True;		//! handle error
				}
			}

			aConsParam.SetAreas( ppAreas, nCount );

			// array is copied in SetAreas
			for( nIndex = 0; nIndex < nCount; nIndex++ )
				delete ppAreas[nIndex];
			delete[] ppAreas;
		}

		aConsParam.bByCol = aConsParam.bByRow = FALSE;
		if( IsXMLToken(sUseLabel, XML_COLUMN ) )
			aConsParam.bByCol = TRUE;
		else if( IsXMLToken( sUseLabel, XML_ROW ) )
			aConsParam.bByRow = TRUE;
		else if( IsXMLToken( sUseLabel, XML_BOTH ) )
			aConsParam.bByCol = aConsParam.bByRow = TRUE;

		aConsParam.bReferenceData = bLinkToSource;

		ScDocument* pDoc = GetScImport().GetDocument();
		if( pDoc )
			pDoc->SetConsolidateDlgData( &aConsParam );
	}
	GetScImport().UnlockSolarMutex();
}

}
