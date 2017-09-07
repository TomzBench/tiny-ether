#include "mtm/dh.h"
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
    ucrypto_ecdh_ctx *ctxa, *ctxa_clone, *ctxb;
    ecp_signature sig;
    uint8_t stest[66];
    ctxa = ucrypto_ecdh_key_alloc(NULL);
    ctxb = ucrypto_ecdh_key_alloc(NULL);
    ctxa_clone = ucrypto_ecdh_key_alloc(&ctxa->d);
    if (!(ctxa && ctxa_clone && ctxb)) goto EXIT;
    memset(stest, 'a', 66);

    // Generate a shared secret
    ucrypto_ecdh_agree(ctxa, ucrypto_ecdh_pubkey(ctxb));
    ucrypto_ecdh_agree(ctxb, ucrypto_ecdh_pubkey(ctxa));
    err = ucrypto_mpi_cmp(ucrypto_ecdh_secret(ctxa), ucrypto_ecdh_secret(ctxb))
              ? -1
              : 0;
    if (!(err == 0)) goto EXIT;

    // Sign our test blob
    err = ucrypto_ecdh_sign(ctxa, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    // Verify with public key
    err = ucrypto_ecdh_verify(ucrypto_ecdh_pubkey(ctxa), stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    err = ucrypto_ecdh_verify(ucrypto_ecdh_pubkey(ctxa_clone), stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    memset(sig, 0, sizeof(sig));

    err = ucrypto_ecdh_sign(ctxa_clone, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    err = ucrypto_ecdh_verify(ucrypto_ecdh_pubkey(ctxa_clone), stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

EXIT:
    if (ctxa) ucrypto_ecdh_key_free(&ctxa);
    if (ctxb) ucrypto_ecdh_key_free(&ctxb);
    if (ctxa_clone) ucrypto_ecdh_key_free(&ctxa_clone);
    return err;
}

//
//
//
