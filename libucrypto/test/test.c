#include "mtm/ecc.h"
#include "mtm/ecies.h"
#include <string.h>

/**
 * @brief Test vectors
 */
const char* kdf1 =
    "0de72f1223915fa8b8bf45dffef67aef8d89792d116eb61c9a1eb02c422a4663";
const char* kdf1_result = "1D0C446F9899A3426F2B89A8CB75C14B";
const char* kdf2 =
    "961c065873443014e0371f1ed656c586c6730bf927415757f389d92acf8268df";
const char* kdf2_result =
    "4050C52E6D9C08755E5A818AC66FABE478B825B1836FD5EFC4D44E40D04DABCC";
const char* alice_pkey_str =
    "5e173f6ac3c669587538e7727cf19b782a4f2fda07c1eaa662c593e5e85e3051";
const char* alice_ekey_str =
    "19c2185f4f40634926ebed3af09070ca9e029f2edd5fae6253074896205f5f6c";
const char* bob_pkey_str =
    "c45f950382d542169ea207959ee0220ec1491755abe405cd7498d6b16adb6df8";
const char* bob_ekey_str =
    "d25688cf0ab10afa1a0e2dba7853ed5f1e5bf1c631757ed4e103b593ff3f5620";
const char* expect_secret_str =
    "E3F407F83FC012470C26A93FDFF534100F2C6F736439CE0CA90E9914F7D1C381";
const char* auth_plain =
    "884c36f7ae6b406637c1f61b2f57e1d2cab813d24c6559aaf843c3f48962f32f46662c066d"
    "39669b7b2e3ba14781477417600e7728399278b1b5d801a519aa570034fdb5419558137e0d"
    "44cd13d319afe5629eeccb47fd9dfe55cc6089426e46cc762dd8a0636e07a54b31169eba0c"
    "7a20a1ac1ef68596f1f283b5c676bae4064abfcce24799d09f67e392632d3ffdc12e3d6430"
    "dcb0ea19c318343ffa7aae74d4cd26fecb93657d1cd9e9eaf4f8be720b56dd1d39f190c4e1"
    "c6b7ec66f077bb1100";
const char* auth_cipher =
    "04a0274c5951e32132e7f088c9bdfdc76c9d91f0dc6078e848f8e3361193dbdc43b94351ea"
    "3d89e4ff33ddcefbc80070498824857f499656c4f79bbd97b6c51a514251d69fd1785ef876"
    "4bd1d262a883f780964cce6a14ff206daf1206aa073a2d35ce2697ebf3514225bef186631b"
    "2fd2316a4b7bcdefec8d75a1025ba2c5404a34e7795e1dd4bc01c6113ece07b0df13b69d3b"
    "a654a36e35e69ff9d482d88d2f0228e7d96fe11dccbb465a1831c7d4ad3a026924b182fc2b"
    "dfe016a6944312021da5cc459713b13b86a686cf34d6fe6615020e4acf26bf0d5b7579ba81"
    "3e7723eb95b3cef9942f01a58bd61baee7c9bdd438956b426a4ffe238e61746a8c93d5e106"
    "80617c82e48d706ac4953f5e1c4c4f7d013c87d34a06626f498f34576dc017fdd3d581e83c"
    "fd26cf125b6d2bda1f1d56";
/**
 * @brief Prototypes
 */
int test_ecc();
int test_kdf();
int test_ecies();

int
main(int argc, char* argv[])
{
    ((void)argc);
    ((void)argv);
    int err = 0;
    err |= test_ecc();
    err |= test_kdf();
    err |= test_ecies();
    return err;
}

int
test_ecc()
{
    int err = 0;
    size_t l = 100;     // buffer sz
    uint8_t stest[l];   // binary buffer
    char secret_str[l]; // ascii buffer
    ucrypto_ecc_ctx ctxa, ctxa_clone, ctxb, ctxc;
    ucrypto_ecc_public_key pubkeya, pubkeyb;
    ucrypto_ecc_signature sig;

    // Init stack
    ucrypto_ecc_key_init_string(&ctxa, 16, alice_ekey_str);
    ucrypto_ecc_key_init_string(&ctxb, 16, bob_ekey_str);
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

int
test_kdf()
{
    int err = -1;
    const char* expect[] = { kdf1_result, kdf2_result };
    const char* kdf[] = { kdf1, kdf2 };
    size_t len[] = { 16, 32 };
    char result_str[66]; // worst case size

    // Init stack
    ucrypto_mpi result_mpi;
    ucrypto_mpi_init(&result_mpi);

    for (int i = 0; i < 2; i++) {
        const char* k = kdf[i];
        const char* r = expect[i];
        uint8_t result_bin[len[i]];
        size_t rlen = 0;
        memset(result_bin, 0, len[i]);
        memset(result_str, 0, 66);

        // kdf
        err = ucrypto_ecies_kdf_string(k, 16, result_bin, len[i]);
        if (!(err == 0)) goto EXIT;

        // mpi_to_b
        err = ucrypto_mpi_read_binary(&result_mpi, result_bin, len[i]);
        if (!(err == 0)) goto EXIT;

        // btoa
        err = ucrypto_mpi_write_string(&result_mpi, 16, result_str, 66, &rlen);
        if (!(err == 0)) goto EXIT;

        // Check result
        err = memcmp(r, result_str, strlen(result_str));
        if (!(err == 0)) goto EXIT;
    }

    err = 0;
EXIT:
    ucrypto_mpi_free(&result_mpi);
    return err;
}

int
test_ecies()
{
    int err = -1;
    size_t l = 300;
    uint8_t plain[l];
    ucrypto_ecc_ctx ctxb;
    ucrypto_ecc_key_init_string(&ctxb, 16, bob_pkey_str);
    err = ucrypto_ecies_decrypt_string(&ctxb, 16, auth_cipher, plain, l);
    if (!(err == 0)) goto EXIT;

    err = 0;
EXIT:
    ucrypto_ecc_key_deinit(&ctxb);
    return err;
}

//
//
//
