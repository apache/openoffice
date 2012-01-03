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

#include <hintids.hxx>

#ifndef _RTL_USTRING_HXX_
#include <rtl/ustring.hxx>
#endif
#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _SHL_HXX //autogen wg. SHL_WRITER
#include <tools/shl.hxx>
#endif

#ifndef _XMLOFF_XMLUCONV_HXX
#include <bf_xmloff/xmluconv.hxx>
#endif
#ifndef _XMLITMPR_HXX
#include "xmlexpit.hxx"
#endif
#ifndef _XMLOFF_NMSPMAP_HXX
#include <bf_xmloff/nmspmap.hxx>
#endif
#ifndef _XMLOFF_XMLTABE_HXX
#include <bf_xmloff/xmltabe.hxx>
#endif
#ifndef _XMLBRSHE_HXX
#include "xmlbrshe.hxx"
#endif

#ifndef _SVX_TSPTITEM_HXX
#include <bf_svx/tstpitem.hxx>
#endif
#ifndef _SVX_BRSHITEM_HXX
#include <bf_svx/brshitem.hxx>
#endif
#ifndef _SVX_UNOMID_HXX
#include <bf_svx/unomid.hxx>
#endif

#include <tools/fldunit.hxx>

#ifndef _SWMODULE_HXX //autogen wg. SW_MOD
#include <swmodule.hxx>
#endif

#ifndef _HORIORNT_HXX
#include <horiornt.hxx>
#endif

#ifndef _DOC_HXX //autogen wg. SwDoc
#include <doc.hxx>
#endif
#ifndef _FMTORNT_HXX
#include "fmtornt.hxx"
#endif
#ifndef _UNOMID_H
#include <unomid.h>
#endif
#ifndef _FRMFMT_HXX
#include "frmfmt.hxx"
#endif
#ifndef _FMTFSIZE_HXX
#include "fmtfsize.hxx"
#endif
#ifndef _SWRECT_HXX
#include "swrect.hxx"
#endif

#ifndef _XMLEXP_HXX
#include "xmlexp.hxx"
#endif
namespace binfilter {

using namespace ::rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::binfilter::xmloff::token;

extern SvXMLItemMapEntry aXMLTableItemMap[];
extern SvXMLItemMapEntry aXMLTableRowItemMap[];
extern SvXMLItemMapEntry aXMLTableCellItemMap[];


class SwXMLTableItemMapper_Impl: public SvXMLExportItemMapper
{
	SwXMLBrushItemExport aBrushItemExport;

protected:

	sal_uInt32 nAbsWidth;

	void AddAttribute( sal_uInt16 nPrefix, enum XMLTokenEnum eLName,
					   const OUString& rValue,
					   const SvXMLNamespaceMap& rNamespaceMap,
					   SvXMLAttributeList& rAttrList ) const;

public:

	SwXMLTableItemMapper_Impl(
			SvXMLItemMapEntriesRef rMapEntries,
			SwXMLExport& rExp );

	virtual ~SwXMLTableItemMapper_Impl();

	virtual void handleSpecialItem( SvXMLAttributeList& rAttrList,
									const SvXMLItemMapEntry& rEntry,
									const SfxPoolItem& rItem,
									const SvXMLUnitConverter& rUnitConverter,
									const SvXMLNamespaceMap& rNamespaceMap,
									const SfxItemSet *pSet = NULL ) const;

	virtual void handleElementItem(
            SvXMLExport& rExport,
			const SvXMLItemMapEntry& rEntry,
			const SfxPoolItem& rItem,
			const SvXMLUnitConverter& rUnitConverter,
			const SfxItemSet& rSet,
			sal_uInt16 nFlags ) const;

