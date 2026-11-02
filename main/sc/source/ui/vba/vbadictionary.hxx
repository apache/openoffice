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


#ifndef SC_VBA_DICTIONARY_HXX
#define SC_VBA_DICTIONARY_HXX

#include <cppuhelper/implbase3.hxx>
#include <ooo/vba/scripting/XDictionary.hpp>
#include <ooo/vba/XHelperInterface.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/script/XDefaultMethod.hpp>
#include <com/sun/star/script/XInvocation.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/Exception.hpp>
#include <rtl/ustring.hxx>
#include <sal/types.h>

#include <vector>
#include <utility>
#include <algorithm>

using namespace com::sun;
using namespace com::sun::star;
using namespace ooo::vba;
using namespace ooo::vba::scripting;
using namespace rtl;

typedef ::cppu::WeakImplHelper3<
    scripting::XDictionary,
    script::XInvocation,
    beans::XPropertySet > ScVbaDictionary_BASE;

class ScVbaDictionary : public ScVbaDictionary_BASE
{
private:
    struct KeyEntry
    {
        uno::Any aKey;
        uno::Any aItem;
        bool bIsObjectKey;
        bool bIsItemObject;
    };

    std::vector< KeyEntry > maEntries;
    sal_Int32 mnCompareMode;

    static bool isEqualKeys(const uno::Any& aKey1, const uno::Any& aKey2, bool bCaseSensitive);
    static bool isObjectKey(const uno::Any& aKey);
    sal_Int32 findKeyIndex(const uno::Any& aKey);
    static OUString formatKey(const uno::Any& aKey);
    bool keysEqual(const OUString& aKey1, const OUString& aKey2) const;
    uno::Any getItem(const uno::Any& Key);
    void setItem(const uno::Any& Key, const uno::Any& Item);

public:
    explicit ScVbaDictionary(const uno::Reference< uno::XComponentContext >& xContext);
    virtual ~ScVbaDictionary() {}

    // XDictionary
    virtual void SAL_CALL Add(const uno::Any& Key, const uno::Any& Item);
    virtual sal_Bool SAL_CALL Exists(const uno::Any& Key);
    virtual uno::Any SAL_CALL Items();
    virtual uno::Any SAL_CALL Keys();
    virtual void SAL_CALL Remove(const uno::Any& Key);
    virtual void SAL_CALL RemoveAll();
    virtual uno::Any SAL_CALL Item(const uno::Any& Key);
    virtual sal_Int32 SAL_CALL getCount();
    virtual void SAL_CALL setCount(sal_Int32 /*nCount*/);
    virtual sal_Int32 SAL_CALL getCompareMode();
    virtual void SAL_CALL setCompareMode(sal_Int32 CompareMode);

    // XDefaultMethod
    virtual OUString SAL_CALL getDefaultMethodName();

    // XInvocation
    virtual uno::Reference< beans::XIntrospectionAccess > SAL_CALL getIntrospection();
    virtual uno::Any SAL_CALL invoke(
        const OUString& FunctionName,
        const uno::Sequence< uno::Any >& Params,
        uno::Sequence< sal_Int16 >& OutParamIndex,
        uno::Sequence< uno::Any >& OutParam);
    virtual void SAL_CALL setValue(const OUString& PropertyName, const uno::Any& Value);
    virtual uno::Any SAL_CALL getValue(const OUString& PropertyName);
    virtual sal_Bool SAL_CALL hasMethod(const OUString& Name);
    virtual sal_Bool SAL_CALL hasProperty(const OUString& Name);

    // XPropertySet (for UNO introspection)
    virtual uno::Reference< beans::XPropertySetInfo > SAL_CALL getPropertySetInfo();
    virtual void SAL_CALL setPropertyValue(const OUString& PropertyName, const uno::Any& Value);
    virtual uno::Any SAL_CALL getPropertyValue(const OUString& PropertyName);
    virtual void SAL_CALL addPropertyChangeListener(const OUString& PropertyName,
        const uno::Reference< beans::XPropertyChangeListener >& xListener);
    virtual void SAL_CALL removePropertyChangeListener(const OUString& PropertyName,
        const uno::Reference< beans::XPropertyChangeListener >& xListener);
    virtual void SAL_CALL addVetoableChangeListener(const OUString& PropertyName,
        const uno::Reference< beans::XVetoableChangeListener >& xListener);
    virtual void SAL_CALL removeVetoableChangeListener(const OUString& PropertyName,
        const uno::Reference< beans::XVetoableChangeListener >& xListener);

    // XHelperInterface
    virtual sal_Int32 SAL_CALL getCreator();
    virtual uno::Reference< ooo::vba::XHelperInterface > SAL_CALL getParent();
    virtual uno::Any SAL_CALL Application();
};

#endif // SC_VBA_DICTIONARY_HXX
