#include "test.h"

#define NUM_PEERS 20

int
test_discovery_table()
{
    rlpx_io peers[NUM_PEERS];
    uecc_ctx keys[NUM_PEERS];
    uint32_t ports[NUM_PEERS];
    uint32_t port = 40404;
    for (uint32_t i = 0; i < NUM_PEERS; i++) {
        ports[i] = port++;
        uecc_key_init_new(&keys[i]);
        rlpx_io_udp_init(&peers[i], &keys[i], &ports[i]);
    }

    // TODO - add/remove nodes

    for (uint32_t i = 0; i < NUM_PEERS; i++) {
        uecc_key_deinit(&keys[i]);
        rlpx_io_deinit(&peers[i]);
    }
    return -1;
}
