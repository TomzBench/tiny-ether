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

#ifndef RLPX_TEST_H_
#define RLPX_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "rlpx_channel.h"
#include "uaes.h"
#include "uecc.h"
#include "ukeccak256.h"
#include "urlp.h"

/*!< setters/getters for internal state (for test purposes) */
uecc_ctx* rlpx_test_skey(rlpx_channel* ch);
uecc_ctx* rlpx_test_ekey(rlpx_channel* ch);
void rlpx_test_nonce_set(rlpx_channel* s, h256* nonce);
void rlpx_test_ekey_set(rlpx_channel* s, uecc_ctx* ekey);
ukeccak256_ctx* rlpx_test_ingress(rlpx_channel* ch);
ukeccak256_ctx* rlpx_test_egress(rlpx_channel* ch);
uaes_ctx* rlpx_test_aes_mac(rlpx_channel* ch);
uaes_ctx* rlpx_test_aes_enc(rlpx_channel* ch);
uaes_ctx* rlpx_test_aes_dec(rlpx_channel* ch);
int rlpx_test_expect_secrets(
    rlpx_channel* s,
    int orig,
    uint8_t* sent,
    uint32_t sentlen,
    uint8_t* recv,
    uint32_t recvlen,
    uint8_t* aes,
    uint8_t* mac,
    uint8_t* foo);
void rlpx_test_mock_devp2p(rlpx_devp2p_protocol_settings* settings);
#ifdef __cplusplus
}
#endif
#endif
