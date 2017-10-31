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

#ifndef UECIES_ENCRYPT_H_
#define UECIES_ENCRYPT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "uaes.h"
#include "uecc.h"
#include "uhash.h"

int uecies_encrypt(
    const uecc_public_key*,
    const uint8_t* shared_mac,
    size_t shared_mac_len,
    const uint8_t* plain,
    size_t plain_len,
    uint8_t* cipher);

size_t uecies_encrypt_size(size_t len);

#ifdef __cplusplus
}
#endif
#endif
