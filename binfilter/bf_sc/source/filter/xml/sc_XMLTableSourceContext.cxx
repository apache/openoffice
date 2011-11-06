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

// INCLUDE ---------------------------------------------------------------

#ifndef _SC_XMLTABLESOURCECONTEXT_HXX
#include "XMLTableSourceContext.hxx"
#endif
#ifndef SC_XMLIMPRT_HXX
#include "xmlimprt.hxx"
#endif
#ifndef SC_DOCUMENT_HXX
#include "document.hxx"
#endif
#ifndef SC_TABLINK_HXX
#include "tablink.hxx"
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include <bf_xmloff/xmlnmspe.hxx>
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include <bf_xmloff/nmspmap.hxx>
#endif
#ifndef _XMLOFF_XMLUCONV_HXX
#include <bf_xmloff/xmluconv.hxx>
#endif

#ifndef _COM_SUN_STAR_SHEET_XSHEETLINKABLE_HPP_
#include <com/sun/star/sheet/XSheetLinkable.hpp>
#endif
namespace binfilter {

using namespace ::com::sun::star;
using namespace xmloff::token;

//------------------------------------------------------------------

ScXMLTableSourceContext::ScXMLTableSourceContext( ScXMLImport& rImport,
									  USHORT nPrfx,
									  const ::rtl::OUString& rLName,
									  const ::com::sun::star::uno::Reference<
									  ::com::sun::star::xml::sax::XAttributeList>& xAttrList) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	sLink(),
	sTableName(),
	sFilterName(),
	sFilterOptions(),
	nRefresh(0),
	nMode(sheet::SheetLinkMode_NORMAL)
{
	sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
	for( sal_Int16 i=0; i < nAttrCount; i++ )
	{
		::rtl::OUString sAttrName = xAttrList->getNameByIndex( i );
		::rtl::OUString aLocalName;
		sal_uInt16 nPrefix = GetScImport().GetNamespaceMap().GetKeyByAttrName(
											sAttrName, &aLocalName );
		::rtl::OUString sValue = xAttrList->getValueByIndex( i );
		if(nPrefix == XML_NAMESPACE_XLINK)
		{
			if (IsXMLToken(aLocalName, XML_HREF))
				sLink = GetScImport().GetAbsoluteReference(sValue);
		}
		else if (nPrefix == XML_NAMESPACE_TABLE)
		{
			if (IsXMLToken(aLocalName, XML_TABLE_NAME))
				sTableName = sValue;
			else if (IsXMLToken(aLocalName, XML_FILTER_NAME))
				sFilterName = sValue;
			else if (IsXMLToken(aLocalName, XML_FILTER_OPTIONS))
				sFilterOptions = sValue;
			else if (IsXMLToken(aLocalName, XML_MODE))
			{
				if (IsXMLToken(sValue, XML_COPY_RESULTS_ONLY))
					nMode = sheet::SheetLinkMode_VALUE;
			}
			else if (IsXMLToken(aLocalName, XML_REFRESH_DELAY))
			{
				double fTime;
				if( SvXMLUnitConverter::convertTime( fTime, sValue ) )
					nRefresh = Max( (sal_Int32)(fTime * 86400.0), (sal_Int32)0 );
			}
		}
	}
}

ScXMLTableSourceContext::~ScXMLTableSourceContext()
{
}

SvXMLImportContext *ScXMLTableSourceContext::CreateChildContext( USHORT nPrefix,
											const ::rtl::OUString& rLName,
											const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xAttrList )
{
	SvXMLImportContext *pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );
	return pContext;
}

void ScXMLTableSourceContext::EndElement()
{
	if (sLink.getLength())
	{
		uno::Reference <sheet::XSheetLinkable> xLinkable (GetScImport().GetTables().GetCurrentXSheet(), uno::UNO_QUERY);
		ScDocument* pDoc = GetScImport().GetDocument();
		if (xLinkable.is() && pDoc)
		{
			GetScImport().LockSolarMutex();
			if (pDoc->RenameTab( GetScImport().GetTables().GetCurrentSheet(),
				GetScImport().GetTables().GetCurrentSheetName(), sal_False, sal_True))
			{
		 		String aFileString(sLink);
				String aFilterString(sFilterName);
				String aOptString(sFilterOptions);
				String aSheetString(sTableName);

				aFileString = ScGlobal::GetAbsDocName( aFileString, pDoc->GetDocumentShell() );
				if ( !aFilterString.Len() )
					ScDocumentLoader::GetFilterName( aFileString, aFilterString, aOptString );

				BYTE nLinkMode = SC_LINK_NONE;
				if ( nMode == sheet::SheetLinkMode_NORMAL )
					nLinkMode = SC_LINK_NORMAL;
				else if ( nMode == sheet::SheetLinkMode_VALUE )
					nLinkMode = SC_LINK_VALUE;

				pDoc->SetLink( GetScImport().GetTables().GetCurrentSheet(),
					nLinkMode, aFileString, aFilterString, aOptString,
					aSheetString, nRefresh );
			}
			GetScImport().UnlockSolarMutex();
		}
	}
}

}
