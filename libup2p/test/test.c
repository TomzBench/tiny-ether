#include "mtm/ecc.h"
#include "mtm/mpi.h"
#include "mtm/rlpx.h"
#include <string.h>

typedef struct
{
    const char* auth;
    const char* ack;
    uint64_t authver;
    uint64_t ackver;
} test_vector;

/**
 * @brief
 *
 * RLPX Handshake test vectors
 * https://github.com/ethereum/EIPs/blob/master/EIPS/eip-8.md
 *
 * @return 0 pass
 */

test_vector g_test_vectors[] = {
    {.auth = "01b304ab7578555167be8154d5cc456f567d5ba302662433674222360f08d5f1"
             "534499d3678b513b0fca474f3a514b18e75683032eb63fccb16c156dc6eb2c0b"
             "1593f0d84ac74f6e475f1b8d56116b849634a8c458705bf83a626ea0384d4d73"
             "41aae591fae42ce6bd5c850bfe0b999a694a49bbbaf3ef6cda61110601d3b4c0"
             "2ab6c30437257a6e0117792631a4b47c1d52fc0f8f89caadeb7d02770bf999cc"
             "147d2df3b62e1ffb2c9d8c125a3984865356266bca11ce7d3a688663a51d82de"
             "faa8aad69da39ab6d5470e81ec5f2a7a47fb865ff7cca21516f9299a07b1bc63"
             "ba56c7a1a892112841ca44b6e0034dee70c9adabc15d76a54f443593fafdc3b2"
             "7af8059703f88928e199cb122362a4b35f62386da7caad09c001edaeb5f8a06d"
             "2b26fb6cb93c52a9fca51853b68193916982358fe1e5369e249875bb8d0d0ec3"
             "6f917bc5e1eafd5896d46bd61ff23f1a863a8a8dcd54c7b109b771c8e61ec9c8"
             "908c733c0263440e2aa067241aaa433f0bb053c7b31a838504b148f570c0ad62"
             "837129e547678c5190341e4f1693956c3bf7678318e2d5b5340c9e488eefea19"
             "8576344afbdf66db5f51204a6961a63ce072c8926c",
     .ack = "01ea0451958701280a56482929d3b0757da8f7fbe5286784beead59d95089c217"
            "c9b917788989470b0e330cc6e4fb383c0340ed85fab836ec9fb8a49672712aeab"
            "bdfd1e837c1ff4cace34311cd7f4de05d59279e3524ab26ef753a0095637ac88f"
            "2b499b9914b5f64e143eae548a1066e14cd2f4bd7f814c4652f11b254f8a2d019"
            "1e2f5546fae6055694aed14d906df79ad3b407d94692694e259191cde171ad542"
            "fc588fa2b7333313d82a9f887332f1dfc36cea03f831cb9a23fea05b33deb999e"
            "85489e645f6aab1872475d488d7bd6c7c120caf28dbfc5d6833888155ed69d34d"
            "bdc39c1f299be1057810f34fbe754d021bfca14dc989753d61c413d261934e1a9"
            "c67ee060a25eefb54e81a4d14baff922180c395d3f998d70f46f6b58306f96962"
            "7ae364497e73fc27f6d17ae45a413d322cb8814276be6ddd13b885b201b943213"
            "656cde498fa0e9ddc8e0b8f8a53824fbd82254f3e2c17e8eaea009c38b4aa0a3f"
            "306e8797db43c25d68e86f262e564086f59a2fc60511c42abfb3057c247a8a8fe"
            "4fb3ccbadde17514b7ac8000cdb6a912778426260c47f38919a91f25f4b5ffb45"
            "5d6aaaf150f7e5529c100ce62d6d92826a71778d809bdf60232ae21ce8a437eca"
            "8223f45ac37f6487452ce626f549b3b5fdee26afd2072e4bc75833c2464c80524"
            "6155289f4",
     .authver = 4,
     .ackver = 4 },
    {.auth = "01b8044c6c312173685d1edd268aa95e1d495474c6959bcdd10067ba4c9013df"
             "9e40ff45f5bfd6f72471f93a91b493f8e00abc4b80f682973de715d77ba3a005"
             "a242eb859f9a211d93a347fa64b597bf280a6b88e26299cf263b01b8dfdb7122"
             "78464fd1c25840b995e84d367d743f66c0e54a586725b7bbf12acca27170ae32"
             "83c1073adda4b6d79f27656993aefccf16e0d0409fe07db2dc398a1b7e8ee93b"
             "cd181485fd332f381d6a050fba4c7641a5112ac1b0b61168d20f01b479e19adf"
             "7fdbfa0905f63352bfc7e23cf3357657455119d879c78d3cf8c8c06375f3f7d4"
             "861aa02a122467e069acaf513025ff196641f6d2810ce493f51bee9c966b15c5"
             "043505350392b57645385a18c78f14669cc4d960446c17571b7c5d725021babb"
             "cd786957f3d17089c084907bda22c2b2675b4378b114c601d858802a55345a15"
             "116bc61da4193996187ed70d16730e9ae6b3bb8787ebcaea1871d850997ddc08"
             "b4f4ea668fbf37407ac044b55be0908ecb94d4ed172ece66fd31bfdadf2b97a8"
             "bc690163ee11f5b575a4b44e36e2bfb2f0fce91676fd64c7773bac6a003f481f"
             "ddd0bae0a1f31aa27504e2a533af4cef3b623f4791b2cca6d490",
     .ack = "01f004076e58aae772bb101ab1a8e64e01ee96e64857ce82b1113817c6cdd52c09"
            "d26f7b90981cd7ae835aeac72e1573b8a0225dd56d157a010846d888dac7464baf"
            "53f2ad4e3d584531fa203658fab03a06c9fd5e35737e417bc28c1cbf5e5dfc666d"
            "e7090f69c3b29754725f84f75382891c561040ea1ddc0d8f381ed1b9d0d4ad2a0e"
            "c021421d847820d6fa0ba66eaf58175f1b235e851c7e2124069fbc202888ddb3ac"
            "4d56bcbd1b9b7eab59e78f2e2d400905050f4a92dec1c4bdf797b3fc9b2f8e84a4"
            "82f3d800386186712dae00d5c386ec9387a5e9c9a1aca5a573ca91082c7d68421f"
            "388e79127a5177d4f8590237364fd348c9611fa39f78dcdceee3f390f07991b7b4"
            "7e1daa3ebcb6ccc9607811cb17ce51f1c8c2c5098dbdd28fca547b3f58c01a424a"
            "c05f869f49c6a34672ea2cbbc558428aa1fe48bbfd61158b1b735a65d99f21e70d"
            "bc020bfdface9f724a0d1fb5895db971cc81aa7608baa0920abb0a565c9c436e2f"
            "d13323428296c86385f2384e408a31e104670df0791d93e743a3a5194ee6b076fb"
            "6323ca593011b7348c16cf58f66b9633906ba54a2ee803187344b394f75dd2e663"
            "a57b956cb830dd7a908d4f39a2336a61ef9fda549180d4ccde21514d117b6c6fd0"
            "7a9102b5efe710a32af4eeacae2cb3b1dec035b9593b48b9d3ca4c13d245d5f041"
            "69b0b1",
     .authver = 56,
     .ackver = 57 },
    { 0, 0, 0, 0 }
};
const char* g_alice_s =
    "49a7b37aa6f6645917e7b807e9d1c00d4fa71f18343b0d4122a4d2df64dd6fee";
