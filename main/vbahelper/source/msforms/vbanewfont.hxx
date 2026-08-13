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



#ifndef VBAHELPER_MSFORMS_VBANEWFONT_HXX
#define VBAHELPER_MSFORMS_VBANEWFONT_HXX

#include <ooo/vba/msforms/XNewFont.hpp>
#include <vbahelper/vbahelperinterface.hxx>

// ============================================================================

typedef InheritedHelperInterfaceImpl1< ov::msforms::XNewFont > VbaNewFont_BASE;

class VbaNewFont : public VbaNewFont_BASE
{
public:
    VbaNewFont(
        const css::uno::Reference< ov::XHelperInterface >& rxParent,
        const css::uno::Reference< css::uno::XComponentContext >& rxContext,
        const css::uno::Reference< css::beans::XPropertySet >& rxModelProps );

    // XNewFont attributes
    virtual ::rtl::OUString SAL_CALL getName();
    virtual void SAL_CALL setName( const ::rtl::OUString& rName );
    virtual double SAL_CALL getSize();
    virtual void SAL_CALL setSize( double fSize );
    virtual sal_Int16 SAL_CALL getCharset();
    virtual void SAL_CALL setCharset( sal_Int16 nCharset );
    virtual sal_Int16 SAL_CALL getWeight();
    virtual void SAL_CALL setWeight( sal_Int16 nWeight );
    virtual sal_Bool SAL_CALL getBold();
    virtual void SAL_CALL setBold( sal_Bool bBold );
    virtual sal_Bool SAL_CALL getItalic();
    virtual void SAL_CALL setItalic( sal_Bool bItalic );
    virtual sal_Bool SAL_CALL getUnderline();
    virtual void SAL_CALL setUnderline( sal_Bool bUnderline );
    virtual sal_Bool SAL_CALL getStrikethrough();
    virtual void SAL_CALL setStrikethrough( sal_Bool bStrikethrough );

    // XHelperInterface
    VBAHELPER_DECL_XHELPERINTERFACE

private:
    css::uno::Reference< css::beans::XPropertySet > mxProps;
};

// ============================================================================

#endif
