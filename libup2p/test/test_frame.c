#include "test.h"
#include "rlpx_test_helpers.h"

extern test_vector g_test_vectors[];
extern const char* g_alice_epub;
extern const char* g_bob_epub;
extern const char* g_hello_packet;
extern const char* g_aes_secret;
extern const char* g_mac_secret;
extern const char* g_foo;

int
test_frame()
{
    int err;
    test_session s;
    test_session_init(&s, 0);
    uint8_t aes[32], mac[32], foo[32];
    memcpy(aes, makebin(g_aes_secret, NULL), 32);
    memcpy(mac, makebin(g_mac_secret, NULL), 32);
    memcpy(foo, makebin(g_foo, NULL), 32);

    // Set some phoney nonces
    rlpx_test_nonce_set(s.bob, &s.bob_n);
    rlpx_test_nonce_set(s.alice, &s.alice_n);
    rlpx_test_remote_nonce_set(s.bob, &s.alice_n);
    rlpx_test_remote_nonce_set(s.alice, &s.bob_n);

    // Update our secrets
    IF_ERR_EXIT(rlpx_auth_read(s.bob, s.auth, s.authlen));
    IF_ERR_EXIT(rlpx_expect_secrets(s.bob, 0, s.ack, s.acklen, s.auth,
                                    s.authlen, aes, mac, foo));
    IF_ERR_EXIT(rlpx_test_hello(s.bob, makebin(g_hello_packet, NULL),
                                strlen(g_hello_packet) / 2));
EXIT:
    test_session_deinit(&s);
    return err;
}
