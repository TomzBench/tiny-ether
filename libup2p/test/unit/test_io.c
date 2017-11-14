#include "test.h"

int
test_io()
{
    uint8_t txme[100];           // Transmitting tx
    uecc_ctx keys[2];            // alice/bob keys
    uint32_t listen[2];          // alice bob receive ports
    rlpx_io io[2];               // alice bob
    memset(txme, 'A', 100);      // init vector
    uecc_key_init_new(&keys[0]); // alice key
    uecc_key_init_new(&keys[1]); // bob key
    listen[0] = 20202;           // alice port
    listen[1] = 20203;           // bob port
    rlpx_io_udp_init(&io[0], &keys[0], &listen[0]);
    rlpx_io_udp_init(&io[1], &keys[1], &listen[1]);

    // Cleanup
    uecc_key_deinit(&keys[0]);
    uecc_key_deinit(&keys[1]);
    rlpx_io_deinit(&io[0]);
    rlpx_io_deinit(&io[1]);
    return 0;
}
