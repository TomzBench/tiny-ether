#include "mtm/rlpx_config.h"

int test_handshake_pain();

int
main(int argc, char* argv[])
{
    ((void)argc);
    ((void)argv);
    int err = 0;

    err |= test_handshake_pain();

    return err;
}

int
test_handshake_pain()
{
}
