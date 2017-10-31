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

#include "uaes.h"
#include <string.h>

int
uaes_init(uaes_ctx* ctx, int keysz, uint8_t* key)
{
    memset(ctx, 0, sizeof(uaes_ctx));
    mbedtls_aes_init(&ctx->ctx);
    int err;
    err = (mbedtls_aes_setkey_enc(&ctx->ctx, key, keysz)) ? -1 : 0;
    if (!(err == 0)) uaes_deinit(&ctx);
    return err;
}

void
uaes_deinit(uaes_ctx** ctx_p)
{
    uaes_ctx* ctx = *ctx_p;
    *ctx_p = NULL;
    mbedtls_aes_free(&ctx->ctx);
}

void
uaes_crypt_reset(uaes_ctx* ctx)
{
    memset(ctx->iv, 0, 16);
}

int
uaes_crypt_ctr(
    int keysz,
    uint8_t* key,
    uint8_t* iv,
    const uint8_t* in,
    size_t inlen,
    uint8_t* out)
{

    int err = 0;
    uaes_ctx tmp;
    uint8_t block[16];
    size_t nc_off = 0;
    err = uaes_init(&tmp, keysz, key);
    if (err) return err;
    err = mbedtls_aes_crypt_ctr(&tmp.ctx, inlen, &nc_off, iv, block, in, out);
    return err;
}

int
uaes_crypt_ctr_update(
    uaes_ctx* ctx,
    const uint8_t* in,
    size_t inlen,
    uint8_t* out)
{
    return uaes_crypt_ctr_op(ctx, ctx->iv, in, inlen, out);
}

int
uaes_crypt_ctr_op(
    uaes_ctx* ctx,
    uint8_t* iv,
    const uint8_t* in,
    size_t inlen,
    uint8_t* out)
{
    int err = 0;
    uint8_t block[16];
    size_t nc_off = 0;
    err = mbedtls_aes_crypt_ctr(&ctx->ctx, inlen, &nc_off, iv, block, in, out);
    return err;
}

int
uaes_crypt_ecb_enc(uaes_ctx* ctx, const uint8_t* in, uint8_t* out)
{
    return mbedtls_aes_crypt_ecb(&ctx->ctx, MBEDTLS_AES_ENCRYPT, in, out);
}

int
uaes_crypt_ecb_dec(uaes_ctx* ctx, const uint8_t* in, uint8_t* out)
{
    return mbedtls_aes_crypt_ecb(&ctx->ctx, MBEDTLS_AES_DECRYPT, in, out);
}

//
//
//
