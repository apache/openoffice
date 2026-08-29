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

#ifndef _DESKTOP_CFGFILTER_HXX_
#define _DESKTOP_CFGFILTER_HXX_

#include <stack>

#include <sal/types.h>
#include <rtl/ustring.hxx>

#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implbase3.hxx>

#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Type.hxx>

#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/configuration/backend/XLayer.hpp>
#include <com/sun/star/configuration/backend/XLayerHandler.hpp>
#include <com/sun/star/configuration/backend/TemplateIdentifier.hpp>


#include "migration_impl.hxx"

#define NS_CSS com::sun::star
#define NS_UNO com::sun::star::uno


namespace desktop {

struct element
{
    rtl::OUString name;
    rtl::OUString path;
    sal_Bool use;

};

typedef std::stack< element > element_stack;

// XInitialization:
//  -> Source : XLayer
// XLayer
// XLayerHandler
class CConfigFilter : public cppu::WeakImplHelper3<
    NS_CSS::configuration::backend::XLayer,
    NS_CSS::configuration::backend::XLayerHandler,
    NS_CSS::lang::XInitialization>
{

private:
    NS_UNO::Reference< NS_CSS::configuration::backend::XLayerHandler > m_xLayerHandler;
    NS_UNO::Reference< NS_CSS::configuration::backend::XLayer > m_xSourceLayer;

    rtl::OUString m_aCurrentComponent;

    const strings_v *m_pvInclude;
    const strings_v *m_pvExclude;

    element_stack m_elementStack;

    void pushElement(rtl::OUString aName, sal_Bool bUse = sal_True);
    void popElement();
    sal_Bool checkElement(rtl::OUString aName);
    sal_Bool checkCurrentElement();

public:
    CConfigFilter(const strings_v* include,  const strings_v* exclude);

    // XInitialization
    virtual void SAL_CALL initialize(const NS_UNO::Sequence< NS_UNO::Any >& seqArgs);

    // XLayer
    virtual void SAL_CALL readData(
        const NS_UNO::Reference< NS_CSS::configuration::backend::XLayerHandler >& layerHandler);

    // XLayerHandler
    virtual void SAL_CALL startLayer();

    virtual void SAL_CALL endLayer();

    virtual void SAL_CALL overrideNode(
            const rtl::OUString& aName,
            sal_Int16 aAttributes,
            sal_Bool bClear);

    virtual void SAL_CALL addOrReplaceNode(
            const rtl::OUString& aName,
            sal_Int16 aAttributes);

    virtual void SAL_CALL  addOrReplaceNodeFromTemplate(
            const rtl::OUString& aName,
            const NS_CSS::configuration::backend::TemplateIdentifier& aTemplate,
            sal_Int16 aAttributes );

    virtual void SAL_CALL  endNode();

    virtual void SAL_CALL  dropNode(
            const rtl::OUString& aName );

    virtual void SAL_CALL  overrideProperty(
            const rtl::OUString& aName,
            sal_Int16 aAttributes,
            const NS_UNO::Type& aType,
            sal_Bool bClear );

    virtual void SAL_CALL  setPropertyValue(
            const NS_UNO::Any& aValue );

    virtual void SAL_CALL setPropertyValueForLocale(
            const NS_UNO::Any& aValue,
            const rtl::OUString& aLocale );

    virtual void SAL_CALL  endProperty();

    virtual void SAL_CALL  addProperty(
            const rtl::OUString& aName,
            sal_Int16 aAttributes,
            const NS_UNO::Type& aType );

    virtual void SAL_CALL  addPropertyWithValue(
            const rtl::OUString& aName,
            sal_Int16 aAttributes,
            const NS_UNO::Any& aValue );

};

} // namespace desktop
#undef NS_CSS
#undef NS_UNO

#endif
