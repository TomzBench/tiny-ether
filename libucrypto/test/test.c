#include "mtm/dh.h"
#include <string.h>

int test_ecdh();
int test_rlpx_handshake();

int
main(int argc, char* argv[])
{
    ((void)argc);
    ((void)argv);
    int err = 0;
    err |= test_ecdh();
    err |= test_rlpx_handshake();
    return err;
}

int
test_ecdh()
{
    int err = 0;
    ucrypto_ecdh_ctx *ctxa, *ctxa_clone, *ctxb;
    ecp_signature sig;
    uint8_t signme[66];
    ctxa = ucrypto_ecdh_key_alloc(NULL);
    ctxb = ucrypto_ecdh_key_alloc(NULL);
    ctxa_clone = ucrypto_ecdh_key_alloc(&ctxa->d);
    if (!(ctxa && ctxa_clone && ctxb)) goto EXIT;
    memset(signme, 'a', 66);

    // Generate a shared secret
    ucrypto_ecdh_agree(ctxa, ucrypto_ecdh_pubkey(ctxb));
    ucrypto_ecdh_agree(ctxb, ucrypto_ecdh_pubkey(ctxa));
    err = ucrypto_mpi_cmp(ucrypto_ecdh_secret(ctxa), ucrypto_ecdh_secret(ctxb))
              ? -1
              : 0;
    if (!(err == 0)) goto EXIT;

    // Sign our test blob
    err = ucrypto_ecdh_sign(ctxa, signme, 66, sig);
    if (!(err == 0)) goto EXIT;

    // Verify with public key
    err = ucrypto_ecdh_verify(ucrypto_ecdh_pubkey(ctxa), signme, 66, sig);
    if (!(err == 0)) goto EXIT;

    err = ucrypto_ecdh_verify(ucrypto_ecdh_pubkey(ctxa_clone), signme, 66, sig);
    if (!(err == 0)) goto EXIT;

    memset(sig, 0, sizeof(sig));

    err = ucrypto_ecdh_sign(ctxa_clone, signme, 66, sig);
    if (!(err == 0)) goto EXIT;

    err = ucrypto_ecdh_verify(ucrypto_ecdh_pubkey(ctxa_clone), signme, 66, sig);
    if (!(err == 0)) goto EXIT;

EXIT:
    if (ctxa) ucrypto_ecdh_key_free(&ctxa);
    if (ctxb) ucrypto_ecdh_key_free(&ctxb);
    if (ctxa_clone) ucrypto_ecdh_key_free(&ctxa_clone);
    return err;
}

int
test_rlpx_handshake()
{
    int err = -1;
    mpi skey_a, skey_b;
    mpi ekey_a, ekey_b;
    mpi nonce_a, nonce_b;
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
}

//
//
//
