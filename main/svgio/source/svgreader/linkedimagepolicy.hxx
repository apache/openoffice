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

#ifndef INCLUDED_SVGIO_LINKEDIMAGEPOLICY_HXX
#define INCLUDED_SVGIO_LINKEDIMAGEPOLICY_HXX

#include "sal/config.h"

#include "rtl/uri.hxx"
#include "rtl/ustring.hxx"
#include "svtools/linkpolicy.hxx"

namespace svgio { namespace svgreader {

/** Resolves an <image> reference against the SVG's own location and decides
    whether it may be opened.

    An <image> href is document content, read while the SVG is decomposed. The
    reference is only followed when it stays with the SVG: the feature it
    exists for is an image delivered alongside the file (a relative href), so
    a target that resolves to some other place on the machine, to another
    host, or into a different scheme is not part of that feature and is
    refused.

    Nothing here needs a document or a service: the decision is made from the
    two strings, so it holds the same way on every route that reaches the
    decompose step, and for a base URL that is itself remote (a remote base
    contains no local files, so every local target falls outside it).

    @param rBase
    the SVG's own absolute location, as the document reports it.

    @param rHref
    the reference as it was written in the file, relative or absolute.

    @param rAbsoluteUrl
    on success, the resolved URL to open; untouched otherwise.

    @return true when the reference may be opened.
 */
inline bool resolveLinkedImageUrl(
    ::rtl::OUString const & rBase,
    ::rtl::OUString const & rHref,
    ::rtl::OUString & rAbsoluteUrl )
{
    if ( rBase.getLength() == 0 || rHref.getLength() == 0 )
        return false;

    // A malformed base makes the reference unresolvable. convertRelToAbs
    // signals that by throwing, and the throw must not leave the decompose
    // call -- it would cross a UNO interface that does not declare it.
    ::rtl::OUString aAbsUrl;
    try
    {
        aAbsUrl = ::rtl::Uri::convertRelToAbs( rBase, rHref );
    }
    catch ( const ::rtl::MalformedUriException & )
    {
        return false;
    }
    if ( aAbsUrl.getLength() == 0 )
        return false;

    // This is opened as a plain file, so a file: reference on this machine is
    // the only thing that can be read here; the test also refuses the schemes
    // that address process state and a UNC reference to another host.
    if ( !::svt::linkpolicy::isLocalFileUrl( aAbsUrl ) )
        return false;

    // ... and only where the SVG itself sits.
    if ( !::svt::linkpolicy::isContainedUnder( rBase, aAbsUrl ) )
        return false;

    rAbsoluteUrl = aAbsUrl;
    return true;
}

} }

#endif
