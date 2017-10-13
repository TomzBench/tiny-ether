#include "test.h"

extern test_vector g_test_vectors[];
extern const char* g_alice_epub;
extern const char* g_bob_epub;
extern const char* g_aes_secret;
extern const char* g_mac_secret;
extern const char* g_foo;

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
    uecc_public_key *qa, *qb;

    int i = 0;
    while (tv->auth) {
        test_session_init(&s, i);
        qa = &s.alice->skey.Q;
        qb = &s.bob->skey.Q;
        rlpx_test_remote_ekey_clr(s.alice);
        rlpx_test_remote_ekey_clr(s.bob);
        if (rlpx_ch_recv_auth(s.bob, qa, s.auth, s.authlen)) break;
        if (rlpx_ch_recv_ack(s.alice, qb, s.ack, s.acklen)) break;
        if (!(s.bob->remote_version == tv->authver)) break;
        if (!(s.alice->remote_version == tv->ackver)) break;
        if ((cmp_q(&s.bob->remote_ekey, &s.alice->ekey.Q))) break;
        if ((cmp_q(&s.alice->remote_ekey, &s.bob->ekey.Q))) break;
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
    test_session s;
    test_session_init(&s, 0);
    uecc_public_key *qa, *qb;
    qa = &s.alice->skey.Q;
    qb = &s.bob->skey.Q;

    // Trade keys
    IF_ERR_EXIT(rlpx_ch_send_auth(s.alice, qb));
    IF_ERR_EXIT(rlpx_ch_recv_auth(s.bob, qa, s.alice->io.b, s.alice->io.len));
    IF_ERR_EXIT(rlpx_ch_send_ack(s.bob, qa));
    IF_ERR_EXIT(rlpx_ch_recv_ack(s.alice, qb, s.bob->io.b, s.bob->io.len));

    IF_ERR_EXIT(check_q(&s.alice->remote_ekey, g_bob_epub));
    IF_ERR_EXIT(check_q(&s.bob->remote_ekey, g_alice_epub));
EXIT:
    test_session_deinit(&s);
    return err;
}

int
test_secrets()
{
    int err;
    uint8_t aes[32], mac[32], foo[32];
    uecc_public_key *qa, *qb;
    test_session s;

    test_session_init(&s, 1);
    qa = &s.alice->skey.Q;
    qb = &s.bob->skey.Q;
    memcpy(aes, makebin(g_aes_secret, NULL), 32);
    memcpy(mac, makebin(g_mac_secret, NULL), 32);
    memcpy(foo, makebin(g_foo, NULL), 32);

    // Set some phoney nonces to read expected secrets
    rlpx_test_nonce_set(s.bob, &s.bob_n);
    rlpx_test_nonce_set(s.alice, &s.alice_n);
    rlpx_test_remote_nonce_set(s.bob, &s.alice_n);
    rlpx_test_remote_nonce_set(s.alice, &s.bob_n);

    rlpx_ch_recv_auth(s.bob, qa, s.auth, s.authlen);
    rlpx_ch_recv_ack(s.alice, qb, s.ack, s.acklen);
    IF_ERR_EXIT(rlpx_test_expect_secrets(s.bob, 0, s.ack, s.acklen, s.auth,
                                         s.authlen, aes, mac, foo));
    IF_ERR_EXIT(rlpx_test_expect_secrets(s.alice, 1, s.auth, s.authlen, s.ack,
                                         s.acklen, aes, mac, foo));
EXIT:
    test_session_deinit(&s);
    return err;
}

//
//
//
