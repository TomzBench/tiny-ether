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
    ucrypto_ecc_ctx ctxa, ctxa_clone, ctxb;
    ucrypto_ecc_public_key pubkeya, pubkeyb;
    ucrypto_ecc_signature sig;
    const ucrypto_mpi *secreta, *secretb;
    uint8_t stest[66];
    ucrypto_ecc_key_init(&ctxa, NULL);
    ucrypto_ecc_key_init(&ctxb, NULL);
    ucrypto_ecc_key_init(&ctxa_clone, &ctxa.d);
    memset(stest, 'a', 66);

    // Generate a shared secret from point
    err |= ucrypto_ecc_agree_point(&ctxa, ucrypto_ecc_pubkey(&ctxb));
    err |= ucrypto_ecc_agree_point(&ctxb, ucrypto_ecc_pubkey(&ctxa));
    if (!(err == 0)) goto EXIT;
    secreta = ucrypto_ecc_secret(&ctxa);
    secretb = ucrypto_ecc_secret(&ctxb);
    err |= ucrypto_mpi_cmp(secreta, secretb) ? -1 : 0;
    if (!(err == 0)) goto EXIT;

    // Generate shared secret from pubkey
    ucrypto_ecc_ptob(&ctxa, &pubkeya);
    ucrypto_ecc_ptob(&ctxb, &pubkeyb);
    err |= ucrypto_ecc_agree(&ctxa, &pubkeyb);
    err |= ucrypto_ecc_agree(&ctxb, &pubkeya);
    if (!(err == 0)) goto EXIT;
    secretb = ucrypto_ecc_secret(&ctxb);
    err |= ucrypto_mpi_cmp(secreta, secretb) ? -1 : 0;
    if (!(err == 0)) goto EXIT;

    // Sign our test blob
    err = ucrypto_ecc_sign(&ctxa, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    // Verify with public key
    err = ucrypto_ecc_verify(ucrypto_ecc_pubkey(&ctxa), stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    err =
        ucrypto_ecc_verify(ucrypto_ecc_pubkey(&ctxa_clone), stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    memset(sig, 0, sizeof(sig));

    err = ucrypto_ecc_sign(&ctxa_clone, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    err =
        ucrypto_ecc_verify(ucrypto_ecc_pubkey(&ctxa_clone), stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

EXIT:
    ucrypto_ecc_key_deinit(&ctxa);
    ucrypto_ecc_key_deinit(&ctxa_clone);
    ucrypto_ecc_key_deinit(&ctxb);
    return err;
}

//
//
//
