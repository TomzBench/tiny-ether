#include "mtm/ecc.h"
#include <string.h>

int test_ecdh();
const char* alice_key_str =
    "5e173f6ac3c669587538e7727cf19b782a4f2fda07c1eaa662c593e5e85e3051";
const char* bob_key_str =
    "c45f950382d542169ea207959ee0220ec1491755abe405cd7498d6b16adb6df8";
const char* expect_secret_str =
    "e3f407f83fc012470c26a93fdff534100f2c6f736439ce0ca90e9914f7d1c381";

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
    size_t l = 100;     // buffer sz
    uint8_t stest[l];   // binary buffer
    char secret_str[l]; // ascii buffer
    const ucrypto_mpi *secreta, *secretb, *secretc;
    ucrypto_ecc_ctx ctxa, ctxa_clone, ctxb, ctxc;
    ucrypto_ecc_public_key pubkeya, pubkeyb;
    ucrypto_ecc_signature sig;

    // Init stack
    ucrypto_ecc_key_init_string(&ctxa, 16, alice_key_str);
    ucrypto_ecc_key_init_string(&ctxb, 16, bob_key_str);
    ucrypto_ecc_key_init_binary(&ctxa_clone, &ctxa.d);
    ucrypto_ecc_key_init_new(&ctxc);
    memset(stest, 'a', l);

    // Generate a shared secret with known private keys with point public key
    err |= ucrypto_ecc_agree_point(&ctxa, ucrypto_ecc_pubkey(&ctxb));
    err |= ucrypto_ecc_agree_point(&ctxb, ucrypto_ecc_pubkey(&ctxa));
    if (!(err == 0)) goto EXIT;
    secreta = ucrypto_ecc_secret(&ctxa);
    secretb = ucrypto_ecc_secret(&ctxb);
    err |= ucrypto_mpi_cmp(secreta, secretb) ? -1 : 0;
    if (!(err == 0)) goto EXIT;
    ucrypto_mpi_write_string(&ctxa.z, 16, secret_str, l, &l);
    printf("%s\n\n", secret_str);

    // Generate shared secret with known private keys with binary public key
    ucrypto_ecc_ptob(&ctxa, &pubkeya);
    ucrypto_ecc_ptob(&ctxb, &pubkeyb);
    err |= ucrypto_ecc_agree(&ctxa, &pubkeyb);
    err |= ucrypto_ecc_agree(&ctxb, &pubkeya);
    if (!(err == 0)) goto EXIT;
    secretb = ucrypto_ecc_secret(&ctxb);
    err |= ucrypto_mpi_cmp(secreta, secretb) ? -1 : 0;
    if (!(err == 0)) goto EXIT;

    // Generated shared secret with random key
    err |= ucrypto_ecc_agree_point(&ctxa, ucrypto_ecc_pubkey(&ctxc));
    err |= ucrypto_ecc_agree_point(&ctxc, ucrypto_ecc_pubkey(&ctxa));
    if (!(err == 0)) goto EXIT;
    secreta = ucrypto_ecc_secret(&ctxa);
    secretc = ucrypto_ecc_secret(&ctxc);
    err |= ucrypto_mpi_cmp(secreta, secretc) ? -1 : 0;
    if (!(err == 0)) goto EXIT;

    // Sign our test blob
    err = ucrypto_ecc_sign(&ctxa, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    // Verify with public key
    err = ucrypto_ecc_verify(ucrypto_ecc_pubkey(&ctxa), stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    err = ucrypto_ecc_verify(ucrypto_ecc_pubkey(&ctxa_clone), stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    memset(sig, 0, sizeof(sig));

    // Verify same key created with key import
    err = ucrypto_ecc_sign(&ctxa_clone, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    err = ucrypto_ecc_verify(ucrypto_ecc_pubkey(&ctxa_clone), stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

EXIT:
    ucrypto_ecc_key_deinit(&ctxa);
    ucrypto_ecc_key_deinit(&ctxa_clone);
    ucrypto_ecc_key_deinit(&ctxb);
    ucrypto_ecc_key_deinit(&ctxc);
    return err;
}

//
//
//
