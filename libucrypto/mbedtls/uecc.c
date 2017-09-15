#include "uecc.h"

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
 * @brief
 *    ECIES Encrypt, where P = recipient public key is:
 *    1) generate r = random value
 *    2) generate shared-secret = kdf( ecdhAgree(r, P) )
 *    3) generate R = rG [same op as generating a public key]
 *    4) send 0x04 || R || AsymmetricEncrypt(shared-secret, plaintext) || tag
 *    currently used by go:
 *    ECIES_AES128_SHA256 = &ECIESParams{
 *        Hash: sha256.New,
 *        hashAlgo: crypto.SHA256,
 *        Cipher: aes.NewCipher,
 *        BlockSize: aes.BlockSize,
 *        KeyLen: 16,
 *        }
 *
 *	let mut msg = vec![0u8; (1 + 64 + 16 + plain.len() + 32)];
 *	0x04 + echd-random-pubk + iv + aes(kdf(shared-secret), plaintext) + hmac
 *
 * 0x04 + echd-random-pubk + iv + aes(kdf(shared-secret), plaintext) + hmac
 * offset 0                65         81               275
 *        [ecies-pubkey:65||aes-iv:16||cipher-text:194||ecies-mac:32]
 *                        ||-----------hmac-----------||

*/

#include "mbedtls/sha256.h"
#include "uaes.h"
//#include "uecc.h"
#include "uhmac.h"
#include <string.h>

int
uecc_key_init(uecc_ctx* ctx, const ubn* d)
{
    return d ? uecc_key_init_binary(ctx, d) : uecc_key_init_new(ctx);
}

int
uecc_key_init_string(uecc_ctx* ctx, int radix, const char* s)
{
    int err = -1;
    ubn d;
    ubn_init(&d);
    err = ubn_str(&d, radix, s);
    if (!(err == 0)) goto EXIT;
    err = uecc_key_init_binary(ctx, &d);
    if (!(err == 0)) goto EXIT;
    err = 0;
EXIT:
    ubn_free(&d);
    return err;
}

int
uecc_key_init_binary(uecc_ctx* ctx, const ubn* d)
{
    int ret;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context rng;

    // initialize stack variables and callers ecdh context.
    mbedtls_ecdh_init(ctx);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&rng);

    // Seed rng
    ret = mbedtls_ctr_drbg_seed(&rng, mbedtls_entropy_func, &entropy, NULL, 0);
    if (!(ret == 0)) goto EXIT;

    // Load curve parameters
    ret = mbedtls_ecp_group_load(&ctx->grp, MBEDTLS_ECP_DP_SECP256K1);
    if (!(ret == 0)) goto EXIT;

    // Copy our key
    ret = mbedtls_mpi_copy(&ctx->d, d);
    if (!(ret == 0)) goto EXIT;

    // Get public key from private key?
    ret = mbedtls_ecp_mul(&ctx->grp, &ctx->Q, &ctx->d, &ctx->grp.G,
                          mbedtls_entropy_func, &entropy);
    if (!(ret == 0)) goto EXIT;

EXIT:
    if (ret) mbedtls_ecdh_free(ctx);
    mbedtls_ctr_drbg_free(&rng);
    mbedtls_entropy_free(&entropy);
    return ret;
}

int
uecc_key_init_new(uecc_ctx* ctx)
{
    int ret;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context rng;

    // initialize stack variables and callers ecc context.
    mbedtls_ecdh_init(ctx);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&rng);

    // Seed rng
    ret = mbedtls_ctr_drbg_seed(&rng, mbedtls_entropy_func, &entropy, NULL, 0);
    if (!(ret == 0)) goto EXIT;

    // Load curve parameters
    ret = mbedtls_ecp_group_load(&ctx->grp, MBEDTLS_ECP_DP_SECP256K1);
    if (!(ret == 0)) goto EXIT;

    // Create ecc public/private key pair
    ret = mbedtls_ecdh_gen_public(&ctx->grp, &ctx->d, &ctx->Q,
                                  mbedtls_ctr_drbg_random, &rng);
    if (!(ret == 0)) goto EXIT;

