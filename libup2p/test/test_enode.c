#include "test.h"

int test_hex();

int
test_enode()
{
    int err = 0;
    err |= test_hex();
    return err;
}

int
test_hex()
{
    int err = 0;
    uint8_t bytes[64];
    char result[129];
    rlpx_node_hex_to_bin(BOB_EPUB, 0, bytes, NULL);
    rlpx_node_bin_to_hex(bytes, 64, result, NULL);
    IF_ERR_EXIT(memcmp(result, BOB_EPUB, 128) ? -1 : 0);
EXIT:
    return err;
}
