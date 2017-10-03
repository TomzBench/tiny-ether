#include "test.h"
#include "rlpx_test_helpers.h"

extern test_vector g_test_vectors[];
extern const char* g_alice_epub;
extern const char* g_bob_epub;
extern const char* g_hello_packet;
extern const char* g_go_aes_secret;
extern const char* g_go_mac_secret;
extern const char* g_go_foo;

int test_frame_read();
int test_frame_write();

int
test_frame()
{
    int err = 0;
    err |= test_frame_read();
    err |= test_frame_write();
    return err;
}

int
test_frame_read()
{
    int err;
    test_session s;
    test_session_init(&s, TEST_VECTOR_LEGACY_GO);
    uint8_t aes[32], mac[32];
    urlp* frame = NULL;
    const urlp* seek;
    uint32_t p2pver;
    memcpy(aes, makebin(g_go_aes_secret, NULL), 32);
    memcpy(mac, makebin(g_go_mac_secret, NULL), 32);

    // Set some phoney nonces
    rlpx_test_nonce_set(s.bob, &s.bob_n);
    rlpx_test_nonce_set(s.alice, &s.alice_n);
    rlpx_test_remote_nonce_set(s.bob, &s.alice_n);
    rlpx_test_remote_nonce_set(s.alice, &s.bob_n);

    // Update our secrets
    IF_ERR_EXIT(rlpx_auth_read(s.bob, s.auth, s.authlen));
    IF_ERR_EXIT(rlpx_expect_secrets(s.bob, 0, s.ack, s.acklen, s.auth,
                                    s.authlen, aes, mac, NULL));
    IF_ERR_EXIT(rlpx_frame_parse(s.bob, makebin(g_hello_packet, NULL),
                                 strlen(g_hello_packet) / 2, &frame));
    seek = urlp_at(urlp_at(frame, 1), 1); // get body frame
    IF_ERR_EXIT(rlpx_frame_hello_p2p_version(seek, &p2pver));
    IF_ERR_EXIT(p2pver == 3 ? 0 : -1);
    IF_ERR_EXIT(rlpx_frame_hello_capabilities(seek, "a", 0));
    IF_ERR_EXIT(rlpx_frame_hello_capabilities(seek, "b", 2));
    urlp_free(&frame);
EXIT:
    test_session_deinit(&s);
    return err;
}

int
test_frame_write()
{

    int err = 0;
    test_session s;
    test_session_init(&s, 1);
    size_t lena = 1000, lenb = 1000, alen = 1000, blen = 1000;
    uint8_t from_alice[lena], from_bob[lenb], a[alen], b[blen];
    urlp *framea, *frameb;

    // Bob exchange alice keys
    IF_ERR_EXIT(rlpx_auth_write(s.alice, rlpx_public_skey(s.bob), a, &alen));
    IF_ERR_EXIT(rlpx_auth_read(s.bob, a, alen));

    // Alice exchange bob keys
    IF_ERR_EXIT(rlpx_ack_write(s.bob, rlpx_public_skey(s.alice), b, &blen));
    IF_ERR_EXIT(rlpx_ack_read(s.alice, b, blen));

    // Check key exchange
    IF_ERR_EXIT(check_q(rlpx_remote_public_ekey(s.alice), g_bob_epub));
    IF_ERR_EXIT(check_q(rlpx_remote_public_ekey(s.bob), g_alice_epub));

    // Update secrets
    IF_ERR_EXIT(rlpx_secrets(s.bob, 0, b, blen, a, alen));
    IF_ERR_EXIT(rlpx_secrets(s.alice, 1, a, alen, b, blen));

    // Write some packets
    IF_ERR_EXIT(rlpx_frame_hello_write(s.alice, from_alice, &lena));
    IF_ERR_EXIT(rlpx_frame_hello_write(s.bob, from_bob, &lenb));
    IF_ERR_EXIT(rlpx_frame_parse(s.alice, from_bob, lenb, &frameb));
    IF_ERR_EXIT(rlpx_frame_parse(s.bob, from_alice, lena, &framea));
EXIT:
    err = 0;
    test_session_deinit(&s);
    return err;
}
