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

#include "vbaquerytable.hxx"
#include "document.hxx"
#include "docsh.hxx"
#include "scaddress.hxx"
#include "scrange.hxx"
#include <tools/rtti.hxx>
#include <sfx2/linkmgr.hxx>
#include <sfx2/lnkbase.hxx>
#include "arealink.hxx"
#include "vbarange.hxx"

using namespace com::sun::star;
using namespace com::sun::star::uno;

ScVbaQueryTable::ScVbaQueryTable( const css::uno::Reference< ov::XHelperInterface >& xParent,
            const css::uno::Reference< css::uno::XComponentContext >& xContext,
            ScDocument* pDocument,
            ScVbaRange* pParent
            )
:	ScVbaQueryTableImpl_BASE( xParent, xContext ),
    m_pDocument( pDocument ),
    m_pParent( pParent )
{
}

ScVbaQueryTable::~ScVbaQueryTable()
{
}

sal_Bool SAL_CALL
ScVbaQueryTable::Refresh( const css::uno::Any& aBackgroundQuery ) throw ( css::uno::RuntimeException )
{
    if ( !m_pParent || !m_pDocument )
        return sal_False;

    // Get parent Info
    sal_Int32 nRow = m_pParent->getRow();
    sal_Int32 nClm = m_pParent->getColumn();
    sal_Int16 nTab = m_pParent->getWorksheet()->getIndex() - 1; // The vba index begins from 1.
    ScAddress crrRngAddr( nClm, nRow, nTab );

    // Get link info
    sfx2::LinkManager* pLinkMng = m_pDocument->GetLinkManager();
    const SvBaseLinks& rLinks = pLinkMng->GetLinks();
    sal_uInt16 nCount = rLinks.Count();

    for ( sal_uInt16 i = 0; i < nCount; ++i )
    {
        SvBaseLink* pBase = *rLinks[i];
        if ( pBase->ISA( ScAreaLink ) )
        {
            ScAreaLink* pAreaLink = static_cast< ScAreaLink* >( pBase );
            const ScRange& destRange = pAreaLink->GetDestArea();
            if ( destRange.In( crrRngAddr ) )
            {
                pBase->Update();
            }
        }
    }

    return sal_True;
}

void SAL_CALL
ScVbaQueryTable::Delete() throw ( css::uno::RuntimeException )
{
    // TODO: Implement Delete method
}

rtl::OUString& ScVbaQueryTable::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("ScVbaQueryTable") );
    return sImplName;
}

css::uno::Sequence< rtl::OUString > ScVbaQueryTable::getServiceNames()
{
    static css::uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.excel.QueryTable") );
    }
    return aServiceNames;
}

namespace querytable
{
namespace sdecl = comphelper::service_decl;
sdecl::vba_service_class_<ScVbaQueryTable, sdecl::with_args<true> > serviceImpl;
extern sdecl::ServiceDecl const serviceDecl(
    serviceImpl,
    "ScVbaQueryTable",
    "ooo.vba.excel.QueryTable" );
}
