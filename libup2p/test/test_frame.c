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
    test_session_init(&s, TEST_VECTOR_LEGACY_GO);
    uint8_t aes[32], mac[32];
    urlp* frame = NULL;
    const urlp* seek;
    uint32_t p2pver;
    memcpy(aes, makebin(g_go_aes_secret, NULL), 32);
    memcpy(mac, makebin(g_go_mac_secret, NULL), 32);

    // Set some phoney nonces
    rlpx_test_nonce_set(s.bob, &s.bob_n);
    rlpx_test_nonce_set(s.alice, &s.alice_n);
    rlpx_test_remote_nonce_set(s.bob, &s.alice_n);
    rlpx_test_remote_nonce_set(s.alice, &s.bob_n);

    // Update our secrets
    IF_ERR_EXIT(rlpx_ch_recv_auth(s.bob, &s.alice->skey.Q, s.auth, s.authlen));
    IF_ERR_EXIT(rlpx_test_expect_secrets(s.bob, 0, s.ack, s.acklen, s.auth,
                                         s.authlen, aes, mac, NULL));
    IF_ERR_EXIT(rlpx_frame_parse(&s.bob->x, makebin(g_hello_packet, NULL),
                                 strlen(g_hello_packet) / 2, &frame));
    seek = urlp_at(urlp_at(frame, 1), 1); // get body frame
    IF_ERR_EXIT(rlpx_devp2p_protocol_p2p_version(seek, &p2pver));
    IF_ERR_EXIT(p2pver == 3 ? 0 : -1);
    IF_ERR_EXIT(rlpx_devp2p_protocol_capabilities(seek, "a", 0));
    IF_ERR_EXIT(rlpx_devp2p_protocol_capabilities(seek, "b", 2));
    urlp_free(&frame);
EXIT:
    test_session_deinit(&s);
    return err;
}

int
test_frame_write()
{
    int err = 0;
    uecc_public_key *qa, *qb;
    test_session s;
    test_session_init(&s, 1);
    size_t lena = 1000, lenb = 1000, alen = 1000, blen = 1000;
    uint8_t from_alice[lena], from_bob[lenb], a[alen], b[blen];
    urlp *framea = NULL, *frameb = NULL;
    const urlp *bodya, *bodyb;
    const char *mema, *memb;
    uint32_t numa, numb;
    qa = &s.alice->skey.Q;
    qb = &s.bob->skey.Q;

    // Send keys
    IF_ERR_EXIT(rlpx_ch_send_auth(s.alice, qb));
    IF_ERR_EXIT(rlpx_ch_send_ack(s.bob, &s.alice->skey.Q));

    // Recv keys
    IF_ERR_EXIT(rlpx_ch_recv_ack(s.alice, qb, s.bob->io.b, s.bob->io.len));
    IF_ERR_EXIT(rlpx_ch_recv_auth(s.bob, qa, s.alice->io.b, s.alice->io.len));

    // Check key exchange
    IF_ERR_EXIT(check_q(&s.alice->remote_ekey, g_bob_epub));
    IF_ERR_EXIT(check_q(&s.bob->remote_ekey, g_alice_epub));

    // Update secrets
    IF_ERR_EXIT(rlpx_ch_secrets(s.bob, 0, b, blen, a, alen));
    IF_ERR_EXIT(rlpx_ch_secrets(s.alice, 1, a, alen, b, blen));

    // Write some packets
    IF_ERR_EXIT(rlpx_ch_write_hello(s.alice, from_alice, &lena));
    IF_ERR_EXIT(rlpx_ch_write_hello(s.bob, from_bob, &lenb));
    IF_ERR_EXIT(rlpx_frame_parse(&s.alice->x, from_bob, lenb, &frameb));
    IF_ERR_EXIT(rlpx_frame_parse(&s.bob->x, from_alice, lena, &framea));

    bodya = urlp_at(urlp_at(framea, 1), 1); // get body frame
    bodyb = urlp_at(urlp_at(frameb, 1), 1); // get body frame

    // Verify p2pver
    rlpx_devp2p_protocol_p2p_version(bodya, &numa);
    rlpx_devp2p_protocol_p2p_version(bodyb, &numb);
    IF_ERR_EXIT((numa == RLPX_VERSION_P2P) ? 0 : -1);
    IF_ERR_EXIT((numb == RLPX_VERSION_P2P) ? 0 : -1);

    // Verify client id read ok
    rlpx_devp2p_protocol_client_id(bodya, &mema, &numa);
    rlpx_devp2p_protocol_client_id(bodyb, &memb, &numb);
    IF_ERR_EXIT((numa == RLPX_CLIENT_ID_LEN) ? 0 : -1);
    IF_ERR_EXIT((numb == RLPX_CLIENT_ID_LEN) ? 0 : -1);
    IF_ERR_EXIT(memcmp(mema, RLPX_CLIENT_ID_STR, numa) ? -1 : 0);
    IF_ERR_EXIT(memcmp(memb, RLPX_CLIENT_ID_STR, numb) ? -1 : 0);

    // Verify capabilities read ok
    IF_ERR_EXIT(rlpx_devp2p_protocol_capabilities(bodya, "les", 2));
    IF_ERR_EXIT(rlpx_devp2p_protocol_capabilities(bodyb, "les", 2));

    // verify listen port
    rlpx_devp2p_protocol_listen_port(bodya, &numa);
    rlpx_devp2p_protocol_listen_port(bodyb, &numb);
    IF_ERR_EXIT((numa == s.alice->listen_port) ? 0 : -1);
    IF_ERR_EXIT((numb == s.bob->listen_port) ? 0 : -1);

    // verify node_id
    rlpx_devp2p_protocol_node_id(bodya, &mema, &numa);
    rlpx_devp2p_protocol_node_id(bodyb, &memb, &numb);
    IF_ERR_EXIT((numa == 65) ? 0 : -1);
    IF_ERR_EXIT((numb == 65) ? 0 : -1);
    IF_ERR_EXIT(memcmp(mema, s.alice->node_id, numa) ? -1 : 0);
    IF_ERR_EXIT(memcmp(memb, s.bob->node_id, numb) ? -1 : 0);

EXIT:
    // clean
    if (framea) urlp_free(&framea);
    if (frameb) urlp_free(&frameb);
    test_session_deinit(&s);
    return err;
}
