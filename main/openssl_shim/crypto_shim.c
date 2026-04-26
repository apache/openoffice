/*
 * BCrypt-backed OpenSSL EVP shim for VS2008 / Windows Vista+.
 *
 * Provides the minimal EVP API that oox/encryption.cxx uses for OOXML
 * document decryption. Implemented in C89 to match the VS2008 toolchain.
 *
 * Algorithms backed by Windows CNG (bcrypt.dll, available since Vista):
 *   AES-128/192/256 ECB/CBC/CFB, SHA-1/256/384/512,
 *   DES-CBC/CFB, 3DES-CBC/CFB, 3DES-112-CBC/CFB, RC2-CBC/CFB.
 * DESX is not in BCrypt; EVP_desx_cbc() returns NULL.
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <bcrypt.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "openssl/evp.h"
#include "openssl/err.h"

/* =========================================================================
 * Error stub
 * ========================================================================= */

void ERR_print_errors_cb(ERR_print_errors_cb_fn cb, void *u)
{
    static const char msg[] = "BCrypt shim: no detailed error information\n";
    cb(msg, sizeof(msg) - 1, u);
}

/* =========================================================================
 * Digest (hash) — backed by BCryptCreateHash
 * ========================================================================= */

struct evp_md_st {
    LPCWSTR alg_id;
    int     digest_size;
};

static const EVP_MD s_sha1   = { BCRYPT_SHA1_ALGORITHM,   20 };
static const EVP_MD s_sha256 = { BCRYPT_SHA256_ALGORITHM, 32 };
static const EVP_MD s_sha384 = { BCRYPT_SHA384_ALGORITHM, 48 };
static const EVP_MD s_sha512 = { BCRYPT_SHA512_ALGORITHM, 64 };
static const EVP_MD s_md5    = { BCRYPT_MD5_ALGORITHM,    16 };
static const EVP_MD s_md4    = { BCRYPT_MD4_ALGORITHM,    16 };

const EVP_MD *EVP_sha1(void)      { return &s_sha1;   }
const EVP_MD *EVP_sha256(void)    { return &s_sha256; }
const EVP_MD *EVP_sha384(void)    { return &s_sha384; }
const EVP_MD *EVP_sha512(void)    { return &s_sha512; }
const EVP_MD *EVP_md5(void)       { return &s_md5;    }
const EVP_MD *EVP_md4(void)       { return &s_md4;    }
const EVP_MD *EVP_md2(void)       { return NULL; }
const EVP_MD *EVP_ripemd160(void) { return NULL; }
const EVP_MD *EVP_whirlpool(void) { return NULL; }

int EVP_MD_size(const EVP_MD *md) { return md ? md->digest_size : -1; }

/* ── digest context ─────────────────────────────────────────────────────── */

struct evp_md_ctx_st {
    const EVP_MD       *md;
    BCRYPT_ALG_HANDLE   hAlg;
    BCRYPT_HASH_HANDLE  hHash;
    PBYTE               pObj;
    DWORD               cbObj;
};

EVP_MD_CTX *EVP_MD_CTX_create(void)
{
    return (EVP_MD_CTX *)calloc(1, sizeof(EVP_MD_CTX));
}

void EVP_MD_CTX_destroy(EVP_MD_CTX *ctx)
{
    if (!ctx) return;
    if (ctx->hHash) BCryptDestroyHash(ctx->hHash);
    if (ctx->hAlg)  BCryptCloseAlgorithmProvider(ctx->hAlg, 0);
    free(ctx->pObj);
    free(ctx);
}

int EVP_DigestInit_ex(EVP_MD_CTX *ctx, const EVP_MD *md, void *e)
{
    NTSTATUS st;
    DWORD cbData;
    (void)e;
    if (ctx->hHash) { BCryptDestroyHash(ctx->hHash); ctx->hHash = NULL; }
    if (ctx->hAlg)  { BCryptCloseAlgorithmProvider(ctx->hAlg, 0); ctx->hAlg = NULL; }
    free(ctx->pObj); ctx->pObj = NULL; ctx->cbObj = 0;

    ctx->md = md;
    st = BCryptOpenAlgorithmProvider(&ctx->hAlg, md->alg_id, NULL, 0);
    if (!BCRYPT_SUCCESS(st)) return 0;

    st = BCryptGetProperty(ctx->hAlg, BCRYPT_OBJECT_LENGTH,
                           (PBYTE)&ctx->cbObj, sizeof(DWORD), &cbData, 0);
    if (!BCRYPT_SUCCESS(st)) return 0;

    ctx->pObj = (PBYTE)malloc(ctx->cbObj);
    if (!ctx->pObj) return 0;

    st = BCryptCreateHash(ctx->hAlg, &ctx->hHash,
                          ctx->pObj, ctx->cbObj, NULL, 0, 0);
    return BCRYPT_SUCCESS(st) ? 1 : 0;
}

