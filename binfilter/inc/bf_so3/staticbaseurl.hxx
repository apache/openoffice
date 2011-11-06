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



#ifndef INCLUDED_SO3_INC_BF_XMLOFF_STATICBASEURL_HXX
#define INCLUDED_SO3_INC_BF_XMLOFF_STATICBASEURL_HXX

#ifndef _SAL_CONFIG_H_
#include "sal/config.h"
#endif

#ifndef _RTL_TEXTENC_H
#include "rtl/textenc.h"
#endif
#ifndef _URLOBJ_HXX
#include "tools/urlobj.hxx"
#endif

#ifndef INCLUDED_SO3DLLAPI_H
#include "bf_so3/so3dllapi.h"
#endif

class ByteString;
class String;
 
namespace binfilter {

/** A collection of static functions from tools/inc/urlobj.hxx (rev. 1.26) and
    svtools/inc/urihelper.hxx (rev. 1.2) that have since been removed, but need
    to remain available in so3 and binfilter.
 */
namespace StaticBaseUrl {

String RelToAbs(
    String const & rTheRelURIRef, bool bIgnoreFragment = false,
    INetURLObject::EncodeMechanism eEncodeMechanism
    = INetURLObject::WAS_ENCODED,
    INetURLObject::DecodeMechanism eDecodeMechanism
    = INetURLObject::DECODE_TO_IURI,
    rtl_TextEncoding eCharset = RTL_TEXTENCODING_UTF8,
    INetURLObject::FSysStyle eStyle = INetURLObject::FSYS_DETECT);

String AbsToRel(
    String const & rTheAbsURIRef,
    INetURLObject::EncodeMechanism eEncodeMechanism
    = INetURLObject::WAS_ENCODED,
    INetURLObject::DecodeMechanism eDecodeMechanism
    = INetURLObject::DECODE_TO_IURI,
    rtl_TextEncoding eCharset = RTL_TEXTENCODING_UTF8,
    INetURLObject::FSysStyle eStyle = INetURLObject::FSYS_DETECT);

bool SetBaseURL(
    String const & rTheBaseURIRef,
    INetURLObject::EncodeMechanism eMechanism = INetURLObject::WAS_ENCODED,
    rtl_TextEncoding eCharset = RTL_TEXTENCODING_UTF8);

String GetBaseURL(
    INetURLObject::DecodeMechanism eMechanism = INetURLObject::DECODE_TO_IURI,
    rtl_TextEncoding eCharset = RTL_TEXTENCODING_UTF8);

String SmartRelToAbs(
    String const & rTheRelURIRef, bool bIgnoreFragment = false,
    INetURLObject::EncodeMechanism eEncodeMechanism
    = INetURLObject::WAS_ENCODED,
    INetURLObject::DecodeMechanism eDecodeMechanism
    = INetURLObject::DECODE_TO_IURI,
    rtl_TextEncoding eCharset = RTL_TEXTENCODING_UTF8,
    INetURLObject::FSysStyle eStyle = INetURLObject::FSYS_DETECT);

}

}

#endif
