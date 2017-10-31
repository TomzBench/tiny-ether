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

#ifndef HMAC_H_
#define HMAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "secp256k1_sha256.h"

typedef secp256k1_hmac_sha256_t uhmac_sha256_ctx;
typedef secp256k1_sha256_t usha256_ctx;
typedef secp256k1_rfc6979_hmac_sha256_t urfc6979_hmac_sha256_ctx;

void usha256_init(usha256_ctx*);
void usha256_update(usha256_ctx*, const uint8_t*, size_t);
void usha256_finish(usha256_ctx*, uint8_t*);
void usha256_free(usha256_ctx*);
void usha256(const uint8_t* msg, size_t msglen, uint8_t* hmac);

void uhmac_sha256_init(uhmac_sha256_ctx*, const uint8_t*, size_t);
void uhmac_sha256_update(uhmac_sha256_ctx*, const uint8_t*, size_t);
void uhmac_sha256_finish(uhmac_sha256_ctx*, uint8_t*);
void uhmac_sha256_free(uhmac_sha256_ctx*);
void uhmac_sha256(
    const uint8_t* key,
    size_t keylen,
    const uint8_t* msg,
    size_t msglen,
    uint8_t* hmac);
void uhash_kdf(uint8_t*, size_t, uint8_t*, size_t);

#ifdef __cplusplus
}
#endif
#endif
