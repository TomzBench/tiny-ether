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
    ecdh_ctx *ctxa, *ctxb;
    int err = 0;
    ctxa = ecdh_key_alloc(&ctxa);
    ctxb = ecdh_key_alloc(&ctxb);
    if (!(ctxa && ctxb)) goto EXIT;

EXIT:
    if (ctxa) ecdh_key_free(&ctxa);
    if (ctxb) ecdh_key_free(&ctxb);
    return err;
}
