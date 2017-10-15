#include "test.h"

int test_hex();
int test_node();

int
test_enode()
{
    int err = 0;
    err |= test_hex();
    err |= test_node();
    return err;
}

int
test_hex()
{
    int err = 0;
    uint8_t raw[64], rawa[65], rawb[65];
    char mem[129], mema[129], memb[129];
    test_session s;
    test_session_init(&s, 1);
    IF_ERR_EXIT(rlpx_node_hex_to_bin(BOB_EPUB, 0, raw, NULL));
    IF_ERR_EXIT(rlpx_node_bin_to_hex(raw, 64, mem, NULL));
    IF_ERR_EXIT(memcmp(mem, BOB_EPUB, 128) ? -1 : 0);
    IF_ERR_EXIT(uecc_qtob(&s.alice->skey.Q, rawa, 65));
    IF_ERR_EXIT(uecc_qtob(&s.bob->skey.Q, rawb, 65));
    IF_ERR_EXIT(rlpx_node_bin_to_hex(&rawa[1], 64, mema, NULL));
    IF_ERR_EXIT(rlpx_node_bin_to_hex(&rawb[1], 64, memb, NULL));
    IF_ERR_EXIT(check_q(&s.alice->skey.Q, mema));
    IF_ERR_EXIT(check_q(&s.bob->skey.Q, memb));
EXIT:
    test_session_deinit(&s);
    return err;
}

int
test_node()
{
    int err = 0;
    char* alice = "enode://" ALICE_SPUB "@1.1.1.1:33.89";
    rlpx_node node;
    test_session s;

    test_session_init(&s, 1);
    IF_ERR_EXIT(rlpx_node_init_enode(&node, alice));
    IF_ERR_EXIT(cmp_q(&node.id, &s.alice->skey.Q));
    IF_ERR_EXIT((node.port_tcp == 33) ? 0 : -1);
    IF_ERR_EXIT((node.port_udp == 89) ? 0 : -1);

EXIT:
    test_session_deinit(&s);
    rlpx_node_deinit(&node);
    return err;
}
