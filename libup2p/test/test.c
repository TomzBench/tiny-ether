#include "test.h"
#include "rlpx.h"
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
const char* g_nonce_a = NONCE_A;
const char* g_nonce_b = NONCE_B;

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
