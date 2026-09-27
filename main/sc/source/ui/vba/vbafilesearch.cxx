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

#include "vbafilesearch.hxx"
#include "vbaapplication.hxx"
#include "vbafoundfiles.hxx"
#include <comphelper/processfactory.hxx>
#include <tools/urlobj.hxx>
#include <tools/wldcrd.hxx>
#include <com/sun/star/ucb/XSimpleFileAccess3.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <vector>
#include <unotools/viewoptions.hxx>
#include <osl/file.hxx>

using namespace ::ooo::vba;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::ucb;
using namespace ::com::sun::star::lang;
using namespace comphelper;

static Reference< XSimpleFileAccess3 > getFileAccess( void )
{
    static Reference< XSimpleFileAccess3 > xSFI;
    if( !xSFI.is() )
    {
        Reference< XMultiServiceFactory > xSMgr = getProcessServiceFactory();
        if( xSMgr.is() )
        {
            xSFI = Reference< XSimpleFileAccess3 >( xSMgr->createInstance
                ( ::rtl::OUString::createFromAscii( "com.sun.star.ucb.SimpleFileAccess" ) ), UNO_QUERY );
        }
    }
    return xSFI;
}

ScVbaFileSearch::ScVbaFileSearch( ScVbaApplication* pApp, const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext )
    : ScVbaFileSearchImpl_BASE( xParent, xContext ), m_pApplication( pApp )
{
    NewSearch();
}

ScVbaFileSearch::~ScVbaFileSearch()
{
}

::rtl::OUString SAL_CALL ScVbaFileSearch::getFileName() throw (css::uno::RuntimeException)
{
    return m_sFileName;
}

void SAL_CALL ScVbaFileSearch::setFileName( const ::rtl::OUString& _fileName ) throw (css::uno::RuntimeException)
{
    m_sFileName = _fileName;
}

::rtl::OUString SAL_CALL ScVbaFileSearch::getLookIn() throw (css::uno::RuntimeException)
{
    return m_sLookIn;
}

void SAL_CALL ScVbaFileSearch::setLookIn( const ::rtl::OUString& _lookIn ) throw (css::uno::RuntimeException)
{
    m_sLookIn = _lookIn;
}

sal_Bool SAL_CALL ScVbaFileSearch::getSearchSubFolders() throw (css::uno::RuntimeException)
{
    return m_bSearchSubFolders;
}

void SAL_CALL ScVbaFileSearch::setSearchSubFolders( sal_Bool _searchSubFolders ) throw (css::uno::RuntimeException)
{
    m_bSearchSubFolders = _searchSubFolders;
}

sal_Bool SAL_CALL ScVbaFileSearch::getMatchTextExactly() throw (css::uno::RuntimeException)
{
    return m_bMatchTextExactly;
}

void SAL_CALL ScVbaFileSearch::setMatchTextExactly( sal_Bool _matchTextExactly ) throw (css::uno::RuntimeException)
{
    m_bMatchTextExactly = _matchTextExactly;
}

static bool IsWildCard( const ::rtl::OUString& fileName )
{
    static sal_Char cWild1 = '*';
    static sal_Char cWild2 = '?';

    return  ( ( fileName.indexOf( cWild1 ) >= 0 )
            || ( fileName.indexOf( cWild2 ) >= 0 ) );
}

