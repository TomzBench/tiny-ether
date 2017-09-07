#include "mtm/rlpx_config.h"
#include "mtm/mpi.h"
#include "mtm/sha.h"
#include "mtm/dh.h"

int test_auth_pain();

int
main(int argc, char* argv[])
{
    ((void)argc);
    ((void)argv);
    int err = 0;

    err |= test_auth_pain();

    return err;
}

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
int
test_auth_pain()
{
    int err = -1;
    const char* auth = ""
                       "048ca79ad18e4b0659fab4853fe5bc58eb8399"
                       "2980f4c9cc147d2aa31532efd29a3d3dc6a3d8"
                       "9eaf913150cfc777ce0ce4af2758bf4810235f"
                       "6e6ceccfee1acc6b22c005e9e3a49d6448610a"
                       "58e98744ba3ac0399e82692d67c1f58849050b"
                       "3024e21a52c9d3b01d871ff5f210817912773e"
                       "610443a9ef142e91cdba0bd77b5fdf0769b056"
                       "71fc35f83d83e4d3b0b000c6b2a1b1bba89e0f"
                       "c51bf4e460df3105c444f14be226458940d606"
                       "1c296350937ffd5e3acaceeaaefd3c6f74be8e"
                       "23e0f45163cc7ebd76220f0128410fd0525027"
                       "3156d548a414444ae2f7dea4dfca2d43c057ad"
                       "b701a715bf59f6fb66b2d1d20f2c703f851cbf"
                       "5ac47396d9ca65b6260bd141ac4d53e2de585a"
                       "73d1750780db4c9ee4cd4d225173a4592ee77e"
                       "2bd94d0be3691f3b406f9bba9b591fc63facc0"
                       "16bfa8";

    ucrypto_mpi skey_a, skey_b;
    ucrypto_mpi ekey_a, ekey_b;
    ucrypto_mpi nonce_a, nonce_b;
    ucrypto_mpi_init(&skey_a);
    ucrypto_mpi_init(&skey_b);
    ucrypto_mpi_init(&ekey_a);
    ucrypto_mpi_init(&ekey_b);
    ucrypto_mpi_init(&nonce_a);
    ucrypto_mpi_init(&nonce_b);
    ucrypto_mpi_read_string(
        &skey_a, 16,
        "49a7b37aa6f6645917e7b807e9d1c00d4fa71f18343b0d4122a4d2df64dd6fee");
    ucrypto_mpi_read_string(
        &skey_b, 16,
        "b71c71a67e1177ad4e901695e1b4b9ee17ae16c6668d313eac2f96dbcda3f291");
    ucrypto_mpi_read_string(
        &ekey_a, 16,
        "869d6ecf5211f1cc60418a13b9d870b22959d0c16f02bec714c960dd2298a32d");
    ucrypto_mpi_read_string(
        &ekey_b, 16,
        "e238eb8e04fee6511ab04c6dd3c89ce097b11f25d584863ac2b6d5b35b1847e4");
    ucrypto_mpi_read_string(
        &nonce_a, 16,
        "7e968bba13b6c50e2c4cd7f241cc0d64d1ac25c7f5952df231ac6a2bda8ee5d6");
    ucrypto_mpi_read_string(
        &nonce_b, 16,
        "559aead08264d5795d3909718cdd05abd49572e84fe55590eef31a88a08fdffd");

    err = 0;
    ucrypto_mpi_free(&skey_a);
    ucrypto_mpi_free(&skey_b);
    ucrypto_mpi_free(&ekey_a);
    ucrypto_mpi_free(&ekey_b);
    ucrypto_mpi_free(&nonce_a);
    ucrypto_mpi_free(&nonce_b);
    return err;
    err = 0;
    return err;
}
