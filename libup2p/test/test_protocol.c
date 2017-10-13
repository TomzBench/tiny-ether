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
    size_t lena = 1000, lenb = 1000, alen = 1000, blen = 1000;
    uint8_t from_alice[lena], from_bob[lenb], a[alen], b[blen];

    test_session_init(&s, TEST_VECTOR_LEGACY_GO);
    rlpx_test_mock_devp2p(&g_test_devp2p_settings);

    // Bob exchange alice keys
    IF_ERR_EXIT(rlpx_ch_write_auth(s.alice, &s.bob->skey.Q, a, &alen));
    IF_ERR_EXIT(rlpx_ch_auth_load(s.bob, a, alen));

    // Alice exchange bob keys
    IF_ERR_EXIT(rlpx_ch_write_ack(s.bob, &s.alice->skey.Q, b, &blen));
    IF_ERR_EXIT(rlpx_ch_ack_load(s.alice, b, blen));

    // Update secrets
    IF_ERR_EXIT(rlpx_ch_secrets(s.bob, 0, b, blen, a, alen));
    IF_ERR_EXIT(rlpx_ch_secrets(s.alice, 1, a, alen, b, blen));

    // Read/Write HELLO
    IF_ERR_EXIT(rlpx_ch_write_hello(s.alice, from_alice, &lena));
    IF_ERR_EXIT(rlpx_ch_write_hello(s.bob, from_bob, &lenb));
    IF_ERR_EXIT(rlpx_ch_read(s.alice, from_bob, lenb));
    IF_ERR_EXIT(rlpx_ch_read(s.bob, from_alice, lena));

    // Read/Write DISCONNECT
    lena = 1000;
    lenb = 1000;
    IF_ERR_EXIT(rlpx_ch_write_disconnect(s.alice, DEVP2P_DISCONNECT_BAD_VERSION,
                                         from_alice, &lena));
    IF_ERR_EXIT(rlpx_ch_write_disconnect(s.bob, DEVP2P_DISCONNECT_BAD_VERSION,
                                         from_bob, &lenb));
    IF_ERR_EXIT(rlpx_ch_read(s.alice, from_bob, lenb));
    IF_ERR_EXIT(rlpx_ch_read(s.bob, from_alice, lena));

    // Read/Write PING
    lena = 1000;
    lenb = 1000;
    IF_ERR_EXIT(rlpx_ch_write_ping(s.alice, from_alice, &lena));
    IF_ERR_EXIT(rlpx_ch_write_ping(s.bob, from_bob, &lenb));
    IF_ERR_EXIT(rlpx_ch_read(s.alice, from_bob, lenb));
    IF_ERR_EXIT(rlpx_ch_read(s.bob, from_alice, lena));

    // Read/Write PONG
    lena = 1000;
    lenb = 1000;
    IF_ERR_EXIT(rlpx_ch_write_pong(s.alice, from_alice, &lena));
    IF_ERR_EXIT(rlpx_ch_write_pong(s.bob, from_bob, &lenb));
    IF_ERR_EXIT(rlpx_ch_read(s.alice, from_bob, lenb));
    IF_ERR_EXIT(rlpx_ch_read(s.bob, from_alice, lena));

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
    IF_ERR_EXIT(rlpx_devp2p_protocol_capabilities(rlp, "les", 2));
    IF_ERR_EXIT(rlpx_devp2p_protocol_capabilities(rlp, "les", 2));

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
