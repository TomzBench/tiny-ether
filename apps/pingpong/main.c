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

#include "ueth.h"
#include "usys_log.h"
#include "usys_signals.h"
#include "usys_time.h"

#define SKEY "5e173f6ac3c669587538e7727cf19b782a4f2fda07c1eaa662c593e5e85e3051"
#define REMOTE                                                                 \
    "ca634cae0d49acb401d8a4c6b6fe8c55b70d115bf400769cc1400f3258cd31387574077f" \
    "301b421bc84df7266c44e9e6d569fc56be00812904767bf5ccd1fc7f"

#define TEST_ENODE_3                                                           \
    "enode://"                                                                 \
    "8c441faab26ed52d0b2ca9755ea042e9e1c153c0fc47ddcadcd55551bae5592a631fefd4" \
    "13270bd6ac41a240f752f7a5c58edd1a8daddd614ea31b6099093358@45.55.63.72:"    \
    "30303"

// const char* g_test_enode = "enode://" REMOTE "@127.0.0.1:30303";

ueth_config config = { //
    .p2p_private_key = SKEY,
    .p2p_enable = 1,
    .udp = 22332
};

int main(int argc, char* argv[]);

int
main(int argc, char* argv[])
{
    ((void)argc);
    ((void)argv);
    int count = 0;
    ueth_context eth;
    rlpx_io_discovery_endpoint src, dst;
    memset(src.ip, 0, sizeof(src.ip));
    memset(dst.ip, 0, sizeof(dst.ip));
    src.iplen = dst.iplen = 4;
    src.tcp = src.udp = usys_htons(22332);
    dst.udp = dst.tcp = usys_htons(30303);

    // Log message
    usys_log_note("Running ping pong demo");

    // Install interrupt control
    usys_install_signal_handlers();

    // start
    ueth_init(&eth, &config);
    // ueth_start(&eth, 1, g_test_enode);
    ueth_start(&eth, 1, TEST_ENODE_3);

    while (usys_running()) {
        // Poll io
        usys_msleep(200);
        ueth_poll(&eth);
        if (count >= 40) {
            count = 0;
        }
        if (count++ == 0) {
            // TODO - make new rng for pubkeys

            // rlpx_io_discovery_send_ping(
            //    eth.discovery.protocols[0].context,
            //    usys_atoh("127.0.0.1"),
            //    30303,
            //    &src,
            //    &dst,
            //    usys_now() + 2);

            rlpx_io_discovery_send_find(
                eth.discovery.protocols[0].context,
                usys_atoh("13.84.180.240"),
                30303,
                // usys_atoh("52.169.14.227"),
                // 30303,
                // usys_atoh("45.55.63.72"),
                // 30303,
                // usys_atoh("127.0.0.1"),
                // 30303,
                NULL,
                usys_now() + 2);

            // rlpx_io_discovery_send_ping(
            //    eth.discovery.protocols[0].context,
            //    usys_atoh("13.84.180.240"),
            //    30303,
            //    // usys_atoh("52.169.14.227"),
            //    // 30303,
            //    // usys_atoh("45.55.63.72"),
            //    // 30303,
            //    &src,
            //    &dst,
            //    usys_now() + 2);
        }
    }

    // Notify remotes of shutdown and clean
    ueth_stop(&eth);
    ueth_deinit(&eth);
    return 0;
}
