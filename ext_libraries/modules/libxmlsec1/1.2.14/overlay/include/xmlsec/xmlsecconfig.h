/* Pre-generated xmlsecconfig.h for Windows/MSVC/mscrypto/no-xslt (OOo Bazel build) */
#ifndef __XMLSEC_CONFIG_H__
#define __XMLSEC_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Crypto engine name */
#ifndef XMLSEC_CRYPTO
#define XMLSEC_CRYPTO "mscrypto"
#endif /* XMLSEC_CRYPTO */

/* MS Crypto backend is the default */
#ifndef XMLSEC_CRYPTO_MSCRYPTO
#define XMLSEC_CRYPTO_MSCRYPTO 1
#endif /* XMLSEC_CRYPTO_MSCRYPTO */

/* XSLT support disabled */
#define XMLSEC_NO_XSLT 1

/* Dynamic loading of crypto engines disabled */
#define XMLSEC_NO_CRYPTO_DYNAMIC_LOADING 1

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __XMLSEC_CONFIG_H__ */
