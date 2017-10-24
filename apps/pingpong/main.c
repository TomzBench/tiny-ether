#include "rlpx_channel.h"
#include "usys_log.h"
#include "usys_signals.h"
#include "usys_time.h"

const char* g_test_enode = "enode://"
                           "ca634cae0d49acb401d8a4c6b6fe8c55b70d115bf400769cc14"
                           "00f3258cd31387574077f301b421bc84df7266c44e9e6d569fc"
                           "56be00812904767bf5ccd1fc7f@127.0.0.1:30303.0";

int main(int argc, char* argv[]);

int
main(int argc, char* argv[])
{
    ((void)argc);
    ((void)argv);
    int c = 0;
    rlpx_channel* alice = rlpx_ch_alloc(NULL, NULL);

    // Log message
    usys_log_note("Running ping pong demo");

    // Install interrupt control
    usys_install_signal_handlers();

    // Try and connect on start.
    rlpx_ch_nonce(alice);
    rlpx_ch_connect_enode(alice, g_test_enode);

    while (usys_running()) {
        usys_msleep(rlpx_ch_is_connected(alice) ? 100 : 5000);

        // TODO - fix connect api to handle nonce.
        if (!rlpx_ch_is_connected(alice)) {
            rlpx_ch_nonce(alice);
            rlpx_ch_connect_enode(alice, g_test_enode);
        }

        if (rlpx_ch_is_ready(alice) && ++c >= 100) {
            rlpx_ch_send_ping(alice);
            c = 0;
        }

        rlpx_ch_poll(&alice, 1, 100);
    }

    // Send disconnect to peer signal, wait at most 5 seconds and quit.
    if (rlpx_ch_is_connected(alice)) {
        c = 0;
        usys_log_note("Disconnecting...");
        rlpx_ch_send_disconnect(alice, DEVP2P_DISCONNECT_QUITTING);
        while ((!rlpx_ch_is_shutdown(alice)) && (++c < 50)) {
            usys_msleep(100);
            rlpx_ch_poll(&alice, 1, 100);
        }
    }

    rlpx_ch_free(&alice);
    return 0;
}
