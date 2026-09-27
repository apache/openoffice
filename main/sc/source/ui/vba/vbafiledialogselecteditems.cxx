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

#include "vbafiledialogselecteditems.hxx"

using namespace ::com::sun::star;

VbaFileDialogSelectedItems::VbaFileDialogSelectedItems(
        const css::uno::Reference< ooo::vba::XHelperInterface >& xParent,
        const css::uno::Reference< css::uno::XComponentContext >& xContext,
        const css::uno::Reference< css::container::XIndexAccess >& xIndexAccess )
    : FileDialogSelectedItems_BASE( xParent, xContext, xIndexAccess )
{
}


rtl::OUString& VbaFileDialogSelectedItems::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("VbaFileDialogSelectedItems") );
    return sImplName;
}

css::uno::Sequence< rtl::OUString > VbaFileDialogSelectedItems::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.FileDialogSelectedItems" ) );
    }
    return aServiceNames;
}

css::uno::Any VbaFileDialogSelectedItems::createCollectionObject( const css::uno::Any& aSource )
{
    css::uno::Any aRet;
    aRet = aSource;
    return aRet;
}


css::uno::Type SAL_CALL
VbaFileDialogSelectedItems::getElementType() throw (css::uno::RuntimeException)
{
    return ooo::vba::XFileDialogSelectedItems::static_type(0);
}

css::uno::Reference< css::container::XEnumeration > SAL_CALL
VbaFileDialogSelectedItems::createEnumeration() throw (css::uno::RuntimeException)
{
    css::uno::Reference< css::container::XEnumeration > xEnumRet( m_xIndexAccess, css::uno::UNO_QUERY );
    return xEnumRet;
}


// VbaFileDialogSelectedObj
////////////////////////////////////////////////////////////////////////////

VbaFileDialogSelectedObj::VbaFileDialogSelectedObj()
{
    m_nIndex = 0;
}


sal_Bool
VbaFileDialogSelectedObj::SetSelectedFile( css::uno::Sequence< rtl::OUString > &sFList )
{
    m_sFileList = sFList;
    return sal_True;
}

sal_Int32 SAL_CALL
VbaFileDialogSelectedObj::getCount() throw ( css::uno::RuntimeException )
{
    sal_Int32 nListCnt = m_sFileList.getLength();
    return nListCnt;
}

css::uno::Any SAL_CALL VbaFileDialogSelectedObj::getByIndex( sal_Int32 nIndex )
    throw ( css::lang::IndexOutOfBoundsException,
            css::lang::WrappedTargetException,
            css::uno::RuntimeException )
{
    if ( nIndex >= getCount() )
    {
        throw css::lang::IndexOutOfBoundsException();
    }

    return uno::makeAny( m_sFileList[nIndex] );
}

css::uno::Type SAL_CALL
VbaFileDialogSelectedObj::getElementType()
    throw ( css::uno::RuntimeException )
{
    return getCppuType( (uno::Reference< com::sun::star::container::XIndexAccess >*)0 );
}

sal_Bool SAL_CALL
VbaFileDialogSelectedObj::hasElements()
    throw ( css::uno::RuntimeException )
{
    return ( getCount() != 0 );
}

sal_Bool SAL_CALL
VbaFileDialogSelectedObj::hasMoreElements()
    throw ( css::uno::RuntimeException )
{
    if ( getCount() > m_nIndex )
    {
        return sal_True;
    }
    return sal_False;
}

css::uno::Any SAL_CALL
VbaFileDialogSelectedObj::nextElement()
    throw ( container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException )
{
    if ( !hasMoreElements() )
    {
        throw container::NoSuchElementException();
    }

    return uno::makeAny( m_sFileList[m_nIndex++] );
}
