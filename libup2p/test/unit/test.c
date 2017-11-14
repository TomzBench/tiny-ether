// Copyright 2017 Altronix Corp.
// This file is part of the tiny-ether library
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

/**
 * @author Thomas Chiantia <thomas@altronix>
 * @date 2017
 */

#include "test.h"
#include <string.h>

extern async_io_mock_settings g_io_mock_settings;

test_vector g_test_vectors[] = { //
    {.auth = AUTH_1,
     .ack = ACK_1,
     .alice_s = ALICE_SPRI,
     .alice_e = ALICE_EPRI,
     .alice_n = NONCE_A,
     .bob_s = BOB_SPRI,
     .bob_e = BOB_EPRI,
     .bob_n = NONCE_B,
     .authver = AUTHVER_1,
     .ackver = ACKVER_1 },
    {.auth = AUTH_2,
     .ack = ACK_2,
     .alice_s = ALICE_SPRI,
     .alice_e = ALICE_EPRI,
     .alice_n = NONCE_A,
     .bob_s = BOB_SPRI,
     .bob_e = BOB_EPRI,
     .bob_n = NONCE_B,
     .authver = AUTHVER_2,
     .ackver = ACKVER_2 },
    {.auth = AUTH_3, //
     .ack = ACK_3,
     .alice_s = ALICE_SPRI,
     .alice_e = ALICE_EPRI,
     .alice_n = NONCE_A,
     .bob_s = BOB_SPRI,
     .bob_e = BOB_EPRI,
     .bob_n = NONCE_B,
     .authver = AUTHVER_3,
     .ackver = ACKVER_3 },
    {.auth = AUTH_GO,
     .ack = ACK_GO,
     .alice_s = ALICE_SPRI_GO,
     .alice_e = ALICE_EPRI_GO,
     .alice_n = ALICE_NONCE_GO,
     .bob_s = BOB_SPRI_GO,
     .bob_e = BOB_EPRI_GO,
     .bob_n = BOB_NONCE_GO,
     .authver = AUTHVER_1,
     .ackver = ACKVER_1 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};
const char* g_alice_spri = ALICE_SPRI;
const char* g_alice_spub = ALICE_SPUB;
const char* g_alice_epri = ALICE_EPRI;
const char* g_alice_epub = ALICE_EPUB;
const char* g_bob_spri = BOB_SPRI;
const char* g_bob_spub = BOB_SPUB;
const char* g_bob_epri = BOB_EPRI;
const char* g_bob_epub = BOB_EPUB;
const char* g_aes_secret = AES_SECRET;
const char* g_mac_secret = MAC_SECRET;
const char* g_foo = FOO_SECRET;
const char* g_go_aes_secret = GO_AES_SECRET;
const char* g_go_mac_secret = GO_MAC_SECRET;
const char* g_go_foo = GO_FOO_SECRET;
const char* g_alice_nonce = NONCE_A;
const char* g_bob_nonce = NONCE_B;
const char* g_hello_packet = HELLO_PACKET;

int
main(int argc, char* argv[])
{
    ((void)argc);
    ((void)argv);
    int err = 0;

    IF_ERR_EXIT(test_handshake());
    IF_ERR_EXIT(test_frame());
    IF_ERR_EXIT(test_protocol());
    IF_ERR_EXIT(test_enode());
    IF_ERR_EXIT(test_discovery());
    IF_ERR_EXIT(test_io());
// IF_ERR_EXIT(test_discovery_table());

EXIT:
    if (!err) {
        usys_log_ok("%s", "[ OK]");
    } else {
        usys_log_err("%s", "[ERR]");
    }
    return err;
}

int
test_session_init(test_session* s, int vec)
{
    // buffers for keys, nonces, cipher text, etc
    uecc_private_key alice_s, alice_e, bob_s, bob_e;
    uecc_ctx ekey_a, ekey_b;

    memset(s, 0, sizeof(test_session));
    memcpy(alice_s.b, makebin(g_test_vectors[vec].alice_s, NULL), 32);
    memcpy(bob_s.b, makebin(g_test_vectors[vec].bob_s, NULL), 32);
    memcpy(alice_e.b, makebin(g_test_vectors[vec].alice_e, NULL), 32);
    memcpy(bob_e.b, makebin(g_test_vectors[vec].bob_e, NULL), 32);
    s->authlen = strlen(g_test_vectors[vec].auth) / 2;
    s->acklen = strlen(g_test_vectors[vec].ack) / 2;
    memcpy(s->auth, makebin(g_test_vectors[vec].auth, NULL), s->authlen);
    memcpy(s->ack, makebin(g_test_vectors[vec].ack, NULL), s->acklen);
    memcpy(s->alice_n.b, makebin(g_test_vectors[vec].alice_n, NULL), 32);
    memcpy(s->bob_n.b, makebin(g_test_vectors[vec].bob_n, NULL), 32);
    s->udp[0] = UDP_TEST_PORT;
    s->udp[1] = UDP_TEST_PORT + 1;

    // init test_session with alice,bob,etc
    uecc_key_init_binary(&s->skey_a, &alice_s);
    uecc_key_init_binary(&s->skey_b, &bob_s);
    uecc_key_init_binary(&ekey_a, &alice_e);
    uecc_key_init_binary(&ekey_b, &bob_e);
    s->alice = rlpx_io_alloc(&s->skey_a, &s->udp[0]);
    s->bob = rlpx_io_alloc(&s->skey_b, &s->udp[1]);
    async_io_install_mock(&s->alice->io, &g_io_mock_settings);
    async_io_install_mock(&s->bob->io, &g_io_mock_settings);
    rlpx_io_devp2p_install(s->alice);
    rlpx_io_devp2p_install(s->bob);

    // Install mock ekeys
    rlpx_test_ekey_set(s->alice, &ekey_a);
    rlpx_test_ekey_set(s->bob, &ekey_b);

    // sanity check
    if ((check_q(rlpx_io_epub(s->alice), g_alice_epub))) return -1;
    if ((check_q(rlpx_io_epub(s->bob), g_bob_epub))) return -1;
    if ((check_q(rlpx_io_spub(s->alice), g_alice_spub))) return -1;
    if ((check_q(rlpx_io_spub(s->bob), g_bob_spub))) return -1;
    return 0;
}

void
test_session_deinit(test_session* s)
{
    rlpx_io_free(&s->alice);
    rlpx_io_free(&s->bob);
    uecc_key_deinit(&s->skey_a);
    uecc_key_deinit(&s->skey_b);
}

void
test_session_connect(test_session* s)
{
    // Prepare state to look like connections were made

    // Set nonce
    rlpx_test_nonce_set(s->alice, &s->alice_n);
    rlpx_test_nonce_set(s->bob, &s->bob_n);

    // fill remote node info
    rlpx_node_init(&s->alice->node, rlpx_io_spub(s->bob), "1.1.1.1", 0, 0);
    rlpx_node_init(&s->bob->node, rlpx_io_spub(s->alice), "1.1.1.1", 0, 0);

    s->alice->hs = rlpx_handshake_alloc(
        1,
        s->alice->skey,
        &s->alice->ekey,
        &s->alice->nonce,
        &s->alice->node.id);
    s->bob->hs = rlpx_handshake_alloc(
        0, //
        s->bob->skey,
        &s->bob->ekey,
        &s->bob->nonce,
        &s->bob->node.id);
}

void
test_session_handshake(test_session* s)
{
    // Do handshakes

    // rlpx_io_recv_ack(s->alice, s->bob->hs->cipher, s->bob->hs->cipher_len);
    // rlpx_io_recv_auth(s->bob, s->alice->hs->cipher,
    // s->alice->hs->cipher_len);
    rlpx_io_recv_ack(
        s->alice, //
        s->bob->hs->cipher,
        s->bob->hs->cipher_len);
    rlpx_io_recv_auth(
        s->bob, //
        s->alice->hs->cipher,
        s->alice->hs->cipher_len);
}

int
cmp_q(const uecc_public_key* a, const uecc_public_key* b)
{
    uint8_t puba[65];
    uint8_t pubb[65];
    uecc_qtob(a, puba, 65);
    uecc_qtob(b, pubb, 65);
    return memcmp(puba, pubb, 65) ? -1 : 0;
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
    static uint8_t buf[1024];
    size_t s;
    if (!len) len = &s;
    *len = strlen(str) / 2;
    if (*len > 1024) *len = 1024;
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
