#include "test.h"
#include "urand.h"

int test_ktable();

int
test_kademlia()
{
    int err = 0;
    IF_ERR_EXIT(test_ktable());
EXIT:
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
