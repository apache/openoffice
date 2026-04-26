#ifndef OPENSSL_EVP_H
#define OPENSSL_EVP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque types — defined in crypto_shim.c */
typedef struct evp_md_st        EVP_MD;
typedef struct evp_md_ctx_st    EVP_MD_CTX;
typedef struct evp_cipher_st    EVP_CIPHER;
typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;
typedef void                    ENGINE; /* always NULL in this shim */

/* ── digest ── */
EVP_MD_CTX *EVP_MD_CTX_create(void);
void        EVP_MD_CTX_destroy(EVP_MD_CTX *ctx);

int EVP_DigestInit_ex(EVP_MD_CTX *ctx, const EVP_MD *type, ENGINE *impl);
int EVP_DigestUpdate(EVP_MD_CTX *ctx, const void *d, size_t cnt);
int EVP_DigestFinal_ex(EVP_MD_CTX *ctx, unsigned char *md, unsigned int *s);

int EVP_MD_size(const EVP_MD *md);

const EVP_MD *EVP_sha1(void);
const EVP_MD *EVP_sha256(void);
const EVP_MD *EVP_sha384(void);
const EVP_MD *EVP_sha512(void);
const EVP_MD *EVP_md5(void);
const EVP_MD *EVP_md4(void);
const EVP_MD *EVP_md2(void);        /* returns NULL — not in BCrypt */
const EVP_MD *EVP_ripemd160(void);  /* returns NULL — not in BCrypt */
const EVP_MD *EVP_whirlpool(void);  /* returns NULL — not in BCrypt */

/* ── cipher ── */
EVP_CIPHER_CTX *EVP_CIPHER_CTX_new(void);
void            EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *ctx);
int             EVP_CIPHER_CTX_set_padding(EVP_CIPHER_CTX *ctx, int pad);

int EVP_CipherInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *type, ENGINE *impl,
                      const unsigned char *key, const unsigned char *iv, int enc);
int EVP_CipherUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl,
                     const unsigned char *in, int inl);
int EVP_CipherFinal(EVP_CIPHER_CTX *ctx, unsigned char *outm, int *outl);

int EVP_EncryptInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *type, ENGINE *impl,
                       const unsigned char *key, const unsigned char *iv);
int EVP_DecryptInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *type, ENGINE *impl,
                       const unsigned char *key, const unsigned char *iv);
int EVP_DecryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl,
                      const unsigned char *in, int inl);
int EVP_DecryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *outm, int *outl);

int EVP_CIPHER_block_size(const EVP_CIPHER *c);

/* base64 decode: returns byte count or -1 */
int EVP_DecodeBlock(unsigned char *t, const unsigned char *f, int n);

/* AES */
const EVP_CIPHER *EVP_aes_128_ecb(void);
const EVP_CIPHER *EVP_aes_128_cbc(void);
const EVP_CIPHER *EVP_aes_128_cfb(void);
const EVP_CIPHER *EVP_aes_192_cbc(void);
const EVP_CIPHER *EVP_aes_192_cfb(void);
const EVP_CIPHER *EVP_aes_256_cbc(void);
const EVP_CIPHER *EVP_aes_256_cfb(void);
/* DES / 3DES */
const EVP_CIPHER *EVP_des_cbc(void);
const EVP_CIPHER *EVP_des_cfb(void);
const EVP_CIPHER *EVP_desx_cbc(void);        /* returns NULL — unsupported */
const EVP_CIPHER *EVP_des_ede3_cbc(void);
const EVP_CIPHER *EVP_des_ede3_cfb(void);
const EVP_CIPHER *EVP_des_ede_cbc(void);
const EVP_CIPHER *EVP_des_ede_cfb(void);
/* RC2 */
const EVP_CIPHER *EVP_rc2_cbc(void);
const EVP_CIPHER *EVP_rc2_cfb(void);

#ifdef __cplusplus
}
#endif

#endif /* OPENSSL_EVP_H */
