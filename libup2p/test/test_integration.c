#include "test.h"
#include "usys_signals.h"
#include "usys_time.h"

const char* g_test_enode = "enode://" BOB_SPUB "@127.0.0.1:30303.0";

int
test_integration()
{
    int err = 0;
    uecc_private_key alice_e, alice_s;
    rlpx_channel* alice = rlpx_ch_alloc(&alice_s, &alice_e);

    // Install interrupt control
    usys_install_signal_handlers();

    // Connect to integration test node.
    // TODO - start test node if not running?
    rlpx_ch_connect_enode(alice, g_test_enode);

    // Enter while 1 loop.
    while (usys_running()) {
        usys_msleep(100);
        rlpx_ch_poll(&alice, 1, 100);
    }

    rlpx_ch_free(&alice);
    return err;
}
