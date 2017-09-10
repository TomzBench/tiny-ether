#include "mtm/ecc.h"
#include <string.h>

/**
 * @brief Test vectors
 */
const char* alice_key_str =
    "19c2185f4f40634926ebed3af09070ca9e029f2edd5fae6253074896205f5f6c";
const char* bob_key_str =
    "d25688cf0ab10afa1a0e2dba7853ed5f1e5bf1c631757ed4e103b593ff3f5620";
const char* expect_secret_str =
    "E3F407F83FC012470C26A93FDFF534100F2C6F736439CE0CA90E9914F7D1C381";

/**
 * @brief Prototypes
 */
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
    size_t l = 100;     // buffer sz
    uint8_t stest[l];   // binary buffer
    char secret_str[l]; // ascii buffer
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
    err |= ucrypto_ecc_agree_point(&ctxa, &ctxb.Q);
    err |= ucrypto_ecc_agree_point(&ctxb, &ctxa.Q);
    if (!(err == 0)) goto EXIT;
    err |= ucrypto_mpi_cmp(&ctxa.z, &ctxb.z) ? -1 : 0;
    if (!(err == 0)) goto EXIT;
    ucrypto_mpi_write_string(&ctxa.z, 16, secret_str, l, &l);
    err = memcmp(expect_secret_str, secret_str, strlen(secret_str)) ? -1 : 0;
    if (!(err == 0)) goto EXIT; // note our write fn prints in caps

    // Generate shared secret with known private keys with binary public key
    ucrypto_ecc_ptob(&ctxa, &pubkeya);
    ucrypto_ecc_ptob(&ctxb, &pubkeyb);
    err |= ucrypto_ecc_agree(&ctxa, &pubkeyb);
    err |= ucrypto_ecc_agree(&ctxb, &pubkeya);
    if (!(err == 0)) goto EXIT;
    err |= ucrypto_mpi_cmp(&ctxa.z, &ctxb.z) ? -1 : 0;
    if (!(err == 0)) goto EXIT;

    // Generated shared secret with random key
    err |= ucrypto_ecc_agree_point(&ctxa, &ctxc.Q);
    err |= ucrypto_ecc_agree_point(&ctxc, &ctxa.Q);
    if (!(err == 0)) goto EXIT;
    err |= ucrypto_mpi_cmp(&ctxa.z, &ctxc.z) ? -1 : 0;
    if (!(err == 0)) goto EXIT;

    // Sign our test blob
    err = ucrypto_ecc_sign(&ctxa, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    // Verify with public key
    err = ucrypto_ecc_verify(&ctxa.Q, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    err = ucrypto_ecc_verify(&ctxa_clone.Q, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    memset(sig, 0, sizeof(sig));

    // Verify same key created with key import
    err = ucrypto_ecc_sign(&ctxa_clone, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    err = ucrypto_ecc_verify(&ctxa_clone.Q, stest, 66, &sig);
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
