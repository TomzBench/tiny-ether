#include "uecies.h"

// 0x04 + echd-random-pubk + iv + aes(kdf(shared-secret), plaintext) + hmac
// * offset 0                65         81               275
// *        [ecies-pubkey:65||aes-iv:16||cipher-text:194||ecies-mac:32]
// *                        ||-----------hmac-----------||

int
uecies_encrypt_str(ucrypto_ecp_point* p,
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
uecies_encrypt_mpi(ucrypto_ecp_point* p,
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
uecies_encrypt(ucrypto_ecp_point* p,
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