int EVP_DigestUpdate(EVP_MD_CTX *ctx, const void *data, size_t len)
{
    NTSTATUS st = BCryptHashData(ctx->hHash, (PUCHAR)data, (ULONG)len, 0);
    return BCRYPT_SUCCESS(st) ? 1 : 0;
}

int EVP_DigestFinal_ex(EVP_MD_CTX *ctx, unsigned char *out, unsigned int *plen)
{
    NTSTATUS st = BCryptFinishHash(ctx->hHash, out, (ULONG)ctx->md->digest_size, 0);
    if (BCRYPT_SUCCESS(st)) {
        if (plen) *plen = (unsigned int)ctx->md->digest_size;
        return 1;
    }
    return 0;
}

/* =========================================================================
 * Cipher — backed by BCryptEncrypt / BCryptDecrypt
 * ========================================================================= */

struct evp_cipher_st {
    LPCWSTR alg_id;
    LPCWSTR chain_mode;
    int     key_len;    /* bytes */
    int     iv_len;     /* bytes; 0 for ECB */
    int     block_size; /* bytes */
};

/* AES */
static const EVP_CIPHER s_aes128ecb   = { BCRYPT_AES_ALGORITHM,      BCRYPT_CHAIN_MODE_ECB, 16,  0, 16 };
static const EVP_CIPHER s_aes128cbc   = { BCRYPT_AES_ALGORITHM,      BCRYPT_CHAIN_MODE_CBC, 16, 16, 16 };
static const EVP_CIPHER s_aes128cfb   = { BCRYPT_AES_ALGORITHM,      BCRYPT_CHAIN_MODE_CFB, 16, 16, 16 };
static const EVP_CIPHER s_aes192cbc   = { BCRYPT_AES_ALGORITHM,      BCRYPT_CHAIN_MODE_CBC, 24, 16, 16 };
static const EVP_CIPHER s_aes192cfb   = { BCRYPT_AES_ALGORITHM,      BCRYPT_CHAIN_MODE_CFB, 24, 16, 16 };
static const EVP_CIPHER s_aes256cbc   = { BCRYPT_AES_ALGORITHM,      BCRYPT_CHAIN_MODE_CBC, 32, 16, 16 };
static const EVP_CIPHER s_aes256cfb   = { BCRYPT_AES_ALGORITHM,      BCRYPT_CHAIN_MODE_CFB, 32, 16, 16 };
/* DES */
static const EVP_CIPHER s_des_cbc     = { BCRYPT_DES_ALGORITHM,      BCRYPT_CHAIN_MODE_CBC,  8,  8,  8 };
static const EVP_CIPHER s_des_cfb     = { BCRYPT_DES_ALGORITHM,      BCRYPT_CHAIN_MODE_CFB,  8,  8,  8 };
/* 3DES (24-byte key = EDE3) */
static const EVP_CIPHER s_des3_cbc    = { BCRYPT_3DES_ALGORITHM,     BCRYPT_CHAIN_MODE_CBC, 24,  8,  8 };
static const EVP_CIPHER s_des3_cfb    = { BCRYPT_3DES_ALGORITHM,     BCRYPT_CHAIN_MODE_CFB, 24,  8,  8 };
/* 3DES-112 (16-byte key = EDE, two unique keys) */
static const EVP_CIPHER s_desede_cbc  = { BCRYPT_3DES_112_ALGORITHM, BCRYPT_CHAIN_MODE_CBC, 16,  8,  8 };
static const EVP_CIPHER s_desede_cfb  = { BCRYPT_3DES_112_ALGORITHM, BCRYPT_CHAIN_MODE_CFB, 16,  8,  8 };
/* RC2 */
static const EVP_CIPHER s_rc2_cbc     = { BCRYPT_RC2_ALGORITHM,      BCRYPT_CHAIN_MODE_CBC, 16,  8,  8 };
static const EVP_CIPHER s_rc2_cfb     = { BCRYPT_RC2_ALGORITHM,      BCRYPT_CHAIN_MODE_CFB, 16,  8,  8 };

