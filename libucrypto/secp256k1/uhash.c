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

#include "uhash.h"
#include <string.h>

void
usha256_init(usha256_ctx* ctx)
{
    secp256k1_sha256_initialize(ctx);
}

void
usha256_update(usha256_ctx* ctx, const uint8_t* msg, size_t mlen)
{
    secp256k1_sha256_write(ctx, msg, mlen);
}

void
usha256_finish(usha256_ctx* ctx, uint8_t* sha)
{
    secp256k1_sha256_finalize(ctx, sha);
}

void
usha256_free(usha256_ctx* ctx)
{
    ((void)ctx);
}

void
usha256(const uint8_t* msg, size_t msglen, uint8_t* sha)
{
    usha256_ctx ctx;
    usha256_init(&ctx);
    usha256_update(&ctx, msg, msglen);
    usha256_finish(&ctx, sha);
    usha256_free(&ctx);
}

void
uhmac_sha256_init(uhmac_sha256_ctx* ctx, const uint8_t* key, size_t klen)
{
    secp256k1_hmac_sha256_initialize(ctx, key, klen);
}

void
uhmac_sha256_update(uhmac_sha256_ctx* ctx, const uint8_t* b, size_t l)
{
    secp256k1_hmac_sha256_write(ctx, b, l);
}

void
uhmac_sha256_finish(uhmac_sha256_ctx* ctx, uint8_t* hmac)
{
    secp256k1_hmac_sha256_finalize(ctx, hmac);
}

void
uhmac_sha256_free(uhmac_sha256_ctx* ctx)
{
    ((void)ctx);
}

void
uhmac_sha256(
    const uint8_t* key,
    size_t keylen,
    const uint8_t* msg,
    size_t msglen,
    uint8_t* hmac)
{
    uhmac_sha256_ctx ctx;
    uhmac_sha256_init(&ctx, key, keylen);
    uhmac_sha256_update(&ctx, msg, msglen);
    uhmac_sha256_finish(&ctx, hmac);
    uhmac_sha256_free(&ctx);
}

void
uhash_kdf(uint8_t* z, size_t zlen, uint8_t* b, size_t keylen)
{
    usha256_ctx sha;
    uint8_t ctr[4] = { 0, 0, 0, 1 };
    uint8_t* end = &b[keylen];
    uint8_t s1 = 0;
    while (b < end) {
        uint8_t tmp[32];
        usha256_init(&sha);
        usha256_update(&sha, ctr, 4);
        usha256_update(&sha, z, zlen);
        usha256_update(&sha, &s1, 0);
        usha256_finish(&sha, tmp);
        memcpy(b, tmp, b + 32 <= end ? 32 : end - b);
        usha256_free(&sha);
        b += 32;

        // Nifty short circuit condition big endian counter
        if (++ctr[3] || ++ctr[2] || ++ctr[1] || ++ctr[0]) continue;
    }
}

//
//
//