EXIT:
    if (ret) mbedtls_ecdh_free(ctx);
    mbedtls_ctr_drbg_free(&rng);
    mbedtls_entropy_free(&entropy);
    return ret;
}

void
uecc_key_deinit(uecc_ctx* ctx)
{
    mbedtls_ecdh_free(ctx);
}

int
uecc_atop(const char* str, int rdx, uecc_point* q)
{
    int err = -1;
    uint8_t buff[65];
    size_t l;
    mbedtls_ecp_group grp;
    mbedtls_mpi bin;

    // init stack
    mbedtls_mpi_init(&bin);
    mbedtls_ecp_group_init(&grp);
    err = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256K1);
    if (!(err == 0)) goto EXIT;

    // Read in string
    mbedtls_mpi_read_string(&bin, rdx, str);
    l = mbedtls_mpi_size(&bin);
    if (!(l == 65)) goto EXIT;
    mbedtls_mpi_write_binary(&bin, buff, l);
    err = mbedtls_ecp_point_read_binary(&grp, q, buff, l);
    if (!(err == 0)) goto EXIT;

    err = 0;

EXIT:
    // Free
    mbedtls_mpi_free(&bin);
    mbedtls_ecp_group_free(&grp);
    return err;
}

int
uecc_ptob(uecc_point* p, uecc_public_key* b)
{
    int err;
    size_t len = 65;
    mbedtls_ecp_group grp;
    mbedtls_ecp_group_init(&grp);
    err = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256K1);
    if (!err) {
        err = mbedtls_ecp_point_write_binary(
            &grp, p, MBEDTLS_ECP_PF_UNCOMPRESSED, &len, b->b, 65);
    }
    mbedtls_ecp_group_free(&grp);
    return err ? -1 : 0;
}

int
uecc_btop(uecc_public_key* k, uecc_point* p)
{
    int err = -1;
    mbedtls_ecp_group grp;
    mbedtls_ecp_group_init(&grp);
    err = mbedtls_ecp_point_read_binary(&grp, p, k->b, 65);
    mbedtls_ecp_group_free(&grp);
    return err;
}

int
uecc_point_copy(const uecc_point* src, uecc_point* dst)
{
    return mbedtls_ecp_copy(dst, src) == 0 ? 0 : -1;
}

int
uecc_point_cmp(const uecc_point* src, const uecc_point* dst)
{
    return mbedtls_ecp_point_cmp(src, dst);
}

int
uecc_ztoa(const uecc_point* p)
{
    return -1;
}

int
uecc_agree(uecc_ctx* ctx, const uecc_public_key* key)
{
    int err;
    uecc_point point;
    mbedtls_ecp_point_init(&point);

    /**
     * @brief note mbedtls_ecp_point_read_binary only accepts types of points
     * where key[0] is 0x04...
     */
    mbedtls_ecp_point_read_binary(&ctx->grp, &point, (uint8_t*)key,
                                  sizeof(uecc_public_key));

    err = uecc_agree_point(ctx, &point);
    mbedtls_ecp_point_free(&point);
    return err;
}

int
uecc_agree_point(uecc_ctx* ctx, const uecc_point* qp)
{
    int err;
    mbedtls_ctr_drbg_context rng;
    mbedtls_entropy_context entropy;

    // initialize stack content
    mbedtls_ctr_drbg_init(&rng);
    mbedtls_entropy_init(&entropy);

    // seed RNG
    err = mbedtls_ctr_drbg_seed(&rng, mbedtls_entropy_func, &entropy, NULL, 0);
    if (!(err == 0)) goto EXIT;

    // Create shared secret with other guys Q
    err = mbedtls_ecdh_compute_shared(&ctx->grp, &ctx->z, qp, &ctx->d,
                                      mbedtls_ctr_drbg_random, &rng);
    if (!(err == 0)) goto EXIT;
EXIT:
    mbedtls_ctr_drbg_free(&rng);
    mbedtls_entropy_free(&entropy);
    return err == 0 ? err : -1;
}

