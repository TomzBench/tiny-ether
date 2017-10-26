#include "rlpx_channel.h"
#include "test_enodes.h"
#include "usys_log.h"
#include "usys_signals.h"
#include "usys_time.h"

const char* g_test_enode = PYDEV_P2P_LOCAL;

/**
 * @brief 1) Connect and authenticate to a remote endpoint.
 * Ping/Pong/Disconnect
 *
 * @param argc not used
 * @param arg[] not used
 *
 * @return -1 err, 0 ok
 */
int
main(int argc, char* arg[])
{
    ((void)argc);
    ((void)arg);
    int err = -1, c = 0, has_connected = 0;
    uint32_t udp = 33433;
    uecc_ctx static_key;
    uecc_key_init_new(&static_key);
    rlpx_channel* alice = rlpx_ch_alloc(&static_key, &udp);

    // Install interrupt control
    usys_install_signal_handlers();

    // Try and connect on start
    rlpx_ch_nonce(alice);
    rlpx_ch_connect_enode(alice, g_test_enode);

    // Enter while 1 loop.
    while (usys_running()) {
        if (!rlpx_ch_is_shutdown(alice)) {
            usys_msleep(rlpx_ch_is_connected(alice) ? 100 : 5000);
        }

        // Need connect?
        if (!rlpx_ch_is_connected(alice)) {
            if (has_connected) {
                usys_shutdown();
            } else {
                rlpx_ch_nonce(alice);
                rlpx_ch_connect_enode(alice, g_test_enode);
            }
        } else {
            has_connected = rlpx_ch_is_ready(alice) ? 1 : 0;
            // send ping every 2s
            if (rlpx_ch_is_ready(alice) && (++c >= 10)) {
                rlpx_ch_send_ping(alice);
                c = 0;
            }

            // Received a pong? send disconnect
            if (alice->devp2p.latency) {
                err = 0;
                rlpx_ch_send_disconnect(alice, DEVP2P_DISCONNECT_QUITTING);
            }
        }

        // Poll io
        rlpx_ch_poll(&alice, 1, 100);
    }

    if (!err) {
        usys_log_ok("%s", "[ OK]");
    } else {
        usys_log_err("%s", "[ERR]");
    }

    rlpx_ch_free(&alice);
    uecc_key_deinit(&static_key);
    return err;
}
