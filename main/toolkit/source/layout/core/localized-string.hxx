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



#ifndef LAYOUT_CORE_LOCALIZED_STRING_HXX
#define LAYOUT_CORE_LOCALIZED_STRING_HXX

#include <com/sun/star/awt/XFixedText.hpp>
#include <toolkit/awt/vclxwindow.hxx>

namespace layoutimpl
{
namespace css = ::com::sun::star;

// FIXME: misuse XFixedText interface for simple string
class LocalizedString :	public css::awt::XFixedText
                      , public VCLXWindow
{
public:
    LocalizedString();

	// css::uno::XInterface
    css::uno::Any SAL_CALL queryInterface( css::uno::Type const& rType );
    void SAL_CALL acquire() throw() { OWeakObject::acquire(); }
    void SAL_CALL release() throw() { OWeakObject::release(); }

    // css::awt::XFixedText
    void SAL_CALL setText( ::rtl::OUString const& s );
    ::rtl::OUString SAL_CALL getText();
    void SAL_CALL setAlignment( sal_Int16 ) { }
    sal_Int16 SAL_CALL getAlignment() { return 0; }

    // css::awt::XLayoutConstrains
    virtual css::awt::Size SAL_CALL getMinimumSize() { return css::awt::Size( 0, 0 ); }
    css::awt::Size SAL_CALL getPreferredSize() { return getMinimumSize(); }
    css::awt::Size SAL_CALL calcAdjustedSize( css::awt::Size const& size ) { return size; }

    static void ImplGetPropertyIds( std::list< sal_uInt16 > &ids );
    virtual void GetPropertyIds( std::list< sal_uInt16 > &ids )
    { return ImplGetPropertyIds( ids ); }
};

} // namespace layoutimpl

#endif /* LAYOUT_CORE_LOCALIZED_STRING_HXX */
