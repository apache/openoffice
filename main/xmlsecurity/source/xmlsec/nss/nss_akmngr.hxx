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
 * Bazel migration shim: declares the NSS "applied keys manager" bridge
 * functions used by securityenvironment_nssimpl.cxx.  These were an
 * OpenOffice-internal extension never distributed with libxmlsec1;
 * implementations live in nss_akmngr.cxx.
 *
 * Also force-includes prerror.h so PRErrorCode / PR_GetError are
 * available everywhere in the NSS backend compilation units.
 */

#ifndef NSS_AKMNGR_HXX
#define NSS_AKMNGR_HXX

/* Ensure PRErrorCode and PR_GetError are available; not transitively
   included by cert.h or secerr.h */
#include "prerror.h"

/* NSS parameter types */
#include "pk11func.h"
#include "keyhi.h"
#include "certdb.h"

/* xmlsec keys manager handle */
#include <sal/types.h>
#if SAL_TYPES_SIZEOFPOINTER != 4
#  define XMLSEC_NO_SIZE_T
#endif
#include <xmlsec/keysmngr.h>

#ifdef __cplusplus
extern "C" {
#endif

xmlSecKeysMngrPtr xmlSecNssAppliedKeysMngrCreate(
    PK11SlotInfo** slots, int cSlots, CERTCertDBHandle* handler);

int xmlSecNssAppliedKeysMngrSymKeyLoad(
    xmlSecKeysMngrPtr mngr, PK11SymKey* symKey);

int xmlSecNssAppliedKeysMngrPubKeyLoad(
    xmlSecKeysMngrPtr mngr, SECKEYPublicKey* pubKey);

int xmlSecNssAppliedKeysMngrPriKeyLoad(
    xmlSecKeysMngrPtr mngr, SECKEYPrivateKey* priKey);

#ifdef __cplusplus
}
#endif

#endif /* NSS_AKMNGR_HXX */