const char* g_alice_e =
    "869d6ecf5211f1cc60418a13b9d870b22959d0c16f02bec714c960dd2298a32d";
const char* g_bob_s =
    "b71c71a67e1177ad4e901695e1b4b9ee17ae16c6668d313eac2f96dbcda3f291";
const char* g_bob_e =
    "e238eb8e04fee6511ab04c6dd3c89ce097b11f25d584863ac2b6d5b35b1847e4";

int test_handshake();

int
main(int argc, char* argv[])
{
    ((void)argc);
    ((void)argv);
    int err = 0;

    err |= test_handshake();

    return err;
}

int
test_handshake()
{
    int err = 0;
    test_vector* tv = g_test_vectors;
    rlpx *alice, *bob;
    alice = rlpx_alloc_keypair(g_alice_s, g_alice_e);
    bob = rlpx_alloc_keypair(g_bob_s, g_bob_e);
    while (tv->auth) {
        size_t len = 1000;
        uint8_t cipher[len];
        err = ucrypto_mpi_atob(16, tv->auth, cipher, &len);
        if (!err) err = rlpx_read_auth(bob, cipher, len);
        tv++;
    }
    rlpx_free(&alice);
    rlpx_free(&bob);
    //    size_t blen = 1000;
    //    uint8_t b[blen]; // buffer
    //    rlpx_handshake handshake;
    //    test_vector* tv = &g_test_vectors[0];
    //    ucrypto_ecc_ctx alice_s, alice_e, bob_s, bob_e;
    //    ucrypto_ecc_key_init_string(&alice_s, 16, g_alice_s);
    //    ucrypto_ecc_key_init_string(&alice_e, 16, g_alice_e);
    //    ucrypto_ecc_key_init_string(&bob_s, 16, g_bob_s);
    //    ucrypto_ecc_key_init_string(&bob_e, 16, g_bob_e);
    //
    //    while (tv->auth) {
    //        blen = 1000;
    //        rlpx_handshake_init(&handshake);
    //        err = ucrypto_mpi_atob(16, tv->auth, b, &blen);
    //        if (!err) {
    //            err = rlpx_read_auth(&handshake, &bob_s, b, blen);
    //        }
    //        tv++;
    //    }
    //
    //    ucrypto_ecc_key_deinit(&alice_e);
    //    ucrypto_ecc_key_deinit(&alice_s);
    //    ucrypto_ecc_key_deinit(&bob_e);
    //    ucrypto_ecc_key_deinit(&bob_s);
    //    err = 0;
    return err;
}

//
//
//
