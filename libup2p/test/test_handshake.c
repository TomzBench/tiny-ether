#include "test.h"
#include "rlpx_test.h"

// clang-format off
#define IF_ERR_EXIT(f)                    \
    do {                                  \
        if ((err = (f)) != 0) goto EXIT;  \
    } while (0)
#define IF_NEG_EXIT(val, f)               \
    do {                                  \
        if ((val = (f)) < 0) goto EXIT;   \
    } while (0)
// clang-format on

extern test_vector g_test_vectors[];
extern const char* g_alice_spri;
extern const char* g_alice_epri;
extern const char* g_bob_spri;
extern const char* g_bob_epri;
extern const char* g_alice_epub;
extern const char* g_bob_epub;
extern const char* g_aes_secret;
extern const char* g_mac_secret;
extern const char* g_foo;
extern const char* g_alice_nonce;
extern const char* g_bob_nonce;

int test_read();
int test_write();
int test_secrets();

int
test_handshake()
{
    int err = 0;
    err |= test_read();
    err |= test_write();
    err |= test_secrets();

    return err;
}

int
test_read()
{
    int err;
    test_session s;
    test_vector* tv = g_test_vectors;

    int i = 0;
    while (tv->auth) {
        test_session_init(&s, i);
        if (rlpx_auth_read(s.bob, s.auth, s.authlen)) break;
        if (rlpx_ack_read(s.alice, s.ack, s.acklen)) break;
        if (!(rlpx_version_remote(s.bob) == tv->authver)) break;
        if (!(rlpx_version_remote(s.alice) == tv->ackver)) break;
        if ((check_q(rlpx_remote_public_ekey(s.bob), g_alice_epub))) break;
        if ((check_q(rlpx_remote_public_ekey(s.alice), g_bob_epub))) break;
        test_session_deinit(&s);
        i++;
        tv++;
    }
    err = tv->auth ? -1 : 0; // broke loop early ? -> error
    return err;
}

int
test_write()
{
    int err;
    size_t l = 800; // ecies+pad
    uint8_t buffer[l];
    test_session s;
    test_session_init(&s, 0);

    IF_ERR_EXIT(rlpx_auth_write(s.alice, rlpx_public_skey(s.bob), buffer, &l));
    IF_ERR_EXIT(rlpx_auth_read(s.bob, buffer, l));

    l = 800;
    IF_ERR_EXIT(rlpx_ack_write(s.bob, rlpx_public_skey(s.alice), buffer, &l));
    IF_ERR_EXIT(rlpx_ack_read(s.alice, buffer, l));

    IF_ERR_EXIT(check_q(rlpx_remote_public_ekey(s.alice), g_bob_epub));
    IF_ERR_EXIT(check_q(rlpx_remote_public_ekey(s.bob), g_alice_epub));
EXIT:
    test_session_deinit(&s);
    return err;
}

int
test_secrets()
{
    int err;
    test_session s;
    test_session_init(&s, 0);
    uint8_t aes[32], mac[32], f[32];
    memcpy(aes, makebin(g_aes_secret, NULL), 32);
    memcpy(mac, makebin(g_mac_secret, NULL), 32);
    memcpy(f, makebin(g_foo, NULL), 32);

    rlpx_auth_read(s.bob, s.auth, s.authlen);
    IF_ERR_EXIT(rlpx_expect_secrets(s.bob, 0, s.auth, s.authlen, s.ack,
                                    s.acklen, aes, mac, f));
EXIT:
    test_session_deinit(&s);
    return err;
}

//
//
//
