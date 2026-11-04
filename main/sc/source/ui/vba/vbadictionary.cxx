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


#include "vbadictionary.hxx"
#include "helperdecl.hxx"
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/beans/Property.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/script/BasicErrorException.hpp>
#include <com/sun/star/uno/Type.hxx>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/DeploymentException.hpp>
#include <comphelper/sequence.hxx>
#include <basic/sbuno.hxx>
#include <osl/diagnose.h>
#include <rtl/ustrbuf.hxx>

using namespace ::com::sun::star;

ScVbaDictionary::ScVbaDictionary(const uno::Reference< uno::XComponentContext >& xContext)
    : mnCompareMode(0)
{
    (void)xContext;
}

bool ScVbaDictionary::isObjectKey(const uno::Any& aKey)
{
    return (aKey.getValueTypeClass() == uno::TypeClass_INTERFACE);
}

OUString ScVbaDictionary::formatKey(const uno::Any& aKey)
{
    OUStringBuffer sBuf;
    switch (aKey.getValueTypeClass())
    {
        case uno::TypeClass_STRING:
        {
            OUString sVal;
            aKey >>= sVal;
            sBuf.append(sVal);
            break;
        }
        case uno::TypeClass_DOUBLE:
        {
            double dVal = 0.0;
            aKey >>= dVal;
            sBuf.append(static_cast<sal_Int32>(dVal));
            break;
        }
        case uno::TypeClass_LONG:
        {
            sal_Int32 nVal = 0;
            aKey >>= nVal;
            sBuf.append(nVal);
            break;
        }
        case uno::TypeClass_BOOLEAN:
        {
            sal_Bool bVal = false;
            aKey >>= bVal;
            sBuf.append(OUString::createFromAscii(bVal ? "True" : "False"));
            break;
        }
        default:
        {
            sBuf.append(OUString::createFromAscii("OBJ:"));
            sBuf.append(aKey.getValueType().getTypeName());
            break;
        }
    }
    return sBuf.makeStringAndClear();
}

bool ScVbaDictionary::isEqualKeys(const uno::Any& aKey1, const uno::Any& aKey2, bool bCaseSensitive)
{
    // Handle object keys
    if (isObjectKey(aKey1) && isObjectKey(aKey2))
    {
        uno::Reference< uno::XInterface > xObj1(aKey1, uno::UNO_QUERY);
        uno::Reference< uno::XInterface > xObj2(aKey2, uno::UNO_QUERY);
        return (xObj1.get() == xObj2.get());
    }
    if (isObjectKey(aKey1) || isObjectKey(aKey2))
        return false;

    // Handle string keys
    if (aKey1.getValueTypeClass() == uno::TypeClass_STRING &&
        aKey2.getValueTypeClass() == uno::TypeClass_STRING)
    {
        OUString s1, s2;
        aKey1 >>= s1;
        aKey2 >>= s2;
        if (bCaseSensitive)
            return s1 == s2;
        else
            return s1.equalsIgnoreAsciiCase(s2);
    }

    // Fall back to string comparison for same types
    if (aKey1.getValueType() != aKey2.getValueType())
        return false;

    OUString s1 = formatKey(aKey1);
    OUString s2 = formatKey(aKey2);
    if (bCaseSensitive)
        return s1 == s2;
    else
        return s1.equalsIgnoreAsciiCase(s2);
}

sal_Int32 ScVbaDictionary::findKeyIndex(const uno::Any& aKey)
{
    bool bCaseSensitive = (mnCompareMode == 0);
    for (sal_Int32 i = 0; i < static_cast<sal_Int32>(maEntries.size()); ++i)
    {
        if (isEqualKeys(maEntries[i].aKey, aKey, bCaseSensitive))
            return i;
    }
    return -1;
}

void ScVbaDictionary::Add(const uno::Any& Key, const uno::Any& Item)
{
    sal_Int32 nIndex = findKeyIndex(Key);
    if (nIndex >= 0)
    {
        throw script::BasicErrorException(
            OUString::createFromAscii("Duplicate key"),
            uno::Reference< uno::XInterface >(),
            457,
            OUString(RTL_CONSTASCII_USTRINGPARAM("Scripting.Dictionary"))
        );
    }

    KeyEntry entry;
    entry.aKey = Key;
    entry.aItem = Item;
    entry.bIsObjectKey = isObjectKey(Key);
    entry.bIsItemObject = (Item.getValueTypeClass() == uno::TypeClass_INTERFACE);

    maEntries.push_back(entry);
}

sal_Bool ScVbaDictionary::Exists(const uno::Any& Key)
{
    return findKeyIndex(Key) >= 0;
}

uno::Any ScVbaDictionary::getItem(const uno::Any& Key)
{
    sal_Int32 nIndex = findKeyIndex(Key);
    if (nIndex < 0)
    {
        return uno::Any(); // Empty/Nothing for missing key
    }
    return maEntries[nIndex].aItem;
}

