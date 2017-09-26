#include "test.h"
#include "rlpx.h"
#include "rlpx_test_helpers.h"
#include <string.h>

test_vector g_test_vectors[] = {
    {.auth = AUTH_2, .ack = ACK_2, .authver = AUTHVER_2, .ackver = ACKVER_2 },
    {.auth = AUTH_3, .ack = ACK_3, .authver = AUTHVER_3, .ackver = ACKVER_3 },
    { 0, 0, 0, 0 }
};
const char* g_alice_spri = ALICE_SPRI;
const char* g_alice_epri = ALICE_EPRI;
const char* g_alice_epub = ALICE_EPUB;
const char* g_bob_spri = BOB_SPRI;
const char* g_bob_epri = BOB_EPRI;
const char* g_bob_epub = BOB_EPUB;
const char* g_aes_secret = AES_SECRET;
const char* g_mac_secret = MAC_SECRET;
const char* g_foo = FOO_SECRET;
const char* g_alice_nonce = NONCE_A;
const char* g_bob_nonce = NONCE_B;

int
main(int argc, char* argv[])
{
    ((void)argc);
    ((void)argv);
    int err = 0;

    err = test_handshake();
    return err;
}

int
test_session_init(test_session* s, int vec)
{
    // buffers for keys, nonces, cipher text, etc
    uecc_private_key alice_e, alice_s, bob_s, bob_e;
    memset(s, 0, sizeof(test_session));
    // read in test vectors
    s->authlen = strlen(g_test_vectors[vec].auth) / 2;
    s->acklen = strlen(g_test_vectors[vec].ack) / 2;
    memcpy(alice_e.b, makebin(g_alice_epri, NULL), 32);
    memcpy(alice_s.b, makebin(g_alice_spri, NULL), 32);
    memcpy(bob_e.b, makebin(g_bob_epri, NULL), 32);
    memcpy(bob_s.b, makebin(g_bob_spri, NULL), 32);
    memcpy(s->auth, makebin(g_test_vectors[vec].auth, NULL), s->authlen);
    memcpy(s->ack, makebin(g_test_vectors[vec].ack, NULL), s->acklen);
    memcpy(s->alice_n.b, makebin(g_alice_nonce, NULL), 32);
    memcpy(s->bob_n.b, makebin(g_bob_nonce, NULL), 32);
    // init test_session with alice,bob,etc
    s->alice = rlpx_alloc_keypair(&alice_s, &alice_e);
    s->bob = rlpx_alloc_keypair(&bob_s, &bob_e);
    // sanity check
    if ((check_q(rlpx_public_ekey(s->alice), g_alice_epub))) return -1;
    if ((check_q(rlpx_public_ekey(s->bob), g_bob_epub))) return -1;
    return 0;
}

void
test_session_deinit(test_session* s)
{
    rlpx_free(&s->alice);
    rlpx_free(&s->bob);
}

int
check_q(const uecc_public_key* key, const char* str)
{
    size_t l = strlen(str) / 2;
    uint8_t a[l];
    uint8_t b[65];
    if (!(l == 64)) return -1;
    memcpy(a, makebin(str, NULL), l);
    uecc_qtob(key, b, 65);
    return memcmp(a, &b[1], 64) ? -1 : 0;
}

const uint8_t*
makebin(const char* str, size_t* len)
{
    static uint8_t buf[512];
    size_t s;
    if (!len) len = &s;
    *len = strlen(str) / 2;
    if (*len > 512) *len = 512;
    for (size_t i = 0; i < *len; i++) {
        uint8_t c = 0;
        if (str[i * 2] >= '0' && str[i * 2] <= '9')
            c += (str[i * 2] - '0') << 4;
        if ((str[i * 2] & ~0x20) >= 'A' && (str[i * 2] & ~0x20) <= 'F')
            c += (10 + (str[i * 2] & ~0x20) - 'A') << 4;
        if (str[i * 2 + 1] >= '0' && str[i * 2 + 1] <= '9')
            c += (str[i * 2 + 1] - '0');
        if ((str[i * 2 + 1] & ~0x20) >= 'A' && (str[i * 2 + 1] & ~0x20) <= 'F')
            c += (10 + (str[i * 2 + 1] & ~0x20) - 'A');
        buf[i] = c;
    }
    return buf;
}

//
//
//
