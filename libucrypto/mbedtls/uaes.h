// Copyright 2017 Altronix Corp.
// This file is part of the tiny-ether library
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/**
 * @author Thomas Chiantia <thomas@altronix>
 * @date 2017
 */

#ifndef AES_H_
#define AES_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/aes.h"

// clang-format off
typedef struct{uint8_t b[16];} uaes_ctr_128_key;
typedef struct{uint8_t b[32];} uaes_ctr_256_key;
typedef struct{uint8_t b[16];} uaes_iv;
// clang-format on

// typedef mbedtls_aes_context uaes_ctx;
typedef struct
{
    mbedtls_aes_context ctx;
    uint8_t iv[16];
} uaes_ctx;

int uaes_init(uaes_ctx* ctx, int keysz, uint8_t* key);
void uaes_deinit(uaes_ctx** ctx);
void uaes_crypt_reset(uaes_ctx* ctx);
int uaes_crypt_ctr(
    int keysz,
    uint8_t* key,
    uint8_t* iv,
    const uint8_t* in,
    size_t inlen,
    uint8_t* out);
int uaes_crypt_ctr_update(
    uaes_ctx* ctx,
    const uint8_t* in,
    size_t inlen,
    uint8_t* out);
int uaes_crypt_ctr_op(
    uaes_ctx* ctx,
    uint8_t* iv,
    const uint8_t* in,
    size_t inlen,
    uint8_t* out);

/**
 * @brief Inline aliaes api wrapper
 */

static inline int
uaes_init_bin(uaes_ctx* ctx, uint8_t* key, size_t keylen)
{
    return uaes_init(ctx, keylen * 8, key);
}

static inline int
uaes_init_128(uaes_ctx* ctx, uint8_t* key)
{
    return uaes_init(ctx, 128, key);
}

static inline int
uaes_init_256(uaes_ctx* ctx, uint8_t* key)
{
    return uaes_init(ctx, 256, key);
}

static inline int
uaes_crypt_ctr_128(
    uaes_ctr_128_key* key,
    uaes_iv* iv,
    const uint8_t* in,
    size_t inlen,
    uint8_t* out)
{
    return uaes_crypt_ctr(128, key->b, iv->b, in, inlen, out);
}

static inline int
uaes_crypt_ctr_256(
    uaes_ctr_128_key* key,
    uaes_iv* iv,
    const uint8_t* in,
    size_t inlen,
    uint8_t* out)
{
    return uaes_crypt_ctr(256, key->b, iv->b, in, inlen, out);
}

int uaes_crypt_ecb_enc(uaes_ctx* ctx, const uint8_t* in, uint8_t* out);
int uaes_crypt_ecb_dec(uaes_ctx* ctx, const uint8_t* in, uint8_t* out);

#ifdef __cplusplus
}
#endif
#endif
