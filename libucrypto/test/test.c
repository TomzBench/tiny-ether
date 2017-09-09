#include "mtm/ecc.h"
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
    ucrypto_ecdh_ctx ctxa, ctxa_clone, ctxb;
    ucrypto_ecdh_public_key pubkeya, pubkeyb;
    ucrypto_ecp_signature sig;
    const ucrypto_mpi *secreta, *secretb;
    uint8_t stest[66];
    ucrypto_ecdh_key_init(&ctxa, NULL);
    ucrypto_ecdh_key_init(&ctxb, NULL);
    ucrypto_ecdh_key_init(&ctxa_clone, &ctxa.d);
    memset(stest, 'a', 66);

    // Generate a shared secret from point
    err |= ucrypto_ecdh_agree_point(&ctxa, ucrypto_ecdh_pubkey(&ctxb));
    err |= ucrypto_ecdh_agree_point(&ctxb, ucrypto_ecdh_pubkey(&ctxa));
    if (!(err == 0)) goto EXIT;
    secreta = ucrypto_ecdh_secret(&ctxa);
    secretb = ucrypto_ecdh_secret(&ctxb);
    err |= ucrypto_mpi_cmp(secreta, secretb) ? -1 : 0;
    if (!(err == 0)) goto EXIT;

    // Generate shared secret from pubkey
    ucrypto_ecdh_point_write(&ctxa, &pubkeya);
    ucrypto_ecdh_point_write(&ctxb, &pubkeyb);
    err |= ucrypto_ecdh_agree(&ctxa, &pubkeyb);
    err |= ucrypto_ecdh_agree(&ctxb, &pubkeya);
    if (!(err == 0)) goto EXIT;
    secretb = ucrypto_ecdh_secret(&ctxb);
    err |= ucrypto_mpi_cmp(secreta, secretb) ? -1 : 0;
    if (!(err == 0)) goto EXIT;

    // Sign our test blob
    err = ucrypto_ecdh_sign(&ctxa, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    // Verify with public key
    err = ucrypto_ecdh_verify(ucrypto_ecdh_pubkey(&ctxa), stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    err =
        ucrypto_ecdh_verify(ucrypto_ecdh_pubkey(&ctxa_clone), stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    memset(sig, 0, sizeof(sig));

    err = ucrypto_ecdh_sign(&ctxa_clone, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    err =
        ucrypto_ecdh_verify(ucrypto_ecdh_pubkey(&ctxa_clone), stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

EXIT:
    ucrypto_ecdh_key_deinit(&ctxa);
    ucrypto_ecdh_key_deinit(&ctxa_clone);
    ucrypto_ecdh_key_deinit(&ctxb);
    return err;
}

//
//
//
