#include "dh.h"

int test_ecdh();

int
main(int argc, char* argv[])
{
    ((void)argc);
    ((void)argv);
    int err = 0;
    err |= test_ecdh();
    return err;
}

int
test_ecdh()
{
    int err = 0;
    crypto_ecdh_ctx* ctx = crypto_ecdh_key_alloc();
    if (!ctx) err = -1;
    crypto_ecdh_key_free(&ctx);
    return err;
}
