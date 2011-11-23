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



#include "sal/config.h"

#include "bf_so3/staticbaseurl.hxx"

#include "com/sun/star/ucb/XCommandEnvironment.hpp"
#include "com/sun/star/uno/Any.hxx"
#include "com/sun/star/uno/Exception.hpp"
#include "com/sun/star/uno/Reference.hxx"
#include "rtl/instance.hxx"
#include "rtl/textenc.h"
#include "rtl/ustring.h"
#include "rtl/ustring.hxx"
#include "bf_svtools/urihelper.hxx"
#include "tools/debug.hxx"
#include "tools/string.hxx"
#include "tools/urlobj.hxx"
#include "ucbhelper/content.hxx"

namespace {

struct BaseURIRef: public rtl::Static< INetURLObject, BaseURIRef > {};

com::sun::star::uno::Any GetCasePreservedURL(INetURLObject const & aObj) {
    DBG_ASSERT( aObj.GetProtocol() != INET_PROT_NOT_VALID, "Invalid URL!" );
    if (aObj.GetProtocol() == INET_PROT_FILE) {
        try {
            com::sun::star::uno::Any aVoidArgument;
            ucbhelper::Content aCnt(
                aObj.GetMainURL(INetURLObject::NO_DECODE),
                com::sun::star::uno::Reference<
                com::sun::star::ucb::XCommandEnvironment >());
            return aCnt.executeCommand(
                rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM("getCasePreservingURL")),
                aVoidArgument);
        } catch (com::sun::star::uno::Exception &) {
            DBG_WARNING("Any other exception");
        }
    }
    return com::sun::star::uno::Any();
}

}

namespace binfilter { namespace StaticBaseUrl {

String RelToAbs(
    String const & rTheRelURIRef, bool bIgnoreFragment,
    INetURLObject::EncodeMechanism eEncodeMechanism,
    INetURLObject::DecodeMechanism eDecodeMechanism, rtl_TextEncoding eCharset,
    INetURLObject::FSysStyle eStyle)
{
    // Backwards compatibility:
    if (rTheRelURIRef.Len() == 0 || rTheRelURIRef.GetChar(0) == '#') {
        return rTheRelURIRef;
    }
    INetURLObject aTheAbsURIRef;
    return (BaseURIRef::get().GetNewAbsURL(
                rTheRelURIRef, &aTheAbsURIRef, eEncodeMechanism, eCharset,
                eStyle, bIgnoreFragment)
            || eEncodeMechanism != INetURLObject::WAS_ENCODED
            || eDecodeMechanism != INetURLObject::DECODE_TO_IURI
            || eCharset != RTL_TEXTENCODING_UTF8)
        ? String(aTheAbsURIRef.GetMainURL(eDecodeMechanism, eCharset))
        : rTheRelURIRef;
}

String AbsToRel(
    String const & rTheAbsURIRef,
    INetURLObject::EncodeMechanism eEncodeMechanism,
    INetURLObject::DecodeMechanism eDecodeMechanism, rtl_TextEncoding eCharset,
    INetURLObject::FSysStyle eStyle)
{
    INetURLObject const & rINetURLObject = BaseURIRef::get();
    com::sun::star::uno::Any aAny;
    if ( rINetURLObject.GetProtocol() != INET_PROT_NOT_VALID )
        aAny = GetCasePreservedURL(rINetURLObject);
    rtl::OUString aBaseURL;
    sal_Bool success = (aAny >>= aBaseURL);
    if (success) {
        INetURLObject aAbsURIRef(rTheAbsURIRef,eEncodeMechanism,eCharset);
        com::sun::star::uno::Any aAny2(GetCasePreservedURL(aAbsURIRef));
        rtl::OUString aAbsURL;
        success = (aAny2 >>= aAbsURL);
        if (success) {
            return INetURLObject::GetRelURL(
                aBaseURL, aAbsURL, INetURLObject::WAS_ENCODED, eDecodeMechanism,
                RTL_TEXTENCODING_UTF8, eStyle);
        } else {
            return INetURLObject::GetRelURL(
                aBaseURL, rTheAbsURIRef, eEncodeMechanism, eDecodeMechanism,
                eCharset, eStyle);
        }
    } else {
        return INetURLObject::GetRelURL(
            rINetURLObject.GetMainURL(INetURLObject::NO_DECODE), rTheAbsURIRef,
            eEncodeMechanism, eDecodeMechanism, eCharset, eStyle);
    }
}

bool SetBaseURL(
    String const & rTheBaseURIRef, INetURLObject::EncodeMechanism eMechanism,
    rtl_TextEncoding eCharset)
{
    return BaseURIRef::get().SetURL(rTheBaseURIRef, eMechanism, eCharset);
}

String GetBaseURL(
    INetURLObject::DecodeMechanism eMechanism, rtl_TextEncoding eCharset)
{
    return BaseURIRef::get().GetMainURL(eMechanism, eCharset);
}

String SmartRelToAbs(
    String const & rTheRelURIRef, bool bIgnoreFragment,
    INetURLObject::EncodeMechanism eEncodeMechanism,
    INetURLObject::DecodeMechanism eDecodeMechanism, rtl_TextEncoding eCharset,
    INetURLObject::FSysStyle eStyle)
{
	return ::binfilter::SmartRel2Abs(
        INetURLObject(GetBaseURL()), rTheRelURIRef,
		::binfilter::GetMaybeFileHdl(), true, bIgnoreFragment, eEncodeMechanism,
        eDecodeMechanism, eCharset, false, eStyle);
}

} }
