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
    test_session_connect(&s);
    test_session_handshake(&s);
    memcpy(aes, makebin(g_go_aes_secret, NULL), 32);
    memcpy(mac, makebin(g_go_mac_secret, NULL), 32);

    err = rlpx_test_expect_secrets(
        s.bob, //
        0,
        s.ack,
        s.acklen,
        s.auth,
        s.authlen,
        aes,
        mac,
        NULL);
    IF_ERR_EXIT(err);
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
    urlp *rlpa = NULL, *rlpb = NULL;
    const urlp *bodya, *bodyb;
    const char *mema, *memb;
    uint32_t numa, numb, lena = 1000, lenb = 1000;
    uint8_t buffa[lena], buffb[lenb];

    // Send keys with mocking a connection
    test_session_init(&s, 1);
    test_session_connect(&s);
    test_session_handshake(&s);

    // Check key exchange
    IF_ERR_EXIT(check_q(rlpx_io_epub_remote(s.alice), g_bob_epub));
    IF_ERR_EXIT(check_q(rlpx_io_epub_remote(s.bob), g_alice_epub));

    // Reset length after write
    lena = sizeof(buffa);
    lenb = sizeof(buffb);

    // alice hello bob
    err = rlpx_io_devp2p_write_hello(
        &s.alice->x, *s.alice->listen_port, &s.alice->node_id[1], buffa, &lena);
    IF_ERR_EXIT(err);

    // bob hello alice
    err = rlpx_io_devp2p_write_hello(
        &s.bob->x, //
        *s.bob->listen_port,
        &s.bob->node_id[1],
        buffb,
        &lenb);
    IF_ERR_EXIT(err);

    // Parse hello (parse returns length processed).
    err = rlpx_frame_parse(&s.bob->x, buffa, lena, &rlpa) ? 0 : -1;
    err = rlpx_frame_parse(&s.alice->x, buffb, lenb, &rlpb) ? 0 : -1;

    // Read body frame
    bodya = urlp_at(urlp_at(rlpa, 1), 1); //
    bodyb = urlp_at(urlp_at(rlpb, 1), 1); //

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
