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
    test_session s;
    test_session_init(&s, TEST_VECTOR_LEGACY_GO);
    rlpx_io_devp2p* a = s.alice->protocols[0].context;
    rlpx_io_devp2p* b = s.bob->protocols[0].context;

    rlpx_io_nonce(s.alice);
    rlpx_io_nonce(s.bob);
    rlpx_io_connect(s.alice, &s.bob->skey->Q, "1.1.1.1", 33);
    rlpx_io_accept(s.bob, &s.alice->skey->Q);

    // Recv keys
    IF_ERR_EXIT(
        rlpx_io_recv_ack(s.alice, rlpx_io_buffer(s.bob), s.bob->io.len));
    IF_ERR_EXIT(rlpx_io_recv_auth(s.bob, s.alice->io.b, s.alice->io.len));

// TODO refactor rlpx_io so to better test protocol
// Read/Write HELLO
// IF_ERR_EXIT(rlpx_io_devp2p_send_hello(s.alice->protocols[0].context));
// IF_ERR_EXIT(rlpx_io_devp2p_send_hello(s.bob->protocols[0].context));
// IF_ERR_EXIT(rlpx_io_recv(s.alice, rlpx_io_buffer(s.bob), s.bob->io.len));
// IF_ERR_EXIT(rlpx_io_recv(s.bob, s.alice->io.b, s.alice->io.len));

// Read/Write DISCONNECT
// IF_ERR_EXIT(
// rlpx_io_devp2p_send_disconnect(a, DEVP2P_DISCONNECT_BAD_VERSION));
// IF_ERR_EXIT(
// rlpx_io_devp2p_send_disconnect(b, DEVP2P_DISCONNECT_BAD_VERSION));
// IF_ERR_EXIT(rlpx_io_recv(s.alice, rlpx_io_buffer(s.bob), s.bob->io.len));
// IF_ERR_EXIT(rlpx_io_recv(s.bob, s.alice->io.b, s.alice->io.len));

// PING/PONG
// IF_ERR_EXIT(rlpx_io_devp2p_send_ping(s.alice->protocols[0].context));
// IF_ERR_EXIT(rlpx_io_recv(s.bob, s.alice->io.b, s.alice->io.len));
// IF_ERR_EXIT(rlpx_io_recv(s.alice, rlpx_io_buffer(s.bob), s.bob->io.len));

// PING/PONG
// IF_ERR_EXIT(rlpx_io_devp2p_send_ping(s.bob->protocols[0].context));
// IF_ERR_EXIT(rlpx_io_recv(s.alice, rlpx_io_buffer(s.bob), s.bob->io.len));
// IF_ERR_EXIT(rlpx_io_recv(s.bob, s.alice->io.b, s.alice->io.len));

EXIT:
    test_session_deinit(&s);
    return err;
}
