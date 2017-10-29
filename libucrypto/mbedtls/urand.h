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

#ifndef URAND_H_
#define URAND_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

int urand(uint8_t* b, size_t l);
int urand_w_custom(uint8_t* b, size_t l, const uint8_t* pers, size_t psz);
int urand_min_max_u8(uint8_t, uint8_t);

#ifdef __cplusplus
}
#endif
#endif
