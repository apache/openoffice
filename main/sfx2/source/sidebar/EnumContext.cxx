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

#include "precompiled_sfx2.hxx"

#include "sidebar/EnumContext.hxx"

namespace sfx2 { namespace sidebar {

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

namespace {

typedef ::std::map<rtl::OUString,EnumContext::Application> ApplicationMap;
typedef ::std::vector<rtl::OUString> ApplicationVector;
static ApplicationMap maApplicationMap;
static ApplicationVector maApplicationVector;

typedef ::std::map<rtl::OUString,EnumContext::Context> ContextMap;
typedef ::std::vector<rtl::OUString> ContextVector;
static ContextMap maContextMap;
static ContextVector maContextVector;

}



EnumContext::EnumContext (void)
    : meApplication(Application_Other),
      meContext(Context_Default)
{
}




EnumContext::EnumContext (
    const Application eApplication,
    const Context eContext)
    : meApplication(eApplication),
      meContext(eContext)
{
}




sal_Int32 EnumContext::GetCombinedContext(void) const
{
    return CombinedEnumContext(meApplication, meContext);
}




bool EnumContext::operator== (const EnumContext aOther)
{
    return meApplication==aOther.meApplication
        && meContext==aOther.meContext;
}




void EnumContext::AddEntry (const ::rtl::OUString& rsName, const Application eApplication)
{
    maApplicationMap[rsName] = eApplication;
    OSL_ASSERT(eApplication<=__LastApplicationEnum);
    if (maApplicationVector.size() <= eApplication)
        maApplicationVector.resize(eApplication+1);
    maApplicationVector[eApplication]=rsName;
}




void EnumContext::ProvideApplicationContainers (void)
{
    if (maApplicationMap.empty())
    {
        maApplicationVector.resize(static_cast<size_t>(EnumContext::__LastApplicationEnum)+1);
        AddEntry(A2S("com.sun.star.text.TextDocument"), EnumContext::Application_Writer);
        AddEntry(A2S("com.sun.star ?calc?"), EnumContext::Application_Calc);
        AddEntry(A2S("com.sun.star ?draw?"), EnumContext::Application_Draw);
        AddEntry(A2S("com.sun.star.presentation.PresentationDocument"), EnumContext::Application_Impress);
        AddEntry(A2S("other"), EnumContext::Application_Other);
    }
}




EnumContext::Application EnumContext::GetApplicationEnum (const ::rtl::OUString& rsApplicationName)
{
    ProvideApplicationContainers();
    
    ApplicationMap::const_iterator iApplication(
        maApplicationMap.find(rsApplicationName));
    if (iApplication != maApplicationMap.end())
        return iApplication->second;
    else
        return EnumContext::Application_Other;
}




const ::rtl::OUString& EnumContext::GetApplicationName (const Application eApplication)
{
    ProvideApplicationContainers();

    const sal_Int32 nIndex (eApplication);
    if (nIndex<0 || nIndex>= __LastApplicationEnum)
        return maApplicationVector[Application_Other];
    else
        return maApplicationVector[nIndex];
}




void EnumContext::AddEntry (const ::rtl::OUString& rsName, const Context eApplication)
{
    maContextMap[rsName] = eApplication;
    OSL_ASSERT(eApplication<=__LastContextEnum);
    if (maContextVector.size() <= eApplication)
        maContextVector.resize(eApplication+1);
    maContextVector[eApplication]=rsName;
}




void EnumContext::ProvideContextContainers (void)
{
    if (maContextMap.empty())
    {
        maContextVector.resize(static_cast<size_t>(__LastContextEnum)+1);
        AddEntry(A2S("text-edit"), Context_Text);
        AddEntry(A2S("text"), Context_Text);
        AddEntry(A2S("default"), Context_Default);
    }
}




EnumContext::Context EnumContext::GetContextEnum (const ::rtl::OUString& rsContextName)
{
    ProvideContextContainers();
    
    ContextMap::const_iterator iContext(
        maContextMap.find(rsContextName));
    if (iContext != maContextMap.end())
        return iContext->second;
    else
        return EnumContext::Context_Default;
}




const ::rtl::OUString& EnumContext::GetContextName (const Context eContext)
{
    ProvideContextContainers();

    const sal_Int32 nIndex (eContext);
    if (nIndex<0 || nIndex>= __LastContextEnum)
        return maContextVector[Context_Default];
    else
        return maContextVector[nIndex];
}




} } // end of namespace sfx2::sidebar