int
uecc_sign(uecc_ctx* ctx, const uint8_t* b, uint32_t sz, uecc_signature* sig_p)
{
    int err, ret = -1;
    uint8_t* sig = sig_p->b;
    ubn r, s;
    mbedtls_ctr_drbg_context rng;
    mbedtls_entropy_context entropy;
    for (int i = 0; i < 65; i++) sig[0] = 0;

    // Init stack content
    mbedtls_ctr_drbg_init(&rng);
    mbedtls_entropy_init(&entropy);
    ubn_init(&r);
    ubn_init(&s);

    // Seed RNG
    err = mbedtls_ctr_drbg_seed(&rng, mbedtls_entropy_func, &entropy, NULL, 0);
    if (!(err == 0)) goto EXIT;

    // Sign message
    err = mbedtls_ecdsa_sign(&ctx->grp, &r, &s, &ctx->d, b, sz,
                             mbedtls_ctr_drbg_random, &rng);
    if (!(err == 0)) goto EXIT;
    ret = 0;

    // Print Keys
    err = mbedtls_mpi_write_binary(&r, &sig[0], 32);
    if (!(err == 0)) goto EXIT;

    err = mbedtls_mpi_write_binary(&s, &sig[32], 32);
    if (!(err == 0)) goto EXIT;

    // TODO: ``\_("/)_/``
    // No idea if this is right. (Lowest bit of public keys Y coordinate)
    sig[64] = mbedtls_mpi_get_bit(&ctx->Q.Y, 0) ? 1 : 0;

EXIT:
    ubn_free(&r);
    ubn_free(&s);
    mbedtls_ctr_drbg_free(&rng);
    mbedtls_entropy_free(&entropy);
    return ret;
}

int
uecc_verify(const uecc_point* q,
            const uint8_t* b,
            uint32_t sz,
            uecc_signature* sig_p)
{
    int err, ret = -1;
    uint8_t* sig = sig_p->b;
    mbedtls_ecp_group grp;
    ubn r, s;

    // Init stack content
    mbedtls_ecp_group_init(&grp);
    ubn_init(&r);
    ubn_init(&s);
    err = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256K1);
    if (!(err == 0)) goto EXIT;

    err = mbedtls_mpi_read_binary(&r, sig, 32);
    if (!(err == 0)) goto EXIT;

    err = mbedtls_mpi_read_binary(&s, &sig[32], 32);
    if (!(err == 0)) goto EXIT;

    // Verify signature of content
    err = mbedtls_ecdsa_verify(&grp, b, sz, q, &r, &s);
    if (!(err == 0)) goto EXIT;

    ret = 0;
EXIT:
    mbedtls_ecp_group_free(&grp);
    ubn_free(&r);
    ubn_free(&s);
    return ret;
}
int
uecc_recover(const uecc_signature* sig,
             const uint8_t* digest,
             int recid,
             uecc_public_key* key)
{
    int err = 0;
    ubn r, s, e;
    uecc_point cp, cp2;
    mbedtls_ecp_group grp;
    mbedtls_ecp_group_init(&grp);
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);
    mbedtls_mpi_init(&e);
    mbedtls_ecp_point_init(&cp);
    mbedtls_ecp_point_init(&cp2);

    // external/trezor-crypto/ecdsa.c
    IF_ERR_EXIT(mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256K1));
    IF_ERR_EXIT(mbedtls_mpi_read_binary(&r, sig->b, 32));
    IF_ERR_EXIT(mbedtls_mpi_read_binary(&s, &sig->b[32], 32));
    IF_NEG_EXIT(err, mbedtls_mpi_cmp_mpi(&r, &grp.N));
    IF_NEG_EXIT(err, mbedtls_mpi_cmp_mpi(&s, &grp.N));

    // cp = R = k * G (k is secret nonce when signing)
    mbedtls_mpi_copy(&cp.X, &r);
    if (recid & 2) {
        mbedtls_mpi_add_mpi(&cp.X, &cp.X, &grp.N);
        IF_NEG_EXIT(err, mbedtls_mpi_cmp_mpi(&cp.X, &grp.N));
    }