void ScVbaDictionary::setItem(const uno::Any& Key, const uno::Any& Item)
{
    sal_Int32 nIndex = findKeyIndex(Key);
    if (nIndex >= 0)
    {
        maEntries[nIndex].aItem = Item;
        maEntries[nIndex].bIsItemObject = (Item.getValueTypeClass() == uno::TypeClass_INTERFACE);
    }
    else
    {
        Add(Key, Item);
    }
}

uno::Any ScVbaDictionary::Item(const uno::Any& Key)
{
    return getItem(Key);
}

uno::Any SAL_CALL ScVbaDictionary::invoke(
    const OUString& FunctionName,
    const uno::Sequence< uno::Any >& Params,
    uno::Sequence< sal_Int16 >& OutParamIndex,
    uno::Sequence< uno::Any >& OutParam)
{
    uno::Any aResult;

    if (FunctionName.equalsIgnoreAsciiCase(OUString::createFromAscii("Item")))
    {
        if (Params.getLength() == 1)
        {
            aResult = getItem(Params[0]);
        }
        else if (Params.getLength() == 2)
        {
            // dict("key") = value - set the item
            setItem(Params[0], Params[1]);
            OutParamIndex.realloc(1);
            OutParamIndex[0] = 1;
            OutParam.realloc(1);
            OutParam[0] = Params[1];
        }
        else
        {
            throw lang::IllegalArgumentException(
                OUString::createFromAscii("Item requires 1 or 2 parameters"),
                uno::Reference< uno::XInterface >(),
                0);
        }
    }
    else if (FunctionName.equalsIgnoreAsciiCase(OUString::createFromAscii("Add")))
    {
        if (Params.getLength() < 2)
            throw lang::IllegalArgumentException(OUString::createFromAscii("Add requires Key and Item"),
                uno::Reference< uno::XInterface >(), 0);
        Add(Params[0], Params[1]);
    }
    else if (FunctionName.equalsIgnoreAsciiCase(OUString::createFromAscii("Key")))
    {
        if (Params.getLength() == 1)
        {
            aResult = Params[0];
        }
        else if (Params.getLength() == 2)
        {
            sal_Int32 nIndex = findKeyIndex(Params[0]);
            if (nIndex >= 0)
                maEntries[nIndex].aKey = Params[1];
            OutParamIndex.realloc(1);
            OutParamIndex[0] = 1;
            OutParam.realloc(1);
            OutParam[0] = Params[1];
        }
    }
    else if (FunctionName.equalsIgnoreAsciiCase(OUString::createFromAscii("Exists")))
    {
        aResult <<= Exists(Params[0]);
    }
    else if (FunctionName.equalsIgnoreAsciiCase(OUString::createFromAscii("Keys")))
    {
        aResult = Keys();
    }
    else if (FunctionName.equalsIgnoreAsciiCase(OUString::createFromAscii("Items")))
    {
        aResult = Items();
    }
    else if (FunctionName.equalsIgnoreAsciiCase(OUString::createFromAscii("Remove")))
    {
        Remove(Params[0]);
    }
    else if (FunctionName.equalsIgnoreAsciiCase(OUString::createFromAscii("RemoveAll")))
    {
        RemoveAll();
    }
    else
    {
        throw script::BasicErrorException(
            OUString::createFromAscii("Unknown method: ") + FunctionName,
            uno::Reference< uno::XInterface >(),
            438,
            FunctionName
        );
    }

    return aResult;
}

void SAL_CALL ScVbaDictionary::setValue(const OUString& PropertyName, const uno::Any& Value)
{
    if (PropertyName.equalsIgnoreAsciiCase(OUString::createFromAscii("CompareMode")))
    {
        sal_Int32 nMode = 0;
        if ((Value >>= nMode) || nMode == 0)
        {
            setCompareMode(nMode);
        }
    }
    else
    {
        throw beans::UnknownPropertyException(
            OUString::createFromAscii("Unknown property: ") + PropertyName,
            static_cast< script::XInvocation* >(this)
        );
    }
}

uno::Any SAL_CALL ScVbaDictionary::getValue(const OUString& PropertyName)
{
    if (PropertyName.equalsIgnoreAsciiCase(OUString::createFromAscii("Count")))
    {
        return uno::makeAny(getCount());
    }
    else if (PropertyName.equalsIgnoreAsciiCase(OUString::createFromAscii("CompareMode")))
    {
        return uno::makeAny(getCompareMode());
    }
    else if (PropertyName.equalsIgnoreAsciiCase(OUString::createFromAscii("Keys")))
    {
        return Keys();
    }
    else if (PropertyName.equalsIgnoreAsciiCase(OUString::createFromAscii("Items")))
    {
        return Items();
    }
    return uno::Any();
}

sal_Bool SAL_CALL ScVbaDictionary::hasMethod(const OUString& Name)
{
    return (Name.equalsIgnoreAsciiCase(OUString::createFromAscii("Item")) ||
            Name.equalsIgnoreAsciiCase(OUString::createFromAscii("Key")) ||
            Name.equalsIgnoreAsciiCase(OUString::createFromAscii("Add")) ||
            Name.equalsIgnoreAsciiCase(OUString::createFromAscii("Exists")) ||
            Name.equalsIgnoreAsciiCase(OUString::createFromAscii("Keys")) ||
            Name.equalsIgnoreAsciiCase(OUString::createFromAscii("Items")) ||
            Name.equalsIgnoreAsciiCase(OUString::createFromAscii("Remove")) ||
            Name.equalsIgnoreAsciiCase(OUString::createFromAscii("RemoveAll")));
}

