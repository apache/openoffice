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

#include "vbafoundfiles.hxx"

///////////////////////////////VbaFoundFilesEnum//////////////////////////////////////////
VbaFoundFilesEnum::VbaFoundFilesEnum() : m_nIndex(0)
{

}

VbaFoundFilesEnum::VbaFoundFilesEnum( css::uno::Sequence<rtl::OUString>& sFileList ) : m_nIndex(0), m_sFileList(sFileList)
{

}

VbaFoundFilesEnum::~VbaFoundFilesEnum()
{

}

void VbaFoundFilesEnum::SetFileList( css::uno::Sequence<rtl::OUString>& sFileList )
{
    m_nIndex = 0;
    m_sFileList = sFileList;
}

sal_Int32 SAL_CALL VbaFoundFilesEnum::getCount() throw ( css::uno::RuntimeException )
{
    return m_sFileList.getLength();
}

css::uno::Any SAL_CALL VbaFoundFilesEnum::getByIndex( sal_Int32 nIndex )
    throw ( css::lang::IndexOutOfBoundsException, css::lang::WrappedTargetException, css::uno::RuntimeException )
{
    if ( nIndex >= getCount() )
    {
        throw css::lang::IndexOutOfBoundsException();
    }

    return css::uno::makeAny( m_sFileList[nIndex] );
}

css::uno::Type SAL_CALL VbaFoundFilesEnum::getElementType() throw ( css::uno::RuntimeException )
{
    return getCppuType( (css::uno::Reference< css::container::XIndexAccess >*)0 );
}

sal_Bool SAL_CALL VbaFoundFilesEnum::hasElements() throw ( css::uno::RuntimeException )
{
    return ( getCount() != 0 );
}

sal_Bool SAL_CALL VbaFoundFilesEnum::hasMoreElements() throw ( css::uno::RuntimeException )
{
    if ( getCount() > m_nIndex )
    {
        return sal_True;
    }
    return sal_False;
}

css::uno::Any SAL_CALL VbaFoundFilesEnum::nextElement() throw ( css::container::NoSuchElementException, css::lang::WrappedTargetException, css::uno::RuntimeException )
{
    if ( !hasMoreElements() )
    {
        throw css::container::NoSuchElementException();
    }

    return css::uno::makeAny( m_sFileList[m_nIndex++] );
}

///////////////////////////////VbaFoundFiles//////////////////////////////////////////
VbaFoundFiles::VbaFoundFiles( const css::uno::Reference< ov::XHelperInterface >& xParent,
    const css::uno::Reference< css::uno::XComponentContext >& xContext,
    const css::uno::Reference< css::container::XIndexAccess >& xIndexAccess
    ) : VbaFoundFilesImpl_BASE( xParent, xContext, xIndexAccess )
{

}

VbaFoundFiles::~VbaFoundFiles()
{

}

css::uno::Reference< css::container::XEnumeration > VbaFoundFiles::createEnumeration() throw ( css::uno::RuntimeException )
{
    css::uno::Reference< css::container::XEnumeration > xEnumRet( m_xIndexAccess, css::uno::UNO_QUERY );
    return xEnumRet;
}

css::uno::Any VbaFoundFiles::createCollectionObject( const css::uno::Any& aSource )
{
    return aSource;
}

rtl::OUString SAL_CALL VbaFoundFiles::File( sal_Int32 nIndex )
    throw ( css::uno::RuntimeException )
{
    return m_xIndexAccess->getByIndex( nIndex - 1 ).get< rtl::OUString >();
}

css::uno::Type VbaFoundFiles::getElementType() throw ( css::uno::RuntimeException )
{
    return ov::XFoundFiles::static_type(0);
}

rtl::OUString& VbaFoundFiles::getServiceImplName()
{
    static rtl::OUString sImplName( RTL_CONSTASCII_USTRINGPARAM("VbaFoundFiles") );
    return sImplName;
}

css::uno::Sequence< rtl::OUString > VbaFoundFiles::getServiceNames()
{
    static css::uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.FoundFiles") );
    }
    return aServiceNames;
}

namespace foundfiles
{
namespace sdecl = comphelper::service_decl;
sdecl::vba_service_class_<VbaFoundFiles, sdecl::with_args<true> > serviceImpl;
extern sdecl::ServiceDecl const serviceDecl(
    serviceImpl,
    "VbaFoundFiles",
    "ooo.vba.FoundFiles" );
}
