#include "mtm/ecc.h"
#include "mtm/ecies.h"
#include <string.h>

// clang-format off
#define IF_ERR_EXIT(f)                    \
    do {                                  \
        if ((err = (f)) != 0) goto EXIT;  \
    } while (0)
#define IF_NEG_EXIT(val, f)               \
    do {                                  \
        if ((val = (f)) < 0) goto EXIT;   \
    } while (0)
// clang-format off

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
const char* g_hmac = "af6623e52208c596e17c72cea6f1cb09";
const char* g_hmac_input = "3461282bcedace970df2";
const char* g_hmac_result =
    "B3CE623BCE08D5793677BA9441B22BB34D3E8A7DE964206D26589DF3E8EB5183";
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
    "884C36F7AE6B406637C1F61B2F57E1D2CAB813D24C6559AAF843C3F48962F32F46662C066D"
    "39669B7B2E3BA14781477417600E7728399278B1B5D801A519AA570034FDB5419558137E0D"
    "44CD13D319AFE5629EECCB47FD9DFE55CC6089426E46CC762DD8A0636E07A54B31169EBA0C"
    "7A20A1AC1EF68596F1F283B5C676BAE4064ABFCCE24799D09F67E392632D3FFDC12E3D6430"
    "DCB0EA19C318343FFA7AAE74D4CD26FECB93657D1CD9E9EAF4F8BE720B56DD1D39F190C4E1"
    "C6B7EC66F077BB1100";
const char* auth =
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
int test_hmac();
int test_ecies_encrypt();
int test_ecies_decrypt();

int
main(int argc, char* argv[])
{
    ((void)argc);
    ((void)argv);
    int err = 0;
    err |= test_ecc();
    err |= test_kdf();
    err |= test_hmac();
    err |= test_ecies_encrypt();
    err |= test_ecies_decrypt();
    return err;
}