const EVP_CIPHER *EVP_aes_128_ecb(void)  { return &s_aes128ecb; }
const EVP_CIPHER *EVP_aes_128_cbc(void)  { return &s_aes128cbc; }
const EVP_CIPHER *EVP_aes_128_cfb(void)  { return &s_aes128cfb; }
const EVP_CIPHER *EVP_aes_192_cbc(void)  { return &s_aes192cbc; }
const EVP_CIPHER *EVP_aes_192_cfb(void)  { return &s_aes192cfb; }
const EVP_CIPHER *EVP_aes_256_cbc(void)  { return &s_aes256cbc; }
const EVP_CIPHER *EVP_aes_256_cfb(void)  { return &s_aes256cfb; }
const EVP_CIPHER *EVP_des_cbc(void)      { return &s_des_cbc;   }
const EVP_CIPHER *EVP_des_cfb(void)      { return &s_des_cfb;   }
const EVP_CIPHER *EVP_desx_cbc(void)     { return NULL; }
const EVP_CIPHER *EVP_des_ede3_cbc(void) { return &s_des3_cbc;   }
const EVP_CIPHER *EVP_des_ede3_cfb(void) { return &s_des3_cfb;   }
const EVP_CIPHER *EVP_des_ede_cbc(void)  { return &s_desede_cbc; }
const EVP_CIPHER *EVP_des_ede_cfb(void)  { return &s_desede_cfb; }
const EVP_CIPHER *EVP_rc2_cbc(void)      { return &s_rc2_cbc; }
const EVP_CIPHER *EVP_rc2_cfb(void)      { return &s_rc2_cfb; }

int EVP_CIPHER_block_size(const EVP_CIPHER *c) { return c ? c->block_size : 0; }

/* ── cipher context ─────────────────────────────────────────────────────── */

#define SHIM_MAX_IV 32

struct evp_cipher_ctx_st {
    const EVP_CIPHER  *cipher;
    BCRYPT_ALG_HANDLE  hAlg;
    BCRYPT_KEY_HANDLE  hKey;
    PBYTE              pKeyObj;
    DWORD              cbKeyObj;
    int                enc;     /* 1 = encrypt, 0 = decrypt */
    int                padding; /* 1 = PKCS7 padding (default), 0 = none */
    BYTE               iv[SHIM_MAX_IV];
    /* Partial-block input buffer */
    PBYTE              buf;
    int                buf_len;
    int                buf_cap;
};

EVP_CIPHER_CTX *EVP_CIPHER_CTX_new(void)
{
    EVP_CIPHER_CTX *ctx = (EVP_CIPHER_CTX *)calloc(1, sizeof(EVP_CIPHER_CTX));
    if (ctx) {
        ctx->buf_cap = 256;
        ctx->buf = (PBYTE)malloc(ctx->buf_cap);
        if (!ctx->buf) { free(ctx); return NULL; }
        ctx->padding = 1;
    }
    return ctx;
}

static void ctx_reset(EVP_CIPHER_CTX *ctx)
{
    if (ctx->hKey)    { BCryptDestroyKey(ctx->hKey);               ctx->hKey    = NULL; }
    if (ctx->hAlg)    { BCryptCloseAlgorithmProvider(ctx->hAlg, 0); ctx->hAlg   = NULL; }
    if (ctx->pKeyObj) { free(ctx->pKeyObj);                         ctx->pKeyObj = NULL; }
    ctx->cbKeyObj = 0;
    ctx->buf_len  = 0;
}

void EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *ctx)
{
    if (!ctx) return;
    ctx_reset(ctx);
    free(ctx->buf);
    free(ctx);
}

int EVP_CIPHER_CTX_set_padding(EVP_CIPHER_CTX *ctx, int pad)
{
    ctx->padding = pad;
    return 1;
}

