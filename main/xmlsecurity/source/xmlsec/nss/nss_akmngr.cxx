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

/*
 * Minimal "NSS applied keys manager" stubs, mirroring the pattern of
 * ext_libraries/modules/libxmlsec1/.../src/mscrypto/akmngr.c.
 *
 * OpenOffice's NSS xmlsec backend (source/xmlsec/nss/) was written
 * against an out-of-tree xmlsec patch that added xmlSecNssApplied*
 * helpers.  Those functions were never merged upstream.  The
 * implementations below create a minimal xmlsec keys manager so that
 * the existing call sites compile and link; key adoption (SymKey,
 * PubKey, PriKey) is a no-op because the runtime path through
 * createKeysManager() relies on the certificate database, not raw
 * key objects.
 */

#include "precompiled_xmlsecurity.hxx"

#include "nss_akmngr.hxx"

#include <xmlsec/xmlsec.h>
#include <xmlsec/keysmngr.h>
#include <xmlsec/errors.h>

extern "C" xmlSecKeysMngrPtr
xmlSecNssAppliedKeysMngrCreate(
    PK11SlotInfo** /* slots */, int /* cSlots */, CERTCertDBHandle* /* handler */)
{
    xmlSecKeysMngrPtr mngr = xmlSecKeysMngrCreate();
    if (mngr == NULL) {
        xmlSecError(XMLSEC_ERRORS_HERE, NULL,
                    "xmlSecKeysMngrCreate",
                    XMLSEC_ERRORS_R_XMLSEC_FAILED,
                    XMLSEC_ERRORS_NO_MESSAGE);
        return NULL;
    }
    mngr->getKey = xmlSecKeysMngrGetKey;
    return mngr;
}

extern "C" int
xmlSecNssAppliedKeysMngrSymKeyLoad(
    xmlSecKeysMngrPtr /* mngr */, PK11SymKey* /* symKey */)
{
    return 0;
}

extern "C" int
xmlSecNssAppliedKeysMngrPubKeyLoad(
    xmlSecKeysMngrPtr /* mngr */, SECKEYPublicKey* /* pubKey */)
{
    return 0;
}

extern "C" int
xmlSecNssAppliedKeysMngrPriKeyLoad(
    xmlSecKeysMngrPtr /* mngr */, SECKEYPrivateKey* /* priKey */)
{
    return 0;
}
