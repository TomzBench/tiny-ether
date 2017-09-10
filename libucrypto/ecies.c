#include "ecies.h"
#include <string.h>

int
ucrypto_ecies_decrypt_string(ucrypto_ecc_ctx* s,
                             int radix,
                             const char* cipher,
                             uint8_t* plain,
                             size_t plain_len)
{
    int err = -1;
    ucrypto_mpi bin;
    ucrypto_mpi_init(&bin);

    // Convert to bignum
    err = ucrypto_mpi_read_string(&bin, radix, cipher);
    if (!(err == 0)) goto EXIT;

    // Decrypt
    err = ucrypto_ecies_decrypt_mpi(s, &bin, plain, plain_len);
EXIT:
    ucrypto_mpi_free(&bin);
    return err;
}

int
ucrypto_ecies_decrypt_mpi(ucrypto_ecc_ctx* s,
                          ucrypto_mpi* bin,
                          uint8_t* plain,
                          size_t plain_len)
{
    int err = -1;
    size_t l = ucrypto_mpi_size(bin);
    uint8_t buff[l];

    // Convert to binary
    err = ucrypto_mpi_write_binary(bin, buff, l);
    if (!(err == 0)) goto EXIT;

    // Decrypt
    err = ucrypto_ecies_decrypt(s, buff, l, plain, plain_len);
EXIT:
    return err;
}

int
ucrypto_ecies_decrypt(ucrypto_ecc_ctx* secret,
                      const uint8_t* cipher,
                      size_t cipher_len,
                      uint8_t* plain,
                      size_t plain_len)
{
    // TODO: Encryption
    // 0x04 + echd-random-pubk + iv + aes(kdf(shared-secret), plaintext) + hmac
    // * offset 0                65         81               275
    // *        [ecies-pubkey:65||aes-iv:16||cipher-text:194||ecies-mac:32]
    int err = 0;
    uint8_t key[32];

    err = ucrypto_ecc_agree(secret, (ucrypto_ecc_public_key*)cipher);
    if (!(err == 0)) goto EXIT;

    err = ucrypto_mpi_size(&secret->z) == 32 ? 0 : -1;
    if (!(err == 0)) goto EXIT;

    ucrypto_ecies_kdf_mpi(&secret->z, key, 32);

EXIT:
    return err;
}

int
ucrypto_ecies_kdf_string(const char* str, int radix, uint8_t* b, size_t keylen)
{
    ucrypto_mpi z;
    int err = -1;
    ucrypto_mpi_init(&z);
    err = ucrypto_mpi_read_string(&z, radix, str);
    if (!(err == 0)) goto EXIT;
    err = ucrypto_ecies_kdf_mpi(&z, b, keylen);
EXIT:
    ucrypto_mpi_free(&z);
    return err;
}

int
ucrypto_ecies_kdf_mpi(const ucrypto_mpi* secret, uint8_t* b, size_t keylen)
{
    int err, zlen = mbedtls_mpi_size(secret);
    uint8_t z[zlen];
    err = mbedtls_mpi_write_binary(secret, z, zlen);
    if (!(err == 0)) goto EXIT;

    ucrypto_ecies_kdf(z, zlen, b, keylen);
EXIT:
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
        if (++ctr[3] || ++ctr[2] || ++ctr[1] || ++ctr[0]) {
            continue;
        }
    }
}

void
ucrypto_hmac_sha256_init(ucrypto_hmac_sha256* ctx,
                         const uint8_t* key,
                         size_t klen)
{
    static uint8_t i_key_pad[64];
    memset(i_key_pad, 0, 64);
    memset(ctx, 0, sizeof(ucrypto_hmac_sha256));
    if (klen > 64) {
        mbedtls_sha256_context sha;
        mbedtls_sha256_init(&sha);
        mbedtls_sha256_starts(&sha, 0);
        mbedtls_sha256_update(&sha, key, klen);
        mbedtls_sha256_finish(&sha, i_key_pad);
        mbedtls_sha256_free(&sha);
    } else {
        memcpy(i_key_pad, key, klen);
    }
    for (int i = 0; i < 64; i++) {
        ctx->pad[i] = i_key_pad[i] ^ 0x5c;
        i_key_pad[i] ^= 0x36;
    }
    mbedtls_sha256_init(&ctx->sha);
    mbedtls_sha256_starts(&ctx->sha, 0);
    mbedtls_sha256_update(&ctx->sha, i_key_pad, 64);
    memset(i_key_pad, 0, sizeof(i_key_pad));
}

void
ucrypto_hmac_sha256_update(ucrypto_hmac_sha256* ctx, const uint8_t* b, size_t l)
{
    mbedtls_sha256_update(&ctx->sha, b, l);
}

void
ucrypto_hmac_sha256_finish(ucrypto_hmac_sha256* ctx, uint8_t* hmac)
{
    mbedtls_sha256_finish(&ctx->sha, hmac);
    mbedtls_sha256_init(&ctx->sha);
    mbedtls_sha256_starts(&ctx->sha, 0);
    mbedtls_sha256_update(&ctx->sha, ctx->pad, 64);
    mbedtls_sha256_update(&ctx->sha, hmac, 32);
    mbedtls_sha256_finish(&ctx->sha, hmac);
}

void
ucrypto_hmac_sha256_free(ucrypto_hmac_sha256* ctx)
{
    mbedtls_sha256_free(&ctx->sha);
}

//
//
//
