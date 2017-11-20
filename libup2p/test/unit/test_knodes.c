#include "test.h"

int test_knodes_storage();

int
test_knodes()
{
    int err = 0;
    err |= test_knodes_storage();
    return err;
}

int
test_knodes_storage()
{
    // the nodes key is an index in an array
    int err = 0, c = 0;
    knodes nodes[100];
    knodes* node = NULL;

    // Init nodes
    knodes_init(nodes, 100);

    // Check init state
    err |= knodes_size(nodes, 100) == 0 ? 0 : -1;

    // Add some nodes
    for (uint32_t i = 0; i < 100; i++) {
        knodes_insert(nodes, i, i, i, i, NULL);
    }

    // Check nodes added
    err |= knodes_size(nodes, 100) == 100 ? 0 : -1;
    for (int i = 0; i < 100; i++) {
        node = knodes_get(nodes, i);
        err |= node->key == i ? 0 : -1;
    }

    // Check shrinking
    for (int i = 50; i < 100; i++) {
        knodes_remove(nodes, i);
    }
    err |= knodes_size(nodes, 100) == 50 ? 0 : -1;

    // Check empty
    node = knodes_get(nodes, 50);
    if (node) err |= -1;

    // Chec overwrite
    for (int i = 0; i < 50; i++) {
        knodes_insert(nodes, i, i, i, i, NULL);
    }
    err |= knodes_size(nodes, 100) == 50 ? 0 : -1;

    // free nodes
    knodes_deinit(nodes, 100);
    return err;
}
