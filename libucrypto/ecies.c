#include "ecies.h"

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
    err = ucrypto_ecc_agree(secret, (ucrypto_ecc_public_key*)cipher);
    if (!(err == 0)) goto EXIT;

    err = ucrypto_mpi_size(&secret->z) == 32 ? 0 : -1;
    if (!(err == 0)) goto EXIT;

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
    err = ucrypto_ecies_kdf(&z, b, keylen);
EXIT:
    ucrypto_mpi_free(&z);
    return err;
}

int
ucrypto_ecies_kdf(const ucrypto_mpi* secret, uint8_t* b, size_t keylen)
{
    // concat hashes (counter||secret||otherInfo)
    // where otherinfo seems to be empty
    // Counter is a big endian 32 bit number initialized to 1.
    // ``\_("/)_/`` ((keylen+7)*8)/(64*8) for sha256
    mbedtls_sha256_context sha;
    uint8_t ctr[4] = { 0, 0, 0, 1 };
    size_t c = 0;
    uint8_t s1 = 0;
    int err = -1, zlen = mbedtls_mpi_size(secret);
    uint8_t z[zlen];
    err = mbedtls_mpi_write_binary(secret, z, zlen);
    if (!(err == 0)) goto EXIT;
    while (c < keylen) {
        mbedtls_sha256_init(&sha);
        mbedtls_sha256_starts(&sha, 0);
        mbedtls_sha256_update(&sha, ctr, 4);
        mbedtls_sha256_update(&sha, z, zlen);
        mbedtls_sha256_update(&sha, &s1, 0);
        mbedtls_sha256_finish(&sha, &b[c]);
        mbedtls_sha256_free(&sha);
        c += 32; // sha256 blocksize

        // Nifty short circuit condition big endian counter
        if (++ctr[3] || ++ctr[2] || ++ctr[1] || ++ctr[0]) {
            continue;
        }
    }
    err = 0;
EXIT:
    return err;
}

//
//
//