	inline void SetAbsWidth( sal_uInt32 nAbs );
};

SwXMLTableItemMapper_Impl::SwXMLTableItemMapper_Impl(
		SvXMLItemMapEntriesRef rMapEntries,
		SwXMLExport& rExp ) :
	SvXMLExportItemMapper( rMapEntries ),
	aBrushItemExport( rExp ),
	nAbsWidth( USHRT_MAX )
{
}

SwXMLTableItemMapper_Impl::~SwXMLTableItemMapper_Impl()
{
}

void SwXMLTableItemMapper_Impl::AddAttribute( sal_uInt16 nPrefix,
		enum XMLTokenEnum eLName,
		const OUString& rValue,
		const SvXMLNamespaceMap& rNamespaceMap,
		SvXMLAttributeList& rAttrList ) const
{
	OUString sName( rNamespaceMap.GetQNameByKey( nPrefix,
                                                 GetXMLToken(eLName) ) );
	rAttrList.AddAttribute( sName, rValue );
}

void SwXMLTableItemMapper_Impl::handleSpecialItem(
		SvXMLAttributeList& rAttrList,
		const SvXMLItemMapEntry& rEntry,
		const SfxPoolItem& rItem,
		const SvXMLUnitConverter& rUnitConverter,
		const SvXMLNamespaceMap& rNamespaceMap,
		const SfxItemSet *pSet ) const
{
	switch( rEntry.nWhichId )
	{
	case RES_LR_SPACE:
		{
			const SfxPoolItem *pItem;
			if( pSet &&
				SFX_ITEM_SET == pSet->GetItemState( RES_HORI_ORIENT, sal_True,
													&pItem ) )
			{
				SwHoriOrient eHoriOrient =
					((const SwFmtHoriOrient *)pItem)->GetHoriOrient();
				sal_Bool bExport = sal_False;
				sal_uInt32 nMemberId = rEntry.nMemberId & MID_FLAG_MASK;
				switch( nMemberId )
				{
				case MID_L_MARGIN:
					bExport = HORI_NONE == eHoriOrient ||
							  HORI_LEFT_AND_WIDTH == eHoriOrient;
					break;
				case MID_R_MARGIN:
					bExport = HORI_NONE == eHoriOrient;
					break;
				}
				OUString sValue;
				if( bExport && SvXMLExportItemMapper::QueryXMLValue(
                    rItem, sValue, nMemberId, rUnitConverter ) )
				{
					AddAttribute( rEntry.nNameSpace, rEntry.eLocalName, sValue,
								  rNamespaceMap, rAttrList );
				}
			}
		}
		break;

	case RES_FRM_SIZE:
		{
			sal_uInt32 nMemberId = rEntry.nMemberId & MID_FLAG_MASK;
			switch( nMemberId )
			{
			case MID_FRMSIZE_WIDTH:
				if( nAbsWidth )
				{
					OUStringBuffer sBuffer;
					rUnitConverter.convertMeasure( sBuffer, nAbsWidth );
					AddAttribute( rEntry.nNameSpace, rEntry.eLocalName,
								  sBuffer.makeStringAndClear(),
								  rNamespaceMap, rAttrList );
				}
				break;
			case MID_FRMSIZE_REL_WIDTH:
				{
					OUString sValue;
					if( SvXMLExportItemMapper::QueryXMLValue(
                        rItem, sValue, nMemberId, rUnitConverter ) )
					{
						AddAttribute( rEntry.nNameSpace, rEntry.eLocalName,
									  sValue, rNamespaceMap, rAttrList );
					}
				}
				break;
			}
		}
		break;
	}
}

/** this method is called for every item that has the
	MID_FLAG_ELEMENT_EXPORT flag set */
void SwXMLTableItemMapper_Impl::handleElementItem(
        SvXMLExport& rExport,
		const SvXMLItemMapEntry& rEntry,
		const SfxPoolItem& rItem,
		const SvXMLUnitConverter& rUnitConverter,
		const SfxItemSet&,
		sal_uInt16 ) const
{
	switch( rEntry.nWhichId )
	{
	case RES_BACKGROUND:
		{
			((SwXMLTableItemMapper_Impl *)this)->aBrushItemExport.exportXML(
												(const SvxBrushItem&)rItem );
		}
		break;
	}
}

inline void SwXMLTableItemMapper_Impl::SetAbsWidth( sal_uInt32 nAbs )
{
	nAbsWidth = nAbs;
}

// ----------------------------------------------------------------------------

void SwXMLExport::_InitItemExport()
{
	// #110680#
	pTwipUnitConv = new SvXMLUnitConverter( MAP_TWIP,
		GetMM100UnitConverter().getXMLMeasureUnit(), getServiceFactory() );

	xTableItemMap = new SvXMLItemMapEntries( aXMLTableItemMap );
	xTableRowItemMap = new SvXMLItemMapEntries( aXMLTableRowItemMap );
	xTableCellItemMap = new SvXMLItemMapEntries( aXMLTableCellItemMap );

	pTableItemMapper = new SwXMLTableItemMapper_Impl( xTableItemMap, *this );
}

void SwXMLExport::_FinitItemExport()
{
	delete pTableItemMapper;
	delete pTwipUnitConv;
}

void SwXMLExport::ExportTableFmt( const SwFrmFmt& rFmt, sal_uInt32 nAbsWidth )
{
	((SwXMLTableItemMapper_Impl *)pTableItemMapper)
		->SetAbsWidth( nAbsWidth );
	ExportFmt( rFmt, XML_TABLE );
}
}
