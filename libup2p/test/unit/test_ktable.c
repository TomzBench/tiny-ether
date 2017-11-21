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

#include "test.h"

int test_ktable_want_ping(struct ktable* table, knodes* n);
int test_ktable_want_find(struct ktable* table, knodes* n, uint8_t*, uint32_t);

uint32_t g_test_ktable_want_find_count = 0;
uint32_t g_test_ktable_want_ping_count = 0;

uecc_ctx g_ktable_test_keys[KTABLE_N_NODES + 1];

ktable_settings g_ktable_settings = {
    .refresh = 300,                     // short interval for testing
    .alpha = 3,                         //
    .pong_timeout = 100,                // short interval for testing
    .want_ping = test_ktable_want_ping, // test callbacks
    .want_find = test_ktable_want_find  // test callbacks
};

int test_ktable_maintenance();
int test_ktable_storage();

int
test_ktable()
{
    int err = 0;

    // Setup test input
    for (int i = 0; i < KTABLE_N_NODES + 1; i++) {
        uecc_key_init_new(&g_ktable_test_keys[i]);
    }

    err |= test_ktable_storage();
    err |= test_ktable_maintenance();

    // Free test input
    for (int i = 0; i < KTABLE_N_NODES + 1; i++) {
        uecc_key_deinit(&g_ktable_test_keys[i]);
    }
    return err;
}

int
test_ktable_storage()
{
    int err = 0, size;
    ktable table;
    ktable_init(&table, &g_ktable_settings, NULL);
    uint8_t puba[65], pubb[65];
    knodes* node = NULL;

    // Insert the same node confirm table doesnt grow
    for (int i = 0; i < 10; i++) {
        ktable_insert(&table, &g_ktable_test_keys[0].Q, i, i, i, NULL);
    }
    err |= knodes_size(table.nodes, KTABLE_N_NODES) == 1 ? 0 : -1;

    // Make sure no overflow
    for (int i = 0; i < KTABLE_N_NODES + 1; i++) {
        ktable_insert(&table, &g_ktable_test_keys[i].Q, i, i, i, NULL);
    }
    err |= knodes_size(table.nodes, KTABLE_N_NODES) == KTABLE_N_NODES ? 0 : -1;

    // Access keys
    for (int i = 0; i < KTABLE_N_NODES; i++) {
        node = ktable_get(&table, &g_ktable_test_keys[i].Q);
        if (!node) {
            err |= -1;
        } else {
            uecc_qtob(&node->nodeid, puba, 65);
            uecc_qtob(&g_ktable_test_keys[i].Q, pubb, 65);
            err |= memcmp(puba, pubb, 65) ? -1 : 0;
        }
    }

    // Test remove
    for (int i = 0; i < 2; i++) {
        ktable_remove(&table, &g_ktable_test_keys[i].Q);
    }
    size = knodes_size(table.nodes, KTABLE_N_NODES);
    err |= size == KTABLE_N_NODES - 2 ? 0 : -1;

    // Free
    ktable_deinit(&table);
    return err;
}

int
test_ktable_maintenance()
{
    int err = 0, size;
    uint32_t tick = 0;
    ktable table;

    // Init table
    ktable_init(&table, &g_ktable_settings, NULL);

    // Make sure pings will clear table:
    // Fill table,
    // Ping all the nodes
    // Only half will pong
    // Wait timers to expire
    // Size should equal half
    g_test_ktable_want_ping_count = 0;
    for (int i = 0; i < KTABLE_N_NODES; i++) {
        ktable_insert(&table, &g_ktable_test_keys[i].Q, i, i, i, NULL);
        ktable_ping(&table, &g_ktable_test_keys[i].Q);
    }
    for (uint32_t i = 0; i < KTABLE_N_NODES; i++) {
        if (!(i % 2)) ktable_on_pong(&table, &g_ktable_test_keys[i].Q, i, i, i);
    }
    usys_msleep(table.settings.pong_timeout + 1);
    ktable_poll(&table);
    size = knodes_size(table.nodes, KTABLE_N_NODES);
    err |= size == KTABLE_N_NODES / 2 ? 0 : -1;

    // Check we send some find requests:
    // Just wait and poll table.
    // Find requests should fire every alpha*table.settings.refresh
    g_test_ktable_want_find_count = 0;
    tick = usys_tick();
    while (usys_tick() < (tick + table.settings.refresh * 3)) {
        ktable_poll(&table);
    }
    err |= g_test_ktable_want_find_count == table.settings.alpha * 3 ? 0 : -1;

    // TODO - make sure we ping a node when table over flows:
    // Fill table
    // Add 1 extra node
    // Wait pong timeout
    // Size should be full-1
    // g_test_ktable_want_ping_count = 0;
    // for (int i = 0; i < KTABLE_N_NODES; i++) {
    //    ktable_insert(&table, &g_ktable_test_keys[i].Q, i, i, i, NULL);
    //}
    // ktable_insert(&table, &g_ktable_test_keys[20].Q, 20, 20, 20, NULL);
    // if (!(g_test_ktable_want_find_count == 1)) err |= -1;
    // usys_msleep(table.settings.pong_timeout + 1);
    // ktable_poll(&table);
    // err |= knodes_size(table.nodes, KTABLE_N_NODES) == KTABLE_N_NODES - 1;

    // free table
    ktable_deinit(&table);
    return err;
}

/*
int
test_ktable_maintenance()
{
    int err = 0;
    uint32_t tick = 0;
    ktable table;
    ktable_init(&table, &g_ktable_settings, NULL);

    // Reset counters for test
    g_test_ktable_want_ping_count = 0;

    // fill table
    for (uint32_t i = 0; i < table.settings.size; i++) {
        ktable_insert(&table, i + 100, i, i, i, NULL, NULL);
    }

    // Pong half table
    for (uint32_t i = 0; i < table.settings.size; i++) {
        if (!(i % 2)) ktable_pong(&table, i + 100, i, i, i, NULL);
    }

    // Make sure table half empty
    usys_msleep(table.settings.pong_timeout + 1);
    ktable_poll(&table);
    err |= ktable_size(&table) == table.settings.size / 2 ? 0 : -1;

    // Check we sent some pings
    err |= g_test_ktable_want_ping_count == table.settings.size ? 0 : -1;

    // Check we send some find requests
    g_test_ktable_want_find_count = 0;
    tick = usys_tick();
    while (usys_tick() < (tick + table.settings.refresh * 10 + 2)) {
        ktable_poll(&table);
    }
    err |= g_test_ktable_want_find_count == 10 * ktable_size(&table) ? 0 : -1;

    // Make sure we ping random nodes that ping us that arent in our table
    g_test_ktable_want_ping_count = 0;
    ktable_ping(&table, 999999999, 9, 9, 9, NULL);
    err |= ktable_size(&table) == table.settings.size / 2 + 1 ? 0 : -1;
    err |= g_test_ktable_want_ping_count == 1 ? 0 : -1;

    ktable_deinit(&table);
    return err;
}
*/

int
test_ktable_want_ping(ktable* self, knodes* node)
{
    ((void)self);
    ((void)node);
    g_test_ktable_want_ping_count++;
    return 0;
}

int
test_ktable_want_find(ktable* self, knodes* node, uint8_t* id, uint32_t idlen)
{
    ((void)self);
    ((void)node);
    ((void)id);
    ((void)idlen);
    g_test_ktable_want_find_count++;
    return 0;
}