// TODO -

// compute y from x
// uncompress_coords(curve, recid & 1, &cp.x, &cp.y);
// y is x
// y is x^2
// y is x^2+a
// y is x^3+ax
// Y is x^3+ax+b
// y = sqrt(y)
// e = -digest
// r := r^-1
// cp := s * R = s * k *G
// cp2 := -digest * G
// cp := (s * k - digest) * G = (r*priv) * G = r * Pub
// cp := r^{-1} * r * Pub = Pub

EXIT:
    mbedtls_ecp_group_free(&grp);
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    mbedtls_mpi_free(&e);
    mbedtls_ecp_point_free(&cp);
    mbedtls_ecp_point_free(&cp2);
    return err;
}

int
uecies_encrypt_str(uecc_point* p,
                   const uint8_t* s,
                   size_t slen,
                   int radix,
                   const char* plain,
                   uint8_t* cipher)
{
    // Convert to bignum and encrypt
    int err = -1;
    ubn bin;
    ubn_init(&bin);
    err = ubn_str(&bin, radix, plain);
    if (!err) err = uecies_encrypt_mpi(p, s, slen, &bin, cipher);
    return err;
}

int
uecies_encrypt_mpi(uecc_point* p,
                   const uint8_t* s,
                   size_t slen,
                   ubn* bin,
                   uint8_t* cipher)
{
    // convert to binary and encrypt
    int err = -1;
    size_t l = ubn_size(bin);
    uint8_t buff[l];
    err = ubn_tob(bin, buff, l);
    if (!err) err = uecies_encrypt(p, s, slen, buff, l, cipher);
    return err;
}

int
uecies_encrypt(uecc_point* p,
               const uint8_t* s,
               size_t slen,
               const uint8_t* in,
               size_t inlen,
               uint8_t* out)
{
    int err = 0;
    uint8_t key[32], mkey[32];
    uaes_iv iv;
    uhmac_sha256_ctx hmac;
    uecc_ctx ecc;
    uecc_public_key* ours = (uecc_public_key*)&out[0];
    uaes_128_ctr_key* ekey = (uaes_128_ctr_key*)&key[0];
    uaes_iv* iv_dst = (uaes_iv*)&out[65];
    uecc_key_init_new(&ecc);
    err = uecc_agree_point(&ecc, p);
    if (!err) {
        // 0x04 || R || IV || aes(kdf(agree(pub)),in) || tag
        uecies_kdf_mpi(&ecc.z, key, 32);
        ucrypto_sha256(&key[16], 16, mkey);
        memcpy(iv_dst->b, "0123456789012345", 16); // TODO init iv
        memcpy(iv.b, "0123456789012345", 16);      // TODO init iv
        err = uecc_ptob(&ecc.Q, ours);
        if (!err) err = uaes_crypt(ekey, &iv, in, inlen, &out[81]);
        if (!err) {
            // uhmac_sha256(mkey, 32, &out[65], 16 + inlen,
            //                    &out[65 + 16 + inlen]);
            uhmac_sha256_init(&hmac, mkey, 32);
            uhmac_sha256_update(&hmac, &out[65], 16 + inlen);
            uhmac_sha256_update(&hmac, s, slen);
            uhmac_sha256_finish(&hmac, &out[65 + 16 + inlen]);
            uhmac_sha256_free(&hmac);
        }
    }

    uecc_key_deinit(&ecc);
    return err;
}

int
uecies_decrypt_str(uecc_ctx* s,
                   const uint8_t* smac,
                   size_t smaclen,
                   int radix,
                   const char* cipher,
                   uint8_t* plain)
{
    // Convert to bignum and decrypt
    int sz = -1;
    ubn bin;
    ubn_init(&bin);
    sz = ubn_str(&bin, radix, cipher);
    if (!sz) sz = uecies_decrypt_mpi(s, smac, smaclen, &bin, plain);
    ubn_free(&bin);
    return sz;
}

