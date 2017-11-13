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

#define PYDEV_P2P_LOCAL                                                        \
    "enode://"                                                                 \
    "ca634cae0d49acb401d8a4c6b6fe8c55b70d115bf400769cc1400f3258cd31387574077f" \
    "301b421bc84df7266c44e9e6d569fc56be00812904767bf5ccd1fc7f@127.0.0.1:"      \
    "30303.30303"

#define TEST_ENODE_3                                                           \
    "enode://"                                                                 \
    "8c441faab26ed52d0b2ca9755ea042e9e1c153c0fc47ddcadcd55551bae5592a631fefd4" \
    "13270bd6ac41a240f752f7a5c58edd1a8daddd614ea31b6099093358@45.55.63.72:"    \
    "30303"

#define PARITY_P2P_LOCAL                                                       \
    "enode://"                                                                 \
    "66fff838be611b6ee574e069f731f47ee6553a6538dc19cc8cd0eb0ff000eacb022154cc" \
    "f5e0af537925b99afab23cdf7fb51985c40e36614f71d2f60edc275e@127.0.0.1:"      \
    "30303"

#define CPP_P2P_LOCAL                                                          \
    "enode://"                                                                 \
    "05b9568c6e5e3b69bcb4edfb762f5f676f7f10dc6a903474d3e136c8fd48365fbaf0b4de" \
    "bba5eeb9bd28204ef38452af10f51aa11534b9401f888cd2fc150456@127.0.0.1:"      \
    "30303.30303"

#define MAIN_NET_0                                                             \
    "enode://"                                                                 \
    "a979fb575495b8d6db44f750317d0f4622bf4c2aa3365d6af7c284339968eef29b69ad0d" \
    "ce72a4d8db5ebb4968de0e3bec910127f134779fbcb0cb6d3331163c:52.16.188.185:"  \
    "30303"

#define MAIN_NET_1                                                             \
    "enode://"                                                                 \
    "de471bccee3d042261d52e9bff31458daecc406142b401d4cd848f677479f73104b9fdeb" \
    "090af9583d3391b7f10cb2ba9e26865dd5fca4fcdc0fb1e3b723c786:54.94.239.50:"   \
    "30303"

#define MAIN_NET_2                                                             \
    "enode://"                                                                 \
    "1118980bf48b0a3640bdba04e0fe78b1add18e1cd99bf22d53daac1fd9972ad650df5217" \
    "6e7c7d89d1114cfef2bc23a2959aa54998a46afcf7d91809f0855082:52.74.57.123:"   \
    "30303"

#define TEST_NET_0                                                             \
    "enode://"                                                                 \
    "6ce05930c72abc632c58e2e4324f7c7ea478cec0ed4fa2528982cf34483094e9cbc9216e" \
    "7aa349691242576d552a2a56aaeae426c5303ded677ce455ba1acd9d@13.84.180.240:"  \
    "30303"

#define TEST_NET_1                                                             \
    "enode://"                                                                 \
    "20c9ad97c081d63397d7b685a412227a40e23c8bdc6688c6f37e97cfbc22d2b4d1db1510" \
    "d8f61e6a8866ad7f0e17c02b14182d37ea7c3c8b9c2683aeb6b733a1@52.169.14.227:"  \
    "30303"
// const char* g_test_enode = "enode://" REMOTE "@127.0.0.1:30303";

ueth_config config = { //
    .p2p_private_key = SKEY,
    .p2p_enable = 1,
    .udp = 22332,
    .interval_discovery = 10
};

int main(int argc, char* argv[]);

int
main(int argc, char* argv[])
{
    ((void)argc);
    ((void)argv);
    ueth_context eth;

    // Log message
    usys_log_note("Running ping pong demo");

    // Install interrupt control
    usys_install_signal_handlers();

    // start
    ueth_init(&eth, &config);
    ueth_boot(//
        &eth,
        3,
        TEST_ENODE_3,
        TEST_NET_0,
        TEST_NET_1);

    while (usys_running()) {
        // Poll io
        usys_msleep(10);
        ueth_poll(&eth);
    }

    // Notify remotes of shutdown and clean
    ueth_stop(&eth);
    ueth_deinit(&eth);
    return 0;
}
