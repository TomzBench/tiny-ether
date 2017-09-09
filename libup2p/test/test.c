#include "mtm/ecc.h"
#include "mtm/mpi.h"
#include "mtm/rlpx.h"
#include "mtm/sha.h"
#include <string.h>

/**
 * @brief
 *
 * RLPX Handshake test vectors
 * https://github.com/ethereum/EIPs/blob/master/EIPS/eip-8.md
 *
 * Static Key A:
 * 49a7b37aa6f6645917e7b807e9d1c00d4fa71f18343b0d4122a4d2df64dd6fee
 *
 * Static Key B:
 * b71c71a67e1177ad4e901695e1b4b9ee17ae16c6668d313eac2f96dbcda3f291
 *
 * Ephemeral Key A:
 * 869d6ecf5211f1cc60418a13b9d870b22959d0c16f02bec714c960dd2298a32d
 *
 * Ephemeral Key B:
 * e238eb8e04fee6511ab04c6dd3c89ce097b11f25d584863ac2b6d5b35b1847e4
 *
 * Nonce A:
 * 7e968bba13b6c50e2c4cd7f241cc0d64d1ac25c7f5952df231ac6a2bda8ee5d6
 *
 * Nonce B:
 * 559aead08264d5795d3909718cdd05abd49572e84fe55590eef31a88a08fdffd
 *
 * @return 0 pass
 */
const char* g_spriv_a =
    "5e173f6ac3c669587538e7727cf19b782a4f2fda07c1eaa662c593e5e85e3051";
const char* g_spriv_b =
    "c45f950382d542169ea207959ee0220ec1491755abe405cd7498d6b16adb6df8";
const char* g_epriv_a =
    "19c2185f4f40634926ebed3af09070ca9e029f2edd5fae6253074896205f5f6c";
const char* g_epriv_b =
    "d25688cf0ab10afa1a0e2dba7853ed5f1e5bf1c631757ed4e103b593ff3f5620";
const char* g_nonce_a =
    "cd26fecb93657d1cd9e9eaf4f8be720b56dd1d39f190c4e1c6b7ec66f077bb11";
const char* g_nonce_b =
    "f37ec61d84cea03dcc5e8385db93248584e8af4b4d1c832d8c7453c0089687a7";
const char* g_auth_1 =
    "04a0274c5951e32132e7f088c9bdfdc76c9d91f0dc6078e848f8e3361193dbdc43b94351ea"
    "3d89e4ff33ddcefbc80070498824857f499656c4f79bbd97b6c51a514251d69fd1785ef876"
    "4bd1d262a883f780964cce6a14ff206daf1206aa073a2d35ce2697ebf3514225bef186631b"
    "2fd2316a4b7bcdefec8d75a1025ba2c5404a34e7795e1dd4bc01c6113ece07b0df13b69d3b"
    "a654a36e35e69ff9d482d88d2f0228e7d96fe11dccbb465a1831c7d4ad3a026924b182fc2b"
    "dfe016a6944312021da5cc459713b13b86a686cf34d6fe6615020e4acf26bf0d5b7579ba81"
    "3e7723eb95b3cef9942f01a58bd61baee7c9bdd438956b426a4ffe238e61746a8c93d5e106"
    "80617c82e48d706ac4953f5e1c4c4f7d013c87d34a06626f498f34576dc017fdd3d581e83c"
    "fd26cf125b6d2bda1f1d56";
// const char* g_spriv_a =
//    "49a7b37aa6f6645917e7b807e9d1c00d4fa71f18343b0d4122a4d2df64dd6fee";
// const char* g_spriv_b =
//    "b71c71a67e1177ad4e901695e1b4b9ee17ae16c6668d313eac2f96dbcda3f291";
// const char* g_epriv_a =
//    "869d6ecf5211f1cc60418a13b9d870b22959d0c16f02bec714c960dd2298a32d";
// const char* g_epriv_b =
//    "e238eb8e04fee6511ab04c6dd3c89ce097b11f25d584863ac2b6d5b35b1847e4";
// const char* g_nonce_a =
//    "7e968bba13b6c50e2c4cd7f241cc0d64d1ac25c7f5952df231ac6a2bda8ee5d6";
// const char* g_nonce_b =
//    "559aead08264d5795d3909718cdd05abd49572e84fe55590eef31a88a08fdffd";
// const char* g_auth_1 = ""
//                       "048ca79ad18e4b0659fab4853fe5bc58eb8399"
//                       "2980f4c9cc147d2aa31532efd29a3d3dc6a3d8"
//                       "9eaf913150cfc777ce0ce4af2758bf4810235f"
//                       "6e6ceccfee1acc6b22c005e9e3a49d6448610a"
//                       "58e98744ba3ac0399e82692d67c1f58849050b"
//                       "3024e21a52c9d3b01d871ff5f210817912773e"
//                       "610443a9ef142e91cdba0bd77b5fdf0769b056"
//                       "71fc35f83d83e4d3b0b000c6b2a1b1bba89e0f"
//                       "c51bf4e460df3105c444f14be226458940d606"
//                       "1c296350937ffd5e3acaceeaaefd3c6f74be8e"
//                       "23e0f45163cc7ebd76220f0128410fd0525027"
//                       "3156d548a414444ae2f7dea4dfca2d43c057ad"
//                       "b701a715bf59f6fb66b2d1d20f2c703f851cbf"
//                       "5ac47396d9ca65b6260bd141ac4d53e2de585a"
//                       "73d1750780db4c9ee4cd4d225173a4592ee77e"
//                       "2bd94d0be3691f3b406f9bba9b591fc63facc0"
//                       "16bfa8";
int test_auth_pain();
int test_rlpx_session();

int
main(int argc, char* argv[])
{
    ((void)argc);
    ((void)argv);
    int err = 0;

    err |= test_auth_pain();
    err |= test_rlpx_session();

    return err;
}

int
test_auth_pain()
{
    int err = 0;
    return err;
}

int
test_rlpx_session()
{
    int err = -1;
    uint8_t auth_binary[307];
    ucrypto_ecdh_ctx static_key;
    ucrypto_mpi auth1;
    ucrypto_mpi skey;
    ucrypto_mpi_init(&auth1);
    ucrypto_mpi_init(&skey);
    ucrypto_mpi_read_string(&auth1, 16, g_auth_1);
    ucrypto_mpi_read_string(&skey, 16, g_spriv_b);

    // Create static key with test param
    ucrypto_ecdh_key_init(&static_key, &skey);

    // Create new session
    rlpx_session* session = rlpx_session_alloc();

    // Print binary auth
    if (!(ucrypto_mpi_size(&auth1) == 307)) goto EXIT;
    ucrypto_mpi_write_binary(&auth1, auth_binary, 307);

    // Read authentication
    err = rlpx_session_read_auth(session, &static_key, auth_binary, 307);
    if (!(err == 0)) goto EXIT;

    err = 0;

EXIT:
    // cleanup
    ucrypto_ecdh_key_deinit(&static_key);
    ucrypto_mpi_free(&auth1);
    ucrypto_mpi_free(&skey);
    rlpx_session_free(&session);
    return err;
}

//
//
//
