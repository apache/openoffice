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

#ifndef SC_VBA_FOUNDFILES_HXX
#define SC_VBA_FOUNDFILES_HXX

#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <ooo/vba/XFoundFiles.hpp>
#include <vbahelper/vbacollectionimpl.hxx>

namespace css = ::com::sun::star;

typedef CollTestImplHelper< ooo::vba::XFoundFiles > VbaFoundFilesImpl_BASE;

class VbaFoundFilesEnum : public cppu::WeakImplHelper2< css::container::XIndexAccess, css::container::XEnumeration >
{
private:
    css::uno::Sequence< rtl::OUString > m_sFileList;
    sal_Int32 m_nIndex;

public:
    VbaFoundFilesEnum();
    VbaFoundFilesEnum( css::uno::Sequence< rtl::OUString >& sFileList );
    ~VbaFoundFilesEnum();

    void SetFileList( css::uno::Sequence< rtl::OUString >& sFileList );

    // XIndexAccess
    virtual sal_Int32 SAL_CALL getCount() throw ( css::uno::RuntimeException );
    virtual css::uno::Any SAL_CALL getByIndex( sal_Int32 nIndex ) throw ( css::lang::IndexOutOfBoundsException, css::lang::WrappedTargetException, css::uno::RuntimeException );

    // XElementAccess
    virtual css::uno::Type SAL_CALL getElementType() throw ( css::uno::RuntimeException );
    virtual sal_Bool SAL_CALL hasElements() throw ( css::uno::RuntimeException );

    // XEnumeration
    virtual sal_Bool SAL_CALL hasMoreElements() throw ( css::uno::RuntimeException );
    virtual css::uno::Any SAL_CALL nextElement() throw ( css::container::NoSuchElementException, css::lang::WrappedTargetException, css::uno::RuntimeException );
};

// MS Excel VBA: Collection of file paths returned by FileSearch.Execute.
// Status: implemented - wraps a sequence of OUString file paths
class VbaFoundFiles : public VbaFoundFilesImpl_BASE
{
private:

public:
    // MS Excel VBA: Constructs FoundFiles with parent, context, and file list.
    // Status: implemented
    VbaFoundFiles( const css::uno::Reference< ov::XHelperInterface >& xParent,
        const css::uno::Reference< css::uno::XComponentContext >& xContext,
        const css::uno::Reference< css::container::XIndexAccess >& xIndexAccess );
    // MS Excel VBA: Factory constructor for service instantiation.
    // Status: implemented
    VbaFoundFiles( const css::uno::Sequence< css::uno::Any >& args,
        const css::uno::Reference< css::uno::XComponentContext >& xContext );
    // MS Excel VBA: Destructor.
    // Status: implemented
    virtual ~VbaFoundFiles();

    // MS Excel VBA: Creates a collection object from a source Any.
    // Status: implemented
    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource );

    // XFoundFiles
    // MS Excel VBA: Returns the file path at the specified index (1-based).
    // Status: implemented - validates index and throws IndexOutOfBoundsException if out of range
    virtual rtl::OUString SAL_CALL File( sal_Int32 nIndex ) throw ( css::uno::RuntimeException );

    // XEnumerationAccess
    // MS Excel VBA: Returns the type of objects in this collection.
    // Status: implemented - returns XFoundFiles
    virtual css::uno::Type SAL_CALL getElementType() throw ( css::uno::RuntimeException );
    // MS Excel VBA: Creates an enumeration for iterating the collection.
    // Status: implemented
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw ( css::uno::RuntimeException );

    // XHelperInterface
    // MS Excel VBA: Returns the implementation name for UNO service identification.
    // Status: implemented - returns "VbaFoundFiles"
    virtual rtl::OUString& getServiceImplName();
    // MS Excel VBA: Returns the service names this implementation supports.
    // Status: implemented - returns "ooo.vba.excel.FoundFiles"
    virtual css::uno::Sequence< rtl::OUString > getServiceNames();
};

#endif