static int ctx_init(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *c,
                    const unsigned char *key, const unsigned char *iv, int enc)
{
    NTSTATUS st;
    DWORD cbData;
    BYTE blob[sizeof(BCRYPT_KEY_DATA_BLOB_HEADER) + 32];
    BCRYPT_KEY_DATA_BLOB_HEADER *hdr;
    ULONG chain_len;

    if (!c || !key) return 0;

    ctx_reset(ctx);
    ctx->cipher  = c;
    ctx->enc     = enc;
    ctx->padding = 1;

    st = BCryptOpenAlgorithmProvider(&ctx->hAlg, c->alg_id, NULL, 0);
    if (!BCRYPT_SUCCESS(st)) return 0;

    chain_len = (ULONG)((wcslen(c->chain_mode) + 1) * sizeof(WCHAR));
    st = BCryptSetProperty(ctx->hAlg, BCRYPT_CHAINING_MODE,
                           (PBYTE)c->chain_mode, chain_len, 0);
    if (!BCRYPT_SUCCESS(st)) return 0;

    st = BCryptGetProperty(ctx->hAlg, BCRYPT_OBJECT_LENGTH,
                           (PBYTE)&ctx->cbKeyObj, sizeof(DWORD), &cbData, 0);
    if (!BCRYPT_SUCCESS(st)) return 0;

    ctx->pKeyObj = (PBYTE)malloc(ctx->cbKeyObj);
    if (!ctx->pKeyObj) return 0;

    hdr = (BCRYPT_KEY_DATA_BLOB_HEADER *)blob;
    hdr->dwMagic   = BCRYPT_KEY_DATA_BLOB_MAGIC;
    hdr->dwVersion = BCRYPT_KEY_DATA_BLOB_VERSION1;
    hdr->cbKeyData = (ULONG)c->key_len;
    memcpy(blob + sizeof(BCRYPT_KEY_DATA_BLOB_HEADER), key, (size_t)c->key_len);

    st = BCryptImportKey(ctx->hAlg, NULL, BCRYPT_KEY_DATA_BLOB,
                         &ctx->hKey, ctx->pKeyObj, ctx->cbKeyObj,
                         blob,
                         (ULONG)(sizeof(BCRYPT_KEY_DATA_BLOB_HEADER) + c->key_len),
                         0);
    if (!BCRYPT_SUCCESS(st)) return 0;

    if (iv && c->iv_len > 0)
        memcpy(ctx->iv, iv, (size_t)c->iv_len);
    else
        memset(ctx->iv, 0, sizeof(ctx->iv));

    ctx->buf_len = 0;
    return 1;
}

int EVP_CipherInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *c, void *e,
                      const unsigned char *key, const unsigned char *iv, int enc)
{ (void)e; return ctx_init(ctx, c, key, iv, enc); }

int EVP_EncryptInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *c, void *e,
                       const unsigned char *key, const unsigned char *iv)
{ (void)e; return ctx_init(ctx, c, key, iv, 1); }

int EVP_DecryptInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *c, void *e,
                       const unsigned char *key, const unsigned char *iv)
{ (void)e; return ctx_init(ctx, c, key, iv, 0); }

/* Append bytes to the partial-block buffer, growing if needed. */
static int buf_append(EVP_CIPHER_CTX *ctx, const unsigned char *in, int inl)
{
    PBYTE nb;
    while (ctx->buf_len + inl > ctx->buf_cap) {
        ctx->buf_cap *= 2;
        nb = (PBYTE)realloc(ctx->buf, ctx->buf_cap);
        if (!nb) return 0;
        ctx->buf = nb;
    }
    memcpy(ctx->buf + ctx->buf_len, in, (size_t)inl);
    ctx->buf_len += inl;
    return 1;
}

/*
 * Process an exact multiple of block_size bytes through BCrypt.
 * For CBC/CFB, ctx->iv is updated in-place by BCrypt between calls.
 */
