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
    ecdh_ctx *ctxa, *ctxb;
    ctxa = ecdh_key_alloc(&ctxa);
    ctxb = ecdh_key_alloc(&ctxb);
    if (!(ctxa && ctxb)) goto EXIT;
    ecdh_agree(ctxa, ecdh_pubkey(ctxb));
    ecdh_agree(ctxb, ecdh_pubkey(ctxa));
    if (mpi_cmp(ecdh_secret(ctxa), ecdh_secret(ctxb))) {
        err = -1;
    } else {
        err = 0;
    }

EXIT:
    if (ctxa) ecdh_key_free(&ctxa);
    if (ctxb) ecdh_key_free(&ctxb);
    return err;
}