int
uecies_decrypt_mpi(uecc_ctx* s,
                   const uint8_t* smac,
                   size_t smaclen,
                   ubn* bin,
                   uint8_t* plain)
{
    // Convert to binary and decrypt
    int sz = -1;
    size_t l = ubn_size(bin);
    uint8_t buff[l];
    sz = ubn_tob(bin, buff, l);
    if (!sz) sz = uecies_decrypt(s, smac, smaclen, buff, l, plain);
    return sz;
}

int
uecies_decrypt(uecc_ctx* secret,
               const uint8_t* smac,
               size_t smaclen,
               const uint8_t* cipher,
               size_t len,
               uint8_t* plain)
{
    int sz = 0;
    uint8_t key[32];  // kdf(ecdh_agree(secret,ecies-pubkey));
    uint8_t mkey[32]; // sha256(key[16]);
    uint8_t tmac[32]; // hmac_sha256(iv+ciphertext)
    uaes_iv* iv = (uaes_iv*)&cipher[65];
    uaes_128_ctr_key* ekey = (uaes_128_ctr_key*)key;
    uhmac_sha256_ctx hmac;

    // Get shared secret key
    sz = uecc_agree(secret, (uecc_public_key*)cipher);
    if (sz) return sz;

    // Check key
    sz = ubn_size(&secret->z) == 32 ? 0 : -1;
    if (sz) return sz;

    // Verify tag
    uecies_kdf_mpi(&secret->z, key, 32);
    ucrypto_sha256(&key[16], 16, mkey);
    uhmac_sha256_init(&hmac, mkey, 32);
    uhmac_sha256_update(&hmac, &cipher[65], len - 32 - 65);
    uhmac_sha256_update(&hmac, smac, smaclen);
    uhmac_sha256_finish(&hmac, tmac);
    uhmac_sha256_free(&hmac);
    // uhmac_sha256(mkey, 32, &cipher[65], len - 32 - 65, tmac);
    for (uint32_t i = 0; i < 32; i++) {
        if (!(tmac[i] == cipher[len - 32 + i])) return -1;
    }

    // decrypt aes-128-ctr
    sz = uaes_crypt(ekey, iv, &cipher[81], len - 32 - 16 - 65, plain);

    // return < -1 or length of plain
    return sz ? sz : len - 32 - 16 - 65;
}

int
uecies_kdf_str(const char* str, int radix, uint8_t* b, size_t keylen)
{
    ubn z;
    int err = -1;
    ubn_init(&z);
    err = ubn_str(&z, radix, str);
    if (!err) err = uecies_kdf_mpi(&z, b, keylen);
    ubn_free(&z);
    return err;
}

int
uecies_kdf_mpi(const ubn* secret, uint8_t* b, size_t keylen)
{
    int err, zlen = mbedtls_mpi_size(secret);
    uint8_t z[zlen];
    err = ubn_tob(secret, z, zlen);
    if (!err) uecies_kdf(z, zlen, b, keylen);
    return err;
}

void
uecies_kdf(uint8_t* z, size_t zlen, uint8_t* b, size_t keylen)
{
    // concat hashes (counter||secret||otherInfo)
    // where otherinfo seems to be empty
    // Counter is a big endian 32 bit number initialized to 1.
    // ``\_("/)_/`` ((keylen+7)*8)/(64*8) for sha256
    mbedtls_sha256_context sha;
    uint8_t ctr[4] = { 0, 0, 0, 1 };
    uint8_t* end = &b[keylen];
    uint8_t s1 = 0;

    while (b < end) {
        uint8_t tmp[32];
        mbedtls_sha256_init(&sha);
        mbedtls_sha256_starts(&sha, 0);
        mbedtls_sha256_update(&sha, ctr, 4);
        mbedtls_sha256_update(&sha, z, zlen);
        mbedtls_sha256_update(&sha, &s1, 0);
        mbedtls_sha256_finish(&sha, tmp);
        memcpy(b, tmp, b + 32 <= end ? 32 : end - b);
        mbedtls_sha256_free(&sha);
        b += 32;

        // Nifty short circuit condition big endian counter
        if (++ctr[3] || ++ctr[2] || ++ctr[1] || ++ctr[0]) continue;
    }
}

//
//
//
