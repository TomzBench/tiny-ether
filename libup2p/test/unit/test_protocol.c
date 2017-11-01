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
    rlpx_io *alice, *bob;
    test_session s;
    test_session_init(&s, TEST_VECTOR_LEGACY_GO);

    alice = (rlpx_io*)s.alice;
    bob = (rlpx_io*)s.bob;
    // rlpx_test_mock_devp2p(&g_test_devp2p_settings);

    rlpx_io_nonce(alice);
    rlpx_io_nonce(bob);
    rlpx_io_connect(alice, &bob->skey->Q, "1.1.1.1", 33);
    rlpx_io_accept(bob, &alice->skey->Q);

    // Recv keys
    IF_ERR_EXIT(rlpx_io_recv_ack(alice, bob->io.b, bob->io.len));
    IF_ERR_EXIT(rlpx_io_recv_auth(bob, alice->io.b, alice->io.len));

    // Read/Write HELLO
    IF_ERR_EXIT(rlpx_io_send_hello(s.alice));
    IF_ERR_EXIT(rlpx_io_send_hello(s.bob));
    IF_ERR_EXIT(rlpx_io_recv(alice, bob->io.b, bob->io.len));
    IF_ERR_EXIT(rlpx_io_recv(bob, alice->io.b, alice->io.len));

    // Read/Write DISCONNECT
    IF_ERR_EXIT(
        rlpx_io_send_disconnect(s.alice, DEVP2P_DISCONNECT_BAD_VERSION));
    IF_ERR_EXIT(rlpx_io_send_disconnect(s.bob, DEVP2P_DISCONNECT_BAD_VERSION));
    IF_ERR_EXIT(rlpx_io_recv(alice, bob->io.b, bob->io.len));
    IF_ERR_EXIT(rlpx_io_recv(bob, alice->io.b, alice->io.len));

    // PING/PONG
    IF_ERR_EXIT(rlpx_io_send_ping(s.alice));
    IF_ERR_EXIT(rlpx_io_recv(bob, alice->io.b, alice->io.len));
    IF_ERR_EXIT(rlpx_io_recv(alice, bob->io.b, bob->io.len)); // recv pong

    // PING/PONG
    IF_ERR_EXIT(rlpx_io_send_ping(s.bob));
    IF_ERR_EXIT(rlpx_io_recv(alice, bob->io.b, bob->io.len));
    IF_ERR_EXIT(rlpx_io_recv(bob, alice->io.b, alice->io.len)); // recv pong

EXIT:
    test_session_deinit(&s);
    return err;
}
