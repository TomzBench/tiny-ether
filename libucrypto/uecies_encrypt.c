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

#include "uecies_encrypt.h"
#include "urand.h"
#include <string.h>

int
uecies_encrypt(const uecc_public_key* p,
               const uint8_t* shared_mac,
               size_t shared_mac_len,
               const uint8_t* in,
               size_t inlen,
               uint8_t* out)
{
    int err = 0;
    uint8_t key[32], mkey[32];
    uaes_iv iv;
    size_t tmp = sizeof(uecc_public_key_w_header);
    uhmac_sha256_ctx hmac;
    uecc_ctx ecc;
    uaes_ctr_128_key* ekey = (uaes_ctr_128_key*)&key[0];
    uaes_iv* iv_dst = (uaes_iv*)&out[65];

    uecc_key_init_new(&ecc);
    secp256k1_ec_pubkey_serialize(ecc.grp, &out[0], &tmp, &ecc.Q,
                                  SECP256K1_EC_UNCOMPRESSED);
    if (!(tmp == 65)) goto EXIT;
    err = uecc_agree(&ecc, p);
    if (err) goto EXIT;
    uhash_kdf(&ecc.z.b[1], 32, key, 32);
    usha256(&key[16], 16, mkey);
    urand(iv_dst->b, 16);
    memcpy(iv.b, iv_dst->b, 16);
    err = uaes_crypt_ctr_128(ekey, &iv, in, inlen, &out[81]);
    if (err) goto EXIT;
    uhmac_sha256_init(&hmac, mkey, 32);
    uhmac_sha256_update(&hmac, &out[65], 16 + inlen);
    uhmac_sha256_update(&hmac, shared_mac, shared_mac_len);
    uhmac_sha256_finish(&hmac, &out[65 + 16 + inlen]);
    uhmac_sha256_free(&hmac);
EXIT:
    uecc_key_deinit(&ecc);
    return err;
}

size_t
uecies_encrypt_size(size_t len)
{
    return len + 32 + 16 + 65;
}

//
//
//
