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



#ifndef INCLUDED_SVTOOLS_SYSLOCALE_HXX
#define INCLUDED_SVTOOLS_SYSLOCALE_HXX

#ifndef INCLUDED_SVTDLLAPI_H
#include "bf_svtools/svtdllapi.h"
#endif

#ifndef _UNOTOOLS_LOCALEDATAWRAPPER_HXX
#include <unotools/localedatawrapper.hxx>
#endif
#ifndef _UNOTOOLS_CHARCLASS_HXX
#include <unotools/charclass.hxx>
#endif
#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

namespace osl { class Mutex; }

class LocaleDataWrapper;

namespace binfilter
{

class SvtSysLocale_Impl;

/**
    SvtSysLocale provides a refcounted single instance of an application wide
    <type>LocaleDataWrapper</type> and <type>CharClass</type> which always
    follow the locale as it is currently configured by the user. You may use
    it anywhere to access the locale data elements like decimal separator and
    simple date formatting and so on. Contructing and destructing a
    SvtSysLocale is not expensive as long as there is at least one instance
    left.
 */
class  SvtSysLocale
{
    friend class SvtSysLocale_Impl;     // access to mutex

    static  SvtSysLocale_Impl*  pImpl;
    static  sal_Int32           nRefCount;

    static  ::osl::Mutex&               GetMutex();

public:
                                        SvtSysLocale();
                                        ~SvtSysLocale();

            const LocaleDataWrapper&    GetLocaleData() const;
            const CharClass&            GetCharClass() const;

    /** It is safe to store the pointers locally and use them AS LONG AS THE
        INSTANCE OF SvtSysLocale LIVES!
        It is a faster access but be sure what you do!
     */
            const LocaleDataWrapper*    GetLocaleDataPtr() const;
            const CharClass*            GetCharClassPtr() const;

};

}

#endif  // INCLUDED_SVTOOLS_SYSLOCALE_HXX
