#include "uecc.h"
#include "uecies_decrypt.h"
#include "uecies_encrypt.h"
#include "uhash.h"
#include "usha3.h"
#include <stdint.h>
#include <string.h>

typedef h256 ubn;

int test_check_cmp(ubn*, const char* hex);
const uint8_t* makebin(const char* str, size_t* len);

// clang-format off
#define IF_ERR_EXIT(f)                    \
    do {                                  \
        if ((err = (f)) != 0) goto EXIT;  \
    } while (0)
#define IF_NEG_EXIT(val, f)               \
    do {                                  \
        if ((val = (f)) < 0) goto EXIT;   \
    } while (0)
// clang-format on

/**
 * @brief Test vectors
 */
const char* kdf1 =
    "0de72f1223915fa8b8bf45dffef67aef8d89792d116eb61c9a1eb02c422a4663";
const char* kdf2 =
    "961c065873443014e0371f1ed656c586c6730bf927415757f389d92acf8268df";
const char* kdf1_result = "1D0C446F9899A3426F2B89A8CB75C14B";
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
    "e3f407f83fc012470c26a93fdff534100f2c6f736439ce0ca90e9914f7d1c381";
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
const char* g_ecdh_pri =
    "332143e9629eedff7d142d741f896258f5a1bfab54dab2121d3ec5000093d74b";
const char* g_ecdh_pub =
    "f0d2b97981bd0d415a843b5dfe8ab77a30300daab3658c578f2340308a2da1a07f082136"
    "7332598b6aa4e180a41e92f4ebbae3518da847f0b1c0bbfe20bcf4e1";
const char* g_ecdh_secret =
    "ee1418607c2fcfb57fda40380e885a707f49000a5dda056d828b7d9bd1f29a08";
/**
 * @brief Prototypes
 */
int test_ecc();
int test_ecdh();
int test_recover();
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
    err |= test_ecdh();
    err |= test_recover();
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
    size_t l = 100;   // buffer sz
    uint8_t stest[l]; // binary buffer
    uecc_ctx ctxa, ctxa_clone, ctxb, ctxc;
    /****uecc_public_key pubkeya, pubkeyb;*/
    uecc_signature sig;

    // Init stack
    uecc_key_init_string(&ctxa, 16, alice_ekey_str);
    uecc_key_init_string(&ctxb, 16, bob_ekey_str);
    uecc_key_init_binary(&ctxa_clone, &ctxa.d);
    uecc_key_init_new(&ctxc);
    memset(stest, 'a', l);

    // Generate a shared secret with known private keys with point public key
    IF_ERR_EXIT(uecc_agree(&ctxa, &ctxb.Q));
    IF_ERR_EXIT(uecc_agree(&ctxa_clone, &ctxb.Q));
    IF_ERR_EXIT(uecc_agree(&ctxb, &ctxa.Q));
    IF_ERR_EXIT(uecc_z_cmp(&ctxa.z, &ctxb.z) ? -1 : 0);
    IF_ERR_EXIT(uecc_z_cmp(&ctxa_clone.z, &ctxb.z) ? -1 : 0);
    IF_ERR_EXIT(uecc_z_cmp_str(&ctxa.z, expect_secret_str));

    // Generated shared secret with random key
    IF_ERR_EXIT(uecc_agree(&ctxa, &ctxc.Q));
    IF_ERR_EXIT(uecc_agree(&ctxa_clone, &ctxc.Q));
    IF_ERR_EXIT(uecc_agree(&ctxc, &ctxa.Q));
    IF_ERR_EXIT(uecc_z_cmp(&ctxa.z, &ctxc.z) ? -1 : 0);
    IF_ERR_EXIT(uecc_z_cmp(&ctxa_clone.z, &ctxc.z) ? -1 : 0);

    // Sign our test blob verify with pubkey
    IF_ERR_EXIT(uecc_sign(&ctxa, stest, 32, &sig));
    IF_ERR_EXIT(uecc_verify(&ctxa.Q, stest, 32, &sig));
    IF_ERR_EXIT(uecc_verify(&ctxa_clone.Q, stest, 32, &sig));
    memset(&sig, 0, sizeof(sig));

    // Verify same key created with key import, check bad sig returns err
    IF_ERR_EXIT(uecc_sign(&ctxa_clone, stest, 32, &sig));
    ctxa_clone.Q.data[3] = 0xff; // fail the sig
    IF_ERR_EXIT(uecc_verify(&ctxa_clone.Q, stest, 32, &sig) ? 0 : -1);

EXIT:
    uecc_key_deinit(&ctxa);
    uecc_key_deinit(&ctxa_clone);
    uecc_key_deinit(&ctxb);
    uecc_key_deinit(&ctxc);
    return err;
}

