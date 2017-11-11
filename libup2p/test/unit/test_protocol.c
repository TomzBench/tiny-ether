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

uint32_t g_test_mask = 0;

int
test_protocol()
{
    int err = 0;
    uint32_t lena = 10000, lenb = 10000;
    uint8_t buffa[lena], buffb[lenb];
    test_session s;

    test_session_init(&s, TEST_VECTOR_LEGACY_GO);
    test_session_connect(&s);
    test_session_handshake(&s);

    // Hello
    err = rlpx_io_devp2p_write_hello(
        &s.alice->x, //
        *s.alice->listen_port,
        &s.alice->node_id[1],
        buffa,
        &lena);
    IF_ERR_EXIT(err);
    err = rlpx_io_devp2p_write_hello(
        &s.bob->x, //
        *s.bob->listen_port,
        &s.bob->node_id[1],
        buffb,
        &lenb);
    IF_ERR_EXIT(err);
    IF_ERR_EXIT(rlpx_io_recv(s.alice, buffb, lenb));
    IF_ERR_EXIT(rlpx_io_recv(s.bob, buffa, lena));

    // Disconnect
    lena = sizeof(buffa);
    lenb = sizeof(buffb);
    err = rlpx_io_devp2p_write_disconnect(
        &s.alice->x, //
        DEVP2P_DISCONNECT_BAD_VERSION,
        buffa,
        &lena);
    IF_ERR_EXIT(err);
    err = rlpx_io_devp2p_write_disconnect(
        &s.bob->x, //
        DEVP2P_DISCONNECT_BAD_VERSION,
        buffb,
        &lenb);
    IF_ERR_EXIT(err);
    IF_ERR_EXIT(rlpx_io_recv(s.alice, buffb, lenb));
    IF_ERR_EXIT(rlpx_io_recv(s.bob, buffa, lena));

    // Ping
    lena = sizeof(buffa);
    lenb = sizeof(buffb);
    IF_ERR_EXIT(rlpx_io_devp2p_write_ping(&s.alice->x, buffa, &lena));
    IF_ERR_EXIT(rlpx_io_recv(s.bob, buffa, lena));

    // Pong
    lena = sizeof(buffa);
    lenb = sizeof(buffb);
    IF_ERR_EXIT(rlpx_io_devp2p_write_pong(&s.alice->x, buffa, &lena));
    IF_ERR_EXIT(rlpx_io_recv(s.bob, buffa, lena));

EXIT:
    test_session_deinit(&s);
    return err;
}
