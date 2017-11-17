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

ktable_settings g_ktable_settings = { //
    .size = 40,
    .refresh = 10,
    .want_ping = test_ktable_want_ping,
    .want_find = test_ktable_want_find
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
    ktable table;
    ktable_init(&table, &g_ktable_settings);

    // fill table
    for (uint32_t i = 0; i < table.settings.size; i++) {
        ktable_insert(&table, i + 100, i, i, i, NULL, NULL);
    }

    // Pong half table
    for (uint32_t i = 0; i < table.settings.size; i++) {
        if (!(i % 2)) ktable_pong(&table, i + 100);
    }

    // Make sure table half empty
    usys_msleep(table.settings.pong_timeout + 1);
    ktable_poll(&table);
    err |= ktable_size(&table) == table.settings.size / 2 ? 0 : -1;

    // Check we send some find requests

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
    ktable_init(&table, &g_ktable_settings);

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
    return 0;
}

int
test_ktable_want_find(ktable* self, knode* node, uint8_t* id, uint32_t idlen)
{
    ((void)self);
    ((void)node);
    ((void)id);
    ((void)idlen);
    return 0;
}
