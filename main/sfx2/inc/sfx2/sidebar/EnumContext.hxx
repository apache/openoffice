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

#ifndef SFX_SIDEBAR_ENUM_CONTEXT_HXX
#define SFX_SIDEBAR_ENUM_CONTEXT_HXX

#include "sfx2/dllapi.h"

#include <rtl/ustring.hxx>


namespace sfx2 { namespace sidebar {

class SFX2_DLLPUBLIC EnumContext
{
public:
    enum Application
    {
        Application_Writer,
        Application_Calc,
        Application_Draw,
        Application_Impress,
        
        Application_Other,

        __LastApplicationEnum = Application_Other
    };
    enum Context
    {
        Context_Annotation,
        Context_Cell,
        Context_EditCell,
        Context_Draw,
        Context_DrawText,
        Context_Graphic,
        Context_OutlineText,
        Context_Pivot,
        Context_Table,
        Context_Text,
        Context_TextObject,
        
        Context_Default,

        __LastContextEnum = Context_Default
    };

    Application meApplication;
    Context meContext;

    EnumContext (void);
    EnumContext (
        const Application eApplication,
        const Context eContext);
    sal_Int32 GetCombinedContext(void) const;
    bool operator == (const EnumContext aOther);
    
    static Application GetApplicationEnum (const ::rtl::OUString& rsApplicationName);
    static const ::rtl::OUString& GetApplicationName (const Application eApplication);

    static Context GetContextEnum (const ::rtl::OUString& rsContextName);
    static const ::rtl::OUString& GetContextName (const Context eContext);

private:
    static void ProvideApplicationContainers (void);
    static void ProvideContextContainers (void);
    static void AddEntry (const ::rtl::OUString& rsName, const Application eApplication);
    static void AddEntry (const ::rtl::OUString& rsName, const Context eContext);
};


#define CombinedEnumContext(a,e) ((static_cast<sal_uInt16>(::sfx2::sidebar::EnumContext::a)<<16)\
        | static_cast<sal_uInt16>(::sfx2::sidebar::EnumContext::e))

} } // end of namespace sfx2::sidebar

#endif