sal_Bool SAL_CALL ScVbaDictionary::hasProperty(const OUString& Name)
{
    return (Name.equalsIgnoreAsciiCase(OUString::createFromAscii("Count")) ||
            Name.equalsIgnoreAsciiCase(OUString::createFromAscii("CompareMode")) ||
            Name.equalsIgnoreAsciiCase(OUString::createFromAscii("Keys")) ||
            Name.equalsIgnoreAsciiCase(OUString::createFromAscii("Items")));
}

uno::Reference< beans::XIntrospectionAccess > SAL_CALL ScVbaDictionary::getIntrospection()
{
    return uno::Reference< beans::XIntrospectionAccess >();
}

uno::Any ScVbaDictionary::Keys()
{
    uno::Sequence< uno::Any > aKeys(maEntries.size());
    for (sal_Int32 i = 0; i < static_cast<sal_Int32>(maEntries.size()); ++i)
    {
        aKeys[i] = maEntries[i].aKey;
    }
    uno::Any aResult;
    aResult <<= aKeys;
    return aResult;
}

uno::Any ScVbaDictionary::Items()
{
    uno::Sequence< uno::Any > aItems(maEntries.size());
    for (sal_Int32 i = 0; i < static_cast<sal_Int32>(maEntries.size()); ++i)
    {
        aItems[i] = maEntries[i].aItem;
    }
    uno::Any aResult;
    aResult <<= aItems;
    return aResult;
}

void ScVbaDictionary::Remove(const uno::Any& Key)
{
    sal_Int32 nIndex = findKeyIndex(Key);
    if (nIndex < 0)
    {
        throw script::BasicErrorException(
            OUString::createFromAscii("Key not found"),
            uno::Reference< uno::XInterface >(),
            32811,
            OUString(RTL_CONSTASCII_USTRINGPARAM("Scripting.Dictionary"))
        );
    }
    maEntries.erase(maEntries.begin() + nIndex);
}

void ScVbaDictionary::RemoveAll()
{
    maEntries.clear();
}

sal_Int32 ScVbaDictionary::getCount()
{
    return static_cast<sal_Int32>(maEntries.size());
}

void SAL_CALL ScVbaDictionary::setCount(sal_Int32 /*nCount*/)
{
}

sal_Int32 ScVbaDictionary::getCompareMode()
{
    return mnCompareMode;
}

void ScVbaDictionary::setCompareMode(sal_Int32 CompareMode)
{
    mnCompareMode = CompareMode;
}

OUString SAL_CALL ScVbaDictionary::getDefaultMethodName()
{
    return OUString(RTL_CONSTASCII_USTRINGPARAM("Item"));
}

uno::Reference< beans::XPropertySetInfo > SAL_CALL ScVbaDictionary::getPropertySetInfo()
{
    return uno::Reference< beans::XPropertySetInfo >();
}

void SAL_CALL ScVbaDictionary::setPropertyValue(const OUString& PropertyName, const uno::Any& Value)
{
    setValue(PropertyName, Value);
}

uno::Any SAL_CALL ScVbaDictionary::getPropertyValue(const OUString& PropertyName)
{
    return getValue(PropertyName);
}

void SAL_CALL ScVbaDictionary::addPropertyChangeListener(const OUString& PropertyName,
    const uno::Reference< beans::XPropertyChangeListener >& xListener)
{
    (void)PropertyName;
    (void)xListener;
}

void SAL_CALL ScVbaDictionary::removePropertyChangeListener(const OUString& PropertyName,
    const uno::Reference< beans::XPropertyChangeListener >& xListener)
{
    (void)PropertyName;
    (void)xListener;
}

void SAL_CALL ScVbaDictionary::addVetoableChangeListener(const OUString& PropertyName,
    const uno::Reference< beans::XVetoableChangeListener >& xListener)
{
    (void)PropertyName;
    (void)xListener;
}

void SAL_CALL ScVbaDictionary::removeVetoableChangeListener(const OUString& PropertyName,
    const uno::Reference< beans::XVetoableChangeListener >& xListener)
{
    (void)PropertyName;
    (void)xListener;
}

sal_Int32 ScVbaDictionary::getCreator()
{
    return 0;
}

uno::Reference< ooo::vba::XHelperInterface > ScVbaDictionary::getParent()
{
    return uno::Reference< ooo::vba::XHelperInterface >();
}

uno::Any ScVbaDictionary::Application()
{
    return uno::Any();
}

namespace scripting
{
namespace sdecl = comphelper::service_decl;
sdecl::vba_service_class_<ScVbaDictionary, sdecl::with_args<false> > serviceImpl;
extern sdecl::ServiceDecl const serviceDecl(
    serviceImpl,
    "ScVbaDictionary",
    "ooo.vba.scripting.Dictionary" );
}
