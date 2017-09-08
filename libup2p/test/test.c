#include "mtm/dh.h"
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
    "49a7b37aa6f6645917e7b807e9d1c00d4fa71f18343b0d4122a4d2df64dd6fee";
const char* g_spriv_b =
    "b71c71a67e1177ad4e901695e1b4b9ee17ae16c6668d313eac2f96dbcda3f291";
const char* g_epriv_a =
    "869d6ecf5211f1cc60418a13b9d870b22959d0c16f02bec714c960dd2298a32d";
const char* g_epriv_b =
    "e238eb8e04fee6511ab04c6dd3c89ce097b11f25d584863ac2b6d5b35b1847e4";
const char* g_nonce_a =
    "7e968bba13b6c50e2c4cd7f241cc0d64d1ac25c7f5952df231ac6a2bda8ee5d6";
const char* g_nonce_b =
    "559aead08264d5795d3909718cdd05abd49572e84fe55590eef31a88a08fdffd";
const char* g_auth_1 = ""
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
    int err = -1;

    // A public key is 64 bytes
    // A signature is 65 bytes
    // A shared secret is 32 bytes?
    // A Hash is 32 bytes?
    uint8_t b[65 + 32 + 64 + 32 + 1];
    uint8_t noncea[32];
    uint8_t nonceb[32];
    uint8_t secret[32];
    ucrypto_ecdh_public_key epubkey;
    ucrypto_ecp_signature sig;
    ucrypto_h256 hepub;
    ucrypto_ecdh_ctx skey_a, skey_b;
    ucrypto_ecdh_ctx ekey_a, ekey_b;
    ucrypto_mpi spriv_a, spriv_b;
    ucrypto_mpi epriv_a, epriv_b;
    ucrypto_mpi nonce_a, nonce_b;
    ucrypto_mpi_init(&spriv_a);
    ucrypto_mpi_init(&spriv_b);
    ucrypto_mpi_init(&epriv_a);
    ucrypto_mpi_init(&epriv_b);
    ucrypto_mpi_init(&nonce_a);
    ucrypto_mpi_init(&nonce_b);
    ucrypto_mpi_read_string(&spriv_a, 16, g_spriv_a);
    ucrypto_mpi_read_string(&spriv_b, 16, g_spriv_b);
    ucrypto_mpi_read_string(&epriv_a, 16, g_epriv_a);
    ucrypto_mpi_read_string(&epriv_b, 16, g_epriv_b);
    ucrypto_mpi_read_string(&nonce_a, 16, g_nonce_a);
    ucrypto_mpi_read_string(&nonce_b, 16, g_nonce_b);
    ucrypto_mpi_write_binary(&nonce_a, noncea, 32);
    ucrypto_mpi_write_binary(&nonce_b, nonceb, 32);
    ucrypto_ecdh_key_init(&skey_a, &spriv_a);
    ucrypto_ecdh_key_init(&skey_b, &spriv_b);
    ucrypto_ecdh_key_init(&ekey_a, &epriv_a);
    ucrypto_ecdh_key_init(&ekey_b, &epriv_b);

    // E(remote-pubk, S(ecdhe-random, ecdh-shared-secret^nonce) ||
    // H(ecdhe-random-pubk) || pubk || nonce || 0x0)

    // sign shared secret^nonce between static keys with an ephermeral pri key
    ucrypto_ecdh_agree(&skey_a, ucrypto_ecdh_pubkey(&skey_b));
    ucrypto_mpi_write_binary(ucrypto_ecdh_secret(&skey_a), secret, 32); // 32?
    for (uint32_t i = 0; i < 32; i++) secret[i] ^= noncea[i];
    ucrypto_ecdh_sign(&ekey_a, secret, 32, &sig);
    memcpy(b, sig, 65);

    // Concat with sha3 hash of ephermeral pub key
    ucrypto_ecdh_pubkey_write(&ekey_a, &epubkey);
    ucrypto_sha3_256(epubkey, 64, hepub);
    memcpy(&b[65], hepub, 32);

    // Concat with pubkey
    memcpy(&b[65 + 32], epubkey, 64);

    // Concat with Nonce
    memcpy(&b[65 + 32 + 64], noncea, 32);

    // Concat with NULL
    b[65 + 32 + 64 + 32] = 0;

    err = 0;
    ucrypto_mpi_free(&spriv_a);
    ucrypto_mpi_free(&spriv_b);
    ucrypto_mpi_free(&epriv_a);
    ucrypto_mpi_free(&epriv_b);
    ucrypto_mpi_free(&nonce_a);
    ucrypto_mpi_free(&nonce_b);
    ucrypto_ecdh_key_deinit(&skey_a);
    ucrypto_ecdh_key_deinit(&skey_b);
    ucrypto_ecdh_key_deinit(&ekey_a);
    ucrypto_ecdh_key_deinit(&ekey_b);
    return err;
    err = 0;
    return err;
}

int
test_rlpx_session()
{
    int err = -1;
    ucrypto_ecdh_ctx static_key;
    const ucrypto_ecdh_ctx* static_key_ptr = &static_key;

    // Create static key
    ucrypto_ecdh_key_init(&static_key, NULL);

    // Create new session
    rlpx_session* session = rlpx_session_alloc(&static_key_ptr);

    // cleanup
    ucrypto_ecdh_key_deinit(&static_key);
    rlpx_session_free(&session);
    if (session) err = -1;
    err = 0;
    return err;
}

//
//
//
