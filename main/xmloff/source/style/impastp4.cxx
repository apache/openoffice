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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_xmloff.hxx"
#include <tools/debug.hxx>
#include <xmloff/xmlaustp.hxx>
#include <xmloff/xmltoken.hxx>
#include <xmloff/nmspmap.hxx>
#include "xmloff/xmlnmspe.hxx"
#include <xmloff/attrlist.hxx>
#include "impastpl.hxx"
#include <xmloff/xmlexppr.hxx>
#include <xmloff/xmlexp.hxx>
#include <xmloff/families.hxx>
#ifndef _XMLOFF_PAGEMASTERSTYLEMAP_HXX
#include <xmloff/PageMasterStyleMap.hxx>
#endif

using namespace ::std;
using ::rtl::OUString;
using ::rtl::OUStringBuffer;

using namespace ::com::sun::star;
using namespace ::xmloff::token;

//#############################################################################
//
// Class SvXMLAutoStylePool_Impl
//

///////////////////////////////////////////////////////////////////////////////
//
// ctor/dtor class SvXMLAutoStylePool_Impl
//

SvXMLAutoStylePoolP_Impl::SvXMLAutoStylePoolP_Impl( SvXMLExport& rExp)
    :	rExport( rExp ),
        maFamilyList( 5, 5 )
{
}