int
test_ecdh()
{
    uecc_ctx ecc;
    uecc_private_key pri;
    uecc_public_key pub;
    int err;
    uint8_t pub_bin[65] = { 0x04 }, secret[32];
    memcpy(pri.b, makebin(g_ecdh_pri, NULL), 32);
    memcpy(&pub_bin[1], makebin(g_ecdh_pub, NULL), 64);
    memcpy(secret, makebin(g_ecdh_secret, NULL), 32);
    uecc_btoq(pub_bin, 65, &pub);

    uecc_key_init_binary(&ecc, &pri);
    uecc_agree(&ecc, &pub);
    IF_ERR_EXIT(memcmp(&ecc.z.b[1], secret, 32) ? -1 : 0);
    memset(ecc.z.b, 0, 33);
    uecc_agree_bin(&ecc, pub_bin, 65);
    IF_ERR_EXIT(memcmp(&ecc.z.b[1], secret, 32) ? -1 : 0);

EXIT:
    uecc_key_deinit(&ecc);
    return err;
}

int
test_recover()
{
    int err;
    uecc_ctx alice;
    uint8_t rawsig[65], rawpub[65], alicepub[65], msg[32];
    uecc_signature sig;
    uecc_public_key pub;
    uecc_key_init_new(&alice);

    // Create message (TODO add more recover api without casting)
    usha3((uint8_t*)"hello bob", 9, msg, 32);
    uecc_sign(&alice, msg, 32, &sig);
    uecc_sig_to_bin(&sig, rawsig);
    uecc_recover_bin(rawsig, (uecc_shared_secret*)&msg, &pub);
    uecc_qtob(&pub, rawpub, 65);
    uecc_qtob(&alice.Q, alicepub, 65);
    err = memcmp(rawpub, alicepub, 65) ? -1 : 0;
    uecc_key_deinit(&alice);
    return err;
}

int
test_kdf()
{
    int err = -1;
    const char* expects[] = { kdf1_result, kdf2_result };
    const char* kdfs[] = { kdf1, kdf2 };
    size_t len[] = { 16, 32 };

    for (int i = 0; i < 2; i++) {
        size_t rlen = len[i], kdflen = strlen(kdfs[i]) / 2;
        uint8_t kdf[kdflen];
        uint8_t expect[rlen];
        uint8_t result[rlen];
        memset(result, 0, rlen);
        memcpy(expect, makebin(expects[i], NULL), rlen);
        memcpy(kdf, makebin(kdfs[i], NULL), kdflen);

        uhash_kdf(kdf, kdflen, result, rlen);
        IF_ERR_EXIT(memcmp(result, expect, rlen) ? -1 : 0);
    }

    err = 0;
EXIT:
    return err;
}

int
test_hmac()
{
    uhmac_sha256_ctx h256;
    int err = -1;
    size_t lkey = strlen(g_hmac) / 2, lin = strlen(g_hmac_input) / 2,
           lres = strlen(g_hmac_result) / 2;
    uint8_t key[lkey];
    uint8_t input[lin];
    uint8_t result[lres];
    uint8_t expect[lres];
    memcpy(key, makebin(g_hmac, NULL), lkey);
    memcpy(input, makebin(g_hmac_input, NULL), lin);
    memcpy(expect, makebin(g_hmac_result, NULL), lres);

    uhmac_sha256_init(&h256, key, 16);
    uhmac_sha256_update(&h256, input, 10);
    uhmac_sha256_finish(&h256, result);
    err = memcmp(expect, result, lres) ? -1 : 0;

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
    uecc_ctx ctxb;
    uecc_private_key bobkey;
    int err = -1;
    size_t cipher_len = strlen(auth) / 2, sz, l = cipher_len;
    size_t plain_len = strlen(auth_plain) / 2;
    uint8_t cipher[cipher_len];
    uint8_t result[plain_len];
    uint8_t expect[plain_len];

    memcpy(cipher, makebin(auth, NULL), cipher_len);
    memcpy(expect, makebin(auth_plain, NULL), plain_len);
    memcpy(bobkey.b, makebin(bob_pkey_str, NULL), 32);

    uecc_key_init_binary(&ctxb, &bobkey);
    IF_NEG_EXIT(sz, uecies_decrypt(&ctxb, NULL, 0, cipher, l, result));
    IF_ERR_EXIT(memcmp(result, expect, plain_len) ? -1 : 0);
    err = 0;
EXIT:
    uecc_key_deinit(&ctxb);
    return err;
}

const uint8_t*
makebin(const char* str, size_t* len)
{
    static uint8_t buf[512];
    size_t s;
    if (!len) len = &s;
    *len = strlen(str) / 2;
    if (*len > 512) *len = 512;
    for (size_t i = 0; i < *len; i++) {
        uint8_t c = 0;
        if (str[i * 2] >= '0' && str[i * 2] <= '9')
            c += (str[i * 2] - '0') << 4;
        if ((str[i * 2] & ~0x20) >= 'A' && (str[i * 2] & ~0x20) <= 'F')
            c += (10 + (str[i * 2] & ~0x20) - 'A') << 4;
        if (str[i * 2 + 1] >= '0' && str[i * 2 + 1] <= '9')
            c += (str[i * 2 + 1] - '0');
        if ((str[i * 2 + 1] & ~0x20) >= 'A' && (str[i * 2 + 1] & ~0x20) <= 'F')
            c += (10 + (str[i * 2 + 1] & ~0x20) - 'A');
        buf[i] = c;
    }
    return buf;
}

//
//
//
