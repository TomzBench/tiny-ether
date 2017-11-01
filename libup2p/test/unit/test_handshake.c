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
    IF_ERR_EXIT(test_read());
    IF_ERR_EXIT(test_write());
    IF_ERR_EXIT(test_secrets());

EXIT:
    return err;
}

int
test_read()
{
    int err;
    test_session s;
    test_vector* tv = g_test_vectors;
    rlpx_io *a, *b;

    int i = 0;
    while (tv->auth) {
        test_session_init(&s, i);
        a = (rlpx_io*)s.alice;
        b = (rlpx_io*)s.bob;
        rlpx_io_nonce(a);
        rlpx_io_nonce(b);
        rlpx_io_connect(a, &b->skey->Q, "1.1.1.1", 33);
        rlpx_io_accept(b, &a->skey->Q);
        if (rlpx_io_recv_auth(b, s.auth, s.authlen)) break;
        if (rlpx_io_recv_ack(a, s.ack, s.acklen)) break;
        if (!(b->hs->version_remote == tv->authver)) break;
        if (!(a->hs->version_remote == tv->ackver)) break;
        if ((cmp_q(&b->hs->ekey_remote, &a->ekey.Q))) break;
        if ((cmp_q(&a->hs->ekey_remote, &b->ekey.Q))) break;
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
    test_session_init(&s, 1);
    rlpx_io *a = (rlpx_io *)s.alice, *b = (rlpx_io *)s.bob;

    // Trade keys
    rlpx_io_nonce(a);
    rlpx_io_nonce(b);
    rlpx_io_connect(a, &b->skey->Q, "1.1.1.1", 33);
    rlpx_io_accept(b, &a->skey->Q);
    IF_ERR_EXIT(rlpx_io_recv_auth(b, a->io.b, a->io.len));
    IF_ERR_EXIT(rlpx_io_recv_ack(a, b->io.b, b->io.len));

    IF_ERR_EXIT(check_q(&a->hs->ekey_remote, g_bob_epub));
    IF_ERR_EXIT(check_q(&b->hs->ekey_remote, g_alice_epub));
EXIT:
    test_session_deinit(&s);
    return err;
}

int
test_secrets()
{
    int err;
    uint8_t aes[32], mac[32], foo[32];
    test_session s;
    rlpx_io *a, *b;

    test_session_init(&s, 1);
    a = (rlpx_io*)s.alice;
    b = (rlpx_io*)s.bob;
    memcpy(aes, makebin(g_aes_secret, NULL), 32);
    memcpy(mac, makebin(g_mac_secret, NULL), 32);
    memcpy(foo, makebin(g_foo, NULL), 32);

    // Set some phoney nonces to read expected secrets
    rlpx_test_nonce_set(s.bob, &s.bob_n);
    rlpx_test_nonce_set(s.alice, &s.alice_n);

    rlpx_io_connect(a, &b->skey->Q, "1.1.1.1", 33);
    rlpx_io_accept(b, &a->skey->Q);
    rlpx_io_recv_auth(b, s.auth, s.authlen);
    rlpx_io_recv_ack(a, s.ack, s.acklen);
    IF_ERR_EXIT(
        rlpx_test_expect_secrets(
            s.bob, 0, s.ack, s.acklen, s.auth, s.authlen, aes, mac, foo));
    IF_ERR_EXIT(
        rlpx_test_expect_secrets(
            s.alice, 1, s.auth, s.authlen, s.ack, s.acklen, aes, mac, foo));
EXIT:
    test_session_deinit(&s);
    return err;
}

//
//
//
