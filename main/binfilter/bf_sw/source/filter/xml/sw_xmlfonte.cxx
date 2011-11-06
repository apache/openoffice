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

#ifndef _HINTIDS_HXX
#include "hintids.hxx"
#endif

#ifndef _COM_SUN_STAR_TEXT_XTEXTDOCUMENT_HPP_ 
#include <com/sun/star/text/XTextDocument.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXT_HPP_ 
#include <com/sun/star/text/XText.hpp>
#endif

#ifndef _COM_SUN_STAR_TEXT_XTEXTDOCUMENT_HPP_ 
#include <com/sun/star/text/XTextDocument.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXT_HPP_ 
#include <com/sun/star/text/XText.hpp>
#endif
#ifndef _XMLOFF_XMLFONTAUTOSTYLEPOOL_HXX
#include <bf_xmloff/XMLFontAutoStylePool.hxx>
#endif
#ifndef _SVX_FONTITEM_HXX 
#include <bf_svx/fontitem.hxx>
#endif

#ifndef _ERRHDL_HXX
#include <errhdl.hxx>
#endif

#ifndef _UNOOBJ_HXX
#include <unoobj.hxx>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _DOC_HXX //autogen wg. SwDoc
#include <doc.hxx>
#endif

#ifndef _XMLEXP_HXX
#include <xmlexp.hxx>
#endif
namespace binfilter {

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::text;

class SwXMLFontAutoStylePool_Impl: public XMLFontAutoStylePool
{
	public:

	SwXMLFontAutoStylePool_Impl( SwXMLExport& rExport );

};

SwXMLFontAutoStylePool_Impl::SwXMLFontAutoStylePool_Impl(
	SwXMLExport& rExport ) :
	XMLFontAutoStylePool( rExport )
{
	sal_uInt16 aWhichIds[3] = { RES_CHRATR_FONT, RES_CHRATR_CJK_FONT,
								RES_CHRATR_CTL_FONT };

	Reference < XTextDocument > xTextDoc( rExport.GetModel(), UNO_QUERY );
	Reference < XText > xText = xTextDoc->getText();
	Reference<XUnoTunnel> xTextTunnel( xText, UNO_QUERY);
	ASSERT( xTextTunnel.is(), "missing XUnoTunnel for Cursor" );
	if( !xTextTunnel.is() )
		return;

	SwXText *pText = (SwXText *)xTextTunnel->getSomething(
										SwXText::getUnoTunnelId() );
	ASSERT( pText, "SwXText missing" );
	if( !pText )
		return;

	const SfxItemPool& rPool = pText->GetDoc()->GetAttrPool();
	const SfxPoolItem* pItem;
	for( sal_uInt16 i=0; i<3; i++ )
	{
		sal_uInt16 nWhichId = aWhichIds[i];

		const SvxFontItem& rFont =
			(const SvxFontItem&)rPool.GetDefaultItem( nWhichId );
		Add( rFont.GetFamilyName(), rFont.GetStyleName(),
			 rFont.GetFamily(), rFont.GetPitch(), rFont.GetCharSet() );
		sal_uInt16 nItems = rPool.GetItemCount( nWhichId );
		for( sal_uInt16 j = 0; j < nItems; ++j )
		{
			if( 0 != (pItem = rPool.GetItem( nWhichId, j ) ) )
			{
				const SvxFontItem *pFont = 
							(const SvxFontItem *)pItem;
				Add( pFont->GetFamilyName(), pFont->GetStyleName(),
					 pFont->GetFamily(), pFont->GetPitch(),
					 pFont->GetCharSet() );
			}
		}
	}
}


XMLFontAutoStylePool* SwXMLExport::CreateFontAutoStylePool()
{
	return new SwXMLFontAutoStylePool_Impl( *this );
}
}
