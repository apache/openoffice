# xmlsecurity — Bazel Migration

Three DLLs implementing XML digital signatures and encryption (XMLSec).

## Targets

| Target | Output | Description |
|--------|--------|-------------|
| `xsec_fw` | xsec_fw.dll | SAX-based XML security framework (event keeper, signature/encryption engines) |
| `xsec_xmlsec` | xsec_xmlsec.dll | Windows mscrypto + libxmlsec1 bridge + NSS backend (UNO services: XXMLSignature, XXMLEncryption, XSEInitializer) |
| `xmlsecurity` | xmlsecurity.dll | Main UNO component (CertificateContainer, DocumentDigitalSignatures, dialogs) |

## External dependencies

### libxmlsec1
libxmlsec1 1.2.14 at `ext_libraries/modules/libxmlsec1/`. Two targets:
- `@libxmlsec1//:libxmlsec1` — core C library (no XSLT support)
- `@libxmlsec1//:libxmlsec1_mscrypto` — Windows CryptoAPI backend (incl. akmngr.c)

OOo-specific additions via overlay: `xmlsecconfig.h`, `mscrypto/akmngr.h+c`.
Both statically linked into xsec_xmlsec.dll.

### NSS
NSS 3.39 at `ext_libraries/modules/nss/`. Eight DLLs: nspr4, plds4, plc4, nssutil3, freebl3, softokn3, nss3, smime3. Consumer headers exposed via `@nss//:nss` cc_library.

## NSS backend split (`_nss_objs`)

The NSS backend (`source/xmlsec/nss/`) is a separate `cc_library` sub-target so it can
use `/FInss_akmngr.hxx` without affecting the mscrypto files.

`nss_akmngr.hxx` (force-included into every NSS TU) does two things:
1. Includes `prerror.h` — `PRErrorCode`/`PR_GetError` are not transitively reachable via `cert.h`
2. Declares `xmlSecNssAppliedKeysMngr*` — OpenOffice-internal functions that were never in upstream libxmlsec1

**`_DEBUG` consistency**: two NSS source files (`seinitializer_nssimpl.cxx`, `nssinitializer.cxx`)
do `#undef _DEBUG` early in their bodies to suppress debug assertions from NSS internals.
With `/MDd` (debug CRT from `compilation_mode=dbg`), `_DEBUG` is defined for the whole TU
before the source file body runs.  The `/FI` force-include fires first, and even the
all-C chain `prerror.h → prtypes.h → <stddef.h> → crtdefs.h` is enough to trigger it:
`crtdefs.h` intentionally lacks `#pragma once` and sets `_CRT_MANIFEST_DEBUG` on its first
encounter (while `_DEBUG` is still live).  Later, after `#undef _DEBUG`, when a C++ SAL
header pulls in `use_ansi.h` for the first time, it finds `_CRT_MANIFEST_DEBUG` already set
but `_DEBUG` gone → `_CRT_MANIFEST_INCONSISTENT` → fatal error C1189.

Fix: `_nss_objs` copts include `/D_CRT_NOFORCE_MANIFEST` (skips the manifest block in
`crtdefs.h`) and `/D_STL_NOFORCE_MANIFEST` (skips the check in `use_ansi.h`).  Both are
safe because `xsec_xmlsec.dll` links with `/MANIFEST:NO`.

`nss_akmngr.hxx` uses `#ifdef _WIN64` (rather than `#include <sal/types.h>`) to decide
`XMLSEC_NO_SIZE_T` — this avoids pulling `sal/config.h → <stdlib.h> → crtdefs.h` a
second time, but is not the primary fix for the `_DEBUG` issue.

`nss_akmngr.cxx` implements them as stubs (create = `xmlSecKeysMngrCreate` + wire `getKey`; Load functions return 0). `alwayslink` is not set — VS2008 link.exe predates `/WHOLEARCHIVE`; `nss_component_getFactory` is explicitly referenced from `xsec_xmlsec.cxx` so objects are pulled in on demand.

## DEF file note

`util/xmlsecurity.def` omits `component_canUnload` — `registerservices.cxx` uses the
single-component factory pattern and does not implement the unloadable-component protocol.

## Key defines

- `XMLSEC_CRYPTO_MSCRYPTO` — selects mscrypto backend in xmlsec headers
- `XMLSEC_NO_XSLT` — disables XSLT transform support
- `/Zc:wchar_t-` — required for sal_Unicode / Sequence<sal_Unicode> compatibility
