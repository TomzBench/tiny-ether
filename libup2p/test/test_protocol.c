#include "test.h"

int test_protocol_hello();

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
    err |= test_protocol_hello();
    return err;
}

int
test_protocol_hello()
{
    int err;
    test_session s;
    size_t lena = 1000, lenb = 1000, alen = 1000, blen = 1000;
    uint8_t from_alice[lena], from_bob[lenb], a[alen], b[blen];

    test_session_init(&s, TEST_VECTOR_LEGACY_GO);
    rlpx_test_mock_devp2p(&g_test_devp2p_settings);

    // Bob exchange alice keys
    IF_ERR_EXIT(rlpx_ch_auth_write(s.alice, rlpx_ch_pub_skey(s.bob), a, &alen));
    IF_ERR_EXIT(rlpx_ch_auth_load(s.bob, a, alen));

    // Alice exchange bob keys
    IF_ERR_EXIT(rlpx_ch_ack_write(s.bob, rlpx_ch_pub_skey(s.alice), b, &blen));
    IF_ERR_EXIT(rlpx_ch_ack_load(s.alice, b, blen));

    // Update secrets
    IF_ERR_EXIT(rlpx_ch_secrets(s.bob, 0, b, blen, a, alen));
    IF_ERR_EXIT(rlpx_ch_secrets(s.alice, 1, a, alen, b, blen));

    // Write some packets
    IF_ERR_EXIT(rlpx_ch_write_hello(s.alice, from_alice, &lena));
    IF_ERR_EXIT(rlpx_ch_write_hello(s.bob, from_bob, &lenb));

    // Read
    IF_ERR_EXIT(rlpx_ch_read(s.alice, from_bob, lenb));
    IF_ERR_EXIT(rlpx_ch_read(s.bob, from_alice, lena));

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
    rlpx_devp2p_hello_p2p_version(rlp, &num);
    IF_ERR_EXIT((num == RLPX_VERSION_P2P) ? 0 : -1);

    // Verify client id
    rlpx_devp2p_hello_client_id(rlp, &mem, &num);
    IF_ERR_EXIT((num == RLPX_CLIENT_ID_LEN) ? 0 : -1);
    IF_ERR_EXIT(memcmp(mem, RLPX_CLIENT_ID_STR, num) ? -1 : 0);

    // Verify caps
    IF_ERR_EXIT(rlpx_devp2p_hello_capabilities(rlp, "les", 2));
    IF_ERR_EXIT(rlpx_devp2p_hello_capabilities(rlp, "les", 2));

    // Verify listen port
    rlpx_devp2p_hello_listen_port(rlp, &num);
    IF_ERR_EXIT((num == 44) ? 0 : -1);

    // verify node_id
    rlpx_devp2p_hello_node_id(rlp, &mem, &num);
    IF_ERR_EXIT((num == 65) ? 0 : -1);
    IF_ERR_EXIT(memcmp(mem, "A", 1) ? -1 : 0);

EXIT:
    return err;
}

int
test_devp2p_on_ping(void* ctx, const urlp* rlp)
{
}

int
test_devp2p_on_pong(void* ctx, const urlp* rlp)
{
}

int
test_devp2p_on_disconnect(void* ctx, const urlp* rlp)
{
}
