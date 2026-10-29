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



#ifndef _SVTOOLS_PASSWORDHELPER_HXX
#define _SVTOOLS_PASSWORDHELPER_HXX

#include "svl/svldllapi.h"
#include "sal/types.h"
#include "com/sun/star/uno/Sequence.hxx"
#include "rtl/ustring.hxx"

class String;

class SvPasswordHelper
{
	static void		GetHashPassword(com::sun::star::uno::Sequence <sal_Int8>& rPassHash, const sal_Char* pPass, sal_uInt32 nLen);
    static void     GetHashPasswordLittleEndian(com::sun::star::uno::Sequence<sal_Int8>& rPassHash, const String& sPass);
    static void     GetHashPasswordBigEndian(com::sun::star::uno::Sequence<sal_Int8>& rPassHash, const String& sPass);
    static void     GetHashPasswordUTF8(com::sun::star::uno::Sequence<sal_Int8>& rPassHash, const String& sPass);

public:
    SVL_DLLPUBLIC static void     GetHashPassword(com::sun::star::uno::Sequence<sal_Int8>& rPassHash, const String& sPass);
    /** SHA-256 of the UTF-8 password, the form ODF 1.2 (19.698, 19.851) asks producers to use */
    SVL_DLLPUBLIC static void     GetHashPasswordSHA256(com::sun::star::uno::Sequence<sal_Int8>& rPassHash, const String& sPass);
    /** the ODF 1.2 digest algorithm IRI for a hash made by GetHashPasswordSHA256, empty for any other hash */
    SVL_DLLPUBLIC static ::rtl::OUString GetDigestAlgorithmIRI(const com::sun::star::uno::Sequence<sal_Int8>& rPassHash);
    /**
    Use this method to compare a given string with another given Hash value.
    This is necessary, because in older versions exists different hashs of the same string. They were endian dependent.
    We need this to handle old files. This method will compare against big and little endian. See #101326#
    A 20 byte hash is SHA-1 of the password as UTF-16LE, UTF-16BE or UTF-8; a 32 byte hash is SHA-256 of the UTF-8 password.
    */
    SVL_DLLPUBLIC static bool     CompareHashPassword(const com::sun::star::uno::Sequence<sal_Int8>& rOldPassHash, const String& sNewPass);
};

#endif
