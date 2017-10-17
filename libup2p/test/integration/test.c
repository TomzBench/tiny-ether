#include "rlpx_channel.h"
#include "usys_signals.h"
#include "usys_time.h"

const char* g_test_enode = "enode://"
                           "ca634cae0d49acb401d8a4c6b6fe8c55b70d115bf400769cc14"
                           "00f3258cd31387574077f301b421bc84df7266c44e9e6d569fc"
                           "56be00812904767bf5ccd1fc7f@127.0.0.1:30303.0";

int
main(int argc, char* arg[])
{
    ((void)argc);
    ((void)arg);
    int err = 0, c = 0;
    rlpx_channel* alice = rlpx_ch_alloc(NULL, NULL);

    // Install interrupt control
    usys_install_signal_handlers();

    // Connect to integration test node.
    // TODO - start test node if not running?
    rlpx_ch_nonce(alice);
    rlpx_ch_connect_enode(alice, g_test_enode);

    // Enter while 1 loop.
    while (usys_running()) {
        usys_msleep(200);
        rlpx_ch_poll(&alice, 1, 100);
    }

    rlpx_ch_free(&alice);
    return err;
}
