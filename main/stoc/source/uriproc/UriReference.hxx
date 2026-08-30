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



#ifndef INCLUDED_stoc_source_uriproc_UriReference_hxx
#define INCLUDED_stoc_source_uriproc_UriReference_hxx

#include "com/sun/star/uno/RuntimeException.hpp"
#include "osl/mutex.hxx"
#include "rtl/ustring.hxx"
#include "sal/types.h"

namespace rtl { class OUStringBuffer; }

namespace stoc { namespace uriproc {

class UriReference {
public:
    UriReference(
        rtl::OUString const & scheme, bool isHierarchical, bool hasAuthority,
        rtl::OUString const & authority, rtl::OUString const & path,
        bool hasQuery, rtl::OUString const & query);

    ~UriReference();

    rtl::OUString getUriReference();

    sal_Bool isAbsolute();

    rtl::OUString getScheme();

    rtl::OUString getSchemeSpecificPart();

    sal_Bool isHierarchical();

    sal_Bool hasAuthority();

    rtl::OUString getAuthority();

    rtl::OUString getPath();

    sal_Bool hasRelativePath();

    sal_Int32 getPathSegmentCount();

    rtl::OUString getPathSegment(sal_Int32 index);

    sal_Bool hasQuery();

    rtl::OUString getQuery();

    sal_Bool hasFragment();

    rtl::OUString getFragment();

    void setFragment(rtl::OUString const & fragment);

    void clearFragment();

    osl::Mutex m_mutex;
    rtl::OUString m_scheme;
    rtl::OUString m_authority;
    rtl::OUString m_path;
    rtl::OUString m_query;
    rtl::OUString m_fragment;
    bool m_isHierarchical;
    bool m_hasAuthority;
    bool m_hasQuery;
    bool m_hasFragment;

private:
    UriReference(UriReference &); // not implemented
    void operator =(UriReference); // not implemented

    void appendSchemeSpecificPart(rtl::OUStringBuffer & buffer) const;
};

} }

#endif
