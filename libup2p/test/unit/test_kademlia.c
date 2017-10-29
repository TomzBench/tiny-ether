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
#include "urand.h"

int test_ktable();

int
test_kademlia()
{
    int err = 0;
    IF_ERR_EXIT(test_ktable());
EXIT:
    err = 0; // TODO
    return err;
}

int
test_ktable()
{
    /*!< stack */
    int err = 0;
    ktable table;   /*!< routing table */
    knode id, node; /*!< our node, add node */

    /*!< init routines */
    urand(id.b, sizeof(id.b)); /*!< initiaze our id */
    ktable_init(&table, &id);  /*!< initialize table */

    /*!< stimulate table */
    for (uint32_t i = 0; i < 1000; i++) {
        urand(node.b, sizeof(node.b));                     /*!< add me */
        ktable_add_node(&table, &node);                    /*!< node added */
        if (ktable_buckets_len(&table) <= i + 2) err = -1; /*!< check */
    }
    if (ktable_buckets_len(&table) <= 512) err = -1;

    /*!< Check results */

    /*!< Free resources */
    ktable_deinit(&table); /*!< free resources */
    return err;
}
