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

#ifndef SC_VBA_FILESEARCH_HXX
#define SC_VBA_FILESEARCH_HXX

#include <ooo/vba/XFileSearch.hpp>
#include <cppuhelper/implbase1.hxx>
#include <vbahelper/vbahelperinterface.hxx>
#include <com/sun/star/uno/Sequence.hxx>

namespace css = ::com::sun::star;

typedef InheritedHelperInterfaceImpl1< ooo::vba::XFileSearch > ScVbaFileSearchImpl_BASE;

class ScVbaApplication;

// MS Excel VBA: Object used to search for files on the filesystem.
// Status: implemented - wraps sc/search results into FoundFiles collection
class ScVbaFileSearch : public ScVbaFileSearchImpl_BASE
{
private:
    rtl::OUString   m_sFileName;
    rtl::OUString   m_sLookIn;
    sal_Bool        m_bSearchSubFolders;
    sal_Bool        m_bMatchTextExactly;
    ScVbaApplication* m_pApplication;
    css::uno::Sequence< rtl::OUString > m_aSearchedFiles;

    // MS Excel VBA: Returns the default search directory path.
    // Status: implemented - falls back to Application.DefaultFilePath
    ::rtl::OUString getInitPath() throw (css::uno::RuntimeException);

public:
    // MS Excel VBA: Constructs FileSearch with application, parent, and context.
    // Status: implemented
    ScVbaFileSearch( ScVbaApplication* pApp, const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext );
    // MS Excel VBA: Factory constructor for service instantiation.
    // Status: implemented
    ScVbaFileSearch( const css::uno::Sequence< css::uno::Any >& /*args*/, const css::uno::Reference< css::uno::XComponentContext >& xContext );
    // MS Excel VBA: Destructor.
    // Status: implemented
    virtual ~ScVbaFileSearch();

    // Attributes
    // MS Excel VBA: Gets/sets the file name criteria for the search.
    // Status: implemented
    virtual ::rtl::OUString SAL_CALL getFileName() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setFileName( const ::rtl::OUString& _fileName ) throw (css::uno::RuntimeException);
    // MS Excel VBA: Gets/sets the folder path to search in.
    // Status: implemented
    virtual ::rtl::OUString SAL_CALL getLookIn() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setLookIn( const ::rtl::OUString& _lookIn ) throw (css::uno::RuntimeException);
    // MS Excel VBA: Gets/sets whether to search subfolders.
    // Status: implemented
    virtual sal_Bool SAL_CALL getSearchSubFolders() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setSearchSubFolders( sal_Bool _searchSubFolders ) throw (css::uno::RuntimeException);
    // MS Excel VBA: Gets/sets whether to match file text exactly.
    // Status: implemented
    virtual sal_Bool SAL_CALL getMatchTextExactly() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setMatchTextExactly( sal_Bool _matchTextExactly ) throw (css::uno::RuntimeException);
    // MS Excel VBA: Returns FoundFiles collection with all matching files.
    // Status: implemented - returns VbaFoundFiles enum
    virtual css::uno::Reference< ::ooo::vba::XFoundFiles > SAL_CALL getFoundFiles() throw (css::uno::RuntimeException);

    // MS Excel VBA: Executes the file search and returns the number of results.
    // Status: implemented - delegates to SearchFiles BASIC function
    virtual sal_Int32 SAL_CALL Execute() throw (css::uno::RuntimeException);
    // MS Excel VBA: Clears all search criteria and results.
    // Status: implemented
    virtual void SAL_CALL NewSearch() throw (css::uno::RuntimeException);

    // XHelperInterface
    // MS Excel VBA: Returns the implementation name for UNO service identification.
    // Status: implemented - returns "ScVbaFileSearch"
    virtual rtl::OUString& getServiceImplName();
    // MS Excel VBA: Returns the service names this implementation supports.
    // Status: implemented - returns "ooo.vba.excel.FileSearch"
    virtual css::uno::Sequence< rtl::OUString > getServiceNames();
};

#endif
