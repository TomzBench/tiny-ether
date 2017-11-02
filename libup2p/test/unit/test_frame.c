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
    uint8_t aes[32], mac[32];
    urlp* frame = NULL;
    const urlp* seek;
    uint32_t p2pver;

    test_session_init(&s, TEST_VECTOR_LEGACY_GO);
    memcpy(aes, makebin(g_go_aes_secret, NULL), 32);
    memcpy(mac, makebin(g_go_mac_secret, NULL), 32);

    // Set some phoney nonces
    rlpx_test_nonce_set(s.bob, &s.bob_n);
    rlpx_test_nonce_set(s.alice, &s.alice_n);

    // Update our secrets
    rlpx_io_connect(s.alice, &s.bob->skey->Q, "1.1.1.1", 33);
    rlpx_io_accept(s.bob, &s.alice->skey->Q);
    IF_ERR_EXIT(rlpx_io_recv_auth(s.bob, s.auth, s.authlen));
    IF_ERR_EXIT(
        rlpx_test_expect_secrets(
            s.bob, 0, s.ack, s.acklen, s.auth, s.authlen, aes, mac, NULL));
    if (!rlpx_frame_parse(
            &s.bob->x,
            makebin(g_hello_packet, NULL),
            strlen(g_hello_packet) / 2,
            &frame)) {
        goto EXIT;
    }
    seek = urlp_at(urlp_at(frame, 1), 1); // get body frame
    IF_ERR_EXIT(rlpx_io_devp2p_p2p_version(seek, &p2pver));
    IF_ERR_EXIT(p2pver == 3 ? 0 : -1);
    IF_ERR_EXIT(rlpx_io_devp2p_capabilities(seek, "a", 0));
    IF_ERR_EXIT(rlpx_io_devp2p_capabilities(seek, "b", 2));
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
    urlp *rlpa = NULL, *rlpb = NULL;
    const urlp *bodya, *bodyb;
    const char *mema, *memb;
    uint32_t numa, numb;

    // Send keys
    rlpx_io_nonce(s.alice);
    rlpx_io_nonce(s.bob);
    rlpx_io_connect(s.alice, &s.bob->skey->Q, "1.1.1.1", 33);
    rlpx_io_accept(s.bob, &s.alice->skey->Q);

    // Recv keys
    IF_ERR_EXIT(rlpx_io_recv_ack(s.alice, s.bob->io.b, s.bob->io.len));
    IF_ERR_EXIT(rlpx_io_recv_auth(s.bob, s.alice->io.b, s.alice->io.len));

    // Check key exchange
    IF_ERR_EXIT(check_q(&s.alice->hs->ekey_remote, g_bob_epub));
    IF_ERR_EXIT(check_q(&s.bob->hs->ekey_remote, g_alice_epub));

    // Write some packets
    IF_ERR_EXIT(rlpx_io_send_hello(s.alice->protocols[0].context));
    IF_ERR_EXIT(rlpx_io_send_hello(s.bob->protocols[0].context));
    if (!rlpx_frame_parse(&s.alice->x, s.bob->io.b, s.bob->io.len, &rlpb)) {
        goto EXIT;
    }
    if (!rlpx_frame_parse(&s.bob->x, s.alice->io.b, s.alice->io.len, &rlpa)) {
        goto EXIT;
    }

    bodya = urlp_at(urlp_at(rlpa, 1), 1); // get body frame
    bodyb = urlp_at(urlp_at(rlpb, 1), 1); // get body frame

    // Verify p2pver
    rlpx_io_devp2p_p2p_version(bodya, &numa);
    rlpx_io_devp2p_p2p_version(bodyb, &numb);
    IF_ERR_EXIT((numa == RLPX_VERSION_P2P) ? 0 : -1);
    IF_ERR_EXIT((numb == RLPX_VERSION_P2P) ? 0 : -1);

    // Verify client id read ok
    rlpx_io_devp2p_client_id(bodya, &mema, &numa);
    rlpx_io_devp2p_client_id(bodyb, &memb, &numb);
    IF_ERR_EXIT((numa == RLPX_CLIENT_ID_LEN) ? 0 : -1);
    IF_ERR_EXIT((numb == RLPX_CLIENT_ID_LEN) ? 0 : -1);
    IF_ERR_EXIT(memcmp(mema, RLPX_CLIENT_ID_STR, numa) ? -1 : 0);
    IF_ERR_EXIT(memcmp(memb, RLPX_CLIENT_ID_STR, numb) ? -1 : 0);

    // Verify capabilities read ok
    IF_ERR_EXIT(rlpx_io_devp2p_capabilities(bodya, "p2p", 4));
    IF_ERR_EXIT(rlpx_io_devp2p_capabilities(bodyb, "p2p", 4));

    // verify listen port
    rlpx_io_devp2p_listen_port(bodya, &numa);
    rlpx_io_devp2p_listen_port(bodyb, &numb);
    IF_ERR_EXIT((numa == *s.alice->listen_port) ? 0 : -1);
    IF_ERR_EXIT((numb == *s.bob->listen_port) ? 0 : -1);

    // verify node_id
    rlpx_io_devp2p_node_id(bodya, &mema, &numa);
    rlpx_io_devp2p_node_id(bodyb, &memb, &numb);
    IF_ERR_EXIT((numa == 64) ? 0 : -1);
    IF_ERR_EXIT((numb == 64) ? 0 : -1);
    IF_ERR_EXIT(memcmp(mema, &s.alice->node_id[1], numa) ? -1 : 0);
    IF_ERR_EXIT(memcmp(memb, &s.bob->node_id[1], numb) ? -1 : 0);

EXIT:
    // clean
    if (rlpa) urlp_free(&rlpa);
    if (rlpb) urlp_free(&rlpb);
    test_session_deinit(&s);
    return err;
}
