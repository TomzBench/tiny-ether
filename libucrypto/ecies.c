#include "ecies.h"

// 0x04 + echd-random-pubk + iv + aes(kdf(shared-secret), plaintext) + hmac
// * offset 0                65         81               275
// *        [ecies-pubkey:65||aes-iv:16||cipher-text:194||ecies-mac:32]
// *                        ||-----------hmac-----------||

int
ucrypto_ecies_encrypt_str(ucrypto_ecp_point* p,
                          const uint8_t* s,
                          size_t slen,
                          int radix,
                          const char* plain,
                          uint8_t* cipher)
{
    // Convert to bignum and encrypt
    int err = -1;
    ucrypto_mpi bin;
    ucrypto_mpi_init(&bin);
    err = ucrypto_mpi_read_string(&bin, radix, plain);
    if (!err) err = ucrypto_ecies_encrypt_mpi(p, s, slen, &bin, cipher);
    return err;
}

int
ucrypto_ecies_encrypt_mpi(ucrypto_ecp_point* p,
                          const uint8_t* s,
                          size_t slen,
                          ucrypto_mpi* bin,
                          uint8_t* cipher)
{
    // convert to binary and encrypt
    int err = -1;
    size_t l = ucrypto_mpi_size(bin);
    uint8_t buff[l];
    err = ucrypto_mpi_write_binary(bin, buff, l);
    if (!err) err = ucrypto_ecies_encrypt(p, s, slen, buff, l, cipher);
    return err;
}

int
ucrypto_ecies_encrypt(ucrypto_ecp_point* p,
                      const uint8_t* s,
                      size_t slen,
                      const uint8_t* in,
                      size_t inlen,
                      uint8_t* out)
{
    int err = 0;
    uint8_t key[32], mkey[32];
    ucrypto_aes_iv iv;
    ucrypto_hmac_sha256_ctx hmac;
    ucrypto_ecc_ctx ecc;
    ucrypto_ecc_public_key* ours = (ucrypto_ecc_public_key*)&out[0];
    ucrypto_aes_128_ctr_key* ekey = (ucrypto_aes_128_ctr_key*)&key[0];
    ucrypto_aes_iv* iv_dst = (ucrypto_aes_iv*)&out[65];
    ucrypto_ecc_key_init_new(&ecc);
    err = ucrypto_ecc_agree_point(&ecc, p);
    if (!err) {
        // 0x04 || R || IV || aes(kdf(agree(pub)),in) || tag
        ucrypto_ecies_kdf_mpi(&ecc.z, key, 32);
        ucrypto_sha256(&key[16], 16, mkey);
        memcpy(iv_dst->b, "0123456789012345", 16); // TODO init iv
        memcpy(iv.b, "0123456789012345", 16);      // TODO init iv
        err = ucrypto_ecc_ptob(&ecc.Q, ours);
        if (!err) err = ucrypto_aes_crypt(ekey, &iv, in, inlen, &out[81]);
        if (!err) {
            // ucrypto_hmac_sha256(mkey, 32, &out[65], 16 + inlen,
            //                    &out[65 + 16 + inlen]);
            ucrypto_hmac_sha256_init(&hmac, mkey, 32);
            ucrypto_hmac_sha256_update(&hmac, &out[65], 16 + inlen);
            ucrypto_hmac_sha256_update(&hmac, s, slen);
            ucrypto_hmac_sha256_finish(&hmac, &out[65 + 16 + inlen]);
            ucrypto_hmac_sha256_free(&hmac);
        }
    }

    ucrypto_ecc_key_deinit(&ecc);
    return err;
}

int
ucrypto_ecies_decrypt_str(ucrypto_ecc_ctx* s,
                          const uint8_t* smac,
                          size_t smaclen,
                          int radix,
                          const char* cipher,
                          uint8_t* plain)
{
    // Convert to bignum and decrypt
    int sz = -1;
    ucrypto_mpi bin;
    ucrypto_mpi_init(&bin);
    sz = ucrypto_mpi_read_string(&bin, radix, cipher);
    if (!sz) sz = ucrypto_ecies_decrypt_mpi(s, smac, smaclen, &bin, plain);
    ucrypto_mpi_free(&bin);
    return sz;
}

int
ucrypto_ecies_decrypt_mpi(ucrypto_ecc_ctx* s,
                          const uint8_t* smac,
                          size_t smaclen,
                          ucrypto_mpi* bin,
                          uint8_t* plain)
{
    // Convert to binary and decrypt
    int sz = -1;
    size_t l = ucrypto_mpi_size(bin);
    uint8_t buff[l];
    sz = ucrypto_mpi_write_binary(bin, buff, l);
    if (!sz) sz = ucrypto_ecies_decrypt(s, smac, smaclen, buff, l, plain);
    return sz;
}

int
ucrypto_ecies_decrypt(ucrypto_ecc_ctx* secret,
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
    ucrypto_aes_iv* iv = (ucrypto_aes_iv*)&cipher[65];
    ucrypto_aes_128_ctr_key* ekey = (ucrypto_aes_128_ctr_key*)key;
    ucrypto_hmac_sha256_ctx hmac;

    // Get shared secret key
    sz = ucrypto_ecc_agree(secret, (ucrypto_ecc_public_key*)cipher);
    if (sz) return sz;

    // Check key
    sz = ucrypto_mpi_size(&secret->z) == 32 ? 0 : -1;
    if (sz) return sz;

    // Verify tag
    ucrypto_ecies_kdf_mpi(&secret->z, key, 32);
    ucrypto_sha256(&key[16], 16, mkey);
    ucrypto_hmac_sha256_init(&hmac, mkey, 32);
    ucrypto_hmac_sha256_update(&hmac, &cipher[65], len - 32 - 65);
    ucrypto_hmac_sha256_update(&hmac, smac, smaclen);
    ucrypto_hmac_sha256_finish(&hmac, tmac);
    ucrypto_hmac_sha256_free(&hmac);
    // ucrypto_hmac_sha256(mkey, 32, &cipher[65], len - 32 - 65, tmac);
    for (uint32_t i = 0; i < 32; i++) {
        if (!(tmac[i] == cipher[len - 32 + i])) return -1;
    }

    // decrypt aes-128-ctr
    sz = ucrypto_aes_crypt(ekey, iv, &cipher[81], len - 32 - 16 - 65, plain);

    // return < -1 or length of plain
    return sz ? sz : len - 32 - 16 - 65;
}

int
ucrypto_ecies_kdf_str(const char* str, int radix, uint8_t* b, size_t keylen)
{
    ucrypto_mpi z;
    int err = -1;
    ucrypto_mpi_init(&z);
    err = ucrypto_mpi_read_string(&z, radix, str);
    if (!err) err = ucrypto_ecies_kdf_mpi(&z, b, keylen);
    ucrypto_mpi_free(&z);
    return err;
}

int
ucrypto_ecies_kdf_mpi(const ucrypto_mpi* secret, uint8_t* b, size_t keylen)
{
    int err, zlen = mbedtls_mpi_size(secret);
    uint8_t z[zlen];
    err = mbedtls_mpi_write_binary(secret, z, zlen);
    if (!err) ucrypto_ecies_kdf(z, zlen, b, keylen);
    return err;
}

void
ucrypto_ecies_kdf(uint8_t* z, size_t zlen, uint8_t* b, size_t keylen)
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
