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

int test_devp2p_on_hello(void* ctx, const urlp* rlp);
int test_devp2p_on_ping(void* ctx, const urlp* rlp);
int test_devp2p_on_pong(void* ctx, const urlp* rlp);
int test_devp2p_on_disconnect(void* ctx, const urlp* rlp);

rlpx_devp2p_protocol_settings g_test_devp2p_settings = {
    .on_hello = test_devp2p_on_hello,
    .on_disconnect = test_devp2p_on_disconnect,
    .on_ping = test_devp2p_on_ping,
    .on_pong = test_devp2p_on_pong
};

int
test_protocol()
{
    int err = 0;
    test_session s;

    test_session_init(&s, TEST_VECTOR_LEGACY_GO);
    rlpx_test_mock_devp2p(&g_test_devp2p_settings);

    rlpx_io_nonce(s.alice);
    rlpx_io_nonce(s.bob);
    rlpx_io_connect(s.alice, &s.bob->skey->Q, "1.1.1.1", 33);
    rlpx_io_accept(s.bob, &s.alice->skey->Q);

    // Recv keys
    IF_ERR_EXIT(rlpx_io_recv_ack(s.alice, s.bob->io.b, s.bob->io.len));
    IF_ERR_EXIT(rlpx_io_recv_auth(s.bob, s.alice->io.b, s.alice->io.len));

    // Read/Write HELLO
    IF_ERR_EXIT(rlpx_io_send_hello(s.alice));
    IF_ERR_EXIT(rlpx_io_send_hello(s.bob));
    IF_ERR_EXIT(rlpx_io_recv(s.alice, s.bob->io.b, s.bob->io.len));
    IF_ERR_EXIT(rlpx_io_recv(s.bob, s.alice->io.b, s.alice->io.len));

    // Read/Write DISCONNECT
    IF_ERR_EXIT(
        rlpx_io_send_disconnect(s.alice, DEVP2P_DISCONNECT_BAD_VERSION));
    IF_ERR_EXIT(rlpx_io_send_disconnect(s.bob, DEVP2P_DISCONNECT_BAD_VERSION));
    IF_ERR_EXIT(rlpx_io_recv(s.alice, s.bob->io.b, s.bob->io.len));
    IF_ERR_EXIT(rlpx_io_recv(s.bob, s.alice->io.b, s.alice->io.len));

    // Read/Write PING
    IF_ERR_EXIT(rlpx_io_send_ping(s.alice));
    IF_ERR_EXIT(rlpx_io_send_ping(s.bob));
    IF_ERR_EXIT(rlpx_io_recv(s.alice, s.bob->io.b, s.bob->io.len));
    IF_ERR_EXIT(rlpx_io_recv(s.bob, s.alice->io.b, s.alice->io.len));

    // Read/Write PONG
    IF_ERR_EXIT(rlpx_io_send_pong(s.alice));
    IF_ERR_EXIT(rlpx_io_send_pong(s.bob));
    IF_ERR_EXIT(rlpx_io_recv(s.alice, s.bob->io.b, s.bob->io.len));
    IF_ERR_EXIT(rlpx_io_recv(s.bob, s.alice->io.b, s.alice->io.len));

    // Confirm all callbacks readback
    IF_ERR_EXIT((g_test_mask == 0x0f) ? 0 : -1);

EXIT:
    test_session_deinit(&s);
    return err;
}

int
test_devp2p_on_hello(void* ctx, const urlp* rlp)
{
    int err = 0;
    const char* mem;
    uint32_t num;
    rlpx_channel* ch = ctx;
    uint8_t remote_id[65];
    uecc_qtob(&ch->node.id, remote_id, 65);

    // Verify p2p ver
    rlpx_devp2p_protocol_p2p_version(rlp, &num);
    IF_ERR_EXIT((num == RLPX_VERSION_P2P) ? 0 : -1);

    // Verify client id
    rlpx_devp2p_protocol_client_id(rlp, &mem, &num);
    IF_ERR_EXIT((num == RLPX_CLIENT_ID_LEN) ? 0 : -1);
    IF_ERR_EXIT(memcmp(mem, RLPX_CLIENT_ID_STR, num) ? -1 : 0);

    // Verify caps
    IF_ERR_EXIT(rlpx_devp2p_protocol_capabilities(rlp, "p2p", 4));
    IF_ERR_EXIT(rlpx_devp2p_protocol_capabilities(rlp, "p2p", 4));

    // Verify listen port
    rlpx_devp2p_protocol_listen_port(rlp, &num);
    if (!((num == UDP_TEST_PORT) || (num == UDP_TEST_PORT + 1))) goto EXIT;

    // verify node_id
    rlpx_devp2p_protocol_node_id(rlp, &mem, &num);
    IF_ERR_EXIT((num == 64) ? 0 : -1);
    IF_ERR_EXIT(memcmp(mem, &remote_id[1], 64) ? -1 : 0);

    g_test_mask |= (0x01 << 0);

EXIT:
    return err;
}
int
test_devp2p_on_disconnect(void* ctx, const urlp* rlp)
{
    ((void)ctx);
    ((void)rlp);
    int err = 0;
    g_test_mask |= (0x01 << 1);
    return err;
}

int
test_devp2p_on_ping(void* ctx, const urlp* rlp)
{
    ((void)ctx);
    ((void)rlp);
    int err = 0;
    g_test_mask |= (0x01 << 2);
    return err;
}

int
test_devp2p_on_pong(void* ctx, const urlp* rlp)
{
    ((void)ctx);
    ((void)rlp);
    int err = 0;
    g_test_mask |= (0x01 << 3);
    return err;
}