SvXMLAutoStylePoolP_Impl::~SvXMLAutoStylePoolP_Impl()
{
	for (;;) {
        XMLFamilyData_Impl* pData = maFamilyList.Remove( sal_uLong(0) );
        if (pData == NULL) {
            break;
        }
		delete pData;
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// Adds stylefamily-informations to sorted list
//

void SvXMLAutoStylePoolP_Impl::AddFamily(
		sal_Int32 nFamily,
		const OUString& rStrName,
		const UniReference < SvXMLExportPropertyMapper > & rMapper,
	   	const OUString& rStrPrefix,
		sal_Bool bAsFamily )
{
	// store family in a list if not already stored
	sal_uLong nPos;

	sal_uInt16 nExportFlags = GetExport().getExportFlags();
	sal_Bool bStylesOnly = (nExportFlags & EXPORT_STYLES) != 0 && (nExportFlags & EXPORT_CONTENT) == 0;

	OUString aPrefix( rStrPrefix );
	if( bStylesOnly )
	{
		aPrefix = OUString( 'M' );
		aPrefix += rStrPrefix;
	}

	XMLFamilyData_Impl *pFamily = new XMLFamilyData_Impl( nFamily, rStrName, rMapper, aPrefix, bAsFamily );
	if( !maFamilyList.Seek_Entry( pFamily, &nPos ) )
		maFamilyList.Insert( pFamily );
	else
		delete pFamily;
}

///////////////////////////////////////////////////////////////////////////////
//
// Adds a name to list
//

void SvXMLAutoStylePoolP_Impl::RegisterName( sal_Int32 nFamily, const OUString& rName )
{
	SvXMLAutoStylePoolNamesP_Impl *pNames = 0;

	sal_uLong nPos;
	XMLFamilyData_Impl aTmp( nFamily );
	if( maFamilyList.Seek_Entry( &aTmp, &nPos ) )
		pNames = maFamilyList.GetObject( nPos )->mpNameList;

	DBG_ASSERT( pNames,
				"SvXMLAutoStylePool_Impl::RegisterName: unknown family" );
	if( pNames )
	{
		OUString *pName = new OUString( rName );
		if( !pNames->Insert( pName ) )
			delete pName;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Retrieve the list of registered names
//

void SvXMLAutoStylePoolP_Impl::GetRegisteredNames(
    uno::Sequence<sal_Int32>& rFamilies,
    uno::Sequence<OUString>& rNames )
{
    // collect registered names + families
    vector<sal_Int32> aFamilies;
    vector<OUString> aNames;

    // iterate over families
    sal_uInt32 nCount = maFamilyList.Count();
    for( sal_uInt32 i = 0; i < nCount; i++ )
    {
        XMLFamilyData_Impl* pFamily = maFamilyList.GetObject( i );

        // iterate over names
        SvXMLAutoStylePoolNamesP_Impl* pNames = pFamily->mpNameList;
        sal_uInt32 nNames = ( pNames != NULL ) ? pNames->Count() : 0;
        for( sal_uInt32 j = 0; j < nNames; j++ )
        {
            aFamilies.push_back( pFamily->mnFamily );
            aNames.push_back( *pNames->GetObject( j ) );
        }
    }

    // copy the families + names into the sequence types
    DBG_ASSERT( aFamilies.size() == aNames.size(), "families != names" );

    rFamilies.realloc( aFamilies.size() );
    std::copy( aFamilies.begin(), aFamilies.end(), rFamilies.getArray() );

    rNames.realloc( aNames.size() );
    std::copy( aNames.begin(), aNames.end(), rNames.getArray() );
}

///////////////////////////////////////////////////////////////////////////////
//
// Adds a array of XMLPropertyState ( vector< XMLPropertyState > ) to list
// if not added, yet.
//

/*OUString SvXMLAutoStylePoolP_Impl::Add( sal_Int32 nFamily,
							 		    const OUString& rParent,
									    const vector< XMLPropertyState >& rProperties,
									    sal_Bool bCache )*/
sal_Bool SvXMLAutoStylePoolP_Impl::Add(OUString& rName, sal_Int32 nFamily,
				const OUString& rParent,
				const ::std::vector< XMLPropertyState >& rProperties,
				sal_Bool bCache,
                bool bDontSeek )
{
	sal_Bool bRet(sal_False);
	sal_uLong nPos;

	XMLFamilyData_Impl *pFamily = 0;
	XMLFamilyData_Impl aTemporary( nFamily );
	if( maFamilyList.Seek_Entry( &aTemporary, &nPos ) )
	{
		pFamily = maFamilyList.GetObject( nPos );
	}

	DBG_ASSERT( pFamily, "SvXMLAutoStylePool_Impl::Add: unknown family" );
	if( pFamily )
	{
		SvXMLAutoStylePoolParentP_Impl aTmp( rParent );
		SvXMLAutoStylePoolParentP_Impl *pParent = 0;

		SvXMLAutoStylePoolParentsP_Impl *pParents = pFamily->mpParentList;
		if( pParents->Seek_Entry( &aTmp, &nPos ) )
		{
			pParent = pParents->GetObject( nPos );
		}
		else
		{
			pParent = new SvXMLAutoStylePoolParentP_Impl( rParent );
			pParents->Insert( pParent );
		}

		if( pParent->Add( pFamily, rProperties, rName, bDontSeek ) )
		{
			pFamily->mnCount++;
			bRet = sal_True;
		}

		if( bCache )
		{
			if( !pFamily->pCache )
				pFamily->pCache = new SvXMLAutoStylePoolCache_Impl( 256, 256 );
			if( pFamily->pCache->Count() < MAX_CACHE_SIZE )
				pFamily->pCache->Insert( new OUString( rName ),
										 pFamily->pCache->Count() );
		}
	}

	return bRet;
}

sal_Bool SvXMLAutoStylePoolP_Impl::AddNamed(const OUString& rName, sal_Int32 nFamily,
                const OUString& rParent, const ::std::vector< XMLPropertyState >& rProperties )
{
    // get family and parent the same way as in Add()
    sal_Bool bRet(sal_False);
    sal_uLong nPos;

    XMLFamilyData_Impl *pFamily = 0;
    XMLFamilyData_Impl aTemporary( nFamily );
    if( maFamilyList.Seek_Entry( &aTemporary, &nPos ) )
    {
        pFamily = maFamilyList.GetObject( nPos );
    }

    DBG_ASSERT( pFamily, "SvXMLAutoStylePool_Impl::Add: unknown family" );
    if( pFamily )
    {
        SvXMLAutoStylePoolParentP_Impl aTmp( rParent );
        SvXMLAutoStylePoolParentP_Impl *pParent = 0;

        SvXMLAutoStylePoolParentsP_Impl *pParents = pFamily->mpParentList;
        if( pParents->Seek_Entry( &aTmp, &nPos ) )
        {
            pParent = pParents->GetObject( nPos );
        }
        else
        {
            pParent = new SvXMLAutoStylePoolParentP_Impl( rParent );
            pParents->Insert( pParent );
        }

        if( pParent->AddNamed( pFamily, rProperties, rName ) )
        {
            pFamily->mnCount++;
            bRet = sal_True;
        }
    }

    return bRet;
}

OUString SvXMLAutoStylePoolP_Impl::AddToCache( sal_Int32 nFamily,
							 		    const OUString& rParent )
{
	sal_uLong nPos;

	XMLFamilyData_Impl *pFamily = 0;
	XMLFamilyData_Impl aTmp( nFamily );
	if( maFamilyList.Seek_Entry( &aTmp, &nPos ) )
	{
		pFamily = maFamilyList.GetObject( nPos );
	}

	DBG_ASSERT( pFamily, "SvXMLAutoStylePool_Impl::Add: unknown family" );
	if( pFamily )
	{
		if( !pFamily->pCache )
			pFamily->pCache = new SvXMLAutoStylePoolCache_Impl( 256, 256 );
		if( pFamily->pCache->Count() < MAX_CACHE_SIZE )
			pFamily->pCache->Insert( new OUString( rParent ),
									 pFamily->pCache->Count() );
	}

	return rParent;
}
///////////////////////////////////////////////////////////////////////////////
//
// Search for a array of XMLPropertyState ( vector< XMLPropertyState > ) in list
//

OUString SvXMLAutoStylePoolP_Impl::Find( sal_Int32 nFamily,
										 const OUString& rParent,
										 const vector< XMLPropertyState >& rProperties ) const
{
	OUString sName;

	sal_uLong nPos;
	XMLFamilyData_Impl aTemporary( nFamily );
	XMLFamilyData_Impl *pFamily = 0;
	if( maFamilyList.Seek_Entry( &aTemporary, &nPos ) )
	{
		pFamily = maFamilyList.GetObject( nPos );
	}

	DBG_ASSERT( pFamily, "SvXMLAutoStylePool_Impl::Find: unknown family" );

	if( pFamily )
	{
		SvXMLAutoStylePoolParentP_Impl aTmp( rParent );

		const SvXMLAutoStylePoolParentsP_Impl* pParents =
			pFamily->mpParentList;
		if( pParents->Seek_Entry( &aTmp, &nPos ) )
			sName = pParents->GetObject( nPos )->Find( pFamily, rProperties );
	}

	return sName;
}

OUString SvXMLAutoStylePoolP_Impl::FindAndRemoveCached( sal_Int32 nFamily ) const
{
	OUString sName;

	sal_uLong nPos;
	XMLFamilyData_Impl aTmp( nFamily );
	XMLFamilyData_Impl *pFamily = 0;
	if( maFamilyList.Seek_Entry( &aTmp, &nPos ) )
	{
		pFamily = maFamilyList.GetObject( nPos );
	}

	DBG_ASSERT( pFamily, "SvXMLAutoStylePool_Impl::Find: unknown family" );

	if( pFamily )
	{
		DBG_ASSERT( pFamily->pCache, "family doesn't have a cache" );

		// The cache may be empty already. This happens if it was filled
		// completely.
		if( pFamily->pCache && pFamily->pCache->Count() )
		{
			OUString *pName = pFamily->pCache->Remove( 0UL );
			sName = *pName;
			delete pName;
		}
	}

	return sName;
}

///////////////////////////////////////////////////////////////////////////////
//
// export
//

void SvXMLAutoStylePoolP_Impl::exportXML(
	   	sal_Int32 nFamily,
		const uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > &,
		const SvXMLUnitConverter&,
		const SvXMLNamespaceMap&,
		const SvXMLAutoStylePoolP *pAntiImpl) const
{
	sal_uInt32 nCount = 0;

	// Get list of parents for current family (nFamily)
	sal_uLong nPos;
	XMLFamilyData_Impl aTmp( nFamily );
	XMLFamilyData_Impl *pFamily = 0;
	if( maFamilyList.Seek_Entry( &aTmp, &nPos ) )
	{
		pFamily = maFamilyList.GetObject( nPos );
		nCount         = pFamily->mnCount;
	}

	DBG_ASSERT( pFamily,
				"SvXMLAutoStylePool_Impl::exportXML: unknown family" );
	if( pFamily && nCount > 0 )
	{
		/////////////////////////////////////////////////////////////////////////////////////
		// create, initialize and fill helper-structure (SvXMLAutoStylePoolProperties_Impl)
		// which contains a parent-name and a SvXMLAutoStylePoolProperties_Impl
		//
		const SvXMLAutoStylePoolParentsP_Impl *pParents =
			pFamily->mpParentList;

		SvXMLAutoStylePoolPExport_Impl* aExpStyles =
			new SvXMLAutoStylePoolPExport_Impl[nCount];

		sal_uInt32 i;
		for( i=0; i < nCount; i++ )
		{
			aExpStyles[i].mpParent = 0;
			aExpStyles[i].mpProperties = 0;
		}

		sal_uInt32 nParents = pParents->Count();
		for( i=0; i < nParents; i++ )
		{
			const SvXMLAutoStylePoolParentP_Impl* pParent =
				pParents->GetObject( i );
			sal_uInt32 nProperties = pParent->GetPropertiesList().Count();
			for( sal_uInt32 j=0; j < nProperties; j++ )
			{
				const SvXMLAutoStylePoolPropertiesP_Impl *pProperties =
					pParent->GetPropertiesList().GetObject( j );
				nPos = pProperties->GetPos();
				DBG_ASSERT( nPos < nCount,
						"SvXMLAutoStylePool_Impl::exportXML: wrong position" );
				if( nPos < nCount )
				{
					DBG_ASSERT( !aExpStyles[nPos].mpProperties,
						"SvXMLAutoStylePool_Impl::exportXML: double position" );
					aExpStyles[nPos].mpProperties = pProperties;
					aExpStyles[nPos].mpParent = &pParent->GetParent();
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////
		//
		// create string to export for each XML-style. That means for each property-list
		//
		OUString aStrFamilyName = pFamily->maStrFamilyName;

		for( i=0; i<nCount; i++ )
		{
			DBG_ASSERT( aExpStyles[i].mpProperties,
						"SvXMLAutoStylePool_Impl::exportXML: empty position" );

			if( aExpStyles[i].mpProperties )
			{
                GetExport().AddAttribute(
                    XML_NAMESPACE_STYLE, XML_NAME,
                    aExpStyles[i].mpProperties->GetName() );

				if( pFamily->bAsFamily )
				{
                    GetExport().AddAttribute(
                        XML_NAMESPACE_STYLE, XML_FAMILY, aStrFamilyName );
				}

				if( aExpStyles[i].mpParent->getLength() )
				{
                    GetExport().AddAttribute(
                        XML_NAMESPACE_STYLE, XML_PARENT_STYLE_NAME,
                    	GetExport().EncodeStyleName(
							*aExpStyles[i].mpParent ) );
				}

                OUString sName;
				if( pFamily->bAsFamily )
					sName = GetXMLToken(XML_STYLE);
				else
					sName = pFamily->maStrFamilyName;

				pAntiImpl->exportStyleAttributes(
                    GetExport().GetAttrList(),
                    nFamily,
                    aExpStyles[i].mpProperties->GetProperties(),
                    *pFamily->mxMapper.get()
						, GetExport().GetMM100UnitConverter(),
                        GetExport().GetNamespaceMap()
                    );

                SvXMLElementExport aElem( GetExport(),
                                          XML_NAMESPACE_STYLE, sName,
                                          sal_True, sal_True );

				sal_Int32 nStart(-1);
				sal_Int32 nEnd(-1);
				if (nFamily == XML_STYLE_FAMILY_PAGE_MASTER)
				{
					nStart = 0;
					sal_Int32 nIndex = 0;
					UniReference< XMLPropertySetMapper > aPropMapper =
						pFamily->mxMapper->getPropertySetMapper();
					sal_Int16 nContextID;
					while(nIndex < aPropMapper->GetEntryCount() && nEnd == -1)
					{
						nContextID = aPropMapper->GetEntryContextId( nIndex );
						if (nContextID && ((nContextID & CTF_PM_FLAGMASK) != XML_PM_CTF_START))
							nEnd = nIndex;
						nIndex++;
					}
					if (nEnd == -1)
						nEnd = nIndex;
				}

				pFamily->mxMapper->exportXML(
                    GetExport(),
                    aExpStyles[i].mpProperties->GetProperties(),
                    nStart, nEnd, XML_EXPORT_FLAG_IGN_WS );

				pAntiImpl->exportStyleContent(
                    GetExport().GetDocHandler(),
                    nFamily,
                    aExpStyles[i].mpProperties->GetProperties(),
                    *pFamily->mxMapper.get(),
                    GetExport().GetMM100UnitConverter(),
                    GetExport().GetNamespaceMap()
                    );
			}
		}

		delete[] aExpStyles;
	}
}

void SvXMLAutoStylePoolP_Impl::ClearEntries()
{
	for(sal_uInt32 a = 0L; a < maFamilyList.Count(); a++)
		maFamilyList[a]->ClearEntries();
}