int
test_ecc()
{
    int err = 0;
    size_t l = 100;     // buffer sz
    uint8_t stest[l];   // binary buffer
    char secret_str[l]; // ascii buffer
    uecc_ctx ctxa, ctxa_clone, ctxb, ctxc;
    uecc_public_key pubkeya, pubkeyb;
    uecc_signature sig;

    // Init stack
    uecc_key_init_string(&ctxa, 16, alice_ekey_str);
    uecc_key_init_string(&ctxb, 16, bob_ekey_str);
    uecc_key_init_binary(&ctxa_clone, &ctxa.d);
    uecc_key_init_new(&ctxc);
    memset(stest, 'a', l);

    // Generate a shared secret with known private keys with point public key
    err |= uecc_agree_point(&ctxa, &ctxb.Q);
    err |= uecc_agree_point(&ctxb, &ctxa.Q);
    if (!(err == 0)) goto EXIT;
    err |= ubn_cmp(&ctxa.z, &ctxb.z) ? -1 : 0;
    if (!(err == 0)) goto EXIT;
    ubn_toa(&ctxa.z, 16, secret_str, l, &l);
    err = memcmp(expect_secret_str, secret_str, strlen(secret_str)) ? -1 : 0;
    if (!(err == 0)) goto EXIT; // note our write fn prints in caps

    // Generate shared secret with known private keys with binary public key
    uecc_ptob(&ctxa.Q, &pubkeya);
    uecc_ptob(&ctxb.Q, &pubkeyb);
    err |= uecc_agree(&ctxa, &pubkeyb);
    err |= uecc_agree(&ctxb, &pubkeya);
    if (!(err == 0)) goto EXIT;
    err |= ubn_cmp(&ctxa.z, &ctxb.z) ? -1 : 0;
    if (!(err == 0)) goto EXIT;

    // Generated shared secret with random key
    err |= uecc_agree_point(&ctxa, &ctxc.Q);
    err |= uecc_agree_point(&ctxc, &ctxa.Q);
    if (!(err == 0)) goto EXIT;
    err |= ubn_cmp(&ctxa.z, &ctxc.z) ? -1 : 0;
    if (!(err == 0)) goto EXIT;

    // Sign our test blob
    err = uecc_sign(&ctxa, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    // Verify with public key
    err = uecc_verify(&ctxa.Q, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    err = uecc_verify(&ctxa_clone.Q, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    memset(&sig, 0, sizeof(sig));

    // Verify same key created with key import
    err = uecc_sign(&ctxa_clone, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

    err = uecc_verify(&ctxa_clone.Q, stest, 66, &sig);
    if (!(err == 0)) goto EXIT;

EXIT:
    uecc_key_deinit(&ctxa);
    uecc_key_deinit(&ctxa_clone);
    uecc_key_deinit(&ctxb);
    uecc_key_deinit(&ctxc);
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
    ubn result_mpi;
    ubn_init(&result_mpi);

    for (int i = 0; i < 2; i++) {
        const char* k = kdf[i];
        const char* r = expect[i];
        uint8_t result_bin[len[i]];
        size_t rlen = 0;
        memset(result_bin, 0, len[i]);
        memset(result_str, 0, 66);

        // kdf
        err = uecies_kdf_str(k, 16, result_bin, len[i]);
        if (!(err == 0)) goto EXIT;

        // mpi_to_b
        err = ubn_bin(&result_mpi, result_bin, len[i]);
        if (!(err == 0)) goto EXIT;

        // btoa
        err = ubn_toa(&result_mpi, 16, result_str, 66, &rlen);
        if (!(err == 0)) goto EXIT;

        // Check result
        err = memcmp(r, result_str, strlen(result_str));
        if (!(err == 0)) goto EXIT;
    }

    err = 0;
EXIT:
    ubn_free(&result_mpi);
    return err;
}

int
test_hmac()
{
    int err = -1;
    uhmac_sha256_ctx h256;
    size_t olen_result = 100, olen_hmac = 16, olen_hmac_input = 10;
    uint8_t hmac[olen_hmac], hmac_input[olen_hmac_input];
    uint8_t hmac_result[32];
    char str_result[olen_result];

    memset(str_result, 0, olen_result);

    err = ubn_atob(16, g_hmac, hmac, &olen_hmac);
    if (!err) ubn_atob(16, g_hmac_input, hmac_input, &olen_hmac_input);
    if (err) return err;

    uhmac_sha256_init(&h256, hmac, 16);
    uhmac_sha256_update(&h256, hmac_input, 10);
    uhmac_sha256_finish(&h256, hmac_result);

    err = ubn_btoa(hmac_result, 32, 16, str_result, &olen_result);
    if (err) return err;

    err = memcmp(g_hmac_result, str_result, olen_result) ? -1 : 0;
    return err;
}

int
test_ecies_encrypt()
{
    int err = 0;
    uint8_t in[] = { 't', 'e', 's', 't', ' ', 'y', 'e', 'a', 'h' };
    uint8_t mac[3] = { 0x30, 0x11, 0x47 };
    uint8_t out[65 + 16 + sizeof(in) + 32];
    uint8_t clr[9];
    size_t sz;
    uecc_ctx bob;
    IF_ERR_EXIT(uecc_key_init_new(&bob));

    IF_ERR_EXIT(uecies_encrypt(&bob.Q, 0, 0, in, 9, out));
    IF_NEG_EXIT(sz, uecies_decrypt(&bob, 0, 0, out, sizeof(out), clr));
    IF_ERR_EXIT(memcmp(clr, in, sizeof(clr)) ? -1 : 0);
    memset(out, 0, 65 + 16 + sizeof(in) + 32);
    memset(clr, 0, 9);
    IF_ERR_EXIT(uecies_encrypt(&bob.Q, mac, 3, in, 9, out));
    IF_NEG_EXIT(sz, uecies_decrypt(&bob, mac, 3, out, sizeof(out), clr));
    IF_ERR_EXIT(memcmp(clr, in, sizeof(clr)) ? -1 : 0);

EXIT:
    uecc_key_deinit(&bob);
    return err;
}

int
test_ecies_decrypt()
{
    int err = -1;
    size_t l = 194, slen = 400;
    uint8_t plain[l];
    size_t sz;
    char str_plain[slen];
    memset(plain, 0, l);
    uecc_ctx ctxb;
    uecc_key_init_string(&ctxb, 16, bob_pkey_str);
    IF_NEG_EXIT(sz, uecies_decrypt_str(&ctxb, NULL, 0, 16, auth, plain));
    IF_ERR_EXIT(ubn_btoa(plain, l, 16, str_plain, &slen));
    IF_ERR_EXIT(memcmp(auth_plain, str_plain, slen) ? -1 : 0);

EXIT:
    uecc_key_deinit(&ctxb);
    return err;
}

//
//
//
