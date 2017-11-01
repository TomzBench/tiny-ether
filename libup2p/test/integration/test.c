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

#include "rlpx_io_devp2p.h"
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
    rlpx_io_devp2p alice;
    rlpx_io* alice_io = (rlpx_io*)&alice;
    rlpx_io_devp2p_init(&alice, &static_key, &udp, NULL, NULL);

    // Install interrupt control
    usys_install_signal_handlers();

    // Try and connect on start
    rlpx_io_nonce(alice_io);
    rlpx_io_connect_enode(alice_io, g_test_enode);

    // Enter while 1 loop.
    while (usys_running()) {
        if (!rlpx_io_is_shutdown(alice_io)) {
            usys_msleep(rlpx_io_is_connected(alice_io) ? 100 : 5000);
        }

        // Need connect?
        if (!rlpx_io_is_connected(alice_io)) {
            if (has_connected) {
                usys_shutdown();
            } else {
                rlpx_io_nonce(alice_io);
                rlpx_io_connect_enode(alice_io, g_test_enode);
            }
        } else {
            has_connected = rlpx_io_is_ready(alice_io) ? 1 : 0;
            // send ping every 2s
            if (rlpx_io_is_ready(alice_io) && (++c >= 10)) {
                rlpx_io_send_ping(&alice);
                c = 0;
            }

            // Received a pong? send disconnect
            if (alice.latency) {
                err = 0;
                rlpx_io_send_disconnect(&alice, DEVP2P_DISCONNECT_QUITTING);
            }
        }

        // Poll io
        rlpx_io_poll(&alice_io, 1, 100);
    }

    if (!err) {
        usys_log_ok("%s", "[ OK]");
    } else {
        usys_log_err("%s", "[ERR]");
    }

    rlpx_io_devp2p_deinit(&alice);
    uecc_key_deinit(&static_key);
    return err;
}
