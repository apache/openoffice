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

#ifndef _SC_XMLTEXTPCONTEXT_HXX
#include "XMLTextPContext.hxx"
#endif
#ifndef SC_XMLIMPRT_HXX
#include "xmlimprt.hxx"
#endif
#ifndef SC_XMLCELLI_HXX
#include "xmlcelli.hxx"
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include <bf_xmloff/xmlnmspe.hxx>
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include <bf_xmloff/nmspmap.hxx>
#endif

namespace binfilter {

using namespace ::com::sun::star;
using namespace xmloff::token;

class ScXMLTextTContext : public SvXMLImportContext
{
	const ScXMLImport& GetScImport() const { return (const ScXMLImport&)GetImport(); }
	ScXMLImport& GetScImport() { return (ScXMLImport&)GetImport(); }
public:
	ScXMLTextTContext( ScXMLImport& rImport, USHORT nPrfx,
						const ::rtl::OUString& rLName,
						const ::com::sun::star::uno::Reference<
										::com::sun::star::xml::sax::XAttributeList>& xAttrList,
						ScXMLTextPContext* pTextPContext);

	virtual ~ScXMLTextTContext();
};


ScXMLTextTContext::ScXMLTextTContext( ScXMLImport& rImport,
									  USHORT nPrfx,
									  const ::rtl::OUString& rLName,
									  const ::com::sun::star::uno::Reference<
									  ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
									  ScXMLTextPContext* pTextPContext) :
	SvXMLImportContext( rImport, nPrfx, rLName )
{
	if (pTextPContext)
	{
		sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
		::rtl::OUString aLocalName;
		::rtl::OUString sValue;
		sal_Int32 nCount(1);
		for( sal_Int16 i=0; i < nAttrCount; i++ )
		{
			sal_uInt16 nPrefix = GetScImport().GetNamespaceMap().GetKeyByAttrName(
												xAttrList->getNameByIndex( i ), &aLocalName );
			sValue = xAttrList->getValueByIndex( i );

			if ((nPrefix == XML_NAMESPACE_TEXT) && IsXMLToken(aLocalName, XML_C))
				nCount = sValue.toInt32();
		}
		pTextPContext->AddSpaces(nCount);
	}
}

ScXMLTextTContext::~ScXMLTextTContext()
{
}

//------------------------------------------------------------------

ScXMLTextPContext::ScXMLTextPContext( ScXMLImport& rImport,
									  USHORT nPrfx,
									  const ::rtl::OUString& rLName,
									  const ::com::sun::star::uno::Reference<
									  ::com::sun::star::xml::sax::XAttributeList>& xTempAttrList,
									  ScXMLTableRowCellContext* pTempCellContext) :
	SvXMLImportContext( rImport, nPrfx, rLName ),
	pTextPContext(NULL),
	pCellContext(pTempCellContext),
	sOUText(),
	sLName(rLName),
	xAttrList(xTempAttrList),
	nPrefix(nPrfx),
	bIsOwn(sal_True)
{
	// here are no attributes
}

ScXMLTextPContext::~ScXMLTextPContext()
{
	if (pTextPContext)
		delete pTextPContext;
}

void ScXMLTextPContext::AddSpaces(sal_Int32 nSpaceCount)
{
	sal_Char* pChars = new sal_Char[nSpaceCount];
	memset(pChars, ' ', nSpaceCount);
	sOUText.appendAscii(pChars, nSpaceCount);
}

SvXMLImportContext *ScXMLTextPContext::CreateChildContext( USHORT nTempPrefix,
											const ::rtl::OUString& rLName,
											const ::com::sun::star::uno::Reference<
									  	::com::sun::star::xml::sax::XAttributeList>& xTempAttrList )
{
	SvXMLImportContext *pContext = NULL;
	if (!pTextPContext &&
		(nTempPrefix == XML_NAMESPACE_TEXT) &&
		IsXMLToken(rLName, XML_S))
		pContext = new ScXMLTextTContext( GetScImport(), nTempPrefix, rLName, xTempAttrList, this);
	else
	{
		if (!pTextPContext)
		{
			pCellContext->SetCursorOnTextImport(sOUText.makeStringAndClear());
			pTextPContext = GetScImport().GetTextImport()->CreateTextChildContext(
									GetScImport(), nPrefix, sLName, xAttrList);
		}
		if (pTextPContext)
			pContext = pTextPContext->CreateChildContext(nTempPrefix, rLName, xTempAttrList);
	}

	if( !pContext )
		pContext = new SvXMLImportContext( GetScImport(), nTempPrefix, rLName );

	return pContext;
}

void ScXMLTextPContext::Characters( const ::rtl::OUString& rChars )
{
	if (!pTextPContext)
		sOUText.append(rChars);
	else
		pTextPContext->Characters(rChars);
}

void ScXMLTextPContext::EndElement()
{
	if (!pTextPContext)
		pCellContext->SetString(sOUText.makeStringAndClear());
//		GetScImport().GetTextImport()->GetCursor()->setString(sOUText.makeStringAndClear());
	else
	{
		pTextPContext->EndElement();
		GetScImport().SetRemoveLastChar(sal_True);
	}
}

}
