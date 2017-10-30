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

// urand.c

#include "urand.h"

int urand(uint8_t* b, size_t l);
int urand_w_custom(uint8_t* b, size_t l, const uint8_t* pers, size_t psz);
int urand_min_max_u8(uint8_t, uint8_t);

int
urand(uint8_t* b, size_t l)
{
    return urand_w_custom(b, l, NULL, 0);
}

int
urand_w_custom(uint8_t* b, size_t l, const uint8_t* pers, size_t psz)
{
    int err;
    mbedtls_ctr_drbg_context ctx;
    mbedtls_entropy_context ent;

    mbedtls_ctr_drbg_init(&ctx);
    mbedtls_entropy_init(&ent);

    err = mbedtls_ctr_drbg_seed(&ctx, mbedtls_entropy_func, &ent, pers, psz);
    if (!err) err = mbedtls_ctr_drbg_random(&ctx, b, l);

    mbedtls_ctr_drbg_free(&ctx);
    mbedtls_entropy_free(&ent);
    return err;
}

// int padAmount(rand()%100 + 100);
int
urand_min_max_u8(uint8_t start, uint8_t end)
{
    uint8_t r;
    urand(&r, 1);
    if (end <= start) return -1;
    return r % (end - start) + start;
}

//
//
//
