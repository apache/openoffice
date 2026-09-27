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

namespace css = ::com::sun::star;

typedef InheritedHelperInterfaceImpl1< ooo::vba::XFileSearch > ScVbaFileSearchImpl_BASE;

class ScVbaApplication;

class ScVbaFileSearch : public ScVbaFileSearchImpl_BASE
{
private:
    rtl::OUString   m_sFileName;
    rtl::OUString   m_sLookIn;
    sal_Bool        m_bSearchSubFolders;
    sal_Bool        m_bMatchTextExactly;
    ScVbaApplication* m_pApplication;
    css::uno::Sequence< rtl::OUString > m_aSearchedFiles;

    ::rtl::OUString getInitPath();

public:
    ScVbaFileSearch( ScVbaApplication* pApp, const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext );
    virtual ~ScVbaFileSearch();

    // Attributes
    virtual ::rtl::OUString SAL_CALL getFileName() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setFileName( const ::rtl::OUString& _fileName ) throw (css::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getLookIn() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setLookIn( const ::rtl::OUString& _lookIn ) throw (css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL getSearchSubFolders() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setSearchSubFolders( sal_Bool _searchSubFolders ) throw (css::uno::RuntimeException);
    virtual sal_Bool SAL_CALL getMatchTextExactly() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setMatchTextExactly( sal_Bool _matchTextExactly ) throw (css::uno::RuntimeException);
    virtual css::uno::Reference< ::ooo::vba::XFoundFiles > SAL_CALL getFoundFiles() throw (css::uno::RuntimeException);

    virtual sal_Int32 SAL_CALL Execute() throw (css::uno::RuntimeException);
    virtual void SAL_CALL NewSearch() throw (css::uno::RuntimeException);

    // XHelperInterface
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence< rtl::OUString > getServiceNames();
};

#endif