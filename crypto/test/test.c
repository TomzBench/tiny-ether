#include "dh.h"
#include <string.h>

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
    ecp_signature sig;
    uint8_t signme[66];
    ctxa = ecdh_key_alloc(&ctxa);
    ctxb = ecdh_key_alloc(&ctxb);
    memset(signme, 'a', 66);
    if (!(ctxa && ctxb)) goto EXIT;

    // Generate a shared secret
    ecdh_agree(ctxa, ecdh_pubkey(ctxb));
    ecdh_agree(ctxb, ecdh_pubkey(ctxa));
    err = mpi_cmp(ecdh_secret(ctxa), ecdh_secret(ctxb)) ? -1 : 0;
    if (!(err == 0)) goto EXIT;

    // Sign verify check
    err = ecdh_sign(ctxa, signme, 66, sig);
    if (!(err == 0)) goto EXIT;

    err = ecdh_verify(ecdh_pubkey(ctxa), signme, 66, sig);
    if (!(err == 0)) goto EXIT;

EXIT:
    if (ctxa) ecdh_key_free(&ctxa);
    if (ctxb) ecdh_key_free(&ctxb);
    return err;
}