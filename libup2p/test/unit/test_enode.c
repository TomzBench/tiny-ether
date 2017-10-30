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

#include "test.h"

int test_hex();
int test_node();

int
test_enode()
{
    int err = 0;
    err |= test_hex();
    err |= test_node();
    return err;
}

int
test_hex()
{
    int err = 0;
    uint8_t raw[64], rawa[65], rawb[65];
    char mem[129], mema[129], memb[129];
    test_session s;
    test_session_init(&s, 1);
    IF_ERR_EXIT(rlpx_node_hex_to_bin(BOB_EPUB, 0, raw, NULL));
    IF_ERR_EXIT(rlpx_node_bin_to_hex(raw, 64, mem, NULL));
    IF_ERR_EXIT(memcmp(mem, BOB_EPUB, 128) ? -1 : 0);
    IF_ERR_EXIT(uecc_qtob(&s.alice->skey->Q, rawa, 65));
    IF_ERR_EXIT(uecc_qtob(&s.bob->skey->Q, rawb, 65));
    IF_ERR_EXIT(rlpx_node_bin_to_hex(&rawa[1], 64, mema, NULL));
    IF_ERR_EXIT(rlpx_node_bin_to_hex(&rawb[1], 64, memb, NULL));
    IF_ERR_EXIT(check_q(&s.alice->skey->Q, mema));
    IF_ERR_EXIT(check_q(&s.bob->skey->Q, memb));
EXIT:
    test_session_deinit(&s);
    return err;
}

int
test_node()
{
    int err = 0;
    char* alice = "enode://" ALICE_SPUB "@1.1.1.1:33.89";
    char* maxok = "enode://" ALICE_SPUB "@111.111.111.111:65535.65535";
    char* failsz = "enode://" ALICE_SPUB "x@111.111.111.111:65535.65535";
    char* failfmt = "enode://" ALICE_SPUB " 111.111.111.111@65535.65535";
    rlpx_node node_alice, node_maxok, node_failsz, node_failfmt;
    test_session s;

    test_session_init(&s, 1);
    IF_ERR_EXIT(rlpx_node_init_enode(&node_alice, alice));
    IF_ERR_EXIT(rlpx_node_init_enode(&node_maxok, maxok));
    IF_ERR_EXIT(rlpx_node_init_enode(&node_failsz, failsz) ? 0 : -1);
    IF_ERR_EXIT(rlpx_node_init_enode(&node_failfmt, failfmt) ? 0 : -1);
    IF_ERR_EXIT(rlpx_node_init_enode(&node_alice, alice));
    IF_ERR_EXIT(cmp_q(&node_alice.id, &s.alice->skey->Q));
    IF_ERR_EXIT((node_alice.port_tcp == 33) ? 0 : -1);
    IF_ERR_EXIT((node_alice.port_udp == 89) ? 0 : -1);
    IF_ERR_EXIT(cmp_q(&node_maxok.id, &s.alice->skey->Q));
    IF_ERR_EXIT((node_maxok.port_tcp == 65535) ? 0 : -1);
    IF_ERR_EXIT((node_maxok.port_udp == 65535) ? 0 : -1);

EXIT:
    test_session_deinit(&s);
    rlpx_node_deinit(&node_alice);
    rlpx_node_deinit(&node_maxok);
    rlpx_node_deinit(&node_failsz);
    rlpx_node_deinit(&node_failfmt);
    return err;
}