static int do_blocks(EVP_CIPHER_CTX *ctx,
                     unsigned char *out, int *outl,
                     const unsigned char *in, int inl)
{
    NTSTATUS st;
    ULONG cbResult;
    PBYTE  piv    = (ctx->cipher->iv_len > 0) ? ctx->iv  : NULL;
    ULONG  iv_len = (ctx->cipher->iv_len > 0) ? (ULONG)ctx->cipher->iv_len : 0;

    *outl = 0;
    if (inl == 0) return 1;

    if (ctx->enc)
        st = BCryptEncrypt(ctx->hKey, (PUCHAR)in, (ULONG)inl, NULL,
                           piv, iv_len, (PUCHAR)out, (ULONG)inl, &cbResult, 0);
    else
        st = BCryptDecrypt(ctx->hKey, (PUCHAR)in, (ULONG)inl, NULL,
                           piv, iv_len, (PUCHAR)out, (ULONG)inl, &cbResult, 0);

    if (!BCRYPT_SUCCESS(st)) return 0;
    *outl = (int)cbResult;
    return 1;
}

static int do_update(EVP_CIPHER_CTX *ctx,
                     unsigned char *out, int *outl,
                     const unsigned char *in, int inl)
{
    int bs       = ctx->cipher->block_size;
    int avail, complete, leftover;

    *outl = 0;
    if (!buf_append(ctx, in, inl)) return 0;

    avail = ctx->buf_len;

    /*
     * When decrypting with padding=1 we hold back one full block so that
     * Final can strip the PKCS7 pad byte.  Formula: (avail-1)/bs*bs gives
     * "all complete blocks except the last one" when avail is block-aligned.
     * For encrypt or no-padding: process all complete blocks immediately.
     */
    if (!ctx->padding || ctx->enc)
        complete = (avail / bs) * bs;
    else
        complete = (avail > 0) ? ((avail - 1) / bs * bs) : 0;

    leftover = avail - complete;
    if (complete == 0) return 1;

    if (!do_blocks(ctx, out, outl, ctx->buf, complete)) return 0;
    memmove(ctx->buf, ctx->buf + complete, (size_t)leftover);
    ctx->buf_len = leftover;
    return 1;
}

int EVP_CipherUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl,
                     const unsigned char *in, int inl)
{ return do_update(ctx, out, outl, in, inl); }

int EVP_DecryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl,
                      const unsigned char *in, int inl)
{ return do_update(ctx, out, outl, in, inl); }

static int do_final(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl)
{
    int bs  = ctx->cipher->block_size;
    int i, pad;

    *outl = 0;

    if (!ctx->padding) {
        /* No padding: any leftover bytes are an error */
        return (ctx->buf_len == 0) ? 1 : 0;
    }

    if (ctx->enc) {
        /* Add PKCS7 padding to fill the last block */
        pad = bs - (ctx->buf_len % bs);
        for (i = 0; i < pad; i++)
            ctx->buf[ctx->buf_len + i] = (unsigned char)pad;
        ctx->buf_len += pad;
    }

    if (ctx->buf_len > 0) {
        if (!do_blocks(ctx, out, outl, ctx->buf, ctx->buf_len)) return 0;
        ctx->buf_len = 0;

        if (!ctx->enc && *outl > 0) {
            /* Strip PKCS7 padding */
            pad = out[*outl - 1];
            if (pad > 0 && pad <= bs)
                *outl -= pad;
        }
    }
    return 1;
}

int EVP_CipherFinal(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl)
{ return do_final(ctx, out, outl); }

int EVP_DecryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl)
{ return do_final(ctx, out, outl); }

/* =========================================================================
 * Base-64 decode (EVP_DecodeBlock)
 * ========================================================================= */

static int b64val(unsigned char c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    if (c == '=') return -2; /* padding */
    return -1;               /* whitespace / invalid */
}

int EVP_DecodeBlock(unsigned char *out, const unsigned char *in, int inl)
{
    int outlen = 0;
    int i      = 0;
    int v0, v1, v2, v3;

    while (i < inl) {
        /* Skip whitespace */
        while (i < inl && (in[i] == ' ' || in[i] == '\n' ||
                            in[i] == '\r' || in[i] == '\t'))
            i++;

        if (i + 3 >= inl) break;

        v0 = b64val(in[i++]);
        v1 = b64val(in[i++]);
        v2 = b64val(in[i++]);
        v3 = b64val(in[i++]);

        if (v0 < 0 || v1 < 0) return -1;
        out[outlen++] = (unsigned char)((v0 << 2) | (v1 >> 4));
        if (v2 < 0) break;
        out[outlen++] = (unsigned char)((v1 << 4) | (v2 >> 2));
        if (v3 < 0) break;
        out[outlen++] = (unsigned char)((v2 << 6) | v3);
    }
    return outlen;
}
