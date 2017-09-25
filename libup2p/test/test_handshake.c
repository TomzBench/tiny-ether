#include "test.h"

extern test_vector g_test_vectors[];
extern const char* g_alice_spri;
extern const char* g_alice_epri;
extern const char* g_bob_spri;
extern const char* g_bob_epri;
extern const char* g_alice_epub;
extern const char* g_bob_epub;
extern const char* g_aes_secret;
extern const char* g_mac_secret;
extern const char* g_alice_nonce;
extern const char* g_bob_nonce;

// Non-public prototypes methods for test
void rlpx_test_nonce(rlpx* s, h256* nonce);
uint8_t* rlpx_aes_secret_debug_export(rlpx*);
uint8_t* rlpx_mac_secret_debug_export(rlpx*);

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
    uecc_private_key alice_e, alice_s, bob_s, bob_e;
    h256 alice_nonce, bob_nonce;
    test_vector* tv = g_test_vectors;
    rlpx *alice, *bob;
    memcpy(alice_e.b, makebin(g_alice_epri, NULL), 32);
    memcpy(alice_s.b, makebin(g_alice_spri, NULL), 32);
    memcpy(bob_e.b, makebin(g_bob_epri, NULL), 32);
    memcpy(bob_s.b, makebin(g_bob_spri, NULL), 32);
    memcpy(alice_nonce.b, makebin(g_alice_nonce, NULL), 32);
    memcpy(bob_nonce.b, makebin(g_bob_nonce, NULL), 32);
    alice = rlpx_alloc_keypair(&alice_s, &alice_e);
    bob = rlpx_alloc_keypair(&bob_s, &bob_e);
    rlpx_test_nonce(alice, &alice_nonce);
    rlpx_test_nonce(bob, &bob_nonce);
    if ((check_q(rlpx_public_ekey(alice), g_alice_epub))) return -1;
    if ((check_q(rlpx_public_ekey(bob), g_bob_epub))) return -1;

    while (tv->auth) {
        size_t authlen = strlen(tv->auth) / 2;
        size_t acklen = strlen(tv->ack) / 2;
        uint8_t auth[authlen];
        uint8_t ack[acklen];
        memcpy(auth, makebin(tv->auth, NULL), authlen);
        memcpy(ack, makebin(tv->ack, NULL), acklen);
        if (rlpx_auth_read(bob, auth, authlen)) break;
        if (rlpx_ack_read(alice, ack, acklen)) break;
        if (!(rlpx_version_remote(bob) == tv->authver)) break;
        if (!(rlpx_version_remote(alice) == tv->ackver)) break;
        if ((check_q(rlpx_remote_public_ekey(bob), g_alice_epub))) break;
        if ((check_q(rlpx_remote_public_ekey(alice), g_bob_epub))) break;
        tv++;
    }
    err = tv->auth ? -1 : 0; // broke loop early ? -> error
    rlpx_free(&alice);
    rlpx_free(&bob);
    return err;
}

int
test_write()
{
    int err;
    size_t l = 800; // ecies+pad
    uint8_t buffer[l];
    rlpx *alice, *bob;
    uecc_private_key alice_e, alice_s, bob_s, bob_e;
    h256 alice_nonce, bob_nonce;
    memcpy(alice_e.b, makebin(g_alice_epri, NULL), 32);
    memcpy(alice_s.b, makebin(g_alice_spri, NULL), 32);
    memcpy(bob_e.b, makebin(g_bob_epri, NULL), 32);
    memcpy(bob_s.b, makebin(g_bob_spri, NULL), 32);
    memcpy(alice_nonce.b, makebin(g_alice_nonce, NULL), 32);
    memcpy(bob_nonce.b, makebin(g_bob_nonce, NULL), 32);
    alice = rlpx_alloc_keypair(&alice_s, &alice_e);
    bob = rlpx_alloc_keypair(&bob_s, &bob_e);
    rlpx_test_nonce(alice, &alice_nonce);
    rlpx_test_nonce(bob, &bob_nonce);
    if ((check_q(rlpx_public_ekey(alice), g_alice_epub))) return -1;
    if ((check_q(rlpx_public_ekey(bob), g_bob_epub))) return -1;

    err = rlpx_auth_write(alice, rlpx_public_skey(bob), buffer, &l);
    if (err) goto EXIT;
    err = rlpx_auth_read(bob, buffer, l);
    if (err) goto EXIT;

    l = 800;
    err = rlpx_ack_write(bob, rlpx_public_skey(alice), buffer, &l);
    if (err) goto EXIT;
    err = rlpx_ack_read(alice, buffer, l);
    if (err) goto EXIT;

    if ((err = check_q(rlpx_remote_public_ekey(alice), g_bob_epub))) goto EXIT;
    if ((err = check_q(rlpx_remote_public_ekey(bob), g_alice_epub))) goto EXIT;

    err = 0;
EXIT:
    rlpx_free(&alice);
    rlpx_free(&bob);
    return err;
}

int
test_secrets()
{
    int err;
    rlpx* bob;
    uecc_private_key bob_s, bob_e;
    h256 bob_nonce;
    uint8_t aes[32], mac[32];
    size_t authlen = strlen(g_test_vectors[0].auth) / 2;
    uint8_t auth[authlen];
    memcpy(aes, makebin(g_aes_secret, NULL), 32);
    memcpy(mac, makebin(g_mac_secret, NULL), 32);
    memcpy(bob_e.b, makebin(g_bob_epri, NULL), 32);
    memcpy(bob_s.b, makebin(g_bob_spri, NULL), 32);
    memcpy(bob_nonce.b, makebin(g_bob_nonce, NULL), 32);
    memcpy(auth, makebin(g_test_vectors[0].auth, NULL), authlen);
    bob = rlpx_alloc_keypair(&bob_s, &bob_e);
    rlpx_test_nonce(bob, &bob_nonce);
    rlpx_auth_read(bob, auth, authlen);

    // Check secret expects
    if ((err = memcmp(rlpx_aes_secret_debug_export(bob), aes, 32))) goto EXIT;
    if ((err = memcmp(rlpx_mac_secret_debug_export(bob), mac, 32))) goto EXIT;
EXIT:
    rlpx_free(&bob);
    return err;
}

//
//
//
