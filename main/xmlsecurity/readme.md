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

`nss_akmngr.cxx` implements them as stubs (create = `xmlSecKeysMngrCreate` + wire `getKey`; Load functions return 0). `alwayslink` is not set — VS2008 link.exe predates `/WHOLEARCHIVE`; `nss_component_getFactory` is explicitly referenced from `xsec_xmlsec.cxx` so objects are pulled in on demand.

## DEF file note

`util/xmlsecurity.def` omits `component_canUnload` — `registerservices.cxx` uses the
single-component factory pattern and does not implement the unloadable-component protocol.

## Key defines

- `XMLSEC_CRYPTO_MSCRYPTO` — selects mscrypto backend in xmlsec headers
- `XMLSEC_NO_XSLT` — disables XSLT transform support
- `/Zc:wchar_t-` — required for sal_Unicode / Sequence<sal_Unicode> compatibility
