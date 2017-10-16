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
    // size_t lena = 1000, lenb = 1000;
    // uint8_t from_alice[lena], from_bob[lenb];

    test_session_init(&s, TEST_VECTOR_LEGACY_GO);
    rlpx_test_mock_devp2p(&g_test_devp2p_settings);

    rlpx_ch_nonce(s.alice);
    rlpx_ch_nonce(s.bob);
    rlpx_ch_connect(s.alice, &s.bob->skey.Q, "1.1.1.1", 33);
    rlpx_ch_accept(s.bob, &s.alice->skey.Q);

    // Recv keys
    IF_ERR_EXIT(rlpx_ch_recv_ack(s.alice, s.bob->io.b, s.bob->io.len));
    IF_ERR_EXIT(rlpx_ch_recv_auth(s.bob, s.alice->io.b, s.alice->io.len));

    // Read/Write HELLO
    IF_ERR_EXIT(rlpx_ch_send_hello(s.alice));
    IF_ERR_EXIT(rlpx_ch_send_hello(s.bob));
    IF_ERR_EXIT(rlpx_ch_recv(s.alice, s.bob->io.b, s.bob->io.len));
    IF_ERR_EXIT(rlpx_ch_recv(s.bob, s.alice->io.b, s.alice->io.len));

    // Read/Write DISCONNECT
    IF_ERR_EXIT(
        rlpx_ch_send_disconnect(s.alice, DEVP2P_DISCONNECT_BAD_VERSION));
    IF_ERR_EXIT(rlpx_ch_send_disconnect(s.bob, DEVP2P_DISCONNECT_BAD_VERSION));
    IF_ERR_EXIT(rlpx_ch_recv(s.alice, s.bob->io.b, s.bob->io.len));
    IF_ERR_EXIT(rlpx_ch_recv(s.bob, s.alice->io.b, s.alice->io.len));

    // Read/Write PING
    IF_ERR_EXIT(rlpx_ch_send_ping(s.alice));
    IF_ERR_EXIT(rlpx_ch_send_ping(s.bob));
    IF_ERR_EXIT(rlpx_ch_recv(s.alice, s.bob->io.b, s.bob->io.len));
    IF_ERR_EXIT(rlpx_ch_recv(s.bob, s.alice->io.b, s.alice->io.len));

    // Read/Write PONG
    IF_ERR_EXIT(rlpx_ch_send_pong(s.alice));
    IF_ERR_EXIT(rlpx_ch_send_pong(s.bob));
    IF_ERR_EXIT(rlpx_ch_recv(s.alice, s.bob->io.b, s.bob->io.len));
    IF_ERR_EXIT(rlpx_ch_recv(s.bob, s.alice->io.b, s.alice->io.len));

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
    IF_ERR_EXIT((num == 44) ? 0 : -1);

    // verify node_id
    rlpx_devp2p_protocol_node_id(rlp, &mem, &num);
    IF_ERR_EXIT((num == 65) ? 0 : -1);
    IF_ERR_EXIT(memcmp(mem, "A", 1) ? -1 : 0);

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
