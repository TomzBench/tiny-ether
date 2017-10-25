#include "test.h"

int test_kademlia_table();

int
test_kademlia()
{
    int err = 0;
    IF_ERR_EXIT(test_kademlia_table());
EXIT:
    return err;
}

int
test_kademlia_table()
{
    /*!< stack */
    int err = 0;
    rlpx_kademlia_context table; /*!< routing table */
    rlpx_kademlia_node id, node; /*!< our node */

    /*!< init routines */
    rlpx_kademlia_random_node_id(&id); /*!< Create our random node id */
    rlpx_kademlia_init(&table, &id);   /*!< initialize table */

    /*!< stimulate table */
    for (int i = 0; i < 1000; i++) {
        rlpx_kademlia_random_node_id(&node);
        rlpx_kademlia_add(&table, &node);
        if (rlpx_kademlia_buckets_len(&table) <= i + 2) err = -1;
    }
    if (rlpx_kademlia_buckets_len(&table) <= 512) err = -1;

    /*!< Check results */

    /*!< Free resources */
    rlpx_kademlia_deinit(&table); /*!< free resources */
    return err;
}
