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

int test_ktable_want_ping(struct ktable* table, knode* n);
int test_ktable_want_find(struct ktable* table, knode* n, uint8_t*, uint32_t);

uint32_t g_test_ktable_want_find_count = 0;
uint32_t g_test_ktable_want_ping_count = 0;

ktable_settings g_ktable_settings = {
    .size = 40,                         // number of nodes
    .refresh = 10,                      // short interval for testing
    .pong_timeout = 100,                // short interval for testing
    .want_ping = test_ktable_want_ping, // test callbacks
    .want_find = test_ktable_want_find  // test callbacks
};

int test_ktable_storage();
int test_ktable_maintenance();

int
test_ktable()
{
    int err = 0;
    err |= test_ktable_storage();
    err |= test_ktable_maintenance();
    return err;
}

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

int
test_ktable_storage()
{
    // the table key is an index in an array, our hash value is i+100 because
    // we want it to be different than the iterator to make sure api is correct
    // during test. So that is why you see i+100 in the loops using the api
    int err = 0;
    ktable table;
    knode* node = NULL;

    // Allocate table
    ktable_init(&table, &g_ktable_settings, NULL);

    // Put some nodes into our map
    for (uint32_t i = 0; i < 20; i++) {
        ktable_insert(&table, i + 100, i, i, i, NULL, NULL);
    }

    // Verify we have our nodes
    for (uint32_t i = 0; i < 20; i++) {
        node = ktable_get(&table, i + 100);
        err |= (node && node->ip == i) ? 0 : -1;
    }
    err |= ktable_size(&table) == 20 ? 0 : -1;

    // Check empty
    node = ktable_get(&table, 1);
    if (node) err |= -1;

    // Check shrinking
    for (uint32_t i = 10; i < 20; i++) {
        ktable_remove(&table, i + 100);
    }
    err |= ktable_size(&table) == 10 ? 0 : -1;

    // Check overwrite
    for (uint32_t i = 0; i < 10; i++) {
        ktable_insert(&table, i + 100, i, i, i, NULL, NULL);
    }
    err |= ktable_size(&table) == 10 ? 0 : -1;

    // Check overflow bounds
    for (uint32_t i = 0; i <= table.settings.size + 1; i++) {
        ktable_insert(&table, i + 200, i, i, i, NULL, NULL);
    }
    err |= ktable_size(&table) == table.settings.size ? 0 : -1;

    // Really Check overflow bounds
    for (uint32_t i = 0; i <= table.settings.size * 10; i++) {
        ktable_insert(&table, i + 200, i, i, i, NULL, NULL);
    }
    err |= ktable_size(&table) == table.settings.size ? 0 : -1;

    // Free hash table
    ktable_deinit(&table);
    return err;
}

int
test_ktable_want_ping(ktable* self, knode* node)
{
    ((void)self);
    ((void)node);
    g_test_ktable_want_ping_count++;
    return 0;
}

int
test_ktable_want_find(ktable* self, knode* node, uint8_t* id, uint32_t idlen)
{
    ((void)self);
    ((void)node);
    ((void)id);
    ((void)idlen);
    g_test_ktable_want_find_count++;
    return 0;
}
