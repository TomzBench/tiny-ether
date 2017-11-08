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

#ifndef TEST_H_
#define TEST_H_

#include "kademlia/ktable.h"
#include "rlpx_io_devp2p.h"
#include "rlpx_io_discovery.h"
#include "rlpx_test_helpers.h"
#include "test_vectors.h"
#include "unonce.h"
#include "usys_log.h"
#include <string.h>

#define IF_ERR_EXIT(f)                                                         \
    do {                                                                       \
        if ((err = (f)) != 0) {                                                \
            goto EXIT;                                                         \
        }                                                                      \
    } while (0)
#define IF_NEG_EXIT(val, f)                                                    \
    do {                                                                       \
        if ((val = (f)) < 0) goto EXIT;                                        \
    } while (0)

typedef struct
{
    const char* auth;    /*!< auth cipher text */
    const char* ack;     /*!< ack cipher text */
    const char* alice_s; /*!< static key */
    const char* alice_e; /*!< ephermeral key */
    const char* alice_n; /*!< nonce */
    const char* bob_s;   /*!< static key */
    const char* bob_e;   /*!< ephermeral key */
    const char* bob_n;   /*!< nonce */
    uint64_t authver;
    uint64_t ackver;
} test_vector;

typedef struct
{
    uecc_ctx skey_a, skey_b;     /*!< static keys */
    uint32_t udp[2];             /*!< */
    rlpx_io *alice, *bob;        /*!< rlpx context */
    size_t authlen, acklen;      /*!< size of cipher */
    uint8_t auth[800], ack[800]; /*!< cipher test buffers */
    h256 alice_n, bob_n;         /*!< nonces used sometimes */
} test_session;

const uint8_t* makebin(const char* str, size_t* len);
int cmp_q(const uecc_public_key* a, const uecc_public_key* b);
int check_q(const uecc_public_key* key, const char* str);
int test_session_init(test_session*, int);
void test_session_deinit(test_session*);
void test_session_connect(test_session* s);
void test_session_handshake(test_session* s);

int test_handshake(void);
int test_frame(void);
int test_protocol(void);
int test_enode(void);
int test_kademlia(void);
int test_discovery(void);

#endif
