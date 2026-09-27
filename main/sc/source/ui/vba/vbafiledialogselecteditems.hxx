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

#ifndef SC_VBA_FILEDIALOGSELECTEDITEMS_HXX
#define SC_VBA_FILEDIALOGSELECTEDITEMS_HXX

#include <ooo/vba/XFileDialogSelectedItems.hpp>
#include <vbahelper/vbacollectionimpl.hxx>
#include <cppuhelper/implbase2.hxx>
#include <com/sun/star/container/XIndexAccess.hpp>

typedef CollTestImplHelper< ooo::vba::XFileDialogSelectedItems > FileDialogSelectedItems_BASE;

class VbaFileDialogSelectedItems : public FileDialogSelectedItems_BASE
{
public:
    VbaFileDialogSelectedItems( const css::uno::Reference< ooo::vba::XHelperInterface >& xParent,
        const css::uno::Reference< css::uno::XComponentContext >& xContext,
        const css::uno::Reference< css::container::XIndexAccess >& xIndexAccess );

    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource );
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException);
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException);

protected:
    virtual rtl::OUString& getServiceImplName();
    virtual css::uno::Sequence< rtl::OUString > getServiceNames();
};


class VbaFileDialogSelectedObj : public cppu::WeakImplHelper2< css::container::XIndexAccess, css::container::XEnumeration >
{
public:
    VbaFileDialogSelectedObj();
    sal_Bool SetSelectedFile( css::uno::Sequence< rtl::OUString > &sFList );
    // XIndexAccess
    virtual sal_Int32 SAL_CALL getCount() throw ( css::uno::RuntimeException );

    virtual css::uno::Any SAL_CALL getByIndex( sal_Int32 nIndex )
        throw ( css::lang::IndexOutOfBoundsException,
                css::lang::WrappedTargetException,
                css::uno::RuntimeException );

    // XElementAccess
    virtual css::uno::Type SAL_CALL getElementType()
        throw ( css::uno::RuntimeException );

    virtual sal_Bool SAL_CALL hasElements() throw ( css::uno::RuntimeException );

    // XEnumeration
    virtual sal_Bool SAL_CALL hasMoreElements() throw ( css::uno::RuntimeException );
    virtual css::uno::Any SAL_CALL nextElement()
        throw ( css::container::NoSuchElementException,
                css::lang::WrappedTargetException,
                css::uno::RuntimeException );

protected:
private:
    css::uno::Sequence< rtl::OUString > m_sFileList;
    sal_Int32 m_nIndex;
};

#endif