static void SearchWildCard( const WildCard& wildCard, const ::rtl::OUString& aDir, bool bSearchSubFolders, css::uno::Sequence< rtl::OUString >& aSearchedFiles )
{
    Reference< XSimpleFileAccess3 > xSFI = getFileAccess();
    Sequence< rtl::OUString > aDirSeq;
    try
    {
        if ( xSFI.is() )
        {
            aDirSeq = xSFI->getFolderContents( aDir, bSearchSubFolders );
        }
    }
    catch( css::uno::Exception& )
    {
    }
    sal_Int32 nLength = aDirSeq.getLength();
    for ( sal_Int32 i = 0; i < nLength; i++ )
    {
        rtl::OUString aURLStr = aDirSeq[i];
        if ( xSFI->isFolder( aURLStr ) )
        {
            if ( bSearchSubFolders )
            {
                SearchWildCard( wildCard, aURLStr, true, aSearchedFiles );
            }
        }
        else
        {
            INetURLObject aFileURL( aURLStr );
            rtl::OUString aFileName = aFileURL.GetLastName( INetURLObject::DECODE_UNAMBIGUOUS );
            if ( wildCard.Matches( aFileName.toAsciiLowerCase() ) )
            {
                sal_Int32 nLength = aSearchedFiles.getLength();
                aSearchedFiles.realloc( nLength + 1 );
                rtl::OUString sSystemPath;
                ::osl::File::getSystemPathFromFileURL( aURLStr, sSystemPath );
                aSearchedFiles[nLength] = sSystemPath;
            }
        }
    }
}

sal_Int32 SAL_CALL ScVbaFileSearch::Execute() throw (css::uno::RuntimeException)
{
    m_aSearchedFiles.realloc(0);
    Reference< XSimpleFileAccess3 > xSFI = getFileAccess();
    if ( !xSFI.is() || !xSFI->isFolder( m_sLookIn ) )
    {
        return 0;
    }

    if ( m_sFileName == ::rtl::OUString::createFromAscii( "" ) )
    {
        return 1;
    }

    ::rtl::OUString aTempFileName = m_sFileName.toAsciiLowerCase();
    if ( IsWildCard( aTempFileName ) )
    {
        bool bEndWithAsterisk = aTempFileName.endsWithAsciiL("*", 1);
        bool bStartWithAsterisk = (aTempFileName.indexOf(::rtl::OUString::createFromAscii("*")) == 0);
        if ( !bEndWithAsterisk && !bStartWithAsterisk )
        {
            aTempFileName = ::rtl::OUString::createFromAscii("*") + aTempFileName + ::rtl::OUString::createFromAscii("*");
        }
    }
    else
    {
        aTempFileName = ::rtl::OUString::createFromAscii("*") + aTempFileName + ::rtl::OUString::createFromAscii("*");
    }
    WildCard wildCard( aTempFileName );
    SearchWildCard( wildCard, m_sLookIn, m_bSearchSubFolders, m_aSearchedFiles );

    return m_aSearchedFiles.getLength();
}

// 2009-11-5 set ScVbaApplication::getDefaultFilePath() as the InitPath for FileSearch
::rtl::OUString ScVbaFileSearch::getInitPath() throw (css::uno::RuntimeException)
{
    String aPath;

    if (m_pApplication != NULL)
    {
        aPath = m_pApplication->getDefaultFilePath();
    }

    return aPath;
}

void SAL_CALL ScVbaFileSearch::NewSearch() throw (css::uno::RuntimeException)
{
    m_sFileName = ::rtl::OUString::createFromAscii( "" );
    m_sLookIn = getInitPath();
    m_bSearchSubFolders = false;
    m_bMatchTextExactly = false;
    m_aSearchedFiles.realloc(0);
}

Reference< XFoundFiles > SAL_CALL ScVbaFileSearch::getFoundFiles() throw (css::uno::RuntimeException)
{
    css::uno::Reference< ov::XFoundFiles > xFoundFiles = new VbaFoundFiles(
        mxParent, mxContext, (css::container::XIndexAccess *) new VbaFoundFilesEnum(m_aSearchedFiles) );
    return xFoundFiles;
}

rtl::OUString& ScVbaFileSearch::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("VbaFileSearch") );
    return sImplName;
}

css::uno::Sequence< rtl::OUString > ScVbaFileSearch::getServiceNames()
{
    static css::uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.FileSearch") );
    }
    return aServiceNames;
}

namespace filesearch
{
namespace sdecl = comphelper::service_decl;
sdecl::vba_service_class_<ScVbaFileSearch, sdecl::with_args<true> > serviceImpl;
extern sdecl::ServiceDecl const serviceDecl(
    serviceImpl,
    "ScVbaFileSearch",
    "ooo.vba.FileSearch" );
}
